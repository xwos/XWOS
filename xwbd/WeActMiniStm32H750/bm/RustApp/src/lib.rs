//! XWOS RUST 应用
//!
//! # License: [MIT](https://opensource.org/licenses/MIT)
//!

#![no_std]
#![allow(non_snake_case)]

pub mod button;

use libc_print::std_name::println;

use xwrust::xwtm;
use xwrust::xwmm::allocator::AllocatorMempool;
use xwrust::xwos::cthd;

use xwrust_example_vector::xwrust_example_vector;
use xwrust_example_dthd::xwrust_example_dthd;
use xwrust_example_sthd::xwrust_example_sthd;
use xwrust_example_swt::xwrust_example_swt;
use xwrust_example_mutex::xwrust_example_mutex;
use xwrust_example_spinlock::xwrust_example_spinlock;
use xwrust_example_seqlock::xwrust_example_seqlock;
use xwrust_example_sem::xwrust_example_sem;
use xwrust_example_cond::xwrust_example_cond;
use xwrust_example_flg::xwrust_example_flg;
use xwrust_example_br::xwrust_example_br;
use xwrust_example_sel::xwrust_example_sel;
use xwrust_example_xwcq::xwrust_example_xwcq;
use xwrust_example_xwmq::xwrust_example_xwmq;

#[global_allocator]
pub static GLOBAL_ALLOCATOR: AllocatorMempool = AllocatorMempool;

#[no_mangle]
pub unsafe extern "C" fn xwrust_main() {
    println!("XWOS RUST APP");
    button::start();

    xwrust_example_vector();
    cthd::sleep(xwtm::ms(300));
    xwrust_example_dthd();
    cthd::sleep(xwtm::ms(300));
    xwrust_example_sthd();
    cthd::sleep(xwtm::ms(300));
    xwrust_example_swt();
    cthd::sleep(xwtm::ms(300));
    xwrust_example_mutex();
    cthd::sleep(xwtm::ms(300));
    xwrust_example_spinlock();
    cthd::sleep(xwtm::ms(300));
    xwrust_example_seqlock();
    cthd::sleep(xwtm::ms(300));
    xwrust_example_sem();
    cthd::sleep(xwtm::ms(300));
    xwrust_example_cond();
    cthd::sleep(xwtm::ms(300));
    xwrust_example_flg();
    cthd::sleep(xwtm::ms(300));
    xwrust_example_br();
    cthd::sleep(xwtm::ms(3000));
    xwrust_example_sel();
    cthd::sleep(xwtm::ms(300));
    xwrust_example_xwmq();
    cthd::sleep(xwtm::ms(300));
    xwrust_example_xwcq();
}
