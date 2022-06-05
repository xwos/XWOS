//! XWOS RUST：SOC
//! ========
//!

extern crate core;
use core::cell::UnsafeCell;
use core::ffi::*;
use core::ops::*;
use core::mem;

use crate::types::*;
use crate::bit;


extern "C" {
    fn xwds_gpio_req(soc: *mut XwdsSoc, port: XwId, pinmsk: XwSq) -> XwEr;
    fn xwds_gpio_rls(soc: *mut XwdsSoc, port: XwId, pinmsk: XwSq) -> XwEr;
    fn xwds_gpio_cfg(soc: *mut XwdsSoc, port: XwId, pinmsk: XwSq,
                     cfg: *mut c_void) -> XwEr;
    fn xwds_gpio_set(soc: *mut XwdsSoc, port: XwId, pinmsk: XwSq) -> XwEr;
    fn xwds_gpio_reset(soc: *mut XwdsSoc, port: XwId, pinmsk: XwSq) -> XwEr;
    fn xwds_gpio_toggle(soc: *mut XwdsSoc, port: XwId, pinmsk: XwSq) -> XwEr;
    fn xwds_gpio_output(soc: *mut XwdsSoc, port: XwId, pinmsk: XwSq,
                        out: XwSq) -> XwEr;
    fn xwds_gpio_input(soc: *mut XwdsSoc, port: XwId, pinmsk: XwSq,
                       inbuf: *mut XwSq) -> XwEr;

    fn xwds_eirq_req(soc: *mut XwdsSoc, port: XwId, pinmsk: XwSq,
                     eirq: XwId, eiflag: XwSq,
                     isr: extern "C" fn(soc: *mut Soc, eirq: XwId, arg: *mut c_void),
                     arg: *mut c_void) -> XwEr;
    fn xwds_eirq_rls(soc: *mut XwdsSoc, port: XwId, pinmsk: XwSq,
                     eirq: XwId) -> XwEr;
}

/// XWOS自旋锁占用的内存大小
#[cfg(target_pointer_width = "32")]
pub const SIZEOF_XWDS_SOC: usize = 128;

/// XWOS自旋锁占用的内存大小
#[cfg(target_pointer_width = "64")]
pub const SIZEOF_XWDS_SOC: usize = 256;

/// SOC设备
#[repr(C)]
#[cfg_attr(target_pointer_width = "32", repr(align(8)))]
#[cfg_attr(target_pointer_width = "64", repr(align(16)))]
pub(crate) struct XwdsSoc {
    pub(crate) obj: [u8; SIZEOF_XWDS_SOC],
}

/// SOC设备
#[repr(C)]
pub struct Soc {
    pub(crate) soc: UnsafeCell<XwdsSoc>,
}

/// GPIO 端口
pub enum GpioPort {
    /// GPIO 端口 A
    A = 0,
    /// GPIO 端口 B
    B,
    /// GPIO 端口 C
    C,
    /// GPIO 端口 D
    D,
    /// GPIO 端口 E
    E,
    /// GPIO 端口 F
    F,
    /// GPIO 端口 G
    G,
    /// GPIO 端口 H
    H,
    /// GPIO 端口 I
    I,
    /// GPIO 端口 J
    J,
    /// GPIO 端口 K
    K,
    /// GPIO 端口 L
    L,
    /// GPIO 端口 M
    M,
    /// GPIO 端口 N
    N,
    /// GPIO 端口 O
    O,
    /// GPIO 端口 P
    P,
    /// GPIO 端口 Q
    Q,
    /// GPIO 端口 R
    R,
    /// GPIO 端口 S
    S,
    /// GPIO 端口 T
    T,
    /// GPIO 端口 U
    U,
    /// GPIO 端口 V
    V,
    /// GPIO 端口 W
    W,
    /// GPIO 端口 X
    X,
    /// GPIO 端口 Y
    Y,
    /// GPIO 端口 Z
    Z,
}

/// SOC的特性：GPIO
pub trait Gpio {
    /// 申请GPIO
    ///
    /// # 参数说明
    ///
    /// + port: GPIO端口，取值 [`GpioPort`]
    /// + pinmsk: GPIO引脚掩码，每一位代表一个引脚，可通过 [`pin!()`] 宏生成
    ///
    /// # 错误码
    ///
    /// + [`-ERANGE`] GPIO端口错误
    /// + [`-EBUSY`] GPIO引脚被占用
    ///
    /// [`pin!()`]: crate::pin!
    /// [`-ERANGE`]: crate::errno::ERANGE
    /// [`-EBUSY`]: crate::errno::EBUSY
    fn gpio_req(&self, port: GpioPort, pinmsk: XwSq) -> XwEr;

    /// 释放GPIO
    ///
    /// # 参数说明
    ///
    /// + port: GPIO端口，取值 [`GpioPort`]
    /// + pinmsk: GPIO引脚掩码，每一位代表一个引脚，可通过 [`pin!()`] 宏生成
    ///
    /// # 错误码
    ///
    /// + [`-ERANGE`] GPIO端口错误
    /// + [`-EPERM`] GPIO引脚未被申请
    ///
    /// [`pin!()`]: crate::pin!
    /// [`-ERANGE`]: crate::errno::ERANGE
    /// [`-EPERM`]: crate::errno::EPERM
    fn gpio_rls(&self, port: GpioPort, pinmsk: XwSq) -> XwEr;

    /// 配置GPIO
    ///
    /// # 参数说明
    ///
    /// + port: GPIO端口，取值 [`GpioPort`]
    /// + pinmsk: GPIO引脚掩码，每一位代表一个引脚，可通过 [`pin!()`] 宏生成
    /// + cfg: 配置数据，配置结构体的定义与平台相关
    ///
    /// # 错误码
    ///
    /// + [`-ERANGE`] GPIO端口错误
    /// + [`-EPERM`] GPIO引脚未被申请
    /// + [`-ENOSYS`] 芯片不支持此方法
    ///
    /// [`pin!()`]: crate::pin!
    /// [`-ERANGE`]: crate::errno::ERANGE
    /// [`-EPERM`]: crate::errno::EPERM
    /// [`-ENOSYS`]: crate::errno::ENOSYS
    fn gpio_cfg(&self, port: GpioPort, pinmsk: XwSq, cfg: *mut c_void) -> XwEr;

    /// 将GPIO设置为高电平
    ///
    /// # 参数说明
    ///
    /// + port: GPIO端口，取值 [`GpioPort`]
    /// + pinmsk: GPIO引脚掩码，每一位代表一个引脚，可通过 [`pin!()`] 宏生成
    ///
    /// # 错误码
    ///
    /// + [`-ERANGE`] GPIO端口错误
    /// + [`-EPERM`] GPIO引脚未被申请
    /// + [`-ENOSYS`] 芯片不支持此方法
    ///
    /// [`pin!()`]: crate::pin!
    /// [`-ERANGE`]: crate::errno::ERANGE
    /// [`-EPERM`]: crate::errno::EPERM
    /// [`-ENOSYS`]: crate::errno::ENOSYS
    fn gpio_set(&self, port: GpioPort, pinmsk: XwSq) -> XwEr;

    /// 将GPIO设置为低电平
    ///
    /// # 参数说明
    ///
    /// + port: GPIO端口，取值 [`GpioPort`]
    /// + pinmsk: GPIO引脚掩码，每一位代表一个引脚，可通过 [`pin!()`] 宏生成
    ///
    /// # 错误码
    ///
    /// + [`-ERANGE`] GPIO端口错误
    /// + [`-EPERM`] GPIO引脚未被申请
    /// + [`-ENOSYS`] 芯片不支持此方法
    ///
    /// [`pin!()`]: crate::pin!
    /// [`-ERANGE`]: crate::errno::ERANGE
    /// [`-EPERM`]: crate::errno::EPERM
    /// [`-ENOSYS`]: crate::errno::ENOSYS
    fn gpio_reset(&self, port: GpioPort, pinmsk: XwSq) -> XwEr;

    /// 翻转GPIO的电平
    ///
    /// # 参数说明
    ///
    /// + port: GPIO端口，取值 [`GpioPort`]
    /// + pinmsk: GPIO引脚掩码，每一位代表一个引脚，可通过 [`pin!()`] 宏生成
    ///
    /// # 错误码
    ///
    /// + [`-ERANGE`] GPIO端口错误
    /// + [`-EPERM`] GPIO引脚未被申请
    /// + [`-ENOSYS`] 芯片不支持此方法
    ///
    /// [`pin!()`]: crate::pin!
    /// [`-ERANGE`]: crate::errno::ERANGE
    /// [`-EPERM`]: crate::errno::EPERM
    /// [`-ENOSYS`]: crate::errno::ENOSYS
    fn gpio_toggle(&self, port: GpioPort, pinmsk: XwSq) -> XwEr;

    /// 写GPIO
    ///
    /// 输出的值 = `pinmsk` & `out`
    ///
    /// # 参数说明
    ///
    /// + port: GPIO端口，取值 [`GpioPort`]
    /// + pinmsk: GPIO引脚掩码，每一位代表一个引脚，可通过 [`pin!()`] 宏生成
    /// + out: 输出值，可通过 [`pin!()`] 宏生成
    ///
    /// # 错误码
    ///
    /// + [`-ERANGE`] GPIO端口错误
    /// + [`-EPERM`] GPIO引脚未被申请
    /// + [`-ENOSYS`] 芯片不支持此方法
    ///
    /// [`pin!()`]: crate::pin!
    /// [`-ERANGE`]: crate::errno::ERANGE
    /// [`-EPERM`]: crate::errno::EPERM
    /// [`-ENOSYS`]: crate::errno::ENOSYS
    fn gpio_output(&self, port: GpioPort, pinmsk: XwSq, out: XwSq) -> XwEr;

    /// 读取GPIO
    ///
    /// 读取的值 = `pinmsk` & GPIO的端口电平状态
    ///
    /// # 参数说明
    ///
    /// + port: GPIO端口，取值 [`GpioPort`]
    /// + pinmsk: GPIO引脚掩码，每一位代表一个引脚，可通过 [`pin!()`] 宏生成
    ///
    /// # 错误码
    ///
    /// + [`-ERANGE`] GPIO端口错误
    /// + [`-EPERM`] GPIO引脚未被申请
    /// + [`-ENOSYS`] 芯片不支持此方法
    ///
    /// [`pin!()`]: crate::pin!
    /// [`-ERANGE`]: crate::errno::ERANGE
    /// [`-EPERM`]: crate::errno::EPERM
    /// [`-ENOSYS`]: crate::errno::ENOSYS
    fn gpio_input(&self, port: GpioPort, pinmsk: XwSq) -> (XwEr, XwSq);
}

impl Gpio for Soc {
    fn gpio_req(&self, port: GpioPort, pinmsk: XwSq) -> XwEr {
        unsafe {
            xwds_gpio_req(self.soc.get(),
                          port as XwSq, pinmsk)
        }
    }

    fn gpio_rls(&self, port: GpioPort, pinmsk: XwSq) -> XwEr {
        unsafe {
            xwds_gpio_rls(self.soc.get(),
                          port as XwSq, pinmsk)
        }
    }

    fn gpio_cfg(&self, port: GpioPort, pinmsk: XwSq, cfg: *mut c_void) -> XwEr {
        unsafe {
            xwds_gpio_cfg(self.soc.get(),
                          port as XwSq, pinmsk, cfg)
        }
    }

    fn gpio_set(&self, port: GpioPort, pinmsk: XwSq) -> XwEr {
        unsafe {
            xwds_gpio_set(self.soc.get(),
                          port as XwSq, pinmsk)
        }
    }

    fn gpio_reset(&self, port: GpioPort, pinmsk: XwSq) -> XwEr {
        unsafe {
            xwds_gpio_reset(self.soc.get(),
                            port as XwSq, pinmsk)
        }
    }

    fn gpio_toggle(&self, port: GpioPort, pinmsk: XwSq) -> XwEr {
        unsafe {
            xwds_gpio_toggle(self.soc.get(),
                             port as XwSq, pinmsk)
        }
    }

    fn gpio_output(&self, port: GpioPort, pinmsk: XwSq, out: XwSq) -> XwEr {
        unsafe {
            xwds_gpio_output(self.soc.get(),
                             port as XwSq, pinmsk, out)
        }
    }

    fn gpio_input(&self, port: GpioPort, pinmsk: XwSq) -> (XwEr, XwSq) {
        unsafe {
            let mut inbuf: XwSq = 0;
            let rc = xwds_gpio_input(self.soc.get(),
                                     port as XwSq, pinmsk, &mut inbuf);
            (rc, inbuf)
        }
    }
}

/// 外部中断标志
pub struct EirqFlag(XwSq);

impl EirqFlag {
    /// 上升沿触发
    pub const RISING: EirqFlag = EirqFlag(bit!(0));
    /// 下降沿触发
    pub const FALLING: EirqFlag = EirqFlag(bit!(1));
    /// 任意边沿触发
    pub const EITHER: EirqFlag = EirqFlag(bit!(0, 1));
    /// 低电平触发
    pub const LOW: EirqFlag = EirqFlag(bit!(2));
    /// 高电平触发
    pub const HIGH: EirqFlag = EirqFlag(bit!(3));
    /// 触发后唤醒系统
    pub const WKUP: EirqFlag = EirqFlag(bit!(4));
    /// 触发后启动DMA传输
    pub const DMA: EirqFlag = EirqFlag(bit!(5));
}

impl From<XwSq> for EirqFlag {
    fn from(flag: XwSq) -> Self {
        Self(flag)
    }
}

impl BitAnd for EirqFlag {
    type Output = Self;

    fn bitand(self, other: Self) -> Self {
        Self(self.0 & other.0)
    }
}

impl BitAndAssign for EirqFlag {
    fn bitand_assign(&mut self, other: Self) {
        self.0 &= other.0;
    }
}

impl BitOr for EirqFlag {
    type Output = Self;

    fn bitor(self, other: Self) -> Self {
        Self(self.0 | other.0)
    }
}

impl BitOrAssign for EirqFlag {
    fn bitor_assign(&mut self, other: Self) {
        self.0 |= other.0;
    }
}

impl BitXor for EirqFlag {
    type Output = Self;

    fn bitxor(self, other: Self) -> Self {
        Self(self.0 ^ other.0)
    }
}

impl BitXorAssign for EirqFlag {
    fn bitxor_assign(&mut self, other: Self) {
        self.0 ^= other.0;
    }
}

/// SOC的特性：外部GPIO中断
pub trait Eirq {
    /// 申请外部中断
    ///
    /// # 参数说明
    ///
    /// + port: GPIO端口，取值 [`GpioPort`]
    /// + pinmsk: GPIO引脚掩码，每一位代表一个引脚，可通过 [`pin!()`] 宏生成
    /// + eirq: 外部中断号
    /// + flag: 外部中断标志，取值 [`EirqFlag`] 的常量组合
    /// + isr: 中断函数
    ///
    /// # 错误码
    ///
    /// + [`-ERANGE`] 外部中断ID错误
    ///
    /// [`pin!()`]: crate::pin!
    /// [`-ERANGE`]: crate::errno::ERANGE
    fn eirq_req(&self, port: GpioPort, pinmsk: XwSq, eirq: XwId, flag: EirqFlag,
                isr: fn(&Self, XwId)) -> XwEr;

    /// 释放外部中断
    ///
    /// # 参数说明
    ///
    /// + port: GPIO端口，取值 [`GpioPort`]
    /// + pinmsk: GPIO引脚掩码，每一位代表一个引脚，可通过 [`pin!()`] 宏生成
    /// + eirq: 外部中断号
    ///
    /// # 错误码
    ///
    /// + [`-ERANGE`] 外部中断ID错误
    ///
    /// [`pin!()`]: crate::pin!
    /// [`-ERANGE`]: crate::errno::ERANGE
    fn eirq_rls(&self, port: GpioPort, pinmsk: XwSq, eirq: XwId) -> XwEr;
}

impl Eirq for Soc {
    fn eirq_req(&self, port: GpioPort, pinmsk: XwSq, eirq: XwId, flag: EirqFlag,
                isr: fn(&Self, XwId)) -> XwEr {
        unsafe {
            xwds_eirq_req(self.soc.get(),
                          port as XwSq, pinmsk, eirq, flag.0,
                          Soc::eisr_entry, isr as *mut c_void)
        }
    }

    fn eirq_rls(&self, port: GpioPort, pinmsk: XwSq, eirq: XwId) -> XwEr {
        unsafe {
            xwds_eirq_rls(self.soc.get(),
                          port as XwSq, pinmsk, eirq)
        }
    }
}

impl Soc {
    /// 外部GPIO中断函数的入口
    extern "C" fn eisr_entry(rawsoc: *mut Soc, eirq: XwId, arg: *mut c_void) {
        unsafe {
            let soc = &*(rawsoc);
            let isr = mem::transmute::<*mut c_void, fn(&Self, XwId)>(arg);
            isr(soc, eirq);
        }
    }
}
