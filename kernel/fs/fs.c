#include "fs/fs.h"
#include "fs/osfs.h"
#include "drivers/ata.h"
#include "lib/string.h"
#include "mm/heap.h"
#include "console.h"

static osfs_super_t super;
static osfs_entry_t* entries;
static uint64_t fs_base_lba;

static int find_entry(const char* path, uint32_t* out_index) {
    if (!path || path[0] != '/') {
        return -1;
    }
    if (strcmp(path, "/") == 0) {
        *out_index = 0;
        return 0;
    }
    uint32_t current = 0;
    const char* p = path + 1;
    char name[OSFS_MAX_NAME];
    size_t len = 0;

    while (1) {
        if (*p == '/' || *p == '\0') {
            name[len] = '\0';
            uint32_t found = 0xFFFFFFFF;
            for (uint32_t i = 0; i < super.entry_count; i++) {
                if (entries[i].parent == current && strcmp(entries[i].name, name) == 0) {
                    found = i;
                    break;
                }
            }
            if (found == 0xFFFFFFFF) {
                return -1;
            }
            current = found;
            len = 0;
            if (*p == '\0') {
                *out_index = current;
                return 0;
            }
        } else if (len + 1 < OSFS_MAX_NAME) {
            name[len++] = *p;
        }
        p++;
    }
    return -1;
}

static int split_parent(const char* path, char* out_parent, char* out_name) {
    size_t len = strlen(path);
    if (len == 0 || path[0] != '/') {
        return -1;
    }
    size_t last = 0;
    for (size_t i = 1; i < len; i++) {
        if (path[i] == '/') {
            last = i;
        }
    }
    if (last == 0) {
        strncpy(out_parent, "/", OSFS_MAX_NAME);
        strncpy(out_name, path + 1, OSFS_MAX_NAME - 1);
        return 0;
    }
    size_t parent_len = last;
    if (parent_len >= OSFS_MAX_NAME) {
        return -1;
    }
    strncpy(out_parent, path, parent_len);
    out_parent[parent_len] = '\0';
    strncpy(out_name, path + last + 1, OSFS_MAX_NAME - 1);
    return 0;
}

void fs_init(uint64_t lba, uint64_t sectors) {
    fs_base_lba = lba;
    uint8_t* buf = (uint8_t*)kmalloc(sectors * 512);
    if (!buf) {
        console_write("FS buffer alloc failed\n");
        return;
    }
    if (ata_read(lba, buf, sectors) != 0) {
        console_write("FS read failed\n");
        return;
    }
    memcpy(&super, buf, sizeof(super));
    if (super.magic != OSFS_MAGIC) {
        console_write("FS magic invalid\n");
        return;
    }
    entries = (osfs_entry_t*)(buf + sizeof(super));
    console_write("FS mounted\n");
}

int fs_read_file(const char* path, uint8_t* buf, size_t max, size_t* out_len) {
    uint32_t idx;
    if (find_entry(path, &idx) != 0) {
        return -1;
    }
    if (entries[idx].type != OSFS_TYPE_FILE) {
        return -1;
    }
    size_t size = entries[idx].size;
    if (size > max) {
        size = max;
    }
    uint64_t start = fs_base_lba + super.data_start_lba + entries[idx].start_block;
    uint32_t sectors = (size + 511) / 512;
    if (ata_read(start, buf, sectors) != 0) {
        return -1;
    }
    *out_len = size;
    return 0;
}

int fs_list_dir(const char* path, void* out, size_t max_entries) {
    uint32_t idx;
    if (find_entry(path, &idx) != 0) {
        return -1;
    }
    size_t count = 0;
    osfs_entry_t* out_entries = (osfs_entry_t*)out;
    for (uint32_t i = 0; i < super.entry_count && count < max_entries; i++) {
        if (entries[i].parent == idx) {
            out_entries[count++] = entries[i];
        }
    }
    return (int)count;
}

int fs_make_dir(const char* path) {
    if (!entries || super.entry_count >= OSFS_MAX_ENTRIES) {
        return -1;
    }
    char parent_path[OSFS_MAX_NAME];
    char name[OSFS_MAX_NAME];
    if (split_parent(path, parent_path, name) != 0) {
        return -1;
    }
    uint32_t parent_idx;
    if (find_entry(parent_path, &parent_idx) != 0) {
        return -1;
    }
    osfs_entry_t* entry = &entries[super.entry_count++];
    entry->parent = parent_idx;
    entry->start_block = 0;
    entry->size = 0;
    entry->type = OSFS_TYPE_DIR;
    memset(entry->reserved, 0, sizeof(entry->reserved));
    strncpy(entry->name, name, OSFS_MAX_NAME - 1);
    entry->name[OSFS_MAX_NAME - 1] = '\0';
    return 0;
}

int fs_make_file(const char* path) {
    if (!entries || super.entry_count >= OSFS_MAX_ENTRIES) {
        return -1;
    }
    char parent_path[OSFS_MAX_NAME];
    char name[OSFS_MAX_NAME];
    if (split_parent(path, parent_path, name) != 0) {
        return -1;
    }
    uint32_t parent_idx;
    if (find_entry(parent_path, &parent_idx) != 0) {
        return -1;
    }
    osfs_entry_t* entry = &entries[super.entry_count++];
    entry->parent = parent_idx;
    entry->start_block = 0;
    entry->size = 0;
    entry->type = OSFS_TYPE_FILE;
    memset(entry->reserved, 0, sizeof(entry->reserved));
    strncpy(entry->name, name, OSFS_MAX_NAME - 1);
    entry->name[OSFS_MAX_NAME - 1] = '\0';
    return 0;
}
