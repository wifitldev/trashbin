#include "isr.h"
#include "arch/x86_64/ports.h"
#include "console.h"
#include "drivers/keyboard.h"
#include "drivers/timer.h"
#include "drivers/mouse.h"
#include "task.h"
#include "sys/syscall.h"

static void pic_eoi(uint8_t irq) {
    if (irq >= 8) {
        outb(0xA0, 0x20);
    }
    outb(0x20, 0x20);
}

void isr_handler(regs_t* regs) {
    if (regs->int_no == 32) {
        extern void timer_tick_internal(void);
        timer_tick_internal();
        task_tick();
        pic_eoi(0);
        return;
    }
    if (regs->int_no == 33) {
        keyboard_isr();
        pic_eoi(1);
        return;
    }
    if (regs->int_no == 44) {
        pic_eoi(12);
        return;
    }
    if (regs->int_no == 128) {
        uint64_t syscall_num = regs->rax;
        if (syscall_num == SYS_WRITE) {
            regs->rax = sys_write((int)regs->rbx, (void*)regs->rcx, (size_t)regs->rdx);
        } else if (syscall_num == SYS_EXIT) {
            regs->rax = sys_exit((int)regs->rbx);
        } else if (syscall_num == SYS_SPAWN) {
            regs->rax = sys_spawn((const char*)regs->rbx);
        } else if (syscall_num == SYS_OPEN) {
            regs->rax = sys_open((const char*)regs->rbx);
        } else if (syscall_num == SYS_READ) {
            regs->rax = sys_read((int)regs->rbx, (void*)regs->rcx, (size_t)regs->rdx);
        } else if (syscall_num == SYS_CLOSE) {
            regs->rax = sys_close((int)regs->rbx);
        } else if (syscall_num == SYS_LISTDIR) {
            regs->rax = sys_listdir((const char*)regs->rbx, (void*)regs->rcx, (size_t)regs->rdx);
        }
        return;
    }

    console_write("Unhandled interrupt: ");
    console_putc('0' + (regs->int_no / 10));
    console_putc('0' + (regs->int_no % 10));
    console_putc('\n');
}
