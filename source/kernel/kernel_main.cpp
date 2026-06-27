#include "interrupts/idt.hpp"
#include "display/terminal.hpp"
#include "display/framebuffer.hpp"
#include "kernel/kernel_context.hpp"
#include "kernel/asm.hpp"
#include "input/keyboard.hpp"
#include "boot/boot_info.hpp"

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
    ENABLE_INTERRUPTS();

    // Higher-resolution framebuffer demo.
    Framebuffer framebuffer;
    framebuffer.init();
    
    framebuffer.fill(0x000000);
    framebuffer.draw_string("TobyOS Kernel", 0, 0, 0x00ff00);
    framebuffer.draw_string("Memory Regions: ", 0, 20, 0xffffff);
    framebuffer.draw_number(static_cast<wuint>(memory_region_count), 16 * Framebuffer::FONT_WIDTH, 20, 0xffffff);
    for (uint i = 0; i < memory_region_count && i < 64; ++i)
    {
        const MemoryRegion &region = boot_info->regions[i];
        framebuffer.draw_string("Region ", 0, 40 + i * 20, 0xffffff);
        framebuffer.draw_number(static_cast<wuint>(i), 7 * Framebuffer::FONT_WIDTH, 40 + i * 20, 0xffffff);
        framebuffer.draw_string(": Base = ", 12 * Framebuffer::FONT_WIDTH, 40 + i * 20, 0xffffff);
        framebuffer.draw_number(static_cast<wuint>(region.base), 22 * Framebuffer::FONT_WIDTH, 40 + i * 20, 0xffffff);
        framebuffer.draw_string(", Length = ", 42 * Framebuffer::FONT_WIDTH, 40 + i * 20, 0xffffff);
        framebuffer.draw_number(static_cast<wuint>(region.length), 54 * Framebuffer::FONT_WIDTH, 40 + i * 20, 0xffffff);
        framebuffer.draw_string(", Type = ", 74 * Framebuffer::FONT_WIDTH, 40 + i * 20, 0xffffff);
        framebuffer.draw_number(static_cast<wuint>(region.type), 84 * Framebuffer::FONT_WIDTH, 40 + i * 20, 0xffffff);
    }
    framebuffer.present();

    // Main application loop
    while (true)
    {
        HALT();
    }
}
