//! XWOS RUST Example: Thread
//! ========
//!

#![no_std]

use xwrust::xwtm;
use xwrust::xwos::thd;
use xwrust::xwos::cthd;

use libc_print::std_name::println;

pub fn xwrust_example_thd() {
    println!("XWOS RUST Example: Thread");

    println!("[主线程]<{} ms> thd: {:?}", xwtm::nowtc(), cthd::i());
    match thd::Builder::new()
        .name("child".into())
        .spawn(move |ele| {
            // 子线程闭包
            println!("[子线程]<{} ms> name: {}, thd: {:?}", xwtm::nowtc(), ele.name().unwrap(), cthd::i());

            let mut count = 0;
            loop {
                cthd::sleep(xwtm::s(1));
                count += 1;
                println!("[子线程]<{} ms> 计数：{}", xwtm::nowtc(), count);
                if cthd::shld_frz() { // 判断是否要冻结子线程
                    cthd::freeze();
                }
                if cthd::shld_stop() { // 判断是否要退出子线程
                    break;
                }
            }
            "OK"
        }) {
            Ok(h) => { // 新建子线程成功
                let mut child = h;
                loop {
                    println!("[主线程] 睡眠 3s ...");
                    cthd::sleep(xwtm::s(3));
                    println!("[主线程]<{} ms> 唤醒", xwtm::nowtc());
                    println!("[主线程] 停止子线程 ...");
                    match child.stop() { // stop() = quit() + join()
                        Ok(r) => {
                            println!("[主线程]<{} ms> 子线程返回值：{}", xwtm::nowtc(), r);
                            break;
                        },
                        Err(e) => {
                            println!("[主线程]<{} ms> 停止子线程失败: {}",
                                     xwtm::nowtc(), e.join_state());
                            child = e;
                        },
                    }
                    if cthd::shld_frz() { // 判断是否要冻结主线程
                        cthd::freeze();
                    }
                    if cthd::shld_stop() { // 判断是否要退出主线程
                        break;
                    }
                }
            },
            Err(e) => { // 新建子线程失败
                println!("[主线程]<{} ms> 创建子线程失败: {}", xwtm::nowtc(), e);
            },
        }
}
