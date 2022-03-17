#![no_std]
#![feature(alloc_error_handler)]

use core::panic::PanicInfo;
#[panic_handler]
fn panic(_info: &PanicInfo) -> ! {
    loop {}
}

use newlib_alloc;
#[global_allocator]
static GLOBAL_ALLOCATOR: newlib_alloc::Alloc = newlib_alloc::Alloc;

#[alloc_error_handler]
fn alloc_error_handler(_layout: core::alloc::Layout) -> ! {
    loop {}
}

use libc_print::std_name::println;

extern crate alloc;
use alloc::vec::Vec;

#[no_mangle]
pub unsafe extern "C" fn rust_main() {
    let mut v = Vec::new();
    v.push(1);
    v.push(2);
    v.push(3);
    v.push(4);
    println!("RUST XWOS!");
    for x in v.iter() {
        println!("x: {}", x);
    }
}
