#pragma once

#include "boot/boot_info.hpp"
#include "types/types.hpp"
#include "core/math.hpp"
#include "display/font.hpp"

class Framebuffer
{
public:
    static constexpr uint8 BACKGROUND_COLOR = 0;
    static constexpr uint MAX_WIDTH = 1024;
    static constexpr uint MAX_HEIGHT = 768;
    static constexpr uint MAX_PIXELS = MAX_WIDTH * MAX_HEIGHT;
    static constexpr uint FONT_WIDTH = 8;
    static constexpr uint FONT_HEIGHT = 8;

    Framebuffer(const BootInfo &boot_info)
        : mp_pixels(reinterpret_cast<volatile uint8 *>(BOOT_INFO_ADDRESS)), m_width(0), m_height(0), m_pitch(0), m_bytes_per_pixel(0), m_bits_per_pixel(0), m_red_mask_size(0), m_red_shift(0), m_green_mask_size(0), m_green_shift(0), m_blue_mask_size(0), m_blue_shift(0)
    {
        init(boot_info);
    }

    uint get_width() const
    {
        return m_width;
    }

    uint get_height() const
    {
        return m_height;
    }

    void clear(uint8 color = BACKGROUND_COLOR)
    {
        for (uint y = 0; y < m_height; ++y)
        {
            for (uint x = 0; x < m_width; ++x)
            {
                m_backbuffer[y * m_width + x] = color;
            }
        }
    }

    void set_pixel(uint x, uint y, uint32 color)
    {
        if (x >= m_width || y >= m_height || m_bytes_per_pixel == 0)
        {
            return;
        }

        m_backbuffer[y * m_width + x] = color;
    }

    uint32 get_pixel(uint x, uint y) const
    {
        if (x >= m_width || y >= m_height)
        {
            return 0;
        }

        return m_backbuffer[y * m_width + x];
    }

    void fill(uint32 color)
    {
        for (uint i = 0; i < m_width * m_height; ++i)
        {
            m_backbuffer[i] = color;
        }
    }

    void draw_pixel(uint x, uint y, uint32 color)
    {
        set_pixel(x, y, color);
    }

    void draw_char(char character, uint x, uint y, uint32 color)
    {
        const uint8 *glyph = glyph_for(character);
        for (uint row = 0; row < FONT_HEIGHT; ++row)
        {
            uint8 bits = glyph[row];
            for (uint column = 0; column < FONT_WIDTH; ++column)
            {
                // discard the pixel if it is empty
                if ((bits & (1 << (7 - column))) == 0)
                {
                    continue;
                }
                draw_pixel(x + column, y + row, color);
            }
        }
    }

    void draw_string(const char *text, uint x, uint y, uint32 color)
    {
        if (text == nullptr)
        {
            return;
        }

        uint cursor_x = x;
        uint cursor_y = y;

        while (*text)
        {
            char character = *text++;
            if (character == '\n')
            {
                cursor_x = x;
                cursor_y += FONT_HEIGHT;
                continue;
            }

            draw_char(character, cursor_x, cursor_y, color);
            cursor_x += FONT_WIDTH;
        }
    }

    void draw_number(wint number, uint x, uint y, uint32 color)
    {
        // if negative, draw a minus sign and convert to positive
        if (number < 0)
        {
            draw_char('-', x, y, color);
            number = -number;
            x += FONT_WIDTH;
        }

        // draw the number an unsigned integer
        draw_number(static_cast<wuint>(number), x, y, color);
    }

    void draw_number(wuint number, uint x, uint y, uint32 color)
    {
        char buffer[32];
        int index = 0;

        if (number == 0)
        {
            buffer[index++] = '0';
        }
        else
        {
            while (number > 0 && index < sizeof(buffer) - 1)
            {
                buffer[index++] = '0' + (number % 10);
                number /= 10;
            }
        }

        // Reverse the string
        for (int i = 0; i < index / 2; ++i)
        {
            char temp = buffer[i];
            buffer[i] = buffer[index - i - 1];
            buffer[index - i - 1] = temp;
        }
        buffer[index] = '\0';

        draw_string(buffer, x, y, color);
    }

    void draw_line(uint x0, uint y0, uint x1, uint y1, uint32 color)
    {
        int dx = Math::abs(x1 - x0);
        int dy = Math::abs(y1 - y0);
        int sx = (x0 < x1) ? 1 : -1;
        int sy = (y0 < y1) ? 1 : -1;
        int err = dx - dy;

        while (true)
        {
            draw_pixel(x0, y0, color);

            if (x0 == x1 && y0 == y1)
                break;

            int err2 = err * 2;
            if (err2 > -dy)
            {
                err -= dy;
                x0 += sx;
            }
            if (err2 < dx)
            {
                err += dx;
                y0 += sy;
            }
        }
    }

    void present()
    {
        if (m_bytes_per_pixel == 0)
        {
            return;
        }

        for (uint y = 0; y < m_height; ++y)
        {
            for (uint x = 0; x < m_width; ++x)
            {
                uint32 color = pack_color(m_backbuffer[y * m_width + x]);
                volatile uint8 *pixel = mp_pixels + (y * m_pitch) + (x * m_bytes_per_pixel);
                for (uint8 i = 0; i < m_bytes_per_pixel; ++i)
                {
                    pixel[i] = static_cast<uint8>(color >> (i * 8));
                }
            }
        }
    }

private:
    void init(const BootInfo &info)
    {
        mp_pixels = reinterpret_cast<volatile uint8 *>(static_cast<uint32>(info.framebuffer_address));
        m_width = info.width;
        m_height = info.height;
        m_pitch = info.pitch;
        m_bits_per_pixel = info.bits_per_pixel;
        m_bytes_per_pixel = static_cast<uint8>((m_bits_per_pixel + 7) / 8);
        m_red_mask_size = info.red_mask_size;
        m_red_shift = info.red_shift;
        m_green_mask_size = info.green_mask_size;
        m_green_shift = info.green_shift;
        m_blue_mask_size = info.blue_mask_size;
        m_blue_shift = info.blue_shift;
        clear(0);
    }

    static const uint8 *glyph_for(char character)
    {
        const uint8 index = static_cast<uint8>(character);
        if (index >= 128)
        {
            return FONT[0]; // Return a default glyph for unsuported characters
        }

        return FONT[index];
    }

    static uint32 scale_channel(uint8 value, uint8 bits)
    {
        if (bits == 0)
        {
            return 0;
        }

        uint32 max_value = (1u << bits) - 1u;
        return (static_cast<uint32>(value) * max_value + 127u) / 255u;
    }

    uint32 pack_color(uint32 rgb) const
    {
        uint8 r = static_cast<uint8>((rgb >> 16) & 0xFF);
        uint8 g = static_cast<uint8>((rgb >> 8) & 0xFF);
        uint8 b = static_cast<uint8>(rgb & 0xFF);

        if (m_bits_per_pixel == 8)
        {
            return static_cast<uint32>((static_cast<uint16>(r) + g + b) / 3);
        }

        if (m_red_mask_size == 0 && m_green_mask_size == 0 && m_blue_mask_size == 0)
        {
            return rgb;
        }

        uint32 packed = 0;
        packed |= scale_channel(r, m_red_mask_size) << m_red_shift;
        packed |= scale_channel(g, m_green_mask_size) << m_green_shift;
        packed |= scale_channel(b, m_blue_mask_size) << m_blue_shift;
        return packed;
    }

    volatile uint8 *mp_pixels;
    uint m_width;
    uint m_height;
    uint m_pitch;
    uint8 m_bytes_per_pixel;
    uint8 m_bits_per_pixel;
    uint8 m_red_mask_size;
    uint8 m_red_shift;
    uint8 m_green_mask_size;
    uint8 m_green_shift;
    uint8 m_blue_mask_size;
    uint8 m_blue_shift;
    uint32 m_backbuffer[MAX_PIXELS];
};