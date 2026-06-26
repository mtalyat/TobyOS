#pragma once

#include "types/types.hpp"


char* const VGA_START = reinterpret_cast<char*>(0xB8000);

enum class TerminalColor : unsigned char
{
    Black = 0,
    Blue = 1,
    Green = 2,
    Cyan = 3,
    Red = 4,
    Magenta = 5,
    Brown = 6,
    LightGray = 7,
    DarkGray = 8,
    LightBlue = 9,
    LightGreen = 10,
    LightCyan = 11,
    LightRed = 12,
    LightMagenta = 13,
    Yellow = 14,
    White = 15
};

class Terminal
{
    public:
    Terminal()
        : mp_vga(VGA_START)
        , m_width(DEFAULT_WIDTH)
        , m_height(DEFAULT_HEIGHT)
    {
    }

    Terminal(uint width, uint height)
        : mp_vga(VGA_START)
        , m_width(width)
        , m_height(height)
    {
    }

    uint get_width() const { return m_width; }

    uint get_height() const { return m_height; }

    void move_cursor(uint x, uint y)
    {
        mp_vga = VGA_START + (y * m_width + x) * 2;
    }

    void write(char c, TerminalColor fg = TerminalColor::White, TerminalColor bg = TerminalColor::Black)
    {
        *mp_vga++ = c;
        *mp_vga++ = create_color_code(fg, bg);
    }

    void write(const char* str, TerminalColor fg = TerminalColor::White, TerminalColor bg = TerminalColor::Black)
    {
        while (*str)
        {
            write(*str++, fg, bg);
        }
    }

    void write(wint value, TerminalColor fg = TerminalColor::White, TerminalColor bg = TerminalColor::Black)
    {
        // Handle negative values
        if (value < 0)
        {
            write('-', fg, bg);
            value = -value;
        }

        // Write as if it were an unsigned integer
        wuint uvalue = static_cast<wuint>(value);
        write(uvalue, fg, bg);
    }

    void write(wuint value, TerminalColor fg = TerminalColor::White, TerminalColor bg = TerminalColor::Black)
    {
        char buffer[32];
        int length = 0;

        if (value == 0)
        {
            buffer[length++] = '0';
        }
        else
        {
            while (value > 0 && length < sizeof(buffer))
            {
                buffer[length++] = '0' + (value % 10);
                value /= 10;
            }
        }

        for (int i = length - 1; i >= 0; --i)
        {
            write(buffer[i], fg, bg);
        }
    }

    void write_at(const char* str, uint x, uint y, TerminalColor fg = TerminalColor::White, TerminalColor bg = TerminalColor::Black)
    {
        move_cursor(x, y);
        write(str, fg, bg);
    }

    void write_at(char c, uint x, uint y, TerminalColor fg = TerminalColor::White, TerminalColor bg = TerminalColor::Black)
    {
        move_cursor(x, y);
        write(c, fg, bg);
    }

    void write_at(wint value, uint x, uint y, TerminalColor fg = TerminalColor::White, TerminalColor bg = TerminalColor::Black)
    {
        move_cursor(x, y);
        write(value, fg, bg);
    }

    void write_at(wuint value, uint x, uint y, TerminalColor fg = TerminalColor::White, TerminalColor bg = TerminalColor::Black)
    {
        move_cursor(x, y);
        write(value, fg, bg);
    }

    void clear(TerminalColor fg = TerminalColor::White, TerminalColor bg = TerminalColor::Black)
    {
        move_cursor(0, 0);
        for (uint y = 0; y < m_height; ++y)
        {
            for (uint x = 0; x < m_width; ++x)
            {
                write(' ', fg, bg);
            }
        }
        move_cursor(0, 0);
    }

    private:
    static char create_color_code(TerminalColor fg, TerminalColor bg)
    {
        return (static_cast<unsigned char>(bg) << 4) | static_cast<unsigned char>(fg);
    }

    static constexpr int DEFAULT_WIDTH = 80;
    static constexpr int DEFAULT_HEIGHT = 25;

    volatile char* mp_vga;
    uint m_width;
    uint m_height;
};