#include "keyboard.hpp"

static const int SCANCODE_TO_KEY_MAP_SIZE = 0x59;
static const Key SCANCODE_TO_KEY_MAP[SCANCODE_TO_KEY_MAP_SIZE] = {
    Key::None,          // 0x00
    Key::Escape,        // 0x01
    Key::D1,            // 0x02
    Key::D2,            // 0x03
    Key::D3,            // 0x04
    Key::D4,            // 0x05
    Key::D5,            // 0x06
    Key::D6,            // 0x07
    Key::D7,            // 0x08
    Key::D8,            // 0x09
    Key::D9,            // 0x0A
    Key::D0,            // 0x0B
    Key::Minus,         // 0x0C
    Key::Equal,         // 0x0D
    Key::Backspace,     // 0x0E
    Key::Tab,           // 0x0F
    Key::Q,             // 0x10
    Key::W,             // 0x11
    Key::E,             // 0x12
    Key::R,             // 0x13
    Key::T,             // 0x14
    Key::Y,             // 0x15
    Key::U,             // 0x16
    Key::I,             // 0x17
    Key::O,             // 0x18
    Key::P,             // 0x19
    Key::LeftBracket,   // 0x1A
    Key::RightBracket,  // 0x1B
    Key::Enter,         // 0x1C
    Key::LeftControl,   // 0x1D
    Key::A,             // 0x1E
    Key::S,             // 0x1F
    Key::D,             // 0x20
    Key::F,             // 0x21
    Key::G,             // 0x22
    Key::H,             // 0x23
    Key::J,             // 0x24
    Key::K,             // 0x25
    Key::L,             // 0x26
    Key::Semicolon,     // 0x27
    Key::Apostrophe,    // 0x28
    Key::Grave,         // 0x29
    Key::LeftShift,     // 0x2A
    Key::Backslash,     // 0x2B
    Key::Z,             // 0x2C
    Key::X,             // 0x2D
    Key::C,             // 0x2E
    Key::V,             // 0x2F
    Key::B,             // 0x30
    Key::N,             // 0x31
    Key::M,             // 0x32
    Key::Comma,         // 0x33
    Key::Period,        // 0x34
    Key::Slash,         // 0x35
    Key::RightShift,    // 0x36
    Key::KeypadMultiply, // 0x37
    Key::LeftAlt,       // 0x38
    Key::Space,         // 0x39
    Key::CapsLock,      // 0x3A
    Key::F1,            // 0x3B
    Key::F2,            // 0x3C
    Key::F3,            // 0x3D
    Key::F4,            // 0x3E
    Key::F5,            // 0x3F
    Key::F6,            // 0x40
    Key::F7,            // 0x41
    Key::F8,            // 0x42
    Key::F9,            // 0x43
    Key::F10,           // 0x44
    Key::NumLock,       // 0x45
    Key::ScrollLock,    // 0x46
    Key::Keypad7,       // 0x47
    Key::Keypad8,       // 0x48
    Key::Keypad9,       // 0x49
    Key::KeypadMinus,   // 0x4A
    Key::Keypad4,       // 0x4B
    Key::Keypad5,       // 0x4C
    Key::Keypad6,       // 0x4D
    Key::KeypadPlus,    // 0x4E
    Key::Keypad1,       // 0x4F
    Key::Keypad2,       // 0x50
    Key::Keypad3,       // 0x51
    Key::Keypad0,       // 0x52
    Key::KeypadDecimal, // 0x53
    Key::None,          // 0x54
    Key::None,          // 0x55
    Key::None,          // 0x56
    Key::F11,           // 0x57
    Key::F12,           // 0x58
};
static const int SCANCODE_TO_KEY_MAP_EXTENDED_SIZE = 0x6E;
static const Key SCANCODE_TO_KEY_MAP_EXTENDED[SCANCODE_TO_KEY_MAP_EXTENDED_SIZE] = {
    Key::None,          // 0x00
    Key::None,          // 0x01
    Key::None,          // 0x02
    Key::None,          // 0x03
    Key::None,          // 0x04
    Key::None,          // 0x05
    Key::None,          // 0x06
    Key::None,          // 0x07
    Key::None,          // 0x08
    Key::None,          // 0x09
    Key::None,          // 0x0A
    Key::None,          // 0x0B
    Key::None,          // 0x0C
    Key::None,          // 0x0D
    Key::None,          // 0x0E
    Key::None,          // 0x0F
    Key::MultimediaPrevious,          // 0x10
    Key::None,          // 0x11
    Key::None,          // 0x12
    Key::None,          // 0x13
    Key::None,          // 0x14
    Key::None,          // 0x15
    Key::None,          // 0x16
    Key::None,          // 0x17
    Key::None,          // 0x18
    Key::MultimediaNext,          // 0x19
    Key::None,          // 0x1A
    Key::None,          // 0x1B
    Key::KeypadEnter,          // 0x1C
    Key::RightControl,          // 0x1D
    Key::None,          // 0x1E
    Key::None,          // 0x1F
    Key::MultimediaMute,          // 0x20
    Key::MultimediaCalculator,          // 0x21
    Key::MultimediaPlay,          // 0x22
    Key::None,          // 0x23
    Key::MultimediaStop,          // 0x24
    Key::None,          // 0x25
    Key::None,          // 0x26
    Key::None,          // 0x27
    Key::None,          // 0x28
    Key::None,          // 0x29
    Key::None,          // 0x2A
    Key::None,          // 0x2B
    Key::None,          // 0x2C
    Key::None,          // 0x2D
    Key::MultimediaVolumeDown,          // 0x2E
    Key::None,          // 0x2F
    Key::MultimediaVolumeUp,          // 0x30
    Key::None,          // 0x31
    Key::MultimediaWwwHome,          // 0x32
    Key::None,          // 0x33
    Key::None,          // 0x34
    Key::KeypadDivide,          // 0x35
    Key::None,          // 0x36
    Key::None,          // 0x37
    Key::RightAlt,          // 0x38
    Key::None,          // 0x39
    Key::None,          // 0x39
    Key::None,          // 0x39
    Key::None,          // 0x39
    Key::None,          // 0x39
    Key::None,          // 0x39
    Key::None,          // 0x39
    Key::None,          // 0x39
    Key::None,          // 0x39
    Key::None,          // 0x39
    Key::None,          // 0x39
    Key::None,          // 0x39
    Key::None,          // 0x39
    Key::None,          // 0x39
    Key::Home,          // 0x47
    Key::Up,          // 0x48
    Key::PageUp,          // 0x49
    Key::None,          // 0x4A
    Key::Left,          // 0x4B
    Key::None,          // 0x4C
    Key::Right,          // 0x4D
    Key::None,          // 0x4E
    Key::End,          // 0x4F
    Key::Down,          // 0x50
    Key::PageDown,          // 0x51
    Key::Insert,          // 0x52
    Key::Delete,          // 0x53
    Key::None,          // 0x54
    Key::None,          // 0x54
    Key::None,          // 0x54
    Key::None,          // 0x54
    Key::None,          // 0x54
    Key::None,          // 0x54
    Key::None,          // 0x54
    Key::LeftSuper,          // 0x5B
    Key::RightSuper,          // 0x5C
    Key::Apps,          // 0x5D
    Key::AcpiPower,          // 0x5E
    Key::AcpiSleep,          // 0x5F
    Key::None,          // 0x60
    Key::None,          // 0x60
    Key::None,          // 0x60
    Key::AcpiWake,          // 0x63
    Key::None,          // 0x64
    Key::MultimediaWwwSearch,          // 0x65
    Key::MultimediaWwwFavorites,          // 0x66
    Key::MultimediaWwwRefresh,          // 0x67
    Key::MultimediaWwwStop,          // 0x68
    Key::MultimediaWwwForward,          // 0x69
    Key::MultimediaWwwBack,          // 0x6A
    Key::MultimediaMyComputer,          // 0x6B
    Key::MultimediaEmail,          // 0x6C
    Key::MultimediaMediaSelect,          // 0x6D
};
static const char* const KEY_TO_STRING_MAP[static_cast<int>(Key::KeyCount)] = {
    "None",
    "A",
    "B",
    "C",
    "D",
    "E",
    "F",
    "G",
    "H",
    "I",
    "J",
    "K",
    "L",
    "M",
    "N",
    "O",
    "P",
    "Q",
    "R",
    "S",
    "T",
    "U",
    "V",
    "W",
    "X",
    "Y",
    "Z",
    "D1",
    "D2",
    "D3",
    "D4",
    "D5",
    "D6",
    "D7",
    "D8",
    "D9",
    "D0",
    "F1",
    "F2",
    "F3",
    "F4",
    "F5",
    "F6",
    "F7",
    "F8",
    "F9",
    "F10",
    "F11",
    "F12",
    "Escape",
    "Grave",
    "Minus",
    "Equal",
    "Backspace",
    "Tab",
    "LeftBracket",
    "RightBracket",
    "Backslash",
    "Semicolon",
    "Apostrophe",
    "Comma",
    "Period",
    "Slash",
    "Space",
    "Enter",
    "LeftShift",
    "RightShift",
    "LeftControl",
    "RightControl",
    "LeftAlt",
    "RightAlt",
    "LeftSuper",
    "RightSuper",
    "CapsLock",
    "NumLock",
    "ScrollLock",
    "Insert",
    "Delete",
    "Home",
    "End",
    "PageUp",
    "PageDown",
    "Up",
    "Down",
    "Left",
    "Right",
    "PrintScreen",
    "Pause",
    "Apps",
    "MultimediaNext",
    "MultimediaPrevious",
    "MultimediaMute",
    "MultimediaPlay",
    "MultimediaStop",
    "MultimediaVolumeUp",
    "MultimediaVolumeDown",
    "MultimediaCalculator",
    "MultimediaMyComputer",
    "MultimediaEmail",
    "MultimediaMediaSelect",
    "MultimediaWwwHome",
    "MultimediaWwwSearch",
    "MultimediaWwwFavorites",
    "MultimediaWwwRefresh",
    "MultimediaWwwStop",
    "MultimediaWwwForward",
    "MultimediaWwwBack",
    "Keypad1",
    "Keypad2",
    "Keypad3",
    "Keypad4",
    "Keypad5",
    "Keypad6",
    "Keypad7",
    "Keypad8",
    "Keypad9",
    "Keypad0",
    "KeypadEnter",
    "KeypadPlus",
    "KeypadMinus",
    "KeypadMultiply",
    "KeypadDivide",
    "KeypadDecimal",
    "KeypadEqual",
    "AcpiPower",
    "AcpiSleep",
    "AcpiWake",
};

KeyPress Keyboard::parse_scancode(uint8 scancode)
{
    static bool extended = false;

    // Handle extended prefix first.
    if (scancode == 0xE0)
    {
        extended = true;
        return {Key::None, false};
    }

    KeyPress event;
    event.pressed = (scancode < 0x80); // Make code (< 0x80) means key down.

    // Normalize break codes by stripping bit 7.
    uint8 keycode = event.pressed ? scancode : static_cast<uint8>(scancode - 0x80);

    // If extended from previous scancode, handle the extended key
    if (extended)
    {
        if (keycode < SCANCODE_TO_KEY_MAP_EXTENDED_SIZE)
        {
            event.key = SCANCODE_TO_KEY_MAP_EXTENDED[keycode];
        }
        else
        {
            event.key = Key::None; // Unknown key
        }
        extended = false; // Reset extended flag after processing
    }
    else
    {
        // Handle non-extended scancodes
        if (keycode < SCANCODE_TO_KEY_MAP_SIZE)
        {
            event.key = SCANCODE_TO_KEY_MAP[keycode];
        }
        else
        {
            event.key = Key::None; // Unknown key
        }
    }

    return event;
}

char const *Keyboard::key_to_string(Key key)
{
    if (static_cast<int>(key) < static_cast<int>(Key::KeyCount))
    {
        return KEY_TO_STRING_MAP[static_cast<int>(key)];
    }
    else
    {
        return "Unknown";
    }
}
