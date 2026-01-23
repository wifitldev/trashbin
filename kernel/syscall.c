#include "sys/syscall.h"
#include "console.h"
#include "fs/vfs.h"
#include "task.h"
#include "lib/string.h"

ssize_t sys_write(int fd, const void* buf, size_t len) {
    (void)fd;
    const char* s = (const char*)buf;
    for (size_t i = 0; i < len; i++) {
        console_putc(s[i]);
    }
    return (ssize_t)len;
}

int sys_exit(int code) {
    console_write("Task exited with code ");
    console_putc('0' + (code % 10));
    console_putc('\n');
    task_yield();
    return 0;
}

int sys_spawn(const char* name) {
    if (!name) {
        return -1;
    }
    if (strcmp(name, "shell") == 0) {
        extern void shell_task(void* arg);
        return (int)task_spawn("shell", shell_task, NULL);
    }
    if (strcmp(name, "explorer") == 0) {
        extern void explorer_task(void* arg);
        return (int)task_spawn("explorer", explorer_task, NULL);
    }
    return -1;
}

int sys_open(const char* path) {
    return vfs_open(path);
}

ssize_t sys_read(int fd, void* buf, size_t len) {
    return vfs_read(fd, buf, len);
}

int sys_close(int fd) {
    return vfs_close(fd);
}

int sys_listdir(const char* path, void* out, size_t max) {
    return vfs_listdir(path, (vfs_entry_t*)out, max);
}
