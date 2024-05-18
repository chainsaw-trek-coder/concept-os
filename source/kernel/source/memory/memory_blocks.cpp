#include "memory/memory_blocks.hpp"

void memory_blocks::initialize(void *address, unsigned size)
{
    free_blocks = reinterpret_cast<free_block *>(address);
    free_blocks_by_address = reinterpret_cast<free_block *>(address);

    this->size = size;

    free_blocks[0].size = size;
    free_blocks[0].predecessor_by_size = nullptr;
    free_blocks[0].predecessor_by_address = nullptr;
    free_blocks[0].smaller_block = nullptr;
    free_blocks[0].larger_block = nullptr;

    free_blocks[0].lower_block = nullptr;
    free_blocks[0].higher_block = nullptr;
}

void memory_blocks::replace_block_at_its_predecessor_by_size(free_block *old_block, free_block *new_block)
{
    if (old_block->predecessor_by_size == nullptr)
        free_blocks = new_block;
    else if (old_block->predecessor_by_size->smaller_block == old_block)
        old_block->predecessor_by_size->smaller_block = new_block;
    else if (old_block->predecessor_by_size->larger_block == old_block)
        old_block->predecessor_by_size->larger_block = new_block;
}

void memory_blocks::replace_block_at_its_predecessor_by_address(free_block *old_block, free_block *new_block)
{
    if (old_block->predecessor_by_address == nullptr)
        free_blocks_by_address = new_block;
    else if (old_block->predecessor_by_address->lower_block == old_block)
        old_block->predecessor_by_address->lower_block = new_block;
    else if (old_block->predecessor_by_address->higher_block == old_block)
        old_block->predecessor_by_address->higher_block = new_block;
}

void memory_blocks::remove_node_from_tree(free_block *block)
{
    remove_node_from_tree_by_size(block);
    remove_node_from_tree_by_address(block);
    block->predecessor_by_size = nullptr;
    block->predecessor_by_address = nullptr;
    block->lower_block = nullptr;
    block->higher_block = nullptr;
    block->smaller_block = nullptr;
    block->larger_block = nullptr;
    size -= block->size;
}

void memory_blocks::remove_node_from_tree_by_size(free_block *block)
{
    // Left node goes up...

    if (block->predecessor_by_size)
        replace_block_at_its_predecessor_by_size(block, block->smaller_block);
    else
        this->free_blocks = block->smaller_block;

    if (block->smaller_block)
        block->smaller_block->predecessor_by_size = block->predecessor_by_size;

    // Right node goes back onto tree somehow

    if (block->larger_block)
    {
        block->larger_block->predecessor_by_size = nullptr;
        add_node_to_tree_by_size(block->larger_block); // TODO: Refactor this so that this function doesn't start at the root.
    }

    // block->predecessor_by_size = nullptr;
    // block->smaller_block = nullptr;
    // block->larger_block = nullptr;
}

void memory_blocks::remove_node_from_tree_by_address(free_block *block)
{
    // Left node goes up...

    if (block->predecessor_by_address)
        replace_block_at_its_predecessor_by_address(block, block->lower_block);
    else
        this->free_blocks_by_address = block->lower_block;

    if (block->lower_block)
        block->lower_block->predecessor_by_address = block->predecessor_by_address;

    // Right node goes back onto tree somehow

    if (block->higher_block)
    {
        block->higher_block->predecessor_by_address = nullptr;
        add_node_to_tree_by_address(block->higher_block); // TODO: Refactor this so that this function doesn't start at the root.
    }

    // block->predecessor_by_address = nullptr;
    // block->lower_block = nullptr;
    // block->higher_block = nullptr;
}

void memory_blocks::add_node_to_tree(free_block *block)
{
    add_node_to_tree_by_size(block);
    add_node_to_tree_by_address(block);
    size += block->size;
}

void memory_blocks::add_node_to_tree_by_size(free_block *block)
{
    if (free_blocks == nullptr)
    {
        free_blocks = block;
        block->predecessor_by_size = nullptr;
        return;
    }

    auto current = free_blocks;

    while (true)
    {
        if (block->size <= current->size)
        {
            // Block should go left.
            if (current->smaller_block != nullptr)
            {
                current = current->smaller_block;
                continue;
            }
            else
            {
                current->smaller_block = block;
                block->predecessor_by_size = current;
                break;
            }
        }

        if (block->size > current->size)
        {
            // Block should go right.
            if (current->larger_block != nullptr)
            {
                current = current->larger_block;
                continue;
            }
            else
            {
                current->larger_block = block;
                block->predecessor_by_size = current;
                break;
            }
        }
    }
}

void memory_blocks::add_node_to_tree_by_address(free_block *block)
{
    if (free_blocks_by_address == nullptr)
    {
        free_blocks_by_address = block;
        block->predecessor_by_address = nullptr;
        return;
    }

    auto current = free_blocks_by_address;

    while (true)
    {
        if (block < current)
        {
            // Block should go left.
            if (current->lower_block != nullptr)
            {
                current = current->lower_block;
                continue;
            }
            else
            {
                current->lower_block = block;
                current->lower_block->predecessor_by_address = current;
                break;
            }
        }

        if (block > current)
        {
            // Block should go right.
            if (current->higher_block != nullptr)
            {
                current = current->higher_block;
                continue;
            }
            else
            {
                current->higher_block = block;
                current->higher_block->predecessor_by_address = current;
                break;
            }
        }

        if (block == current)
        {
            // What are we doing here. Block is already on the tree.
            break;
        }
    }
}

void *memory_blocks::allocate(unsigned size_in_bytes)
{
    // No more memory...
    if (free_blocks == nullptr)
        return nullptr;

    // Find suitable block.
    auto current_block = free_blocks;

    while (true)
    {
        // We're using >= to bias the algorithm for selecting leaf blocks. We're going to have to traverse
        // the entire tree anyway to remove the node. Why not make the removal easy.
        if (current_block->smaller_block != nullptr && current_block->smaller_block->size >= size_in_bytes)
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

    remove_node_from_tree(current_block);
    
    auto blocks_needed = number_of_blocks(size_in_bytes);
    auto blocks_in_node = number_of_blocks(current_block->size);
    
    if (blocks_in_node > blocks_needed)
    {
        // Split block.
        auto new_size = (blocks_in_node - blocks_needed) * 4096;

        auto &new_block = *reinterpret_cast<free_block *>((reinterpret_cast<char *>(current_block) + (blocks_needed * 4096)));
        new_block.size = new_size;
        new_block.predecessor_by_size = nullptr;
        new_block.predecessor_by_address = nullptr;
        new_block.smaller_block = nullptr;
        new_block.larger_block = nullptr;
        new_block.lower_block = nullptr;
        new_block.higher_block = nullptr;

        add_node_to_tree(&new_block);
    }

    // Return newly allocated block.
    return current_block;
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
    auto new_block = reinterpret_cast<free_block *>(address);
    new_block->predecessor_by_size = nullptr;
    new_block->predecessor_by_address = nullptr;
    new_block->smaller_block = nullptr;
    new_block->larger_block = nullptr;
    new_block->lower_block = nullptr;
    new_block->higher_block = nullptr;
    new_block->size = 4096;

    // Merge new bock into adjacent blocks...
    auto current = free_blocks_by_address;

    while (current)
    {
        // If we are at block that is adjacent... merge.

        // Check if current is to the left of the new block.
        if ((reinterpret_cast<unsigned char *>(current) + current->size) == reinterpret_cast<unsigned char *>(new_block))
        {
            // Merge...
            remove_node_from_tree(current);
            current->size += new_block->size;
            new_block = current;
            current = free_blocks_by_address; // Reset search...
            continue;
        }

        // Check if current is to the right of the new block.
        if ((reinterpret_cast<unsigned char *>(new_block) + new_block->size) == reinterpret_cast<unsigned char *>(current))
        {
            // Merge...
            remove_node_from_tree(current);
            new_block->size += current->size;
            current = free_blocks_by_address; // Reset search...
            continue;
        }

        if (current < new_block)
        {
            current = current->higher_block;
            continue;
        }

        if (current > new_block)
        {
            current = current->lower_block;
            continue;
        }
    }

    add_node_to_tree(new_block);
}