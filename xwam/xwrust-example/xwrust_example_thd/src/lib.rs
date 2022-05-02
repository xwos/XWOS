//! XWOS RUST Example: Thread
//! ========
//!

#![no_std]

use xwrust::xwtm;
use xwrust::xwos::thd;
use xwrust::xwos::cthd;

use libc_print::std_name::println;

pub fn xwrust_example_thd() {
    println!("XWOS RUST Example: Thread");

    println!("[main] thd: {:?}", cthd::i());
    println!("[main] now: {} ms", xwtm::nowtc());
    match thd::Builder::new()
        .name("child".into())
        .spawn(move |ele| {
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
                    cthd::sleep(xwtm::s(3));
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
        }
}
