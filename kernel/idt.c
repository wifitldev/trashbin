#include "arch/x86_64/idt.h"
#include "arch/x86_64/ports.h"
#include "types.h"

extern void isr0();
extern void isr1();
extern void isr2();
extern void isr3();
extern void isr4();
extern void isr5();
extern void isr6();
extern void isr7();
extern void isr8();
extern void isr9();
extern void isr10();
extern void isr11();
extern void isr12();
extern void isr13();
extern void isr14();
extern void isr15();
extern void isr16();
extern void isr17();
extern void isr18();
extern void isr19();
extern void isr20();
extern void isr21();
extern void isr22();
extern void isr23();
extern void isr24();
extern void isr25();
extern void isr26();
extern void isr27();
extern void isr28();
extern void isr29();
extern void isr30();
extern void isr31();
extern void isr32();
extern void isr33();
extern void isr34();
extern void isr35();
extern void isr36();
extern void isr37();
extern void isr38();
extern void isr39();
extern void isr40();
extern void isr41();
extern void isr42();
extern void isr43();
extern void isr44();
extern void isr45();
extern void isr46();
extern void isr47();
extern void isr128();

static idt_entry_t idt[256];
static idt_ptr_t idt_ptr;

static void idt_set_gate(int n, void* handler, uint8_t flags) {
    uint64_t addr = (uint64_t)handler;
    idt[n].offset_low = addr & 0xFFFF;
    idt[n].selector = 0x08;
    idt[n].ist = 0;
    idt[n].type_attr = flags;
    idt[n].offset_mid = (addr >> 16) & 0xFFFF;
    idt[n].offset_high = (addr >> 32) & 0xFFFFFFFF;
    idt[n].zero = 0;
}

static void pic_remap(void) {
    outb(0x20, 0x11);
    outb(0xA0, 0x11);
    outb(0x21, 0x20);
    outb(0xA1, 0x28);
    outb(0x21, 0x04);
    outb(0xA1, 0x02);
    outb(0x21, 0x01);
    outb(0xA1, 0x01);
    outb(0x21, 0x0);
    outb(0xA1, 0x0);
}

void idt_init(void) {
    idt_ptr.limit = sizeof(idt) - 1;
    idt_ptr.base = (uint64_t)&idt;

    for (int i = 0; i < 256; i++) {
        idt_set_gate(i, isr0, 0x8E);
    }

    idt_set_gate(0, isr0, 0x8E);
    idt_set_gate(1, isr1, 0x8E);
    idt_set_gate(2, isr2, 0x8E);
    idt_set_gate(3, isr3, 0x8E);
    idt_set_gate(4, isr4, 0x8E);
    idt_set_gate(5, isr5, 0x8E);
    idt_set_gate(6, isr6, 0x8E);
    idt_set_gate(7, isr7, 0x8E);
    idt_set_gate(8, isr8, 0x8E);
    idt_set_gate(9, isr9, 0x8E);
    idt_set_gate(10, isr10, 0x8E);
    idt_set_gate(11, isr11, 0x8E);
    idt_set_gate(12, isr12, 0x8E);
    idt_set_gate(13, isr13, 0x8E);
    idt_set_gate(14, isr14, 0x8E);
    idt_set_gate(15, isr15, 0x8E);
    idt_set_gate(16, isr16, 0x8E);
    idt_set_gate(17, isr17, 0x8E);
    idt_set_gate(18, isr18, 0x8E);
    idt_set_gate(19, isr19, 0x8E);
    idt_set_gate(20, isr20, 0x8E);
    idt_set_gate(21, isr21, 0x8E);
    idt_set_gate(22, isr22, 0x8E);
    idt_set_gate(23, isr23, 0x8E);
    idt_set_gate(24, isr24, 0x8E);
    idt_set_gate(25, isr25, 0x8E);
    idt_set_gate(26, isr26, 0x8E);
    idt_set_gate(27, isr27, 0x8E);
    idt_set_gate(28, isr28, 0x8E);
    idt_set_gate(29, isr29, 0x8E);
    idt_set_gate(30, isr30, 0x8E);
    idt_set_gate(31, isr31, 0x8E);

    idt_set_gate(32, isr32, 0x8E);
    idt_set_gate(33, isr33, 0x8E);
    idt_set_gate(34, isr34, 0x8E);
    idt_set_gate(35, isr35, 0x8E);
    idt_set_gate(36, isr36, 0x8E);
    idt_set_gate(37, isr37, 0x8E);
    idt_set_gate(38, isr38, 0x8E);
    idt_set_gate(39, isr39, 0x8E);
    idt_set_gate(40, isr40, 0x8E);
    idt_set_gate(41, isr41, 0x8E);
    idt_set_gate(42, isr42, 0x8E);
    idt_set_gate(43, isr43, 0x8E);
    idt_set_gate(44, isr44, 0x8E);
    idt_set_gate(45, isr45, 0x8E);
    idt_set_gate(46, isr46, 0x8E);
    idt_set_gate(47, isr47, 0x8E);

    idt_set_gate(128, isr128, 0xEE);

    pic_remap();

    __asm__ volatile ("lidt %0" : : "m"(idt_ptr));
    __asm__ volatile ("sti");
}
