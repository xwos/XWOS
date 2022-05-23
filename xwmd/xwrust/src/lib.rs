//! XWOS Rust Framework
//! ========
//!
//! # 简介
//!
//! XWOS是纯C语言实现的RTOS内核。XWOS RUST基于XWOS的C-API编写的RUST语言 `#![no_std]` 环境中的框架代码，
//! 可让XWOS从C语言的RTOS进化成RUST语言的RTOS。
//!
//! 在编写XWOS RUST时，作者充分阅读并参考了RUST的std库的代码，
//! 尽量仿照std库的形式提供API。使得熟悉RUST语言的朋友可以很快掌握如何使用XWOS RUST。
//!
//!
//! 就像RUST程序的入口是 `main.rs` 的 `fn main()` 一样，
//! XWOS RUST的入口是 `pub unsafe extern "C" fn xwrust_main()` 。
//! 用户需要在工程目录（例如电路板模块 `bm` 文件夹 或 `OEM` 文件夹）建立一个独立的 **玄武模块** 来定义此函数。
//!
//! ```rust
//! // bm/rustapp/src/lib.rs
//! // crate-type = ["staticlib"]
//! #![no_std]
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
//! + [消息队列](crate::xwmd::xwmq)
//! + 其他
//!   + [时间](crate::xwtm)
//!   + [类型](crate::types)
//!   + [错误码](crate::errno)
//!   + [位图](crate::xwbmp)
//!
//!
//! # 返回XWOS首页
//!
//! [XWOS首页](/)

#![no_std]
#![feature(alloc_error_handler)]
#![feature(core_ffi_c)]
#![feature(negative_impls)]
#![feature(generic_const_exprs)]
#![allow(incomplete_features)]

#[macro_use]
pub mod macros;

pub mod types;

pub mod errno;

pub mod panic;

#[allow(dead_code)]
pub mod xwbmp;

pub mod xwtm;

#[allow(dead_code)]
pub mod xwos;

#[allow(dead_code)]
pub mod xwmd;

pub mod xwmm;
