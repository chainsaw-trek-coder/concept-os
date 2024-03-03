#include <gtest/gtest.h>
#include "auto_delete.h"
#include "memory/utils.h"

#if defined(__i386__)
#include "memory/x86_32/paging.hpp"

// Page table tests

TEST(MemoryTests, page_table_entry_can_set_address)
{
    // Allocate page aligned page table entry.
    auto data = new char[4096 * 2];
    auto_delete<char> _(data);

    auto aligned_data = get_aligned_address(reinterpret_cast<void*>(data));

    page_table_entry entry;

    entry.data = 0;
    entry.set_address(reinterpret_cast<void*>(aligned_data));

    EXPECT_EQ(reinterpret_cast<unsigned>(aligned_data), entry.data & 0xFFFFF000);
    EXPECT_EQ(0, entry.data & 0xFFF);
    EXPECT_EQ(entry.get_address(), reinterpret_cast<void*>(aligned_data));
}

TEST(MemoryTests, page_table_entry_can_set_global)
{
    // Allocate page aligned page table entry.
    auto data = new char[4096 * 2];
    auto_delete<char> _(data);

    auto aligned_data = get_aligned_address(reinterpret_cast<void*>(data));

    page_table_entry entry;

    entry.data = 0;
    entry.set_address(reinterpret_cast<void*>(aligned_data));

    auto mask = 0x100;

    entry.set_global_page(true);
    EXPECT_EQ(entry.is_global_page(), true);
    EXPECT_EQ((entry.data & mask) > 0, true);

    entry.set_global_page(false);
    EXPECT_EQ(entry.is_global_page(), false);
    EXPECT_EQ(entry.data & mask, 0);

    entry.set_global_page(true);
    EXPECT_EQ(entry.is_global_page(), true);
    EXPECT_EQ((entry.data & mask) > 0, true);
}

// Page directory tests

TEST(MemoryTests, page_directory_entry_can_set_address)
{
    // Allocate page aligned page table entry.
    auto data = new char[sizeof(page_table_entry) + 4096];
    auto_delete<char> _(data);

    auto aligned_data = get_aligned_address(reinterpret_cast<void*>(data));
    auto table_entry = reinterpret_cast<page_table_entry*>(aligned_data);

    page_directory_entry entry;

    entry.data = 0;
    entry.set_address(reinterpret_cast<page_table*>(aligned_data));

    EXPECT_EQ(reinterpret_cast<unsigned>(aligned_data), entry.data & 0xFFFFF000);
    EXPECT_EQ(0, entry.data & 0xFFF);
    EXPECT_EQ(entry.get_address(), reinterpret_cast<void*>(aligned_data));
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
    entry.set_address(reinterpret_cast<page_table*>(aligned_data));

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

TEST(MemoryTests, page_directory_entry_can_set_present)
{
    // Allocate page aligned page table entry.
    auto data = new char[sizeof(page_table_entry) + 4096];
    auto_delete<char> _(data);

    auto aligned_data = get_aligned_address(reinterpret_cast<void*>(data));
    auto table_entry = reinterpret_cast<page_table_entry*>(aligned_data);

    page_directory_entry entry;

    entry.data = 0;
    entry.set_address(reinterpret_cast<page_table*>(aligned_data));

    entry.set_present(true);
    EXPECT_EQ(entry.is_present(), true);
    EXPECT_EQ((entry.data & 0x1) > 0, true);

    entry.set_present(false);
    EXPECT_EQ(entry.is_present(), false);
    EXPECT_EQ(entry.data & 0x1, 0);

    entry.set_present(true);
    EXPECT_EQ(entry.is_present(), true);
    EXPECT_EQ((entry.data & 0x1) > 0, true);
}

#endif