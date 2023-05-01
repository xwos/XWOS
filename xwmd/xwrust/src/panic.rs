//! XWOS RUST：panic
//! ========
//!

extern crate core;
use core::panic::PanicInfo;

/// 申请内存失败时的错误处理函数
///
///
/// 内存申请失败往往需要分析失败的原因，通常需要进行backtrace分析。
/// 在MCU开发中，调试器会帮我们完成这一点。
///
///
#[cfg(not(test))]
#[inline(never)]
#[alloc_error_handler]
pub fn xwrust_alloc_error_handler(_: core::alloc::Layout) -> ! {
    loop {
    }
}

/// XWOS RUST的panic处理函数
///
///
/// panic处理函数常用于打印程序的backtrace。在MCU开发中，调试器会帮我们完成这一点。
///
#[cfg(not(test))]
#[inline(never)]
#[panic_handler]
pub fn xwrust_panic(_: &PanicInfo) -> ! {
    loop {
    }
}
