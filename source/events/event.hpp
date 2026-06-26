#pragma once

#include "types/types.hpp"

enum class EventType
{
    None,

    // error events
    Error, // data1 = error code

    // timer events
    Timer, // data1 = timer ID

    // input events
    KeyDown, // data1 = key code
    KeyUp, // data1 = key code
    MouseMove, // data1 = x, data2 = y
    MouseButtonDown, // data1 = button code
    MouseButtonUp, // data1 = button code
    MouseWheel, // data1 = wheel delta y, data2 = wheel delta x
};

struct Event
{
    EventType type;
    int data1;
    int data2;
};