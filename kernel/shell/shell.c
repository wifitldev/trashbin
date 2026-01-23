#include "shell/shell.h"
#include "console.h"
#include "drivers/keyboard.h"
#include "fs/vfs.h"
#include "task.h"
#include "lib/string.h"
#include "mm/memory.h"
#include "sys/syscall.h"

static char cwd[64] = "/";

static void shell_prompt(void) {
    console_write("os:");
    console_write(cwd);
    console_write("$ ");
}

static void cmd_help(void) {
    console_write("Commands: help ls cd cat echo mkdir touch clear ps mem run\n");
}

static void cmd_ls(void) {
    vfs_entry_t entries[32];
    int count = vfs_listdir(cwd, entries, 32);
    if (count < 0) {
        console_write("ls: error\n");
        return;
    }
    for (int i = 0; i < count; i++) {
        console_write(entries[i].name);
        console_write(entries[i].type == 2 ? "/" : "");
        console_putc('\n');
    }
}

static void cmd_cd(const char* path) {
    if (!path || path[0] == '\0') {
        return;
    }
    if (path[0] == '/') {
        strncpy(cwd, path, sizeof(cwd) - 1);
        return;
    }
    if (strcmp(cwd, "/") == 0) {
        strncpy(cwd, "/", sizeof(cwd) - 1);
        strcat(cwd, path);
    } else {
        strcat(cwd, "/");
        strcat(cwd, path);
    }
}

static void cmd_cat(const char* path) {
    if (!path) {
        return;
    }
    char full[64];
    if (path[0] == '/') {
        strncpy(full, path, sizeof(full) - 1);
    } else {
        strncpy(full, cwd, sizeof(full) - 1);
        if (strcmp(cwd, "/") != 0) {
            strcat(full, "/");
        }
        strcat(full, path);
    }
    int fd = vfs_open(full);
    if (fd < 0) {
        console_write("cat: cannot open\n");
        return;
    }
    char buf[256];
    int n = vfs_read(fd, buf, sizeof(buf) - 1);
    if (n > 0) {
        buf[n] = '\0';
        console_write(buf);
        console_putc('\n');
    }
    vfs_close(fd);
}

static void cmd_echo(const char* text) {
    if (text) {
        console_write(text);
    }
    console_putc('\n');
}

static void cmd_mkdir(const char* path) {
    if (path && vfs_mkdir(path) == 0) {
        console_write("mkdir ok\n");
    } else {
        console_write("mkdir failed\n");
    }
}

static void cmd_touch(const char* path) {
    if (path && vfs_touch(path) == 0) {
        console_write("touch ok\n");
    } else {
        console_write("touch failed\n");
    }
}

static void cmd_ps(void) {
    task_list();
}

static void cmd_mem(void) {
    char buf[32];
    size_t total = memory_total_kib();
    size_t free = memory_free_kib();
    console_write("Memory total KiB: ");
    buf[0] = '\0';
    int t = (int)total;
    int i = 0;
    if (t == 0) {
        buf[i++] = '0';
    } else {
        char tmp[16];
        int j = 0;
        while (t > 0 && j < 15) {
            tmp[j++] = '0' + (t % 10);
            t /= 10;
        }
        while (j--) {
            buf[i++] = tmp[j];
        }
    }
    buf[i] = '\0';
    console_write(buf);
    console_putc('\n');
    console_write("Free KiB: ");
    i = 0;
    int f = (int)free;
    if (f == 0) {
        buf[i++] = '0';
    } else {
        char tmp[16];
        int j = 0;
        while (f > 0 && j < 15) {
            tmp[j++] = '0' + (f % 10);
            f /= 10;
        }
        while (j--) {
            buf[i++] = tmp[j];
        }
    }
    buf[i] = '\0';
    console_write(buf);
    console_putc('\n');
}

static void cmd_run(const char* name) {
    if (!name) return;
    if (sys_spawn(name) < 0) {
        console_write("run: unknown app\n");
    }
}

static void execute(char* line) {
    if (strcmp(line, "help") == 0) {
        cmd_help();
    } else if (strcmp(line, "ls") == 0) {
        cmd_ls();
    } else if (strncmp(line, "cd ", 3) == 0) {
        cmd_cd(line + 3);
    } else if (strncmp(line, "cat ", 4) == 0) {
        cmd_cat(line + 4);
    } else if (strncmp(line, "echo ", 5) == 0) {
        cmd_echo(line + 5);
    } else if (strncmp(line, "mkdir ", 6) == 0) {
        cmd_mkdir(line + 6);
    } else if (strncmp(line, "touch ", 6) == 0) {
        cmd_touch(line + 6);
    } else if (strcmp(line, "clear") == 0) {
        console_clear();
    } else if (strcmp(line, "ps") == 0) {
        cmd_ps();
    } else if (strcmp(line, "mem") == 0) {
        cmd_mem();
    } else if (strncmp(line, "run ", 4) == 0) {
        cmd_run(line + 4);
    } else if (line[0]) {
        console_write("Unknown command\n");
    }
}

void shell_task(void* arg) {
    (void)arg;
    char line[128];
    size_t len = 0;
    console_write("os shell ready\n");
    for (;;) {
        shell_prompt();
        len = 0;
        while (1) {
            char c;
            if (keyboard_read(&c)) {
                if (c == '\n') {
                    line[len] = '\0';
                    console_putc('\n');
                    execute(line);
                    break;
                } else if (c == '\b') {
                    if (len > 0) {
                        len--;
                        console_putc('\b');
                        console_putc(' ');
                        console_putc('\b');
                    }
                } else {
                    if (len + 1 < sizeof(line)) {
                        line[len++] = c;
                        console_putc(c);
                    }
                }
            }
        }
    }
}
