//! XWOS RUST Example: Vector
//! ========
//!

#![no_std]

extern crate alloc;
use alloc::vec::Vec;

use libc_print::std_name::println;

pub fn xwrust_example_vector() {
    println!("XWOS RUST Example: Vector");
    let mut v = Vec::with_capacity(16);
    v.push(1);
    v.push(2);
    v.push(3);
    v.push(4);
    for x in v.iter() {
        println!("x: {}", x);
    }
}
