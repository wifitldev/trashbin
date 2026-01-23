[bits 64]
section .text
global _start
extern kmain

_start:
    cli
    mov rsp, stack_top
    xor rbp, rbp
    mov rdi, rdi
    call kmain
.hang:
    hlt
    jmp .hang

section .bss
align 16
stack_bottom:
    resb 16384
stack_top:
