//! XWOS的RUST应用
//!
//! # author
//! + 隐星魂 (Roy Sun) <xwos@xwos.tech>
//!
//! # copyright
//! + © 2015 xwos.tech, All Rights Reserved.
//! > This Source Code Form is subject to the terms of the Mozilla Public
//! > License, v. 2.0. If a copy of the MPL was not distributed with this
//! > file, You can obtain one at http://mozilla.org/MPL/2.0/.
//!

#![no_std]

#[global_allocator]
pub static ALLOCATOR: xwrust::xwmm::Allocator = xwrust::xwmm::Allocator;

extern crate alloc;
use alloc::vec::Vec;
use libc_print::std_name::println;

#[no_mangle]
pub unsafe extern "C" fn xwrust_main() {
    let mut v = Vec::with_capacity(16);
    v.push(1);
    v.push(2);
    v.push(3);
    v.push(4);
    println!("RUST XWOS!");
    for x in v.iter() {
        println!("x: {}", x);
    }
}
