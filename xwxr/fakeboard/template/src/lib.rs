//! XWOS RUST 应用模板
//!
//! # author
//!
//! + 隐星魂 (Roy Sun) <xwos@xwos.tech>
//!
//! # License: [MIT](https://opensource.org/licenses/MIT)
//!

#![no_std]

extern crate core;
use core::ffi::*;
use core::panic::PanicInfo;

extern crate alloc;

use libc_print::std_name::println;

#[global_allocator]
pub static ALLOCATOR: xwrust::xwmm::Allocator = xwrust::xwmm::Allocator;

#[no_mangle]
pub unsafe extern "C" fn xwrust_main() {
    println!("XWOS RUST");
}

#[no_mangle]
pub unsafe extern "C" fn board_alloc_error_handler(_size: usize, _align: usize) -> ! {
    loop {
    }
}

#[no_mangle]
pub unsafe extern "C" fn board_panic(raw: *mut c_void) -> ! {
    let _info = Box::from_raw(raw as *mut &PanicInfo);
    loop {
    }
}
