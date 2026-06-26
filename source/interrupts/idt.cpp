#include "idt.hpp"
#include "interrupt_handlers.hpp"
#include "interrupt_type.hpp"

#if defined(__INTELLISENSE__)
#define ASM_VOLATILE(...)
#elif defined(__GNUC__) || defined(__clang__)
#define ASM_VOLATILE(...) __asm__ __volatile__(__VA_ARGS__)
#else
#define ASM_VOLATILE(...)
#endif

// Define the IDT and IDTR in code section
IDTEntry IDT::entries[IDT_ENTRIES];
IDTR IDT::idtr = {sizeof(IDTEntry) * IDT_ENTRIES - 1, reinterpret_cast<uint32>(entries)};

// Forward declarations for ISR/IRQ handlers
extern "C"
{
    void isr0();
    void isr1();
    void isr2();
    void isr3();
    void isr4();
    void isr5();
    void isr6();
    void isr7();
    void isr8();
    void isr9();
    void isr10();
    void isr11();
    void isr12();
    void isr13();
    void isr14();
    void isr15();
    void isr16();
    void isr17();
    void isr18();
    void isr19();
    void isr20();
    void isr21();
    void isr22();
    void isr23();
    void isr24();
    void isr25();
    void isr26();
    void isr27();
    void isr28();
    void isr29();
    void isr30();
    void isr31();

    void irq0();
    void irq1();
    void irq2();
    void irq3();
    void irq4();
    void irq5();
    void irq6();
    void irq7();
    void irq8();
    void irq9();
    void irq10();
    void irq11();
    void irq12();
    void irq13();
    void irq14();
    void irq15();
}

void IDT::init()
{
    // Install CPU exceptions (ISR 0-31)
    set_entry(0, reinterpret_cast<uint32>(isr0), 0x08, IDT_INTERRUPT_GATE);
    set_entry(1, reinterpret_cast<uint32>(isr1), 0x08, IDT_INTERRUPT_GATE);
    set_entry(2, reinterpret_cast<uint32>(isr2), 0x08, IDT_INTERRUPT_GATE);
    set_entry(3, reinterpret_cast<uint32>(isr3), 0x08, IDT_INTERRUPT_GATE);
    set_entry(4, reinterpret_cast<uint32>(isr4), 0x08, IDT_INTERRUPT_GATE);
    set_entry(5, reinterpret_cast<uint32>(isr5), 0x08, IDT_INTERRUPT_GATE);
    set_entry(6, reinterpret_cast<uint32>(isr6), 0x08, IDT_INTERRUPT_GATE);
    set_entry(7, reinterpret_cast<uint32>(isr7), 0x08, IDT_INTERRUPT_GATE);
    set_entry(8, reinterpret_cast<uint32>(isr8), 0x08, IDT_INTERRUPT_GATE);
    set_entry(9, reinterpret_cast<uint32>(isr9), 0x08, IDT_INTERRUPT_GATE);
    set_entry(10, reinterpret_cast<uint32>(isr10), 0x08, IDT_INTERRUPT_GATE);
    set_entry(11, reinterpret_cast<uint32>(isr11), 0x08, IDT_INTERRUPT_GATE);
    set_entry(12, reinterpret_cast<uint32>(isr12), 0x08, IDT_INTERRUPT_GATE);
    set_entry(13, reinterpret_cast<uint32>(isr13), 0x08, IDT_INTERRUPT_GATE);
    set_entry(14, reinterpret_cast<uint32>(isr14), 0x08, IDT_INTERRUPT_GATE);
    set_entry(15, reinterpret_cast<uint32>(isr15), 0x08, IDT_INTERRUPT_GATE);
    set_entry(16, reinterpret_cast<uint32>(isr16), 0x08, IDT_INTERRUPT_GATE);
    set_entry(17, reinterpret_cast<uint32>(isr17), 0x08, IDT_INTERRUPT_GATE);
    set_entry(18, reinterpret_cast<uint32>(isr18), 0x08, IDT_INTERRUPT_GATE);
    set_entry(19, reinterpret_cast<uint32>(isr19), 0x08, IDT_INTERRUPT_GATE);
    set_entry(20, reinterpret_cast<uint32>(isr20), 0x08, IDT_INTERRUPT_GATE);
    set_entry(21, reinterpret_cast<uint32>(isr21), 0x08, IDT_INTERRUPT_GATE);
    set_entry(22, reinterpret_cast<uint32>(isr22), 0x08, IDT_INTERRUPT_GATE);
    set_entry(23, reinterpret_cast<uint32>(isr23), 0x08, IDT_INTERRUPT_GATE);
    set_entry(24, reinterpret_cast<uint32>(isr24), 0x08, IDT_INTERRUPT_GATE);
    set_entry(25, reinterpret_cast<uint32>(isr25), 0x08, IDT_INTERRUPT_GATE);
    set_entry(26, reinterpret_cast<uint32>(isr26), 0x08, IDT_INTERRUPT_GATE);
    set_entry(27, reinterpret_cast<uint32>(isr27), 0x08, IDT_INTERRUPT_GATE);
    set_entry(28, reinterpret_cast<uint32>(isr28), 0x08, IDT_INTERRUPT_GATE);
    set_entry(29, reinterpret_cast<uint32>(isr29), 0x08, IDT_INTERRUPT_GATE);
    set_entry(30, reinterpret_cast<uint32>(isr30), 0x08, IDT_INTERRUPT_GATE);
    set_entry(31, reinterpret_cast<uint32>(isr31), 0x08, IDT_INTERRUPT_GATE);

    // Install IRQ handlers (mapped to vectors 32-47)
    set_entry(32, reinterpret_cast<uint32>(irq0), 0x08, IDT_INTERRUPT_GATE);
    set_entry(33, reinterpret_cast<uint32>(irq1), 0x08, IDT_INTERRUPT_GATE);
    set_entry(34, reinterpret_cast<uint32>(irq2), 0x08, IDT_INTERRUPT_GATE);
    set_entry(35, reinterpret_cast<uint32>(irq3), 0x08, IDT_INTERRUPT_GATE);
    set_entry(36, reinterpret_cast<uint32>(irq4), 0x08, IDT_INTERRUPT_GATE);
    set_entry(37, reinterpret_cast<uint32>(irq5), 0x08, IDT_INTERRUPT_GATE);
    set_entry(38, reinterpret_cast<uint32>(irq6), 0x08, IDT_INTERRUPT_GATE);
    set_entry(39, reinterpret_cast<uint32>(irq7), 0x08, IDT_INTERRUPT_GATE);
    set_entry(40, reinterpret_cast<uint32>(irq8), 0x08, IDT_INTERRUPT_GATE);
    set_entry(41, reinterpret_cast<uint32>(irq9), 0x08, IDT_INTERRUPT_GATE);
    set_entry(42, reinterpret_cast<uint32>(irq10), 0x08, IDT_INTERRUPT_GATE);
    set_entry(43, reinterpret_cast<uint32>(irq11), 0x08, IDT_INTERRUPT_GATE);
    set_entry(44, reinterpret_cast<uint32>(irq12), 0x08, IDT_INTERRUPT_GATE);
    set_entry(45, reinterpret_cast<uint32>(irq13), 0x08, IDT_INTERRUPT_GATE);
    set_entry(46, reinterpret_cast<uint32>(irq14), 0x08, IDT_INTERRUPT_GATE);
    set_entry(47, reinterpret_cast<uint32>(irq15), 0x08, IDT_INTERRUPT_GATE);

    load();
}

void IDT::set_entry(uint8 index, uint32 offset, uint16 selector, uint8 attr)
{
    if (index < IDT_ENTRIES)
    {
        entries[index].set(offset, selector, attr);
    }
}

void IDT::load()
{
    // Update IDTR base address
    idtr.base = reinterpret_cast<uint32>(entries);
    idtr.limit = sizeof(IDTEntry) * IDT_ENTRIES - 1;

    // Load IDT using inline asm
    ASM_VOLATILE("lidt %0" : : "m"(idtr));
}

// PIC chip I/O ports
#define PIC1_COMMAND 0x20
#define PIC1_DATA 0x21
#define PIC2_COMMAND 0xA0
#define PIC2_DATA 0xA1

// PIC init control words
#define ICW1 0x11
#define ICW4 0x01

void pic_init()
{
    // ICW1: Start init, cascade mode, ICW4 needed
    ASM_VOLATILE("out %b0, %b1" : : "a"(ICW1), "Nd"(PIC1_COMMAND));
    ASM_VOLATILE("out %b0, %b1" : : "a"(ICW1), "Nd"(PIC2_COMMAND));

    // ICW2: Set base vectors (PIC1 at 32, PIC2 at 40)
    ASM_VOLATILE("out %b0, %b1" : : "a"(0x20), "Nd"(PIC1_DATA));
    ASM_VOLATILE("out %b0, %b1" : : "a"(0x28), "Nd"(PIC2_DATA));

    // ICW3: Tell master about slave at IRQ2
    ASM_VOLATILE("out %b0, %b1" : : "a"(0x04), "Nd"(PIC1_DATA));
    ASM_VOLATILE("out %b0, %b1" : : "a"(0x02), "Nd"(PIC2_DATA));

    // ICW4: 8086 mode
    ASM_VOLATILE("out %b0, %b1" : : "a"(ICW4), "Nd"(PIC1_DATA));
    ASM_VOLATILE("out %b0, %b1" : : "a"(ICW4), "Nd"(PIC2_DATA));

    // OCW1: Unmask all IRQs (0x00 = unmask all, 0xFF = mask all)
    ASM_VOLATILE("out %b0, %b1" : : "a"(0x00), "Nd"(PIC1_DATA));
    ASM_VOLATILE("out %b0, %b1" : : "a"(0x00), "Nd"(PIC2_DATA));
}

void pic_eoi(uint8 irq)
{
    if (irq >= 8)
    {
        // Send EOI to slave PIC
        ASM_VOLATILE("out %b0, %b1" : : "a"(0x20), "Nd"(PIC2_COMMAND));
    }
    // Send EOI to master PIC
    ASM_VOLATILE("out %b0, %b1" : : "a"(0x20), "Nd"(PIC1_COMMAND));
}

// PIT chip I/O ports
#define PIT_CHANNEL0 0x40
#define PIT_CONTROL 0x43

// PIT frequency (1193182 Hz base clock)
#define PIT_BASE_FREQ 1193182

void pit_init(uint16 frequency)
{
    // Calculate divisor
    uint16 divisor = PIT_BASE_FREQ / frequency;

    // Send control byte: Channel 0, Both bytes, Rate generator mode
    ASM_VOLATILE("out %b0, %b1" : : "a"(0x34), "Nd"(PIT_CONTROL));

    // Send divisor (low byte, then high byte)
    ASM_VOLATILE("out %b0, %b1" : : "a"(static_cast<uint8>(divisor & 0xFF)), "Nd"(PIT_CHANNEL0));
    ASM_VOLATILE("out %b0, %b1" : : "a"(static_cast<uint8>((divisor >> 8) & 0xFF)), "Nd"(PIT_CHANNEL0));
}

extern "C"
{
    void isr_handler(Registers *regs)
    {
        // CPU exception handler - can be customized
        InterruptType int_type = static_cast<InterruptType>(regs->int_no);
        switch (int_type)
        {
        default:
            break;
        }
    }

    void irq_handler(Registers *regs)
    {
        // IRQ handler - can be customized
        InterruptType int_type = static_cast<InterruptType>(regs->int_no);
        switch (int_type)
        {
        case InterruptType::Timer:
            interrupt_timer();
            break;
        case InterruptType::Keyboard:
            interrupt_keyboard();
            break;
        default:
            break;
        }

        pic_eoi(regs->int_no - 32);
    }
}
