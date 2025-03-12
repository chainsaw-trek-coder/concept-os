pub struct DriveUnsafe {
    pub get_size: fn(handle: u32) -> u64,
    pub read: fn(handle: u32, buffer: &mut [u8], offset: u64, size: u64) -> u64,
    pub write: fn(handle: u32, buffer: &[u8], offset: u64, size: u64) -> u64,
    pub flush: fn(handle: u32) -> u64
}

pub struct DriveSafe {
    drive: DriveUnsafe
}
