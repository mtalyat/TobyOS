#pragma once

#include "types/types.hpp"

struct BootInfo
{
    uint32 framebuffer_address;
    uint16 width;
    uint16 height;
    uint16 pitch;
    uint8 bits_per_pixel;
    uint8 red_mask_size;
    uint8 red_shift;
    uint8 green_mask_size;
    uint8 green_shift;
    uint8 blue_mask_size;
    uint8 blue_shift;
    uint8 reserved;
};

static constexpr uint32 BOOT_INFO_ADDRESS = 0x0500;
