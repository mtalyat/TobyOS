#pragma once

#include "types/types.hpp"
#include "io/port.hpp"

constexpr uint16 COM1_PORT = 0x3F8; // COM1 base port address

class Serial
{
public:
    Serial()
    {
        static bool initialized = false;
        if (!initialized)
        {
            set_port(COM1_PORT + 1, 0x00); // Disable interrupts
            set_port(COM1_PORT + 3, 0x80); // Enable DLAB
            set_port(COM1_PORT + 0, 0x03); // Divisor low  (38400 baud)
            set_port(COM1_PORT + 1, 0x00); // Divisor high
            set_port(COM1_PORT + 3, 0x03); // 8 bits, no parity, one stop bit
            set_port(COM1_PORT + 2, 0xC7); // Enable FIFO, clear, 14-byte threshold
            set_port(COM1_PORT + 4, 0x0B); // IRQs enabled, RTS/DSR set
            initialized = true;
        }
    }

    void write(char c)
    {
        while ((get_port(COM1_PORT + 5) & 0x20) == 0)
        {
        }
        set_port(COM1_PORT + 0, static_cast<uint8>(c));
    }

    void write(const char *s)
    {
        while (*s)
        {
            if (*s == '\n')
                write('\r');
            write(*s++);
        }
    }

    void write(const char *s, uint length)
    {
        for (uint i = 0; i < length; ++i)
        {
            if (s[i] == '\n')
                write('\r');
            write(s[i]);
        }
    }

    void write(wuint number)
    {
        char buffer[32];
        int index = 0;

        if (number == 0)
        {
            write('0');
            return;
        }

        while (number > 0 && index < sizeof(buffer) - 1)
        {
            buffer[index++] = '0' + (number % 10);
            number /= 10;
        }

        for (int i = index - 1; i >= 0; --i)
        {
            write(buffer[i]);
        }
    }

    void write(wint number)
    {
        if (number < 0)
        {
            write('-');
            number = -number;
        }
        write(static_cast<wuint>(number));
    }
};
