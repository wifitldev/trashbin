[bits 16]
[org 0x7E00]

%define KERNEL_LOAD 0x00100000
%define BOOTINFO_ADDR 0x00007000
%define E820_ADDR 0x00005000
%define VBEINFO_ADDR 0x00006000

start:
    cli
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00

    mov [boot_drive], dl

    mov dword [BOOTINFO_ADDR + 0], 0x4F534F42
    mov dword [BOOTINFO_ADDR + 4], 0
    mov dword [BOOTINFO_ADDR + 8], E820_ADDR
    mov dword [BOOTINFO_ADDR + 12], 0
    mov dword [BOOTINFO_ADDR + 16], 0
    mov dword [BOOTINFO_ADDR + 20], 0

    call enable_a20
    call detect_memory
    call setup_vbe
    call load_kernel

    lgdt [gdt_descriptor]

    mov eax, cr0
    or eax, 1
    mov cr0, eax
    jmp CODE32:protected_mode

[bits 32]
protected_mode:
    mov ax, DATA32
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov esp, 0x90000

    call setup_paging

    mov ecx, 0xC0000080
    rdmsr
    or eax, 0x00000100
    wrmsr

    mov eax, cr4
    or eax, 0x00000020
    mov cr4, eax

    mov eax, cr0
    or eax, 0x80000000
    mov cr0, eax

    jmp CODE64:long_mode

[bits 64]
long_mode:
    mov ax, DATA64
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov rsp, 0x90000

    mov rdi, BOOTINFO_ADDR
    jmp KERNEL_LOAD

[bits 16]

load_kernel:
    mov si, dap
    mov word [dap+4], KERNEL_LOAD & 0xFFFF
    mov word [dap+6], KERNEL_LOAD >> 16
    mov dword [dap+8], KERNEL_LBA
    mov dword [dap+12], 0
    mov cx, KERNEL_SECTORS
    mov [dap+2], cx
    mov ah, 0x42
    mov dl, [boot_drive]
    int 0x13
    jc disk_error
    ret

setup_vbe:
    mov ax, 0x4F01
    mov cx, 0x118
    mov di, VBEINFO_ADDR
    int 0x10

    mov ax, 0x4F02
    mov bx, 0x4118
    int 0x10

    mov ax, [VBEINFO_ADDR + 0x12]
    mov [BOOTINFO_ADDR + 32], eax
    mov ax, [VBEINFO_ADDR + 0x14]
    mov [BOOTINFO_ADDR + 36], eax
    mov ax, [VBEINFO_ADDR + 0x10]
    mov [BOOTINFO_ADDR + 40], eax
    mov al, [VBEINFO_ADDR + 0x19]
    mov [BOOTINFO_ADDR + 44], eax

    mov eax, [VBEINFO_ADDR + 0x28]
    mov [BOOTINFO_ADDR + 24], eax
    mov dword [BOOTINFO_ADDR + 28], 0

    mov eax, FS_LBA
    mov [BOOTINFO_ADDR + 48], eax
    mov eax, FS_SECTORS
    mov [BOOTINFO_ADDR + 52], eax
    mov eax, KERNEL_LBA
    mov [BOOTINFO_ADDR + 56], eax
    mov eax, KERNEL_SECTORS
    mov [BOOTINFO_ADDR + 60], eax

    ret

detect_memory:
    xor ebx, ebx
    mov di, E820_ADDR
    mov dword [BOOTINFO_ADDR + 16], 0
.next:
    mov eax, 0xE820
    mov edx, 0x534D4150
    mov ecx, 24
    int 0x15
    jc .done
    cmp eax, 0x534D4150
    jne .done
    add di, 24
    inc dword [BOOTINFO_ADDR + 16]
    cmp ebx, 0
    jne .next
.done:
    ret

enable_a20:
    in al, 0x92
    or al, 2
    out 0x92, al
    ret

setup_paging:
    mov eax, PML4
    mov cr3, eax

    mov dword [PML4], PDPT + 0x003
    mov dword [PML4+4], 0
    mov dword [PDPT], PD + 0x003
    mov dword [PDPT+4], 0

    mov ecx, 0
.fill_pd:
    mov eax, ecx
    shl eax, 21
    or eax, 0x083
    mov [PD + ecx*8], eax
    mov dword [PD + ecx*8 + 4], 0
    inc ecx
    cmp ecx, 512
    jne .fill_pd

    ret

disk_error:
    mov si, err_msg
    call print
.hang:
    hlt
    jmp .hang

print:
    mov ah, 0x0E
.next:
    lodsb
    test al, al
    jz .done
    int 0x10
    jmp .next
.done:
    ret

boot_drive db 0
err_msg db 'Stage2 disk error',0

align 16

gdt:
    dq 0
    dq 0x00CF9A000000FFFF
    dq 0x00CF92000000FFFF
    dq 0x00AF9A000000FFFF
    dq 0x00AF92000000FFFF

gdt_descriptor:
    dw gdt_end - gdt - 1
    dd gdt

gdt_end:

CODE32 equ 0x08
DATA32 equ 0x10
CODE64 equ 0x18
DATA64 equ 0x20

align 4096
PML4: times 512 dq 0
align 4096
PDPT: times 512 dq 0
align 4096
PD: times 512 dq 0

align 16

dap:
    db 0x10
    db 0
    dw 0
    dw 0
    dw 0
    dq 0

    times (512*STAGE2_SECTORS)-($-$$) db 0
