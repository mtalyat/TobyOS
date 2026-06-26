#pragma once

#include "input/key.hpp"
#include "types/types.hpp"
#include "types/bitmap.hpp"

struct KeyPress
{
    Key key;
    bool pressed;
};

class Keyboard
{
public:
    Keyboard()
    {
    }

    bit get_key(Key key) const
    {
        return m_keys.get(static_cast<int>(key));
    }

    void set_key(Key key, bit value)
    {
        // set new value
        m_keys.set(static_cast<int>(key), value);
    }

    static KeyPress parse_scancode(uint8 scancode);

    static char const* key_to_string(Key key);

private:
    Bitmap<static_cast<int>(Key::KeyCount)> m_keys;
};