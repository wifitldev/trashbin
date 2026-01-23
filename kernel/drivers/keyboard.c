#include "drivers/keyboard.h"
#include "arch/x86_64/ports.h"

static char key_buffer[128];
static int key_head;
static int key_tail;

static const char scancode_map[128] = {
    0, 27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
    0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,
    '*', 0, ' ', 0
};

void keyboard_init(void) {
    key_head = 0;
    key_tail = 0;
}

static void push_key(char c) {
    int next = (key_head + 1) % (int)sizeof(key_buffer);
    if (next == key_tail) {
        return;
    }
    key_buffer[key_head] = c;
    key_head = next;
}

void keyboard_isr(void) {
    uint8_t sc = inb(0x60);
    if (sc & 0x80) {
        return;
    }
    char c = scancode_map[sc];
    if (c) {
        push_key(c);
    }
}

int keyboard_read(char* out) {
    if (out && key_tail != key_head) {
        *out = key_buffer[key_tail];
        key_tail = (key_tail + 1) % (int)sizeof(key_buffer);
        return 1;
    }
    return 0;
}
