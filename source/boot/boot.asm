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
VBE_MODE_INFO equ 0x8000
BOOT_INFO equ 0x0500
VBE_MODE_LIST equ vbe_mode_list

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

    call select_graphics_mode
    jc graphics_error

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

graphics_error:
    mov si, graphics_error_msg
    call print_string
    jmp $

select_graphics_mode:
    pusha
    mov si, vbe_mode_list

.next_mode:
    lodsw
    cmp ax, 0
    je .fail

    mov [selected_mode], ax

    ; Query VBE mode info for the candidate mode.
    xor ax, ax
    mov es, ax
    mov di, VBE_MODE_INFO
    mov cx, [selected_mode]
    mov ax, 0x4F01
    int 0x10
    cmp ax, 0x004F
    jne .next_mode

    ; Bit 7 of the mode attributes indicates linear framebuffer support.
    test word [VBE_MODE_INFO], 0x0080
    jz .next_mode

    ; Set the VBE mode with the linear framebuffer bit enabled.
    mov bx, [selected_mode]
    or bx, 0x4000
    mov ax, 0x4F02
    int 0x10
    cmp ax, 0x004F
    jne .next_mode

    ; Publish framebuffer details for the kernel at a known low-memory address.
    mov eax, [VBE_MODE_INFO + 40]    ; PhysBasePtr
    mov [BOOT_INFO + 0], eax
    mov ax, [VBE_MODE_INFO + 18]     ; XResolution
    mov [BOOT_INFO + 4], ax
    mov ax, [VBE_MODE_INFO + 20]     ; YResolution
    mov [BOOT_INFO + 6], ax
    mov ax, [VBE_MODE_INFO + 16]     ; BytesPerScanLine
    mov [BOOT_INFO + 8], ax
    mov al, [VBE_MODE_INFO + 25]     ; BitsPerPixel
    mov [BOOT_INFO + 10], al
    mov al, [VBE_MODE_INFO + 31]     ; RedMaskSize
    mov [BOOT_INFO + 11], al
    mov al, [VBE_MODE_INFO + 32]     ; RedFieldPosition
    mov [BOOT_INFO + 12], al
    mov al, [VBE_MODE_INFO + 33]     ; GreenMaskSize
    mov [BOOT_INFO + 13], al
    mov al, [VBE_MODE_INFO + 34]     ; GreenFieldPosition
    mov [BOOT_INFO + 14], al
    mov al, [VBE_MODE_INFO + 35]     ; BlueMaskSize
    mov [BOOT_INFO + 15], al
    mov al, [VBE_MODE_INFO + 36]     ; BlueFieldPosition
    mov [BOOT_INFO + 16], al
    xor ax, ax
    mov [BOOT_INFO + 17], al

    popa
    clc
    ret

.fail:
    popa
    stc
    ret

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
    ; Keep stack above kernel .bss (framebuffer backbuffer is large).
    mov esp, 0x800000

    mov eax, KERNEL_OFFSET
    call eax

.hang:
    cli
    hlt
    jmp .hang

BITS 16
boot_drive db 0
disk_error_msg db 'Disk read error', 0
graphics_error_msg db 'No suitable graphics mode', 0
selected_mode dw 0

vbe_mode_list:
    dw 0x118    ; 1024x768x16/24 depending on adapter
    dw 0x115    ; 800x600x24
    dw 0x114    ; 800x600x16
    dw 0x112    ; 640x480x32
    dw 0x101    ; 640x480x8 fallback
    dw 0x0000

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