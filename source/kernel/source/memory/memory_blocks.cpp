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
    // No successors
    if (block->smaller_block == nullptr && block->larger_block == nullptr)
    {
        replace_block_at_its_predecessor_by_size(block, nullptr);
        return;
    }

    // One successor - part 1.
    if (block->smaller_block == nullptr && block->larger_block != nullptr)
    {
        block->larger_block->predecessor_by_size = block->predecessor_by_size;
        replace_block_at_its_predecessor_by_size(block, block->larger_block);
        return;
    }

    // One successor - part 2.
    if (block->larger_block == nullptr && block->smaller_block != nullptr)
    {
        block->smaller_block->predecessor_by_size = block->predecessor_by_size;
        replace_block_at_its_predecessor_by_size(block, block->smaller_block);
        return;
    }

    // Multiple successors.

    // Find in-order successor...
    auto successor = block;

    successor = successor->larger_block;
    while (successor->smaller_block != nullptr)
        successor = successor->smaller_block;

    // Make sure to remove block from tree.
    if (successor->larger_block != nullptr)
        successor->predecessor_by_size->smaller_block = successor->larger_block;

    // Link it in the same way block is linked.
    replace_block_at_its_predecessor_by_size(block, successor);

    successor->predecessor_by_size = block->predecessor_by_size;

    if (block->smaller_block != successor)
        successor->smaller_block = block->smaller_block;

    if (block->larger_block != successor)
        successor->larger_block = block->larger_block; // Given the algorithm probably should never happen.

    if (successor->larger_block != nullptr)
        successor->larger_block->predecessor_by_size = successor;

    if (successor->smaller_block != nullptr)
        successor->smaller_block->predecessor_by_size = successor;
}

void memory_blocks::remove_node_from_tree_by_address(free_block *block)
{
    // No successors
    if (block->lower_block == nullptr && block->higher_block == nullptr)
    {
        replace_block_at_its_predecessor_by_address(block, nullptr);
        return;
    }

    // One successor - part 1.
    if (block->lower_block == nullptr && block->higher_block != nullptr)
    {
        block->higher_block->predecessor_by_address = block->predecessor_by_address;
        replace_block_at_its_predecessor_by_address(block, block->higher_block);
        return;
    }

    // One successor - part 2.
    if (block->higher_block == nullptr && block->lower_block != nullptr)
    {
        block->lower_block->predecessor_by_address = block->predecessor_by_address;
        replace_block_at_its_predecessor_by_address(block, block->lower_block);
        return;
    }

    // Multiple successors.

    // Find in-order successor...
    auto successor = block;

    successor = successor->higher_block;
    while (successor->lower_block != nullptr)
        successor = successor->lower_block;

    // Make sure to remove block from tree.
    if (successor->higher_block != nullptr)
    {
        if (successor->predecessor_by_address->lower_block == successor)
            successor->predecessor_by_address->lower_block = successor->higher_block;
        else if (successor->predecessor_by_address->higher_block == successor)
            successor->predecessor_by_address->higher_block = successor->higher_block;

        successor->higher_block->predecessor_by_address = successor->predecessor_by_address;
    }

    // Link it in the same way block is linked.
    replace_block_at_its_predecessor_by_address(block, successor);

    successor->predecessor_by_address = block->predecessor_by_address;

    if (block->lower_block != successor)
        successor->lower_block = block->lower_block;

    if (block->higher_block != successor)
        successor->higher_block = block->higher_block; // Given the algorithm probably should never happen.

    if (successor->higher_block != nullptr)
        successor->higher_block->predecessor_by_address = successor;

    if (successor->lower_block != nullptr)
        successor->lower_block->predecessor_by_address = successor;
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
                block->predecessor_by_address = current;
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
                block->predecessor_by_address = current;
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

    if (current_block->size > size_in_bytes)
    {
        // Split block.
        auto blocks_needed = number_of_blocks(size_in_bytes);
        auto blocks_in_node = number_of_blocks(current_block->size);

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

    // TODO: Finish structuring tree by address so that merging is possible.

    // Merge into adjacent block if possible.
}