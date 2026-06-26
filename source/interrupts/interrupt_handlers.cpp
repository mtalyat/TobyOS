#include "interrupt_handlers.hpp"
#include "kernel/kernel_context.hpp"
#include "kernel/portio.hpp"
#include "input/keyboard.hpp"

void interrupt_timer()
{
    // increase time
    gp_kernel_context->time.ticks++;

    // add a timer event to the incoming events queue
    gp_kernel_context->incomingEvents.add({EventType::Timer, 0, 0});
}

void interrupt_keyboard()
{
    // get the scancode from the keyboard controller (I/O port 0x60)
    uint8 scancode = get_port(0x60);

    // parse the scancode into a KeyPress event
    KeyPress keyPress = Keyboard::parse_scancode(scancode);

    // if the key is valid, add it to the incoming events queue
    if (keyPress.key != Key::None)
    {
        gp_kernel_context->incomingEvents.add({keyPress.pressed ? EventType::KeyDown : EventType::KeyUp, static_cast<int>(keyPress.key), 0});
    }    
}
