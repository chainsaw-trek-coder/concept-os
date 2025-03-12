#![no_std]

mod drive;
use drive::DriveUnsafe;

use core::ffi::c_char;

#[panic_handler]
fn panic(_info: &core::panic::PanicInfo) -> ! {
    loop {} // TODO: Write abort code.
}

#[unsafe(no_mangle)]
pub extern "C" fn test() -> i32 {
    return 123456;
}

#[unsafe(no_mangle)]
pub extern "C" fn register_drive(drive: &DriveUnsafe) -> i32 {
    return -1;
}

#[unsafe(no_mangle)]
pub extern "C" fn format_drive(drive: &DriveUnsafe) -> i32 {
    // TODO: Create boot sector here.

    // TODO: Create B+ tree on storage.
    return -1;
}

#[unsafe(no_mangle)]
pub extern "C" fn open(drive: &DriveUnsafe, file_name: *const c_char) -> i32 {
    // TODO: Look for file in file system.
    return -1;
}
