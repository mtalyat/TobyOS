#include "interrupts/idt.hpp"
#include "display/terminal.hpp"
#include "display/framebuffer.hpp"
#include "kernel/kernel_context.hpp"
#include "kernel/asm.hpp"
#include "input/keyboard.hpp"

KernelContext *gp_kernel_context;

extern "C" void kernel_main()
{
    DISABLE_INTERRUPTS();

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

    // Main application loop
    int channel = 0;
    byte red = 0, green = 0, blue = 0;
    byte* selected_channel = &red;
    while (true)
    {
        DISABLE_INTERRUPTS();
        uint ticks = static_cast<uint>(context.time.ticks);
        ENABLE_INTERRUPTS();

        if (*selected_channel < 250)
        {
            *selected_channel += 5;
        }
        else
        {
            *selected_channel = 0;
            if (selected_channel == &red)
            {
                selected_channel = &green;
            }
            else if (selected_channel == &green)
            {
                selected_channel = &blue;
            }
            else
            {
                selected_channel = &red;
            }
        }
        framebuffer.fill((red << 16) | (green << 8) | blue);
        framebuffer.draw_line(0, 0, framebuffer.get_width() - 1, framebuffer.get_height() - 1, 0xFFFFFF);
        framebuffer.present();

        HALT();
    }
}
