#include "memory/x86_32/paging.hpp"

void page_directory_entry::set_address(page_table_entry *address)
{
    // TODO: Should I check that the address is 4K aligned here
    //       or in a unit test of code using this method? 🤔

    data &= 0xFFF; // Clear current base address.
    data |= (reinterpret_cast<unsigned>(address) & 0xFFFFF000);
}

page_table_entry *page_directory_entry::get_address()
{
    // Make sure to chop off the flags.
    return reinterpret_cast<page_table_entry *>(data & 0xFFFFF000);
}

bool page_directory_entry::is_4mb_page()
{
    return data & 0x80;
}

// TODO: Write a test that checks this flag is set after page is "accessed"
bool page_directory_entry::is_accessed()
{
    return data & 0x20;
}

void page_directory_entry::clear_accessed()
{
    // Clear 5th bit. 0xD == 13 or 1101.
    data &= 0xFFFFFFDF;
}
bool page_directory_entry::is_cache_disabled()
{
    return data & 0x10;
}

bool page_directory_entry::is_write_through()
{
    return data & 0x8;
}

page_directory_entry_type page_directory_entry::get_type()
{
    if (data & 0x4)
        return page_directory_entry_type::user;
    else
        return page_directory_entry_type::supervisor;
}

bool page_directory_entry::is_writable()
{
    return (data & 0x2) > 0;
}

void page_directory_entry::set_writable(bool is_writable)
{
    if(is_writable)
        data |= 0x2;
    else
        data &= 0xFFFFFFFD;
}

bool page_directory_entry::is_present()
{
    return false;
}
