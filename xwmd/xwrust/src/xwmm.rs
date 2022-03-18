//! XWOS的RUST中间件：内存管理
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

use newlib_alloc;
#[global_allocator]
static GLOBAL_ALLOCATOR: newlib_alloc::Alloc = newlib_alloc::Alloc;

#[alloc_error_handler]
fn alloc_error_handler(_layout: core::alloc::Layout) -> ! {
    loop {}
}
