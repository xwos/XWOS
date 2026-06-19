//! XWOS RUST Example: XWCQ
//! ========
//!

#![no_std]
#![allow(internal_features)]
#![feature(slice_internals)]

extern crate core;
use core::str;
use core::slice::memchr::memchr;

use xwrust::xwtm;
use xwrust::xwos::thd::SThd;
use xwrust::xwos::cthd;
use xwrust::xwmd::xwcq::*;

use libc_print::std_name::println;

static CQ: Xwcq<64, 16> = Xwcq::new();
static CHILD0: SThd<1024, &str> = SThd::new("CHILD0", true);
static CHILD1: SThd<1024, &str> = SThd::new("CHILD1", true);

pub fn xwrust_example_xwcq() {
    println!("XWOS RUST Example: XWCQ");
    CQ.init();

    println!("XWCQ<Size:{}, Num:{}>", CQ.size(), CQ.capacity());
    CHILD0.run(|_| { // 子线程0
            cthd::sleep(xwtm::ms(200));
            println!("[子线程0] 发送消息");
            let mut buffer: [u8; 64] = [0; 64];
            let msg = "子线程消息 0".as_bytes();
            let msgsize = msg.len();
            buffer[..msgsize].copy_from_slice(msg);
            let rc = CQ.eq(buffer);
            match rc {
                Ok(sz) => {
                    println!("[子线程0] 发送消息成功：{} 字节。", sz);
                },
                Err(e) => {
                    println!("[子线程0] 发送消息失败：{:?}。", e);
                },
            };
            "OK"
        });

    CHILD1.run(|_| { // 子线程1
            cthd::sleep(xwtm::ms(500));
            println!("[子线程1] 发送消息");
            let mut buffer: [u8; 64] = [0; 64];
            let msg = "子线程消息 1".as_bytes();
            let msgsize = msg.len();
            buffer[..msgsize].copy_from_slice(msg);
            let rc = CQ.eq(buffer);
            match rc {
                Ok(sz) => {
                    println!("[子线程1] 发送消息成功：{} 字节。", sz);
                },
                Err(e) => {
                    println!("[子线程1] 发送消息失败：{:?}。", e);
                },
            };
            "OK"
        });

    loop {
        let rc = CQ.dq_to::<[u8; 64]>(xwtm::ft(xwtm::s(1)));
        match rc {
            Ok(d) => {
                cthd::sleep(xwtm::ms(100));
                println!("[主线程] 消息[u8; 64]：{:?}", d);
                let totalslice = &d[0..64];
                let nullpos = memchr(0, totalslice).unwrap_or(64);
                println!("[主线程] nullpos：{}", nullpos);
                let msgu8slice = &d[0..nullpos];
                let msg = str::from_utf8(msgu8slice);
                println!("[主线程] 消息[str]：{:?}", msg);
            },
            Err(e) => {
                println!("[主线程] 等待消息失败：{:?}。", e);
                break;
            },
        };
    }
}
