#include "memory/x86_32/gdt.hpp"

segment_descriptor::segment_descriptor()
{
    dword1 = 0;
    dword2 = 0;

    // Set D/B flag which is recommended by Intel for 32-bit mode.
    // Only clear in 16-bit is recommended by Intel for 32-bit mode. Only clear in 16-bit mode.
    dword1 |= 0x00400000;

    set_present(true);
}

void segment_descriptor::set_type(segment_type type)
{
    dword1 &= 0xFFFFF0FF;
    dword1 |= (0xF & type) << 8;
}

void segment_descriptor::set_base_address(void *address)
{
    // Clear Base 31:24 and Base23:16
    dword1 &= 0x00FFFF00;

    // Set Base 31:24
    dword1 |= (reinterpret_cast<unsigned>(address) & 0xFF000000);

    // Set Base 23:16
    dword1 |= (reinterpret_cast<unsigned>(address) & 0x00FF0000) >> 16;

    // Clear Base 15:00
    dword2 &= 0xFFFF;

    // Set Base 15:00
    dword2 |= (reinterpret_cast<unsigned>(address) & 0xFFFF) << 16;
}

void segment_descriptor::set_limit(size_t limit)
{
    // Looks like bits 19:16 in dword1 lines up with 19:16 in the limit.
    // How convienient. Lets hope I get my offsets right.

    // Clear Limit 19:16
    dword1 &= 0xFFF0FFFF;

    // Set Limit 19:16
    dword1 |= (0x000F0000 & limit);

    // Clear Limit 15:0
    dword2 &= 0xFFFF0000;

    // Set Limit 15:0
    dword2 |= (0x0000FFFF & limit);

    // auto target = reinterpret_cast<unsigned char*>(&dword1);

    // // Encode the limit
    // target[0] = limit & 0xFF;
    // target[1] = (limit >> 8) & 0xFF;
    // target[6] = (limit >> 16) & 0x0F;
}

void segment_descriptor::set_granularity_flag()
{
    dword1 |= 0x00800000;
}

void segment_descriptor::clear_granularity_flag()
{
    dword1 &= 0x00700000;
}

void segment_descriptor::set_present(bool is_present)
{
    if (is_present)
        dword1 |= 0x00008000;
    else
        dword1 &= 0xFFFF7FFF;
}

void segment_descriptor::set_priviledge_level(unsigned char priviledge)
{
    dword1 &= 0xFFFF9FFF;
    dword1 |= (0x03 & priviledge) << 13;
}

void segment_descriptor::set_is_system(bool is_system)
{
    if (is_system)
        dword1 &= 0xFFFFEFFF;
    else
        dword1 |= 0x00001000;
}

void flat_global_descriptor_table::initialize()
{
    code_segment.set_base_address(nullptr);
    code_segment.set_type(segment_type::execute_read_only_conforming);
    code_segment.set_present(true);
    code_segment.set_is_system(false);
    code_segment.set_priviledge_level(0);
    code_segment.dword1 |= 0x00400000;

    data_segment.set_base_address(nullptr);
    data_segment.set_type(segment_type::read_write);
    data_segment.set_present(true);
    data_segment.set_is_system(false);
    data_segment.set_priviledge_level(0);
    data_segment.dword1 |= 0x00400000;

    // Set limit to 4 gig
    code_segment.set_granularity_flag();
    data_segment.set_granularity_flag();

    code_segment.set_limit(0xFFFFF);
    data_segment.set_limit(0xFFFFF);
}