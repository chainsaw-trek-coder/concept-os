struct __attribute__((packed)) free_block
{
    unsigned size;

    free_block *predecessor_by_size;
    free_block *predecessor_by_address;

    free_block *smaller_block;
    free_block *larger_block;

    free_block *lower_block;
    free_block *higher_block;

private:
    unsigned char padding[4096 - 28];
};

struct memory_blocks
{
    free_block *free_blocks;
    free_block *free_blocks_by_address;
    unsigned size;

    /// @brief Set starting address and size of total available memory.
    /// @param address Starting address of available memory.
    /// @param size Total size of available memory.
    void initialize(void *address, unsigned size);

    /// @brief Allocate blocks anywhere in memory.
    /// @param size_in_bytes Memory requirements of total blocks.
    /// @return Address of allocated pages.
    void *allocate(unsigned size_in_bytes);

    /// @brief Allocate blocks as close to an address as possible.
    /// @param address Address to allocate close to.
    /// @param size_in_bytes Memory requirement of total blocks.
    /// @return Address of allocated blocks.
    void *allocate_next_to(void *address, unsigned size_in_bytes);

    /// @brief Deallocate a block.
    /// @param address Address of block to deallocate.
    void deallocate(void *address);

private:
    unsigned number_of_blocks(unsigned size_in_bytes);
    void replace_block_at_its_predecessor_by_size(free_block *old_block, free_block *new_block);
    void replace_block_at_its_predecessor_by_address(free_block *old_block, free_block *new_block);
    void remove_node_from_tree(free_block *block);
    void remove_node_from_tree_by_size(free_block *block);
    void remove_node_from_tree_by_address(free_block *block);
    void add_node_to_tree(free_block *block);
    void add_node_to_tree_by_size(free_block *block);
    void add_node_to_tree_by_address(free_block *block);
};

inline unsigned memory_blocks::number_of_blocks(unsigned size_in_bytes)
{
    return (size_in_bytes / 4096) + (size_in_bytes % 4096 > 0 ? 1 : 0);
}

extern memory_blocks system_memory_blocks;