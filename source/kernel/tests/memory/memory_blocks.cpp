#include <gtest/gtest.h>
#include "memory/memory_blocks.hpp"
#include <algorithm>
#include <random>
#include <vector>
#include <iostream>

using namespace std;

void validate_free_blocks(free_block *root, vector<free_block *> &visited, unsigned &free_size)
{
    if (root == nullptr)
        return;

    auto has_visited = find(visited.begin(), visited.end(), root) != visited.end();

    EXPECT_FALSE(has_visited);

    visited.push_back(root);
    free_size += root->size;

    if (root->smaller_block != nullptr)
    {
        EXPECT_TRUE(root->size >= root->smaller_block->size);
        EXPECT_EQ(root, root->smaller_block->predecessor_by_size);
        validate_free_blocks(root->smaller_block, visited, free_size);
    }

    if (root->larger_block != nullptr)
    {
        EXPECT_TRUE(root->size <= root->larger_block->size);
        EXPECT_EQ(root, root->larger_block->predecessor_by_size);
        validate_free_blocks(root->larger_block, visited, free_size);
    }

    // visited.pop_back();
}

void validate_free_blocks(free_block *root, unsigned expected_free_size)
{
    vector<free_block *> visited;
    unsigned free_size = 0;
    validate_free_blocks(root, visited, free_size);

    EXPECT_EQ(expected_free_size, free_size);
}

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

TEST(MemoryTests, blocks_can_simply_allocate_and_deallocate)
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
    validate_free_blocks(_memory_blocks.free_blocks, 4096 * 15);

    auto allocated_block_2 = _memory_blocks.allocate(4096);
    // Tree should have yet an even smaller block.
    validate_free_blocks(_memory_blocks.free_blocks, 4096 * 14);

    auto allocated_block_3 = _memory_blocks.allocate(4096);
    // Tree should have yet an even smaller block.
    validate_free_blocks(_memory_blocks.free_blocks, 4096 * 13);

    _memory_blocks.deallocate(allocated_block_1);
    validate_free_blocks(_memory_blocks.free_blocks, 4096 * 14);

    _memory_blocks.deallocate(allocated_block_3);
    validate_free_blocks(_memory_blocks.free_blocks, 4096 * 15);

    _memory_blocks.deallocate(allocated_block_2);
    validate_free_blocks(_memory_blocks.free_blocks, 4096 * 16);

    allocated_block_1 = _memory_blocks.allocate(4096);
    validate_free_blocks(_memory_blocks.free_blocks, 4096 * 15);

    allocated_block_2 = _memory_blocks.allocate(4096);
    validate_free_blocks(_memory_blocks.free_blocks, 4096 * 14);

    _memory_blocks.deallocate(allocated_block_2);
    validate_free_blocks(_memory_blocks.free_blocks, 4096 * 15);

    _memory_blocks.deallocate(allocated_block_1);
    validate_free_blocks(_memory_blocks.free_blocks, 4096 * 16);
}

TEST(MemoryTests, blocks_can_randomly_allocate_and_deallocate)
{
    auto num_blocks = 16;
    auto initial_size = 4096 * num_blocks;
    unsigned char memory[initial_size];

    for (unsigned i = 0; i < 4096 * num_blocks; i++)
        memory[i] = static_cast<unsigned char>(0xFF);

    memory_blocks _memory_blocks;
    _memory_blocks.initialize(reinterpret_cast<void *>(memory), initial_size);
    // Tree is one big block.

    vector<void *> blocks;

    random_device random("f3604415-e89f-416b-a80f-9a48fe2a8337");
    uniform_int_distribution<int> operation_dist(0, 1);
    uniform_int_distribution<int> deallocate_dist(0, num_blocks);

    unsigned blocks_allocated = 0;

    for (auto i = 0; i < 100; i++)
    {
        auto operation = operation_dist(random);

        switch (operation)
        {
        // Allocate
        case 0:
            if (blocks_allocated < 16)
            {
                cout << "Allocating block..." << endl;
                blocks.push_back(_memory_blocks.allocate(4096));                
                blocks_allocated++;
                validate_free_blocks(_memory_blocks.free_blocks, (num_blocks - blocks_allocated) * 4096);
            }
            else
            {
                cout << "Allocated block while out of memory..." << endl;
                EXPECT_EQ(_memory_blocks.allocate(4096), nullptr);
                validate_free_blocks(_memory_blocks.free_blocks, (num_blocks - blocks_allocated) * 4096);
            }
            break;

        // Deallocate
        case 2:
            if (blocks_allocated > 0)
            {
                cout << "Deallocating block..." << endl;
                auto block_to_deallocate_position = deallocate_dist(random) % blocks.size();
                auto block_to_deallocate = blocks[block_to_deallocate_position];
                blocks.erase(find(blocks.begin(), blocks.end(), block_to_deallocate));
                _memory_blocks.deallocate(block_to_deallocate);
                blocks_allocated--;
                validate_free_blocks(_memory_blocks.free_blocks, (num_blocks - blocks_allocated) * 4096);
            }

            // Note if we attempt to deallocate what doesn't exist... we should end up with memory corruption.
            break;
        }
    }
}