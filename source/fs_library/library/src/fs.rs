extern crate alloc;
use alloc::sync::Arc;

use crate ::drive::PartitionSafe;

// File System Structures
pub struct FS {
    // Reference to the partition
    partition: Arc<PartitionSafe>,

    // Write cache

    // Read cache
}

#[repr(C)]
pub struct FSHeader {
    files: u64,
    free_blocks: u64,
    journal_address: u64,
    journal_size: u64,
    current_transaction_size: u64,
    padding: [u8; 512 - 40],
}

#[repr(C)]
pub struct FreeBlock {
    size: u64,

    predecessor_by_size: u64,

    smaller_block: u64,
    larger_block: u64,
}

impl FS {
    pub fn new(partition: Arc<PartitionSafe>) -> Self {
        FS {
            partition,
        }
    }

    pub async fn read_partition(offset: u64, size: u64) -> i64 {

        // I need a concept of an I/O chunk in both C and C++.

        return -1;
    }

    pub async fn allocate_space(file_id: u64, size: u64) -> i64 {

        

        return -1;
    }
}