extern crate alloc;

use core::pin::Pin;
use core::task::{Context, Poll};
use alloc::sync::Arc;
use core::cell::RefCell;

use crate ::io_chunk::IoChunkSafe;
use crate ::io_chunk::IoChunk;

pub struct PartitionUnsafe {
    pub get_size_async: fn(handle: u64, closure: *const RefCell<dyn FnMut(u64)>),
    pub read_async: fn(handle: u64, offset: u64, size: u64, closure: *const RefCell<dyn FnMut(IoChunk)>),
    pub write_async: fn(handle: u64, io_chunk: IoChunk, offset: u64, size: u64, closure: *const RefCell<dyn FnMut(u64)>),
    pub flush_async: fn(handle: u64, closure: *const RefCell<dyn FnMut(u64)>)
}

impl PartitionUnsafe {

    pub fn clone(&self) -> PartitionUnsafe {
        return PartitionUnsafe {
            get_size_async: self.get_size_async,

            read_async: self.read_async,

            write_async: self.write_async,

            flush_async: self.flush_async
        };
    }
}

pub struct PartitionSafe {
    partition: PartitionUnsafe,
    handle: u64
}

struct SharedState<TResult> {
    result: Option<TResult>,
    waker: Option<core::task::Waker>
}

struct PartitionFuture<TResult> {
    called: bool,
    shared_state: Arc<RefCell<SharedState<TResult>>>,
    on_call: Arc<RefCell<dyn FnMut(Arc<RefCell<dyn FnMut(TResult)>>)>>
}

impl<TResult> PartitionFuture<TResult> {

    pub fn new(on_call: Arc<RefCell<dyn FnMut(Arc<RefCell<dyn FnMut(TResult)>>)>>) -> Self {
        PartitionFuture {
            called: false,
            shared_state: Arc::new(RefCell::new(SharedState {
                result: None,
                waker: None
            })),
            on_call: on_call
        }
    }

}

impl<TResult> Future for PartitionFuture<TResult> 
    where TResult: Copy + 'static
{

    type Output = TResult;

    fn poll(mut self: Pin<&mut Self>, cx: &mut Context) -> Poll<Self::Output> {

        let state = self.shared_state.clone();

        if !state.borrow_mut().waker.is_some() {
            state.borrow_mut().waker = Some(cx.waker().clone());
        }

        if !self.called {
            self.called = true;

            (self.on_call.borrow_mut())(Arc::new(RefCell::new(move |result: TResult| {
                let mut state = state.borrow_mut();
                state.result = Some(result);

                if let Some(waker) = state.waker.take() {
                    waker.wake();
                }
            })));

            return Poll::Pending;
        }

        if let Some(result) = self.shared_state.borrow().result {
            return Poll::Ready(result);
        } else {
            return Poll::Pending;
        }
    }

}

#[unsafe(no_mangle)]
pub extern fn partition_async_return_u64(result: u64, closure: *const RefCell<dyn FnMut(u64)>) {
    unsafe {
        let closure = Arc::from_raw(closure as *const RefCell<dyn FnMut(u64)>);
        (*closure.borrow_mut())(result);
        drop(closure); 
    }
}

#[unsafe(no_mangle)]
pub extern fn partition_async_return_io_chunk(result: IoChunk, closure: *const RefCell<dyn FnMut(IoChunk)>) {
    unsafe {
        let closure = Arc::from_raw(closure as *const RefCell<dyn FnMut(IoChunk)>);
        (*closure.borrow_mut())(result);
        drop(closure); 
    }
}

impl PartitionSafe {

    pub fn new(partition: *mut PartitionUnsafe, handle: u64) -> Self {
        PartitionSafe {
            partition: unsafe { (*partition).clone() },
            handle: handle
        }
    }

    pub async fn get_size(&self) -> u64 {

        let partition = self.partition.clone();
        let handle = self.handle;

        PartitionFuture::new(
            Arc::new(RefCell::new(move |on_complete: Arc<RefCell<dyn FnMut(u64)>>| {
                // Incredibly important that the C++ function calls on_ccomplete
                (partition.get_size_async)(handle, Arc::into_raw(on_complete));
            }))).await
    }

    pub async fn read(&self, offset: u64, size: u64) -> Arc<IoChunkSafe> {

        let partition = self.partition.clone();
        let handle = self.handle;

        let chunk = PartitionFuture::new(
            Arc::new(RefCell::new(move |on_complete: Arc<RefCell<dyn FnMut(IoChunk)>>| {
                // Incredibly important that the C++ function calls on_complete
                (partition.read_async)(handle, offset, size, Arc::into_raw(on_complete));
            }))).await;

        return Arc::new(IoChunkSafe::from_raw(chunk));
    }

    pub async fn write(&self, io_chunk: IoChunkSafe, offset: u64, size: u64) -> u64 {

        let partition = self.partition.clone();
        let handle = self.handle;

        PartitionFuture::new(
            Arc::new(RefCell::new(move |on_complete: Arc<RefCell<dyn FnMut(u64)>>| {
                // Incredibly important that the C++ function calls on_complete
                (partition.write_async)(handle, io_chunk.chunk, offset, size, Arc::into_raw(on_complete));
            }))).await
    }

    pub async fn flush(&self) -> u64 {

        let partition = self.partition.clone();
        let handle = self.handle;

        PartitionFuture::new(
            Arc::new(RefCell::new(move |on_complete: Arc<RefCell<dyn FnMut(u64)>>| {
                // Incredibly important that the C++ function calls on_ccomplete
                (partition.flush_async)(handle, Arc::into_raw(on_complete));
            }))).await
    }

}