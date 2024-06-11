#include <gtest/gtest.h>
#include "memory/utils.h"

TEST(MemoryTests, utils_can_page_align)
{
    void* address = reinterpret_cast<void*>(0xBAADF00D);
    void* aligned_address = reinterpret_cast<void*>(0xBAAE0000);

    void* test_aligned_address = get_aligned_address(address);

    EXPECT_EQ(aligned_address, test_aligned_address);
}