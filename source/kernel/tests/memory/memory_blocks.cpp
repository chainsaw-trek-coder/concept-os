#include <gtest/gtest.h>
#include "memory/memory_blocks.hpp"

TEST(MemoryTests, blocks_can_initialize_free_blocks)
{
    unsigned char memory[4096*16];

    memory_blocks _memory_blocks;
    _memory_blocks.initialize(reinterpret_cast<void*>(memory), 4096*16);

    auto first_free_block = reinterpret_cast<free_block*>(memory);

    EXPECT_EQ(_memory_blocks.size, 4096 * 16);
    EXPECT_EQ(first_free_block->size, 4096 * 16);
    EX
}