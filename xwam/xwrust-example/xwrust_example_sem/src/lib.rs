//! XWOS RUST Example: Semaphore
//! ========
//!

#![no_std]

extern crate alloc;
use alloc::sync::Arc;

use xwrust::xwtm;
use xwrust::errno::*;
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

    println!("[main] thd: {:?}", cthd::i());
    match thd::Builder::new()
        .name("child".into())
        .spawn(move |_| { // 子线程闭包
            println!("[child] thd: {:?}", cthd::i());
            cthd::sleep(xwtm::ms(500));
            sema_c.post();
            println!("[child]<{} ms> 发布信号量。", xwtm::nowtc());
            "OK"
        }) {
            Ok(_) => {},
            Err(e) => {
                println!("[main] 创建子线程失败：{:?}。", e);
                return;
            },
        };
    let rc = sema.wait();
    if XWOK == rc {
        println!("[main] 获取信号量。");
    } else {
        println!("[main] 等待信号量失败：{}。", rc);
    }
}
