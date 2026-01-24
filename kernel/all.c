// Amalgamated kernel build unit.
#include "console.c"
#include "idt.c"
#include "interrupts.c"
#include "syscall.c"
#include "kernel.c"

#include "lib/string.c"
#include "lib/printf.c"

#include "mm/memory.c"
#include "mm/paging.c"
#include "mm/heap.c"

#include "task.c"

#include "drivers/timer.c"
#include "drivers/keyboard.c"
#include "drivers/mouse.c"
#include "drivers/ata.c"

#include "fs/fs.c"
#include "fs/vfs.c"

#include "gui/gui.c"
#include "gui/explorer.c"

#include "shell/shell.c"
