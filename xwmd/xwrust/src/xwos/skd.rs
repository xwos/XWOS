//! XWOS RUST：调度器
//! ========
//!

use crate::types::*;

pub enum Context {
    InitExit,
    Thd,
    Isr(XwIrq),
    Bh,
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
        0 => Context::InitExit,
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
