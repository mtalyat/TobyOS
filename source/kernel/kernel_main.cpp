#include "interrupts/idt.hpp"
#include "display/terminal.hpp"
#include "display/framebuffer.hpp"
#include "kernel/kernel_context.hpp"
#include "kernel/control.hpp"
#include "input/keyboard.hpp"
#include "boot/boot_info.hpp"
#include "io/serial.hpp"
#include "memory/PhysicalMemoryManager.hpp"

KernelContext *gp_kernel_context;
// static PhysicalMemoryManager g_physical_memory_manager;

extern "C" void kernel_main(BootInfo *boot_info)
{
    Control::disable_interrupts();

    if (boot_info == nullptr)
    {
        while (true)
        {
            Control::halt();
        }
    }

    const uint32 memory_region_count = boot_info->region_count;

    // Initialize context
    KernelContext context(*boot_info);
    gp_kernel_context = &context;

    // Initialize PIC, IDT, and PIT before enabling interrupts.
    pic_init();
    IDT::init();
    pit_init(100); // 100 Hz timer
    Control::enable_interrupts();

    Framebuffer& framebuffer = context.framebuffer;
    Serial& serial = context.serial;

    serial.write("TobyOS Kernel\n");
    serial.write("Creating PMM...\n");

    PhysicalMemoryManager pmm(*boot_info);

    serial.write("\n\nPMM Initialized.\n");

    serial.write("Total Pages: ");
    serial.write(pmm.get_total_page_count());
    serial.write(", Used Pages: ");
    serial.write(pmm.get_used_page_count());
    serial.write(", Free Pages: ");
    serial.write(pmm.get_free_page_count());
    serial.write("\n");

    // Main application loop
    while (true)
    {
        time ticks = context.time.ticks;

        framebuffer.fill(0x000000);
        framebuffer.draw_string("TobyOS Kernel", 0, 0, 0x00ff00);
        framebuffer.draw_string("Ticks: ", 0, 16, 0x00ff00);
        framebuffer.draw_number(ticks, Framebuffer::FONT_WIDTH * 7, 16, 0xffffff);
        framebuffer.present();

        Control::halt();
    }
}
