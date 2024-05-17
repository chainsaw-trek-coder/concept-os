#include <gtest/gtest.h>
#include "memory/memory_blocks.hpp"
#include <algorithm>
#include <random>
#include <vector>
#include <iostream>

using namespace std;

void validate_free_blocks_by_size(free_block *root, vector<free_block *> &visited, unsigned &free_size)
{
    if (root == nullptr)
        return;

    auto has_visited = find(visited.begin(), visited.end(), root) != visited.end();

    EXPECT_FALSE(has_visited);

    if(has_visited)
        return; // Don't want to get caught up in an infinite recursion.

    visited.push_back(root);
    free_size += root->size;

    if (root->smaller_block != nullptr)
    {
        EXPECT_TRUE(root->size >= root->smaller_block->size);
        EXPECT_EQ(root, root->smaller_block->predecessor_by_size);
        validate_free_blocks_by_size(root->smaller_block, visited, free_size);
    }

    if (root->larger_block != nullptr)
    {
        EXPECT_TRUE(root->size <= root->larger_block->size);
        EXPECT_EQ(root, root->larger_block->predecessor_by_size);
        validate_free_blocks_by_size(root->larger_block, visited, free_size);
    }
}

void validate_free_blocks_by_address(free_block *root, vector<free_block *> &visited, unsigned &free_size)
{
    if (root == nullptr)
        return;

    auto has_visited = find(visited.begin(), visited.end(), root) != visited.end();

    EXPECT_FALSE(has_visited);

    if(has_visited)
        return; // Don't want to get caught up in an infinite recursion.

    visited.push_back(root);
    free_size += root->size;

    if (root->lower_block != nullptr)
    {
        EXPECT_TRUE(root > root->lower_block);
        EXPECT_NE(root, root->lower_block);
        EXPECT_EQ(root, root->lower_block->predecessor_by_address);
        validate_free_blocks_by_address(root->lower_block, visited, free_size);
    }

    if (root->higher_block != nullptr)
    {
        EXPECT_TRUE(root < root->higher_block);
        EXPECT_NE(root, root->higher_block);
        EXPECT_EQ(root, root->higher_block->predecessor_by_address);
        validate_free_blocks_by_address(root->higher_block, visited, free_size);
    }
}

void validate_free_blocks(free_block *root, free_block *root_by_address, unsigned expected_free_size)
{
    vector<free_block *> visited;
    unsigned free_size = 0;
    validate_free_blocks_by_size(root, visited, free_size);

    EXPECT_EQ(expected_free_size, free_size);
    
    visited.clear();
    free_size = 0;
    validate_free_blocks_by_address(root_by_address, visited, free_size);

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
    validate_free_blocks(_memory_blocks.free_blocks, _memory_blocks.free_blocks_by_address, 4096 * 15);

    auto allocated_block_2 = _memory_blocks.allocate(4096);
    // Tree should have yet an even smaller block.
    validate_free_blocks(_memory_blocks.free_blocks, _memory_blocks.free_blocks_by_address, 4096 * 14);

    auto allocated_block_3 = _memory_blocks.allocate(4096);
    // Tree should have yet an even smaller block.
    validate_free_blocks(_memory_blocks.free_blocks, _memory_blocks.free_blocks_by_address, 4096 * 13);

    _memory_blocks.deallocate(allocated_block_1);
    validate_free_blocks(_memory_blocks.free_blocks, _memory_blocks.free_blocks_by_address, 4096 * 14);

    _memory_blocks.deallocate(allocated_block_3);
    validate_free_blocks(_memory_blocks.free_blocks, _memory_blocks.free_blocks_by_address, 4096 * 15);

    _memory_blocks.deallocate(allocated_block_2);
    validate_free_blocks(_memory_blocks.free_blocks, _memory_blocks.free_blocks_by_address, 4096 * 16);

    allocated_block_1 = _memory_blocks.allocate(4096);
    validate_free_blocks(_memory_blocks.free_blocks, _memory_blocks.free_blocks_by_address, 4096 * 15);

    allocated_block_2 = _memory_blocks.allocate(4096);
    validate_free_blocks(_memory_blocks.free_blocks, _memory_blocks.free_blocks_by_address, 4096 * 14);


    _memory_blocks.deallocate(allocated_block_2);
    validate_free_blocks(_memory_blocks.free_blocks, _memory_blocks.free_blocks_by_address, 4096 * 15);

    _memory_blocks.deallocate(allocated_block_1);
    validate_free_blocks(_memory_blocks.free_blocks, _memory_blocks.free_blocks_by_address, 4096 * 16);
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

    random_device random;
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
                validate_free_blocks(_memory_blocks.free_blocks, _memory_blocks.free_blocks_by_address, (num_blocks - blocks_allocated) * 4096);
            }
            else
            {
                cout << "Allocated block while out of memory..." << endl;
                EXPECT_EQ(_memory_blocks.allocate(4096), nullptr);
                validate_free_blocks(_memory_blocks.free_blocks, _memory_blocks.free_blocks_by_address, (num_blocks - blocks_allocated) * 4096);
            }
            break;

        // Deallocate
        case 1:
            if (blocks_allocated > 0)
            {                
                auto block_to_deallocate_position = deallocate_dist(random) % blocks.size();
                auto block_to_deallocate = blocks[block_to_deallocate_position];

                cout << "Deallocating block " << block_to_deallocate_position << "..." << endl;

                blocks.erase(find(blocks.begin(), blocks.end(), block_to_deallocate));
                _memory_blocks.deallocate(block_to_deallocate);
                blocks_allocated--;
                validate_free_blocks(_memory_blocks.free_blocks, _memory_blocks.free_blocks_by_address, (num_blocks - blocks_allocated) * 4096);
            }

            // Note if we attempt to deallocate what doesn't exist... we should end up with memory corruption.
            break;
        }
    }
}

TEST(MemoryTests, blocks_can_randomly_allocate_and_deallocate_1thousand_times)
{
    auto num_blocks = 1024;
    auto initial_size = 4096 * num_blocks;
    unsigned char memory[initial_size];

    for (unsigned i = 0; i < 4096 * num_blocks; i++)
        memory[i] = static_cast<unsigned char>(0xFF);

    memory_blocks _memory_blocks;
    _memory_blocks.initialize(reinterpret_cast<void *>(memory), initial_size);
    // Tree is one big block.

    vector<void *> blocks;

    random_device random;
    uniform_int_distribution<int> operation_dist(0, 1);
    uniform_int_distribution<int> deallocate_dist(0, num_blocks);

    unsigned blocks_allocated = 0;

    for (auto i = 0; i < 1000; i++)
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
                validate_free_blocks(_memory_blocks.free_blocks, _memory_blocks.free_blocks_by_address, (num_blocks - blocks_allocated) * 4096);
            }
            else
            {
                cout << "Allocated block while out of memory..." << endl;
                EXPECT_EQ(_memory_blocks.allocate(4096), nullptr);
                validate_free_blocks(_memory_blocks.free_blocks, _memory_blocks.free_blocks_by_address, (num_blocks - blocks_allocated) * 4096);
            }
            break;

        // Deallocate
        case 1:
            if (blocks_allocated > 0)
            {                
                auto block_to_deallocate_position = deallocate_dist(random) % blocks.size();
                auto block_to_deallocate = blocks[block_to_deallocate_position];

                cout << "Deallocating block " << block_to_deallocate_position << "..." << endl;

                blocks.erase(find(blocks.begin(), blocks.end(), block_to_deallocate));
                _memory_blocks.deallocate(block_to_deallocate);
                blocks_allocated--;
                validate_free_blocks(_memory_blocks.free_blocks, _memory_blocks.free_blocks_by_address, (num_blocks - blocks_allocated) * 4096);
            }

            // Note if we attempt to deallocate what doesn't exist... we should end up with memory corruption.
            break;
        }
    }
}

TEST(MemoryTests, blocks_can_allocate_and_deallocate_failure_scenario_1)
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

    int operations[32] = { 1, 1, 1, 1, 1, -2, -3, 1, -2, 1, 1, -4, 0, 0, 0, 0, 1, 0, 1, 1, 1, 1, 1, -2, -3, 0, 0, 1, -1, 1, -1 ,0 };

    unsigned blocks_allocated = 0;

    for (auto i = 0; i < 32; i++)
    {
        auto operation = operations[i];

        switch (operation)
        {
        // Allocate
        case 1:
            if (blocks_allocated < 16)
            {
                cout << "Allocating block..." << endl;
                blocks.push_back(_memory_blocks.allocate(4096));                
                blocks_allocated++;
                validate_free_blocks(_memory_blocks.free_blocks, _memory_blocks.free_blocks_by_address, (num_blocks - blocks_allocated) * 4096);
            }
            else
            {
                cout << "Allocated block while out of memory..." << endl;
                EXPECT_EQ(_memory_blocks.allocate(4096), nullptr);
                validate_free_blocks(_memory_blocks.free_blocks, _memory_blocks.free_blocks_by_address, (num_blocks - blocks_allocated) * 4096);
            }
            break;

        // Deallocate
        default:
            if (blocks_allocated > 0)
            {                
                auto block_to_deallocate_position = operations[i] * -1;
                auto block_to_deallocate = blocks[block_to_deallocate_position];

                cout << "Deallocating block " << block_to_deallocate_position << "..." << endl;

                blocks.erase(find(blocks.begin(), blocks.end(), block_to_deallocate));
                _memory_blocks.deallocate(block_to_deallocate);
                blocks_allocated--;
                validate_free_blocks(_memory_blocks.free_blocks, _memory_blocks.free_blocks_by_address, (num_blocks - blocks_allocated) * 4096);
            }

            // Note if we attempt to deallocate what doesn't exist... we should end up with memory corruption.
            break;
        }
    }
}

TEST(MemoryTests, blocks_can_allocate_and_deallocate_failure_scenario_2)
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

    int operations[27] = { 1, 1, 1, 1, 1, -1, -1, 1, 1, -3, 1, 0, 1, 1, 1, 1, 1, 1, 1, -1, 1, 1, 0, -1, -3, -5, 0 };

    unsigned blocks_allocated = 0;

    for (auto i = 0; i < 27; i++)
    {
        auto operation = operations[i];

        switch (operation)
        {
        // Allocate
        case 1:
            if (blocks_allocated < 16)
            {
                cout << "Allocating block..." << endl;
                blocks.push_back(_memory_blocks.allocate(4096));                
                blocks_allocated++;
                validate_free_blocks(_memory_blocks.free_blocks, _memory_blocks.free_blocks_by_address, (num_blocks - blocks_allocated) * 4096);
            }
            else
            {
                cout << "Allocated block while out of memory..." << endl;
                EXPECT_EQ(_memory_blocks.allocate(4096), nullptr);
                validate_free_blocks(_memory_blocks.free_blocks, _memory_blocks.free_blocks_by_address, (num_blocks - blocks_allocated) * 4096);
            }
            break;

        // Deallocate
        default:
            if (blocks_allocated > 0)
            {                
                auto block_to_deallocate_position = operations[i] * -1;
                auto block_to_deallocate = blocks[block_to_deallocate_position];

                cout << "Deallocating block " << block_to_deallocate_position << "..." << endl;

                blocks.erase(find(blocks.begin(), blocks.end(), block_to_deallocate));
                _memory_blocks.deallocate(block_to_deallocate);
                blocks_allocated--;
                validate_free_blocks(_memory_blocks.free_blocks, _memory_blocks.free_blocks_by_address, (num_blocks - blocks_allocated) * 4096);
            }

            // Note if we attempt to deallocate what doesn't exist... we should end up with memory corruption.
            break;
        }
    }
}

TEST(MemoryTests, blocks_can_allocate_and_deallocate_failure_scenario_3)
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

    vector<int> operations = {
        1, 1, 1, -2, 1, 1, -2, 1, -1, -1, 1, -2, 0, 0, 1, 1, -1, 1, -1, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 0,
        1, 0, 1, 0, 1, 1, 1, 1, 1, -3, -3, 0, 1, 1, -1, -2, 1, -2, 1, -2, 0, 1, 0, 0, 1, 1, 1, -2, 1, 1, 
        1, 1, -1, -3, 1, -3, -2, 1, -3, -2, 1, -1, 1, -2, -1, 0, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, -4, -4, 1,
        -2, -2, -2, -1, 1, 1, -2, 1, -1, -1, 0, 1, 0, 1, 1, 0, 1, 1, 1, 1, 0, 0, 1, -3, -2, 1, 0, 0, 0, 
        1, 1, 1, 1, 1, -3, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, -7, -3, 1, 1, 0, -2, -1, 1, 1, 1, -8, 1, -4,
        -9, -9, -5, 1, -4, 1, -1, 1, 1, -2, 1, -6, -3, 1, 1, -1, -1, -4, 1, 1, 0, -4, 1, 1, -2, -4, -5,
        1, 1, 1, 1, -1, 1, 0, 1, 1, -6, -7, 1, 1, -6, -8, 1, -10, 1, -5, -8, -1, 1, -3, 1, -1, 1, -2, 1, 
        -8, -7, 1, 1, -7, 1, 1, 1, 1, -2, -4, -1, -5, -1, 1, 0, -1, 1, 1, 0, 1, 1, 0, 1, 1, 1, -9, 1, 1,
        1, -7, -8, 1, -5, -7, -5, 1, 1, 1, 1, -7, -7, 1, -8, -10, -8, -3, 1, 1, 1, 1, 1, -7, 1, 1, -1, 1,
        -1, -1, -11, -4, 1, 1, 1, 1, -7, -1, -9, 1, 1, -1, 1, 1, -13, 1, -12, -12, 1, -1, 1, -10, -11, -1,
        1, 1, 1, 1, -3, 1, -1, 1, -7, -12, -5, 0, 1, 1, -5, -3, 1, -7, -3, -6, -6, 1, 1, 1, -1, 1, -6, -8,
        1, -2, 1, -4, 1, 1, -4, -3, -3, -4, -4, 1, -5, 1
    };

    unsigned blocks_allocated = 0;

    for (auto i = 0; i < operations.size(); i++)
    {
        auto operation = operations[i];

        switch (operation)
        {
        // Allocate
        case 1:
            if (blocks_allocated < 16)
            {
                cout << "Allocating block..." << endl;
                blocks.push_back(_memory_blocks.allocate(4096));                
                blocks_allocated++;
                validate_free_blocks(_memory_blocks.free_blocks, _memory_blocks.free_blocks_by_address, (num_blocks - blocks_allocated) * 4096);
            }
            else
            {
                cout << "Allocated block while out of memory..." << endl;
                EXPECT_EQ(_memory_blocks.allocate(4096), nullptr);
                validate_free_blocks(_memory_blocks.free_blocks, _memory_blocks.free_blocks_by_address, (num_blocks - blocks_allocated) * 4096);
            }
            break;

        // Deallocate
        default:
            if (blocks_allocated > 0)
            {                
                auto block_to_deallocate_position = (operations[i] * -1) % blocks.size();
                auto block_to_deallocate = blocks[block_to_deallocate_position];

                cout << "Deallocating block " << block_to_deallocate_position << "..." << endl;

                blocks.erase(find(blocks.begin(), blocks.end(), block_to_deallocate));
                _memory_blocks.deallocate(block_to_deallocate);
                blocks_allocated--;
                validate_free_blocks(_memory_blocks.free_blocks, _memory_blocks.free_blocks_by_address, (num_blocks - blocks_allocated) * 4096);
            }

            // Note if we attempt to deallocate what doesn't exist... we should end up with memory corruption.
            break;
        }
    }
}