#pragma once

#include "events/event.hpp"
#include "types/types.hpp"
#include "types/vector.hpp"
#include "time/time.hpp"
#include "io/serial.hpp"
#include "display/framebuffer.hpp"

struct KernelContext
{
    // time
    Time time;

    // events
    Vector<Event, 128> events;
    Vector<Event, 128> incomingEvents;

    // display
    Framebuffer framebuffer;

    // io
    Serial serial;

    KernelContext(const BootInfo &boot_info)
        : time(),
        events(),
        incomingEvents(),
        framebuffer(boot_info),
        serial()
    {
    }
};

extern KernelContext* gp_kernel_context;