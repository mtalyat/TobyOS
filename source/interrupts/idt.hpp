#pragma once

#include "types/types.hpp"

struct Registers
{
    uint32 gs;
    uint32 fs;
    uint32 es;
    uint32 ds;

    uint32 edi;
    uint32 esi;
    uint32 ebp;
    uint32 esp;

    uint32 ebx;
    uint32 edx;
    uint32 ecx;
    uint32 eax;

    uint32 int_no;
    uint32 err_code;

    uint32 eip;
    uint32 cs;
    uint32 eflags;
};

// Interrupt Descriptor Table Entry
struct IDTEntry
{
    uint16 offset_low;      // Offset bits 0..15
    uint16 selector;        // Code segment selector
    uint8  ist;             // Interrupt Stack Table (bits 0..2), reserved (bits 3..7)
    uint8  type_attr;       // Type and attributes
    uint16 offset_high;     // Offset bits 16..31

    IDTEntry()
        : offset_low(0), selector(0), ist(0), type_attr(0), offset_high(0)
    {
    }

    void set(uint32 offset, uint16 sel, uint8 attr)
    {
        offset_low = offset & 0xFFFF;
        offset_high = (offset >> 16) & 0xFFFF;
        selector = sel;
        ist = 0;
        type_attr = attr;
    }
};

// IDTR structure (for lidt instruction)
#pragma pack(push, 1)
struct IDTR
{
    uint16 limit;
    uint32 base;
};
#pragma pack(pop)

// IDT attributes for 32-bit protected mode
#define IDT_ATTR_PRESENT        0x80
#define IDT_ATTR_DPL_0          0x00
#define IDT_ATTR_DPL_3          0x60
#define IDT_ATTR_INTERRUPT_GATE 0x0E
#define IDT_ATTR_TRAP_GATE      0x0F

#define IDT_INTERRUPT_GATE (IDT_ATTR_PRESENT | IDT_ATTR_DPL_0 | IDT_ATTR_INTERRUPT_GATE)
#define IDT_TRAP_GATE      (IDT_ATTR_PRESENT | IDT_ATTR_DPL_0 | IDT_ATTR_TRAP_GATE)

// Number of IDT entries (0-31 are CPU exceptions, 32+ are IRQs)
#define IDT_ENTRIES 256

class IDT
{
public:
    static void init();
    static void set_entry(uint8 index, uint32 offset, uint16 selector, uint8 attr);
    static void load();

private:
    static IDTEntry entries[IDT_ENTRIES];
    static IDTR idtr;
};

// PIC (Programmable Interrupt Controller) control
void pic_init();
void pic_eoi(uint8 irq);  // End of Interrupt

// PIT (Programmable Interval Timer) control
void pit_init(uint16 frequency);

// Generic interrupt handler typedef
extern "C" void isr_handler(Registers* regs);
extern "C" void irq_handler(Registers* regs);
