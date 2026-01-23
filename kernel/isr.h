#pragma once
#include "types.h"

typedef struct {
    uint64_t r15, r14, r13, r12, r11, r10, r9, r8;
    uint64_t rdi, rsi, rbp, rdx, rcx, rbx, rax;
    uint64_t int_no, err_code;
} regs_t;

void isr_handler(regs_t* regs);
