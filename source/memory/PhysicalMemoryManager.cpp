#include "PhysicalMemoryManager.hpp"
#include "kernel/kernel_context.hpp"
#include "core/limits.hpp"

void PhysicalMemoryManager::init(const BootInfo &boot_info)
{
    Serial &serial = gp_kernel_context->serial;
    serial.write("\nInitializing PhysicalMemoryManager...");

    // find the end of tracked memory
    uint32 max_tracked_end = 0;
    bool has_usable_region = false;
    const uint32 region_count = (boot_info.region_count > MAX_REGIONS) ? MAX_REGIONS : boot_info.region_count;
    for (uint32 i = 0; i < region_count; ++i)
    {
        const MemoryRegion &region = boot_info.regions[i];
        serial.write("\nRegion ");
        serial.write(i);
        serial.write(": Base = ");
        if (region.base > UINT32_MAX)
        {
            continue; // Skip regions that exceed the 32-bit addressable range.
        }
        uint32 base32 = static_cast<uint32>(region.base);
        serial.write(base32);
        serial.write(", Length = ");
        if (region.length > UINT32_MAX)
        {
            continue; // Skip regions that exceed the 32-bit addressable range.
        }
        uint32 length32 = static_cast<uint32>(region.length);
        serial.write(length32);
        serial.write(", Type = ");
        serial.write(region.type);
        const wuint region_end = clamp_end(region.base, region.length);
        if (region_end > max_tracked_end)
        {
            max_tracked_end = region_end;
        }
    }

    serial.write("\nMax tracked end: ");
    serial.write(max_tracked_end);

    // calculate the total number of pages to track
    const wuint total_pages64 = (max_tracked_end == 0) ? 1 : (align_up(max_tracked_end) / PAGE_SIZE);
    m_total_pages = (total_pages64 > MAX_PAGES)
                        ? MAX_PAGES
                        : static_cast<uint32>(total_pages64);

    serial.write("\nTotal tracked pages: ");
    serial.write(m_total_pages);

    // reserve all pages initially
    reserve_all();

    serial.write("\nAll pages reserved. Used pages: ");
    serial.write(m_used_page_count);

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

    serial.write("\nUsable pages marked. Used pages: ");
    serial.write(m_used_page_count);

    // if no usable region was found, fallback to a default usable range of 2 MiB..64 MiB
    if (!has_usable_region)
    {
        mark_range(0x00200000ull, 0x03E00000ull, false);
        serial.write("\nNo usable region found. Fallback to default usable range. Used pages: ");
        serial.write(m_used_page_count);
    }

    // Keep low memory and early boot/kernel ranges reserved.
    mark_range(0x00000000ull, 0x00100000ull, true); // First MiB.
    mark_range(0x00100000ull, 0x00100000ull, true); // Kernel/early allocations.
    mark_range(0x00800000ull, 0x00010000ull, true); // Bootstrap stack around 8 MiB.

    serial.write("\nReserved low memory and early boot/kernel ranges. Used pages: ");
    serial.write(m_used_page_count);

    // Keep framebuffer memory reserved if available.
    const wuint framebuffer_base = static_cast<wuint>(boot_info.framebuffer_address);
    const wuint framebuffer_bytes = static_cast<wuint>(boot_info.pitch) * static_cast<wuint>(boot_info.height);
    if (framebuffer_base != 0 && framebuffer_bytes != 0)
    {
        mark_range(framebuffer_base, framebuffer_bytes, true);
    }

    serial.write("\nReserved framebuffer memory. Used pages: ");
    serial.write(m_used_page_count);
}