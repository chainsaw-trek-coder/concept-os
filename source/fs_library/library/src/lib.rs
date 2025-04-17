#![no_std]

extern crate alloc;

mod drive;
use drive::PartitionUnsafe;
use drive::PartitionSafe;

use core::ffi::c_char;
use core::alloc::{GlobalAlloc, Layout};
use alloc::boxed::Box;
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
    padding: [u8; 512 - 40]
}

#[repr(C)]
struct FreeBlock {
    size: u64,

    predecessor_by_size: u64,

    smaller_block: u64,
    larger_block: u64,
}

struct FileHandle {
    partition: *mut PartitionSafe,
    handle_id: i64,
    file_id: u64,
    is_new: bool,
    can_write: bool,
    position: u64,
}

impl FileHandle {
    fn new(partition: *mut PartitionSafe,  handle_id: i64, file_id: u64, is_new: bool) -> Self {
        FileHandle {
            partition: partition,
            handle_id,
            file_id,
            is_new,
            can_write: false,
            position: 0
        }
    }
}

impl Copy for FileHandle {}
impl Clone for FileHandle {
    fn clone(&self) -> Self {
        FileHandle {
            partition: self.partition,
            handle_id: self.handle_id,
            file_id: self.file_id,
            is_new: self.is_new,
            can_write: self.can_write,
            position: self.position
        }
    }
}

// Global File Handle List
static mut FILE_HANDLE_LIST: [FileHandle; 256] = [
    FileHandle {
        partition: core::ptr::null_mut(),
        handle_id: -1,
        file_id: 0,
        is_new: false,
        can_write: false,
        position: 0
    };
    256
];

// Create file handle
fn create_file_handle(partition: *mut PartitionSafe, file_id: u64, is_new: bool) -> i64 {

    unsafe {
        for i in 0..256 {
            if FILE_HANDLE_LIST[i].handle_id == -1 {
                FILE_HANDLE_LIST[i] = FileHandle::new(partition, i as i64, file_id, is_new);
                return i as i64;
            }
        }
        return -1;
    }
}

// Rust Interface

pub async fn format_partition_async(partition: &PartitionSafe, handle: u64) -> i64 {

    let journal_size = DEFAULT_JOURNAL_SIZE;
    let partition_size = partition.get_size(handle).await;

    // Create header block is BLOCK_SIZE bytes.
    let header = FSHeader {
        files: 0,
        free_blocks: BLOCK_SIZE, // First block after header is a free block.
        journal_address: partition_size - (BLOCK_SIZE * journal_size),
        journal_size,
        current_transaction_size: 0,
        padding: [0; (BLOCK_SIZE - 40) as usize]
    };

    // Create free tree on storage.
    let free_block = FreeBlock {
        size: partition_size - (size_of::<FSHeader>() as u64) - (BLOCK_SIZE * journal_size),
        predecessor_by_size: 0,
        smaller_block: 0,
        larger_block: 0
    };

    // Write header to storage.
    partition.write(handle, (&header as *const FSHeader) as *const u8, 0, BLOCK_SIZE).await;

    // Write free block to storage.
    partition.write(handle, (&free_block as *const FreeBlock) as *const u8, BLOCK_SIZE, BLOCK_SIZE).await;

    // Flush
    partition.flush(handle).await;

    return 0;    

}

pub async fn create_anonymous_file_async(partition: &PartitionSafe, handle: u64) -> i64 {
    let file_handle = create_file_handle(partition, true);

    if file_handle == -1 {
        return -1;
    }

    return file_handle;
}

// C/C++ Interface
fn await_async(future: impl Future<Output = i64>) -> i64
{
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
    return await_async(format_partition_async(&PartitionSafe::new(partition), handle));
}

#[unsafe(no_mangle)]
pub extern fn create_anonymous_file(partition: *mut PartitionUnsafe, handle: u64) -> i64 {
    return await_async(create_anonymous_file_async(&PartitionSafe::new(partition), handle));
}

// TODO: Create file function.
// #[unsafe(no_mangle)]
// pub extern fn create_file(partition: *mut PartitionUnsafe, handle: u64, path: *const c_char, name: *const c_char) -> i32 {
//     return -1;
// }

// TODO: Write to file function.

// TODO: Close file function.

// #[unsafe(no_mangle)]
// pub extern "C" fn open(_partition: &PartitionUnsafe, file_name: *const c_char) -> i32 {
//     // TODO: Look for file in file system.
//     return -1;
// }
