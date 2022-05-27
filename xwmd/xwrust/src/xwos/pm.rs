//! XWOS RUST：电源管理
//! ========
//!

use crate::types::*;

extern "C" {
    fn xwrustffi_pm_suspend() -> XwEr;
    fn xwrustffi_pm_resume() -> XwEr;
}

/// 将系统切换为低功耗状态
///
/// 调用此方法后，所有线程都将开始冻结。冻结完成后，系统开始进入低功耗状态。
///
/// # 错误码
///
/// + [`XWOK`] 没有错误
/// + [`-EACCES`] 系统不是运行状态
///
/// # 上下文
///
/// + 任意
///
/// [`XWOK`]: crate::errno::XWOK
/// [`-EACCES`]: crate::errno::EACCES
pub fn suspend() -> XwEr {
    unsafe {
        xwrustffi_pm_suspend()
    }
}

/// 唤醒系统
///
/// 只可在唤醒中断中调用。
///
/// # 错误码
///
/// + [`XWOK`] 成功
/// + [`-EALREADY`] 系统正在运行
///
/// # 上下文
///
/// + 中断
///
/// [`XWOK`]: crate::errno::XWOK
/// [`-EALREADY`]: crate::errno::EALREADY
pub fn resume() -> XwEr {
    unsafe {
        xwrustffi_pm_resume()
    }
}
