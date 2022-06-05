//! XWOS RUST：SOC
//! ========
//!

extern crate core;
use core::ffi::*;
use core::mem;

use crate::types::*;
use crate::pin;


extern "C" {
    fn xwds_gpio_req(soc: *mut Soc, port: XwId, pinmsk: XwSq) -> XwEr;
    fn xwds_gpio_rls(soc: *mut Soc, port: XwId, pinmsk: XwSq) -> XwEr;
    fn xwds_gpio_cfg(soc: *mut Soc, port: XwId, pinmsk: XwSq,
                     cfg: *mut c_void) -> XwEr;
    fn xwds_gpio_set(soc: *mut Soc, port: XwId, pinmsk: XwSq) -> XwEr;
    fn xwds_gpio_reset(soc: *mut Soc, port: XwId, pinmsk: XwSq) -> XwEr;
    fn xwds_gpio_toggle(soc: *mut Soc, port: XwId, pinmsk: XwSq) -> XwEr;
    fn xwds_gpio_output(soc: *mut Soc, port: XwId, pinmsk: XwSq,
                        out: XwSq) -> XwEr;
    fn xwds_gpio_input(soc: *mut Soc, port: XwId, pinmsk: XwSq,
                       inbuf: *mut XwSq) -> XwEr;

    fn xwds_eirq_req(soc: *mut Soc, port: XwId, pinmsk: XwSq,
                     eirq: XwId, eiflag: XwSq,
                     isr: extern "C" fn(soc: *mut Soc, eirq: XwId, arg: *mut c_void),
                     arg: *mut c_void) -> XwEr;
    fn xwds_eirq_rls(soc: *mut Soc, port: XwId, pinmsk: XwSq,
                     eirq: XwId) -> XwEr;
}

/// XWOS自旋锁占用的内存大小
#[cfg(target_pointer_width = "32")]
pub const SIZEOF_XWDS_SOC: usize = 128;

/// XWOS自旋锁占用的内存大小
#[cfg(target_pointer_width = "64")]
pub const SIZEOF_XWDS_SOC: usize = 256;

#[repr(C)]
#[cfg_attr(target_pointer_width = "32", repr(align(8)))]
#[cfg_attr(target_pointer_width = "64", repr(align(16)))]
pub struct Soc {
    pub(crate) obj: [u8; SIZEOF_XWDS_SOC],
}

/// GPIO 端口
pub enum GpioPort {
    A = 0,
    B,
    C,
    D,
    E,
    F,
    G,
    H,
    I,
    J,
    K,
    L,
    M,
    N,
    O,
    P,
    Q,
    R,
    S,
    T,
    U,
    V,
    W,
    X,
    Y,
    Z,
}

pub trait Gpio {
    fn gpio_req(&self, port: GpioPort, pinmsk: XwSq) -> XwEr;
    fn gpio_rls(&self, port: GpioPort, pinmsk: XwSq) -> XwEr;
    fn gpio_cfg(&self, port: GpioPort, pinmsk: XwSq, cfg: *mut c_void) -> XwEr;
    fn gpio_set(&self, port: GpioPort, pinmsk: XwSq) -> XwEr;
    fn gpio_reset(&self, port: GpioPort, pinmsk: XwSq) -> XwEr;
    fn gpio_toggle(&self, port: GpioPort, pinmsk: XwSq) -> XwEr;
    fn gpio_output(&self, port: GpioPort, pinmsk: XwSq, out: XwSq) -> XwEr;
    fn gpio_input(&self, port: GpioPort, pinmsk: XwSq) -> (XwEr, XwSq);
}

impl Gpio for Soc {
    fn gpio_req(&self, port: GpioPort, pinmsk: XwSq) -> XwEr {
        unsafe {
            xwds_gpio_req(self as *const Self as *mut Self,
                          port as XwSq, pinmsk)
        }
    }

    fn gpio_rls(&self, port: GpioPort, pinmsk: XwSq) -> XwEr {
        unsafe {
            xwds_gpio_rls(self as *const Self as *mut Self,
                          port as XwSq, pinmsk)
        }
    }

    fn gpio_cfg(&self, port: GpioPort, pinmsk: XwSq, cfg: *mut c_void) -> XwEr {
        unsafe {
            xwds_gpio_cfg(self as *const Self as *mut Self,
                          port as XwSq, pinmsk, cfg)
        }
    }

    fn gpio_set(&self, port: GpioPort, pinmsk: XwSq) -> XwEr {
        unsafe {
            xwds_gpio_set(self as *const Self as *mut Self,
                          port as XwSq, pinmsk)
        }
    }

    fn gpio_reset(&self, port: GpioPort, pinmsk: XwSq) -> XwEr {
        unsafe {
            xwds_gpio_reset(self as *const Self as *mut Self,
                            port as XwSq, pinmsk)
        }
    }

    fn gpio_toggle(&self, port: GpioPort, pinmsk: XwSq) -> XwEr {
        unsafe {
            xwds_gpio_toggle(self as *const Self as *mut Self,
                             port as XwSq, pinmsk)
        }
    }

    fn gpio_output(&self, port: GpioPort, pinmsk: XwSq, out: XwSq) -> XwEr {
        unsafe {
            xwds_gpio_output(self as *const Self as *mut Self,
                             port as XwSq, pinmsk, out)
        }
    }

    fn gpio_input(&self, port: GpioPort, pinmsk: XwSq) -> (XwEr, XwSq) {
        unsafe {
            let mut inbuf: XwSq = 0;
            let rc = xwds_gpio_input(self as *const Self as *mut Self,
                                     port as XwSq, pinmsk, &mut inbuf);
            (rc, inbuf)
        }
    }
}

/// 外部中断标志
pub struct EirqFlag(XwSq);

impl EirqFlag {
    /// 上升沿触发
    pub const RISING: XwSq = pin!(0);
    /// 下降沿触发
    pub const FALLING: XwSq = pin!(1);
    /// 任意边沿触发
    pub const EITHER: XwSq = pin!(0, 1);
    /// 低电平触发
    pub const LOW: XwSq = pin!(2);
    /// 高电平触发
    pub const HIGH: XwSq = pin!(3);
    /// 触发后唤醒系统
    pub const WKUP: XwSq = pin!(4);
    /// 触发后启动DMA传输
    pub const DMA: XwSq = pin!(5);
}

impl From<XwSq> for EirqFlag {
    fn from(flag: XwSq) -> Self {
        Self(flag)
    }
}

pub trait Eirq {
    fn eirq_req(&self, port: GpioPort, pinmsk: XwSq, eirq: XwId, flag: EirqFlag,
                isr: fn(&Self, XwId)) -> XwEr;
    fn eirq_rls(&self, port: GpioPort, pinmsk: XwSq, eirq: XwId) -> XwEr;
}

impl Eirq for Soc {
    fn eirq_req(&self, port: GpioPort, pinmsk: XwSq, eirq: XwId, flag: EirqFlag,
                isr: fn(&Self, XwId)) -> XwEr {
        unsafe {
            xwds_eirq_req(self as *const Self as *mut Self,
                          port as XwSq, pinmsk, eirq, flag.0,
                          Soc::eisr_entry, isr as *mut c_void)
        }
    }

    fn eirq_rls(&self, port: GpioPort, pinmsk: XwSq, eirq: XwId) -> XwEr {
        unsafe {
            xwds_eirq_rls(self as *const Self as *mut Self,
                          port as XwSq, pinmsk, eirq)
        }
    }
}

impl Soc {
    extern "C" fn eisr_entry(rawsoc: *mut Soc, eirq: XwId, arg: *mut c_void) {
        unsafe {
            let soc = &*(rawsoc);
            let isr = mem::transmute::<*mut c_void, fn(&Self, XwId)>(arg);
            isr(soc, eirq);
        }
    }
}
