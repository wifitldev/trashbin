#include "console.h"
#include "lib/string.h"
#include "types.h"

static void print_hex(uint64_t val) {
    const char* hex = "0123456789ABCDEF";
    char buf[17];
    buf[16] = '\0';
    for (int i = 15; i >= 0; i--) {
        buf[i] = hex[val & 0xF];
        val >>= 4;
    }
    console_write("0x");
    console_write(buf);
}

static void print_dec(int64_t val) {
    char buf[24];
    int i = 0;
    int neg = 0;
    if (val < 0) {
        neg = 1;
        val = -val;
    }
    do {
        buf[i++] = '0' + (val % 10);
        val /= 10;
    } while (val && i < 23);
    if (neg) {
        buf[i++] = '-';
    }
    for (int j = i - 1; j >= 0; j--) {
        console_putc(buf[j]);
    }
}

int kprintf(const char* fmt, ...) {
    __builtin_va_list args;
    __builtin_va_start(args, fmt);
    for (size_t i = 0; fmt[i]; i++) {
        if (fmt[i] != '%') {
            console_putc(fmt[i]);
            continue;
        }
        i++;
        char c = fmt[i];
        if (c == 's') {
            const char* s = __builtin_va_arg(args, const char*);
            console_write(s ? s : "(null)");
        } else if (c == 'd') {
            int val = __builtin_va_arg(args, int);
            print_dec(val);
        } else if (c == 'x') {
            uint64_t val = __builtin_va_arg(args, uint64_t);
            print_hex(val);
        } else if (c == 'c') {
            char v = (char)__builtin_va_arg(args, int);
            console_putc(v);
        } else if (c == '%') {
            console_putc('%');
        }
    }
    __builtin_va_end(args);
    return 0;
}
