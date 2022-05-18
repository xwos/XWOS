//! XWOS RUST Example: Selector
//! ========
//!

#![no_std]

extern crate alloc;
use alloc::sync::Arc;

use xwrust::xwtm;
use xwrust::types::*;
use xwrust::xwbmp::*;
use xwrust::xwos::thd;
use xwrust::xwos::cthd;
use xwrust::xwos::sync::sem::*;
use xwrust::xwos::sync::cond::*;
use xwrust::xwos::sync::br::*;
use xwrust::xwos::sync::sel::*;

use libc_print::std_name::println;

pub fn xwrust_example_sel() {
    println!("XWOS RUST Example: Selector");

    let sel = Sel::<8>::new();
    sel.init();

    // 信号选择器0
    let sel0 = Sel::<32>::new();
    sel0.init();
    let sel0sel = match sel0.bind(&sel, 0) {
        Ok(s) => {
            s
        },
        Err(e) => {
            println!("[child] sel0 绑定失败：{:?}", e);
            return;
        }
    };

    // 信号量1
    let sem1 = Arc::new(Sem::new());
    sem1.init(0, XwSsq::MAX);
    let csem1 = sem1.clone();
    let sem1sel = match sem1.bind(&sel0, 1) {
        Ok(s) => {
            s
        },
        Err(e) => {
            println!("[child] sem1 绑定失败：{:?}", e);
            return;
        }
    };

    // 条件量2
    let cond2 = Arc::new(Cond::new());
    cond2.init();
    let ccond2 = cond2.clone();
    let cond2sel = match cond2.bind(&sel0, 2) {
        Ok(s) => {
            s
        },
        Err(e) => {
            println!("[child] cond2 绑定失败：{:?}", e);
            return;
        }
    };

    // 线程栅栏3
    let br3 = Arc::new(Br::<2>::new());
    br3.init();
    let cbr3 = br3.clone();
    let br3sel = match br3.bind(&sel0, 3) {
        Ok(s) => {
            s
        },
        Err(e) => {
            println!("[child] br3 绑定失败：{:?}", e);
            return;
        }
    };

    // 创建发送 **选择信号** 的子线程
    let _ = thd::spawn(move |_| { // 子线程闭包
        // 信号量1
        cthd::sleep(xwtm::ms(200));
        println!("[child] sem1 发布");
        csem1.post();
        cthd::sleep(xwtm::ms(200));
        // 条件量2
        println!("[child] cond2 广播");
        ccond2.broadcast();
        cthd::sleep(xwtm::ms(200));
        // 线程栅栏3
        for idx in 0..2 {
            let c = cbr3.clone();
            let _ = thd::spawn(move |_| { // 孙子线程闭包
                cthd::sleep(xwtm::ms(idx * 100));
                c.wait();
            });
        }
        "OK"
    });

    let msk = Bmp::<8>::new();
    msk.s1all();
    loop {
        let res = sel.select_to(&msk, xwtm::ft(xwtm::s(3)));
        match res {
            Ok(t) => {
                if sel0sel.selected(&t) {
                    println!("[main]<{} ms> 获取 sel0", xwtm::nowtc());
                    let msk0 = Bmp::<32>::new();
                    msk0.s1all();
                    match sel0.tryselect(&msk0) {
                        Ok(t0) => {
                            if sem1sel.selected(&t0) {
                                sem1.trywait();
                                println!("[main]<{} ms> 获取 sem1", xwtm::nowtc());
                            }
                            if cond2sel.selected(&t0) {
                                println!("[main]<{} ms> 获取 cond2", xwtm::nowtc());
                            }
                            if br3sel.selected(&t0) {
                                println!("[main]<{} ms> 获取 br3", xwtm::nowtc());
                            }
                        },
                        Err(e) => {
                            println!("[main]<{} ms> 测试sel0失败 ... {:?}", xwtm::nowtc(), e);
                        },
                    }
                }
            },
            Err(e) => {
                println!("[main]<{} ms> 等待sel失败 ... {:?}", xwtm::nowtc(), e);
                break;
            },
        }
    }
}
