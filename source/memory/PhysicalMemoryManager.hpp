#pragma once

#include "boot/boot_info.hpp"
#include "types/bitmap.hpp"
#include "types/types.hpp"

class PhysicalMemoryManager
{
public:
    static constexpr wuint PAGE_SIZE = 4096;
    static constexpr wuint MAX_PHYS_MEM = 1024ull * 1024ull * 1024ull; // 1 GiB tracking window.
    static constexpr uint32 MAX_PAGES = static_cast<uint32>(MAX_PHYS_MEM / PAGE_SIZE);
    static constexpr uint32 MAX_REGIONS = 64;
    static constexpr uint32 INVALID_FRAME = 0;

    PhysicalMemoryManager(const BootInfo &boot_info)
        : m_used_pages(), m_total_pages(MAX_PAGES), m_used_page_count(MAX_PAGES)
    {
        init(boot_info);
    }

    uint32 allocate_frame();

    void free_frame(uint32 physical_address);

    inline uint32 get_total_page_count() const
    {
        return m_total_pages;
    }

    inline uint32 get_used_page_count() const
    {
        return m_used_page_count;
    }

    inline uint32 get_free_page_count() const
    {
        return m_total_pages - m_used_page_count;
    }

private:
    void init(const BootInfo &boot_info);

    inline void reserve_all()
    {
        m_used_pages.fill(~0);
        m_used_page_count = m_total_pages;
    }

    inline static wuint align_down(wuint value)
    {
        return value & ~(PAGE_SIZE - 1);
    }

    inline static wuint align_up(wuint value)
    {
        return (value + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1);
    }

    inline static wuint clamp_end(wuint base, wuint length)
    {
        if (length == 0 || base >= MAX_PHYS_MEM)
        {
            return MAX_PHYS_MEM;
        }

        if (length > (MAX_PHYS_MEM - base))
        {
            return MAX_PHYS_MEM;
        }

        return base + length;
    }

    void mark_range(wuint base, wuint length, bool used);

    void set_page_state(uint32 page, bool used);

    Bitmap<MAX_PAGES> m_used_pages;
    uint32 m_total_pages;
    uint32 m_used_page_count;
};