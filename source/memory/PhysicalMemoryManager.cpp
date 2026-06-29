#include "PhysicalMemoryManager.hpp"
#include "kernel/kernel_context.hpp"
#include "core/limits.hpp"

uint32 PhysicalMemoryManager::allocate_frame()
{
    for (uint32 page = 0; page < m_total_pages; ++page)
    {
        if (!m_used_pages.get(page))
        {
            set_page_state(page, true);
            return page * static_cast<uint32>(PAGE_SIZE);
        }
    }

    return INVALID_FRAME;
}

void PhysicalMemoryManager::free_frame(uint32 physical_address)
{
    const wuint page = static_cast<wuint>(physical_address) / PAGE_SIZE;
    if (page >= m_total_pages)
    {
        return;
    }

    set_page_state(static_cast<uint32>(page), false);
}

void PhysicalMemoryManager::init(const BootInfo &boot_info)
{
    // find the end of tracked memory
    uint32 max_tracked_end = 0;
    bool has_usable_region = false;
    const uint32 region_count = (boot_info.region_count > MAX_REGIONS) ? MAX_REGIONS : boot_info.region_count;
    for (uint32 i = 0; i < region_count; ++i)
    {
        const MemoryRegion &region = boot_info.regions[i];
        if (region.base > UINT32_MAX)
        {
            continue; // Skip regions that exceed the 32-bit addressable range.
        }
        uint32 base32 = static_cast<uint32>(region.base);
        if (region.length > UINT32_MAX)
        {
            continue; // Skip regions that exceed the 32-bit addressable range.
        }
        uint32 length32 = static_cast<uint32>(region.length);
        const wuint region_end = clamp_end(region.base, region.length);
        if (region_end > max_tracked_end)
        {
            max_tracked_end = region_end;
        }
    }

    // calculate the total number of pages to track
    const wuint total_pages64 = (max_tracked_end == 0) ? 1 : (align_up(max_tracked_end) / PAGE_SIZE);
    m_total_pages = (total_pages64 > MAX_PAGES)
                        ? MAX_PAGES
                        : static_cast<uint32>(total_pages64);

    // reserve all pages initially
    reserve_all();

    // free usable pages based on the memory map provided by the bootloader
    // usable if region type is 1 (available RAM)
    for (uint32 i = 0; i < region_count; ++i)
    {
        const MemoryRegion &region = boot_info.regions[i];
        if (region.type == 1)
        {
            has_usable_region = true;
            mark_range(region.base, region.length, false);
        }
    }

    // if no usable region was found, fallback to a default usable range of 2 MiB..64 MiB
    if (!has_usable_region)
    {
        mark_range(0x00200000ull, 0x03E00000ull, false);
    }

    // Keep low memory and early boot/kernel ranges reserved.
    mark_range(0x00000000ull, 0x00100000ull, true); // First MiB.
    mark_range(0x00100000ull, 0x00100000ull, true); // Kernel/early allocations.
    mark_range(0x00800000ull, 0x00010000ull, true); // Bootstrap stack around 8 MiB.

    // Keep framebuffer memory reserved if available.
    const wuint framebuffer_base = static_cast<wuint>(boot_info.framebuffer_address);
    const wuint framebuffer_bytes = static_cast<wuint>(boot_info.pitch) * static_cast<wuint>(boot_info.height);
    if (framebuffer_base != 0 && framebuffer_bytes != 0)
    {
        mark_range(framebuffer_base, framebuffer_bytes, true);
    }
}

void PhysicalMemoryManager::mark_range(wuint base, wuint length, bool used)
{
    if (length == 0)
    {
        return;
    }

    const wuint tracked_limit = static_cast<wuint>(m_total_pages) * PAGE_SIZE;
    wuint start = align_down(base);
    wuint end = align_up(clamp_end(base, length));

    if (start >= tracked_limit)
    {
        return;
    }

    if (end > tracked_limit)
    {
        end = tracked_limit;
    }

    for (wuint address = start; address < end; address += PAGE_SIZE)
    {
        const uint32 page = static_cast<uint32>(address / PAGE_SIZE);
        set_page_state(page, used);
    }
}

void PhysicalMemoryManager::set_page_state(uint32 page, bool used)
{
    if (page >= m_total_pages)
    {
        return;
    }

    const bool currently_used = m_used_pages.get(page);
    if (currently_used == used)
    {
        return;
    }

    m_used_pages.set(page, used);
    if (used)
    {
        ++m_used_page_count;
    }
    else
    {
        --m_used_page_count;
    }
}
