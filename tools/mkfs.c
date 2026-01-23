#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>

#define OSFS_MAGIC 0x5346534F
#define OSFS_MAX_NAME 32
#define OSFS_MAX_ENTRIES 128
#define OSFS_BLOCK_SIZE 512
#define OSFS_TYPE_FILE 1
#define OSFS_TYPE_DIR 2

typedef struct {
    uint32_t magic;
    uint32_t entry_count;
    uint32_t data_start_lba;
    uint32_t data_blocks;
} __attribute__((packed)) osfs_super_t;

typedef struct {
    uint32_t parent;
    uint32_t start_block;
    uint32_t size;
    uint8_t type;
    uint8_t reserved[3];
    char name[OSFS_MAX_NAME];
} __attribute__((packed)) osfs_entry_t;

typedef struct {
    osfs_entry_t entries[OSFS_MAX_ENTRIES];
    uint32_t count;
} entry_table_t;

static void add_entry(entry_table_t* table, const char* name, uint32_t parent, uint8_t type, uint32_t start_block, uint32_t size) {
    if (table->count >= OSFS_MAX_ENTRIES) {
        return;
    }
    osfs_entry_t* e = &table->entries[table->count++];
    memset(e, 0, sizeof(*e));
    e->parent = parent;
    e->start_block = start_block;
    e->size = size;
    e->type = type;
    strncpy(e->name, name, OSFS_MAX_NAME - 1);
}

static uint32_t data_start_lba;

static uint32_t add_file(entry_table_t* table, const char* path, const char* name, uint32_t parent, FILE* out) {
    FILE* f = fopen(path, "rb");
    if (!f) {
        return 0;
    }
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);
    uint32_t blocks = (size + OSFS_BLOCK_SIZE - 1) / OSFS_BLOCK_SIZE;
    uint32_t start_block = (uint32_t)(ftell(out) / OSFS_BLOCK_SIZE) - data_start_lba;

    add_entry(table, name, parent, OSFS_TYPE_FILE, start_block, (uint32_t)size);

    uint8_t buf[OSFS_BLOCK_SIZE];
    for (uint32_t i = 0; i < blocks; i++) {
        size_t read = fread(buf, 1, OSFS_BLOCK_SIZE, f);
        if (read < OSFS_BLOCK_SIZE) {
            memset(buf + read, 0, OSFS_BLOCK_SIZE - read);
        }
        fwrite(buf, 1, OSFS_BLOCK_SIZE, out);
    }

    fclose(f);
    return blocks;
}

static void walk_dir(entry_table_t* table, const char* root, uint32_t parent, FILE* out) {
    DIR* dir = opendir(root);
    if (!dir) {
        return;
    }
    struct dirent* ent;
    while ((ent = readdir(dir)) != NULL) {
        if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0) {
            continue;
        }
        char path[256];
        snprintf(path, sizeof(path), "%s/%s", root, ent->d_name);
        struct stat st;
        if (stat(path, &st) != 0) {
            continue;
        }
        if (S_ISDIR(st.st_mode)) {
            uint32_t dir_index = table->count;
            add_entry(table, ent->d_name, parent, OSFS_TYPE_DIR, 0, 0);
            walk_dir(table, path, dir_index, out);
        } else if (S_ISREG(st.st_mode)) {
            add_file(table, path, ent->d_name, parent, out);
        }
    }
    closedir(dir);
}

int main(int argc, char** argv) {
    if (argc < 3) {
        fprintf(stderr, "usage: mkfs <root> <out>\n");
        return 1;
    }
    const char* root = argv[1];
    const char* out_path = argv[2];

    FILE* out = fopen(out_path, "wb+");
    if (!out) {
        perror("fopen");
        return 1;
    }

    osfs_super_t super = {0};
    entry_table_t table;
    memset(&table, 0, sizeof(table));

    add_entry(&table, "/", 0, OSFS_TYPE_DIR, 0, 0);

    data_start_lba = (sizeof(super) + sizeof(osfs_entry_t) * OSFS_MAX_ENTRIES) / OSFS_BLOCK_SIZE;
    fseek(out, data_start_lba * OSFS_BLOCK_SIZE, SEEK_SET);
    walk_dir(&table, root, 0, out);

    long data_end = ftell(out);
    uint32_t data_blocks = (data_end + OSFS_BLOCK_SIZE - 1) / OSFS_BLOCK_SIZE;

    super.magic = OSFS_MAGIC;
    super.entry_count = table.count;
    super.data_start_lba = data_start_lba;
    super.data_blocks = data_blocks;

    fseek(out, 0, SEEK_SET);
    fwrite(&super, sizeof(super), 1, out);
    fwrite(table.entries, sizeof(osfs_entry_t), OSFS_MAX_ENTRIES, out);

    fclose(out);
    return 0;
}
