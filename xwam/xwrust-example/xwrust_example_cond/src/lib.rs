//! XWOS RUST Example: Condition
//! ========
//!

#![no_std]

extern crate alloc;
use alloc::sync::Arc;

use xwrust::xwtm;
use xwrust::xwos::thd;
use xwrust::xwos::cthd;
use xwrust::xwos::lock::mtx::*;
use xwrust::xwos::sync::cond::*;

use libc_print::std_name::println;

pub fn xwrust_example_cond() {
    println!("XWOS RUST Example: Condition");
    let pair = Arc::new((Mutex::new(true), Cond::new()));
    pair.0.init();
    pair.1.init();
    let pair_c = pair.clone();

    println!("[main] thd: {:?}", cthd::i());
    match thd::Builder::new()
        .name("child".into())
        .spawn(move |_| {
            // 子线程闭包
            println!("[child] thd: {:?}", cthd::i());
            cthd::sleep(xwtm::ms(500));
            let (lock, cvar) = &*pair_c;
            match lock.lock() {
                Ok(mut guard) => {
                    *guard = false;
                    drop(guard); // 先解锁再触发条件可提高效率
                    println!("[child]<{} ms> 广播", xwtm::nowtc());
                    cvar.broadcast();
                }
                Err(e) => {
                    println!("[child] 获取锁失败：{:?}", e);
                }
            }
            "OK"
        }) {
            Ok(_) => {},
            Err(e) => {
                println!("[main] 创建子线程失败：{:?}", e);
                return;
            },
        }

    let (lock, cvar) = &*pair;
    let mut guard;
    match lock.lock() {
        Ok(g) => {
            guard = g;
            while *guard {
                match guard.wait(cvar) {
                    Ok(g) => {
                        println!("[main]<{} ms> 收到条件量的通知: {}", xwtm::nowtc(), *g);
                        guard = g;
                    },
                    Err(e) => {
                        println!("[main] 等待条件量失败：{:?}", e);
                        break;
                    },
                }
            }
        },
        Err(e) => {
            println!("[main] 上锁失败：{:?}", e);
        },
    };
}
