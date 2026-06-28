#include "control.hpp"

enum class CR0Flags : uint32
{
    ProtectionEnable = 1 << 0, // Protection Enable
    MonitorCoprocessor = 1 << 1, // Monitor Coprocessor
    Emulation = 1 << 2, // Emulation
    TaskSwitched = 1 << 3, // Task Switched
    ExtensionType = 1 << 4, // Extension Type
    NumericError = 1 << 5, // Numeric Error
    WriteProtect = 1 << 16, // Write Protect
    AlignmentMask = 1 << 18, // Alignment Mask
    NotWriteThrough = 1 << 29, // Not Write-through
    CacheDisable = 1 << 30, // Cache Disable
    Paging = 1u << 31u, // Paging
};

enum class CR4Flags : uint32
{
    Virtual8086ModeExtensions = 1 << 0, // Virtual-8086 Mode Extensions
    ProtectedModeVirtualInterrupts = 1 << 1, // Protected-Mode Virtual Interrupts
    TimeStampDisable = 1 << 2, // Time Stamp Disable
    DebuggingExtensions = 1 << 3, // Debugging Extensions
    PageSizeExtensions = 1 << 4, // Page Size Extensions
    PhysicalAddressExtension = 1 << 5, // Physical Address Extension
    MachineCheckEnable = 1 << 6, // Machine Check Enable
    PageGlobalEnable = 1 << 7, // Page Global Enable
    PerformanceMonitoringCounterEnable = 1 << 8, // Performance-Monitoring Counter Enable
    OSFXSRSupport = 1 << 9, // OS Support for FXSAVE and FXRSTOR instructions
    OSXMMESupport = 1 << 10, // OS Support for Unmasked SIMD Floating-Point Exceptions
};

inline static void set_cr0_flags(CR0Flags flags)
{
    ASM_VOLATILE("mov %%cr0, %%eax\n"
        "or %0, %%eax\n"
        "mov %%eax, %%cr0\n"
        :
        : "r"(static_cast<uint32>(flags))
        : "%eax");
}

inline static void clear_cr0_flags(CR0Flags flags)
{
    ASM_VOLATILE("mov %%cr0, %%eax\n"
        "and %0, %%eax\n"
        "mov %%eax, %%cr0\n"
        :
        : "r"(~static_cast<uint32>(flags))
        : "%eax");
}

inline static void set_cr3(uint32 value)
{
    ASM_VOLATILE("mov %0, %%cr3\n"
        :
        : "r"(value)
        : "%eax");
}

inline static void set_cr4_flags(CR4Flags flags)
{
    ASM_VOLATILE("mov %%cr4, %%eax\n"
        "or %0, %%eax\n"
        "mov %%eax, %%cr4\n"
        :
        : "r"(static_cast<uint32>(flags))
        : "%eax");
}

inline static void clear_cr4_flags(CR4Flags flags)
{
    ASM_VOLATILE("mov %%cr4, %%eax\n"
        "and %0, %%eax\n"
        "mov %%eax, %%cr4\n"
        :
        : "r"(~static_cast<uint32>(flags))
        : "%eax");
}

inline void Control::halt()
{
    ASM_VOLATILE("hlt");
}

inline void Control::enable_interrupts()
{
    ASM_VOLATILE("sti");
}

inline void Control::disable_interrupts()
{
    ASM_VOLATILE("cli");
}

inline void Control::enable_protected_mode()
{
    set_cr0_flags(CR0Flags::ProtectionEnable);
}

inline void Control::disable_protected_mode()
{
    clear_cr0_flags(CR0Flags::ProtectionEnable);
}

inline void Control::set_page_directory(void *page_directory)
{
    set_cr3(reinterpret_cast<uint32>(page_directory));
}

inline void Control::enable_paging()
{
    set_cr0_flags(CR0Flags::Paging);
    set_cr4_flags(CR4Flags::PageSizeExtensions);
}

inline void Control::disable_paging()
{
    clear_cr0_flags(CR0Flags::Paging);
    clear_cr4_flags(CR4Flags::PageSizeExtensions);
}
