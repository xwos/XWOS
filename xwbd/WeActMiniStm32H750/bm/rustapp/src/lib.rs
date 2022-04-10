//! XWOS RUST 应用
//!
//! # License: [MIT](https://opensource.org/licenses/MIT)
//!

#![no_std]

extern crate core;
use core::ffi::*;
use core::panic::PanicInfo;

extern crate alloc;
use alloc::vec::Vec;
use alloc::boxed::Box;

use cortex_m::asm;

use libc_print::std_name::println;

use xwrust::xwos::thd;

#[global_allocator]
pub static ALLOCATOR: xwrust::xwmm::Allocator = xwrust::xwmm::Allocator;

#[no_mangle]
pub unsafe extern "C" fn xwrust_main() {
    let mut v = Vec::with_capacity(16);
    v.push(1);
    v.push(2);
    v.push(3);
    v.push(4);
    println!("XWOS RUST");
    for x in v.iter() {
        println!("x: {}", x);
    }

    let child1_builder = thd::Builder::new().name("child".into());
    let child1 = child1_builder.spawn(|ele| {
        println!("Thread name: {}", ele.name().unwrap());
        3
    }).unwrap();
    let rc1 = child1.join().unwrap();
    println!("rc: {}", rc1);

    let ref2 = &1;
    let f = move |_| {
        ref2
    };
    let child2 = thd::spawn(f).unwrap();
    let rc2 = child2.join().unwrap();
    println!("rc: {}", rc2);
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
