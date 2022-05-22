//! XWOS RUST：信号选择器
//! ========
//!
//! 信号选择器使用位图 [`Bmp`] 来管理一组 **同步对象** 。使得单一线程可以同时等待多个 **同步对象** 。
//!
//! 每个 **同步对象** 在信号选择器位图中都绑定一个特定的 **位** ，
//!
//! 当这些 **同步对象** 发送 **选择信号** 时，信号选择器位图中特定的 **位** 被置 **1** ，同时唤醒正在等待信号选择器的线程。
//! 线程唤醒后可以通过检测哪些 **位** 被置 **1** 来判断哪些 **同步对象** 发送了 **选择信号** 。
//!
//!
//! # 同步对象的绑定与解绑
//!
//! XWOS RUST的所有 **同步对象** 都有一个相似的用于绑定的方法：
//!
//! + [`Sem::bind()`]
//! + [`Cond::bind()`]
//! + [`Flg::bind()`]
//! + [`Br::bind()`]
//! + [`Sel::bind()`]
//!
//! 绑定后会返回各个 **同步对象** 的 **选择子** ：
//!
//! + [`SemSel`]
//! + [`CondSel`]
//! + [`FlgSel`]
//! + [`BrSel`]
//! + [`SelSel`]
//!
//! **选择子** 被 [`drop()`] 时，会自动解绑。
//!
//!
//! **同步对象** 绑定操作又分为 **独占绑定** 与 **非独占绑定** ：
//!
//! + 独占绑定：是指 **同步对象** 一旦绑定了信号选择器位图中的某一位后，其他 **同步对象** 不可再绑定此位，发送的 **选择信号** 也被称为 **独占** 方式的 **选择信号** ；
//! + 非独占绑定：是指 **同步对象** 一旦绑定了信号选择器位图中的某一位后，其他 **同步对象** 还可继续再绑定此位，发送的 **选择信号** 也被称为 **非独占** 方式的 **选择信号** 。
//!
//!
//! **同步对象** 采用的绑定方式：
//!
//! + 信号量：独占绑定
//! + 条件量：非独占绑定
//! + 事件标志：非独占绑定
//! + 线程栅栏：非独占绑定
//! + 信号选择器：非独占绑定
//!
//!
//! # 选择信号
//!
//! ## 选择信号的设置
//!
//! **选择信号** 的设置，对于不同的 **同步对象** 有不同解释：
//!
//! + 信号量
//!   + 计数器的值大于 **0** 时就会被设置。
//! + 条件量
//!   + 广播操作 [`Cond::broadcast`]
//! + 事件标志
//!   + 事件标志位图中任何一位发生改变的操作：
//!     + [`Flg::s1m`]
//!     + [`Flg::s1i`]
//!     + [`Flg::c0m`]
//!     + [`Flg::c0i`]
//!     + [`Flg::x1m`]
//!     + [`Flg::x1i`]
//! + 线程栅栏
//!   + 所有线程抵达栅栏，并同时被唤醒；
//! + 信号选择器
//!   + 信号选择器本身也是 **同步对象** ，也可绑定在另一个信号选择器上。当源信号选择器收到了 **选择信号** ，会将其传递到绑定的另一个目的信号选择器上。
//!
//! ## 选择信号的清除
//!
//! ### **独占** 方式的 **选择信号**
//!
//! + 信号量：当信号量中的计数器的值小于等于 **0** 时， **选择信号** 才会被清除。
//!
//! ### **非独占** 方式的 **选择信号**
//!
//! **非独占** 方式绑定的 **同步对象** 向信号选择器发送 **选择信号** 后，其位图中的位置会被置 **1** 。
//! 同时会唤醒所有等待的线程，此时线程们会竞争进入信号选择器的临界区。
//!
//! 最先进入的线程会读取信号选择器的 **选择信号** 位图，并与调用函数时传递的 **掩码** 进行比较，判断是否有 **掩码** 中的 **选择信号** ：
//!
//! + 如果有，会清除信号选择器位图中 **所有** 的 **非独占** 方式的 **选择信号** ，包括 **掩码** 中没有设置的 **选择信号** 。
//!   因此后续线程将无法再检测到任何 **非独占** 方式的 **选择信号** ，会重新阻塞等待。
//!   XWOS不推荐在信号选择器上，多于1个线程等待。
//! + 如果没有，线程会重新阻塞等待。然后下一个线程进入临界区检测。
//!
//!
//! # 创建
//!
//! XWOS RUST的信号量可使用 [`Sel::new()`] 创建。
//! 创建时需要指明泛型中的常量 `N` ，表示信号选择器中有个多少个位置可以绑定。
//!
//! + 可以创建具有静态生命周期 [`'static`] 约束的全局变量：
//!
//! ```rust
//! use xwrust::xwos::sync::sel::*;
//!
//! static GLOBAL_SEL: Sel<8> = Sel::new();
//! ```
//!
//! + 也可以使用 [`alloc::sync::Arc`] 在heap中创建：
//!
//! ```rust
//! extern crate alloc;
//! use alloc::sync::Arc;
//!
//! use xwrust::xwos::sync::sel::*;
//!
//! pub fn xwrust_example_sel() {
//!     let sel = Arc::new(Sel::<8>::new());
//! }
//! ```
//!
//!
//! # 初始化
//!
//! 无论以何种方式创建的信号量，都必须在使用前调用 [`Sel::init()`] 进行初始化：
//!
//! ```rust
//! pub fn xwrust_example_sel() {
//!     GLOBAL_SEL.init();
//!     sel.init();
//! }
//! ```
//!
//!
//! ## 等待信号选择器中的 **选择信号**
//!
//! [`Sel::select()`] 可用于等待信号选择器中的 **选择信号** 。
//!
//! + 当没有同步对象向信号选择器发送 **选择信号** 时，线程会阻塞等待。
//! + 当 **任意** 同步对象向信号选择器发送 **选择信号** 时，线程被唤醒，然后返回 [`Bmp<N>`] 。
//! + 当线程阻塞等待被中断时，返回 [`SelError::Interrupt`] 。
//!
//! ## 限时等待信号选择器中的 **选择信号**
//!
//! [`Sel::select_to()`] 可用于限时等待信号选择器中的 **选择信号** 。
//!
//! + 当没有同步对象向信号选择器发送 **选择信号** 时，线程会阻塞等待，等待时会指定一个唤醒时间点。
//! + 当 **任意** 同步对象向信号选择器发送 **选择信号** 时，线程被唤醒，然后返回 [`Bmp<N>`] 。
//! + 当线程阻塞等待被中断时，返回 [`SelError::Interrupt`] 。
//! + 当到达指定的唤醒时间点，线程被唤醒，并返回 [`SelError::Timedout`] 。
//!
//! [`Sel::tryselect()`] 检测信号选择器中是否有 **选择信号** 。
//!
//! + 当检测到 **任意** 同步对象向信号选择器发送 **选择信号** 时，立即返回 [`Bmp<N>`] 。
//! + 当没有同步对象向信号选择器发送 **选择信号** 时，立即返回 [`SelError::NoSignal`] 。
//!
//!
//! # 获取信号选择器中同步对象槽的数量
//!
//! 可以通过方法 [`Sel::get_num()`] 获取信号选择器中位置的数量，这个值是在创建信号选择器时通过泛型常量 `N` 指定的。
//!
//!
//! # 绑定到其他信号选择器
//!
//! 信号选择器也是 **同步对象**， 可以通过方法 [`Sel::bind()`] 将信号选择器绑定到另一个信号选择器上，由此可将 **选择信号** 进行传递。
//!
//! 信号选择器采用 **非独占** 的方式进行绑定。
//!
//!
//! # 示例
//!
//! [XWOS/xwam/xwrust-example/xwrust_example_sel](https://gitee.com/xwos/XWOS/blob/main/xwam/xwrust-example/xwrust_example_sel/src/lib.rs)
//!
//!
//! [`Bmp`]: crate::xwbmp::Bmp
//! [`Sem::post()`]: super::sem::Sem::post
//! [`Cond::broadcast`]: super::cond::Cond::broadcast
//! [`Flg::s1m`]: super::flg::Flg::s1m
//! [`Flg::s1i`]: super::flg::Flg::s1i
//! [`Flg::c0m`]: super::flg::Flg::c0m
//! [`Flg::c0i`]: super::flg::Flg::c0i
//! [`Flg::x1m`]: super::flg::Flg::x1m
//! [`Flg::x1i`]: super::flg::Flg::x1i
//! [`'static`]: <https://doc.rust-lang.org/std/keyword.static.html>
//! [`alloc::sync::Arc`]: <https://doc.rust-lang.org/alloc/sync/struct.Arc.html>
//! [`drop()`]: https://doc.rust-lang.org/std/mem/fn.drop.html
//! [`Sem::bind()`]: super::sem::Sem::bind
//! [`Cond::bind()`]: super::cond::Cond::bind
//! [`Flg::bind()`]: super::flg::Flg::bind
//! [`Br::bind()`]: super::br::Br::bind
//! [`SemSel`]: super::sem::SemSel
//! [`CondSel`]: super::cond::CondSel
//! [`FlgSel`]: super::flg::FlgSel
//! [`BrSel`]: super::br::BrSel

extern crate core;
use core::ffi::*;
use core::cell::UnsafeCell;
use core::result::Result;

use crate::types::*;
use crate::errno::*;
use crate::xwbmp::*;


extern "C" {
    pub(crate) fn xwrustffi_sel_init(sel: *mut c_void, num: XwSz, bmp: *mut XwBmp, msk: *mut XwBmp) -> XwEr;
    pub(crate) fn xwrustffi_sel_fini(sel: *mut c_void) -> XwEr;
    pub(crate) fn xwrustffi_sel_grab(sel: *mut c_void) -> XwEr;
    pub(crate) fn xwrustffi_sel_put(sel: *mut c_void) -> XwEr;
    pub(crate) fn xwrustffi_sel_gettik(sel: *mut c_void) -> XwSq;
    pub(crate) fn xwrustffi_sel_acquire(sel: *mut c_void, tik: XwSq) -> XwEr;
    pub(crate) fn xwrustffi_sel_release(sel: *mut c_void, tik: XwSq) -> XwEr;
    pub(crate) fn xwrustffi_sel_bind(src: *mut c_void, dst: *mut c_void, pos: XwSq) -> XwEr;
    pub(crate) fn xwrustffi_sel_unbind(src: *mut c_void, dst: *mut c_void) -> XwEr;
    pub(crate) fn xwrustffi_sel_select(sel: *mut c_void, msk: *mut XwBmp, trg: *mut XwBmp) -> XwEr;
    pub(crate) fn xwrustffi_sel_select_to(sel: *mut c_void, msk: *mut XwBmp, trg: *mut XwBmp, to: XwTm) -> XwEr;
    pub(crate) fn xwrustffi_sel_tryselect(sel: *mut c_void, msk: *mut XwBmp, trg: *mut XwBmp) -> XwEr;
}

/// 条件量的错误码
#[derive(Debug)]
pub enum SelError {
    /// 没有错误
    Ok,
    /// 条件量没有初始化
    NotInit,
    /// 等待被中断
    Interrupt,
    /// 等待超时
    Timedout,
    /// 没有检测到 **选择信号**
    NoSignal,
    /// 不在线程上下文内
    NotThreadContext,
    /// 信号选择器的位置超出范围
    OutOfSelPos,
    /// 信号量已经绑定
    AlreadyBound,
    /// 信号选择器的位置被占用
    SelPosBusy,
    /// 未知错误
    Unknown(XwEr),
}

/// XWOS条件量对象占用的内存大小
#[cfg(target_pointer_width = "32")]
pub const SIZEOF_XWOS_SEL: usize = 64;

/// XWOS条件量对象占用的内存大小
#[cfg(target_pointer_width = "64")]
pub const SIZEOF_XWOS_SEL: usize = 128;

/// 用于构建条件量的内存数组类型
#[repr(C)]
#[cfg_attr(target_pointer_width = "32", repr(align(8)))]
#[cfg_attr(target_pointer_width = "64", repr(align(16)))]
pub(crate) struct XwosSel<const N: XwSz>
where
    [XwBmp; ((N + XwBmp::BITS as usize - 1) / XwBmp::BITS as usize)]: Sized
{
    pub(crate) obj: [u8; SIZEOF_XWOS_SEL],
    pub(crate) bmp: [XwBmp; ((N + XwBmp::BITS as usize - 1) / XwBmp::BITS as usize)],
    pub(crate) msk: [XwBmp; ((N + XwBmp::BITS as usize - 1) / XwBmp::BITS as usize)],
}

/// 条件量对象结构体
pub struct Sel<const N: XwSz>
where
    [XwBmp; ((N + XwBmp::BITS as usize - 1) / XwBmp::BITS as usize)]: Sized
{
    /// 用于初始化XWOS条件量对象的内存空间
    pub(crate) sel: UnsafeCell<XwosSel<N>>,
    /// 条件量对象的标签
    pub(crate) tik: UnsafeCell<XwSq>,
}

unsafe impl<const N: XwSz> Send for Sel<N>
where
    [XwBmp; ((N + XwBmp::BITS as usize - 1) / XwBmp::BITS as usize)]: Sized
{}

unsafe impl<const N: XwSz> Sync for Sel<N>
where
    [XwBmp; ((N + XwBmp::BITS as usize - 1) / XwBmp::BITS as usize)]: Sized
{}

impl<const N: XwSz> Drop for Sel<N>
where
    [XwBmp; ((N + XwBmp::BITS as usize - 1) / XwBmp::BITS as usize)]: Sized
{
    fn drop(&mut self) {
        unsafe {
            xwrustffi_sel_fini(self.sel.get() as _);
        }
    }
}

impl<const N: XwSz> Sel<N>
where
    [XwBmp; ((N + XwBmp::BITS as usize - 1) / XwBmp::BITS as usize)]: Sized
{
    /// 新建信号选择器对象。
    ///
    /// 此方法是编译期方法。
    ///
    /// # 示例
    ///
    /// + 具有 [`'static`] 约束的全局变量全局变量：
    ///
    /// ```rust
    /// extern crate alloc;
    /// use xwrust::xwos::sync::sel::*;
    ///
    /// static GLOBAL_SEL: Sel<8> = Sel::new();
    /// ```
    ///
    /// + 在heap中创建：
    ///
    /// ```rust
    /// use alloc::sync::Arc;
    ///
    /// pub fn xwrust_example_sel() {
    ///     let sel = Arc::new(Sel::<8>::new());
    /// }
    /// ```
    ///
    /// [`'static`]: https://doc.rust-lang.org/std/keyword.static.html
    pub const fn new() -> Self {
        Self {
            sel: UnsafeCell::new(XwosSel {
                obj: [0; SIZEOF_XWOS_SEL],
                bmp: [0; ((N + XwBmp::BITS as usize - 1) / XwBmp::BITS as usize)],
                msk: [0; ((N + XwBmp::BITS as usize - 1) / XwBmp::BITS as usize)],
            }),
            tik: UnsafeCell::new(0),
        }
    }

    /// 初始化信号选择器对象。
    ///
    /// 信号选择器对象必须调用此方法一次，方可正常使用。
    ///
    /// # 上下文
    ///
    /// + 任意
    ///
    /// # 示例
    ///
    /// ```rust
    /// use xwrust::xwos::sync::sel::*;
    ///
    /// static GLOBAL_SEL: Sel<8> = Sel::new();
    ///
    /// pub fn xwrust_example_sel() {
    ///     // ...省略...
    ///     GLOBAL_SEL.init();
    ///     // 从此处开始 GLOBAL_SEL 可正常使用
    /// }
    /// ```
    pub fn init(&self) {
        unsafe {
            let rc = xwrustffi_sel_acquire(self.sel.get() as _, *self.tik.get());
            if rc == 0 {
                xwrustffi_sel_put(self.sel.get() as _);
            } else {
                xwrustffi_sel_init(self.sel.get() as _,
                                   N,
                                   &mut (*self.sel.get()).bmp as _,
                                   &mut (*self.sel.get()).msk as _);
                *self.tik.get() = xwrustffi_sel_gettik(self.sel.get() as _);
            }
        }
    }

    /// 获取信号选择器中线程槽的数量。
    pub const fn get_num(&self) -> XwSz {
        N
    }

    /// 等待信号选择器中的 **选择信号** 。
    ///
    /// + 当没有同步对象向信号选择器发送 **选择信号** 时，线程会阻塞等待。
    /// + 当 **任意** 同步对象向信号选择器发送 **选择信号** 时，线程被唤醒，然后返回 [`Bmp<N>`] 。
    /// + 当线程阻塞等待被中断时，返回 [`SelError::Interrupt`] 。
    ///
    /// # 上下文
    ///
    /// + 线程
    ///
    /// # 错误码
    ///
    /// + [`SelError::NotInit`] 信号量没有初始化
    /// + [`SelError::Interrupt`] 等待被中断
    /// + [`SelError::NotThreadContext`] 不在线程上下文内
    ///
    /// # 示例
    ///
    /// ```rust
    /// use xwrust::xwbmp::*;
    /// use xwrust::xwos::sync::sel::*;
    ///
    /// pub fn xwrust_example_sel() {
    ///     let sel = Sel::<8>::new();
    ///     sel.init();
    ///     // ...省略bind过程...
    ///     let msk = Bmp::<8>::new();
    ///     msk.s1all(); // 设置掩码
    ///     let res = sel.select(msk);
    ///     match res {
    ///         Ok(t) => { // 等待成功， `t` 为 **选择信号** 的位图
    ///         },
    ///         Err(e) => { // 等待失败， `e` 为 `SelError`
    ///         },
    ///     }
    /// }
    /// ```
    pub fn select(&self, msk: &Bmp<N>) -> Result<Bmp<N>, SelError> {
        unsafe {
            let mut rc = xwrustffi_sel_acquire(self.sel.get() as _, *self.tik.get());
            if rc == 0 {
                let trg = Bmp::<N>::new();
                rc = xwrustffi_sel_select(self.sel.get() as _,
                                          msk.bmp.get() as _, trg.bmp.get() as _);
                xwrustffi_sel_put(self.sel.get() as _);
                if XWOK == rc {
                    Ok(trg)
                } else if -EINTR == rc {
                    Err(SelError::Interrupt)
                } else if -ENOTTHDCTX == rc {
                    Err(SelError::NotThreadContext)
                } else {
                    Err(SelError::Unknown(rc))
                }
            } else {
                Err(SelError::NotInit)
            }
        }
    }

    /// 限时等待所有线程到达栅栏。
    ///
    /// + 当信号选择器中的线程数量小于指定数量，线程会阻塞等待，等待时会指定一个唤醒时间点。
    /// + 当信号选择器中的线程数量达到指定数量，线程被唤醒，然后返回 [`Bmp<N>`] 。
    /// + 当线程阻塞等待被中断时，返回 [`SelError::Interrupt`] 。
    /// + 当到达指定的唤醒时间点，线程被唤醒，并返回 [`SelError::Timedout`] 。
    ///
    /// # 上下文
    ///
    /// + 线程
    ///
    /// # 错误码
    ///
    /// + [`SelError::NotInit`] 信号量没有初始化
    /// + [`SelError::Interrupt`] 等待被中断
    /// + [`SelError::Timedout`] 等待超时
    /// + [`SelError::NotThreadContext`] 不在线程上下文内
    ///
    /// # 示例
    ///
    /// ```rust
    /// use xwrust::xwbmp::*;
    /// use xwrust::xwos::sync::sel::*;
    ///
    /// pub fn xwrust_example_sel() {
    ///     let sel = Sel::<8>::new();
    ///     sel.init();
    ///     // ...省略bind过程...
    ///     let msk = Bmp::<8>::new();
    ///     msk.s1all(); // 设置掩码
    ///     let res = sel.select_to(msk, xwtm::ft(xwtm::s(3)));
    ///     match res {
    ///         Ok(t) => { // 等待成功， `t` 为 **选择信号** 的位图
    ///         },
    ///         Err(e) => { // 等待失败， `e` 为 `SelError`
    ///         },
    ///     }
    /// }
    /// ```
    pub fn select_to(&self, msk: &Bmp<N>, to: XwTm) -> Result<Bmp<N>, SelError> {
        unsafe {
            let mut rc = xwrustffi_sel_acquire(self.sel.get() as _, *self.tik.get());
            if rc == 0 {
                let trg = Bmp::<N>::new();
                rc = xwrustffi_sel_select_to(self.sel.get() as _,
                                             msk.bmp.get() as _, trg.bmp.get() as _,
                                             to);
                xwrustffi_sel_put(self.sel.get() as _);
                if XWOK == rc {
                    Ok(trg)
                } else if -EINTR == rc {
                    Err(SelError::Interrupt)
                } else if -ETIMEDOUT == rc {
                    Err(SelError::Timedout)
                } else if -ENOTTHDCTX == rc {
                    Err(SelError::NotThreadContext)
                } else {
                    Err(SelError::Unknown(rc))
                }
            } else {
                Err(SelError::NotInit)
            }
        }
    }

    /// 检测信号选择器中是否有 **选择信号** 。
    ///
    /// + 当检测到 **任意** 同步对象向信号选择器发送 **选择信号** 时，立即返回 [`Bmp<N>`] 。
    /// + 当没有同步对象向信号选择器发送 **选择信号** 时，立即返回 [`SelError::NoSignal`] 。
    ///
    /// # 上下文
    ///
    /// + 线程
    ///
    /// # 错误码
    ///
    /// + [`SelError::NotInit`] 信号量没有初始化
    /// + [`SelError::NoSignal`] 没有检测到 **选择信号**
    ///
    /// # 示例
    ///
    /// ```rust
    /// use xwrust::xwbmp::*;
    /// use xwrust::xwos::sync::sel::*;
    ///
    /// pub fn xwrust_example_sel() {
    ///     let sel = Sel::<8>::new();
    ///     sel.init();
    ///     // ...省略bind过程...
    ///     let msk = Bmp::<8>::new();
    ///     msk.s1all(); // 设置掩码
    ///     let res = sel.tryselect(msk);
    ///     match res {
    ///         Ok(t) => { // 等待成功， `t` 为 **选择信号** 的位图
    ///         },
    ///         Err(e) => { // 等待失败， `e` 为 `SelError`
    ///         },
    ///     }
    /// }
    /// ```
    pub fn tryselect(&self, msk: &Bmp<N>) -> Result<Bmp<N>, SelError> {
        unsafe {
            let mut rc = xwrustffi_sel_acquire(self.sel.get() as _, *self.tik.get());
            if rc == 0 {
                let trg = Bmp::<N>::new();
                rc = xwrustffi_sel_tryselect(self.sel.get() as _,
                                             msk.bmp.get() as _, trg.bmp.get() as _);
                xwrustffi_sel_put(self.sel.get() as _);
                if XWOK == rc {
                    Ok(trg)
                } else if -EINTR == rc {
                    Err(SelError::Interrupt)
                } else if -ENODATA == rc {
                    Err(SelError::NoSignal)
                } else {
                    Err(SelError::Unknown(rc))
                }
            } else {
                Err(SelError::NotInit)
            }
        }
    }

    /// 绑定源信号选择器 `src` 到目的信号选择器 `dst` 。
    ///
    /// + 源信号选择器 `src` 绑定到目的信号选择器 `dst` 上时， 采用 **非独占** 的方式进行绑定。
    /// + 绑定成功，通过 [`Ok()`] 返回 [`SelSel<'a, N, M>`] 。
    /// + 如果位置已被其他 **同步对象** 以 **独占** 的方式占领，通过 [`Err()`] 返回 [`SelError::SelPosBusy`] 。
    /// + 当指定的位置超出范围（例如 `dst` 只有8个位置，用户偏偏要绑定到位置9 ），通过 [`Err()`] 返回 [`SelError::OutOfSelPos`] 。
    /// + 重复绑定，通过 [`Err()`] 返回 [`SelError::AlreadyBound`] 。
    ///
    /// [`SelSel<'a, N, M>`] 中包含绑定信息。 [`SelSel<'a, N, M>`] 与 `src` 与 `dst` 具有相同的生命周期约束 `'a` 。
    /// [`SelSel::selected()`] 可用来判断 `src` 是否被选择。当 [`SelSel<'a, N, M>`] [`drop()`] 时，会自动将 `src` 从 `dst` 解绑。
    ///
    /// # 上下文
    ///
    /// + 任意
    ///
    /// # 错误码
    ///
    /// + [`SelError::OutOfSelPos`] 信号选择器的位置超出范围
    /// + [`SelError::AlreadyBound`] 信号选择器已经绑定
    /// + [`SelError::SelPosBusy`] 信号选择器的位置被占用
    ///
    /// # 示例
    ///
    /// ```rust
    /// pub fn xwrust_example_sel() {
    ///     // ...省略...
    ///     let sel0 = Arc::new(Sel::<8>::new());
    ///     sel0.init();
    ///     let sel0sel = match sel0.bind(&sel, 0) {
    ///         Ok(s) => { // 绑定成功，`s` 为 `SelSel`
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
    /// [`SelSel<'a, N, M>`]: SelSel
    /// [`Ok()`]: <https://doc.rust-lang.org/core/result/enum.Result.html#variant.Ok>
    /// [`Err()`]: <https://doc.rust-lang.org/core/result/enum.Result.html#variant.Err>
    /// [`Sel`]: super::sel::Sel
    /// [`drop()`]: https://doc.rust-lang.org/std/mem/fn.drop.html
    pub fn bind<'a, const M: XwSz>(&'a self, dst: &'a Sel<M>, pos: XwSq) ->
                                   Result<SelSel<'a, N, M>, SelError>
    where
        [XwBmp; ((M + XwBmp::BITS as usize - 1) / XwBmp::BITS as usize)]: Sized
    {
        unsafe {
            let mut rc = xwrustffi_sel_acquire(self.sel.get() as _, *self.tik.get());
            if rc == 0 {
                rc = xwrustffi_sel_bind(self.sel.get() as _, dst.sel.get() as _, pos);
                if XWOK == rc {
                    Ok(SelSel {
                        src: self,
                        dst: dst,
                        pos: pos,
                    })
                } else if -ECHRNG == rc {
                    Err(SelError::OutOfSelPos)
                } else if -EALREADY == rc {
                    Err(SelError::AlreadyBound)
                } else if -EBUSY == rc {
                    Err(SelError::SelPosBusy)
                } else {
                    Err(SelError::Unknown(rc))
                }
            } else {
                Err(SelError::NotInit)
            }
        }
    }
}

/// 信号选择器的选择子
///
/// `SelSel<'a, N, M>` 与源信号选择器 `src` 与 目的信号选择器 `dst` 具有相同的生命周期约束 `'a` 。因为 `SelSel<'a, N, M>` 中的 `src` 与 `dst` 是引用。
///
/// `SelSel<'a, N, M>` 中包含了绑定的位置，信号选择器 采用 **非独占** 的方式进行绑定。
///
/// **选择信号** 首先从其他 **同步对象** 发送给 `src` ，再由 `src` 传递给 `dst` 。
///
/// [`SelSel::selected()`] 可用来判断信号选择器是否被选择。
///
/// 当 `SelSel<'a, N, M>` 被 [`drop()`] 时，会自动将 `src` 从 `dst` 解绑。
///
/// [`drop()`]: https://doc.rust-lang.org/std/mem/fn.drop.html
pub struct SelSel<'a, const N: XwSz, const M: XwSz>
where
    [XwBmp; ((N + XwBmp::BITS as usize - 1) / XwBmp::BITS as usize)]: Sized,
    [XwBmp; ((M + XwBmp::BITS as usize - 1) / XwBmp::BITS as usize)]: Sized
{
    src: &'a Sel<N>,
    dst: &'a Sel<M>,
    pos: XwSq,
}

unsafe impl<'a, const N: XwSz, const M: XwSz> Send for SelSel<'a, N, M>
where
    [XwBmp; ((N + XwBmp::BITS as usize - 1) / XwBmp::BITS as usize)]: Sized,
    [XwBmp; ((M + XwBmp::BITS as usize - 1) / XwBmp::BITS as usize)]: Sized
{}

unsafe impl<'a, const N: XwSz, const M: XwSz> Sync for SelSel<'a, N, M>
where
    [XwBmp; ((N + XwBmp::BITS as usize - 1) / XwBmp::BITS as usize)]: Sized,
    [XwBmp; ((M + XwBmp::BITS as usize - 1) / XwBmp::BITS as usize)]: Sized
{}

impl<'a, const N: XwSz, const M: XwSz> Drop for SelSel<'a, N, M>
where
    [XwBmp; ((N + XwBmp::BITS as usize - 1) / XwBmp::BITS as usize)]: Sized,
    [XwBmp; ((M + XwBmp::BITS as usize - 1) / XwBmp::BITS as usize)]: Sized
{
    fn drop(&mut self) {
        unsafe {
            xwrustffi_sel_unbind(self.src.sel.get() as _, self.dst.sel.get() as _);
            xwrustffi_sel_put(self.src.sel.get() as _);
        }
    }
}

impl<'a, const N: XwSz, const M: XwSz> SelSel<'a, N, M>
where
    [XwBmp; ((N + XwBmp::BITS as usize - 1) / XwBmp::BITS as usize)]: Sized,
    [XwBmp; ((M + XwBmp::BITS as usize - 1) / XwBmp::BITS as usize)]: Sized
{
    /// 判断触发的 **选择信号** 是否包括此信号选择器
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
    ///                 if sel0sel.selected(&t) { // 信号选择器0被选择到
    ///                     let msk0 = Bmp::<16>::new(); // 16位位图
    ///                     msk0.s1all(); // 掩码为0xFFFF
    ///                     let res0 = sel0.tryselect(msk0); // 继续从sel0中选择
    ///                     match res0 {
    ///                         Ok(t0) => { // 信号选择器0上有 **选择信号** ， `t0` 为 **选择信号** 的位图。
    ///                         },
    ///                         Err(e0) => { // 等待信号选择器0失败，`e` 为 `SelError`
    ///                         },
    ///                     }
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
