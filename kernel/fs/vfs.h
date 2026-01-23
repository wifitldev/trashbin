#pragma once
#include "types.h"

#define VFS_MAX_OPEN 16

typedef struct {
    char name[32];
    uint32_t type;
    uint32_t size;
} vfs_entry_t;

int vfs_open(const char* path);
int vfs_read(int fd, void* buf, size_t len);
int vfs_close(int fd);
int vfs_listdir(const char* path, vfs_entry_t* out, size_t max_entries);
int vfs_mkdir(const char* path);
int vfs_touch(const char* path);
