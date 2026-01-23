#include "drivers/timer.h"
#include "arch/x86_64/ports.h"

static volatile uint64_t ticks;

void timer_init(uint32_t hz) {
    uint32_t divisor = 1193180 / hz;
    outb(0x43, 0x36);
    outb(0x40, divisor & 0xFF);
    outb(0x40, (divisor >> 8) & 0xFF);
}

uint64_t timer_ticks(void) {
    return ticks;
}

void timer_tick_internal(void) {
    ticks++;
}
