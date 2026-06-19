//! XWOS RUST Example: Evetn Flag
//! ========
//!

#![no_std]

extern crate alloc;
use alloc::sync::Arc;

use xwrust::xwtm;
use xwrust::xwbmp::Bmp;
use xwrust::xwos::thd;
use xwrust::xwos::cthd;
use xwrust::xwos::sync::flg::*;

use libc_print::std_name::println;

pub fn xwrust_example_flg() {
    println!("XWOS RUST Example: Evetn Flag");
    let flg = Arc::new(Flg::<8>::new());
    flg.init();
    let cflg = flg.clone();

    println!("[主线程] thd: {:?}", cthd::i());
    let _ = thd::spawn(move |_| { // 子线程闭包
        println!("[子线程] thd: {:?}", cthd::i());
        cthd::sleep(xwtm::ms(200));
        println!("[子线程]<{} ms> 设置事件", xwtm::nowtc());
        cflg.s1i(0);
        "OK"
    });

    let mut tg = Bmp::<8>::new();
    let msk = Bmp::<8>::new();
    msk.s1all();
    let res = flg.wait(Trigger::SetAny, true, &mut tg, &msk);
    match res {
        FlgError::Ok(_) => {
            println!("[主线程]<{} ms> 事件发生 {:?}", xwtm::nowtc(), tg);
        },
        _ => {
            println!("[主线程]<{} ms> 等待事件失败 {:?}", xwtm::nowtc(), res);
        },
    };

}
