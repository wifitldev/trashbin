#pragma once
#include "types.h"

#define SYS_WRITE 1
#define SYS_EXIT 2
#define SYS_SPAWN 3
#define SYS_OPEN 4
#define SYS_READ 5
#define SYS_CLOSE 6
#define SYS_LISTDIR 7

ssize_t sys_write(int fd, const void* buf, size_t len);
int sys_exit(int code);
int sys_spawn(const char* name);
int sys_open(const char* path);
ssize_t sys_read(int fd, void* buf, size_t len);
int sys_close(int fd);
int sys_listdir(const char* path, void* out, size_t max);
