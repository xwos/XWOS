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
//! + 可以创建具有静态生命周期 [`static`] 约束的全局变量：
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
//! # 绑定到信号选择器
//!
//! 线程栅栏是 **同步对象** ，可以通过方法 [`Br::bind()`] 将线程栅栏绑定到信号选择器 [`Sel<M>`] 上，通过 [`Sel<M>`] ，单一线程可以同时等待多个不同的 **同步对象** 。
//!
//! 线程栅栏采用 **非独占** 的方式进行绑定。
//!
//!
//! # 示例
//!
//! [XWOS/xwam/xwrust-example/xwrust_example_br](https://gitee.com/xwos/XWOS/blob/main/xwam/xwrust-example/xwrust_example_br/src/lib.rs)
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
    pub(crate) fn xwrustffi_br_init(br: *mut c_void, num: XwSz, bmp: *mut XwBmp, msk: *mut XwBmp) -> XwEr;
    pub(crate) fn xwrustffi_br_fini(br: *mut c_void) -> XwEr;
    pub(crate) fn xwrustffi_br_grab(br: *mut c_void) -> XwEr;
    pub(crate) fn xwrustffi_br_put(br: *mut c_void) -> XwEr;
    pub(crate) fn xwrustffi_br_get_tik(br: *mut c_void) -> XwSq;
    pub(crate) fn xwrustffi_br_acquire(br: *mut c_void, tik: XwSq) -> XwEr;
    pub(crate) fn xwrustffi_br_release(br: *mut c_void, tik: XwSq) -> XwEr;
    pub(crate) fn xwrustffi_br_bind(br: *mut c_void, sel: *mut c_void, pos: XwSq) -> XwEr;
    pub(crate) fn xwrustffi_br_unbind(br: *mut c_void, sel: *mut c_void) -> XwEr;
    pub(crate) fn xwrustffi_br_wait(br: *mut c_void) -> XwEr;
    pub(crate) fn xwrustffi_br_wait_to(br: *mut c_void, to: XwTm) -> XwEr;
}

/// 线程栅栏的错误码
#[derive(Debug)]
pub enum BrError {
    /// 没有错误
    Ok(XwEr),
    /// 线程栅栏没有初始化
    NotInit(XwEr),
    /// 线程数量超出范围
    OutOfRange(XwEr),
    /// 等待被中断
    Interrupt(XwEr),
    /// 等待超时
    Timedout(XwEr),
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

impl BrError {
    /// 消费掉 `BrError` 自身，返回内部的错误码。
    pub fn unwrap(self) -> XwEr {
        match self {
            Self::Ok(rc) => rc,
            Self::NotInit(rc) => rc,
            Self::OutOfRange(rc) => rc,
            Self::Interrupt(rc) => rc,
            Self::Timedout(rc) => rc,
            Self::NotThreadContext(rc) => rc,
            Self::OutOfSelPos(rc) => rc,
            Self::AlreadyBound(rc) => rc,
            Self::SelPosBusy(rc) => rc,
            Self::Unknown(rc) => rc,
        }
    }

    /// 如果错误码是 [`BrError::Ok`] ，返回 `true` 。
    pub const fn is_ok(&self) -> bool {
        matches!(*self, Self::Ok(_))
    }

    /// 如果错误码不是 [`BrError::Ok`] ，返回 `true` 。
    pub const fn is_err(&self) -> bool {
        !self.is_ok()
    }
}

/// XWOS线程栅栏对象占用的内存大小
#[cfg(target_pointer_width = "32")]
pub const SIZEOF_XWOS_BR: usize = 64;

/// XWOS线程栅栏对象占用的内存大小
#[cfg(target_pointer_width = "64")]
pub const SIZEOF_XWOS_BR: usize = 128;

/// 用于构建线程栅栏的内存数组类型
#[repr(C)]
#[cfg_attr(target_pointer_width = "32", repr(align(8)))]
#[cfg_attr(target_pointer_width = "64", repr(align(16)))]
pub(crate) struct XwosBr<const N: XwSz>
where
    [XwBmp; (N + XwBmp::BITS as usize - 1) / XwBmp::BITS as usize]: Sized
{
    pub(crate) obj: [u8; SIZEOF_XWOS_BR],
    pub(crate) bmp: [XwBmp; (N + XwBmp::BITS as usize - 1) / XwBmp::BITS as usize],
    pub(crate) msk: [XwBmp; (N + XwBmp::BITS as usize - 1) / XwBmp::BITS as usize],
}

/// 线程栅栏对象结构体
pub struct Br<const N: XwSz>
where
    [XwBmp; (N + XwBmp::BITS as usize - 1) / XwBmp::BITS as usize]: Sized
{
    /// 用于初始化XWOS线程栅栏对象的内存空间
    pub(crate) br: UnsafeCell<XwosBr<N>>,
    /// 线程栅栏对象的标签
    pub(crate) tik: UnsafeCell<XwSq>,
}

unsafe impl<const N: XwSz> Send for Br<N>
where
    [XwBmp; (N + XwBmp::BITS as usize - 1) / XwBmp::BITS as usize]: Sized
{}

unsafe impl<const N: XwSz> Sync for Br<N>
where
    [XwBmp; (N + XwBmp::BITS as usize - 1) / XwBmp::BITS as usize]: Sized
{}

impl<const N: XwSz> Drop for Br<N>
where
    [XwBmp; (N + XwBmp::BITS as usize - 1) / XwBmp::BITS as usize]: Sized
{
    fn drop(&mut self) {
        unsafe {
            xwrustffi_br_fini(self.br.get() as _);
        }
    }
}

impl<const N: XwSz> Br<N>
where
    [XwBmp; (N + XwBmp::BITS as usize - 1) / XwBmp::BITS as usize]: Sized
{
    /// 新建线程栅栏对象
    ///
    /// 此方法是编译期方法。
    ///
    /// # 示例
    ///
    /// + 具有 [`static`] 约束的全局变量全局变量：
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
    /// extern crate alloc;
    /// use alloc::sync::Arc;
    ///
    /// pub fn xwrust_example_br() {
    ///     let br = Arc::new(Br::<8>::new());
    /// }
    /// ```
    ///
    /// [`static`]: https://doc.rust-lang.org/std/keyword.static.html
    pub const fn new() -> Self {
        Self {
            br: UnsafeCell::new(XwosBr {
                obj: [0; SIZEOF_XWOS_BR],
                bmp: [0; (N + XwBmp::BITS as usize - 1) / XwBmp::BITS as usize],
                msk: [0; (N + XwBmp::BITS as usize - 1) / XwBmp::BITS as usize],
            }),
            tik: UnsafeCell::new(0),
        }
    }

    /// 初始化线程栅栏对象
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
                *self.tik.get() = xwrustffi_br_get_tik(self.br.get() as _);
            }
        }
    }

    /// 获取线程栅栏中线程槽的数量
    pub const fn get_num(&self) -> XwSz {
        N
    }

    /// 等待所有线程到达栅栏
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
                    BrError::Ok(rc)
                } else if -ECHRNG == rc {
                    BrError::OutOfRange(rc)
                } else if -EINTR == rc {
                    BrError::Interrupt(rc)
                } else if -ENOTTHDCTX == rc {
                    BrError::NotThreadContext(rc)
                } else {
                    BrError::Unknown(rc)
                }
            } else {
                BrError::NotInit(rc)
            }
        }
    }

    /// 限时等待所有线程到达栅栏
    ///
    /// + 当线程栅栏中的线程数量小于指定数量，线程会阻塞等待，等待时会指定一个唤醒时间点。
    /// + 当线程栅栏中的线程数量达到指定数量，全部线程被唤醒，然后返回 [`BrError::Ok`] 。
    /// + 当线程阻塞等待被中断时，返回 [`BrError::Interrupt`] 。
    /// + 当到达指定的唤醒时间点，线程被唤醒，并返回 [`BrError::Timedout`] 。
    ///
    /// # 参数说明
    ///
    /// + to: 期望唤醒的时间点
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
                    BrError::Ok(rc)
                } else if -ECHRNG == rc {
                    BrError::OutOfRange(rc)
                } else if -EINTR == rc {
                    BrError::Interrupt(rc)
                } else if -ETIMEDOUT == rc {
                    BrError::Timedout(rc)
                } else if -ENOTTHDCTX == rc {
                    BrError::NotThreadContext(rc)
                } else {
                    BrError::Unknown(rc)
                }
            } else {
                BrError::NotInit(rc)
            }
        }
    }

    /// 绑定线程栅栏对象到信号选择器
    ///
    /// + 线程栅栏绑定到信号选择器上时，采用 **非独占** 的方式进行绑定。
    /// + 绑定成功，通过 [`Ok()`] 返回 [`BrSel<'a, N, M>`] 。
    /// + 如果位置已被其他 **同步对象** 以 **独占** 的方式占领，通过 [`Err()`] 返回 [`BrError::SelPosBusy`] 。
    /// + 当指定的位置超出范围（例如 [`Sel<M>`] 只有8个位置，用户偏偏要绑定到位置9 ），通过 [`Err()`] 返回 [`BrError::OutOfSelPos`] 。
    /// + 重复绑定，通过 [`Err()`] 返回 [`BrError::AlreadyBound`] 。
    ///
    /// [`BrSel<'a, N, M>`] 中包含线程栅栏的绑定信息。 [`BrSel<'a, N, M>`] 与 [`Br<N>`] 与 [`Sel<M>`] 具有相同的生命周期约束 `'a` 。
    /// [`BrSel::selected()`] 可用来判断线程栅栏是否被选择。当 [`BrSel<'a, N, M>`] [`drop()`] 时，会自动解绑。
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
    /// + [`BrError::OutOfSelPos`] 信号选择器的位置超出范围
    /// + [`BrError::AlreadyBound`] 线程栅栏已经绑定
    /// + [`BrError::SelPosBusy`] 信号选择器的位置被占用
    ///
    /// # 示例
    ///
    /// ```rust
    /// pub fn xwrust_example_sel() {
    ///     // ...省略...
    ///     let br0 = Arc::new(Br::new());
    ///     br0.init();
    ///     let br0sel = match br0.bind(&sel, 0) {
    ///         Ok(s) => { // 绑定成功，`s` 为 `BrSel`
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
    /// [`BrSel<'a, N, M>`]: BrSel
    /// [`Ok()`]: <https://doc.rust-lang.org/core/result/enum.Result.html#variant.Ok>
    /// [`Err()`]: <https://doc.rust-lang.org/core/result/enum.Result.html#variant.Err>
    /// [`Sel<M>`]: super::sel::Sel
    /// [`drop()`]: https://doc.rust-lang.org/std/mem/fn.drop.html
    pub fn bind<'a, const M: XwSz>(&'a self, sel: &'a Sel<M>, pos: XwSq) ->
                                   Result<BrSel<'a, N, M>, BrError>
    where
        [XwBmp; (M + XwBmp::BITS as usize - 1) / XwBmp::BITS as usize]: Sized
    {
        unsafe {
            let mut rc = xwrustffi_br_acquire(self.br.get() as _, *self.tik.get());
            if rc == 0 {
                rc = xwrustffi_br_bind(self.br.get() as _, sel.sel.get() as _, pos);
                if XWOK == rc {
                    Ok(BrSel {
                        br: self,
                        sel: sel,
                        pos: pos,
                    })
                } else if -ECHRNG == rc {
                    Err(BrError::OutOfSelPos(rc))
                } else if -EALREADY == rc {
                    Err(BrError::AlreadyBound(rc))
                } else if -EBUSY == rc {
                    Err(BrError::SelPosBusy(rc))
                } else {
                    Err(BrError::Unknown(rc))
                }
            } else {
                Err(BrError::NotInit(rc))
            }
        }
    }
}

/// 线程栅栏的选择子
///
/// `BrSel<'a, N, M>` 与 [`Br<N>`] 与 [`Sel<M>`] 具有相同的生命周期约束 `'a` 。因为 `BrSel<'a, N, M>` 中包含了 [`Br<N>`] 与 [`Sel<M>`] 的引用。
///
/// `BrSel<'a, N, M>` 中包含了绑定的位置信息，线程栅栏采用 **非独占** 的方式进行绑定。
///
/// [`BrSel::selected()`] 可用来判断线程栅栏是否被选择。
///
/// 当 `BrSel<'a, N, M>` 被 [`drop()`] 时，会自动将 [`Br<N>`] 从 [`Sel<M>`] 解绑。
///
/// [`Sel<M>`]: super::sel::Sel
/// [`drop()`]: https://doc.rust-lang.org/std/mem/fn.drop.html
pub struct BrSel<'a, const N: XwSz, const M: XwSz>
where
    [XwBmp; (N + XwBmp::BITS as usize - 1) / XwBmp::BITS as usize]: Sized,
    [XwBmp; (M + XwBmp::BITS as usize - 1) / XwBmp::BITS as usize]: Sized
{
    /// 线程栅栏
    pub br: &'a Br<N>,
    /// 信号选择器
    pub sel: &'a Sel<M>,
    /// 位置
    pub pos: XwSq,
}

unsafe impl<'a, const N: XwSz, const M: XwSz> Send for BrSel<'a, N, M>
where
    [XwBmp; (N + XwBmp::BITS as usize - 1) / XwBmp::BITS as usize]: Sized,
    [XwBmp; (M + XwBmp::BITS as usize - 1) / XwBmp::BITS as usize]: Sized
{}

unsafe impl<'a, const N: XwSz, const M: XwSz> Sync for BrSel<'a, N, M>
where
    [XwBmp; (N + XwBmp::BITS as usize - 1) / XwBmp::BITS as usize]: Sized,
    [XwBmp; (M + XwBmp::BITS as usize - 1) / XwBmp::BITS as usize]: Sized
{}

impl<'a, const N: XwSz, const M: XwSz> Drop for BrSel<'a, N, M>
where
    [XwBmp; (N + XwBmp::BITS as usize - 1) / XwBmp::BITS as usize]: Sized,
    [XwBmp; (M + XwBmp::BITS as usize - 1) / XwBmp::BITS as usize]: Sized
{
    fn drop(&mut self) {
        unsafe {
            xwrustffi_br_unbind(self.br.br.get() as _, self.br.br.get() as _);
            xwrustffi_br_put(self.br.br.get() as _);
        }
    }
}

impl<'a, const N: XwSz, const M: XwSz> BrSel<'a, N, M>
where
    [XwBmp; (N + XwBmp::BITS as usize - 1) / XwBmp::BITS as usize]: Sized,
    [XwBmp; (M + XwBmp::BITS as usize - 1) / XwBmp::BITS as usize]: Sized
{
    /// 判断触发的 **选择信号** 是否包括此线程栅栏
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
    ///                 if br0sel.selected(&t) { // 线程栅栏被选择到
    ///                 }
    ///             },
    ///             Err(e) => { // 等待信号选择器失败，`e` 为 `SelError`
    ///                 break;
    ///             },
    ///         }
    ///     }
    /// ```
    pub fn selected(&self, trg: &Bmp<M>) -> bool {
        trg.t1i(self.pos)
    }
}
