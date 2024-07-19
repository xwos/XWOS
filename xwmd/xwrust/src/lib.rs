//! XWOS RUST
//! ========
//!
//! # 简介
//!
//! XWOS RUST是Rust语言 `#![no_std]` 环境下的XWOS框架，可让用户使用RUST语言开发RTOS的应用。
//!
//! 在编写XWOS RUST时，作者充分阅读并参考了RUST语言std库的代码，尽量仿照std库的形式提供API。
//!
//!
//! 传统RUST程序的入口是 `main.rs` 的 `fn main()` ，
//! XWOS RUST的入口是 `pub unsafe extern "C" fn xwrust_main()` 。
//!
//!
//! 编写XWOS RUST的应用时，需要以下步骤：
//!
//! + 1. 在工程目录（例如电路板模块 `bm` 文件夹 或 `$(OEM)` 文件夹）建立一个独立的 [**玄武模块**](../../Docs/TechRefManual/BuildSystem/#玄武模块) ，
//!   并创建 `makefile` 。
//!
//! ```makefile
//! include $(XWOS_WKSPC_DIR)/XWOS.cfg
//! include xwbs/functions.mk
//! include xwbs/xwmo.rust.mk
//! ```
//!
//! + 2. 创建 `Cargo.toml` 。
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
//!
//! [dependencies]
//! cortex-m = "0.7"
//! xwrust = {path = "../../../../xwmd/xwrust"}
//! ```
//!
//! + 3. 创建 `.cargo/config.toml` 。
//!
//! ```toml
//! [build]
//! rustflags = [
//!  "--cfg", "unix",
//!  "--cfg", "target_env=\"newlib\"",
//! ]
//! target-dir = "target"
//!
//! [unstable]
//! build-std = ["core", "alloc"]
//! ```
//!
//! + 4. 在 `src/lib.rs` 中实现 `pub unsafe extern "C" fn xwrust_main()` 。
//!
//! ```rust
//! #![no_std]
//!
//! use xwrust::xwmm::allocator::AllocatorMempool;
//!
//! #[global_allocator]
//! pub static GLOBAL_ALLOCATOR: AllocatorMempool = AllocatorMempool;
//!
//! #[no_mangle]
//! pub unsafe extern "C" fn xwrust_main() {
//!     // 用户代码
//! }
//! ```
//!
//! + 5. 在XWOS的C语言入口 `xwos_main()` 中创建一个线程来调用 `xwrust_main()` 。
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
//! xwer_t xwrust_task(void * arg)
//! {
//!         xwrust_main();
//! }
//! ```
//!
//!
//! # XWOS RUST 的功能
//!
//! XWOS RUST提供了RTOS的基本功能：
//!
//! + [内存管理](crate::xwmm)
//!   + [全局内存分配器](crate::xwmm::allocator)
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
//! [**XWOS首页**](/)
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
