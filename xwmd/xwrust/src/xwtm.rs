//! XWOS RUST：系统时间
//! ========
//!
//! XWOS的系统时间库。
//!

use crate::types::*;

extern "C" {
    fn xwrustffi_xwtm_now() -> XwTm;
    fn xwrustffi_xwtm_ft(time: XwTm) -> XwTm;
    fn xwrustffi_xwtm_nowts() -> XwTm;
    fn xwrustffi_xwtm_fts(time: XwTm) -> XwTm;
    fn xwrustffi_xwtm_nowtc() -> u64;
}

/// 1微秒
pub const MICROSECOND: XwTm = 1000;
/// 1毫秒
pub const MILLISECOND: XwTm = 1000000;
/// 1秒
pub const SECOND: XwTm = 1000000000;
/// 1分
pub const MINUTE: XwTm = 60000000000;
/// 1小时
pub const HOUR: XwTm = 3600000000000;
/// 1天
pub const DAY: XwTm = 86400000000000;
/// 最大值
pub const MAX: XwTm = i64::MAX;


/// 获取当前CPU的**系统时间**点
///
/// # 示例
///
/// ```rust
/// use use xwrust::xwtm;
///
/// let now = xwtm::now();
/// ```
pub fn now() -> XwTm {
    unsafe { xwrustffi_xwtm_now() }
}


/// 获取当前CPU的未来**系统时间**点
///
/// # 示例
///
/// ```rust
/// use use xwrust::xwtm;
///
/// let ftp = xwtm::ft(xwtm::s(1));
/// ```
pub fn ft(time: XwTm) -> XwTm {
    unsafe { xwrustffi_xwtm_ft(time) }
}


/// 获取当前CPU的**系统时间戳**
///
/// # 示例
///
/// ```rust
/// use use xwrust::xwtm;
///
/// let ts = xwtm::nowts();
/// ```
pub fn nowts() -> XwTm {
    unsafe { xwrustffi_xwtm_nowts() }
}


/// 获取当前CPU的未来**系统时间戳**
///
/// # 示例
///
/// ```rust
/// use use xwrust::xwtm;
///
/// let ftp = xwtm::fts(xwtm::s(1));
/// ```
pub fn fts(time: XwTm) -> XwTm {
    unsafe { xwrustffi_xwtm_fts(time) }
}


/// 获取当前CPU的系统滴答计数
///
/// # 示例
///
/// ```rust
/// use use xwrust::xwtm;
///
/// let cnt = xwtm::nowtc();
/// ```
pub fn nowtc() -> u64 {
    unsafe { xwrustffi_xwtm_nowtc() }
}


/// 返回以微秒为单位的系统时间
///
/// # 示例
///
/// ```rust
/// use use xwrust::xwtm;
///
/// let tp = xwtm::us(8); // 8微妙
/// ```
pub fn us(us: XwTm) -> XwTm {
    us * MICROSECOND
}


/// 返回以毫秒为单位的系统时间
///
/// # 示例
///
/// ```rust
/// use use xwrust::xwtm;
///
/// let tp = xwtm::ms(8); // 8毫妙
/// ```
pub fn ms(ms: XwTm) -> XwTm {
    ms * MILLISECOND
}


/// 返回以秒为单位的系统时间
///
/// # 示例
///
/// ```rust
/// use use xwrust::xwtm;
///
/// let tp = xwtm::s(8); // 8妙
/// ```
pub fn s(s: XwTm) -> XwTm {
    s * SECOND
}


/// 返回以分为单位的系统时间
///
/// # 示例
///
/// ```rust
/// use use xwrust::xwtm;
///
/// let tp = xwtm::m(8); // 8分
/// ```
pub fn m(m: XwTm) -> XwTm {
    m * MINUTE
}


/// 返回以小时为单位的系统时间
///
/// # 示例
///
/// ```rust
/// use use xwrust::xwtm;
///
/// let tp = xwtm::h(8); // 8小时
/// ```
pub fn h(h: XwTm) -> XwTm {
    h * HOUR
}


/// 返回以天为单位的系统时间
///
/// # 示例
///
/// ```rust
/// use use xwrust::xwtm;
///
/// let tp = xwtm::d(8); // 8天
/// ```
pub fn d(d: XwTm) -> XwTm {
    d * DAY
}
