#include "interrupts/idt.hpp"
#include "display/terminal.hpp"
#include "display/framebuffer.hpp"
#include "kernel/kernel_context.hpp"
#include "core/asm.hpp"
#include "input/keyboard.hpp"
#include "boot/boot_info.hpp"
#include "io/serial.hpp"

KernelContext *gp_kernel_context;
// static PhysicalMemoryManager g_physical_memory_manager;

extern "C" void kernel_main(BootInfo* boot_info)
{
    DISABLE_INTERRUPTS();

    if (boot_info == nullptr)
    {
        while (true)
        {
            HALT();
        }
    }

    const uint32 memory_region_count = boot_info->region_count;

    // Initialize context
    KernelContext context;
    gp_kernel_context = &context;

    // Initialize PIC, IDT, and PIT before enabling interrupts.
    pic_init();
    IDT::init();
    pit_init(100); // 100 Hz timer
    Serial serial;

    ENABLE_INTERRUPTS();

    // Higher-resolution framebuffer demo.
    Framebuffer framebuffer;
    framebuffer.init();
    
    framebuffer.fill(0x000000);
    framebuffer.draw_string("TobyOS Kernel", 0, 0, 0x00ff00);
    framebuffer.present();

    serial.write("TobyOS Kernel\n");
    serial.write("Memory Regions: ");
    serial.write(static_cast<wuint>(memory_region_count));
    for (uint i = 0; i < memory_region_count && i < 64; ++i)
    {
        const MemoryRegion &region = boot_info->regions[i];
        serial.write("Region ");
        serial.write(static_cast<wuint>(i));
        serial.write(": Base = ");
        serial.write(static_cast<wuint>(region.base));
        serial.write(", Length = ");
        serial.write(static_cast<wuint>(region.length));
        serial.write(", Type = ");
        serial.write(static_cast<wuint>(region.type));
        serial.write("\n");
    }

    // Main application loop
    while (true)
    {
        HALT();
    }
}
