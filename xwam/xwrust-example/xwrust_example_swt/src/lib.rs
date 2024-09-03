//! XWOS RUST Example: Software Timer
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
    println!("XWOS RUST Example: Software Timer");

    SWT.once(xwtm::now(), xwtm::ms(200), |swt| {
        // 定时器回调函数运行在中断底半部，不可使用会阻塞的函数。
        // `println!()` 会调用UART的发送函数，此函数会阻塞，不可在此处打印消息。
        // 中断底半部中只可使用自选锁或顺序锁。
        let mut guard = swt.as_ref().lock(SpinlockMode::LockCpuirq);
        *guard = skd::context();
    });
    cthd::sleep(xwtm::ms(500));
    let guard = SWT.as_ref().lock(SpinlockMode::LockCpuirq);
    let ctx = *guard;
    drop(guard);
    println!("[主线程] 定时器回调函数的上下文：{:?}", ctx);
}
