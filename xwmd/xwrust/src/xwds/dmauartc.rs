//! XWOS RUST：DMA模式的串口控制器
//! ========
//!

extern crate core;
use core::cell::UnsafeCell;

use crate::types::*;

extern "C" {
    fn xwds_dmauartc_tx(dmauartc: *mut XwdsDmaUartC,
                        txd: *const u8, size: *mut XwSz,
                        to: XwTm) -> XwEr;
    fn xwds_dmauartc_putc(dmauartc: *mut XwdsDmaUartC, byte: u8, to: XwTm) -> XwEr;
    fn xwds_dmauartc_rx(dmauartc: *mut XwdsDmaUartC,
                        rxd: *mut u8, size: *mut XwSz,
                        to: XwTm) -> XwEr;
    fn xwds_dmauartc_try_rx(dmauartc: *mut XwdsDmaUartC,
                            rxd: *mut u8, size: *mut XwSz) -> XwEr;
}

/// XWOS自旋锁占用的内存大小
#[cfg(target_pointer_width = "32")]
pub const SIZEOF_XWDS_DMAUARTC: usize = 768;

/// XWOS自旋锁占用的内存大小
#[cfg(target_pointer_width = "64")]
pub const SIZEOF_XWDS_DMAUARTC: usize = 1024;

/// SPI主机模式控制器
#[repr(C)]
#[cfg_attr(target_pointer_width = "32", repr(align(8)))]
#[cfg_attr(target_pointer_width = "64", repr(align(16)))]
pub struct XwdsDmaUartC {
    pub(crate) obj: [u8; SIZEOF_XWDS_DMAUARTC],
}

/// SPI主机模式控制器
#[repr(C)]
pub struct DmaUartC {
    pub(crate) dmauartc: UnsafeCell<XwdsDmaUartC>,
}

impl DmaUartC {
    /// 配置DMA通道发送数据
    ///
    /// # 参数说明
    ///
    /// + txd: 待发送的数据的缓冲区
    /// + size: 传输的大小
    ///   + (I) 作为输入时，表示期望发送的数据的大小（单位：字节）
    ///   + (O) 作为输出时，返回实际发送的数据大小
    /// + to: 期望唤醒的时间点
    ///
    /// # 错误码
    ///
    /// + [`-ENOSYS`] 不支持此操作
    /// + [`-ECANCELED`] 发送被取消
    /// + [`-ETIMEDOUT`] 超时
    ///
    /// [`-ENOSYS`]: crate::errno::ENOSYS
    /// [`-ECANCELED`]: crate::errno::ECANCELED
    /// [`-ETIMEDOUT`]: crate::errno::ETIMEDOUT
    pub fn tx(&self, txd: &[u8], size: &mut XwSz, to: XwTm) -> XwEr {
        unsafe {
            xwds_dmauartc_tx(self.dmauartc.get(),
                             txd.as_ptr(), size as *mut XwSz,
                             to)
        }
    }

    /// 发送一字节数据数据（非DMA模式）
    ///
    /// # 参数说明
    ///
    /// + byte: 待发送的字节
    /// + to: 期望唤醒的时间点
    ///
    /// # 错误码
    ///
    /// + [`-ENOSYS`] 不支持此操作
    /// + [`-ETIMEDOUT`] 超时
    ///
    /// [`-ENOSYS`]: crate::errno::ENOSYS
    /// [`-ETIMEDOUT`]: crate::errno::ETIMEDOUT
    pub fn putc(&self, byte: u8, to: XwTm) -> XwEr {
        unsafe {
            xwds_dmauartc_putc(self.dmauartc.get(), byte, to)
        }
    }

    /// 从接收队列中获取数据
    ///
    /// # 参数说明
    ///
    /// + rxd: 接收数据的缓冲区
    /// + size: 传输的大小
    ///   + (I) 作为输入时，表示缓冲区大小（单位：字节）
    ///   + (O) 作为输出时，返回实际读取的数据大小
    /// + to: 期望唤醒的时间点
    ///
    /// # 错误码
    ///
    /// + [`-ETIMEDOUT`] 传输超时
    ///
    /// [`-ETIMEDOUT`]: crate::errno::ETIMEDOUT
    pub fn rx(&self, rxd: &mut [u8], size: &mut XwSz, to: XwTm) -> XwEr {
        unsafe {
            xwds_dmauartc_rx(self.dmauartc.get(),
                             rxd.as_mut_ptr(), size as *mut XwSz,
                             to)
        }
    }

    /// 尝试从接收队列中获取数据
    ///
    /// # 参数说明
    ///
    /// + rxd: 接收数据的缓冲区
    /// + size: 传输的大小
    ///   + (I) 作为输入时，表示缓冲区大小（单位：字节）
    ///   + (O) 作为输出时，返回实际读取的数据大小
    ///
    /// # 错误码
    ///
    /// + [`-ENODATA`] 没有数据
    ///
    /// [`-ENODATA`]: crate::errno::ENODATA
    pub fn try_rx(&self, rxd: &mut [u8], size: &mut XwSz) -> XwEr {
        unsafe {
            xwds_dmauartc_try_rx(self.dmauartc.get(),
                                 rxd.as_mut_ptr(), size as *mut XwSz)
        }
    }
}
