//! XWOS RUST：SPI主机模式控制器
//! ========
//!

extern crate core;
use core::cell::UnsafeCell;

use crate::types::*;

extern "C" {
    fn xwds_spim_buscfg(spim: *mut XwdsSpim, cfgid: XwId, to: XwTm) -> XwEr;
    fn xwds_spim_xfer(spim: *mut XwdsSpim,
                      txd: *const u8, rxd: *mut u8, size: *mut XwSz,
                      to: XwTm) -> XwEr;
    fn xwds_spim_abort(spim: *mut XwdsSpim, to: XwTm) -> XwEr;
}

/// XWOS自旋锁占用的内存大小
#[cfg(target_pointer_width = "32")]
pub const SIZEOF_XWDS_SPIM: usize = 144;

/// XWOS自旋锁占用的内存大小
#[cfg(target_pointer_width = "64")]
pub const SIZEOF_XWDS_SPIM: usize = 288;

/// SPI主机模式控制器
#[repr(C)]
#[cfg_attr(target_pointer_width = "32", repr(align(8)))]
#[cfg_attr(target_pointer_width = "64", repr(align(16)))]
pub struct XwdsSpim {
    pub(crate) obj: [u8; SIZEOF_XWDS_SPIM],
}

/// SPI主机模式控制器
#[repr(C)]
pub struct Spim {
    pub(crate) spim: UnsafeCell<XwdsSpim>,
}

impl Spim {
    /// 配置总线
    ///
    /// # 参数说明
    ///
    /// + cfgid: 总线配置ID
    /// + to: 期望唤醒的时间点
    ///
    /// # 错误码
    ///
    /// + [`-ENOSYS`] 不支持配置总线操作
    /// + [`-ECHRNG`] 配置ID不在配置表范围内
    /// + [`-ETIMEDOUT`] 超时
    ///
    /// [`-ENOSYS`]: crate::errno::ENOSYS
    /// [`-ECHRNG`]: crate::errno::ECHRNG
    /// [`-ETIMEDOUT`]: crate::errno::ETIMEDOUT
    pub fn buscfg(&self, cfgid: XwId, to: XwTm) -> XwEr {
        unsafe {
            xwds_spim_buscfg(self.spim.get(), cfgid, to)
        }
    }

    /// 传输SPI消息
    ///
    /// # 参数说明
    ///
    /// + txd: 发送数据的缓冲区
    /// + rxd: 接收数据的缓冲区
    /// + size: 传输的大小
    /// + to: 期望唤醒的时间点
    ///
    /// # 错误码
    ///
    /// + [`-EBUSY`] 总线繁忙
    /// + [`-EIO`] 传输错误
    /// + [`-ETIMEDOUT`] 传输超时
    ///
    /// [`-EBUSY`]: crate::errno::EBUSY
    /// [`-EIO`]: crate::errno::EIO
    /// [`-ETIMEDOUT`]: crate::errno::ETIMEDOUT
    pub fn xfer(&self,
                txd: &[u8], rxd: &mut [u8], size: &mut XwSz,
                to: XwTm) -> XwEr {
        unsafe {
            xwds_spim_xfer(self.spim.get(),
                           txd.as_ptr(), rxd.as_mut_ptr(), size as *mut XwSz,
                           to)
        }
    }

    /// 中止SPI总线传输
    ///
    /// # 参数说明
    ///
    /// + to: 期望唤醒的时间点
    ///
    /// # 错误码
    ///
    /// + [`-ETIMEDOUT`] 传输超时
    ///
    /// [`-ETIMEDOUT`]: crate::errno::ETIMEDOUT
    pub fn abort(&self,
                 to: XwTm) -> XwEr {
        unsafe {
            xwds_spim_abort(self.spim.get(), to)
        }
    }
}
