//! XWOS RUST Example: Semaphore
//! ========
//!

#![no_std]

extern crate alloc;
use alloc::sync::Arc;

use xwrust::xwtm;
use xwrust::types::*;
use xwrust::xwos::thd;
use xwrust::xwos::cthd;
use xwrust::xwos::sync::sem::*;

use libc_print::std_name::println;

pub fn xwrust_example_sem() {
    println!("XWOS RUST Example: Semaphore");
    let sema = Arc::new(Sem::new());
    sema.init(0, XwSsq::MAX);
    let sema_c = sema.clone();

    println!("[主线程] thd: {:?}", cthd::i());
    match thd::Builder::new()
        .name("child".into())
        .spawn(move |_| { // 子线程闭包
            println!("[子线程] thd: {:?}", cthd::i());
            cthd::sleep(xwtm::ms(500));
            println!("[子线程]<{} ms> 发布信号量。", xwtm::nowtc());
            sema_c.post();
            "OK"
        }) {
            Ok(_) => {},
            Err(e) => {
                println!("[主线程] 创建子线程失败：{:?}。", e);
                return;
            },
        };
    let rc = sema.wait();
    match rc {
        SemError::Ok => {
            println!("[主线程] 获取信号量。");
        },
        _ => {
            println!("[主线程] 等待信号量失败：{:?}。", rc);
        },
    };
}
