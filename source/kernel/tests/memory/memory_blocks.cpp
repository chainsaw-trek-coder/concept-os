#include <gtest/gtest.h>
#include "memory/memory_blocks.hpp"

TEST(MemoryTests, blocks_can_initialize_free_blocks)
{
    unsigned char memory[4096*16];

    for(unsigned i = 0; i < 4096 * 16; i++)
        memory[i] = static_cast<unsigned char>(0xFF);

    memory_blocks _memory_blocks;
    _memory_blocks.initialize(reinterpret_cast<void*>(memory), 4096*16);

    auto first_free_block = reinterpret_cast<free_block*>(memory);

    EXPECT_EQ(_memory_blocks.size, 4096 * 16);
    EXPECT_EQ(first_free_block->size, 4096 * 16);
    EXPECT_EQ(first_free_block->smaller_block, nullptr);
    EXPECT_EQ(first_free_block->larger_block, nullptr);
}