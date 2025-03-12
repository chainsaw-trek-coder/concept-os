#include <gtest/gtest.h>
#include "memory/mem_allocator.hpp"
#include <algorithm>
#include <random>
#include <vector>
#include <map>
#include <iostream>

using namespace std;

extern void validate_free_blocks_by_size(free_block *root, vector<free_block *> &visited, unsigned &free_size);
extern void validate_free_blocks_by_address(free_block *root, vector<free_block *> &visited, unsigned &free_size);
extern void validate_free_blocks(free_block *root, free_block *root_by_address, unsigned expected_free_size);

TEST(MemoryTests, blocks_can_randomly_allocate_and_deallocate_with_tracking)
{
    auto num_blocks = 16;
    auto initial_size = 4096 * num_blocks;
    auto block_size = sizeof(free_block);
    unsigned char memory[initial_size];

    for (unsigned i = 0; i < 4096 * num_blocks; i++)
        memory[i] = static_cast<unsigned char>(0xFF);

    mem_allocator _allocator;
    _allocator.initialize(reinterpret_cast<void *>(memory), initial_size, block_size, true);
    // Tree is one big block.

    vector<void *> blocks;
    map<void *, unsigned> block_sizes;

    random_device random;
    uniform_int_distribution<int> operation_dist(0, 1);
    uniform_int_distribution<int> deallocate_dist(0, num_blocks);
    uniform_int_distribution<int> size_dist(block_size, 4096 * 5);

    unsigned blocks_allocated = 0;
    unsigned free_space = initial_size - (initial_size % block_size);

    for (auto i = 0; i < 10000; i++)
    {
        auto operation = operation_dist(random);
        auto size = size_dist(random);
        auto allocated_size = number_of_blocks(size + sizeof(allocated_block), block_size) * block_size;

        switch (operation)
        {
        // Allocate
        case 0:
            if (_allocator.free_space >= allocated_size)
            {
                cout << "Allocating block...";
                auto new_block = _allocator.allocate(size);
                cout << new_block << endl;
                // EXPECT_NE(new_block, nullptr); -- This can happen due to fragmentation.

                if (new_block != nullptr)
                {
                    blocks.push_back(new_block);
                    block_sizes.insert(pair<void *, unsigned>(new_block, allocated_size));
                    blocks_allocated++;
                    free_space -= allocated_size;
                }
                else
                {
                    cout << "Allocation failure, possibly due to fragmentation." << endl;
                }

                validate_free_blocks(_allocator.free_blocks, _allocator.free_blocks_by_address, free_space);
            }
            else
            {
                cout << "Allocated block while out of memory..." << endl;
                EXPECT_EQ(_allocator.allocate(size), nullptr);
                validate_free_blocks(_allocator.free_blocks, _allocator.free_blocks_by_address, free_space);
            }
            break;

        // Deallocate
        case 1:
            if (blocks_allocated > 0)
            {
                auto block_to_deallocate_position = deallocate_dist(random) % blocks.size();
                auto block_to_deallocate = blocks[block_to_deallocate_position];

                cout << "Deallocating block " << block_to_deallocate_position << "..." << block_to_deallocate << endl;

                auto deallocation_size = block_sizes[block_to_deallocate];
                blocks.erase(find(blocks.begin(), blocks.end(), block_to_deallocate));
                block_sizes.erase(block_to_deallocate);
                _allocator.deallocate(block_to_deallocate);
                blocks_allocated--;
                free_space += deallocation_size;
                validate_free_blocks(_allocator.free_blocks, _allocator.free_blocks_by_address, free_space);
            }

            // Note if we attempt to deallocate what doesn't exist... we should end up with memory corruption.
            break;
        }
    }
}
