//! XWOS RUST：调度器
//! ========
//!

use crate::types::*;

/// 上下文枚举
pub enum Context {
    /// 启动
    Boot,
    /// 线程
    Thd,
    /// 中断（中断号）
    Isr(XwIrq),
    /// 中断底半部
    Bh,
    /// 空闲任务
    Idle,
}

extern "C" {
    fn xwrustffi_skd_get_context_lc(ctxbuf: *mut XwSq, irqnbuf: *mut XwIrq);
    fn xwrustffi_skd_dspmpt_lc();
    fn xwrustffi_skd_enpmpt_lc();
}

/// 获取当前代码的上下文
///
///
/// # 示例
///
/// ```rust
/// use xwrust::xwos::skd;
///
/// let ctx = skd::context();
/// ```
pub fn context() -> Context
{
    let mut ctx: XwSq = 0;
    let mut irq: XwIrq = 0;

    unsafe { xwrustffi_skd_get_context_lc(&mut ctx, &mut irq); }
    match ctx {
        0 => Context::Boot,
        1 => Context::Thd,
        2 => Context::Isr(irq),
        3 => Context::Bh,
        _ => Context::Idle,
    }
}

/// 关闭当前CPU调度器的抢占
///
pub fn dspmpt() {
    unsafe { xwrustffi_skd_dspmpt_lc(); }
}

/// 打开当前CPU调度器的抢占
///
pub fn enpmpt() {
    unsafe { xwrustffi_skd_enpmpt_lc(); }
}
