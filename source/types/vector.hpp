#pragma once

#include "types/types.hpp"

template<typename T, uint Size>
class Vector
{
    public:
    using Iterator = T*;
    using ConstIterator = const T*;

    Vector()
        : m_size(0)
    {
    }

    bool add(const T& item)
    {
        if (m_size >= Size)
        {
            return false; // Vector is full
        }

        m_data[m_size++] = item;
        return true;
    }

    bool remove(const T& item)
    {
        for (uint i = 0; i < m_size; ++i)
        {
            if (m_data[i] == item)
            {
                for (uint j = i + 1; j < m_size; ++j)
                {
                    m_data[j - 1] = m_data[j];
                }

                --m_size;
                return true;
            }
        }

        return false; // Item not found
    }

    void clear()
    {
        m_size = 0;
    }

    bool is_empty() const
    {
        return m_size == 0;
    }

    uint get_size() const
    {
        return m_size;
    }

    uint get_capacity() const
    {
        return Size;
    }

    T& operator[](uint index)
    {
        return m_data[index];
    }

    const T& operator[](uint index) const
    {
        return m_data[index];
    }

    Iterator begin()
    {
        return m_data;
    }

    Iterator end()
    {
        return m_data + m_size;
    }

    ConstIterator begin() const
    {
        return m_data;
    }

    ConstIterator end() const
    {
        return m_data + m_size;
    }

    ConstIterator cbegin() const
    {
        return m_data;
    }

    ConstIterator cend() const
    {
        return m_data + m_size;
    }

    private:
    T m_data[Size];
    uint m_size;
};