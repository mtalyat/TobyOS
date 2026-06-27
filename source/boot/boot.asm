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
BOOT_INFO_REGION_COUNT equ BOOT_INFO + 0
BOOT_INFO_REGIONS equ BOOT_INFO + 4
BOOT_INFO_FRAMEBUFFER_ADDR equ BOOT_INFO + 1284
BOOT_INFO_WIDTH equ BOOT_INFO + 1288
BOOT_INFO_HEIGHT equ BOOT_INFO + 1290
BOOT_INFO_PITCH equ BOOT_INFO + 1292
BOOT_INFO_BPP equ BOOT_INFO + 1294
BOOT_INFO_RED_MASK_SIZE equ BOOT_INFO + 1295
BOOT_INFO_RED_SHIFT equ BOOT_INFO + 1296
BOOT_INFO_GREEN_MASK_SIZE equ BOOT_INFO + 1297
BOOT_INFO_GREEN_SHIFT equ BOOT_INFO + 1298
BOOT_INFO_BLUE_MASK_SIZE equ BOOT_INFO + 1299
BOOT_INFO_BLUE_SHIFT equ BOOT_INFO + 1300
BOOT_INFO_RESERVED equ BOOT_INFO + 1301
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

    call detect_memory_map

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
    mov [BOOT_INFO_FRAMEBUFFER_ADDR], eax
    mov ax, [VBE_MODE_INFO + 18]     ; XResolution
    mov [BOOT_INFO_WIDTH], ax
    mov ax, [VBE_MODE_INFO + 20]     ; YResolution
    mov [BOOT_INFO_HEIGHT], ax
    mov ax, [VBE_MODE_INFO + 16]     ; BytesPerScanLine
    mov [BOOT_INFO_PITCH], ax
    mov al, [VBE_MODE_INFO + 25]     ; BitsPerPixel
    mov [BOOT_INFO_BPP], al
    mov al, [VBE_MODE_INFO + 31]     ; RedMaskSize
    mov [BOOT_INFO_RED_MASK_SIZE], al
    mov al, [VBE_MODE_INFO + 32]     ; RedFieldPosition
    mov [BOOT_INFO_RED_SHIFT], al
    mov al, [VBE_MODE_INFO + 33]     ; GreenMaskSize
    mov [BOOT_INFO_GREEN_MASK_SIZE], al
    mov al, [VBE_MODE_INFO + 34]     ; GreenFieldPosition
    mov [BOOT_INFO_GREEN_SHIFT], al
    mov al, [VBE_MODE_INFO + 35]     ; BlueMaskSize
    mov [BOOT_INFO_BLUE_MASK_SIZE], al
    mov al, [VBE_MODE_INFO + 36]     ; BlueFieldPosition
    mov [BOOT_INFO_BLUE_SHIFT], al
    xor ax, ax
    mov [BOOT_INFO_RESERVED], al

    popa
    clc
    ret

.fail:
    popa
    stc
    ret

detect_memory_map:
    pusha

    xor eax, eax
    mov [BOOT_INFO_REGION_COUNT], eax
    xor ebx, ebx

.next_entry:
    xor ax, ax
    mov es, ax
    mov di, e820_buffer

    mov eax, 0xE820
    mov edx, 0x534D4150
    mov ecx, 20
    int 0x15
    jc .done
    cmp eax, 0x534D4150
    jne .done

    mov eax, [BOOT_INFO_REGION_COUNT]
    cmp eax, 64
    jae .continue

    ; destination = BOOT_INFO_REGIONS + (region_count * 20)
    mov ax, [BOOT_INFO_REGION_COUNT]
    mov dx, ax
    shl ax, 4
    shl dx, 2
    add ax, dx
    add ax, BOOT_INFO_REGIONS
    mov di, ax

    mov si, e820_buffer
    mov cx, 20
    rep movsb

    mov eax, [BOOT_INFO_REGION_COUNT]
    inc eax
    mov [BOOT_INFO_REGION_COUNT], eax

.continue:
    test ebx, ebx
    jne .next_entry

.done:
    popa
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

    mov eax, BOOT_INFO
    push eax
    mov eax, KERNEL_OFFSET
    call eax
    add esp, 4

.hang:
    cli
    hlt
    jmp .hang

BITS 16
boot_drive db 0
disk_error_msg db 'Disk read error', 0
graphics_error_msg db 'No suitable graphics mode', 0
selected_mode dw 0
e820_buffer:
    times 20 db 0

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