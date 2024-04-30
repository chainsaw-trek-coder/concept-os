#include <gtest/gtest.h>
#include "memory/memory_blocks.hpp"

TEST(MemoryTests, blocks_can_initialize_free_blocks)
{
    auto initial_size = 4096 * 16;
    unsigned char memory[initial_size];

    for (unsigned i = 0; i < 4096 * 16; i++)
        memory[i] = static_cast<unsigned char>(0xFF);

    memory_blocks _memory_blocks;
    _memory_blocks.initialize(reinterpret_cast<void *>(memory), initial_size);

    auto first_free_block = reinterpret_cast<free_block *>(memory);

    EXPECT_EQ(first_free_block, _memory_blocks.free_blocks);
    EXPECT_EQ(_memory_blocks.size, 4096 * 16);
    EXPECT_EQ(first_free_block->size, 4096 * 16);
    EXPECT_EQ(first_free_block->smaller_block, nullptr);
    EXPECT_EQ(first_free_block->larger_block, nullptr);
}

TEST(MemoryTests, free_blocks_are_4096_bytes)
{
    EXPECT_EQ(sizeof(free_block), 4096);
}

TEST(MemoryTests, blocks_can_allocate_a_block)
{
    auto initial_size = 4096 * 16;
    unsigned char memory[initial_size];

    for (unsigned i = 0; i < 4096 * 16; i++)
        memory[i] = static_cast<unsigned char>(0xFF);

    memory_blocks _memory_blocks;
    _memory_blocks.initialize(reinterpret_cast<void *>(memory), initial_size);

    auto allocated_block = _memory_blocks.allocate(4096);

    EXPECT_EQ(_memory_blocks.free_blocks->size, initial_size - 4096);
    EXPECT_NE(reinterpret_cast<void *>(_memory_blocks.free_blocks), allocated_block);
    EXPECT_EQ(_memory_blocks.free_blocks->predecessor_by_size, nullptr);
    EXPECT_EQ(_memory_blocks.free_blocks->smaller_block, nullptr);
    EXPECT_EQ(_memory_blocks.free_blocks->larger_block, nullptr);
}

TEST(MemoryTests, blocks_can_allocate_two_blocks)
{
    auto initial_size = 4096 * 16;
    unsigned char memory[initial_size];

    for (unsigned i = 0; i < 4096 * 16; i++)
        memory[i] = static_cast<unsigned char>(0xFF);

    memory_blocks _memory_blocks;
    _memory_blocks.initialize(reinterpret_cast<void *>(memory), initial_size);

    auto allocated_block_1 = _memory_blocks.allocate(4096);
    auto allocated_block_2 = _memory_blocks.allocate(4096);

    EXPECT_EQ(_memory_blocks.free_blocks->size, initial_size - 4096 - 4096);
    EXPECT_NE(reinterpret_cast<void *>(_memory_blocks.free_blocks), allocated_block_1);
    EXPECT_NE(reinterpret_cast<void *>(_memory_blocks.free_blocks), allocated_block_2);
    EXPECT_NE(allocated_block_1, allocated_block_2);
    EXPECT_EQ(_memory_blocks.free_blocks->predecessor_by_size, nullptr);
    EXPECT_EQ(_memory_blocks.free_blocks->smaller_block, nullptr);
    EXPECT_EQ(_memory_blocks.free_blocks->larger_block, nullptr);
}

TEST(MemoryTests, blocks_can_deallocate_a_block)
{
    auto initial_size = 4096 * 16;
    unsigned char memory[initial_size];

    for (unsigned i = 0; i < 4096 * 16; i++)
        memory[i] = static_cast<unsigned char>(0xFF);

    memory_blocks _memory_blocks;
    _memory_blocks.initialize(reinterpret_cast<void *>(memory), initial_size);

    auto allocated_block_1 = _memory_blocks.allocate(4096);
    auto allocated_block_2 = _memory_blocks.allocate(4096);
    auto allocated_block_3 = _memory_blocks.allocate(4096);

    _memory_blocks.deallocate(allocated_block_2);

    EXPECT_EQ(_memory_blocks.free_blocks->size, initial_size - 4096 - 4096 - 4096);
    EXPECT_EQ(_memory_blocks.free_blocks->smaller_block->size, 4096);

    EXPECT_NE(reinterpret_cast<void *>(_memory_blocks.free_blocks), allocated_block_1);
    EXPECT_NE(reinterpret_cast<void *>(_memory_blocks.free_blocks), allocated_block_3);

    EXPECT_EQ(_memory_blocks.free_blocks->predecessor_by_size, nullptr);
    EXPECT_EQ(_memory_blocks.free_blocks->smaller_block, reinterpret_cast<free_block *>(allocated_block_2));
    EXPECT_EQ(_memory_blocks.free_blocks->larger_block, nullptr);
}

TEST(MemoryTests, blocks_can_randomly_allocate_and_deallocate)
{
    auto initial_size = 4096 * 16;
    unsigned char memory[initial_size];

    for (unsigned i = 0; i < 4096 * 16; i++)
        memory[i] = static_cast<unsigned char>(0xFF);

    memory_blocks _memory_blocks;
    _memory_blocks.initialize(reinterpret_cast<void *>(memory), initial_size);
    // Tree is one big block.

    auto allocated_block_1 = _memory_blocks.allocate(4096);
    // Tree should have on smaller block.

    auto allocated_block_2 = _memory_blocks.allocate(4096);
    // Tree should have yet an even smaller block.

    auto allocated_block_3 = _memory_blocks.allocate(4096);
    // Tree should have yet an even smaller block.

    _memory_blocks.deallocate(allocated_block_1);
    /* Tree should look like this...

                13 blocks
            /

        1 block

    */

    EXPECT_EQ(_memory_blocks.free_blocks->size, 4096 * 13);
    EXPECT_EQ(_memory_blocks.free_blocks->larger_block, nullptr);
    EXPECT_NE(_memory_blocks.free_blocks->smaller_block, nullptr);
    EXPECT_EQ(_memory_blocks.free_blocks->smaller_block->size, 4096 * 1);
    EXPECT_EQ(_memory_blocks.free_blocks->smaller_block->smaller_block, nullptr);
    EXPECT_EQ(_memory_blocks.free_blocks->smaller_block->larger_block, nullptr);

    _memory_blocks.deallocate(allocated_block_3);
    /* Tree should look like this...

                13 blocks
            /

        1 block
            |
        1 block

    */
    EXPECT_EQ(_memory_blocks.free_blocks->size, 4096 * 13);
    EXPECT_EQ(_memory_blocks.free_blocks->larger_block, nullptr);
    EXPECT_NE(_memory_blocks.free_blocks->smaller_block, nullptr);
    EXPECT_EQ(_memory_blocks.free_blocks->smaller_block->size, 4096 * 1);
    EXPECT_TRUE(_memory_blocks.free_blocks->smaller_block->smaller_block == nullptr || _memory_blocks.free_blocks->smaller_block->larger_block == nullptr);
    EXPECT_FALSE(_memory_blocks.free_blocks->smaller_block->smaller_block == nullptr && _memory_blocks.free_blocks->smaller_block->larger_block == nullptr);

    if (_memory_blocks.free_blocks->smaller_block->smaller_block != nullptr)
    {
        EXPECT_EQ(_memory_blocks.free_blocks->smaller_block->smaller_block->size, 4096 * 1);
        EXPECT_EQ(_memory_blocks.free_blocks->smaller_block->smaller_block->smaller_block, nullptr);
        EXPECT_EQ(_memory_blocks.free_blocks->smaller_block->smaller_block->larger_block, nullptr);
    }

    if (_memory_blocks.free_blocks->smaller_block->larger_block != nullptr)
    {
        EXPECT_EQ(_memory_blocks.free_blocks->smaller_block->larger_block->size, 4096 * 1);
        EXPECT_EQ(_memory_blocks.free_blocks->smaller_block->larger_block->smaller_block, nullptr);
        EXPECT_EQ(_memory_blocks.free_blocks->smaller_block->larger_block->larger_block, nullptr);
    }

    _memory_blocks.deallocate(allocated_block_2);
    /* Tree should look like this...

                9 blocks
            /

        1 block
            |
        1 block
            |
        1 block

    */

    allocated_block_1 = _memory_blocks.allocate(4096);
    /* Tree should look like this...

                13 blocks
            /

        1 block
            |
        1 block

    */
    EXPECT_EQ(_memory_blocks.free_blocks->size, 4096 * 13);
    EXPECT_EQ(_memory_blocks.free_blocks->larger_block, nullptr);
    EXPECT_NE(_memory_blocks.free_blocks->smaller_block, nullptr);
    EXPECT_EQ(_memory_blocks.free_blocks->smaller_block->size, 4096 * 1);
    EXPECT_TRUE(_memory_blocks.free_blocks->smaller_block->smaller_block == nullptr || _memory_blocks.free_blocks->smaller_block->larger_block == nullptr);
    EXPECT_FALSE(_memory_blocks.free_blocks->smaller_block->smaller_block == nullptr && _memory_blocks.free_blocks->smaller_block->larger_block == nullptr);

    if (_memory_blocks.free_blocks->smaller_block->smaller_block != nullptr)
    {
        EXPECT_EQ(_memory_blocks.free_blocks->smaller_block->smaller_block->size, 4096 * 1);
        EXPECT_EQ(_memory_blocks.free_blocks->smaller_block->smaller_block->smaller_block, nullptr);
        EXPECT_EQ(_memory_blocks.free_blocks->smaller_block->smaller_block->larger_block, nullptr);
    }

    if (_memory_blocks.free_blocks->smaller_block->larger_block != nullptr)
    {
        EXPECT_EQ(_memory_blocks.free_blocks->smaller_block->larger_block->size, 4096 * 1);
        EXPECT_EQ(_memory_blocks.free_blocks->smaller_block->larger_block->smaller_block, nullptr);
        EXPECT_EQ(_memory_blocks.free_blocks->smaller_block->larger_block->larger_block, nullptr);
    }


    allocated_block_2 = _memory_blocks.allocate(4096);
    /* Tree should look like this...

                13 blocks
            /

        1 block

    */

    _memory_blocks.deallocate(allocated_block_2);
    /* Tree should look like this...

                13 blocks
            /

        1 block
            |
        1 block

    */
    EXPECT_EQ(_memory_blocks.free_blocks->size, 4096 * 13);
    EXPECT_EQ(_memory_blocks.free_blocks->larger_block, nullptr);
    EXPECT_NE(_memory_blocks.free_blocks->smaller_block, nullptr);
    EXPECT_EQ(_memory_blocks.free_blocks->smaller_block->size, 4096 * 1);
    EXPECT_TRUE(_memory_blocks.free_blocks->smaller_block->smaller_block == nullptr || _memory_blocks.free_blocks->smaller_block->larger_block == nullptr);
    EXPECT_FALSE(_memory_blocks.free_blocks->smaller_block->smaller_block == nullptr && _memory_blocks.free_blocks->smaller_block->larger_block == nullptr);

    if (_memory_blocks.free_blocks->smaller_block->smaller_block != nullptr)
    {
        EXPECT_EQ(_memory_blocks.free_blocks->smaller_block->smaller_block->size, 4096 * 1);
        EXPECT_EQ(_memory_blocks.free_blocks->smaller_block->smaller_block->smaller_block, nullptr);
        EXPECT_EQ(_memory_blocks.free_blocks->smaller_block->smaller_block->larger_block, nullptr);
    }

    if (_memory_blocks.free_blocks->smaller_block->larger_block != nullptr)
    {
        EXPECT_EQ(_memory_blocks.free_blocks->smaller_block->larger_block->size, 4096 * 1);
        EXPECT_EQ(_memory_blocks.free_blocks->smaller_block->larger_block->smaller_block, nullptr);
        EXPECT_EQ(_memory_blocks.free_blocks->smaller_block->larger_block->larger_block, nullptr);
    }

    _memory_blocks.deallocate(allocated_block_1);
    /* Tree should look like this...

                13 blocks
            /

        1 block
            |
        1 block
            |
        1 block

    */
}