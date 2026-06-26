#pragma once

#include "types/types.hpp"

template<typename T, uint Size>
class Queue
{
    public:
    Queue()
        : m_head(0)
        , m_tail(0)
        , m_count(0)
    {
    }

    bool add(const T& item)
    {
        if (m_count >= Size)
        {
            return false; // Queue is full
        }

        m_items[m_tail] = item;
        m_tail = (m_tail + 1) % Size;
        ++m_count;
        return true;
    }

    bool remove(T& item)
    {
        if (m_count == 0)
        {
            return false; // Queue is empty
        }

        item = m_items[m_head];
        m_head = (m_head + 1) % Size;
        --m_count;
        return true;
    }

    bool is_empty() const
    {
        return m_count == 0;
    }

    bool is_full() const
    {
        return m_count >= Size;
    }

    uint get_size() const
    {
        return m_count;
    }

    private:
    T m_items[Size];
    uint m_head;
    uint m_tail;
    uint m_count;
};