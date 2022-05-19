//! XWOS RUST：XWOS的同步机制
//! ========
//!
//! 在XWOS中，信号量、条件量、事件标志、线程栅栏、信号选择器统称为 **同步对象** 。
//!

pub mod sem;
pub mod cond;
pub mod flg;
pub mod br;
pub mod sel;
