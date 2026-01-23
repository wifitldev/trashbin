#include "drivers/mouse.h"
#include "arch/x86_64/ports.h"

static mouse_state_t state;
static uint8_t packet[3];
static uint8_t packet_index;

static void mouse_write(uint8_t val) {
    outb(0x64, 0xD4);
    outb(0x60, val);
}

static uint8_t mouse_read(void) {
    return inb(0x60);
}

void mouse_init(void) {
    outb(0x64, 0xA8);
    outb(0x64, 0x20);
    uint8_t status = inb(0x60) | 2;
    outb(0x64, 0x60);
    outb(0x60, status);

    mouse_write(0xF6);
    mouse_read();
    mouse_write(0xF4);
    mouse_read();

    state.x = 320;
    state.y = 240;
    state.buttons = 0;
    packet_index = 0;
}

static void handle_packet(void) {
    int dx = (int8_t)packet[1];
    int dy = (int8_t)packet[2];
    state.x += dx;
    state.y -= dy;
    if (state.x < 0) state.x = 0;
    if (state.y < 0) state.y = 0;
    if (state.x > 1023) state.x = 1023;
    if (state.y > 767) state.y = 767;
    state.buttons = packet[0] & 0x7;
}

mouse_state_t mouse_state(void) {
    while (inb(0x64) & 1) {
        uint8_t data = inb(0x60);
        if (packet_index == 0 && !(data & 0x08)) {
            continue;
        }
        packet[packet_index++] = data;
        if (packet_index == 3) {
            packet_index = 0;
            handle_packet();
        }
    }
    return state;
}
