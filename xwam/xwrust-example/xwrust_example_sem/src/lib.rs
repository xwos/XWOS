//! XWOS RUST Example: Semaphore
//! ========
//!

#![no_std]

extern crate alloc;
use alloc::sync::Arc;

use xwrust::xwtm;
use xwrust::types::*;
use xwrust::xwos::thd::*;
use xwrust::xwos::cthd;
use xwrust::xwos::sync::sem::*;

use libc_print::std_name::println;

pub fn xwrust_example_sem() {
    println!("XWOS RUST Example: Semaphore");
    let sem = Arc::new(Sem::new());
    sem.init(0, XwSsq::MAX);
    let sem_c = sem.clone();

    println!("[主线程] thd: {:?}", cthd::i());
    match DThdBuilder::new()
        .name("child".into())
        .spawn(move |_| { // 子线程闭包
            println!("[子线程] thd: {:?}", cthd::i());
            cthd::sleep(xwtm::ms(500));
            println!("[子线程]<{} ms> 发布信号量。", xwtm::nowtc());
            sem_c.post();
            "OK"
        }) {
            Ok(_) => {},
            Err(e) => {
                println!("[主线程] 创建子线程失败：{:?}。", e);
                return;
            },
        };
    let rc = sem.wait();
    match rc {
        SemError::Ok(_) => {
            println!("[主线程] 获取信号量。");
        },
        _ => {
            println!("[主线程] 等待信号量失败：{:?}。", rc);
        },
    };
}
