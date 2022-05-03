//! XWOS RUST Example: Spinlock
//! ========
//!

#![no_std]

extern crate alloc;
use alloc::sync::Arc;

use xwrust::xwtm;
use xwrust::xwos::thd;
use xwrust::xwos::cthd;
use xwrust::xwos::lock::spinlock::*;

use libc_print::std_name::println;

pub fn xwrust_example_spinlock() {
    println!("XWOS RUST Example: Spinlock");
    println!("[主线程] thd: {:?}", cthd::i());

    println!("[主线程] 创建局部数据并初始化局部自旋锁。");
    let local_spinlock: Arc<Spinlock<u32>> = Arc::new(Spinlock::new(10));
    local_spinlock.init();
    let local_spinlock_child = local_spinlock.clone();

    match thd::Builder::new()
        .name("child".into())
        .spawn(move |_| {
            // 子线程闭包
            println!("[子线程] thd: {:?}", cthd::i());
            let result = local_spinlock_child.lock(SpinlockMode::LockCpuirqSave(None));
            match result {
                Ok(mut guard) => {
                    *guard += 1;
                    let val = *guard;
                    drop(guard);
                    // 不可在自旋锁临界区内使用会阻塞/睡眠的方法
                    println!("[子线程] 获取局部自旋锁成功。写局部数据: {}", val);
                }
                Err(e) => {
                    println!("[子线程] 获取局部自旋锁失败：{:?}", e);
                }
            }
            "OK"
        }) {
            Ok(_) => { // 新建子线程成功
            },
            Err(e) => { // 新建子线程失败
                println!("[主线程] 创建子线程失败: {}", e);
            },
        }

    cthd::sleep(xwtm::ms(500));
    let result = local_spinlock.lock(SpinlockMode::LockCpuirqSave(None));
    match result {
        Ok(mut guard) => {
            *guard += 1;
            let val = *guard;
            drop(guard);
            // 不可在自旋锁临界区内使用会阻塞/睡眠的方法
            println!("[主线程] 获取局部自旋锁成功。写局部数据: {}", val);
        }
        Err(e) => {
            println!("[主线程] 获取局部自旋锁失败：{:?}", e);
        }
    }
}
