//! XWOS RUST Example: Barrier
//! ========
//!

#![no_std]

extern crate alloc;
use alloc::sync::Arc;

use xwrust::xwtm;
use xwrust::xwos::thd;
use xwrust::xwos::cthd;
use xwrust::xwos::sync::br::*;

use libc_print::std_name::println;

pub fn xwrust_example_br() {
    println!("XWOS RUST Example: Barrier");
    let br = Arc::new(Br::<8>::new());
    br.init();

    for idx in 0..8 {
        let c = br.clone();
        let _ = thd::spawn(move |_| {
            // 子线程闭包
            cthd::sleep(xwtm::ft(xwtm::ms(idx * 100)));
            println!("[thd{}] {:?}", idx, cthd::i());
            c.wait();
            cthd::sleep(xwtm::ft(xwtm::ms(idx * 100)));
            println!("[thd{}]<{} ms> 开始运行", idx, xwtm::nowtc());
            "OK"
        });
    }
}
