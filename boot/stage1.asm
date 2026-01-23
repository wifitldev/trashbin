[bits 16]
[org 0x7C00]

start:
    cli
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00

    mov [BOOT_DRIVE], dl

    mov si, dap
    mov word [dap+4], 0x7E00
    mov word [dap+6], 0x0000
    mov dword [dap+8], 1
    mov dword [dap+12], 0

    mov ah, 0x42
    mov dl, [BOOT_DRIVE]
    int 0x13
    jc disk_error

    jmp 0x0000:0x7E00

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

BOOT_DRIVE: db 0

err_msg db 'Stage1 disk error', 0

align 16

dap:
    db 0x10
    db 0
    dw STAGE2_SECTORS
    dw 0x7E00
    dw 0x0000
    dq 1

    times 510-($-$$) db 0
    dw 0xAA55
