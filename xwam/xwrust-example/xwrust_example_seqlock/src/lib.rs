//! XWOS RUST Example: Seqlock
//! ========
//!

#![no_std]

extern crate alloc;

use xwrust::xwtm;
use xwrust::xwos::thd::*;
use xwrust::xwos::cthd;
use xwrust::xwos::lock::seqlock::*;

use libc_print::std_name::println;

static GLOBAL_SEQLOCK: Seqlock<u32> = Seqlock::new(0);

pub fn xwrust_example_seqlock() {
    println!("XWOS RUST Example: Seqlock");
    println!("[主线程] thd: {:?}", cthd::i());

    let res = DThdBuilder::new()
        .name("child".into())
        .spawn(move |_| {
            // 子线程闭包
            println!("[子线程] thd: {:?}", cthd::i());
            loop {
                loop {
                    let guard = GLOBAL_SEQLOCK.read_begin();
                    // 可在顺序锁的共享读临界区内使用会阻塞/睡眠的方法
                    cthd::sleep(xwtm::ms(400));
                    println!("[子线程]<{} ms> 进入共享读临界区。读取全局数据: {}",
                             xwtm::nowtc(), *guard);
                    if !guard.read_retry() {
                        break;
                    }
                }
                if cthd::shld_frz() { // 判断是否要冻结子线程
                    cthd::freeze();
                }
                if cthd::shld_stop() { // 判断是否要退出子线程
                    break;
                }
            }
            "OK"
        });
    let handle = match res {
        Ok(h) => {
            h
        },
        Err(e) => {
            println!("[主线程] 创建子线程失败: {}", e);
            return;
        },
    };

    cthd::sleep(xwtm::ms(100));
    let mut guard = GLOBAL_SEQLOCK.lock(SeqlockMode::WriteLockCpuirqSave(None));
    *guard += 1;
    let val = *guard;
    drop(guard);
    // 不可在顺序锁的写临界区以及独占读临界区内使用会阻塞/睡眠的方法
    println!("[主线程]<{} ms> 进入写临界区。写全局数据: {}", xwtm::nowtc(), val);

    cthd::sleep(xwtm::ms(900));
    handle.quit();
}
