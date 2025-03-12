#ifndef a832fa80_07fa_4f65_a331_0af0942c7980
#define a832fa80_07fa_4f65_a331_0af0942c7980

#include "bytes.hpp"

struct __attribute__((packed)) free_block
{
    unsigned size;

    free_block *predecessor_by_size;
    free_block *predecessor_by_address;

    free_block *smaller_block;
    free_block *larger_block;

    free_block *lower_block;
    free_block *higher_block;

    // Disabling so that we can make these block sizes variable.
    // private:
    //     unsigned char padding[4096 - 28];
};

struct __attribute__((packed)) allocated_block
{
    unsigned size;
};

struct mem_allocator
{
    free_block *free_blocks;
    free_block *free_blocks_by_address;
    unsigned block_size;
    unsigned size;
    unsigned free_space; // TODO: Update tests to validate this value.
    bool track_allocated_size;

    /// @brief Set starting address and size of total available memory.
    /// @param address Starting address of available memory.
    /// @param size Total size of available memory.
    /// @param block_size Total size sof blocks
    /// @param track_allocated_size Allocated blocks should track their size. Useful for deallocation.
    void initialize(void *address, unsigned size, unsigned block_size, bool track_allocated_size);

    /// @brief Allocate blocks anywhere in memory.
    /// @param size_in_bytes Memory requirements of total blocks.
    /// @return Address of allocated pages.
    void *allocate(unsigned size_in_bytes);

    /// @brief Allocates blocks anywhere in memory with bounds included.
    /// @param size_in_bytes Memory requirements of total blocks.
    /// @return Byte object containing address and size of block.
    os::bytes allocate_protected(unsigned size_in_bytes) { return os::bytes(allocate(size_in_bytes), size_in_bytes); }

    /// @brief Allocate blocks as close to an address as possible.
    /// @param address Address to allocate close to.
    /// @param size_in_bytes Memory requirement of total blocks.
    /// @return Address of allocated blocks.
    // void *allocate_next_to(void *address, unsigned size_in_bytes); -- TODO

    /// @brief Allocates blocks at a specific address if possible.
    /// @param address Address to allocate at.
    /// @param size_in_bytes Memory requirement of total blocks.
    /// @return Address of the allocated blocks
    void *allocate_at(void *address, unsigned size_in_bytes);

    /// @brief Allocates blocks at a specific address if possible and includes bounds.
    /// @param address Address to allocate at.
    /// @param size_in_bytes Memory requirement of total blocks.
    /// @return Address of allocated blocks with bounds included.
     os::bytes allocate_at_protected(void *address, unsigned size_in_bytes) { return os::bytes(allocate_at(address, size_in_bytes), size_in_bytes); }

    /// @brief Deallocate a block.
    /// @param address Address of block to deallocate.
    void deallocate(void *address);

private:
    void replace_block_at_its_predecessor_by_size(free_block *old_block, free_block *new_block);
    void replace_block_at_its_predecessor_by_address(free_block *old_block, free_block *new_block);
    void remove_node_from_tree(free_block *block);
    void remove_node_from_tree_by_size(free_block *block);
    void remove_node_from_tree_by_address(free_block *block);
    void add_node_to_tree(free_block *block);
    void add_node_to_tree_by_size(free_block *block);
    void add_node_to_tree_by_address(free_block *block);
};

inline unsigned number_of_blocks(unsigned size_in_bytes, unsigned block_size)
{
    return (size_in_bytes / block_size) + (size_in_bytes % block_size > 0 ? 1 : 0);
}

inline unsigned number_of_blocks_contained(unsigned size_in_bytes, unsigned block_size)
{
    return number_of_blocks(size_in_bytes - (size_in_bytes % block_size), block_size);
}

#endif
// extern memory_blocks system_memory_blocks;