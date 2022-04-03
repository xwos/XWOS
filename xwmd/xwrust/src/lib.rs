//! XWOS RUST Framework
//! ========
//!
//! # author
//! + 隐星魂 (Roy Sun) <xwos@xwos.tech>
//!
//! # copyright
//! + © 2015 xwos.tech, All Rights Reserved.
//! > This Source Code Form is subject to the terms of the Mozilla Public
//! > License, v. 2.0. If a copy of the MPL was not distributed with this
//! > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
//!
//!
//! # 简介
//!
//! XWOS是纯C语言实现的RTOS内核。XWOS RUST基于XWOS的C-API编写的RUST语言no_std环境中的框架代码，
//! 可让XWOS从C语言的RTOS进化成RUST语言的RTOS。
//!
//! 在编写XWOS RUST时，作者充分阅读并参考了RUST的std库的代码，
//! 尽量仿照std库的形式提供API。使得熟悉RUST语言的朋友可以很快掌握如何使用XWOS RUST。
//!
//! 就像RUST程序的入口是`main.rs`的`fn main()`一样，
//! XWOS RUST的入口是`pub unsafe extern "C" fn xwrust_main()` 。
//! 需要用户在工程目录（例如电路板模块文件夹bm或OEM文件）建立一个独立的玄武模块来定义此函数。
//! XWOS会创建一个线程来调用 `xwrust_main()` 。
//! 但是因为需要与C语言编写的内核进行静态链接，用户应该创建的是基于`lib.rs`的静态库而非`main.rs`。
//!
//! ```rust
//! // bm/rustapp/src/lib.rs
//! // crate-type = ["staticlib"]
//! #![no_std]
//! #[no_mangle]
//! pub unsafe extern "C" fn xwrust_main() {
//!     // 主线程的代码
//! }
//! ```

#![no_std]
#![feature(alloc_error_handler)]
#![feature(core_ffi_c)]

pub mod types;
pub mod panic;
#[allow(dead_code)]
pub mod xwos;
pub mod xwmm;
