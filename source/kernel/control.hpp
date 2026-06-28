#pragma once

#include "types/types.hpp"
#include "core/asm.hpp"

class Control
{
    public:
    Control() = delete;
    ~Control() = delete;

    inline static void halt();

    inline static void enable_interrupts();
    inline static void disable_interrupts();

    inline static void enable_protected_mode();
    inline static void disable_protected_mode();

    inline static void set_page_directory(void* page_directory);
    inline static void enable_paging();
    inline static void disable_paging();
};

#include "control.inl"