#pragma once

#include "types/types.hpp"

using time = long long;

constexpr time TICKS_PER_SECOND = 100;

struct Time
{
    volatile time ticks = 0;
};
