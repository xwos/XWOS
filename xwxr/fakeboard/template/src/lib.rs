//! XWOS RUST 应用模板
//!
//! # author
//!
//! + 隐星魂 (Roy Sun) <xwos@xwos.tech>
//!
//! # License: [MIT](https://opensource.org/licenses/MIT)
//!

#![no_std]

use libc_print::std_name::println;

#[global_allocator]
pub static ALLOCATOR: xwrust::xwmm::Allocator = xwrust::xwmm::Allocator;

#[no_mangle]
pub unsafe extern "C" fn xwrust_main() {
    println!("XWOS RUST");
}
