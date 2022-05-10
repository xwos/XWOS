//! XWOS RUST：当前线程
//! ========
//!
//! 此模块中的方法，只会对代码所运行的线程起作用。命名中的 **c** 是 **current** 的意思。
//!
//!
//! # 获取自身的线程描述符
//!
//! 线程可通过方法 [`i()`] 获取自身的线程描述符。
//!
//! # 通知调度器重新调度线程
//!
//! 线程可通过方法 [`yield_now()`] 主动让出CPU的使用权。
//!
//! # 判断当前线程是否可被冻结
//!
//! 线程可通过方法 [`shld_frz()`] 判断是否需要冻结，如果是，可调用 [`freeze()`] 冻结自身。
//!
//! 线程的冻结只会发生在一些特殊的条件下：
//!
//! + 系统开始低功耗流程；
//! + 线程在CPU之间迁移。
//!
//! # 判断当前线程是否可退出
//!
//! 线程可通过方法 [`shld_stop()`] 判断是否被设置了 **退出状态** ，**退出状态** 是其他线程通过 [`ThdHandle::stop()`] 方法设置的。
//!
//! # 线程睡眠
//!
//! ## 睡眠
//!
//! 线程可以通过方法 [`sleep()`] 指定需要睡眠多长时间，线程唤醒的时间可能会略微超过指定的时长。
//!
//! ## 睡眠到指定的时间点
//!
//! 当需要到某个时间点唤醒时，可以通过 [`sleep_to()`] 方法睡眠。此方法不关心睡眠多长时间，只关心到哪个时间唤醒。
//!
//! ## 从一个时间起点睡眠到另一个时间点
//!
//! 方法 [`sleep_from()`] 可以指定从某个时间点开始睡眠，持续多长时间。
//!
//! # 冻结线程
//!
//! 当线程通过方法 [`shld_frz()`] 判断出需要冻结时，可通过方法 [`freeze()`] 冻结自身。冻结之前需要释放掉所有资源。
//!
//!
//! [`ThdHandle::stop()`]: crate::xwos::thd::ThdHandle::stop

extern crate core;
use core::ffi::*;
use core::ptr;

use crate::types::*;
use super::thd::ThdD;

extern "C" {
    fn xwrustffi_cthd_self(thd: *mut *mut c_void, tik: *mut XwSq);
    fn xwrustffi_cthd_yield();
    fn xwrustffi_cthd_shld_frz() -> bool;
    fn xwrustffi_cthd_shld_stop() -> bool;
    fn xwrustffi_cthd_sleep(time: XwTm) -> XwEr;
    fn xwrustffi_cthd_sleep_to(to: XwTm) -> XwEr;
    fn xwrustffi_cthd_sleep_from(origin: *mut XwTm,
                                 inc: XwTm) -> XwEr;
    fn xwrustffi_cthd_freeze() -> XwEr;
}


/// 获取当前线程的对象描述符
///
/// # 示例
///
/// ```rust
/// use use xwrust::xwos::cthd;
///
/// let thdd = cthd::i();
/// ```
pub fn i() -> ThdD {
    let mut thd: *mut c_void = ptr::null_mut();
    let mut tik: XwSq = 0;
    unsafe {
        xwrustffi_cthd_self(&mut thd, &mut tik);
    }
    ThdD {thd, tik}
}


/// 通知调度器重新调度线程
///
/// # 示例
///
/// ```rust
/// use use xwrust::xwos::cthd;
///
/// cthd::yield_now();
/// ```
pub fn yield_now() {
    unsafe { xwrustffi_cthd_yield(); }
}


/// 判断当前线程是否可被冻结
///
/// # 示例
///
/// ```rust
/// use use xwrust::xwos::cthd;
///
/// if cthd::shld_frz() {
///     cthd::freeze();
/// }
/// ```
pub fn shld_frz() -> bool {
    unsafe { xwrustffi_cthd_shld_frz() }
}


/// 判断当前线程是否可退出
///
/// # 示例
///
/// ```rust
/// use use xwrust::xwos::cthd;
///
/// if cthd::shld_stop() {
///     return result;
/// }
/// ```
pub fn shld_stop() -> bool {
    unsafe { xwrustffi_cthd_shld_stop() }
}


/// 当前线程睡眠一段时间
///
/// 调用此方法的线程会睡眠**dur**后被唤醒。
///
/// 此方法等价于 `cthd::sleep_to(xwtm::ft(xwtm::s(dur)))` 。
///
/// # 返回值
///
/// 0: 睡眠成功
/// -EINTR: 睡眠被中断
///
/// # 示例
///
/// ```rust
/// use use xwrust::xwos::cthd;
/// use use xwrust::xwtx;
///
/// let rc = cthd::sleep(xwtm::s(1)); // 睡眠1s
/// ```
pub fn sleep(dur: XwTm) -> XwEr {
    unsafe { xwrustffi_cthd_sleep(dur) }
}


/// 当前线程睡眠到一个时间点
///
/// 调用此方法的线程会睡眠到时间点**to**后被唤醒。
/// 如果**to**是过去的时间点，将直接返回 `-ETIMEDOUT` 。
///
/// # 返回值
///
/// + 0: 睡眠成功
/// + -EINTR: 睡眠被中断
/// + -ETIMEDOUT: 时间点是过去
///
/// # 示例
///
/// ```rust
/// use use xwrust::xwos::cthd;
/// use use xwrust::xwtx;
///
/// let rc = cthd::sleep_to(xwtm::ft(xwtm::s(1))); // 睡眠1s
/// ```
pub fn sleep_to(to: XwTm) -> XwEr {
    unsafe { xwrustffi_cthd_sleep_to(to) }
}


/// 当前线程从一个时间起点睡眠到另一个时间点
///
/// 调用此方法前，需要先确定一个时间起点 `from` ，可以通过 `xwtm::now()` 获取当前的系统时间作为起点。
/// 唤醒时间为 `from + dur` 。
/// 此方法会将线程唤醒时的系统时间返回，可作为下一次调用的时间起点。
///
/// # 返回值 (rc, from)
///
/// + rc
///   + 0: 睡眠成功
///   + -EINTR: 睡眠被中断
///   + -ETIMEDOUT: 时间点是过去
/// + from: 线程唤醒时的系统时间
///
/// # 示例
///
/// ```rust
/// use use xwrust::xwos::cthd;
/// use use xwrust::xwtx;
///
/// let mut from = xwtm::now();
/// loop {
///     let mut (rc, from) = cthd::sleep_from(from, xwtm::s(1)); // 从from开始睡眠1s
/// }
/// ```
pub fn sleep_from(from: XwTm, dur: XwTm) -> (XwEr, XwTm) {
    let mut origin: XwTm = from;
    unsafe {
        let rc = xwrustffi_cthd_sleep_from(&mut origin, dur);
        (rc, origin)
    }
}


/// 冻结当前线程
///
/// 线程并不能随时冻结，必须满足下列条件之一：
///
/// + 系统已经开始准备进入低功耗;
/// + 线程正准备开始迁移。
///
/// 线程可通过 [`shld_frz()`] 判断是否满足冻结条件。
///
/// 通常冻结操作之前，还需要对资源进行释放，以防止线程阻碍系统进入低功耗；
/// 通常解冻操作之后，还需要重新获取资源。
///
/// # 返回值
///
/// + 0: 睡眠成功
/// + -EPERM: 当前不需要冻结线程
///
/// # 示例
///
/// ```rust
/// use use xwrust::xwos::cthd;
///
/// if cthd::shld_frz() {
///     let rc = cthd::freeze();
/// }
/// ```
pub fn freeze() -> XwEr {
    unsafe { xwrustffi_cthd_freeze() }
}
