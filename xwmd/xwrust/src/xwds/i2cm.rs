//! XWOS RUST：I2C主机模式控制器
//! ========
//!

extern crate core;
use core::cell::UnsafeCell;
use core::ops::*;

use crate::types::*;
use crate::bit;

extern "C" {
    fn xwds_i2cm_xfer(i2cm: *mut XwdsI2cm,
                      msg: *mut Msg,
                      to: XwTm) -> XwEr;
    fn xwds_i2cm_abort(i2cm: *mut XwdsI2cm,
                       address: u16, addrmode: u16,
                       to: XwTm) -> XwEr;
}

/// XWOS自旋锁占用的内存大小
#[cfg(target_pointer_width = "32")]
pub const SIZEOF_XWDS_I2CM: usize = 224;

/// XWOS自旋锁占用的内存大小
#[cfg(target_pointer_width = "64")]
pub const SIZEOF_XWDS_I2CM: usize = 448;

/// I2C主机模式控制器
#[repr(C)]
#[cfg_attr(target_pointer_width = "32", repr(align(8)))]
#[cfg_attr(target_pointer_width = "64", repr(align(16)))]
pub struct XwdsI2cm {
    pub(crate) obj: [u8; SIZEOF_XWDS_I2CM],
}

/// I2C主机模式控制器
#[repr(C)]
pub struct I2cm {
    pub(crate) i2cm: UnsafeCell<XwdsI2cm>,
}

impl I2cm {
    /// 传输I2C消息
    ///
    /// # 参数说明
    ///
    /// + addr: 外设地址
    /// + flag: 传输消息标志，取值 [`MsgFlag`] 的常量组合
    /// + data: 数据数组的切片
    /// + size: 数据大小
    /// + to: 期望唤醒的时间点
    ///
    /// # 错误码
    ///
    /// + [`-ESHUTDOWN`] I2C主机模式控制器没有运行
    /// + [`-EADDRNOTAVAIL`] I2C地址没有应答
    /// + [`-ETIMEDOUT`] 传输超时
    ///
    /// [`-ESHUTDOWN`]: crate::errno::ESHUTDOWN
    /// [`-EADDRNOTAVAIL`]: crate::errno::EADDRNOTAVAIL
    /// [`-ETIMEDOUT`]: crate::errno::ETIMEDOUT
    pub fn xfer(&self,
                addr: u16, flag: MsgFlag, data: &mut [u8], size: XwSz,
                to: XwTm) -> XwEr {
        unsafe {
            let mut msg: Msg = Msg {
                addr: addr,
                flag: flag.0,
                data: data.as_mut_ptr(),
                size: size,
            };
            xwds_i2cm_xfer(self.i2cm.get(), &mut msg as *mut Msg, to)
        }
    }

    /// 中止I2C总线传输
    ///
    /// # 参数说明
    ///
    /// + addr: I2C地址
    /// + addrflag: I2C地址模式，取值 [`MsgFlag`] 的常量组合
    /// + to: 期望唤醒的时间点
    ///
    /// # 错误码
    ///
    /// + [`-ESHUTDOWN`] I2C主机模式控制器没有运行
    /// + [`-EADDRNOTAVAIL`] I2C地址没有应答
    /// + [`-ETIMEDOUT`] 传输超时
    ///
    /// [`-ESHUTDOWN`]: crate::errno::ESHUTDOWN
    /// [`-EADDRNOTAVAIL`]: crate::errno::EADDRNOTAVAIL
    /// [`-ETIMEDOUT`]: crate::errno::ETIMEDOUT
    pub fn abort(&self,
                 addr: u16, addrflag: MsgFlag,
                 to: XwTm) -> XwEr {
        unsafe {
            xwds_i2cm_abort(self.i2cm.get(), addr, addrflag.0, to)
        }
    }
}

/// I2C消息
#[repr(C)]
struct Msg {
    addr: u16,
    flag: u16,
    data: *mut u8,
    size: XwSz,
}

/// 外部中断标志
pub struct MsgFlag(u16);

impl MsgFlag {
    /// 7位地址
    pub const ADDR7BIT: MsgFlag = MsgFlag(0);
    /// 10位地址
    pub const ADDR10BIT: MsgFlag = MsgFlag(bit!(0));
    /// 地址掩码
    pub const ADDRMSG: MsgFlag = MsgFlag(bit!(0));

    /// 传输方向：写
    pub const WR: MsgFlag = MsgFlag(0);
    /// 传输方向：读
    pub const RD: MsgFlag = MsgFlag(bit!(1));
    /// 传输方向掩码
    pub const DIRMSG: MsgFlag = MsgFlag(bit!(1));

    /// 产生起始条件
    pub const START: MsgFlag = MsgFlag(bit!(2));

    /// 产生停止条件
    pub const STOP: MsgFlag = MsgFlag(bit!(3));

}

impl From<u16> for MsgFlag {
    fn from(flag: u16) -> Self {
        Self(flag)
    }
}

impl BitAnd for MsgFlag {
    type Output = Self;

    fn bitand(self, other: Self) -> Self {
        Self(self.0 & other.0)
    }
}

impl BitAndAssign for MsgFlag {
    fn bitand_assign(&mut self, other: Self) {
        self.0 &= other.0;
    }
}

impl BitOr for MsgFlag {
    type Output = Self;

    fn bitor(self, other: Self) -> Self {
        Self(self.0 | other.0)
    }
}

impl BitOrAssign for MsgFlag {
    fn bitor_assign(&mut self, other: Self) {
        self.0 |= other.0;
    }
}

impl BitXor for MsgFlag {
    type Output = Self;

    fn bitxor(self, other: Self) -> Self {
        Self(self.0 ^ other.0)
    }
}

impl BitXorAssign for MsgFlag {
    fn bitxor_assign(&mut self, other: Self) {
        self.0 ^= other.0;
    }
}
