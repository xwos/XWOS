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
use xwrust::xwos::cthd;
use xwrust::xwtm;

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

    println!("[main] thd: {:?}", cthd::i());
    println!("[main] now: {} ms", xwtm::nowtc());
    match thd::Builder::new()
        .name("child".into())
        .spawn(|ele| {
            // 子线程闭包
            println!("[child] name: {}", ele.name().unwrap());
            println!("[child] thd: {:?}", cthd::i());
            println!("[child] now: {} ms", xwtm::nowtc());
            let mut count = 0;
            loop {
                cthd::sleep(xwtm::s(1));
                count += 1;
                println!("[child] count: {}, now: {} ms", count, xwtm::nowtc());
                if cthd::shld_frz() { // 判断是否要冻结子线程
                    cthd::freeze();
                }
                if cthd::shld_stop() { // 判断是否要退出子线程
                    break;
                }
            }
            "OK"
        }) {
            Ok(h) => { // 新建子线程成功
                let mut child = h;
                loop {
                    println!("[main] sleep 3s ...");
                    cthd::sleep(xwtm::s(3)); // 主线程睡眠3s
                    println!("[main] now: {} ms", xwtm::nowtc());
                    println!("[main] stop child ...");
                    match child.stop() { // stop() = quit() + join()
                        Ok(r) => {
                            println!("[main] Child thread return: {}, now: {} ms", r, xwtm::nowtc());
                            break;
                        },
                        Err(e) => {
                            println!("[main] Failed to stop child thread: {}, now: {} ms", e.join_state(), xwtm::nowtc());
                            child = e;
                        },
                    }
                    if cthd::shld_frz() { // 判断是否要冻结主线程
                        cthd::freeze();
                    }
                    if cthd::shld_stop() { // 判断是否要退出主线程
                        break;
                    }
                }
            },
            Err(rc) => { // 新建子线程失败
                println!("[main] Failed to spawn child thread: {}, now: {} ms", rc, xwtm::nowtc());
            },
        };
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
