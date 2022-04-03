//! XWOS RUST：panic
//! ========
//!

extern crate core;
use core::ffi::*;
use core::panic::PanicInfo;

extern crate alloc;
use alloc::boxed::Box;

extern "C" {
    fn board_alloc_error_handler(size: usize, align: usize) -> !;
    fn board_panic(info: *mut c_void) -> !;
}

/// 申请内存失败时的错误处理函数
///
/// 此函数会调用`board_alloc_error_handler()`交由用户处理，
/// 用户需要在APP模块中实现`fn board_alloc_error_handler(size: usize, align: usize) -> !;`。
///
/// 内存申请失败往往需要分析失败的原因，通常需要对调用进行回退分析。
/// 在MCU开发中，调试器会帮我们完成这一点。因此可以通过一条断点指令实现此函数。
/// 例如基于ARM-M的MCU可以这样实现：
///
/// ```rust
/// use cortex_m::asm;
///
/// pub fn board_alloc_error_handler(_size: usize, _align: usize) -> ! {
///     loop {
///         asm::bkpt();
///     }
/// }
/// ```
///
///
#[cfg(not(test))]
#[inline(never)]
#[alloc_error_handler]
pub fn xwrust_alloc_error_handler(layout: core::alloc::Layout) -> ! {
    unsafe {
        board_alloc_error_handler(layout.size(), layout.align());
    }
}

/// XWOS RUST的panic处理函数
///
/// 此函数会调用`board_panic()`交由用户处理，
/// 用户需要在APP模块中实现`fn board_panic(info: &PanicInfo) -> !;`。
///
/// panic处理函数常用于打印程序的backtrace。
///
/// 在MCU开发中，调试器会帮我们完成这一点。因此可以通过一条断点指令实现此函数。
/// 例如基于ARM-M的MCU可以这样实现：
///
/// ```rust
/// use cortex_m::asm;
/// use core::panic::PanicInfo;
///
/// pub fn board_panic(_info: &PanicInfo) -> ! {
///     loop {
///         asm::bkpt();
///     }
/// }
/// ```
///
#[cfg(not(test))]
#[inline(never)]
#[panic_handler]
pub fn xwrust_panic(info: &PanicInfo) -> ! {
    let raw = Box::into_raw(Box::new(info));
    unsafe {
        board_panic(raw as *mut c_void);
    }
}
