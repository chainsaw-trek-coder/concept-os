#include "memory/mem_allocator.hpp"

void mem_allocator::initialize(void *address, unsigned size, unsigned block_size, bool track_allocated_size)
{
    if (block_size < sizeof(free_block))
    {
        // TODO: Throw an exception.
    }

    if (block_size < sizeof(allocated_block))
    {
        // TODO: Throw an exception.
    }

    free_blocks = reinterpret_cast<free_block *>(address);
    free_blocks_by_address = reinterpret_cast<free_block *>(address);

    this->block_size = block_size;
    this->track_allocated_size = track_allocated_size;
    this->size = size;
    this->free_space = size;

    free_blocks[0].size = size;
    free_blocks[0].predecessor_by_size = nullptr;
    free_blocks[0].predecessor_by_address = nullptr;
    free_blocks[0].smaller_block = nullptr;
    free_blocks[0].larger_block = nullptr;

    free_blocks[0].lower_block = nullptr;
    free_blocks[0].higher_block = nullptr;
}

void mem_allocator::replace_block_at_its_predecessor_by_size(free_block *old_block, free_block *new_block)
{
    if (old_block->predecessor_by_size == nullptr)
        free_blocks = new_block;
    else if (old_block->predecessor_by_size->smaller_block == old_block)
        old_block->predecessor_by_size->smaller_block = new_block;
    else if (old_block->predecessor_by_size->larger_block == old_block)
        old_block->predecessor_by_size->larger_block = new_block;
}

void mem_allocator::replace_block_at_its_predecessor_by_address(free_block *old_block, free_block *new_block)
{
    if (old_block->predecessor_by_address == nullptr)
        free_blocks_by_address = new_block;
    else if (old_block->predecessor_by_address->lower_block == old_block)
        old_block->predecessor_by_address->lower_block = new_block;
    else if (old_block->predecessor_by_address->higher_block == old_block)
        old_block->predecessor_by_address->higher_block = new_block;
}

void mem_allocator::remove_node_from_tree(free_block *block)
{
    remove_node_from_tree_by_size(block);
    remove_node_from_tree_by_address(block);
    block->predecessor_by_size = nullptr;
    block->predecessor_by_address = nullptr;
    block->lower_block = nullptr;
    block->higher_block = nullptr;
    block->smaller_block = nullptr;
    block->larger_block = nullptr;
    free_space -= block->size;
}

void mem_allocator::remove_node_from_tree_by_size(free_block *block)
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

    // block->predecessor_by_size = nullptr; - All tests are passing so not sure if I really need these.
    // block->smaller_block = nullptr;
    // block->larger_block = nullptr;
}

void mem_allocator::remove_node_from_tree_by_address(free_block *block)
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

void mem_allocator::add_node_to_tree(free_block *block)
{
    add_node_to_tree_by_size(block);
    add_node_to_tree_by_address(block);
    free_space += block->size;
}

void mem_allocator::add_node_to_tree_by_size(free_block *block)
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

void mem_allocator::add_node_to_tree_by_address(free_block *block)
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

void *mem_allocator::allocate(unsigned size_in_bytes)
{
    // No more memory...
    if (free_blocks == nullptr)
        return nullptr;

    // Find suitable block.
    auto current_block = free_blocks;

    if (this->track_allocated_size)
    {
        size_in_bytes += sizeof(allocated_block); // Need extra room for tracking information.
    }

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

    // Blocks used will not match exactly the memory requested.
    auto blocks_needed = number_of_blocks(size_in_bytes, this->block_size);                  // An overestimate is needed here.
    auto blocks_in_node = number_of_blocks_contained(current_block->size, this->block_size); // An under estimate is needed here.

    if (blocks_in_node > blocks_needed)
    {
        // Split block.
        auto new_size = (blocks_in_node - blocks_needed) * this->block_size;

        auto &new_block = *reinterpret_cast<free_block *>((reinterpret_cast<char *>(current_block) + (blocks_needed * this->block_size)));
        new_block.size = new_size;
        new_block.predecessor_by_size = nullptr;
        new_block.predecessor_by_address = nullptr;
        new_block.smaller_block = nullptr;
        new_block.larger_block = nullptr;
        new_block.lower_block = nullptr;
        new_block.higher_block = nullptr;

        current_block->size = blocks_needed * this->block_size;

        add_node_to_tree(&new_block);
    }

    if (this->track_allocated_size)
    {
        // Transform block. May not make any changes due to similaritize in structures.
        auto block_size = current_block->size;
        auto _allocated_block = reinterpret_cast<allocated_block *>(current_block);
        _allocated_block->size = block_size;

        // Return just past the header of the newly allocated block.
        return (_allocated_block + 1);
    }
    else
    {
        // Return newly allocated block.
        return current_block;
    }
}

// void *mem_allocator::allocate_next_to(void *address, unsigned size_in_bytes) -- TODO
// {
//     auto blocks_needed = size_in_bytes / this->block_size + (size_in_bytes % this->block_size > 0 ? 1 : 0);

//     // Find suitable block CLOSE to address.

//     // Remove block from tree.

//     // Split block if necessary.

//     // Add new free block back to tree.

//     // Return newly allocated block.

//     return nullptr;
// }

void *mem_allocator::allocate_at(void *address, unsigned size_in_bytes)
{
    // No more memory...
    if (free_blocks == nullptr)
        return nullptr;

    if (this->track_allocated_size)
    {
        size_in_bytes += sizeof(allocated_block); // Need extra room for tracking information.
    }

    // Find suitable block.
    auto current_block = free_blocks_by_address;
    void *upper_bound_of_block = nullptr;

    void *upper_bound_of_allocation = reinterpret_cast<void *>(reinterpret_cast<unsigned char *>(address) + size_in_bytes);

    // Attempt to find a block that contains address first and then check to see if it can contain the section needed.
    while (current_block)
    {
        upper_bound_of_block = reinterpret_cast<void *>(reinterpret_cast<unsigned char *>(current_block) + current_block->size);

        if (current_block < address && current_block->higher_block)
        {
            current_block = current_block->higher_block;
            continue;
        }

        if (current_block > upper_bound_of_allocation && current_block->lower_block)
        {
            current_block = current_block->lower_block;
            continue;
        }

        if (current_block <= address && upper_bound_of_block >= upper_bound_of_allocation)
        {
            // Eureka!!! We found a match.
            break;
        }
        else
        {
            // No match found.
            current_block = nullptr;
        }
    }

    if (current_block == nullptr)
    {
        // No match found.
        return nullptr;
    }

    // Remote node.
    remove_node_from_tree(current_block);

    // Split into 0 - 3 nodes
    auto size_before = reinterpret_cast<unsigned char *>(address) - reinterpret_cast<unsigned char *>(current_block);
    auto size_after = reinterpret_cast<unsigned char *>(upper_bound_of_block) - reinterpret_cast<unsigned char *>(upper_bound_of_allocation);
    auto block_size = current_block->size;

    if (number_of_blocks_contained(size_after, 4096))
    {
        // Split block.
        auto new_size = number_of_blocks_contained(size_after, 4096) * this->block_size;

        auto &new_block = *reinterpret_cast<free_block *>((reinterpret_cast<char *>(current_block) + current_block->size - new_size));
        new_block.size = new_size;
        new_block.predecessor_by_size = nullptr;
        new_block.predecessor_by_address = nullptr;
        new_block.smaller_block = nullptr;
        new_block.larger_block = nullptr;
        new_block.lower_block = nullptr;
        new_block.higher_block = nullptr;

        current_block->size -= new_size;
        block_size = current_block->size;

        add_node_to_tree(&new_block);
    }

    if (number_of_blocks_contained(size_before, 4096) > 0)
    {
        // Split block.
        auto new_size = number_of_blocks_contained(size_before, 4096) * this->block_size;

        auto &new_block = *current_block;
        new_block.size = new_size;
        new_block.predecessor_by_size = nullptr;
        new_block.predecessor_by_address = nullptr;
        new_block.smaller_block = nullptr;
        new_block.larger_block = nullptr;
        new_block.lower_block = nullptr;
        new_block.higher_block = nullptr;

        current_block = reinterpret_cast<free_block *>(reinterpret_cast<unsigned char *>(current_block) + new_size);
        block_size -= new_size;

        add_node_to_tree(&new_block);
    }

    if (this->track_allocated_size)
    {
        // Transform block. May not make any changes due to similaritize in structures.
        auto _allocated_block = reinterpret_cast<allocated_block *>(current_block);
        _allocated_block->size = block_size;

        // Return just past the header of the newly allocated block.
        return (_allocated_block + 1);
    }
    else
    {
        // Return newly allocated block.
        return current_block;
    }
}

void mem_allocator::deallocate(void *address)
{
    if (this->track_allocated_size)
    {
        // Readjust address to the true beginning of the block.
        address = reinterpret_cast<allocated_block *>(address) - 1;
    }

    auto chunk_size = this->track_allocated_size ? reinterpret_cast<allocated_block *>(address)->size : this->block_size;

    // Setup block as a free data structure.
    auto new_block = reinterpret_cast<free_block *>(address);
    new_block->predecessor_by_size = nullptr;
    new_block->predecessor_by_address = nullptr;
    new_block->smaller_block = nullptr;
    new_block->larger_block = nullptr;
    new_block->lower_block = nullptr;
    new_block->higher_block = nullptr;
    new_block->size = chunk_size;

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