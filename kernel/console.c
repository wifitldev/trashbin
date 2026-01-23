#include "console.h"
#include "lib/string.h"
#include "types.h"

static volatile uint16_t* vga = (uint16_t*)0xB8000;
static uint8_t cursor_x;
static uint8_t cursor_y;
static uint8_t color = 0x0F;

static void console_scroll(void) {
    if (cursor_y < 25) {
        return;
    }
    for (int y = 1; y < 25; y++) {
        for (int x = 0; x < 80; x++) {
            vga[(y - 1) * 80 + x] = vga[y * 80 + x];
        }
    }
    for (int x = 0; x < 80; x++) {
        vga[24 * 80 + x] = (uint16_t)color << 8 | ' ';
    }
    cursor_y = 24;
}

void console_init(void) {
    cursor_x = 0;
    cursor_y = 0;
    console_clear();
}

void console_set_color(uint8_t fg, uint8_t bg) {
    color = (bg << 4) | (fg & 0x0F);
}

void console_putc(char c) {
    if (c == '\n') {
        cursor_x = 0;
        cursor_y++;
        console_scroll();
        return;
    }
    if (c == '\r') {
        cursor_x = 0;
        return;
    }
    vga[cursor_y * 80 + cursor_x] = (uint16_t)color << 8 | (uint8_t)c;
    cursor_x++;
    if (cursor_x >= 80) {
        cursor_x = 0;
        cursor_y++;
        console_scroll();
    }
}

void console_write(const char* s) {
    for (size_t i = 0; s[i]; i++) {
        console_putc(s[i]);
    }
}

void console_clear(void) {
    for (int y = 0; y < 25; y++) {
        for (int x = 0; x < 80; x++) {
            vga[y * 80 + x] = (uint16_t)color << 8 | ' ';
        }
    }
    cursor_x = 0;
    cursor_y = 0;
}
