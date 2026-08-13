//! XWOS RUST 应用模板
//!
//! # author
//!
//! + 隐星曜 (Roy Sun) <xwos@xwos.tech>
//!
//! # License: [MIT](https://opensource.org/licenses/MIT)
//!

#![no_std]

use xwrust::xwmm::allocator::XwrustAllocator;

#[global_allocator]
pub static GLOBAL_ALLOCATOR: XwrustAllocator = XwrustAllocator;

#[no_mangle]
pub unsafe extern "C" fn xwrust_main() {
}
