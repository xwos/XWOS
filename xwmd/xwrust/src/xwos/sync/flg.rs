//! XWOS RUST：事件标志
//! ========
//!
//! 事件标志使用位图来管理一组事件，位图中的每个位代表一个事件，
//! 当一个或多个事件状态发生变化时，事件对应的位也会发生变化，并唤醒正在等待的线程。
//! 线程唤醒后，就可从事件位图中获取事件的状态。
//!
//! + 线程可以等待位图中的事件位被置 **1** ，也可以等待事件位被清 **0** 。
//! + 线程可以等待位图中的事件位同时被置 **1** （事件与事件之间是逻辑 **与** 的关系），也可以等待其中任意一个位被置 **1** （事件与事件之间是逻辑 **或** 的关系）。
//! + 线程可以等待位图中的事件位同时被清 **0** （事件与事件之间是逻辑 **与** 的关系），也可以等待其中任意一个位被清 **0** （事件与事件之间是逻辑 **或** 的关系）。
//! + 线程可以选择是否 **消费** 事件。 **消费** 事件是指，当事件到来，线程被唤醒时，可以选择是否 **清除** 事件。
//! + 线程可以等待事件标志位发生 **翻转** ， **翻转** 是指事件标志位由 **1** 变为 **0** ，或由 **0** 变为 **1** 。
//!
//!
//! # 创建
//!
//! XWOS RUST的事件标志可使用 [`Flg::new()`] 创建。
//!
//! + 可以创建具有静态生命周期 [`static`] 约束的全局变量：
//!
//! ```rust
//! use xwrust::xwos::sync::flg::*;
//!
//! static GLOBAL_FLG: Flg<8> = Flg::new();
//! ```
//!
//! + 也可以使用 [`alloc::sync::Arc`] 在heap中创建：
//!
//! ```rust
//! extern crate alloc;
//! use alloc::sync::Arc;
//!
//! use xwrust::xwos::sync::flg::*;
//!
//! pub fn xwrust_example_flg() {
//!     let flg = Arc::new(Flg::<8>::new());
//! }
//! ```
//!
//!
//! # 初始化
//!
//! 无论以何种方式创建的事件标志，都必须在使用前调用 [`Flg::init()`] 进行初始化：
//!
//! ```rust
//! pub fn xwrust_example_flg() {
//!     GLOBAL_FLG.init();
//!     flg.init();
//! }
//! ```
//!
//!
//! # 产生事件
//!
//! + [`Flg::s1m()`] 同时设置多个事件标志位
//! + [`Flg::s1i()`] 设置单个事件标志位
//! + [`Flg::c0m()`] 同时清除多个事件标志位
//! + [`Flg::c0i()`] 清除单个事件标志位
//! + [`Flg::x1m()`] 同时翻转多个事件标志位
//! + [`Flg::x1i()`] 翻转单个事件标志位
//!
//! 产生事件的方法，除了会修改事件标志位图的状态，还会通过 **广播** 唤醒所有正在等待的线程。
//! 然后，线程通过比对位图状态，确定事件是否已经满足触发条件。
//! 若满足触发条件，就退出等待；若未满足触发条件，重新进入阻塞等待状态。
//!
//!
//! # 等待事件的方法
//!
//! + [`Flg::wait()`] 可用于等待事件。
//! + [`Flg::wait_to()`] 可用于限时等待事件。
//! + [`Flg::trywait()`] 可用于检测事件。
//!
//! ## 事件的触发条件
//!
//! 等待事件时，可通过参数 `tg` 指明事件的触发条件。事件的触发条件分为两类。
//!
//! ### 电平触发
//!
//! **电平触发** 源于数字电路，是一种类比概念，是指事件位图的特定状态（ **1** 或 **0** ）所产生的触发事件，包括：
//!
//! + [`Trigger::SetAll`] 所有事件位被置 **1** 触发
//! + [`Trigger::SetAny`] 任意事件位被置 **1** 触发
//! + [`Trigger::ClearAll`] 所有事件位被清 **0** 触发
//! + [`Trigger::ClearAny`] 任意事件位被清 **0** 触发
//!
//! **电平触发** 时，可在线程读取事件位图后，**清除** 事件触发条件。
//!
//! ### 边沿触发
//!
//! **边沿触发** 源于数字电路，是一种类比概念，是指事件状态发生改变（ **1** 变 **0** 或 **0** 变 **1** ）时产生的唤醒信号，包括：
//!
//! + [`Trigger::SetAll`] 所有事件位发生翻转触发
//! + [`Trigger::SetAny`] 任意事件位发生翻转触发
//!
//! **边沿触发** 时，事件的触发条件不需要清除。
//! **边沿触发** 时，必须要有一个初始状态，就像数字电路一样：
//!
//! + 当位的初始值为 **0** (低电平)，然后跳变到 **1** (高电平)的瞬间被称为上升沿。此时触发的事件被称为上升沿触发。
//! + 当位的初始值为 **1** (高电平)，然后跳变到 **0** (低电平)的瞬间被称为下降沿。此时触发的事件被称为下降沿触发。
//!
//! ## 清除事件
//!
//! + 当采用 **电平触发** 时，需要在读取事件位图后 **清除** 事件标志位，否则事件会一直处于触发状态。
//!   可以在调用等待事件的方法时，指定参数 `consumption` 为 `true` 。
//!   **清除** 的含义是：
//!   + 当线程等待的是位图中的事件位被置 **1** ， **清除** 是指将这些位清 **0** ；
//!   + 当线程等待的是位图中的事件位被清 **0** ， **清除** 是指将这些位置 **1** ；
//! + 当采用 **边沿触发** 时，不需要 **清除** 事件标志位。
//!
//!
//! # 读取事件
//!
//! 用户可以通过方法 [`Flg::read()`] 直接读取事件的位图状态。此函数立即返回，不会阻塞。
//!
//! # 获取事件标志中事件槽的数量
//!
//! 可以通过方法 [`Flg::get_num()`] 获取事件标志中事件槽的数量。
//!
//!
//! # 绑定到信号选择器
//!
//! 事件标志是 **同步对象** ，可以通过方法 [`Flg::bind()`] 将事件标志绑定到信号选择器 [`Sel<M>`] 上，通过 [`Sel<M>`] ，单一线程可以同时等待多个不同的 **同步对象** 。
//!
//! 事件标志采用 **非独占** 的方式进行绑定。
//!
//!
//! # 示例
//!
//! [XWOS/xwam/xwrust-example/xwrust_example_flg](https://gitee.com/xwos/XWOS/blob/main/xwam/xwrust-example/xwrust_example_flg/src/lib.rs)
//!
//!
//! [`static`]: <https://doc.rust-lang.org/std/keyword.static.html>
//! [`alloc::sync::Arc`]: <https://doc.rust-lang.org/alloc/sync/struct.Arc.html>
//! [`Sel<M>`]: super::sel::Sel

extern crate core;
use core::ffi::*;
use core::cell::UnsafeCell;

use crate::types::*;
use crate::errno::*;
use crate::xwbmp::*;
use crate::xwos::sync::sel::*;


extern "C" {
    pub(crate) fn xwrustffi_flg_init(flg: *mut c_void, num: XwSz, bmp: *mut XwBmp, msk: *mut XwBmp) -> XwEr;
    pub(crate) fn xwrustffi_flg_fini(flg: *mut c_void) -> XwEr;
    pub(crate) fn xwrustffi_flg_grab(flg: *mut c_void) -> XwEr;
    pub(crate) fn xwrustffi_flg_put(flg: *mut c_void) -> XwEr;
    pub(crate) fn xwrustffi_flg_get_tik(flg: *mut c_void) -> XwSq;
    pub(crate) fn xwrustffi_flg_acquire(flg: *mut c_void, tik: XwSq) -> XwEr;
    pub(crate) fn xwrustffi_flg_release(flg: *mut c_void, tik: XwSq) -> XwEr;
    pub(crate) fn xwrustffi_flg_bind(flg: *mut c_void, sel: *mut c_void, pos: XwSq) -> XwEr;
    pub(crate) fn xwrustffi_flg_unbind(flg: *mut c_void, sel: *mut c_void) -> XwEr;
    pub(crate) fn xwrustffi_flg_read(flg: *mut c_void, out: *mut XwBmp) -> XwEr;
    pub(crate) fn xwrustffi_flg_s1m(flg: *mut c_void, msk: *mut XwBmp) -> XwEr;
    pub(crate) fn xwrustffi_flg_s1i(flg: *mut c_void, pos: XwSq) -> XwEr;
    pub(crate) fn xwrustffi_flg_c0m(flg: *mut c_void, msk: *mut XwBmp) -> XwEr;
    pub(crate) fn xwrustffi_flg_c0i(flg: *mut c_void, pos: XwSq) -> XwEr;
    pub(crate) fn xwrustffi_flg_x1m(flg: *mut c_void, msk: *mut XwBmp) -> XwEr;
    pub(crate) fn xwrustffi_flg_x1i(flg: *mut c_void, pos: XwSq) -> XwEr;
    pub(crate) fn xwrustffi_flg_wait(flg: *mut c_void, tg: XwSq, consumption: bool,
                                     origin: *mut XwBmp, msk: *mut XwBmp) -> XwEr;
    pub(crate) fn xwrustffi_flg_wait_to(flg: *mut c_void, tg: XwSq, consumption: bool,
                                        origin: *mut XwBmp, msk: *mut XwBmp, to: XwTm) -> XwEr;
    pub(crate) fn xwrustffi_flg_trywait(flg: *mut c_void, tg: XwSq, consumption: bool,
                                        origin: *mut XwBmp, msk: *mut XwBmp) -> XwEr;
    pub(crate) fn xwrustffi_flg_wait_unintr(flg: *mut c_void, tg: XwSq, consumption: bool,
                                            origin: *mut XwBmp, msk: *mut XwBmp) -> XwEr;
}

/// 事件标志的错误码
#[derive(Debug)]
pub enum FlgError {
    /// 没有错误
    Ok(XwEr),
    /// 事件标志没有初始化
    NotInit(XwEr),
    /// 位置超出范围
    OutOfRange(XwEr),
    /// 等待被中断
    Interrupt(XwEr),
    /// 等待超时
    Timedout(XwEr),
    /// 没有检测到事件
    NoEvent(XwEr),
    /// 不在线程上下文内
    NotThreadContext(XwEr),
    /// 信号选择器的位置超出范围
    OutOfSelPos(XwEr),
    /// 事件标志已经绑定
    AlreadyBound(XwEr),
    /// 信号选择器的位置被占用
    SelPosBusy(XwEr),
    /// 未知错误
    Unknown(XwEr),
}

impl FlgError {
    /// 消费掉 `FlgError` 自身，返回内部的错误码。
    pub fn unwrap(self) -> XwEr {
        match self {
            Self::Ok(rc) => rc,
            Self::NotInit(rc) => rc,
            Self::OutOfRange(rc) => rc,
            Self::Interrupt(rc) => rc,
            Self::Timedout(rc) => rc,
            Self::NoEvent(rc) => rc,
            Self::NotThreadContext(rc) => rc,
            Self::OutOfSelPos(rc) => rc,
            Self::AlreadyBound(rc) => rc,
            Self::SelPosBusy(rc) => rc,
            Self::Unknown(rc) => rc,
        }
    }

    /// 如果错误码是 [`FlgError::Ok`] ，返回 `true` 。
    pub const fn is_ok(&self) -> bool {
        matches!(*self, Self::Ok(_))
    }

    /// 如果错误码不是 [`FlgError::Ok`] ，返回 `true` 。
    pub const fn is_err(&self) -> bool {
        !self.is_ok()
    }
}

/// 触发条件
pub enum Trigger {
    /// 所有事件位被置 **1** 触发
    SetAll = 0,
    /// 任意事件位被置 **1** 触发
    SetAny = 1,
    /// 所有事件位被清 **0** 触发
    ClearAll = 2,
    /// 任意事件位被清 **0** 触发
    ClearAny = 3,
    /// 所有事件位发生翻转触发
    ToggleAll = 4,
    /// 任意事件位发生翻转触发
    ToggleAny = 5,
}

/// XWOS事件标志对象占用的内存大小
#[cfg(target_pointer_width = "32")]
pub const SIZEOF_XWOS_FLG: usize = 64;

/// XWOS事件标志对象占用的内存大小
#[cfg(target_pointer_width = "64")]
pub const SIZEOF_XWOS_FLG: usize = 128;

/// 用于构建事件标志的内存数组类型
#[repr(C)]
#[cfg_attr(target_pointer_width = "32", repr(align(8)))]
#[cfg_attr(target_pointer_width = "64", repr(align(16)))]
pub(crate) struct XwosFlg<const N: XwSz>
where
    [XwBmp; (N + XwBmp::BITS as usize - 1) / XwBmp::BITS as usize]: Sized
{
    pub(crate) obj: [u8; SIZEOF_XWOS_FLG],
    pub(crate) bmp: [XwBmp; (N + XwBmp::BITS as usize - 1) / XwBmp::BITS as usize],
    pub(crate) msk: [XwBmp; (N + XwBmp::BITS as usize - 1) / XwBmp::BITS as usize],
}

/// 事件标志对象结构体
pub struct Flg<const N: XwSz>
where
    [XwBmp; (N + XwBmp::BITS as usize - 1) / XwBmp::BITS as usize]: Sized
{
    /// 用于初始化XWOS事件标志对象的内存空间
    pub(crate) flg: UnsafeCell<XwosFlg<N>>,
    /// 事件标志对象的标签
    pub(crate) tik: UnsafeCell<XwSq>,
}

unsafe impl<const N: XwSz> Send for Flg<N>
where
    [XwBmp; (N + XwBmp::BITS as usize - 1) / XwBmp::BITS as usize]: Sized
{}

unsafe impl<const N: XwSz> Sync for Flg<N>
where
    [XwBmp; (N + XwBmp::BITS as usize - 1) / XwBmp::BITS as usize]: Sized
{}

impl<const N: XwSz> Drop for Flg<N>
where
    [XwBmp; (N + XwBmp::BITS as usize - 1) / XwBmp::BITS as usize]: Sized
{
    fn drop(&mut self) {
        unsafe {
            xwrustffi_flg_fini(self.flg.get() as _);
        }
    }
}

impl<const N: XwSz> Flg<N>
where
    [XwBmp; (N + XwBmp::BITS as usize - 1) / XwBmp::BITS as usize]: Sized
{
    /// 新建事件标志对象
    ///
    /// 此方法是编译期方法。
    ///
    /// # 示例
    ///
    /// + 具有 [`static`] 约束的全局变量全局变量：
    ///
    /// ```rust
    /// use xwrust::xwos::sync::flg::*;
    ///
    /// static GLOBAL_FLG: Flg<8> = Flg::new();
    /// ```
    ///
    /// + 在heap中创建：
    ///
    /// ```rust
    /// extern crate alloc;
    /// use alloc::sync::Arc;
    ///
    /// pub fn xwrust_example_flg() {
    ///     let flg = Arc::new(Flg::<8>::new());
    /// }
    /// ```
    ///
    /// [`static`]: https://doc.rust-lang.org/std/keyword.static.html
    pub const fn new() -> Self {
        Self {
            flg: UnsafeCell::new(XwosFlg {
                obj: [0; SIZEOF_XWOS_FLG],
                bmp: [0; (N + XwBmp::BITS as usize - 1) / XwBmp::BITS as usize],
                msk: [0; (N + XwBmp::BITS as usize - 1) / XwBmp::BITS as usize],
            }),
            tik: UnsafeCell::new(0),
        }
    }

    /// 初始化事件标志对象
    ///
    /// 事件标志对象必须调用此方法一次，方可正常使用。
    ///
    /// # 上下文
    ///
    /// + 任意
    ///
    /// # 示例
    ///
    /// ```rust
    /// use xwrust::xwos::sync::flg::*;
    ///
    /// static GLOBAL_FLG: Flg<8> = Flg::new();
    ///
    /// pub fn xwrust_example_flg() {
    ///     // ...省略...
    ///     GLOBAL_FLG.init();
    ///     // 从此处开始 GLOBAL_FLG 可正常使用
    /// }
    /// ```
    pub fn init(&self) {
        unsafe {
            let rc = xwrustffi_flg_acquire(self.flg.get() as _, *self.tik.get());
            if rc == 0 {
                xwrustffi_flg_put(self.flg.get() as _);
            } else {
                xwrustffi_flg_init(self.flg.get() as _,
                                  N,
                                  &mut (*self.flg.get()).bmp as _,
                                  &mut (*self.flg.get()).msk as _);
                *self.tik.get() = xwrustffi_flg_get_tik(self.flg.get() as _);
            }
        }
    }

    /// 获取事件标志中事件槽的数量
    pub const fn get_num(&self) -> XwSz {
        N
    }

    /// 读取事件标志位图的值
    ///
    /// 直接读取位图的值，不会等待，也不会改变位图的状态。
    pub fn read(&self) -> Result<Bmp<N>, FlgError> {
        unsafe {
            let mut rc = xwrustffi_flg_acquire(self.flg.get() as _, *self.tik.get());
            if rc == 0 {
                let out = Bmp::<N>::new();
                rc = xwrustffi_flg_read(self.flg.get() as _, out.bmp.get() as _,);
                xwrustffi_flg_put(self.flg.get() as _);
                if XWOK == rc {
                    Ok(out)
                } else {
                    Err(FlgError::Unknown(rc))
                }
            } else {
                Err(FlgError::NotInit(rc))
            }
        }
    }

    /// 同时设置多个事件标志位
    ///
    /// + 此RustAPI会将事件标志位图中，被掩码 `msk` 覆盖的位全部设置为 **1** ，
    ///   然后 **广播** 所有正在等待的线程。
    /// + 线程唤醒后通过比对位图状态，确定事件是否已经满足触发条件，
    ///   若满足触发条件，就退出等待；若未满足触发条件，重新进入阻塞等待状态。
    ///
    /// # 参数说明
    ///
    /// + msk: 事件的位图掩码
    pub fn s1m(&self, msk: &Bmp<N>) -> FlgError {
        unsafe {
            let mut rc = xwrustffi_flg_acquire(self.flg.get() as _, *self.tik.get());
            if rc == 0 {
                rc = xwrustffi_flg_s1m(self.flg.get() as _, msk.bmp.get() as _);
                xwrustffi_flg_put(self.flg.get() as _);
                if XWOK == rc {
                    FlgError::Ok(rc)
                } else {
                    FlgError::Unknown(rc)
                }
            } else {
                FlgError::NotInit(rc)
            }
        }
    }

    /// 设置单个事件标志位
    ///
    /// + 此CAPI会将事件标志位图中，序号为 `pos` 的单个位设置为 **1** ，
    ///   然后 **广播** 所有正在等待的线程。
    /// + 线程唤醒后通过比对位图状态，确定事件是否已经满足触发条件，
    ///   若满足触发条件，就退出等待；若未满足触发条件，重新进入阻塞等待状态。
    ///
    /// # 参数说明
    ///
    /// + pos: 事件的序号
    pub fn s1i(&self, pos: XwSq) -> FlgError {
        unsafe {
            let mut rc = xwrustffi_flg_acquire(self.flg.get() as _, *self.tik.get());
            if rc == 0 {
                rc = xwrustffi_flg_s1i(self.flg.get() as _, pos);
                xwrustffi_flg_put(self.flg.get() as _);
                if XWOK == rc {
                    FlgError::Ok(rc)
                } else {
                    FlgError::Unknown(rc)
                }
            } else {
                FlgError::NotInit(rc)
            }
        }
    }

    /// 同时清除多个事件标志位
    ///
    /// + 此CAPI会将事件标志位图中，被掩码 `msk` 覆盖的位全部清 **0** ，
    ///   然后 **广播** 所有正在等待的线程。
    /// + 线程唤醒后通过比对位图状态，确定事件是否已经满足触发条件，
    ///   若满足触发条件，就退出等待；若未满足触发条件，重新进入阻塞等待状态。
    ///
    /// # 参数说明
    ///
    /// + msk: 事件的位图掩码
    pub fn c0m(&self, msk: &Bmp<N>) -> FlgError {
        unsafe {
            let mut rc = xwrustffi_flg_acquire(self.flg.get() as _, *self.tik.get());
            if rc == 0 {
                rc = xwrustffi_flg_c0m(self.flg.get() as _, msk.bmp.get() as _);
                xwrustffi_flg_put(self.flg.get() as _);
                if XWOK == rc {
                    FlgError::Ok(rc)
                } else {
                    FlgError::Unknown(rc)
                }
            } else {
                FlgError::NotInit(rc)
            }
        }
    }

    /// 清除单个事件标志位
    ///
    /// + 此CAPI会将事件标志位图中，序号为 `pos` 的单个位清 **0** ，
    ///   然后 **广播** 所有正在等待的线程。
    /// + 线程唤醒后通过比对位图状态，确定事件是否已经满足触发条件，
    ///   若满足触发条件，就退出等待；若未满足触发条件，重新进入阻塞等待状态。
    ///
    /// # 参数说明
    ///
    /// + pos: 事件的序号
    pub fn c0i(&self, pos: XwSq) -> FlgError {
        unsafe {
            let mut rc = xwrustffi_flg_acquire(self.flg.get() as _, *self.tik.get());
            if rc == 0 {
                rc = xwrustffi_flg_c0i(self.flg.get() as _, pos);
                xwrustffi_flg_put(self.flg.get() as _);
                if XWOK == rc {
                    FlgError::Ok(rc)
                } else {
                    FlgError::Unknown(rc)
                }
            } else {
                FlgError::NotInit(rc)
            }
        }
    }

    /// 同时翻转多个事件标志位
    ///
    /// + 此CAPI会将事件标志位图中，被掩码 `msk` 覆盖的位全部翻转，
    ///   然后 **广播** 所有正在等待的线程。
    /// + 线程唤醒后通过比对位图状态，确定事件是否已经满足触发条件，
    ///   若满足触发条件，就退出等待；若未满足触发条件，重新进入阻塞等待状态。
    ///
    /// # 参数说明
    ///
    /// + msk: 事件的位图掩码
    pub fn x1m(&self, msk: &Bmp<N>) -> FlgError {
        unsafe {
            let mut rc = xwrustffi_flg_acquire(self.flg.get() as _, *self.tik.get());
            if rc == 0 {
                rc = xwrustffi_flg_x1m(self.flg.get() as _, msk.bmp.get() as _);
                xwrustffi_flg_put(self.flg.get() as _);
                if XWOK == rc {
                    FlgError::Ok(rc)
                } else {
                    FlgError::Unknown(rc)
                }
            } else {
                FlgError::NotInit(rc)
            }
        }
    }

    /// 翻转单个事件标志位
    ///
    /// + 此CAPI会将事件标志位图中，序号为 `pos` 的单个位翻转，
    ///   然后 **广播** 所有正在等待的线程。
    /// + 线程唤醒后通过比对位图状态，确定事件是否已经满足触发条件，
    ///   若满足触发条件，就退出等待；若未满足触发条件，重新进入阻塞等待状态。
    ///
    /// # 参数说明
    ///
    /// + pos: 事件的序号
    pub fn x1i(&self, pos: XwSq) -> FlgError {
        unsafe {
            let mut rc = xwrustffi_flg_acquire(self.flg.get() as _, *self.tik.get());
            if rc == 0 {
                rc = xwrustffi_flg_x1i(self.flg.get() as _, pos);
                xwrustffi_flg_put(self.flg.get() as _);
                if XWOK == rc {
                    FlgError::Ok(rc)
                } else {
                    FlgError::Unknown(rc)
                }
            } else {
                FlgError::NotInit(rc)
            }
        }
    }

    /// 等待事件
    ///
    /// + 当没有检测到事件时，线程会阻塞等待。
    /// + 当检测到事件时，线程被唤醒，然后返回 [`FlgError::Ok`] 。
    /// + 当线程阻塞等待被中断时，返回 [`FlgError::Interrupt`] 。
    ///
    /// # 事件的触发条件
    ///
    /// 事件的触发条件可通过参数 `tg` 设置。事件的触发条件分为两类。
    ///
    /// ## 电平触发
    ///
    /// **电平触发** 源于数字电路，是一种类比概念，是指事件位图的特定状态（ **1** 或 **0** ）所产生的触发事件，包括：
    ///
    /// + [`Trigger::SetAll`] 所有事件位被置 **1** 触发
    /// + [`Trigger::SetAny`] 任意事件位被置 **1** 触发
    /// + [`Trigger::ClearAll`] 所有事件位被清 **0** 触发
    /// + [`Trigger::ClearAny`] 任意事件位被清 **0** 触发
    ///
    /// **电平触发** 时，若参数 `consumption` 取值 `true` ，表示线程在读取事件位图后，会 **清除** 事件位图。
    /// **电平触发** 时，参数 `origin` 返回事件触发 **之前** 的位图状态。
    ///
    /// ## 边沿触发
    ///
    /// **边沿触发** 源于数字电路，是一种类比概念，是指事件状态发生改变（ **1** 变 **0** 或 **0** 变 **1** ）时产生的唤醒信号，包括：
    ///
    /// + [`Trigger::SetAll`] 所有事件位发生翻转触发
    /// + [`Trigger::SetAny`] 任意事件位发生翻转触发
    ///
    /// **边沿触发** 时，参数 `consumption` 没有意义。
    /// **边沿触发** 时，必须要有一个初始状态，就像数字电路一样：
    ///
    /// + 当位的初始值为 **0** (低电平)，然后跳变到 **1** (高电平)的瞬间被称为上升沿。此时触发的事件被称为上升沿触发。
    /// + 当位的初始值为 **1** (高电平)，然后跳变到 **0** (低电平)的瞬间被称为下降沿。此时触发的事件被称为下降沿触发。
    ///
    /// 初始值就由参数 `origin` 指明，触发后，事件位发生跳变，跳变后的结果还是由参数 `origin` 返回。
    ///
    /// # 参数说明
    ///
    /// + tg: 事件触发条件
    ///   + **电平触发**
    ///     + [`Trigger::SetAll`] 所有事件位被置 **1** 触发
    ///     + [`Trigger::SetAny`] 任意事件位被置 **1** 触发
    ///     + [`Trigger::ClearAll`] 所有事件位被清 **0** 触发
    ///     + [`Trigger::ClearAny`] 任意事件位被清 **0** 触发
    ///   + **边沿触发**
    ///     + [`Trigger::SetAll`] 所有事件位发生翻转触发
    ///     + [`Trigger::SetAny`] 任意事件位发生翻转触发
    /// + consumption: 事件触发后是否清除事件
    /// + origin: 事件位图
    ///   + **电平触发** 时，参数 `origin` 用于返回事件触发 **之前** 的位图状态。
    ///   + **边沿触发** 时
    ///     + 输入时 `origin` 指明事件标志位图的初始值，用于评估事件位图是否发生变化。
    ///     + 输出时 `origin` 返回触发后的事件标志位图状态（可作为下一次调用的初始值）。
    /// + msk: 事件的位图掩码，表示只关注掩码部分的事件
    ///
    /// # 上下文
    ///
    /// + 线程
    ///
    /// # 错误码
    ///
    /// + [`FlgError::Ok`] 没有错误
    /// + [`FlgError::NotInit`] 事件标志没有初始化
    /// + [`FlgError::Interrupt`] 等待被中断
    /// + [`FlgError::NotThreadContext`] 不在线程上下文内
    ///
    /// # 示例
    ///
    /// ```rust
    /// extern crate alloc;
    /// use alloc::sync::Arc;
    ///
    /// use xwrust::xwos::sync::flg::*;
    ///
    /// pub fn xwrust_example_flg() {
    /// }
    /// ```
    pub fn wait(&self, tg: Trigger, consumption: bool,
                origin: &mut Bmp<N>, msk: &Bmp<N>) -> FlgError {
        unsafe {
            let mut rc = xwrustffi_flg_acquire(self.flg.get() as _, *self.tik.get());
            if rc == 0 {
                rc = xwrustffi_flg_wait(self.flg.get() as _, tg as XwSq, consumption,
                                        origin.bmp.get() as _, msk.bmp.get() as _);
                xwrustffi_flg_put(self.flg.get() as _);
                if XWOK == rc {
                    FlgError::Ok(rc)
                } else if -EINTR == rc {
                    FlgError::Interrupt(rc)
                } else if -ENOTTHDCTX == rc {
                    FlgError::NotThreadContext(rc)
                } else {
                    FlgError::Unknown(rc)
                }
            } else {
                FlgError::NotInit(rc)
            }
        }
    }

    /// 限时等待事件
    ///
    /// + 当没有检测到事件时，线程会阻塞等待，等待时会指定一个唤醒时间点 `to` 。
    /// + 当检测到事件时，线程被唤醒，然后返回 [`FlgError::Ok`] 。
    /// + 当线程阻塞等待被中断时，返回 [`FlgError::Interrupt`] 。
    /// + 当到达指定的唤醒时间点时，线程被唤醒，并返回 [`FlgError::Timedout`] 。
    ///
    /// # 事件的触发条件
    ///
    /// 事件的触发条件可通过参数 `tg` 设置。事件的触发条件分为两类。
    ///
    /// ## 电平触发
    ///
    /// **电平触发** 源于数字电路，是一种类比概念，是指事件位图的特定状态（ **1** 或 **0** ）所产生的触发事件，包括：
    ///
    /// + [`Trigger::SetAll`] 所有事件位被置 **1** 触发
    /// + [`Trigger::SetAny`] 任意事件位被置 **1** 触发
    /// + [`Trigger::ClearAll`] 所有事件位被清 **0** 触发
    /// + [`Trigger::ClearAny`] 任意事件位被清 **0** 触发
    ///
    /// **电平触发** 时，若参数 `consumption` 取值 `true` ，表示线程在读取事件位图后，会 **清除** 事件位图。
    /// **电平触发** 时，参数 `origin` 返回事件触发 **之前** 的位图状态。
    ///
    /// ## 边沿触发
    ///
    /// **边沿触发** 源于数字电路，是一种类比概念，是指事件状态发生改变（ **1** 变 **0** 或 **0** 变 **1** ）时产生的唤醒信号，包括：
    ///
    /// + [`Trigger::SetAll`] 所有事件位发生翻转触发
    /// + [`Trigger::SetAny`] 任意事件位发生翻转触发
    ///
    /// **边沿触发** 时，参数 `consumption` 没有意义。
    /// **边沿触发** 时，必须要有一个初始状态，就像数字电路一样：
    ///
    /// + 当位的初始值为 **0** (低电平)，然后跳变到 **1** (高电平)的瞬间被称为上升沿。此时触发的事件被称为上升沿触发。
    /// + 当位的初始值为 **1** (高电平)，然后跳变到 **0** (低电平)的瞬间被称为下降沿。此时触发的事件被称为下降沿触发。
    ///
    /// 初始值就由参数 `origin` 指明，触发后，事件位发生跳变，跳变后的结果还是由参数 `origin` 返回。
    ///
    /// # 参数说明
    ///
    /// + tg: 事件触发条件
    ///   + **电平触发**
    ///     + [`Trigger::SetAll`] 所有事件位被置 **1** 触发
    ///     + [`Trigger::SetAny`] 任意事件位被置 **1** 触发
    ///     + [`Trigger::ClearAll`] 所有事件位被清 **0** 触发
    ///     + [`Trigger::ClearAny`] 任意事件位被清 **0** 触发
    ///   + **边沿触发**
    ///     + [`Trigger::SetAll`] 所有事件位发生翻转触发
    ///     + [`Trigger::SetAny`] 任意事件位发生翻转触发
    /// + consumption: 事件触发后是否清除事件
    /// + origin: 事件位图
    ///   + **电平触发** 时，参数 `origin` 用于返回事件触发 **之前** 的位图状态。
    ///   + **边沿触发** 时
    ///     + 输入时 `origin` 指明事件标志位图的初始值，用于评估事件位图是否发生变化。
    ///     + 输出时 `origin` 返回触发后的事件标志位图状态（可作为下一次调用的初始值）。
    /// + msk: 事件的位图掩码，表示只关注掩码部分的事件
    ///
    /// # 上下文
    ///
    /// + 线程
    ///
    /// # 错误码
    ///
    /// + [`FlgError::Ok`] 没有错误
    /// + [`FlgError::NotInit`] 事件标志没有初始化
    /// + [`FlgError::Interrupt`] 等待被中断
    /// + [`FlgError::Timedout`] 等待超时
    /// + [`FlgError::NotThreadContext`] 不在线程上下文内
    ///
    /// # 示例
    ///
    /// ```rust
    /// extern crate alloc;
    /// use alloc::sync::Arc;
    ///
    /// use xwrust::xwos::sync::flg::*;
    ///
    /// pub fn xwrust_example_flg() {
    /// }
    /// ```
    pub fn wait_to(&self, tg: Trigger, consumption: bool,
                   origin: &mut Bmp<N>, msk: &Bmp<N>, to: XwTm) -> FlgError {
        unsafe {
            let mut rc = xwrustffi_flg_acquire(self.flg.get() as _, *self.tik.get());
            if rc == 0 {
                rc = xwrustffi_flg_wait_to(self.flg.get() as _, tg as XwSq, consumption,
                                           origin.bmp.get() as _, msk.bmp.get() as _, to);
                xwrustffi_flg_put(self.flg.get() as _);
                if XWOK == rc {
                    FlgError::Ok(rc)
                } else if -EINTR == rc {
                    FlgError::Interrupt(rc)
                } else if -ETIMEDOUT == rc {
                    FlgError::Timedout(rc)
                } else if -ENOTTHDCTX == rc {
                    FlgError::NotThreadContext(rc)
                } else {
                    FlgError::Unknown(rc)
                }
            } else {
                FlgError::NotInit(rc)
            }
        }
    }

    /// 检查事件
    ///
    /// + 当检测到事件时，立即返回 [`FlgError::Ok`] 。
    /// + 当没有检测到事件时，立即返回 [`FlgError::NoEvent`] 。
    ///
    /// # 事件的触发条件
    ///
    /// 事件的触发条件可通过参数 `tg` 设置。事件的触发条件分为两类。
    ///
    /// ## 电平触发
    ///
    /// **电平触发** 源于数字电路，是一种类比概念，是指事件位图的特定状态（ **1** 或 **0** ）所产生的触发事件，包括：
    ///
    /// + [`Trigger::SetAll`] 所有事件位被置 **1** 触发
    /// + [`Trigger::SetAny`] 任意事件位被置 **1** 触发
    /// + [`Trigger::ClearAll`] 所有事件位被清 **0** 触发
    /// + [`Trigger::ClearAny`] 任意事件位被清 **0** 触发
    ///
    /// **电平触发** 时，若参数 `consumption` 取值 `true` ，表示线程在读取事件位图后，会 **清除** 事件位图。
    /// **电平触发** 时，参数 `origin` 返回事件触发 **之前** 的位图状态。
    ///
    /// ## 边沿触发
    ///
    /// **边沿触发** 源于数字电路，是一种类比概念，是指事件状态发生改变（ **1** 变 **0** 或 **0** 变 **1** ）时产生的唤醒信号，包括：
    ///
    /// + [`Trigger::SetAll`] 所有事件位发生翻转触发
    /// + [`Trigger::SetAny`] 任意事件位发生翻转触发
    ///
    /// **边沿触发** 时，参数 `consumption` 没有意义。
    /// **边沿触发** 时，必须要有一个初始状态，就像数字电路一样：
    ///
    /// + 当位的初始值为 **0** (低电平)，然后跳变到 **1** (高电平)的瞬间被称为上升沿。此时触发的事件被称为上升沿触发。
    /// + 当位的初始值为 **1** (高电平)，然后跳变到 **0** (低电平)的瞬间被称为下降沿。此时触发的事件被称为下降沿触发。
    ///
    /// 初始值就由参数 `origin` 指明，触发后，事件位发生跳变，跳变后的结果还是由参数 `origin` 返回。
    ///
    /// # 参数说明
    ///
    /// + tg: 事件触发条件
    ///   + **电平触发**
    ///     + [`Trigger::SetAll`] 所有事件位被置 **1** 触发
    ///     + [`Trigger::SetAny`] 任意事件位被置 **1** 触发
    ///     + [`Trigger::ClearAll`] 所有事件位被清 **0** 触发
    ///     + [`Trigger::ClearAny`] 任意事件位被清 **0** 触发
    ///   + **边沿触发**
    ///     + [`Trigger::SetAll`] 所有事件位发生翻转触发
    ///     + [`Trigger::SetAny`] 任意事件位发生翻转触发
    /// + consumption: 事件触发后是否清除事件
    /// + origin: 事件位图
    ///   + **电平触发** 时，参数 `origin` 用于返回事件触发 **之前** 的位图状态。
    ///   + **边沿触发** 时
    ///     + 输入时 `origin` 指明事件标志位图的初始值，用于评估事件位图是否发生变化。
    ///     + 输出时 `origin` 返回触发后的事件标志位图状态（可作为下一次调用的初始值）。
    /// + msk: 事件的位图掩码，表示只关注掩码部分的事件
    ///
    /// # 上下文
    ///
    /// + 任意
    ///
    /// # 错误码
    ///
    /// + [`FlgError::Ok`] 没有错误
    /// + [`FlgError::NotInit`] 事件标志没有初始化
    /// + [`FlgError::NoEvent`] 没有检测到事件
    ///
    /// # 示例
    ///
    /// ```rust
    /// extern crate alloc;
    /// use alloc::sync::Arc;
    ///
    /// use xwrust::xwos::sync::flg::*;
    ///
    /// pub fn xwrust_example_flg() {
    /// }
    /// ```
    pub fn trywait(&self, tg: Trigger, consumption: bool,
                   origin: &mut Bmp<N>, msk: &Bmp<N>) -> FlgError {
        unsafe {
            let mut rc = xwrustffi_flg_acquire(self.flg.get() as _, *self.tik.get());
            if rc == 0 {
                rc = xwrustffi_flg_trywait(self.flg.get() as _, tg as XwSq, consumption,
                                           origin.bmp.get() as _, msk.bmp.get() as _);
                xwrustffi_flg_put(self.flg.get() as _);
                if XWOK == rc {
                    FlgError::Ok(rc)
                } else if -ENODATA == rc {
                    FlgError::NoEvent(rc)
                } else {
                    FlgError::Unknown(rc)
                }
            } else {
                FlgError::NotInit(rc)
            }
        }
    }

    /// 等待事件，且等待不可被中断
    ///
    /// + 当没有检测到事件时，线程会阻塞等待。
    /// + 当检测到事件时，线程被唤醒，然后返回 [`FlgError::Ok`] 。
    /// + 当线程阻塞等待被中断时，返回 [`FlgError::Interrupt`] 。
    ///
    /// # 事件的触发条件
    ///
    /// 事件的触发条件可通过参数 `tg` 设置。事件的触发条件分为两类。
    ///
    /// ## 电平触发
    ///
    /// **电平触发** 源于数字电路，是一种类比概念，是指事件位图的特定状态（ **1** 或 **0** ）所产生的触发事件，包括：
    ///
    /// + [`Trigger::SetAll`] 所有事件位被置 **1** 触发
    /// + [`Trigger::SetAny`] 任意事件位被置 **1** 触发
    /// + [`Trigger::ClearAll`] 所有事件位被清 **0** 触发
    /// + [`Trigger::ClearAny`] 任意事件位被清 **0** 触发
    ///
    /// **电平触发** 时，若参数 `consumption` 取值 `true` ，表示线程在读取事件位图后，会 **清除** 事件位图。
    /// **电平触发** 时，参数 `origin` 返回事件触发 **之前** 的位图状态。
    ///
    /// ## 边沿触发
    ///
    /// **边沿触发** 源于数字电路，是一种类比概念，是指事件状态发生改变（ **1** 变 **0** 或 **0** 变 **1** ）时产生的唤醒信号，包括：
    ///
    /// + [`Trigger::SetAll`] 所有事件位发生翻转触发
    /// + [`Trigger::SetAny`] 任意事件位发生翻转触发
    ///
    /// **边沿触发** 时，参数 `consumption` 没有意义。
    /// **边沿触发** 时，必须要有一个初始状态，就像数字电路一样：
    ///
    /// + 当位的初始值为 **0** (低电平)，然后跳变到 **1** (高电平)的瞬间被称为上升沿。此时触发的事件被称为上升沿触发。
    /// + 当位的初始值为 **1** (高电平)，然后跳变到 **0** (低电平)的瞬间被称为下降沿。此时触发的事件被称为下降沿触发。
    ///
    /// 初始值就由参数 `origin` 指明，触发后，事件位发生跳变，跳变后的结果还是由参数 `origin` 返回。
    ///
    /// # 参数说明
    ///
    /// + tg: 事件触发条件
    ///   + **电平触发**
    ///     + [`Trigger::SetAll`] 所有事件位被置 **1** 触发
    ///     + [`Trigger::SetAny`] 任意事件位被置 **1** 触发
    ///     + [`Trigger::ClearAll`] 所有事件位被清 **0** 触发
    ///     + [`Trigger::ClearAny`] 任意事件位被清 **0** 触发
    ///   + **边沿触发**
    ///     + [`Trigger::SetAll`] 所有事件位发生翻转触发
    ///     + [`Trigger::SetAny`] 任意事件位发生翻转触发
    /// + consumption: 事件触发后是否清除事件
    /// + origin: 事件位图
    ///   + **电平触发** 时，参数 `origin` 用于返回事件触发 **之前** 的位图状态。
    ///   + **边沿触发** 时
    ///     + 输入时 `origin` 指明事件标志位图的初始值，用于评估事件位图是否发生变化。
    ///     + 输出时 `origin` 返回触发后的事件标志位图状态（可作为下一次调用的初始值）。
    /// + msk: 事件的位图掩码，表示只关注掩码部分的事件
    ///
    /// # 上下文
    ///
    /// + 线程
    ///
    /// # 错误码
    ///
    /// + [`FlgError::Ok`] 没有错误
    /// + [`FlgError::NotInit`] 事件标志没有初始化
    /// + [`FlgError::Interrupt`] 等待被中断
    /// + [`FlgError::NotThreadContext`] 不在线程上下文内
    ///
    /// # 示例
    ///
    /// ```rust
    /// extern crate alloc;
    /// use alloc::sync::Arc;
    ///
    /// use xwrust::xwos::sync::flg::*;
    ///
    /// pub fn xwrust_example_flg() {
    /// }
    /// ```
    pub fn wait_unintr(&self, tg: Trigger, consumption: bool,
                       origin: &mut Bmp<N>, msk: &Bmp<N>) -> FlgError {
        unsafe {
            let mut rc = xwrustffi_flg_acquire(self.flg.get() as _, *self.tik.get());
            if rc == 0 {
                rc = xwrustffi_flg_wait_unintr(self.flg.get() as _, tg as XwSq, consumption,
                                               origin.bmp.get() as _, msk.bmp.get() as _);
                xwrustffi_flg_put(self.flg.get() as _);
                if XWOK == rc {
                    FlgError::Ok(rc)
                } else if -EINTR == rc {
                    FlgError::Interrupt(rc)
                } else if -ENOTTHDCTX == rc {
                    FlgError::NotThreadContext(rc)
                } else {
                    FlgError::Unknown(rc)
                }
            } else {
                FlgError::NotInit(rc)
            }
        }
    }

    /// 绑定事件标志对象到信号选择器
    ///
    /// + 事件标志绑定到信号选择器上时，采用 **非独占** 的方式进行绑定。
    /// + 绑定成功，通过 [`Ok()`] 返回 [`FlgSel<'a, N, M>`] 。
    /// + 如果位置已被其他 **同步对象** 以 **独占** 的方式占领，通过 [`Err()`] 返回 [`FlgError::SelPosBusy`] 。
    /// + 当指定的位置超出范围（例如 [`Sel<M>`] 只有8个位置，用户偏偏要绑定到位置9 ），通过 [`Err()`] 返回 [`FlgError::OutOfSelPos`] 。
    /// + 重复绑定，通过 [`Err()`] 返回 [`FlgError::AlreadyBound`] 。
    ///
    /// [`FlgSel<'a, N, M>`] 中包含事件标志的绑定信息。 [`FlgSel<'a, N, M>`] 与 [`Flg<N>`] 与 [`Sel<M>`] 具有相同的生命周期约束 `'a` 。
    /// [`FlgSel::selected()`] 可用来判断事件标志是否被选择。当 [`FlgSel<'a, N, M>`] [`drop()`] 时，会自动解绑。
    ///
    /// # 参数说明
    ///
    /// + sel: 信号选择器的引用
    /// + pos: 位置
    ///
    /// # 上下文
    ///
    /// + 任意
    ///
    /// # 错误码
    ///
    /// + [`FlgError::OutOfSelPos`] 信号选择器的位置超出范围
    /// + [`FlgError::AlreadyBound`] 事件标志已经绑定
    /// + [`FlgError::SelPosBusy`] 信号选择器的位置被占用
    ///
    /// # 示例
    ///
    /// ```rust
    /// pub fn xwrust_example_sel() {
    ///     // ...省略...
    ///     let flg0 = Arc::new(Flg::new());
    ///     flg0.init();
    ///     let flg0sel = match flg0.bind(&sel, 0) {
    ///         Ok(s) => { // 绑定成功，`s` 为 `FlgSel`
    ///             s
    ///         },
    ///         Err(e) => { // 绑定失败，`e` 为 `SelError`
    ///             return;
    ///         }
    ///     };
    ///     // ...省略...
    /// }
    /// ```
    ///
    /// [`FlgSel<'a, N, M>`]: FlgSel
    /// [`Ok()`]: <https://doc.rust-lang.org/core/result/enum.Result.html#variant.Ok>
    /// [`Err()`]: <https://doc.rust-lang.org/core/result/enum.Result.html#variant.Err>
    /// [`Sel<M>`]: super::sel::Sel
    /// [`drop()`]: https://doc.rust-lang.org/std/mem/fn.drop.html
    pub fn bind<'a, const M: XwSz>(&'a self, sel: &'a Sel<M>, pos: XwSq) ->
                                   Result<FlgSel<'a, N, M>, FlgError>
    where
        [XwBmp; (M + XwBmp::BITS as usize - 1) / XwBmp::BITS as usize]: Sized
    {
        unsafe {
            let mut rc = xwrustffi_flg_acquire(self.flg.get() as _, *self.tik.get());
            if rc == 0 {
                rc = xwrustffi_flg_bind(self.flg.get() as _, sel.sel.get() as _, pos);
                if XWOK == rc {
                    Ok(FlgSel {
                        flg: self,
                        sel: sel,
                        pos: pos,
                    })
                } else if -ECHRNG == rc {
                    Err(FlgError::OutOfSelPos(rc))
                } else if -EALREADY == rc {
                    Err(FlgError::AlreadyBound(rc))
                } else if -EBUSY == rc {
                    Err(FlgError::SelPosBusy(rc))
                } else {
                    Err(FlgError::Unknown(rc))
                }
            } else {
                Err(FlgError::NotInit(rc))
            }
        }
    }
}

/// 事件标志的选择子
///
/// `FlgSel<'a, N, M>` 与 [`Flg<N>`] 与 [`Sel<M>`] 具有相同的生命周期约束 `'a` 。因为 `FlgSel<'a, N, M>` 中包含了 [`Flg<N>`] 与 [`Sel<M>`] 的引用。
///
/// `FlgSel<'a, N, M>` 中包含了绑定的位置信息，事件标志采用 **非独占** 的方式进行绑定。
///
/// [`FlgSel::selected()`] 可用来判断事件标志是否被选择。
///
/// 当 `FlgSel<'a, N, M>` 被 [`drop()`] 时，会自动将 [`Flg<N>`] 从 [`Sel<M>`] 解绑。
///
/// [`Sel<M>`]: super::sel::Sel
/// [`drop()`]: https://doc.rust-lang.org/std/mem/fn.drop.html
pub struct FlgSel<'a, const N: XwSz, const M: XwSz>
where
    [XwBmp; (N + XwBmp::BITS as usize - 1) / XwBmp::BITS as usize]: Sized,
    [XwBmp; (M + XwBmp::BITS as usize - 1) / XwBmp::BITS as usize]: Sized
{
    /// 事件标志
    pub flg: &'a Flg<N>,
    /// 信号选择器
    pub sel: &'a Sel<M>,
    /// 位置
    pub pos: XwSq,
}

unsafe impl<'a, const N: XwSz, const M: XwSz> Send for FlgSel<'a, N, M>
where
    [XwBmp; (N + XwBmp::BITS as usize - 1) / XwBmp::BITS as usize]: Sized,
    [XwBmp; (M + XwBmp::BITS as usize - 1) / XwBmp::BITS as usize]: Sized
{}

unsafe impl<'a, const N: XwSz, const M: XwSz> Sync for FlgSel<'a, N, M>
where
    [XwBmp; (N + XwBmp::BITS as usize - 1) / XwBmp::BITS as usize]: Sized,
    [XwBmp; (M + XwBmp::BITS as usize - 1) / XwBmp::BITS as usize]: Sized
{}

impl<'a, const N: XwSz, const M: XwSz> Drop for FlgSel<'a, N, M>
where
    [XwBmp; (N + XwBmp::BITS as usize - 1) / XwBmp::BITS as usize]: Sized,
    [XwBmp; (M + XwBmp::BITS as usize - 1) / XwBmp::BITS as usize]: Sized
{
    fn drop(&mut self) {
        unsafe {
            xwrustffi_flg_unbind(self.flg.flg.get() as _, self.flg.flg.get() as _);
            xwrustffi_flg_put(self.flg.flg.get() as _);
        }
    }
}

impl<'a, const N: XwSz, const M: XwSz> FlgSel<'a, N, M>
where
    [XwBmp; (N + XwBmp::BITS as usize - 1) / XwBmp::BITS as usize]: Sized,
    [XwBmp; (M + XwBmp::BITS as usize - 1) / XwBmp::BITS as usize]: Sized
{
    /// 判断触发的 **选择信号** 是否包括此事件标志
    ///
    /// # 示例
    ///
    /// ```rust
    ///     let msk = Bmp::<8>::new(); // 8位位图
    ///     msk.s1all(); // 掩码为0xFF
    ///     loop {
    ///         let res = sel.select(&msk);
    ///         match res {
    ///             Ok(t) => { // 信号选择器上有 **选择信号** ， `t` 为 **选择信号** 的位图。
    ///                 if flg0sel.selected(&t) { // 事件标志被选择到
    ///                 }
    ///             },
    ///             Err(e) => { // 等待信号选择器失败，`e` 为 `SelError`
    ///                 flgeak;
    ///             },
    ///         }
    ///     }
    /// ```
    pub fn selected(&self, trg: &Bmp<M>) -> bool {
        trg.t1i(self.pos)
    }
}
