//! XWOS RUST：全局内存分配器
//! ========
//!
//! Rust的 `#![no_std]` 环境要求用户定义 [`global_allocator`] ，作为动态内存管理的实现。
//!
//!
//! # 允许动态内存的情况
//!
//! 若用户需要使用基于动态内存的特性，例如 [`Box<T>`] 和 [`Arc<T>`] ，
//! 需要在应用代码中定义 `GLOBAL_ALLOCATOR` 并赋值为 [`XwrustAllocator`] 。
//!
//! ```rust
//! #![no_std]
//! use xwrust::xwmm::allocator::XwrustAllocator;
//!
//! #[global_allocator]
//! pub static GLOBAL_ALLOCATOR: XwrustAllocator = XwrustAllocator;
//!
//! #[no_mangle]
//! pub unsafe extern "C" fn xwrust_main() {
//!     // 用户代码
//! }
//! ```
//!
//!
//! # 禁止动态内存的情况
//!
//! 若用户禁止在代码中使用动态内存，只使用静态内存，
//! 需要在应用代码中定义 `GLOBAL_ALLOCATOR` 并赋值为 [`DummyAllocator`] 。
//!
//! ```rust
//! use xwrust::xwmm::allocator::DummyAllocator;
//!
//! #[global_allocator]
//! pub static GLOBAL_ALLOCATOR: DummyAllocator = DummyAllocator;
//!
//! #[no_mangle]
//! pub unsafe extern "C" fn xwrust_main() {
//!     // 用户代码
//! }
//! ```
//!
//! 在禁止使用动态内存管理的场合下，下列模块不可以使用：
//!
//! + [`Box<T>`]
//! + [`Arc<T>`]
//! + [动态线程]
//! + [Xwmq]
//!
//!
//! [`global_allocator`]: <https://doc.rust-lang.org/core/prelude/v1/attr.global_allocator.html>
//! [`Box<T>`]: <https://doc.rust-lang.org/alloc/boxed/struct.Box.html>
//! [`Arc<T>`]: <https://doc.rust-lang.org/alloc/sync/struct.Arc.html>
//! [动态线程]: crate::xwos::thd
//! [Xwmq]: crate::xwmd::xwmq

extern crate core;
use core::ffi::*;
use core::ptr;
use core::alloc::{GlobalAlloc, Layout};

extern "C" {
    fn xwrustffi_allocator_alloc(alignment: usize, size: usize) -> *mut c_void;
    fn xwrustffi_allocator_free(mem: *mut c_void);
}

/// 内存分配器
pub struct XwrustAllocator;

unsafe impl GlobalAlloc for XwrustAllocator {
    unsafe fn alloc(&self, layout: Layout) -> *mut u8 {
        xwrustffi_allocator_alloc(layout.align(), layout.size()) as *mut _
    }

    unsafe fn dealloc(&self, ptr: *mut u8, _layout: Layout) {
        xwrustffi_allocator_free(ptr as *mut _);
    }
}

/// 虚假的内存分配器
pub struct DummyAllocator;

unsafe impl GlobalAlloc for DummyAllocator {
    #[allow(unreachable_code)]
    unsafe fn alloc(&self, _layout: Layout) -> *mut u8 {
        loop {
        }
        ptr::null_mut()
    }

    unsafe fn dealloc(&self, _ptr: *mut u8, _layout: Layout) {
        loop {
        }
    }
}
