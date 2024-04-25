#include "memory/memory_blocks.hpp"

void memory_blocks::initialize(void *address, unsigned size)
{
    free_blocks = reinterpret_cast<free_block *>(address);
    this->size = size;

    free_blocks[0].size = size;
    free_blocks[0].predecessor_by_size nullptr;
    free_blocks[0].smaller_block = nullptr;
    free_blocks[0].larger_block = nullptr;

    free_blocks[0].lower_block = nullptr;
    free_blocks[0].higher_block = nullptr;
}

void memory_blocks::remove_node_from_tree(free_block *block)
{
    // No successors
    if (block->smaller_block == nullptr && block->larger_block == nullptr)
    {
        free_blocks = nullptr;
        return;
    }

    // One successor - part 1.
    if (block->smaller_block == nullptr && block->larger_block != nullptr)
    {
        free_blocks = block->larger_block;
        return;
    }

    // One successor - part 2.
    if (block->larger_block == nullptr && block->smaller_block != nullptr)
    {
        free_blocks = block->smaller_block;
        return;
    }

    // Multiple successors.

    // Find in-order successor...
    auto successor = block;

    successor = successor->larger_block;
    while (successor->smaller_block != nullptr)
        successor = successor->smaller_block;

    // Make sure to remove block from tree.

    // Problem... we don't have a link to the predecessor.
}

void *memory_blocks::allocate(unsigned size_in_bytes)
{
    auto blocks_needed = size_in_bytes / 4096 + (size_in_bytes % 4096 > 0 ? 1 : 0);

    // Find suitable block.
    auto current_block = free_blocks;

    while (true)
    {
        if (current_block->smaller_block != nullptr && current_block->smaller_block->size > size_in_bytes)
        {
            // Visit smaller block.
            current_block = current_block->smaller_block;
            continue;
        }

        if (current_block->size < size_in_bytes && current_block->larger_block != nullptr)
        {
            // Visit larger block.
            current_block = current_block->larger_block;
            continue;
        }

        break;
    }

    if (current_block->size < size_in_bytes)
    {
        return nullptr; // Block is not large enough.
    }

    // Remove block from tree.
    remove_node_from_tree(current_block);

    if (current_block->size > size_in_bytes)
    {
        // Split block.
    }

    // Add new free block back to tree.

    // Return newly allocated block.

    return nullptr;
}

void *memory_blocks::allocate_next_to(void *address, unsigned size_in_bytes)
{
    auto blocks_needed = size_in_bytes / 4096 + (size_in_bytes % 4096 > 0 ? 1 : 0);

    // Find suitable block CLOSE to address.

    // Remove block from tree.

    // Split block if necessary.

    // Add new free block back to tree.

    // Return newly allocated block.

    return nullptr;
}

void memory_blocks::deallocate(void *address)
{
    // Setup block as a free data structure.

    // Look for spot on tree to add it to.

    // Merge into adjacent block if possible.
}