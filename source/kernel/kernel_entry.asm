; kernel_entry.asm

BITS 32

global _start
extern kernel_main

_start:
    mov eax, [esp + 4]
    push eax
    call kernel_main
    add esp, 4

.hang:
    jmp .hang