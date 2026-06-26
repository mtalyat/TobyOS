#pragma once

#include "types/types.hpp"

static inline void set_port(uint16 port, uint8 value)
{
    asm volatile(
        "outb %0, %1"
        :
        : "a"(value), "Nd"(port));
}

static inline uint8 get_port(uint16 port)
{
    uint8 ret;

    asm volatile(
        "inb %1, %0"
        : "=a"(ret)
        : "Nd"(port));

    return ret;
}