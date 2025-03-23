pub struct PartitionUnsafe {
    pub get_size: fn(handle: u64) -> u64,
    pub get_size_async: fn(handle: u64, closure: *mut dyn Fn(u64) -> u64),

    pub read: fn(handle: u64, buffer: *mut u8, offset: u64, size: u64) -> u64,
    pub read_async: fn(handle: u64, buffer: *mut u8, offset: u64, size: u64, closure: *mut dyn Fn(u64) -> u64),

    pub write: fn(handle: u64, buffer: *const u8, offset: u64, size: u64) -> u64,
    pub write_async: fn(handle: u64, buffer: *const u8, offset: u64, size: u64, closure: *mut dyn Fn(u64) -> u64),

    pub flush: fn(handle: u64) -> u64
    pub flush_async: fn(handle: u64, closure: *mut dyn Fn(u64) -> u64)
}

// pub struct PartitionSafe {
//     drive: PartitionUnsafe
// }
