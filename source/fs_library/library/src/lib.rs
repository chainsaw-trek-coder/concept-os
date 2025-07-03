#![no_std]

extern crate alloc;

mod drive;
mod fs;
mod io_chunk;

use drive::PartitionSafe;
use drive::PartitionUnsafe;
use io_chunk::IoChunkSafe;

use alloc::boxed::Box;
use core::alloc::{GlobalAlloc, Layout};
use core::mem::{align_of, size_of};
use core::task::{Context, RawWaker, RawWakerVTable, Waker};

// Constants
const BLOCK_SIZE: u64 = 512;
const DEFAULT_JOURNAL_SIZE: u64 = 16;

// Basic Rust Plumbing
#[panic_handler]
fn panic(_info: &core::panic::PanicInfo) -> ! {
    loop {} // TODO: Write abort code.
}

#[unsafe(no_mangle)]
pub extern "C" fn rust_eh_personality() {}

struct CPPAllocator;

unsafe extern "C" {
    fn alloc_for_rust(size: usize) -> *mut u8;
    fn dealloc_for_rust(ptr: *mut u8);
}

unsafe impl GlobalAlloc for CPPAllocator {
    unsafe fn alloc(&self, layout: Layout) -> *mut u8 {
        let size = layout.size();

        unsafe {
            let ptr = alloc_for_rust(size);
            return ptr as *mut u8;
        }
        // if ptr.is_null() {
        //     core::alloc::handle_alloc_error(layout);
        // }
    }

    unsafe fn dealloc(&self, ptr: *mut u8, _layout: Layout) {
        unsafe {
            dealloc_for_rust(ptr);
        }
    }
}

#[global_allocator]
static GLOBAL: CPPAllocator = CPPAllocator;

// Async support

fn create_no_op_waker() -> Waker {
    fn clone(data: *const ()) -> RawWaker {
        RawWaker::new(data, &VTABLE)
    }

    fn wake(_data: *const ()) {
        // Do nothing...
    }

    fn wake_by_ref(_data: *const ()) {
        // Do nothing...
    }

    fn drop(_: *const ()) {}

    static VTABLE: RawWakerVTable = RawWakerVTable::new(clone, wake, wake_by_ref, drop);

    // Pass null pointer as data, since we don't need to store any data in the waker.
    // In a real implementation, you would pass a pointer to your data here.
    let data: *const () = core::ptr::null();
    let raw_waker = RawWaker::new(data, &VTABLE);
    unsafe { Waker::from_raw(raw_waker) }
}

// File System Structures
#[repr(C)]
struct FSHeader {
    files: u64,
    free_blocks: u64,
    journal_address: u64,
    journal_size: u64,
    current_transaction_size: u64,
    padding: [u8; 512 - 40],
}

#[repr(C)]
struct FreeBlock {
    size: u64,

    predecessor_by_size: u64,

    smaller_block: u64,
    larger_block: u64,
}

struct FileHandle {
    partition_id: i64,
    handle_id: i64,
    file_id: u64,
    is_new: bool,
    can_write: bool,
    position: u64,
}

impl FileHandle {
    fn new(partition_id: i64, handle_id: i64, file_id: u64, is_new: bool) -> Self {
        FileHandle {
            partition_id: partition_id,
            handle_id,
            file_id,
            is_new,
            can_write: false,
            position: 0,
        }
    }
}

impl Copy for FileHandle {}
impl Clone for FileHandle {
    fn clone(&self) -> Self {
        FileHandle {
            partition_id: self.partition_id,
            handle_id: self.handle_id,
            file_id: self.file_id,
            is_new: self.is_new,
            can_write: self.can_write,
            position: self.position,
        }
    }
}

// Global Partition List
static mut PARTITION_LIST: [Option<PartitionSafe>; 256] = [const { None }; 256];

// Global File Handle List
static mut FILE_HANDLE_LIST: [Option<FileHandle>; 256] = [const { None }; 256];

// Internal Functions
fn create_file_handle(partition_id: i64, file_id: u64, is_new: bool) -> i64 {
    unsafe {
        for i in 0..256 {
            if FILE_HANDLE_LIST[i].is_none() {
                FILE_HANDLE_LIST[i] =
                    Some(FileHandle::new(partition_id, i as i64, file_id, is_new));
                return i as i64;
            }
        }
    }
    return -1;
}

// Created with CoPilot.
fn bytes_to_struct<'a, T>(bytes: &'a [u8]) -> Option<&'a T> {
    if bytes.len() < size_of::<T>() {
        return None;
    }
    let ptr = bytes.as_ptr();
    if ptr as usize % align_of::<T>() != 0 {
        return None; // Not properly aligned
    }
    // SAFETY: The caller must ensure the bytes are valid for T
    Some(unsafe { &*(ptr as *const T) })
}

async fn add_free_block_to_tree_async(partition_id: i64, block_address: u64, size: u64) -> i64 {
    let partition = unsafe { PARTITION_LIST[partition_id as usize].as_ref().unwrap() };

    // Read header from partition.
    let header_chunk = partition.read(0, 512).await;
    let header = &header_chunk.get_data_as::<FSHeader>().unwrap()[0];

    if header.free_blocks == 0 {
        // No free blocks, this is the first one.
        let mut new_free_block_chunk = IoChunkSafe::new(BLOCK_SIZE as usize);
        
        let free_block = &mut new_free_block_chunk.get_data_as_mut::<FreeBlock>().unwrap()[0];
        free_block.size = size;
        free_block.predecessor_by_size = 0;
        free_block.smaller_block = 0;
        free_block.larger_block = 0;
        
        partition.write(new_free_block_chunk, block_address, BLOCK_SIZE).await;

        let mut mutable_header_chunk = IoChunkSafe::new_from(header_chunk.as_ref());
        let header = &mut mutable_header_chunk.get_data_as_mut::<FSHeader>().unwrap()[0];
        header.free_blocks = block_address;
        partition.write(mutable_header_chunk, 0, BLOCK_SIZE).await;
        return 0;
    }

    // Read first free block from partition.
    let mut current_block_address = header.free_blocks;
    let mut current_block_chunk = partition.read(current_block_address, BLOCK_SIZE).await;

    // Find location to put block
    loop {
        let current_block = &current_block_chunk.get_data_as::<FreeBlock>().unwrap()[0];

        if size < current_block.size && current_block.smaller_block != 0 {
            let smaller_block_chunk = partition
                .read(current_block.smaller_block, BLOCK_SIZE)
                .await;

            let smaller_block = &smaller_block_chunk.get_data_as::<FreeBlock>().unwrap()[0];

            if smaller_block.size >= size {
                current_block_address = current_block.smaller_block;
                current_block_chunk = smaller_block_chunk;
                continue;
            }
        }
        if size >= current_block.size && current_block.larger_block != 0 {
            let larger_block_chunk = partition.read(current_block.larger_block, BLOCK_SIZE).await;
            current_block_address = current_block.larger_block;
            current_block_chunk = larger_block_chunk;
            continue;
        }
        break;
    }

    let current_block = &current_block_chunk.get_data_as::<FreeBlock>().unwrap()[0];

    if size < current_block.size {

        // Insert new block before current block.
        let mut new_free_block_chunk = IoChunkSafe::new(BLOCK_SIZE as usize);
        
        let free_block = &mut new_free_block_chunk.get_data_as_mut::<FreeBlock>().unwrap()[0];
        free_block.size = size;
        free_block.predecessor_by_size = current_block_address;
        free_block.smaller_block = 0;
        partition.write(new_free_block_chunk, block_address, BLOCK_SIZE).await;

        // Update current block's predecessor to point to the new block.
        let mut mutable_current_chunk = IoChunkSafe::new_from(current_block_chunk.as_ref());
        let current = &mut mutable_current_chunk.get_data_as_mut::<FreeBlock>().unwrap()[0];
        current.smaller_block = block_address;
        partition.write(mutable_current_chunk, current_block_address, BLOCK_SIZE).await;

        return 0; // Successfully added new free block.
    }

    if size >= current_block.size {

        // Insert new block after current block.
        let mut new_free_block_chunk = IoChunkSafe::new(BLOCK_SIZE as usize);
        
        let free_block = &mut new_free_block_chunk.get_data_as_mut::<FreeBlock>().unwrap()[0];
        free_block.size = size;
        free_block.predecessor_by_size = current_block_address;
        free_block.smaller_block = 0;
        partition.write(new_free_block_chunk, block_address, BLOCK_SIZE).await;

        // Update current block's successor to point to the new block.
        let mut mutable_current_chunk = IoChunkSafe::new_from(current_block_chunk.as_ref());
        let current = &mut mutable_current_chunk.get_data_as_mut::<FreeBlock>().unwrap()[0];
        current.larger_block = block_address;
        partition.write(mutable_current_chunk, current_block_address, BLOCK_SIZE).await;

        return 0; // Successfully added new free block.
    }

    return -1;
}

async fn remove_free_block_from_tree_async(partition_id: i64, block_address: u64) -> i64 {
    
    let partition = unsafe { PARTITION_LIST[partition_id as usize].as_ref().unwrap() };

    // Read free block from partition.
    let free_block_chunk = partition.read(block_address, BLOCK_SIZE).await;
    let free_block = &free_block_chunk.get_data_as::<FreeBlock>().unwrap()[0];

    if free_block.predecessor_by_size == 0 {

        let header_chunk = partition.read(0, 512).await;
        let mut mutable_header_chunk = IoChunkSafe::new_from(header_chunk.as_ref());
        let header = &mut mutable_header_chunk.get_data_as_mut::<FSHeader>().unwrap()[0];

        header.free_blocks = 0;

        partition.write(mutable_header_chunk, 0, BLOCK_SIZE).await;
        return 0; // No more free blocks.
    }

    let predecessor_block_chunk = partition.read(free_block.predecessor_by_size, BLOCK_SIZE).await;
    let mut mutable_predecessor_block_chunk = IoChunkSafe::new_from(predecessor_block_chunk.as_ref());
    let predecessor_block = &mut mutable_predecessor_block_chunk.get_data_as_mut::<FreeBlock>().unwrap()[0];

    if predecessor_block.smaller_block == block_address {
        predecessor_block.smaller_block = 0;
    } else if predecessor_block.larger_block == block_address {
        predecessor_block.larger_block = 0;
    } else {
        // This should never happen, but just in case.
        return -1;
    }

    partition.write(mutable_predecessor_block_chunk, free_block.predecessor_by_size, BLOCK_SIZE).await;
    return 0;
}

struct AllocateSpaceResult {
    address: u64,
    size: u64,
}

async fn allocate_space_for_file_async(partition_id: i64, file_id: u64, size: u64) -> Result<AllocateSpaceResult, i64> {
    let partition = unsafe { PARTITION_LIST[partition_id as usize].as_ref().unwrap() };

    // Read header from partition.
    let header_chunk = partition.read(0, 512).await;
    let header = &header_chunk.get_data_as::<FSHeader>().unwrap()[0];

    if header.free_blocks == 0 {
        // No free blocks available.
        return Err(-1);
    }

    // Read first free block from partition.
    let mut current_block_address = header.free_blocks;
    let mut current_block_chunk = partition.read(current_block_address, BLOCK_SIZE).await;

    // Find free block of sufficient size.
    loop {
        let current_block = &current_block_chunk.get_data_as::<FreeBlock>().unwrap()[0];

        if current_block.smaller_block != 0 {
            let smaller_block_chunk = partition
                .read(current_block.smaller_block, BLOCK_SIZE)
                .await;

            let smaller_block = &smaller_block_chunk.get_data_as::<FreeBlock>().unwrap()[0];

            if smaller_block.size >= size {
                current_block_address = current_block.smaller_block;
                current_block_chunk = smaller_block_chunk;
                continue;
            }
        }

        if current_block.larger_block != 0 && current_block.size < size {
            let larger_block_chunk = partition.read(current_block.larger_block, BLOCK_SIZE).await;
            current_block_address = current_block.larger_block;
            current_block_chunk = larger_block_chunk;
            continue;
        }

        break;
    }

    let current_block = &current_block_chunk.get_data_as::<FreeBlock>().unwrap()[0];

    // Return block regardless of size.
    // If the block is too small, another call will allocate more space.
    //if current_block.size < size {
    //    // No free block of sufficient size found.
    //    return Err(-1);
    //}

    if current_block.smaller_block != 0 {
        // Remove smaller block from the free block tree.
        let remove_result = remove_free_block_from_tree_async(partition_id, current_block.smaller_block).await;
        if remove_result != 0 {
            return Err(-1); // Error removing block from tree.
        }
    }

    if current_block.larger_block != 0 {
        // Remove larger block from the free block tree.
        let remove_result = remove_free_block_from_tree_async(partition_id, current_block.larger_block).await;
        if remove_result != 0 {
            return Err(-1); // Error removing block from tree.
        }
    }

    remove_free_block_from_tree_async(partition_id, current_block_address).await;

    if current_block.smaller_block != 0 {
        // Update the smaller block's predecessor to point to the current block's predecessor.
        let smaller_block_chunk = partition.read(current_block.smaller_block, BLOCK_SIZE).await;
        let smaller_block = &smaller_block_chunk.get_data_as::<FreeBlock>().unwrap()[0];
        add_free_block_to_tree_async(partition_id, current_block.smaller_block, smaller_block.size).await;
    }

    if current_block.larger_block != 0 {
        // Update the larger block's predecessor to point to the current block's predecessor.
        let larger_block_chunk = partition.read(current_block.larger_block, BLOCK_SIZE).await;
        let larger_block = &larger_block_chunk.get_data_as::<FreeBlock>().unwrap()[0];
        add_free_block_to_tree_async(partition_id, current_block.larger_block, larger_block.size).await;
    }

    let allocated_size = if size % BLOCK_SIZE == 0 {
        size
    } else {
        (size / BLOCK_SIZE + 1) * BLOCK_SIZE
    };

    if current_block.size > allocated_size {
        // Split the block if it's larger than the requested size.
        let remaining_size = current_block.size - allocated_size;

        // Create a new free block for the remaining space rounded to the next block size.
        let new_free_block_address = current_block_address + allocated_size;
        let add_result = add_free_block_to_tree_async(partition_id, new_free_block_address, remaining_size).await;
        if add_result != 0 {
            return Err(-1); // Error adding new free block.
        }
    }

    return Ok(AllocateSpaceResult {
        address: current_block_address,
        size: allocated_size,
    });
}

// Rust Interface

pub async fn format_partition_async(partition: &PartitionSafe) -> i64 {
    let journal_size = DEFAULT_JOURNAL_SIZE;
    let partition_size = partition.get_size().await;

    // Create header block is BLOCK_SIZE bytes.
    let header = FSHeader {
        files: 0,
        free_blocks: BLOCK_SIZE, // First block after header is a free block.
        journal_address: partition_size - (BLOCK_SIZE * journal_size),
        journal_size,
        current_transaction_size: 0,
        padding: [0; (BLOCK_SIZE - 40) as usize],
    };

    // Create free tree on storage.
    let free_block = FreeBlock {
        size: partition_size - (size_of::<FSHeader>() as u64) - (BLOCK_SIZE * journal_size),
        predecessor_by_size: 0,
        smaller_block: 0,
        larger_block: 0,
    };

    // Write header to storage.
    {
        let mut header_chunk: IoChunkSafe = IoChunkSafe::new(BLOCK_SIZE as usize);
        let chunk_data = header_chunk.get_data_mut().unwrap().as_mut_ptr() as *mut FSHeader;

        unsafe {
            core::ptr::copy_nonoverlapping(&header as *const FSHeader, chunk_data, 1);
        }
        partition.write(header_chunk, 0, BLOCK_SIZE).await;
    }

    // Write free block to storage.
    {
        let mut free_chunk: IoChunkSafe = IoChunkSafe::new(BLOCK_SIZE as usize);
        let free_block_ptr = free_chunk.get_data_mut().unwrap().as_mut_ptr() as *mut FreeBlock;
        unsafe {
            core::ptr::copy_nonoverlapping(&free_block as *const FreeBlock, free_block_ptr, 1);
        }
        partition.write(free_chunk, BLOCK_SIZE, BLOCK_SIZE).await;
    }

    // Flush
    partition.flush().await;

    return 0;
}

pub async fn create_anonymous_file_async(partition_id: i64) -> i64 {
    let file_handle = create_file_handle(partition_id, 0, true);

    if file_handle == -1 {
        return -1;
    }

    return file_handle;
}

pub async fn write_file_async(
    file_handle: i64,
    io_chunk: IoChunkSafe,
    offset: u64,
    size: u64,
) -> i64 {
    let partition_id = unsafe {
        FILE_HANDLE_LIST[file_handle as usize]
            .as_ref()
            .unwrap()
            .partition_id
    };
    let partition = unsafe { PARTITION_LIST[partition_id as usize].as_ref().unwrap() };

    // TODO: Allocate space for file in partition.

    return -1;
}

// C/C++ Interface
fn await_async(future: impl Future<Output = i64>) -> i64 {
    let waker = create_no_op_waker();
    let mut context = Context::from_waker(&waker);

    let mut f = Box::pin(future);

    loop {
        let result = f.as_mut().poll(&mut context);

        match result {
            core::task::Poll::Ready(result) => {
                return result;
            }
            core::task::Poll::Pending => {
                // Do nothing, wait for the async function to complete.
            }
        }
    }
}

#[unsafe(no_mangle)]
pub extern "C" fn format_partition(partition: *mut PartitionUnsafe, handle: u64) -> i64 {
    return await_async(format_partition_async(&PartitionSafe::new(
        partition, handle,
    )));
}

#[unsafe(no_mangle)]
pub extern "C" fn register_partition(partition: *mut PartitionUnsafe, handle: u64) -> i64 {
    unsafe {
        for i in 0..256 {
            if PARTITION_LIST[i].is_none() {
                PARTITION_LIST[i] = Some(PartitionSafe::new(partition, handle));
                return i as i64;
            }
        }
    }
    return -1;
}

#[unsafe(no_mangle)]
pub extern "C" fn create_anonymous_file(partition_id: i64) -> i64 {
    return await_async(create_anonymous_file_async(partition_id));
}
