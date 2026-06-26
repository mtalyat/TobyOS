#include "interrupts/idt.hpp"
#include "display/terminal.hpp"
#include "kernel/kernel_context.hpp"
#include "kernel/asm.hpp"
#include "input/keyboard.hpp"

KernelContext *gp_kernel_context;

static void play_splashscreen()
{
    constexpr time SPLASH_DURATION = TICKS_PER_SECOND;
    constexpr TerminalColor SPLASH_FG = TerminalColor::Black;
    constexpr TerminalColor SPLASH_BG = TerminalColor::LightGreen;

    // draw the splash screen
    Terminal &terminal = gp_kernel_context->terminal;
    terminal.clear(SPLASH_FG, SPLASH_BG);
    terminal.write_at(" ____  ____  ___  _  _    ____  ____ ", 1, 1, SPLASH_FG, SPLASH_BG);
    terminal.write_at("|_  _|| __ || __\\| || |  | __ || ___|", 1, 2, SPLASH_FG, SPLASH_BG);
    terminal.write_at("  ||  ||__||||--< \\  /   ||__|||___ |", 1, 3, SPLASH_FG, SPLASH_BG);
    terminal.write_at("  ||  |____||___/  ||    |____||____|", 1, 4, SPLASH_FG, SPLASH_BG);

    // Wait for the splash duration
    time start_ticks = gp_kernel_context->time.ticks;
    time elapsed = 0;
    while (elapsed < SPLASH_DURATION)
    {
        elapsed = gp_kernel_context->time.ticks - start_ticks;
    }

    // clear back to normal
    terminal.clear();
}

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

    // Splash screen
    play_splashscreen();

    // Clear the terminal
    Terminal &terminal = gp_kernel_context->terminal;

    // Main application loop
    while (true)
    {
        // Race-free handoff: atomically snapshot and clear incoming events.
        DISABLE_INTERRUPTS();
        context.events = context.incomingEvents;
        context.incomingEvents.clear();
        ENABLE_INTERRUPTS();

        // TODO: run processes

        // TEMP: render tick count from an atomic snapshot.

        DISABLE_INTERRUPTS();
        time ticks = context.time.ticks;
        ENABLE_INTERRUPTS();
        terminal.write_at("Ticks:                ", 0, 0);
        terminal.write_at(static_cast<wuint>(ticks), 7, 0);

        terminal.write_at("Events:    ", 0, 1);
        terminal.move_cursor(8, 1);
        terminal.write(static_cast<wuint>(context.events.get_size()));

        if (context.events.is_empty())
        {
            terminal.write_at("No events", 0, 2);
        }
        else
        {
            uint y = 2;
            for (const Event& event : context.events)
            {
                // Write the event type
                switch (event.type)
                {
                case EventType::Error:
                    terminal.write_at("Error", 0, y);
                    break;
                case EventType::Timer: // skip
                    // terminal.write_at("Timer", 0, y);
                    y--;
                    break;
                case EventType::KeyDown:
                    terminal.write_at("Key Down: ", 0, y);
                    terminal.write(Keyboard::key_to_string(static_cast<Key>(event.data1)));
                    break;
                case EventType::KeyUp:
                    terminal.write_at("Key Up: ", 0, y);
                    terminal.write(Keyboard::key_to_string(static_cast<Key>(event.data1)));
                    break;
                case EventType::MouseMove:
                    terminal.write_at("Mouse Move", 0, y);
                    break;
                case EventType::MouseButtonDown:
                    terminal.write_at("Mouse Button Down", 0, y);
                    break;
                case EventType::MouseButtonUp:
                    terminal.write_at("Mouse Button Up", 0, y);
                    break;
                case EventType::MouseWheel:
                    terminal.write_at("Mouse Wheel", 0, y);
                    break;
                default:
                    terminal.write_at("Unknown Event", 0, y);
                    break;
                }

                ++y;
            }
        }

        // Pause and render to the screen
        HALT();

        // Sleep until an interrupt wakes us, but read queue state with interrupts masked.
        while (true)
        {
            DISABLE_INTERRUPTS();
            bool has_incoming = !context.incomingEvents.is_empty();
            ENABLE_INTERRUPTS();

            if (has_incoming)
            {
                break;
            }

            HALT();
        }
    }
}
