//! XWOS RUST Example: Static Thread
//! ========
//!

#![no_std]

use xwrust::xwtm;
use xwrust::xwos::thd::*;
use xwrust::xwos::cthd;

use libc_print::std_name::println;

static STHD: SThd<1024, &str> = SThd::new("SThd", true);

pub fn xwrust_example_sthd() {
    println!("XWOS RUST Example: Static Thread");
    println!("[主线程]<{} ms> thd: {:?}", xwtm::nowtc(), cthd::i());

    let h = STHD.run(|sthd| {
        println!("[子线程]<{} ms> name: {}, ThdD: {:?}",
                 xwtm::nowtc(), sthd.name(), cthd::i());
        "Ok"
    });
    let res = h.stop();
    match res {
        Ok(r) => {
            println!("[主线程]<{} ms> 子线程返回值：{}", xwtm::nowtc(), r);
        },
        Err(e) => {
            println!("[主线程]<{} ms> 停止子线程失败: {:?}", xwtm::nowtc(), e.state());
        },
    };
}
