//! XWOS Rust Framework
//! ========
//!
//! # 简介
//!
//! XWOS RUST是基于XWOS的CAPI编写的RUST语言 `#![no_std]` 环境的框架，
//! 可让XWOS从C语言的RTOS进化成RUST语言的RTOS。
//!
//! 在编写XWOS RUST时，作者充分阅读并参考了RUST的std库的代码，
//! 尽量仿照std库的形式提供API。使得熟悉RUST语言的朋友更容易掌握如何使用XWOS RUST。
//!
//!
//! 传统RUST程序的入口是 `main.rs` 的 `fn main()` ，
//! XWOS RUST的入口是 `pub unsafe extern "C" fn xwrust_main()` 。
//! 用户需要在工程目录（例如电路板模块 `bm` 文件夹 或 `OEM` 文件夹）建立一个独立的 [**玄武模块**](../../docs/user-manual/build-system/#玄武模块) 来定义此函数。
//!
//! ```rust
//! // bm/rustapp/src/lib.rs
//! // crate-type = ["staticlib"]
//! #![no_std]
//!
//! #[global_allocator]
//! pub static ALLOCATOR: xwrust::xwmm::Allocator = xwrust::xwmm::Allocator;
//!
//! #[no_mangle]
//! pub unsafe extern "C" fn xwrust_main() {
//!     // 主线程的代码
//! }
//! ```
//!
//! 然后在XWOS的C语言入口 `xwos_main()` 中创建一个线程来调用 `xwrust_main()` 。
//!
//! ```C
//! extern void xwrust_main(void);
//! xwer_t xwrust_task(void * arg);
//!
//! xwos_thd_d xwrust_thd;
//! xwer_t xwos_main(void)
//! {
//!         struct xwos_thd_attr attr;
//!
//!         xwos_thd_attr_init(&attr);
//!         attr.name = "xwrust.thd";
//!         attr.stack = NULL;
//!         attr.stack_size = 8192;
//!         attr.priority = XWRUST_THD_PRIORITY;
//!         attr.detached = true;
//!         attr.privileged = true;
//!         xwos_thd_create(&xwrust_thd, &attr, xwrust_task, NULL);
//! }
//!
//! // 独立的RUST线程
//! xwer_t xwrust_task(void * arg)
//! {
//!         xwrust_main();
//! }
//! ```
//!
//! 因为需要与C语言编写的内核进行静态链接，用户应该创建的是基于 `lib.rs` 的静态库而非 `main.rs` 。
//!
//! ```toml
//! [package]
//! name = "rustapp"
//! version = "0.1.0"
//! edition = "2021"
//!
//! [lib]
//! name = "rustapp"
//! crate-type = ["staticlib"]
//! ```
//!
//! # XWOS RUST 的功能
//!
//! XWOS RUST提供了RTOS的基本功能：
//!
//! + [调度器](crate::xwos::skd)
//! + [电源管理](crate::xwos::pm)
//! + [线程](crate::xwos::thd)
//! + [软件定时器](crate::xwos::swt)
//! + [锁](crate::xwos::lock)
//!   + [互斥锁](crate::xwos::lock::mtx)
//!   + [自旋锁](crate::xwos::lock::spinlock)
//!   + [顺序锁](crate::xwos::lock::seqlock)
//! + [同步机制](crate::xwos::sync)
//!   + [信号量](crate::xwos::sync::sem)
//!   + [条件量](crate::xwos::sync::cond)
//!   + [事件标志](crate::xwos::sync::flg)
//!   + [线程栅栏](crate::xwos::sync::br)
//!   + [信号选择器](crate::xwos::sync::sel)
//! + 线程间通讯
//!   + [消息队列](crate::xwmd::xwmq)
//!   + [循环队列](crate::xwmd::xwcq)
//! + 其他
//!   + [时间](crate::xwtm)
//!   + [类型](crate::types)
//!   + [错误码](crate::errno)
//!   + [位图](crate::xwbmp)
//!
//! XWOS RUST所有功能都提供了 [`static`] 创建的方法，如果不使用 [`Arc<T>`], [`Box<T>`] 等，
//! 即可实现完全静态内存分配的代码。
//!
//!
//! # 返回XWOS首页
//!
//! [XWOS首页](/)
//!
//!
//! [`static`]: <https://doc.rust-lang.org/std/keyword.static.html>
//! [`Box<T>`]: <https://doc.rust-lang.org/alloc/boxed/struct.Box.html>
//! [`Arc<T>`]: <https://doc.rust-lang.org/alloc/sync/struct.Arc.html>

#![no_std]
#![feature(alloc_error_handler)]
#![feature(negative_impls)]
#![feature(generic_const_exprs)]
#![allow(incomplete_features)]

pub mod cfg;

#[macro_use]
pub mod macros;

pub mod types;

pub mod errno;

pub mod panic;

#[allow(dead_code)]
pub mod xwbmp;

pub mod xwtm;

pub mod xwmm;

#[allow(dead_code)]
pub mod xwos;

#[allow(dead_code)]
pub mod xwmd;

#[macro_use]
pub mod xwds;
