//! XWOS RUST：全局内存分配器
//! ========
//!
//! Rust的 `#![no_std]` 环境要求用户定义 [`global_allocator`] ，作为动态内存管理的实现。
//! 标准的Rust库依赖crate libc的 `memalign()` 与 `free()` 函数来实现 [`global_allocator`] 。
//! XWOS RUST不依赖crate libc，使用 `xwos/mm/mempool` 算法来实现 [`global_allocator`] 。
//! 同时，XWOS RUST也提供虚假的 [`global_allocator`] ，用于禁止动态内存的情况。
//!
//!
//! # 允许动态内存的情况
//!
//! 若用户需要使用基于动态内存的特性，例如 [`Box<T>`] 和 [`Arc<T>`] ，
//! 需要在应用代码中定义 `GLOBAL_ALLOCATOR` 并赋值为 [`AllocatorMempool`] 。
//!
//! ```rust
//! #![no_std]
//! use xwrust::xwmm::allocator::AllocatorMempool;
//!
//! #[global_allocator]
//! pub static GLOBAL_ALLOCATOR: AllocatorMempool = AllocatorMempool;
//!
//! #[no_mangle]
//! pub unsafe extern "C" fn xwrust_main() {
//!     // 用户代码
//! }
//! ```
//!
//! 同时，用户需要在C语言层面提供 `xwrust_mempool` 的定义，
//! 例如 `XWOS/xwbd/WeActMiniStm32H750/bm/xwac/xwrust/allocator.c`
//!
//! ```C
//! #include <xwos/mm/mempool/allocator.h>
//!
//! extern xwsz_t axisram_mr_origin[];
//! struct xwmm_mempool * xwrust_mempool = (void *)axisram_mr_origin;
//! ```
//!
//!
//! # 禁止动态内存的情况
//!
//! 若用户禁止在代码中使用动态内存，只使用静态内存，
//! 需要在应用代码中定义 `GLOBAL_ALLOCATOR` 并赋值为 [`AllocatorDummy`] 。
//!
//! ```rust
//! use xwrust::xwmm::allocator::AllocatorDummy;
//!
//! #[global_allocator]
//! pub static GLOBAL_ALLOCATOR: AllocatorDummy = AllocatorDummy;
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

/// 基于mempool的内存分配器
pub struct AllocatorMempool;

unsafe impl GlobalAlloc for AllocatorMempool {
    unsafe fn alloc(&self, layout: Layout) -> *mut u8 {
        xwrustffi_allocator_alloc(layout.align(), layout.size()) as *mut _
    }

    unsafe fn dealloc(&self, ptr: *mut u8, _layout: Layout) {
        xwrustffi_allocator_free(ptr as *mut _);
    }
}

/// 虚假的内存分配器
pub struct AllocatorDummy;

unsafe impl GlobalAlloc for AllocatorDummy {
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
