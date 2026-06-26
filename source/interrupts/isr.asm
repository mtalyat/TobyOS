; isr_asm.asm - Interrupt Service Routine stubs
; Handles CPU exceptions (0-31) and IRQs (32-47)

BITS 32

; Extern C handlers
extern isr_handler
extern irq_handler

; Macro to create ISR stub without error code
%macro ISR_NOERRCODE 1
global isr%1
isr%1:
    push dword 0           ; Push dummy error code
    push dword %1          ; Push interrupt number
    jmp isr_common
%endmacro

; Macro to create ISR stub with error code (CPU already pushes it)
%macro ISR_ERRCODE 1
global isr%1
isr%1:
    push dword %1          ; Push interrupt number
    jmp isr_common
%endmacro

; Macro to create IRQ stub
%macro IRQ_HANDLER 1
global irq%1
irq%1:
    push dword 0           ; Push dummy error code
    push dword (%1 + 32)   ; Push IRQ number
    jmp irq_common
%endmacro

; CPU Exceptions (0-31)
ISR_NOERRCODE 0    ; Divide by zero
ISR_NOERRCODE 1    ; Debug
ISR_NOERRCODE 2    ; Non-maskable interrupt
ISR_NOERRCODE 3    ; Breakpoint
ISR_NOERRCODE 4    ; Overflow
ISR_NOERRCODE 5    ; Bound range exceeded
ISR_NOERRCODE 6    ; Invalid opcode
ISR_NOERRCODE 7    ; Device not available
ISR_ERRCODE 8      ; Double fault (has error code)
ISR_NOERRCODE 9    ; Coprocessor segment overrun
ISR_ERRCODE 10     ; Invalid TSS (has error code)
ISR_ERRCODE 11     ; Segment not present (has error code)
ISR_ERRCODE 12     ; Stack-segment fault (has error code)
ISR_ERRCODE 13     ; General protection fault (has error code)
ISR_ERRCODE 14     ; Page fault (has error code)
ISR_NOERRCODE 15   ; Reserved
ISR_NOERRCODE 16   ; x87 FPU error
ISR_ERRCODE 17     ; Alignment check (has error code)
ISR_NOERRCODE 18   ; Machine check
ISR_NOERRCODE 19   ; SIMD floating-point
ISR_NOERRCODE 20   ; Virtualization exception
ISR_ERRCODE 21     ; Control protection exception (has error code)
ISR_NOERRCODE 22   ; Reserved
ISR_NOERRCODE 23   ; Reserved
ISR_NOERRCODE 24   ; Reserved
ISR_NOERRCODE 25   ; Reserved
ISR_NOERRCODE 26   ; Reserved
ISR_NOERRCODE 27   ; Reserved
ISR_NOERRCODE 28   ; Reserved
ISR_NOERRCODE 29   ; Reserved
ISR_NOERRCODE 30   ; Reserved
ISR_NOERRCODE 31   ; Reserved

; IRQ Handlers (0-15, mapped to vectors 32-47)
IRQ_HANDLER 0      ; Timer
IRQ_HANDLER 1      ; Keyboard
IRQ_HANDLER 2      ; Cascade
IRQ_HANDLER 3      ; Serial port 2
IRQ_HANDLER 4      ; Serial port 1
IRQ_HANDLER 5      ; Parallel port 2
IRQ_HANDLER 6      ; Floppy disk
IRQ_HANDLER 7      ; Parallel port 1
IRQ_HANDLER 8      ; Real-time clock
IRQ_HANDLER 9      ; Reserved
IRQ_HANDLER 10     ; Reserved
IRQ_HANDLER 11     ; Reserved
IRQ_HANDLER 12     ; PS/2 mouse
IRQ_HANDLER 13     ; FPU coprocessor
IRQ_HANDLER 14     ; ATA primary
IRQ_HANDLER 15     ; ATA secondary

; Common ISR handler
isr_common:
    pusha              ; Push all general-purpose registers
    push ds            ; Push data segment
    push es
    push fs
    push gs

    mov eax, 0x10     ; Load kernel data segment
    mov ds, eax
    mov es, eax
    mov fs, eax
    mov gs, eax

    mov eax, esp      ; Stack pointer to eax
    push eax          ; Pass as parameter
    call isr_handler

    pop eax           ; Clean up
    pop gs            ; Restore segments
    pop fs
    pop es
    pop ds
    popa              ; Restore general-purpose registers

    add esp, 8        ; Remove error code and interrupt number
    iret              ; Return from interrupt

; Common IRQ handler
irq_common:
    pusha              ; Push all general-purpose registers
    push ds            ; Push data segment
    push es
    push fs
    push gs

    mov eax, 0x10     ; Load kernel data segment
    mov ds, eax
    mov es, eax
    mov fs, eax
    mov gs, eax

    mov eax, esp      ; Stack pointer to eax
    push eax          ; Pass as parameter
    call irq_handler

    pop eax           ; Clean up
    pop gs            ; Restore segments
    pop fs
    pop es
    pop ds
    popa              ; Restore general-purpose registers

    add esp, 8        ; Remove dummy error code and IRQ number
    iret              ; Return from interrupt
