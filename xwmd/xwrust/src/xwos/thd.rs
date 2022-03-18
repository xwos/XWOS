//! XWOS的RUST中间件：线程库
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

pub struct XwosThd {
    tid: u32,
}

unsafe impl Send for XwosThd {}
unsafe impl Sync for XwosThd {}

impl XwosThd {
}
