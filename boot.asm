; boot.asm
; Stage-1 bootloader: load kernel, switch to protected mode, jump to kernel entry.

BITS 16
ORG 0x7C00

KERNEL_OFFSET equ 0x1000

%ifndef KERNEL_SECTORS
%define KERNEL_SECTORS 20
%endif

CODE_SEG equ 0x08
DATA_SEG equ 0x10

start:
    cli
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00
    sti

    mov [boot_drive], dl

    mov bx, KERNEL_OFFSET
    mov dh, KERNEL_SECTORS
    call disk_load

    cli
    lgdt [gdt_descriptor]
    mov eax, cr0
    or eax, 0x1
    mov cr0, eax
    jmp CODE_SEG:protected_mode_start

disk_load:
    pusha
    mov ah, 0x02        ; BIOS read sectors
    mov al, dh          ; sector count
    mov ch, 0x00        ; cylinder
    mov dh, 0x00        ; head
    mov cl, 0x02        ; start at sector 2 (sector 1 is boot sector)
    mov dl, [boot_drive]
    int 0x13
    jc disk_error
    popa
    ret

disk_error:
    mov si, disk_error_msg
    call print_string
    jmp $

print_string:
    pusha
    mov ah, 0x0E
.print_char:
    lodsb
    test al, al
    jz .done
    int 0x10
    jmp .print_char
.done:
    popa
    ret

BITS 32
protected_mode_start:
    mov ax, DATA_SEG
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov esp, 0x90000

    mov eax, KERNEL_OFFSET
    call eax

.hang:
    cli
    hlt
    jmp .hang

BITS 16
boot_drive db 0
disk_error_msg db 'Disk read error', 0

gdt_start:
gdt_null:
    dd 0x00000000
    dd 0x00000000

gdt_code:
    dw 0xFFFF
    dw 0x0000
    db 0x00
    db 10011010b
    db 11001111b
    db 0x00

gdt_data:
    dw 0xFFFF
    dw 0x0000
    db 0x00
    db 10010010b
    db 11001111b
    db 0x00

gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start

times 510 - ($ - $$) db 0
dw 0xAA55