#include <gtest/gtest.h>
#include "auto_delete.h"
#include "memory/utils.h"

#if defined(__i386__)
#include "memory/x86_32/paging.hpp"

TEST(MemoryTests, page_directory_entry_can_address)
{
    // Allocate page aligned page table entry.
    auto data = new char[sizeof(page_table_entry) + 4096];
    auto_delete<char> _(data);

    auto aligned_data = get_aligned_address(reinterpret_cast<void*>(data));
    auto table_entry = reinterpret_cast<page_table_entry*>(aligned_data);

    page_directory_entry entry;

    entry.data = 0;
    entry.set_address(reinterpret_cast<page_table_entry*>(aligned_data));

    EXPECT_EQ(reinterpret_cast<unsigned>(aligned_data), entry.data & 0xFFFFF000);
    EXPECT_EQ(0, entry.data & 0xFFF);
}

TEST(MemoryTests, page_directory_entry_can_set_writable)
{
    // Allocate page aligned page table entry.
    auto data = new char[sizeof(page_table_entry) + 4096];
    auto_delete<char> _(data);

    auto aligned_data = get_aligned_address(reinterpret_cast<void*>(data));
    auto table_entry = reinterpret_cast<page_table_entry*>(aligned_data);

    page_directory_entry entry;

    entry.data = 0;
    entry.set_address(reinterpret_cast<page_table_entry*>(aligned_data));

    entry.set_writable(true);
    EXPECT_EQ(entry.is_writable(), true);
    EXPECT_EQ((entry.data & 0x2) > 0, true);

    entry.set_writable(false);
    EXPECT_EQ(entry.is_writable(), false);
    EXPECT_EQ(entry.data & 0x2, 0);

    entry.set_writable(true);
    EXPECT_EQ(entry.is_writable(), true);
    EXPECT_EQ((entry.data & 0x2) > 0, true);
}

#endif