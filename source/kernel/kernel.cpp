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

    Terminal(int width, int height)
        : mp_vga(VGA_START)
        , m_width(width)
        , m_height(height)
    {
    }

    void move_cursor(int x, int y)
    {
        mp_vga = VGA_START + (y * m_width + x) * 2;
    }

    void write_char(char c, TerminalColor fg = TerminalColor::White, TerminalColor bg = TerminalColor::Black)
    {
        *mp_vga++ = c;
        *mp_vga++ = create_color_code(fg, bg);
    }

    void write(const char* str, TerminalColor fg = TerminalColor::White, TerminalColor bg = TerminalColor::Black)
    {
        while (*str)
        {
            write_char(*str++, fg, bg);
        }
    }

    private:
    static char create_color_code(TerminalColor fg, TerminalColor bg)
    {
        return (static_cast<unsigned char>(bg) << 4) | static_cast<unsigned char>(fg);
    }

    static constexpr int DEFAULT_WIDTH = 80;
    static constexpr int DEFAULT_HEIGHT = 25;

    char* mp_vga;
    int m_width;
    int m_height;
};

extern "C" void kernel_main()
{
    Terminal terminal;
    terminal.write("Hello, World!", TerminalColor::LightGreen, TerminalColor::Black);

    while (true)
    {
    }
}