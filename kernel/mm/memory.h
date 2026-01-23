#pragma once
#include "types.h"
#include "boot.h"

void memory_init(const boot_info_t* boot);
void* phys_alloc_page(void);
void phys_free_page(void* addr);
size_t memory_total_kib(void);
size_t memory_free_kib(void);
