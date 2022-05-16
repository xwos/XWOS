//! XWOS RUST：线程栅栏
//! ========
//!
//! 线程栅栏是用于协调多个线程并行工作的同步机制。
//!
//! 线程栅栏在创建时，会指明有多少个线程槽。
//! 当线程到达线程栅栏时会阻塞并等待，直到指定数量的线程都达到线程栅栏，所有线程被同时唤醒。
//!
//!
//! # 创建
//!
//! XWOS RUST的线程栅栏可使用 [`Br::new()`] 创建。
//!
//! + 可以创建具有静态生命周期 [`'static`] 约束的全局变量：
//!
//! ```rust
//! use xwrust::xwos::sync::br::*;
//!
//! static GLOBAL_BR: Br<8> = Br::new();
//! ```
//!
//! + 也可以使用 [`alloc::sync::Arc`] 在heap中创建：
//!
//! ```rust
//! extern crate alloc;
//! use alloc::sync::Arc;
//!
//! use xwrust::xwos::sync::br::*;
//!
//! pub fn xwrust_example_br() {
//!     let br = Arc::new(Br::<8>::new());
//! }
//! ```
//!
//!
//! # 初始化
//!
//! 无论以何种方式创建的线程栅栏，都必须在使用前调用 [`Br::init()`] 进行初始化：
//!
//! ```rust
//! pub fn xwrust_example_br() {
//!     GLOBAL_BR.init();
//!     br.init();
//! }
//! ```
//!
//!
//! ## 等待所有线程到达栅栏
//!
//! [`Br::wait()`] 可用于等待所有线程到达栅栏。
//!
//! + 当线程栅栏中的线程数量小于指定数量，线程会阻塞等待。
//! + 当线程栅栏中的线程数量达到指定数量，全部线程被唤醒，然后返回 [`BrError::Ok`] 。
//! + 当线程阻塞等待被中断时，返回 [`BrError::Interrupt`] 。
//!
//! ## 限时等待所有线程到达栅栏
//!
//! [`Br::wait_to()`] 可用于限时等待所有线程到达栅栏。
//!
//! + 当线程栅栏中的线程数量小于指定数量，线程会阻塞等待，等待时会指定一个唤醒时间点。
//! + 当线程栅栏中的线程数量达到指定数量，全部线程被唤醒，然后返回 [`BrError::Ok`] 。
//! + 当线程阻塞等待被中断时，返回 [`BrError::Interrupt`] 。
//! + 当到达指定的唤醒时间点，线程被唤醒，并返回 [`BrError::Timedout`] 。
//!
//!
//! # 获取线程栅栏中线程槽的数量
//!
//! 可以通过方法 [`Br::get_num()`] 获取线程栅栏中线程槽的数量。
//!
//!
//! # 示例
//!
//! [XWOS/xwam/xwrust-example/xwrust_example_br](https://gitee.com/xwos/XWOS/blob/main/xwam/xwrust-example/xwrust_example_br/src/lib.rs)
//!
//!
//! [`'static`]: <https://doc.rust-lang.org/std/keyword.static.html>
//! [`alloc::sync::Arc`]: <https://doc.rust-lang.org/alloc/sync/struct.Arc.html>

extern crate core;
use core::ffi::*;
use core::cell::UnsafeCell;

use crate::types::*;
use crate::errno::*;


extern "C" {
    pub(crate) fn xwrustffi_br_init(br: *mut c_void, num: XwSz, bmp: *mut XwBmp, msk: *mut XwBmp) -> XwEr;
    pub(crate) fn xwrustffi_br_fini(br: *mut c_void) -> XwEr;
    pub(crate) fn xwrustffi_br_grab(br: *mut c_void) -> XwEr;
    pub(crate) fn xwrustffi_br_put(br: *mut c_void) -> XwEr;
    pub(crate) fn xwrustffi_br_gettik(br: *mut c_void) -> XwSq;
    pub(crate) fn xwrustffi_br_acquire(br: *mut c_void, tik: XwSq) -> XwEr;
    pub(crate) fn xwrustffi_br_release(br: *mut c_void, tik: XwSq) -> XwEr;
    pub(crate) fn xwrustffi_br_wait(br: *mut c_void) -> XwEr;
    pub(crate) fn xwrustffi_br_wait_to(br: *mut c_void, to: XwTm) -> XwEr;
}

/// 条件量的错误码
#[derive(Debug)]
pub enum BrError {
    /// 没有错误
    Ok,
    /// 条件量没有初始化
    NotInit,
    /// 线程数量超出范围
    OutOfRange,
    /// 等待被中断
    Interrupt,
    /// 等待超时
    Timedout,
    /// 不在线程上下文内
    NotThreadContext,
    /// 抢占被关闭
    CannotPmpt,
    /// 中断底半部被关闭
    CannotBh,
    /// 未知错误
    Unknown(XwEr),
}

/// XWOS条件量对象占用的内存大小
pub const SIZEOF_XWOS_BR: usize = 64;

/// 用于构建条件量的内存数组类型
#[repr(C)]
#[cfg_attr(target_pointer_width = "32", repr(align(8)))]
#[cfg_attr(target_pointer_width = "64", repr(align(16)))]
pub struct XwosBr<const N: XwSz>
where
    [XwBmp; ((N + XwBmp::BITS as usize - 1) / XwBmp::BITS as usize)]: Sized
{
    #[doc(hidden)]
    obj: [u8; SIZEOF_XWOS_BR],
    #[doc(hidden)]
    bmp: [XwBmp; ((N + XwBmp::BITS as usize - 1) / XwBmp::BITS as usize)],
    msk: [XwBmp; ((N + XwBmp::BITS as usize - 1) / XwBmp::BITS as usize)],
}

/// 条件量对象结构体
pub struct Br<const N: XwSz>
where
    [XwBmp; ((N + XwBmp::BITS as usize - 1) / XwBmp::BITS as usize)]: Sized
{
    /// 用于初始化XWOS条件量对象的内存空间
    pub(crate) br: UnsafeCell<XwosBr<N>>,
    /// 条件量对象的标签
    pub(crate) tik: UnsafeCell<XwSq>,
}

unsafe impl<const N: XwSz> Send for Br<N>
where
    [XwBmp; ((N + XwBmp::BITS as usize - 1) / XwBmp::BITS as usize)]: Sized
{}

unsafe impl<const N: XwSz> Sync for Br<N>
where
    [XwBmp; ((N + XwBmp::BITS as usize - 1) / XwBmp::BITS as usize)]: Sized
{}

impl<const N: XwSz> Br<N>
where
    [XwBmp; ((N + XwBmp::BITS as usize - 1) / XwBmp::BITS as usize)]: Sized
{
    /// 新建线程栅栏对象。
    ///
    /// 此方法是编译期方法。
    ///
    /// # 示例
    ///
    /// + 具有 [`'static`] 约束的全局变量全局变量：
    ///
    /// ```rust
    /// use xwrust::xwos::sync::br::*;
    ///
    /// static GLOBAL_BR: Br<8> = Br::new();
    /// ```
    ///
    /// + 在heap中创建：
    ///
    /// ```rust
    /// use alloc::sync::Arc;
    ///
    /// pub fn xwrust_example_br() {
    ///     let br = Arc::new(Br::<8>::new());
    /// }
    /// ```
    ///
    /// [`'static`]: https://doc.rust-lang.org/std/keyword.static.html
    pub const fn new() -> Self {
        Self {
            br: UnsafeCell::new(XwosBr {
                obj: [0; SIZEOF_XWOS_BR],
                bmp: [0; ((N + XwBmp::BITS as usize - 1) / XwBmp::BITS as usize)],
                msk: [0; ((N + XwBmp::BITS as usize - 1) / XwBmp::BITS as usize)],
            }),
            tik: UnsafeCell::new(0),
        }
    }

    /// 初始化线程栅栏对象。
    ///
    /// 线程栅栏对象必须调用此方法一次，方可正常使用。
    ///
    /// # 上下文
    ///
    /// + 任意
    ///
    /// # 示例
    ///
    /// ```rust
    /// use xwrust::xwos::sync::br::*;
    ///
    /// static GLOBAL_BR: Br<8> = Br::new();
    ///
    /// pub fn xwrust_example_br() {
    ///     // ...省略...
    ///     GLOBAL_BR.init();
    ///     // 从此处开始 GLOBAL_BR 可正常使用
    /// }
    /// ```
    pub fn init(&self) {
        unsafe {
            let rc = xwrustffi_br_acquire(self.br.get() as _, *self.tik.get());
            if rc == 0 {
                xwrustffi_br_put(self.br.get() as _);
            } else {
                xwrustffi_br_init(self.br.get() as _,
                                  N,
                                  &mut (*self.br.get()).bmp as _,
                                  &mut (*self.br.get()).msk as _);
                *self.tik.get() = xwrustffi_br_gettik(self.br.get() as _);
            }
        }
    }

    /// 获取线程栅栏中线程槽的数量。
    pub const fn get_num(&self) -> XwSz {
        N
    }

    /// 等待所有线程到达栅栏。
    ///
    /// + 当线程栅栏中的线程数量小于指定数量，线程会阻塞等待。
    /// + 当线程栅栏中的线程数量达到指定数量，全部线程被唤醒，然后返回 [`BrError::Ok`] 。
    /// + 当线程阻塞等待被中断时，返回 [`BrError::Interrupt`] 。
    ///
    /// # 上下文
    ///
    /// + 线程
    ///
    /// # 错误码
    ///
    /// + [`BrError::Ok`] 没有错误
    /// + [`BrError::NotInit`] 线程栅栏没有初始化
    /// + [`BrError::OutOfRange`] 线程数量超出范围
    /// + [`BrError::Interrupt`] 等待被中断
    /// + [`BrError::NotThreadContext`] 不在线程上下文内
    /// + [`BrError::CannotPmpt`] 抢占被关闭
    /// + [`BrError::CannotBh`] 中断底半部被关闭
    ///
    /// # 示例
    ///
    /// ```rust
    /// extern crate alloc;
    /// use alloc::sync::Arc;
    ///
    /// use xwrust::xwos::sync::br::*;
    ///
    /// pub fn xwrust_example_br() {
    ///     let br = Arc::new(Br::<8>::new());
    ///     br.init();
    ///     for idx in 0..8 {
    ///         let c = br.clone();
    ///         let _ = thd::spawn(move |_| { // 子线程闭包
    ///             c.wait();
    ///         });
    ///     }
    /// }
    /// ```
    pub fn wait(&self) -> BrError {
        unsafe {
            let mut rc = xwrustffi_br_acquire(self.br.get() as _, *self.tik.get());
            if rc == 0 {
                rc = xwrustffi_br_wait(self.br.get() as _);
                xwrustffi_br_put(self.br.get() as _);
                if XWOK == rc {
                    BrError::Ok
                } else if -ECHRNG == rc {
                    BrError::OutOfRange
                } else if -EINTR == rc {
                    BrError::Interrupt
                } else if -ENOTTHDCTX == rc {
                    BrError::NotThreadContext
                } else if -ECANNOTPMPT == rc {
                    BrError::CannotPmpt
                } else if -ECANNOTBH == rc {
                    BrError::CannotBh
                } else {
                    BrError::Unknown(rc)
                }
            } else {
                BrError::NotInit
            }
        }
    }

    /// 限时等待所有线程到达栅栏。
    ///
    /// + 当线程栅栏中的线程数量小于指定数量，线程会阻塞等待，等待时会指定一个唤醒时间点。
    /// + 当线程栅栏中的线程数量达到指定数量，全部线程被唤醒，然后返回 [`BrError::Ok`] 。
    /// + 当线程阻塞等待被中断时，返回 [`BrError::Interrupt`] 。
    /// + 当到达指定的唤醒时间点，线程被唤醒，并返回 [`BrError::Timedout`] 。
    ///
    /// # 上下文
    ///
    /// + 线程
    ///
    /// # 错误码
    ///
    /// + [`BrError::Ok`] 没有错误
    /// + [`BrError::NotInit`] 线程栅栏没有初始化
    /// + [`BrError::OutOfRange`] 线程数量超出范围
    /// + [`BrError::Interrupt`] 等待被中断
    /// + [`BrError::Timedout`] 等待超时
    /// + [`BrError::NotThreadContext`] 不在线程上下文内
    /// + [`BrError::CannotPmpt`] 抢占被关闭
    /// + [`BrError::CannotBh`] 中断底半部被关闭
    ///
    /// # 示例
    ///
    /// ```rust
    /// extern crate alloc;
    /// use alloc::sync::Arc;
    ///
    /// use xwrust::xwos::sync::br::*;
    ///
    /// pub fn xwrust_example_br() {
    ///     let br = Arc::new(Br::<8>::new());
    ///     br.init();
    ///     for idx in 0..8 {
    ///         let c = br.clone();
    ///         let _ = thd::spawn(move |_| { // 子线程闭包
    ///             c.wait_to(xwtm::ft(xwtm::s(3)));
    ///         });
    ///     }
    /// }
    /// ```
    pub fn wait_to(&self, to: XwTm) -> BrError {
        unsafe {
            let mut rc = xwrustffi_br_acquire(self.br.get() as _, *self.tik.get());
            if rc == 0 {
                rc = xwrustffi_br_wait_to(self.br.get() as _, to);
                xwrustffi_br_put(self.br.get() as _);
                if XWOK == rc {
                    BrError::Ok
                } else if -ECHRNG == rc {
                    BrError::OutOfRange
                } else if -EINTR == rc {
                    BrError::Interrupt
                } else if -ETIMEDOUT == rc {
                    BrError::Timedout
                } else if -ENOTTHDCTX == rc {
                    BrError::NotThreadContext
                } else if -ECANNOTPMPT == rc {
                    BrError::CannotPmpt
                } else if -ECANNOTBH == rc {
                    BrError::CannotBh
                } else {
                    BrError::Unknown(rc)
                }
            } else {
                BrError::NotInit
            }
        }
    }
}
