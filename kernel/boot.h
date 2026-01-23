#pragma once
#include "types.h"

typedef struct {
    uint64_t magic;
    uint64_t mem_map;
    uint32_t mem_entries;
    uint32_t reserved0;
    uint64_t fb_addr;
    uint32_t fb_width;
    uint32_t fb_height;
    uint32_t fb_pitch;
    uint32_t fb_bpp;
    uint64_t fs_lba;
    uint64_t fs_sectors;
    uint64_t kernel_lba;
    uint64_t kernel_sectors;
} boot_info_t;

typedef struct {
    uint64_t base;
    uint64_t length;
    uint32_t type;
    uint32_t acpi;
} __attribute__((packed)) e820_entry_t;
