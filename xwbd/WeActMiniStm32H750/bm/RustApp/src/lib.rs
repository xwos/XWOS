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

use xwrust_sample_vector::xwrust_sample_vector;
use xwrust_sample_dthd::xwrust_sample_dthd;
use xwrust_sample_sthd::xwrust_sample_sthd;
use xwrust_sample_swt::xwrust_sample_swt;
use xwrust_sample_mutex::xwrust_sample_mutex;
use xwrust_sample_spinlock::xwrust_sample_spinlock;
use xwrust_sample_seqlock::xwrust_sample_seqlock;
use xwrust_sample_sem::xwrust_sample_sem;
use xwrust_sample_cond::xwrust_sample_cond;
use xwrust_sample_flg::xwrust_sample_flg;
use xwrust_sample_br::xwrust_sample_br;
use xwrust_sample_sel::xwrust_sample_sel;
use xwrust_sample_xwcq::xwrust_sample_xwcq;
use xwrust_sample_xwmq::xwrust_sample_xwmq;

#[global_allocator]
pub static GLOBAL_ALLOCATOR: AllocatorMempool = AllocatorMempool;

#[no_mangle]
pub unsafe extern "C" fn xwrust_main() {
    println!("XWOS RUST APP");
    button::start();

    xwrust_sample_vector();
    cthd::sleep(xwtm::ms(300));
    xwrust_sample_dthd();
    cthd::sleep(xwtm::ms(300));
    xwrust_sample_sthd();
    cthd::sleep(xwtm::ms(300));
    xwrust_sample_swt();
    cthd::sleep(xwtm::ms(300));
    xwrust_sample_mutex();
    cthd::sleep(xwtm::ms(300));
    xwrust_sample_spinlock();
    cthd::sleep(xwtm::ms(300));
    xwrust_sample_seqlock();
    cthd::sleep(xwtm::ms(300));
    xwrust_sample_sem();
    cthd::sleep(xwtm::ms(300));
    xwrust_sample_cond();
    cthd::sleep(xwtm::ms(300));
    xwrust_sample_flg();
    cthd::sleep(xwtm::ms(300));
    xwrust_sample_br();
    cthd::sleep(xwtm::ms(3000));
    xwrust_sample_sel();
    cthd::sleep(xwtm::ms(300));
    xwrust_sample_xwmq();
    cthd::sleep(xwtm::ms(300));
    xwrust_sample_xwcq();
}
