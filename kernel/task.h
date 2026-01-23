#pragma once
#include "types.h"

typedef void (*task_entry_t)(void*);

typedef struct context {
    uint64_t r15, r14, r13, r12, r11, r10, r9, r8;
    uint64_t rsi, rdi, rbp, rdx, rcx, rbx, rax;
    uint64_t rsp;
} context_t;

typedef struct task {
    context_t context;
    uint64_t id;
    uint8_t active;
    char name[32];
    struct task* next;
} task_t;

void task_init(void);
uint64_t task_spawn(const char* name, task_entry_t entry, void* arg);
void task_yield(void);
void task_tick(void);
void task_list(void);
