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
use xwrust::xwos::sync::flg::*;
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
            println!("[子线程] sel0 绑定失败：{:?}", e);
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
            println!("[子线程] sem1 绑定失败：{:?}", e);
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
            println!("[子线程] cond2 绑定失败：{:?}", e);
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
            println!("[子线程] br3 绑定失败：{:?}", e);
            return;
        }
    };

    // 事件标志4
    let flg4 = Arc::new(Flg::<8>::new());
    flg4.init();
    let cflg4 = flg4.clone();
    let flg4sel = match flg4.bind(&sel0, 4) {
        Ok(s) => {
            s
        },
        Err(e) => {
            println!("[子线程] flg4 绑定失败：{:?}", e);
            return;
        }
    };

    // 创建发送 **选择信号** 的子线程
    let _ = thd::spawn(move |_| { // 子线程闭包
        // 信号量1
        cthd::sleep(xwtm::ms(200));
        println!("[子线程] sem1 发布");
        csem1.post();
        cthd::sleep(xwtm::ms(200));
        // 条件量2
        println!("[子线程] cond2 广播");
        ccond2.broadcast();
        // 线程栅栏3
        for idx in 0..2 {
            let c = cbr3.clone();
            let _ = thd::spawn(move |_| { // 孙子线程闭包
                cthd::sleep(xwtm::ms(idx * 100));
                c.wait();
            });
        }
        cthd::sleep(xwtm::ms(200));
        println!("[子线程] flg4.s1i(0)");
        cflg4.s1i(0);
        "OK"
    });

    let msk = Bmp::<8>::new();
    msk.s1all();
    loop {
        let res = sel.select_to(&msk, xwtm::ft(xwtm::s(3)));
        match res {
            Ok(t) => {
                if sel0sel.selected(&t) {
                    println!("[主线程]<{} ms> 获取 sel0", xwtm::nowtc());
                    let msk0 = Bmp::<32>::new();
                    msk0.s1all();
                    match sel0.tryselect(&msk0) {
                        Ok(t0) => {
                            if sem1sel.selected(&t0) {
                                sem1.trywait();
                                println!("[主线程]<{} ms> 获取 sem1", xwtm::nowtc());
                            }
                            if cond2sel.selected(&t0) {
                                println!("[主线程]<{} ms> 获取 cond2", xwtm::nowtc());
                            }
                            if br3sel.selected(&t0) {
                                println!("[主线程]<{} ms> 获取 br3", xwtm::nowtc());
                            }
                            if flg4sel.selected(&t0) {
                                let mut tg = Bmp::<8>::new();
                                let msk = Bmp::<8>::new();
                                msk.s1all();
                                let _ = flg4.trywait(Trigger::SetAny, true,
                                                     &mut tg, &msk);
                                println!("[主线程]<{} ms> 事件 {:?}",
                                         xwtm::nowtc(), tg);
                            }
                        },
                        Err(e) => {
                            println!("[主线程]<{} ms> 测试sel0失败 ... {:?}",
                                     xwtm::nowtc(), e);
                        },
                    }
                }
            },
            Err(e) => {
                println!("[主线程]<{} ms> 等待sel失败 ... {:?}", xwtm::nowtc(), e);
                break;
            },
        };
    }
}
