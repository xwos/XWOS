//! XWOS RUST：软件定时器
//! ========
//!
//! XWOS RUST的软件定时器是基于XWOS内核中的软件定时器来实现的。
//!
//! 软件定时器的 **回调函数** 运行在中断上下文或中断底半部上下文，是由硬件的中断驱动的。
//!
//! RUST的编译器无法获知硬件的运行状态，所以无法预测 **回调函数** 什么时候会执行，也无法检测软件定时器对象的生命周期。
//!
//! 如果 **回调函数** 是闭包，编译器也无法检测捕获环境中引用的生命周期。
//!
//! **回调函数** 甚至可能不执行，如果变量移动到 **回调函数** ，有可能因 **回调函数** 不执行而无法被 [`drop()`] 。
//!
//!
//! 因此，软件定时器对象只能定义为静态生命周期的全局变量，软件定时器的 **回调函数** 只能是 [`fn`] 或什么也不捕获的 [`Fn()`] 。
//!
//!
//! # 创建
//!
//! XWOS RUST的软件定时器可使用 [`Swt::new()`] 创建。只能创建具有静态生命周期 [`static`] 约束的全局变量：
//!
//! ```rust
//! use xwrust::xwos::swt::*;
//! use xwrust::xwos::lock::spinlock::*;
//!
//! // 创建一个带有自选锁的软件定时器
//! static SWT: Swt<Spinlock<u32>> = Swt::new(Spinlock::new(1));
//! ```
//!
//!
//! # 内部数据
//!
//! 软件定时器可以携带内部数据 `T` ，由于软件定时器具有静态生命周期，因此内部数据 `T` 也为静态生命周期。
//! 访问 `T` 时需要上锁，防止不同上下文之间竞争数据。但由于软件定时器的 **回调函数** 只能运行在中断上下文或中断底半部上下文，
//! 锁只能是 [`Spinlock`] 或 [`Seqlock`] 。
//!
//! ```rust
//! // 内部数据 `T` 为 `Spinlock<u32>`
//! static SWT: Swt<Spinlock<u32>> = Swt::new(Spinlock::new(1));
//! ```
//!
//!
//! # 回调函数
//!
//! 软件定时器的 **回调函数** 的形式为 `fn(&Swt)` ，其中 `&Swt` 是指向软件定时器自己的一个引用。
//! 通过此应用，可以访问定时器携带的数据。
//!
//! [`Swt::as_ref()`] 可以获取定时器内部数据 `T` 的引用。
//! [`Swt::as_mut()`] 可以获取定时器内部数据 `T` 的可变引用。
//!
//!
//! # 启动
//!
//! 软件定时器由两种启动方式：
//!
//! [`Swt::once()`] 软件定时器只会运行一次；
//! [`Swt::repeat()`] 软件定时器会重复运行。
//!
//!
//! # 停止
//!
//! [`Swt::stop()`] 可用于停止软件定时器，若软件定时器未到达超时时间，回调函数不会被调用。
//!
//!
//! [`drop()`]: <https://doc.rust-lang.org/core/ops/trait.Drop.html#tymethod.drop>
//! [`fn()`]: https://doc.rust-lang.org/std/keyword.fn.html
//! [`Fn()`]: https://doc.rust-lang.org/core/ops/trait.Fn.html
//! [`static`]: <https://doc.rust-lang.org/std/keyword.static.html>
//! [`Spinlock`]: crate::xwos::lock::spinlock::Spinlock
//! [`Seqlock`]: crate::xwos::lock::seqlock::Seqlock

extern crate core;
use core::ffi::*;
use core::option::Option;
use core::cell::UnsafeCell;
use core::convert::AsRef;
use core::convert::AsMut;

use crate::types::*;

extern "C" {
    fn xwrustffi_swt_once(swt: *mut XwosSwt,
                          origin: XwTm,
                          period: XwTm,
                          callback: extern "C" fn(*mut XwosSwt, *mut c_void),
                          arg: *mut c_void) -> XwEr;

    fn xwrustffi_swt_repeat(swt: *mut XwosSwt,
                            origin: XwTm,
                            period: XwTm,
                            callback: extern "C" fn(*mut XwosSwt, *mut c_void),
                            arg: *mut c_void) -> XwEr;

    fn xwrustffi_swt_stop(swt: *mut XwosSwt) -> XwEr;
}

/// XWOS软件定时器对象占用的内存大小
#[cfg(target_pointer_width = "32")]
pub const SIZEOF_XWOS_SWT: usize = 96;

/// XWOS软件定时器占用的内存大小
#[cfg(target_pointer_width = "64")]
pub const SIZEOF_XWOS_SWT: usize = 192;

/// 用于构建软件定时器的内存数组类型
#[repr(C)]
#[cfg_attr(target_pointer_width = "32", repr(align(8)))]
#[cfg_attr(target_pointer_width = "64", repr(align(16)))]
pub(crate) struct XwosSwt {
    pub(crate) obj: [u8; SIZEOF_XWOS_SWT],
}

/// 用于构建软件定时器的内存数组常量
///
/// 此常量的作用是告诉编译器软件定时器对象需要多大的内存。
pub(crate) const XWOS_SWT_INITIALIZER: XwosSwt = XwosSwt {
    obj: [0; SIZEOF_XWOS_SWT],
};

/// 软件定时器对象结构体
pub struct Swt<T> {
    /// 用于初始化XWOS软件定时器对象的内存空间
    pub(crate) swt: UnsafeCell<XwosSwt>,
    /// 软件定时器对象的标签
    pub(crate) tik: UnsafeCell<XwSq>,
    /// 回调函数
    pub(crate) cb: UnsafeCell<Option<fn(&Self)>>,
    /// 数据
    pub(crate) data: UnsafeCell<T>,
}

impl<T> !Send for Swt<T> {}

unsafe impl<T> Sync for Swt<T> {}

impl<T> Swt<T> {
    /// 新建软件定时器对象
    ///
    /// 此方法是编译期方法。
    ///
    /// # 参数说明
    ///
    /// + d: 定时器附带的数据
    ///
    /// # 示例
    ///
    /// 软件定时器只能作为静态生命周期 [`static`] 的全局变量：
    ///
    /// ```rust
    /// use xwrust::xwos::swt::*;
    /// use xwrust::xwos::lock::spinlock::*;
    ///
    /// // 创建一个带有自选锁的软件定时器
    /// static SWT: Swt<Spinlock<u32>> = Swt::new(Spinlock::new(1));
    /// ```
    ///
    /// [`static`]: https://doc.rust-lang.org/std/keyword.static.html
    pub const fn new(d: T) -> Self {
        Self {
            swt: UnsafeCell::new(XWOS_SWT_INITIALIZER),
            tik: UnsafeCell::new(0),
            cb: UnsafeCell::new(None),
            data: UnsafeCell::new(d),
        }
    }

    /// 启动软件定时器，并且软件定时器只运行一次
    ///
    /// 软件定时器的超时时间由 `origin` + `period` 决定。
    /// `origin` 是时间的起点， `period` 是时间的增量。
    ///
    /// 超时后，回调函数 `cb` 会被调用，且 `cb` 运行在中断上下文或中断底半部上下文。其中不可使用任何会导致阻塞睡眠的方法。
    ///
    /// 例如 [`Mutex::lock()`] , [`Sem::wait()`] , [`cthd::sleep()`] , [`println!()`] 等。
    ///
    ///
    /// `cb` 只能是函数或不捕获任何东西的闭包。
    ///
    /// # 参数说明
    ///
    /// + origin: 时间的起点
    /// + period: 时间的增量
    /// + cb: 定时器回调函数
    ///
    /// # 示例
    ///
    ///
    /// ```rust
    /// pub fn xwrust_example_swt() {
    ///     SWT.once(xwtm::now(), xwtm::ms(200), |swt| {
    ///         // 回调函数
    ///     });
    /// }
    /// ```
    ///
    /// [`Mutex::lock()`]: crate::xwos::lock::mtx::Mutex::lock
    /// [`Sem::wait()`]: crate::xwos::sync::sem::Sem::wait
    /// [`cthd::sleep()`]: crate::xwos::cthd::sleep
    /// [`println!()`]: <https://docs.rs/libc-print/latest/libc_print/macro.libc_print.html>
    pub fn once(&'static self,
                origin: XwTm, period: XwTm,
                cb: fn(&Swt<T>)) {
        unsafe {
            *self.cb.get() = Some(cb);
            let _ = xwrustffi_swt_once(self.swt.get(),
                                       origin, period,
                                       Swt::<T>::xwrustffi_swt_callback_entry,
                                       self as *const Swt<T> as _);
        }
    }

    /// 启动软件定时器，并且软件定时器可重复运行
    ///
    /// 软件定时器的超时时间由 `origin` + `period` 决定。
    /// `origin` 是时间的起点， `period` 是时间的增量。
    ///
    /// 超时后软件定时器会在超时的时间点上再次增加 `period` 重启软件定时器并执行回调函数 `cb` ，
    /// 也就是 `cb` 会以 `period` 为周期重复执行。
    ///
    ///
    /// 回调函数 `cb` 运行在中断上下文或中断底半部上下文。其中不可使用任何会导致阻塞睡眠的方法。
    ///
    /// 例如 [`Mutex::lock()`] , [`Sem::wait()`] , [`cthd::sleep()`] , [`println!()`] 等。
    ///
    ///
    /// `cb` 只能是函数或不捕获任何东西的闭包。
    ///
    /// # 参数说明
    ///
    /// + origin: 时间的起点
    /// + period: 时间的增量
    /// + cb: 定时器回调函数
    ///
    /// # 示例
    ///
    ///
    /// ```rust
    /// pub fn xwrust_example_swt() {
    ///     SWT.repeat(xwtm::now(), xwtm::ms(200), |swt| {
    ///         // 回调函数
    ///     });
    /// }
    /// ```
    ///
    /// [`Mutex::lock()`]: crate::xwos::lock::mtx::Mutex::lock
    /// [`Sem::wait()`]: crate::xwos::sync::sem::Sem::wait
    /// [`cthd::sleep()`]: crate::xwos::cthd::sleep
    /// [`println!()`]: <https://docs.rs/libc-print/latest/libc_print/macro.libc_print.html>
    pub fn repeat(&'static self,
                  origin: XwTm, period: XwTm,
                  cb: fn(&Swt<T>)) {
        unsafe {
            *self.cb.get() = Some(cb);
            let _ = xwrustffi_swt_repeat(self.swt.get(),
                                         origin, period,
                                         Swt::<T>::xwrustffi_swt_callback_entry,
                                         self as *const Swt<T> as _);
        }
    }

    extern "C" fn xwrustffi_swt_callback_entry(_: *mut XwosSwt, arg: *mut c_void) {
        unsafe {
            let swt: &Swt<T> = &*(arg as *const Swt<T>);
            let cb = (*swt.cb.get()).unwrap();
            cb(swt);
            *swt.cb.get() = Some(cb);
        }
    }

    /// 停止软件定时器
    ///
    /// 若软件定时器未到达超时时间，回调函数不会被调用。
    pub fn stop(&'static self) {
        unsafe {
            xwrustffi_swt_stop(self.swt.get());
        }
    }
}

impl<T> AsRef<T> for Swt<T> {
    fn as_ref(&self) -> &T {
        unsafe {
            &*self.data.get()
        }
    }
}

impl<T> AsMut<T> for Swt<T> {
    fn as_mut(&mut self) -> &mut T {
        unsafe {
            &mut *self.data.get()
        }
    }
}
