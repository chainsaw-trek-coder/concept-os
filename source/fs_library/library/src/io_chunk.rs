unsafe extern {
    fn alloc_io_chunk(size: usize) -> *mut u8;
    fn dealloc_io_chunk(ptr: *mut u8);
}

#[repr(C)]
#[derive(Copy, Clone)]
pub struct IoChunk {
    data: *mut u8,
    size: usize
}

pub struct IoChunkSafe {
    pub chunk: IoChunk,
    is_mutable: bool
}

impl IoChunkSafe {
    pub fn new(size: usize) -> Self {
        let chunk = unsafe { alloc_io_chunk(size as usize) };

        IoChunkSafe {
            chunk: IoChunk {
                data: chunk,
                size
            },
            is_mutable: true
        }
    }

    pub fn new_from(chunk: &IoChunkSafe) -> Self {
        let new_chunk = unsafe { alloc_io_chunk(chunk.chunk.size) };

        unsafe {
            core::ptr::copy_nonoverlapping(chunk.chunk.data, new_chunk, chunk.chunk.size);
        }

        IoChunkSafe {
            chunk: IoChunk {
                data: new_chunk,
                size: chunk.chunk.size
            },
            is_mutable: true
        }
    }

    pub fn from_raw(chunk: IoChunk) -> Self {
        IoChunkSafe {
            chunk: chunk,
            is_mutable: false
        }
    }

    pub fn get_data(&self) -> Option<&[u8]> {
        if self.chunk.data.is_null() {
            None
        } else {
            unsafe { Some(core::slice::from_raw_parts(self.chunk.data, self.chunk.size as usize)) }
        }
    }

    pub fn get_data_as<T>(&self) -> Option<&[T]> {
        if self.chunk.data.is_null() {
            None
        } else {
            unsafe { Some(core::slice::from_raw_parts(self.chunk.data as *const T, self.chunk.size / core::mem::size_of::<T>() as usize) ) }
        }
    }

    pub fn get_data_mut(&mut self) -> Option<&mut [u8]> {
        if self.chunk.data.is_null() || !self.is_mutable {
            None
        } else {
            unsafe { Some(core::slice::from_raw_parts_mut(self.chunk.data, self.chunk.size as usize)) }
        }
    }

    pub fn get_data_as_mut<T>(&mut self) -> Option<&mut [T]> {
        if self.chunk.data.is_null() || !self.is_mutable {
            None
        } else {
            unsafe { Some(core::slice::from_raw_parts_mut(self.chunk.data as *mut T, self.chunk.size / core::mem::size_of::<T>() as usize)) }
        }
    }
}

impl Drop for IoChunkSafe {
    fn drop(&mut self) {
        unsafe { dealloc_io_chunk(self.chunk.data) };
    }
}