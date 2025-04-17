extern crate alloc;

use core::pin::Pin;
use core::task::{Context, Poll};
use alloc::sync::Arc;
use core::cell::RefCell;

pub struct PartitionUnsafe {
    // pub get_size: fn(handle: u64) -> u64,
    pub get_size_async: fn(handle: u64, closure: *const RefCell<dyn FnMut(u64)>),

    // pub read: fn(handle: u64, buffer: *mut u8, offset: u64, size: u64) -> u64,
    pub read_async: fn(handle: u64, buffer: *mut u8, offset: u64, size: u64, closure: *const RefCell<dyn FnMut(u64)>),

    // pub write: fn(handle: u64, buffer: *const u8, offset: u64, size: u64) -> u64,
    pub write_async: fn(handle: u64, buffer: *const u8, offset: u64, size: u64, closure: *const RefCell<dyn FnMut(u64)>),

    // pub flush: fn(handle: u64) -> u64,
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
    partition: PartitionUnsafe
}

struct SharedState {
    result: Option<u64>,
    waker: Option<core::task::Waker>
}

struct PartitionFuture {
    called: bool,
    shared_state: Arc<RefCell<SharedState>>,
    on_call: Arc<RefCell<dyn FnMut(Arc<RefCell<dyn FnMut(u64)>>)>>
}

impl PartitionFuture {

    pub fn new(on_call: Arc<RefCell<dyn FnMut(Arc<RefCell<dyn FnMut(u64)>>)>>) -> Self {
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

impl Future for PartitionFuture {

    type Output = u64;

    fn poll(mut self: Pin<&mut Self>, cx: &mut Context) -> Poll<Self::Output> {

        let state = self.shared_state.clone();

        if !state.borrow_mut().waker.is_some() {
            state.borrow_mut().waker = Some(cx.waker().clone());
        }

        if !self.called {
            self.called = true;

            (self.on_call.borrow_mut())(Arc::new(RefCell::new(move |result| {
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
pub extern fn partition_complete_future(result: u64, closure: *const RefCell<dyn FnMut(u64)>) {
    unsafe {
        let closure = Arc::from_raw(closure as *const RefCell<dyn FnMut(u64)>);
        (*closure.borrow_mut())(result);
        drop(closure); 
    }
}

impl PartitionSafe {

    pub fn new(partition: *mut PartitionUnsafe) -> Self {
        PartitionSafe {
            partition: unsafe { (*partition).clone() }
        }
    }

    pub async fn get_size(&self, handle: u64) -> u64 {

        let partition = self.partition.clone();

        PartitionFuture::new(
            Arc::new(RefCell::new(move |on_complete: Arc<RefCell<dyn FnMut(u64)>>| {
                // Incredibly important that the C++ function calls on_ccomplete
                (partition.get_size_async)(handle, Arc::into_raw(on_complete));
            }))).await
    }

    pub async fn read(&self, handle: u64, buffer: *mut u8, offset: u64, size: u64) -> u64 {

        let partition = self.partition.clone();

        PartitionFuture::new(
            Arc::new(RefCell::new(move |on_complete: Arc<RefCell<dyn FnMut(u64)>>| {
                // Incredibly important that the C++ function calls on_ccomplete
                (partition.read_async)(handle, buffer, offset, size, Arc::into_raw(on_complete));
            }))).await
    }

    pub async fn write(&self, handle: u64, buffer: *const u8, offset: u64, size: u64) -> u64 {

        let partition = self.partition.clone();

        PartitionFuture::new(
            Arc::new(RefCell::new(move |on_complete: Arc<RefCell<dyn FnMut(u64)>>| {
                // Incredibly important that the C++ function calls on_complete
                (partition.write_async)(handle, buffer, offset, size, Arc::into_raw(on_complete));
            }))).await
    }

    pub async fn flush(&self, handle: u64) -> u64 {

        let partition = self.partition.clone();

        PartitionFuture::new(
            Arc::new(RefCell::new(move |on_complete: Arc<RefCell<dyn FnMut(u64)>>| {
                // Incredibly important that the C++ function calls on_ccomplete
                (partition.flush_async)(handle, Arc::into_raw(on_complete));
            }))).await
    }

}