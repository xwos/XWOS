//! XWOS RUST 应用
//!
//! # License: [MIT](https://opensource.org/licenses/MIT)
//!

#![no_std]

extern crate core;
use core::ffi::*;
use core::panic::PanicInfo;

extern crate alloc;
use alloc::boxed::Box;

use cortex_m::asm;
use libc_print::std_name::println;

use xwrust_example_vector::xwrust_example_vector;
use xwrust_example_thd::xwrust_example_thd;
use xwrust_example_mutex::xwrust_example_mutex;
use xwrust_example_spinlock::xwrust_example_spinlock;
use xwrust_example_seqlock::xwrust_example_seqlock;
use xwrust_example_sem::xwrust_example_sem;
use xwrust_example_cond::xwrust_example_cond;
use xwrust_example_xwmq::xwrust_example_xwmq;

#[global_allocator]
pub static ALLOCATOR: xwrust::xwmm::Allocator = xwrust::xwmm::Allocator;

#[no_mangle]
pub unsafe extern "C" fn xwrust_main() {
    println!("XWOS RUST App");
    xwrust_example_vector();
    xwrust_example_thd();
    xwrust_example_mutex();
    xwrust_example_spinlock();
    xwrust_example_seqlock();
    xwrust_example_sem();
    xwrust_example_cond();
    xwrust_example_xwmq();
}

#[no_mangle]
pub unsafe extern "C" fn board_alloc_error_handler(_size: usize, _align: usize) -> ! {
    loop {
        asm::bkpt();
    }
}

#[no_mangle]
pub unsafe extern "C" fn board_panic(raw: *mut c_void) -> ! {
    let _info = Box::from_raw(raw as *mut &PanicInfo);
    loop {
        asm::bkpt();
    }
}
