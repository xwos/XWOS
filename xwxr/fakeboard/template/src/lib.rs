//! XWOS RUST 应用模板
//!
//! # author
//!
//! + 隐星曜 (Roy Sun) <xwos@xwos.tech>
//!
//! # License: [MIT](https://opensource.org/licenses/MIT)
//!

#![no_std]

use xwrust::xwmm::allocator::AllocatorMempool;

#[global_allocator]
pub static GLOBAL_ALLOCATOR: AllocatorMempool = AllocatorMempool;

#[no_mangle]
pub unsafe extern "C" fn xwrust_main() {
}
