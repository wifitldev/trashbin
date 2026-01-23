#include "mm/paging.h"

void paging_init(void) {
    __asm__ volatile ("mov %%cr3, %%rax" : : : "rax");
}

void* paging_map(uint64_t phys, uint64_t virt) {
    (void)phys;
    return (void*)virt;
}
