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
use alloc::sync::Arc;

use xwrust::xwtm;
use xwrust::xwos::thd;
use xwrust::xwos::cthd;
use xwrust::xwos::lock::mtx::*;
use xwrust::xwos::lock::spinlock::*;

use cortex_m::asm;
use libc_print::std_name::println;

#[global_allocator]
pub static ALLOCATOR: xwrust::xwmm::Allocator = xwrust::xwmm::Allocator;

static GLOBAL_MUTEX: Mutex<[u32; 8]> = Mutex::new([0; 8]);
static GLOBAL_SPINLOCK: Spinlock<u32> = Spinlock::new(0);

#[no_mangle]
pub unsafe extern "C" fn xwrust_main() {
    println!("XWOS RUST");
    let mut v = Vec::with_capacity(16);
    v.push(1);
    v.push(2);
    v.push(3);
    v.push(4);
    for x in v.iter() {
        println!("x: {}", x);
    }

    GLOBAL_MUTEX.init();
    match GLOBAL_MUTEX.lock() {
        Ok(mut guard) => {
            guard[0] = 1;
        }
        Err(mtxerr) => {
            println!("[main] failed to lock: {:?}", mtxerr);
        }
    }

    GLOBAL_SPINLOCK.init();
    match GLOBAL_SPINLOCK.lock(SpinlockMode::LockCpuirqSave(0)) {
        Ok(mut guard) => {
            *guard = 1;
        }
        Err(splkerr) => {
            println!("[main] failed to lock: {:?}", splkerr);
        }
    }



    let lock: Arc<Mutex<u32>> = Arc::new(Mutex::new(10));
    lock.init();
    let lock_child = lock.clone();

    println!("[main] thd: {:?}", cthd::i());
    println!("[main] now: {} ms", xwtm::nowtc());
    match thd::Builder::new()
        .name("child".into())
        .spawn(move |ele| {
            // 子线程闭包
            println!("[child] name: {}", ele.name().unwrap());
            println!("[child] thd: {:?}", cthd::i());
            println!("[child] now: {} ms", xwtm::nowtc());

            match GLOBAL_MUTEX.lock() {
                Ok(mut guard) => {
                    guard[0] += 1;
                    let value = guard[0];
                    println!("[child] locked, value: {}", value);
                }
                Err(mtxerr) => {
                    println!("[child] failed to lock: {:?}", mtxerr);
                }
            }

            match lock_child.lock() {
                Ok(mut guard) => {
                    *guard += 1;
                    let value = *guard;
                    println!("[child] locked, value: {}", value);
                }
                Err(mtxerr) => {
                    println!("[child] failed to lock: {:?}", mtxerr);
                }
            }

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
                    println!("[main] sleep 1s ...");
                    cthd::sleep(xwtm::s(1));
                    match GLOBAL_MUTEX.lock() {
                        Ok(mut guard) => {
                            guard[0] += 1;
                            let value = guard[0];
                            println!("[main] locked, value {}", value);
                        }
                        Err(mtxerr) => {
                            println!("[main] failed to lock: {:?}", mtxerr);
                        }
                    }
                    match lock.lock() {
                        Ok(mut guard) => {
                            *guard += 1;
                            let value = *guard;
                            println!("[main] locked, value {}", value);
                        }
                        Err(mtxerr) => {
                            println!("[main] failed to lock: {:?}", mtxerr);
                        }
                    }

                    println!("[main] sleep 2s ...");
                    cthd::sleep(xwtm::s(2));
                    println!("[main] now: {} ms", xwtm::nowtc());
                    println!("[main] stop child ...");
                    match child.stop() { // stop() = quit() + join()
                        Ok(r) => {
                            println!("[main] Child thread return: {}, now: {} ms",
                                     r, xwtm::nowtc());
                            break;
                        },
                        Err(e) => {
                            println!("[main] Failed to stop child thread: {}, now: {} ms",
                                     e.join_state(), xwtm::nowtc());
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
            Err(e) => { // 新建子线程失败
                println!("[main] Failed to spawn child thread: {}, now: {} ms",
                         e, xwtm::nowtc());
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
