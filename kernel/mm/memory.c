#include "mm/memory.h"
#include "lib/string.h"
#include "console.h"

#define MAX_PAGES 65536
static uint8_t bitmap[MAX_PAGES / 8];
static size_t total_pages;
static size_t free_pages;

static void bitmap_set(size_t idx) {
    bitmap[idx / 8] |= (1 << (idx % 8));
}

static void bitmap_clear(size_t idx) {
    bitmap[idx / 8] &= ~(1 << (idx % 8));
}

static int bitmap_test(size_t idx) {
    return bitmap[idx / 8] & (1 << (idx % 8));
}

void memory_init(const boot_info_t* boot) {
    memset(bitmap, 0xFF, sizeof(bitmap));
    total_pages = 0;
    free_pages = 0;

    const e820_entry_t* entries = (const e820_entry_t*)(uintptr_t)boot->mem_map;
    for (uint32_t i = 0; i < boot->mem_entries; i++) {
        if (entries[i].type != 1) {
            continue;
        }
        uint64_t base = entries[i].base;
        uint64_t length = entries[i].length;
        uint64_t end = base + length;
        for (uint64_t addr = base; addr + 4096 <= end; addr += 4096) {
            size_t page = addr / 4096;
            if (page < MAX_PAGES) {
                bitmap_clear(page);
                total_pages++;
                free_pages++;
            }
        }
    }

    for (uint64_t addr = 0; addr < 0x200000; addr += 4096) {
        size_t page = addr / 4096;
        if (page < MAX_PAGES && !bitmap_test(page)) {
            bitmap_set(page);
            free_pages--;
        }
    }

    console_write("Memory manager initialized\n");
}

void* phys_alloc_page(void) {
    for (size_t i = 0; i < MAX_PAGES; i++) {
        if (!bitmap_test(i)) {
            bitmap_set(i);
            free_pages--;
            return (void*)(i * 4096);
        }
    }
    return NULL;
}

void phys_free_page(void* addr) {
    size_t page = (uintptr_t)addr / 4096;
    if (page < MAX_PAGES && bitmap_test(page)) {
        bitmap_clear(page);
        free_pages++;
    }
}

size_t memory_total_kib(void) {
    return total_pages * 4;
}

size_t memory_free_kib(void) {
    return free_pages * 4;
}
