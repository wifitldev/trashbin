#pragma once
#include "types.h"

#define OSFS_MAGIC 0x5346534F
#define OSFS_MAX_NAME 32
#define OSFS_MAX_ENTRIES 128
#define OSFS_BLOCK_SIZE 512

typedef struct {
    uint32_t magic;
    uint32_t entry_count;
    uint32_t data_start_lba;
    uint32_t data_blocks;
} __attribute__((packed)) osfs_super_t;

typedef struct {
    uint32_t parent;
    uint32_t start_block;
    uint32_t size;
    uint8_t type;
    uint8_t reserved[3];
    char name[OSFS_MAX_NAME];
} __attribute__((packed)) osfs_entry_t;

#define OSFS_TYPE_FILE 1
#define OSFS_TYPE_DIR 2
