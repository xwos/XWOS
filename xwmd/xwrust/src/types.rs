//! XWOS RUST：基本类型
//! ========
//!

/// 寄存器类型，位宽与指针一样
pub type XwReg = usize;
/// 有符号寄存器类型，位宽与指针一样
pub type XwSreg = isize;
/// 错误类型，有符号，位宽与指针一样，取值通常负数，取值范围参考libc的头文件 `errno.h`
pub type XwEr = isize;
/// 位图
pub type XwBmp = usize;
/// 大小值，无符号，位宽与指针一样
pub type XwSz = usize;
/// 大小值，有符号，位宽与指针一样
pub type XwSsz = isize;
/// 栈，无符号，位宽与指针一样
pub type XwStk = usize;
/// 顺序值，无符号，位宽与指针一样
pub type XwSq = usize;
/// 顺序值，有符号，位宽与指针一样
pub type XwSsq = isize;
/// ID，无符号，位宽与指针一样
pub type XwId = usize;
/// ID，有符号，位宽与指针一样
pub type XwSid = isize;
/// 中断号，有符号，32位
pub type XwIrq = i32;
/// 优先级，有符号，32位
pub type XwPr = i32;
/// XWOS的时间值，有符号，64位，单位：纳秒
pub type XwTm = i64;
