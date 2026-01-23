#pragma once
#include "types.h"

void paging_init(void);
void* paging_map(uint64_t phys, uint64_t virt);
