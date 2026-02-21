//! XWOS RUST：电源管理
//! ========
//!

extern crate core;
use core::ffi::*;
use core::ptr;
use core::cmp::{PartialOrd, Ordering};

use crate::types::*;

extern "C" {
    fn xwrustffi_pm_set_cb(resume: PmCallback,
                           suspend: PmCallback,
                           wakeup: PmCallback,
                           sleep: PmCallback,
                           arg: *mut c_void);
    fn xwrustffi_pm_suspend();
    fn xwrustffi_pm_resume();
    fn xwrustffi_pm_get_stage() -> XwSq;
}

/// 电源管理回调函数类型
pub type PmCallback = extern "C" fn(*mut c_void);

/// 设置电源管理的回调函数
///
/// # 参数说明
///
/// + resume: 从暂停模式恢复的回调函数
/// + suspend: 进入暂停模式的回调函数
/// + wakeup: 唤醒时回调函数
/// + sleep: 休眠时的回调函数
pub fn set_cb(resume: PmCallback, suspend: PmCallback,
              wakeup: PmCallback, sleep: PmCallback) {
    unsafe {
        xwrustffi_pm_set_cb(resume, suspend, wakeup, sleep, ptr::null_mut());
    }
}

/// 将系统切换为低功耗状态
///
/// 调用此方法后，所有线程都将开始冻结。冻结完成后，系统开始进入低功耗状态。
///
/// # 上下文
///
/// + 任意
pub fn suspend() {
    unsafe {
        xwrustffi_pm_suspend();
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
pub fn resume() {
    unsafe {
        xwrustffi_pm_resume();
    }
}

pub struct PmStage(XwSq);

impl PmStage {
    /// 已经暂停
    pub const SUSPENDED: PmStage = PmStage(0);
    /// 正在暂停
    pub const SUSPENDING: PmStage = PmStage(1);
    /// 正在恢复
    pub const RESUMING: PmStage = PmStage(1);
    /// 正在冻结线程
    pub const FREEZING: PmStage = PmStage(3);
    /// 正在解冻线程
    pub const THAWING: PmStage = PmStage(3);
    /// 正常运行
    pub const RUNNING: PmStage = PmStage(4);
}

/// 获取当前电源管理阶段
///
/// 电源管理是复杂的异步操作，当系统正在进入低功耗、或从低功耗唤醒时可通过此函数获取进展的阶段。
///
/// 返回值是枚举 [`PmStage`] 。
pub fn get_stage() -> PmStage {
    unsafe {
        PmStage(xwrustffi_pm_get_stage())
    }
}

impl PartialEq for PmStage {
    fn eq(&self, other: &Self) -> bool {
        self.0 == other.0
    }
}

impl PartialOrd for PmStage {
    fn partial_cmp(&self, other: &Self) -> Option<Ordering> {
        if self.0 < other.0 {
            Some(Ordering::Less)
        } else if self.0 > other.0 {
            Some(Ordering::Greater)
        } else {
            Some(Ordering::Equal)
        }
    }
}
