//! XWOS RUST Example: Swt
//! ========
//!

#![no_std]

use xwrust::xwtm;
use xwrust::xwos::skd;
use xwrust::xwos::swt::*;
use xwrust::xwos::cthd;
use xwrust::xwos::lock::spinlock::*;

use libc_print::std_name::println;

static SWT: Swt<Spinlock<skd::Context>> = Swt::new(Spinlock::new(skd::Context::Boot));

pub fn xwrust_example_swt() {
    println!("XWOS RUST Example: Swt");

    SWT.as_ref().init(); // 初始化自选锁
    SWT.once(xwtm::now(), xwtm::ms(200), |swt| {
        // 定时器回调函数运行在中断底半部，不可使用会阻塞的函数。
        // `println!()`会调用UART的TX函数，此函数会阻塞，因此不可在定时器回调函数中打印消息。
        // 中断底半部中只可使用自选锁或顺序锁。
        let res = swt.as_ref().lock(SpinlockMode::LockCpuirq);
        match res {
            Ok(mut guard) => {
                *guard = skd::context();
            },
            Err(_) => {},
        };
    });
    cthd::sleep(xwtm::ms(500));
    let res = SWT.as_ref().lock(SpinlockMode::LockCpuirq);
    match res {
        Ok(guard) => {
            let ctx = *guard;
            drop(guard);
            println!("[主线程] 定时器回调函数的上下文：{:?}", ctx);
        },
        Err(_) => {},
    };
}
