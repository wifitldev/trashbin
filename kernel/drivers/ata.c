#include "drivers/ata.h"
#include "arch/x86_64/ports.h"

int ata_read(uint64_t lba, uint8_t* buf, uint32_t sectors) {
    for (uint32_t s = 0; s < sectors; s++) {
        uint64_t cur = lba + s;
        outb(0x1F6, 0xE0 | ((cur >> 24) & 0x0F));
        outb(0x1F2, 1);
        outb(0x1F3, (uint8_t)(cur & 0xFF));
        outb(0x1F4, (uint8_t)((cur >> 8) & 0xFF));
        outb(0x1F5, (uint8_t)((cur >> 16) & 0xFF));
        outb(0x1F7, 0x20);

        uint8_t status;
        do {
            status = inb(0x1F7);
        } while (status & 0x80);

        if (status & 0x01) {
            return -1;
        }

        for (int i = 0; i < 256; i++) {
            uint16_t data = inw(0x1F0);
            buf[s * 512 + i * 2] = data & 0xFF;
            buf[s * 512 + i * 2 + 1] = data >> 8;
        }
    }
    return 0;
}
