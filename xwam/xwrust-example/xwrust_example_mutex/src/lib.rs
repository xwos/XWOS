//! XWOS RUST Example: Mutex
//! ========
//!

#![no_std]

extern crate alloc;
use alloc::sync::Arc;

use xwrust::xwtm;
use xwrust::xwos::thd;
use xwrust::xwos::cthd;
use xwrust::xwos::lock::mtx::*;

use libc_print::std_name::println;

static GLOBAL_MUTEX: Mutex<[u32; 8]> = Mutex::new([0; 8]);

pub fn xwrust_example_mutex() {
    println!("XWOS RUST Example: Mutex");
    println!("[主线程] thd: {:?}", cthd::i());

    println!("[主线程] 初始化全局互斥锁。");
    GLOBAL_MUTEX.init();
    let result = GLOBAL_MUTEX.lock();
    match result {
        Ok(mut guard) => {
            guard[0] = 1;
            println!("[主线程] 获取全局互斥锁成功。写全局数据[0]: {}", guard[0]);
        }
        Err(e) => {
            println!("[主线程] 获取全局互斥锁失败：{:?}", e);
        }
    }

    println!("[主线程] 创建局部数据并初始化局部互斥锁。");
    let local_mutex: Arc<Mutex<u32>> = Arc::new(Mutex::new(10));
    local_mutex.init();
    let local_mutex_child = local_mutex.clone();

    match thd::Builder::new()
        .name("child".into())
        .spawn(move |_| {
            // 子线程闭包
            println!("[子线程] thd: {:?}", cthd::i());
            let result = GLOBAL_MUTEX.lock();
            match result {
                Ok(mut guard) => {
                    guard[0] += 1;
                    println!("[子线程] 获取全局互斥锁成功。写全局数据[0]: {}", guard[0]);
                }
                Err(e) => {
                    println!("[子线程] 获取全局互斥锁失败：{:?}", e);
                }
            }

            let result = local_mutex_child.lock();
            match result {
                Ok(mut guard) => {
                    *guard += 1;
                    println!("[子线程] 获取局部互斥锁成功。写局部数据: {}", *guard);
                }
                Err(e) => {
                    println!("[子线程] 获取局部互斥锁失败：{:?}", e);
                }
            }
            "OK"
        }) {
            Ok(_) => { // 新建子线程成功
            },
            Err(e) => { // 新建子线程失败
                println!("[主线程] 创建子线程失败: {}", e);
            },
        };

    cthd::sleep(xwtm::ms(500));
    let result = GLOBAL_MUTEX.lock();
    match result {
        Ok(mut guard) => {
            guard[0] += 1;
            println!("[主线程] 获取全局互斥锁成功。写全局数据[0]: {}", guard[0]);
        }
        Err(e) => {
            println!("[主线程] 获取全局互斥锁失败：{:?}", e);
        }
    };

    let result = local_mutex.lock();
    match result {
        Ok(mut guard) => {
            *guard += 1;
            println!("[主线程] 获取局部互斥锁成功。写局部数据: {}", *guard);
        }
        Err(e) => {
            println!("[主线程] 获取局部互斥锁失败：{:?}", e);
        }
    };
}
