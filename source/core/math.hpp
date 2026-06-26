#pragma once

#include "types/types.hpp"

namespace Math
{
    template<typename T>
    inline T abs(T value)
    {
        return (value < 0) ? -value : value;
    }
}