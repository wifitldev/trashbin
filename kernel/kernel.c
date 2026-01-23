#include "boot.h"
#include "console.h"
#include "lib/printf.h"
#include "arch/x86_64/idt.h"
#include "drivers/timer.h"
#include "drivers/keyboard.h"
#include "drivers/mouse.h"
#include "drivers/ata.h"
#include "mm/memory.h"
#include "mm/heap.h"
#include "mm/paging.h"
#include "task.h"
#include "fs/fs.h"
#include "gui/gui.h"
#include "shell/shell.h"
#include "gui/explorer.h"

extern uint8_t __kernel_end;

void kmain(boot_info_t* boot) {
    console_init();
    console_write("os kernel starting\n");

    idt_init();
    timer_init(100);
    keyboard_init();
    mouse_init();

    memory_init(boot);
    paging_init();

    uintptr_t heap_start = (uintptr_t)&__kernel_end + 0x1000;
    heap_init((void*)heap_start, 1024 * 1024);

    fs_init(boot->fs_lba, boot->fs_sectors);

    gui_init(boot->fb_addr, boot->fb_width, boot->fb_height, boot->fb_pitch, boot->fb_bpp);

    task_init();
    task_spawn("shell", shell_task, NULL);
    task_spawn("explorer", explorer_task, NULL);

    console_write("os kernel ready\n");

    for (;;) {
        task_yield();
    }
}
