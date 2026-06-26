#include "bitmap.hpp"

template <uint Size>
inline bit Bitmap<Size>::get(uint index) const
{
    return (m_bits[index / 8] >> (index % 8)) & 1;
}

template <uint Size>
inline void Bitmap<Size>::set(uint index, bit value)
{
    if (value)
    {
        m_bits[index / 8] |= (1 << (index % 8));
    }
    else
    {
        m_bits[index / 8] &= ~(1 << (index % 8));
    }
}
