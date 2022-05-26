//! XWOS RUST Example: XWMQ
//! ========
//!

#![no_std]

extern crate alloc;
use alloc::string::String;
use alloc::string::ToString;

use xwrust::xwtm;
use xwrust::xwos::thd;
use xwrust::xwos::cthd;
use xwrust::xwmd::xwmq::*;

use libc_print::std_name::println;

static MQ: Xwmq<16, String> = Xwmq::new();

pub fn xwrust_example_xwmq() {
    println!("XWOS RUST Example: XWMQ");
    let (tx, rx) = MQ.init();
    let tx1 = tx.clone();

    println!("[主线程] thd: {:?}", cthd::i());
    match thd::DThdBuilder::new()
        .name("child0".into())
        .spawn(move |_| { // 子线程闭包
            cthd::sleep(xwtm::ms(200));
            println!("[子线程0] thd: {:?}", cthd::i());
            println!("[子线程0] 发送消息");
            tx.eq("child0 message".to_string());
            "OK"
        }) {
            Ok(_) => {},
            Err(e) => {
                println!("[主线程] 创建子线程失败：{:?}。", e);
                return;
            },
        };

    match thd::DThdBuilder::new()
        .name("child1".into())
        .spawn(move |_| { // 子线程闭包
            cthd::sleep(xwtm::ms(500));
            println!("[子线程1] thd: {:?}", cthd::i());
            println!("[子线程1] 发送消息");
            tx1.eq("child1 Message".to_string());
            "OK"
        }) {
            Ok(_) => {},
            Err(e) => {
                println!("[主线程] 创建子线程失败：{:?}。", e);
                return;
            },
        };

    loop {
        let rc = rx.dq_to(xwtm::ft(xwtm::s(1)));
        match rc {
            Ok(d) => {
                println!("[主线程] 收到消息：{:?}", d);
            },
            Err(e) => {
                println!("[主线程] 等待消息失败：{:?}。", e);
                break;
            },
        };
    }
}
