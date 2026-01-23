#include "mm/heap.h"
#include "lib/string.h"

static uint8_t* heap_start;
static uint8_t* heap_end;
static uint8_t* heap_curr;

void heap_init(void* start, size_t size) {
    heap_start = (uint8_t*)start;
    heap_end = heap_start + size;
    heap_curr = heap_start;
    memset(heap_start, 0, size);
}

void* kmalloc(size_t size) {
    size = (size + 15) & ~((size_t)15);
    if (heap_curr + size >= heap_end) {
        return NULL;
    }
    void* ptr = heap_curr;
    heap_curr += size;
    return ptr;
}

void kfree(void* ptr) {
    (void)ptr;
}
