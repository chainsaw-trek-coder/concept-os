#include "memory/x86_32/paging.hpp"
#include "paging.hpp"

// TODO: Inline all of these functions for performance.

/* Page table functions. */

page_table_entry::page_table_entry()
{
    data = 0;
}

void page_table_entry::set_address(void* address)
{
    // TODO: Should I check that the address is 4K aligned here
    //       or in a unit test of code using this method? 🤔

    data &= 0xFFF; // Clear current base address.
    data |= (reinterpret_cast<unsigned>(address) & 0xFFFFF000);
}

void *page_table_entry::get_address()
{
    // Make sure to chop off the flags.
    return reinterpret_cast<void*>(data & 0xFFFFF000);
}

bool page_table_entry::is_global_page()
{
    return (data & 0x100) > 0;
}

void page_table_entry::set_global_page(bool is_global_page)
{
    if(is_global_page)
        data |= 0x100;
    else
        data &= ~(0x100);
}

short page_table_entry::page_table_attribute_index()
{
    auto mask = 0x80;
    return (data & mask) > 0;
}


bool page_table_entry::is_dirty()
{
    auto mask = 0x40;
    return (data & mask) > 0;
}

void page_table_entry::set_dirty(bool is_dirty)
{
    auto mask = 0x40;

    if(is_dirty)
        data |= mask;
    else
        data &= ~mask;
}

// TODO: Write a test that checks this flag is set after page is "accessed"
bool page_table_entry::is_accessed()
{
    return data & 0x20;
}

void page_table_entry::clear_accessed()
{
    data &= ~(0x20);
}

bool page_table_entry::is_cache_disabled()
{
    return data & 0x10;
}

bool page_table_entry::is_write_through()
{
    return data & 0x8;
}

void page_table_entry::set_type(page_entry_type type)
{
    auto mask = 0x4;

    if(type == page_entry_type::user)
        data |= mask;
    
    if(type == page_entry_type::supervisor)
        data &= ~mask;
}

page_entry_type page_table_entry::get_type()
{
    if (data & 0x4)
        return page_entry_type::user;
    else
        return page_entry_type::supervisor;
}

bool page_table_entry::is_writable()
{
    return (data & 0x2) > 0;
}

void page_table_entry::set_writable(bool is_writable)
{
    auto mask = 0x2;

    if(is_writable)
        data |= mask;
    else
        data &= ~mask;
}

bool page_table_entry::is_present()
{
    return (data & 0x1) > 0;
}

void page_table_entry::set_present(bool is_present)
{
    auto mask = 0x1;

    if(is_present)
        data |= mask;
    else
        data &= ~mask;
}

/* Page directory entry functions. */

page_directory_entry::page_directory_entry()
{
    data = 0;
}

void page_directory_entry::set_address(page_table *address)
{
    // TODO: Should I check that the address is 4K aligned here
    //       or in a unit test of code using this method? 🤔

    data &= 0xFFF; // Clear current base address.
    data |= (reinterpret_cast<unsigned>(address) & 0xFFFFF000);
}

page_table *page_directory_entry::get_address()
{
    // Make sure to chop off the flags.
    return reinterpret_cast<page_table *>(data & 0xFFFFF000);
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
    // Clear 5th bit.
    data &= ~(0x20);
}
bool page_directory_entry::is_cache_disabled()
{
    return data & 0x10;
}

bool page_directory_entry::is_write_through()
{
    return data & 0x8;
}

page_entry_type page_directory_entry::get_type()
{
    if (data & 0x4)
        return page_entry_type::user;
    else
        return page_entry_type::supervisor;
}

bool page_directory_entry::is_writable()
{
    return (data & 0x2) > 0;
}

void page_directory_entry::set_writable(bool is_writable)
{
    auto mask = 0x2;

    if(is_writable)
        data |= mask;
    else
        data &= ~mask;
}

bool page_directory_entry::is_present()
{
    return (data & 0x1) > 0;
}

void page_directory_entry::set_present(bool is_present)
{
    auto mask = 0x1;

    if(is_present)
        data |= mask;
    else
        data &= ~mask;
}