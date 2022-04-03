//! XWOS RUST：内存管理
//! ========
//!
//! XWOS已经在C语言层面已经实现了libc的`malloc()`、`free()`、`memalign()`、`realloc()`等一系列函数，
//! XWOS RUST的内存分配器只是对这些函数进行封装。
//!
//! 用户需要在`xwrust_main()`函数之前设置全局内存分配器，例如：
//!
//! ```rust
//! #[global_allocator]
//! pub static ALLOCATOR: xwrust::xwmm::Allocator = xwrust::xwmm::Allocator;
//!
//! #[no_mangle]
//! pub unsafe extern "C" fn xwrust_main() {
//!     // 主线程代码
//! }
//! ```
//!

extern crate core;
use core::alloc::{GlobalAlloc, Layout};

/// XWOS的内存分配器
pub struct Allocator;

unsafe impl GlobalAlloc for Allocator {
    unsafe fn alloc(&self, layout: Layout) -> *mut u8 {
        libc::memalign(layout.align(), layout.size()) as *mut _
    }

    unsafe fn dealloc(&self, ptr: *mut u8, _layout: Layout) {
        libc::free(ptr as *mut _);
    }
}
