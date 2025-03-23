#![no_std]

mod drive;
use drive::PartitionUnsafe;

use core::ffi::c_char;

const BLOCK_SIZE: u64 = 512;
const DEFAULT_JOURNAL_SIZE: u64 = 16;

#[repr(C)]
struct FSHeader {
    files: u64,
    free_blocks: u64,
    journal_address: u64,
    journal_size: u64,
    current_transaction_size: u64,
    padding: [u8; 512 - 40]
}

#[repr(C)]
struct FreeBlock {
    size: u64,

    predecessor_by_size: u64,

    smaller_block: u64,
    larger_block: u64,

}

#[panic_handler]
fn panic(_info: &core::panic::PanicInfo) -> ! {
    loop {} // TODO: Write abort code.
}

#[unsafe(no_mangle)]
pub extern "C" fn call_closure_one_param(closure *mut dyn Fn(u64), param: u64) {
    unsafe {
        (*closure)(param);
    }
}

#[unsafe(no_mangle)]
pub extern "C" fn register_partition(_partition: &PartitionUnsafe) -> i32 {
    return -1;
}

#[unsafe(no_mangle)]
pub extern "C" fn format_partition(_partition: &PartitionUnsafe, handle: u64) -> i32 {

    let _journal_size = DEFAULT_JOURNAL_SIZE;
    let _partition_size = (_partition.get_size)(handle);

    // Create header block is BLOCK_SIZE bytes.
    let _header = FSHeader {
        files: 0,
        free_blocks: BLOCK_SIZE, // First block after header is a free block.
        journal_address: _partition_size - (BLOCK_SIZE * _journal_size),
        journal_size: _journal_size,
        current_transaction_size: 0,
        padding: [0; (BLOCK_SIZE - 40) as usize]
    };

    // Create free tree on storage.
    let _free_block = FreeBlock {
        size: _partition_size - (size_of::<FSHeader>() as u64) - (BLOCK_SIZE * _journal_size),
        predecessor_by_size: 0,
        smaller_block: 0,
        larger_block: 0
    };

    // Write header to storage.
    (_partition.write)(handle, (&_header as *const FSHeader) as *const u8, 0, BLOCK_SIZE);

    // Write free block to storage.
    (_partition.write)(handle, (&_free_block as *const FreeBlock) as *const u8, BLOCK_SIZE, BLOCK_SIZE);

    // Flush
    (_partition.flush)(handle);
    
    return 0;
}

#[unsafe(no_mangle)]
pub extern "C" fn open(_partition: &PartitionUnsafe, _file_name: *const c_char) -> i32 {
    // TODO: Look for file in file system.
    return -1;
}
