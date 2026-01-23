#pragma once
#include "types.h"

void heap_init(void* start, size_t size);
void* kmalloc(size_t size);
void kfree(void* ptr);
