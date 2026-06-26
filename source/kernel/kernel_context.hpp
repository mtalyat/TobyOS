#pragma once

#include "display/terminal.hpp"
#include "events/event.hpp"
#include "types/types.hpp"
#include "types/vector.hpp"
#include "time/time.hpp"

struct KernelContext
{
    // time
    Time time = {};

    // events
    Vector<Event, 128> events = {};
    Vector<Event, 128> incomingEvents = {};

    // temp
    Terminal terminal = {};
};

extern KernelContext* gp_kernel_context;