#include "fs/vfs.h"
#include "fs/fs.h"
#include "fs/osfs.h"
#include "lib/string.h"
#include "mm/heap.h"

typedef struct {
    char path[64];
    size_t offset;
    size_t size;
} open_file_t;

static open_file_t open_files[VFS_MAX_OPEN];

int vfs_open(const char* path) {
    for (int i = 0; i < VFS_MAX_OPEN; i++) {
        if (open_files[i].path[0] == '\0') {
            strncpy(open_files[i].path, path, sizeof(open_files[i].path) - 1);
            open_files[i].offset = 0;
            open_files[i].size = 0;
            return i;
        }
    }
    return -1;
}

int vfs_read(int fd, void* buf, size_t len) {
    if (fd < 0 || fd >= VFS_MAX_OPEN) {
        return -1;
    }
    if (open_files[fd].path[0] == '\0') {
        return -1;
    }
    size_t out_len = 0;
    int res = fs_read_file(open_files[fd].path, buf, len, &out_len);
    if (res != 0) {
        return -1;
    }
    return (int)out_len;
}

int vfs_close(int fd) {
    if (fd < 0 || fd >= VFS_MAX_OPEN) {
        return -1;
    }
    open_files[fd].path[0] = '\0';
    return 0;
}

int vfs_listdir(const char* path, vfs_entry_t* out, size_t max_entries) {
    osfs_entry_t* entries = (osfs_entry_t*)kmalloc(sizeof(osfs_entry_t) * max_entries);
    if (!entries) {
        return -1;
    }
    int count = fs_list_dir(path, entries, max_entries);
    if (count < 0) {
        return -1;
    }
    for (int i = 0; i < count; i++) {
        strncpy(out[i].name, entries[i].name, sizeof(out[i].name) - 1);
        out[i].type = entries[i].type;
        out[i].size = entries[i].size;
    }
    return count;
}

int vfs_mkdir(const char* path) {
    return fs_make_dir(path);
}

int vfs_touch(const char* path) {
    return fs_make_file(path);
}
