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

    void fill(uint8 value = 0)
    {
        for (uint i = 0; i < sizeof(m_bits); ++i)
        {
            m_bits[i] = value;
        }
    }

    inline void clear()
    {
        fill(0);
    }

    private:
    uint8 m_bits[(Size + 7) / 8];
};

#include "types/bitmap.inl"