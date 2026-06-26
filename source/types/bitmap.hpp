#pragma once

#include "types/types.hpp"

template<uint Size>
class Bitmap
{
    public:
    Bitmap()
        : m_bits{0}
    {
    }

    bit get(uint index) const;

    void set(uint index, bit value);

    private:
    uint8 m_bits[(Size + 7) / 8];
};