#include "memory/memory_blocks.hpp"

void memory_blocks::initialize(void * address, unsigned size)
{
    free_blocks = reinterpret_cast<free_block*>(address);

    free_blocks[0].size = size;
    free_blocks[0].smaller_block = nullptr;
    free_blocks[0].larger_block = nullptr;

    free_blocks[0].lower_block = nullptr;
    free_blocks[0].higher_block = nullptr;
    
}

void *memory_blocks::allocate(unsigned size_in_bytes)
{
    auto blocks_needed = size_in_bytes / 4096 + (size_in_bytes % 4096 > 0 ? 1 : 0);

    // Find suitable block.

    // Remove block from tree.

    // Split block if necessary.

    // Add new free block back to tree.

    // Return newly allocated block.

    return nullptr;
}

void * memory_blocks::allocate_next_to(void * address, unsigned size_in_bytes)
{
    auto blocks_needed = size_in_bytes / 4096 + (size_in_bytes % 4096 > 0 ? 1 : 0);

    // Find suitable block CLOSE to address.

    // Remove block from tree.

    // Split block if necessary.

    // Add new free block back to tree.

    // Return newly allocated block.

    return nullptr;
}

void memory_blocks::deallocate(void * address)
{
    // Setup block as a free data structure.

    // Look for spot on tree to add it to.

    // Merge into adjacent block if possible.
}