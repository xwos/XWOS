//! XWOS RUST：信号量
//! ========
//!
//! 信号量是操作系统比较底层的同步机制，可以同时阻塞一个或多个线程。
//!
//! 信号量中包含一个整数值，当信号量的值大于等于0时，信号量可以唤醒一个正在等待的线程。线程被唤醒时会取走一个值，信号量的值减少1。
//!
//! 任意上下文都可增加信号量的值，这个操作被称为 **发布** 。
//!
//! 信号量常常用于在中断中唤醒一个线程，并将耗时较长的操作放在线程中执行。可减少中断上下文的执行时间，增加中断吞吐量，降低中断延迟。
//!
//!
//! # 创建
//!
//! XWOS RUST的信号量可使用 [`Sem::new()`] 创建。
//!
//! + 可以创建具有静态生命周期 [`static`] 约束的全局变量：
//!
//! ```rust
//! use xwrust::xwos::sync::sem::*;
//!
//! static GLOBAL_SEM: Sem = Sem::new();
//! ```
//!
//! + 也可以使用 [`alloc::sync::Arc`] 在heap中创建：
//!
//! ```rust
//! extern crate alloc;
//! use alloc::sync::Arc;
//!
//! use xwrust::xwos::sync::sem::*;
//!
//! pub fn xwrust_example_sem() {
//!     let sem = Arc::new(Sem::new());
//! }
//! ```
//!
//!
//! # 初始化
//!
//! 无论以何种方式创建的信号量，都必须在使用前调用 [`Sem::init()`] 进行初始化：
//!
//! ```rust
//! pub fn xwrust_example_sem() {
//!     GLOBAL_SEM.init(0, XwSsq::MAX);
//!     sem.init(0, XwSsq::MAX);
//! }
//! ```
//!
//!
//! # 发布信号量
//!
//! [`Sem::post()`] 方法可用来增加信号量的值。此方法可在 **任意** 上下文使用。
//! 当信号量的值大与0时，会唤醒信号量等待队列中的一个线程。被唤醒的线程会取走一个值，使得信号量的计数器减1。
//!
//!
//! # 获取信号量
//!
//! 当信号量的值大于 **0** 时，可以直接取走一个，此时信号量的值减 **1** ；
//! 当信号量的值等于 **0** 时，获取信号量的线程就只能阻塞等待，XWOS提供四种方式：
//!
//! ## 等待并获取信号量
//!
//! [`Sem::wait()`] 方法可用于等待并获取信号量：
//!
//! + 若信号量的值小于等于0，线程会阻塞等待。
//! + 当信号量的值大于0时，线程被唤醒，并取走一个值（信号量的值减少1），然后返回 [`SemError::Ok`] 。
//! + 当线程阻塞等待被中断时，返回 [`SemError::Interrupt`] 。
//!
//! ## 限时等待并获取信号量
//!
//! [`Sem::wait_to()`] 方法可用来限时等待并获取信号量：
//!
//! + 若信号量的值小于等于0，线程会阻塞等待，等待时会指定一个唤醒时间点。
//! + 当信号量的值大于0时，线程被唤醒，并取走一个值（信号量的值减少1），然后返回 [`SemError::Ok`] 。
//! + 当线程阻塞等待被中断时，返回 [`SemError::Interrupt`] 。
//! + 当到达指定的唤醒时间点，线程被唤醒，并返回 [`SemError::Timedout`] 。
//!
//! ## 不可中断等待并获取信号量
//!
//! [`Sem::wait_unintr()`] 方法可用来不可中断等待并获取信号量：
//!
//! + 若信号量的值小于等于0，线程会阻塞等待，且不可被中断，也不会超时。
//! + 当信号量的值大于0时，线程被唤醒，并取走一个值（信号量的值减少1），然后返回 [`SemError::Ok`] 。
//!
//! ## 尝试获取信号量
//!
//! [`Sem::trywait()`] 方法可用来尝试获取信号量：
//!
//! + 当信号量的值大于0时，就取走一个值（信号量的值减少1），然后返回 [`SemError::Ok`] 。
//! + 当信号量的值小于等于0时，立即返回 [`SemError::NoData`] 。
//!
//!
//! # 冻结与解冻
//!
//! ## 冻结
//!
//! XWOS RUST的信号量可以使用方法 [`Sem::freeze()`] 进行 **冻结** 操作，被冻结的信号量的值为负数，不影响对信号量的 **等待** 操作。
//! 但不能发布信号量。
//!
//! ## 解冻
//!
//! 可以通过 [`Sem::thaw()`] 方法将已经冻结的信号量 **解冻**，信号量 **解冻** 后，值被重置为0，此时可重新开始发布信号量。
//!
//!
//! # 获取信号量的最大值
//!
//! 可以通过方法 [`Sem::get_max()`] 获取信号量的最大值。
//!
//!
//! # 获取信号量的值
//!
//! 可以通过方法 [`Sem::get_value()`] 获取信号量的值，此方法只是读取值，不会 **消费** 信号量。
//!
//!
//! # 绑定到信号选择器
//!
//! 信号量是 **同步对象** ，可以通过方法 [`Sem::bind()`] 将信号量绑定到信号选择器 [`Sel<M>`] 上，通过 [`Sel<M>`] ，单一线程可以同时等待多个不同的 **同步对象** 。
//!
//! 信号量采用 **独占** 的方式进行绑定。
//!
//!
//! # 示例
//!
//! [XWOS/xwam/xwrust-example/xwrust_example_sem](https://gitee.com/xwos/XWOS/blob/main/xwam/xwrust-example/xwrust_example_sem/src/lib.rs)
//!
//!
//! [`static`]: <https://doc.rust-lang.org/std/keyword.static.html>
//! [`alloc::sync::Arc`]: <https://doc.rust-lang.org/alloc/sync/struct.Arc.html>
//! [`Sel<M>`]: super::sel::Sel

extern crate core;
use core::ffi::*;
use core::cell::UnsafeCell;
use core::result::Result;

use crate::types::*;
use crate::errno::*;
use crate::xwbmp::*;
use crate::xwos::sync::sel::*;


extern "C" {
    fn xwrustffi_sem_init(sem: *mut XwosSem, val: XwSsq, max: XwSsq) -> XwEr;
    fn xwrustffi_sem_fini(sem: *mut XwosSem) -> XwEr;
    fn xwrustffi_sem_grab(sem: *mut XwosSem) -> XwEr;
    fn xwrustffi_sem_put(sem: *mut XwosSem) -> XwEr;
    fn xwrustffi_sem_get_tik(sem: *mut XwosSem) -> XwSq;
    fn xwrustffi_sem_acquire(sem: *mut XwosSem, tik: XwSq) -> XwEr;
    fn xwrustffi_sem_release(sem: *mut XwosSem, tik: XwSq) -> XwEr;
    fn xwrustffi_sem_bind(sem: *mut XwosSem, sel: *mut c_void, pos: XwSq) -> XwEr;
    fn xwrustffi_sem_unbind(sem: *mut XwosSem, sel: *mut c_void) -> XwEr;
    fn xwrustffi_sem_freeze(sem: *mut XwosSem) -> XwEr;
    fn xwrustffi_sem_thaw(sem: *mut XwosSem) -> XwEr;
    fn xwrustffi_sem_post(sem: *mut XwosSem) -> XwEr;
    fn xwrustffi_sem_wait(sem: *mut XwosSem) -> XwEr;
    fn xwrustffi_sem_trywait(sem: *mut XwosSem) -> XwEr;
    fn xwrustffi_sem_wait_to(sem: *mut XwosSem, to: XwTm) -> XwEr;
    fn xwrustffi_sem_wait_unintr(sem: *mut XwosSem) -> XwEr;
    fn xwrustffi_sem_get_max(sem: *mut XwosSem, max: *mut XwSsq) -> XwEr;
    fn xwrustffi_sem_get_value(sem: *mut XwosSem, val: *mut XwSsq) -> XwEr;
}

/// 信号量的错误码
#[derive(Debug)]
pub enum SemError {
    /// 没有错误
    Ok(XwEr),
    /// 信号量没有初始化
    NotInit(XwEr),
    /// 信号量已被冻结
    AlreadyFrozen(XwEr),
    /// 信号量已解冻
    AlreadyThawed(XwEr),
    /// 等待被中断
    Interrupt(XwEr),
    /// 等待超时
    Timedout(XwEr),
    /// 不在线程上下文内
    NotThreadContext(XwEr),
    /// 抢占被关闭
    DisPmpt(XwEr),
    /// 中断底半部被关闭
    DisBh(XwEr),
    /// 中断被关闭
    DisIrq(XwEr),
    /// 信号量不可用
    NoData(XwEr),
    /// 信号选择器的位置超出范围
    OutOfSelPos(XwEr),
    /// 信号量已经绑定
    AlreadyBound(XwEr),
    /// 信号选择器的位置被占用
    SelPosBusy(XwEr),
    /// 未知错误
    Unknown(XwEr),
}

impl SemError {
    /// 消费掉 `SemError` 自身，返回内部的错误码。
    pub fn unwrap(self) -> XwEr {
        match self {
            Self::Ok(rc) => rc,
            Self::NotInit(rc) => rc,
            Self::AlreadyFrozen(rc) => rc,
            Self::AlreadyThawed(rc) => rc,
            Self::Interrupt(rc) => rc,
            Self::Timedout(rc) => rc,
            Self::NotThreadContext(rc) => rc,
            Self::DisPmpt(rc) => rc,
            Self::DisBh(rc) => rc,
            Self::DisIrq(rc) => rc,
            Self::NoData(rc) => rc,
            Self::OutOfSelPos(rc) => rc,
            Self::AlreadyBound(rc) => rc,
            Self::SelPosBusy(rc) => rc,
            Self::Unknown(rc) => rc,
        }
    }

    /// 如果信号量的错误码是 [`SemError::Ok`] ，返回 `true` 。
    pub const fn is_ok(&self) -> bool {
        matches!(*self, Self::Ok(_))
    }

    /// 如果信号量的错误码不是 [`SemError::Ok`] ，返回 `true` 。
    pub const fn is_err(&self) -> bool {
        !self.is_ok()
    }
}

/// XWOS信号量对象占用的内存大小
#[cfg(target_pointer_width = "32")]
pub const SIZEOF_XWOS_SEM: usize = 64;

/// XWOS信号量对象占用的内存大小
#[cfg(target_pointer_width = "64")]
pub const SIZEOF_XWOS_SEM: usize = 128;

/// 用于构建信号量的内存数组类型
#[repr(C)]
#[cfg_attr(target_pointer_width = "32", repr(align(8)))]
#[cfg_attr(target_pointer_width = "64", repr(align(16)))]
pub(crate) struct XwosSem {
    pub(crate) obj: [u8; SIZEOF_XWOS_SEM],
}

/// 用于构建信号量的内存数组常量
///
/// 此常量的作用是告诉编译器信号量对象需要多大的内存。
pub(crate) const XWOS_SEM_INITIALIZER: XwosSem = XwosSem {
    obj: [0; SIZEOF_XWOS_SEM],
};

/// 信号量对象结构体
pub struct Sem {
    /// 用于初始化XWOS信号量对象的内存空间
    pub(crate) sem: UnsafeCell<XwosSem>,
    /// 信号量对象的标签
    pub(crate) tik: UnsafeCell<XwSq>,
}

unsafe impl Send for Sem {}
unsafe impl Sync for Sem {}

impl Drop for Sem {
    fn drop(&mut self) {
        unsafe {
            xwrustffi_sem_fini(self.sem.get());
        }
    }
}

impl Sem {
    /// 新建信号量对象
    ///
    /// 此方法是编译期方法。
    ///
    /// # 示例
    ///
    /// + 具有 [`static`] 约束的全局变量全局变量：
    ///
    /// ```rust
    /// use xwrust::xwos::sync::sem::*;
    ///
    /// static GLOBAL_SEM: Sem  = Sem::new();
    /// ```
    ///
    /// + 在heap中创建：
    ///
    /// ```rust
    /// extern crate alloc;
    /// use alloc::sync::Arc;
    ///
    /// pub fn xwrust_example_sem() {
    ///     let sem = Arc::new(Sem::new());
    /// }
    /// ```
    ///
    /// [`static`]: https://doc.rust-lang.org/std/keyword.static.html
    pub const fn new() -> Self {
        Self {
            sem: UnsafeCell::new(XWOS_SEM_INITIALIZER),
            tik: UnsafeCell::new(0),
        }
    }

    /// 初始化信号量对象
    ///
    /// 信号量对象必须调用此方法一次，方可正常使用。
    ///
    /// # 参数说明
    ///
    /// + val: 初始值
    /// + max: 最大值
    ///
    /// # 上下文
    ///
    /// + 任意
    ///
    /// # 示例
    ///
    /// ```rust
    /// use xwrust::types::*;
    /// use xwrust::xwos::sync::sem::*;
    ///
    /// static GLOBAL_SEM: Sem = Sem::new();
    ///
    /// pub fn xwrust_example_sem() {
    ///     // ...省略...
    ///     GLOBAL_SEM.init(0, XwSsq::max);
    ///     // 从此处开始 GLOBAL_SEM 可正常使用
    /// }
    /// ```
    pub fn init(&self, val: XwSsq, max: XwSsq) {
        unsafe {
            let rc = xwrustffi_sem_acquire(self.sem.get(), *self.tik.get());
            if rc == 0 {
                xwrustffi_sem_put(self.sem.get());
            } else {
                xwrustffi_sem_init(self.sem.get(), val, max);
                *self.tik.get() = xwrustffi_sem_get_tik(self.sem.get());
            }
        }
    }

    /// 冻结信号量
    ///
    /// 信号量被冻结后，可被线程等待，但被能被单播 [`Sem::post()`] 。
    ///
    /// # 上下文
    ///
    /// + 任意
    ///
    /// # 错误码
    ///
    /// + [`SemError::Ok`] 没有错误
    /// + [`SemError::NotInit`] 信号量没有初始化
    /// + [`SemError::AlreadyFrozen`] 信号量已被冻结
    ///
    /// # 示例
    ///
    /// ```rust
    /// use xwrust::types::*;
    /// use xwrust::errno::*;
    /// use xwrust::xwos::sync::sem::*;
    ///
    /// pub fn xwrust_example_sem() {
    ///     // ...省略...
    ///     let sem: Sem = Sem::new();
    ///     sem.init(0, XwSsq::max);
    ///     sem.freeze();
    /// }
    /// ```
    pub fn freeze(&self) -> SemError {
        unsafe {
            let mut rc = xwrustffi_sem_acquire(self.sem.get(), *self.tik.get());
            if rc == 0 {
                rc = xwrustffi_sem_freeze(self.sem.get());
                xwrustffi_sem_put(self.sem.get());
                if XWOK == rc {
                    SemError::Ok(rc)
                } else if -EALREADY == rc {
                    SemError::AlreadyFrozen(rc)
                } else {
                    SemError::Unknown(rc)
                }
            } else {
                SemError::NotInit(rc)
            }
        }
    }

    /// 解冻信号量
    ///
    /// 被冻结的信号量解冻后，可被单播 [`Sem::post()`] 。
    ///
    /// # 上下文
    ///
    /// + 任意
    ///
    /// # 错误码
    ///
    /// + [`SemError::Ok`] 没有错误
    /// + [`SemError::NotInit`] 信号量没有初始化
    /// + [`SemError::AlreadyThawed`] 信号量已解冻
    ///
    /// # 示例
    ///
    /// ```rust
    /// use xwrust::types::*;
    /// use xwrust::errno::*;
    /// use xwrust::xwos::sync::sem::*;
    ///
    /// pub fn xwrust_example_sem() {
    ///     // ...省略...
    ///     let sem: Sem = Sem::new();
    ///     sem.init(0, XwSsq::max);
    ///     sem.freeze(); // 冻结
    ///     // ...省略...
    ///     sem.thaw(); // 解冻
    /// }
    /// ```
    pub fn thaw(&self) -> SemError {
        unsafe {
            let mut rc = xwrustffi_sem_acquire(self.sem.get(), *self.tik.get());
            if rc == 0 {
                rc = xwrustffi_sem_thaw(self.sem.get());
                xwrustffi_sem_put(self.sem.get());
                if XWOK == rc {
                    SemError::Ok(rc)
                } else if -EALREADY == rc {
                    SemError::AlreadyThawed(rc)
                } else {
                    SemError::Unknown(rc)
                }
            } else {
                SemError::NotInit(rc)
            }
        }
    }

    /// 发布信号量
    ///
    /// # 上下文
    ///
    /// + 任意
    ///
    /// # 错误码
    ///
    /// + [`SemError::Ok`] 没有错误
    /// + [`SemError::NotInit`] 信号量没有初始化
    /// + [`SemError::AlreadyFrozen`] 信号量已被冻结
    ///
    /// # 示例
    ///
    /// ```rust
    /// use xwrust::types::*;
    /// use xwrust::errno::*;
    /// use xwrust::xwos::sync::sem::*;
    ///
    /// pub fn xwrust_example_sem() {
    ///     // ...省略...
    ///     let sem: Sem = Sem::new();
    ///     sem.init(0, XwSsq::max);
    ///     // ...省略...
    ///     sem.post();
    /// }
    /// ```
    pub fn post(&self) -> SemError {
        unsafe {
            let mut rc = xwrustffi_sem_acquire(self.sem.get(), *self.tik.get());
            if rc == 0 {
                rc = xwrustffi_sem_post(self.sem.get());
                xwrustffi_sem_put(self.sem.get());
                if XWOK == rc {
                    SemError::Ok(rc)
                } else if -ENEGATIVE == rc {
                    SemError::AlreadyFrozen(rc)
                } else {
                    SemError::Unknown(rc)
                }
            } else {
                SemError::NotInit(rc)
            }
        }
    }

    /// 获取信号量对象计数器的最大值
    ///
    /// 成功将在 [`Ok()`] 中返回信号量对象计数器的值。
    ///
    /// # 错误码
    ///
    /// + Err([`SemError::NotInit`]) 信号量没有初始化
    ///
    /// # 示例
    ///
    /// ```rust
    /// use xwrust::types::*;
    /// use xwrust::errno::*;
    /// use xwrust::xwos::sync::sem::*;
    ///
    /// pub fn xwrust_example_sem() {
    ///     // ...省略...
    ///     let sem: Sem = Sem::new();
    ///     sem.init(0, XwSsq::MAX);
    ///     // ...省略...
    ///     let res = sem.get_max();
    ///     match res {
    ///         Ok(max) => {
    ///             // 返回信号量的值
    ///         },
    ///         Err(e) => {
    ///             // 返回错误码
    ///         },
    ///     };
    /// }
    /// ```
    ///
    /// [`Ok()`]: <https://doc.rust-lang.org/core/result/enum.Result.html#variant.Ok>
    pub fn get_max(&self) -> Result<XwSsq, SemError> {
        unsafe {
            let rc = xwrustffi_sem_acquire(self.sem.get(), *self.tik.get());
            if rc == 0 {
                let mut max: XwSsq = 0;
                xwrustffi_sem_get_max(self.sem.get(), &mut max);
                xwrustffi_sem_put(self.sem.get());
                Ok(max)
            } else {
                Err(SemError::NotInit(rc))
            }
        }
    }

    /// 获取信号量对象计数器的值
    ///
    /// 成功将在 [`Ok()`] 中返回信号量对象计数器的值。
    ///
    /// # 错误码
    ///
    /// + Err([`SemError::NotInit`]) 信号量没有初始化
    ///
    /// # 示例
    ///
    /// ```rust
    /// use xwrust::types::*;
    /// use xwrust::errno::*;
    /// use xwrust::xwos::sync::sem::*;
    ///
    /// pub fn xwrust_example_sem() {
    ///     // ...省略...
    ///     let sem: Sem = Sem::new();
    ///     sem.init(0, XwSsq::MAX);
    ///     // ...省略...
    ///     let res = sem.get_value();
    ///     match res {
    ///         Ok(val) => {
    ///             // 返回信号量的值
    ///         },
    ///         Err(e) => {
    ///             // 返回错误码
    ///         },
    ///     };
    /// }
    /// ```
    ///
    /// [`Ok()`]: <https://doc.rust-lang.org/core/result/enum.Result.html#variant.Ok>
    pub fn get_value(&self) -> Result<XwSsq, SemError> {
        unsafe {
            let rc = xwrustffi_sem_acquire(self.sem.get(), *self.tik.get());
            if rc == 0 {
                let mut val: XwSsq = 0;
                xwrustffi_sem_get_value(self.sem.get(), &mut val);
                xwrustffi_sem_put(self.sem.get());
                Ok(val)
            } else {
                Err(SemError::NotInit(rc))
            }
        }
    }

    /// 等待并获取信号量
    ///
    /// + 若信号量的值小于等于0，线程会阻塞等待。
    /// + 当信号量的值大于0时，线程被唤醒，并取走一个值（信号量的值减少1），然后返回 [`SemError::Ok`] 。
    /// + 当线程阻塞等待被中断时，返回 [`SemError::Interrupt`] 。
    ///
    /// # 上下文
    ///
    /// + 线程
    ///
    /// # 错误码
    ///
    /// + [`SemError::Ok`] 没有错误
    /// + [`SemError::NotInit`] 信号量没有初始化
    /// + [`SemError::Interrupt`] 等待被中断
    /// + [`SemError::NotThreadContext`] 不在线程上下文内
    /// + [`SemError::DisPmpt`] 抢占被关闭
    /// + [`SemError::DisBh`] 中断底半部被关闭
    /// + [`SemError::DisIrq`] 中断被关闭
    ///
    /// # 示例
    ///
    /// ```rust
    /// use xwrust::types::*;
    /// use xwrust::xwos::sync::sem::*;
    ///
    /// pub fn xwrust_example_sem() {
    ///     // ...省略...
    ///     let sem: Sem = Sem::new();
    ///     sem.init(0, XwSsq::max);
    ///     // ...省略...
    ///     let rc = sem.wait();
    ///     match rc {
    ///         SemError::Ok => {
    ///             // 获取信号量
    ///         },
    ///         _ => {
    ///             // 等待信号量失败
    ///         },
    ///     };
    /// }
    /// ```
    pub fn wait(&self) -> SemError {
        unsafe {
            let mut rc = xwrustffi_sem_acquire(self.sem.get(), *self.tik.get());
            if rc == 0 {
                rc = xwrustffi_sem_wait(self.sem.get());
                xwrustffi_sem_put(self.sem.get());
                if XWOK == rc {
                    SemError::Ok(rc)
                } else if -EINTR == rc {
                    SemError::Interrupt(rc)
                } else if -ENOTTHDCTX == rc {
                    SemError::NotThreadContext(rc)
                } else if -EDISPMPT == rc {
                    SemError::DisPmpt(rc)
                } else if -EDISBH == rc {
                    SemError::DisBh(rc)
                } else if -EDISIRQ == rc {
                    SemError::DisIrq(rc)
                } else {
                    SemError::Unknown(rc)
                }
            } else {
                SemError::NotInit(rc)
            }
        }
    }

    /// 限时等待并获取信号量
    ///
    /// + 若信号量的值小于等于0，线程会阻塞等待，等待时会指定一个唤醒时间点。
    /// + 当信号量的值大于0时，线程被唤醒，并取走一个值（信号量的值减少1），然后返回 [`SemError::Ok`] 。
    /// + 当线程阻塞等待被中断时，返回 [`SemError::Interrupt`] 。
    /// + 当到达指定的唤醒时间点，线程被唤醒，并返回 [`SemError::Timedout`] 。
    /// + 如果 `to` 是过去的时间点，将直接返回 [`SemError::Timedout`] 。
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
    /// + [`SemError::Ok`] 没有错误
    /// + [`SemError::NotInit`] 信号量没有初始化
    /// + [`SemError::Interrupt`] 等待被中断
    /// + [`SemError::Timedout`] 等待超时
    /// + [`SemError::NotThreadContext`] 不在线程上下文内
    /// + [`SemError::DisPmpt`] 抢占被关闭
    /// + [`SemError::DisBh`] 中断底半部被关闭
    /// + [`SemError::DisIrq`] 中断被关闭
    ///
    /// # 示例
    ///
    /// ```rust
    /// use xwrust::types::*;
    /// use xwrust::errno::*;
    /// use xwrust::xwos::sync::sem::*;
    ///
    /// pub fn xwrust_example_sem() {
    ///     // ...省略...
    ///     let sem: Sem = Sem::new();
    ///     sem.init(0, XwSsq::max);
    ///     // ...省略...
    ///     let rc = sem.wait_to(xwtm::ft(xwtm::s(1))); // 最多等待1s
    ///     match rc {
    ///         SemError::Ok => {
    ///             // 获取信号量
    ///         },
    ///         _ => {
    ///             // 等待信号量失败
    ///         },
    ///     };
    /// }
    /// ```
    pub fn wait_to(&self, to: XwTm) -> SemError {
        unsafe {
            let mut rc = xwrustffi_sem_acquire(self.sem.get(), *self.tik.get());
            if rc == 0 {
                rc = xwrustffi_sem_wait_to(self.sem.get(), to);
                xwrustffi_sem_put(self.sem.get());
                if XWOK == rc {
                    SemError::Ok(rc)
                } else if -EINTR == rc {
                    SemError::Interrupt(rc)
                } else if -ETIMEDOUT == rc {
                    SemError::Timedout(rc)
                } else if -ENOTTHDCTX == rc {
                    SemError::NotThreadContext(rc)
                } else if -EDISPMPT == rc {
                    SemError::DisPmpt(rc)
                } else if -EDISBH == rc {
                    SemError::DisBh(rc)
                } else if -EDISIRQ == rc {
                    SemError::DisIrq(rc)
                } else {
                    SemError::Unknown(rc)
                }
            } else {
                SemError::NotInit(rc)
            }
        }
    }

    /// 等待并获取信号量，且等待不可被中断
    ///
    /// + 若信号量的值小于等于0，线程会阻塞等待，且不可被中断，也不会超时。
    /// + 当信号量的值大于0时，线程被唤醒，并取走一个值（信号量的值减少1），然后返回 [`SemError::Ok`] 。
    ///
    /// # 上下文
    ///
    /// + 线程
    ///
    /// # 错误码
    ///
    /// + [`SemError::Ok`] 没有错误
    /// + [`SemError::NotInit`] 信号量没有初始化
    /// + [`SemError::NotThreadContext`] 不在线程上下文内
    /// + [`SemError::DisPmpt`] 抢占被关闭
    /// + [`SemError::DisBh`] 中断底半部被关闭
    /// + [`SemError::DisIrq`] 中断被关闭
    ///
    /// # 示例
    ///
    /// ```rust
    /// use xwrust::types::*;
    /// use xwrust::errno::*;
    /// use xwrust::xwos::sync::sem::*;
    ///
    /// pub fn xwrust_example_sem() {
    ///     // ...省略...
    ///     let sem: Sem = Sem::new();
    ///     sem.init(0, XwSsq::max);
    ///     // ...省略...
    ///     let rc = sem.wait_unintr();
    ///     match rc {
    ///         SemError::Ok => {
    ///             // 获取信号量
    ///         },
    ///         _ => {
    ///             // 等待信号量失败
    ///         },
    ///     };
    /// }
    /// ```
    pub fn wait_unintr(&self) -> SemError {
        unsafe {
            let mut rc = xwrustffi_sem_acquire(self.sem.get(), *self.tik.get());
            if rc == 0 {
                rc = xwrustffi_sem_wait_unintr(self.sem.get());
                xwrustffi_sem_put(self.sem.get());
                if XWOK == rc {
                    SemError::Ok(rc)
                } else if -ENOTTHDCTX == rc {
                    SemError::NotThreadContext(rc)
                } else if -EDISPMPT == rc {
                    SemError::DisPmpt(rc)
                } else if -EDISBH == rc {
                    SemError::DisBh(rc)
                } else if -EDISIRQ == rc {
                    SemError::DisIrq(rc)
                } else {
                    SemError::Unknown(rc)
                }
            } else {
                SemError::NotInit(rc)
            }
        }
    }

    /// 尝试获取信号量
    ///
    /// + 当信号量的值大于0时，就取走一个值（信号量的值减少1），然后返回 [`SemError::Ok`] 。
    /// + 当信号量的值小于等于0时，立即返回 [`SemError::NoData`] 。
    ///
    /// # 上下文
    ///
    /// + 任意
    ///
    /// # 错误码
    ///
    /// + [`SemError::Ok`] 没有错误
    /// + [`SemError::NotInit`] 信号量没有初始化
    /// + [`SemError::NoData`] 信号量不可用
    ///
    /// # 示例
    ///
    /// ```rust
    /// use xwrust::types::*;
    /// use xwrust::errno::*;
    /// use xwrust::xwos::sync::sem::*;
    ///
    /// pub fn xwrust_example_sem() {
    ///     // ...省略...
    ///     let sem: Sem = Sem::new();
    ///     sem.init(0, XwSsq::max);
    ///     // ...省略...
    ///     let rc = sem.trywait();
    ///     match rc {
    ///         SemError::Ok => {
    ///             // 获取信号量
    ///         },
    ///         _ => {
    ///             // 等待信号量失败
    ///         },
    ///     };
    /// }
    /// ```
    pub fn trywait(&self) -> SemError {
        unsafe {
            let mut rc = xwrustffi_sem_acquire(self.sem.get(), *self.tik.get());
            if rc == 0 {
                rc = xwrustffi_sem_trywait(self.sem.get());
                xwrustffi_sem_put(self.sem.get());
                if XWOK == rc {
                    SemError::Ok(rc)
                } else if -ENODATA == rc {
                    SemError::NoData(rc)
                } else {
                    SemError::Unknown(rc)
                }
            } else {
                SemError::NotInit(rc)
            }
        }
    }

    /// 绑定信号量对象到信号选择器
    ///
    /// + 信号量绑定到信号选择器上时， **独占** 一个位置。
    /// + 绑定成功，通过 [`Ok()`] 返回 [`SemSel<'a, M>`] 。
    /// + 如果位置已被占领，通过 [`Err()`] 返回 [`SemError::SelPosBusy`] 。
    /// + 当指定的位置超出范围（例如 [`Sel<M>`] 只有8个位置，用户偏偏要绑定到位置9 ），通过 [`Err()`] 返回 [`SemError::OutOfSelPos`] 。
    /// + 重复绑定，通过 [`Err()`] 返回 [`SemError::AlreadyBound`] 。
    ///
    /// [`SemSel<'a, M>`] 中包含信号量的绑定信息。 [`SemSel<'a, M>`] 与 [`Sem`] 与 [`Sel<M>`] 具有相同的生命周期约束 `'a` 。
    /// [`SemSel::selected()`] 可用来判断信号量是否被选择。当 [`SemSel<'a, M>`] [`drop()`] 时，会自动解绑。
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
    /// + [`SemError::OutOfSelPos`] 信号选择器的位置超出范围
    /// + [`SemError::AlreadyBound`] 信号量已经绑定
    /// + [`SemError::SelPosBusy`] 信号选择器的位置被占用
    ///
    /// # 示例
    ///
    /// ```rust
    /// pub fn xwrust_example_sel() {
    ///     // ...省略...
    ///     let sem0 = Arc::new(Sem::new());
    ///     sem0.init(0, XwSsq::MAX);
    ///     let sem0sel = match sem0.bind(&sel, 0) {
    ///         Ok(s) => { // 绑定成功，`s` 为 `SemSel`
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
    /// [`SemSel<'a, M>`]: SemSel
    /// [`Ok()`]: <https://doc.rust-lang.org/core/result/enum.Result.html#variant.Ok>
    /// [`Err()`]: <https://doc.rust-lang.org/core/result/enum.Result.html#variant.Err>
    /// [`Sel<M>`]: super::sel::Sel
    /// [`drop()`]: https://doc.rust-lang.org/std/mem/fn.drop.html
    pub fn bind<'a, const M: XwSz>(&'a self, sel: &'a Sel<M>, pos: XwSq)
                                   -> Result<SemSel<'a, M>, SemError>
    where
        [XwBmp; (M + XwBmp::BITS as usize - 1) / XwBmp::BITS as usize]: Sized
    {
        unsafe {
            let mut rc = xwrustffi_sem_acquire(self.sem.get(), *self.tik.get());
            if rc == 0 {
                rc = xwrustffi_sem_bind(self.sem.get(), sel.sel.get() as _, pos);
                if XWOK == rc {
                    Ok(SemSel {
                        sem: self,
                        sel: sel,
                        pos: pos,
                    })
                } else if -ECHRNG == rc {
                    Err(SemError::OutOfSelPos(rc))
                } else if -EALREADY == rc {
                    Err(SemError::AlreadyBound(rc))
                } else if -EBUSY == rc {
                    Err(SemError::SelPosBusy(rc))
                } else {
                    Err(SemError::Unknown(rc))
                }
            } else {
                Err(SemError::NotInit(rc))
            }
        }
    }
}

/// 信号量的选择子
///
/// `SemSel<'a, M>` 与 [`Sem`] 与 [`Sel<M>`] 具有相同的生命周期约束 `'a` 。因为 `SemSel<'a, M>` 中包含了 [`Sem`] 与 [`Sel<M>`] 的引用。
///
/// `SemSel<'a, M>` 中包含了绑定的位置，信号量 **独占** 一个位置。
///
/// [`SemSel::selected()`] 可用来判断信号量是否被选择。
///
/// 当 `SemSel<'a, M>` 被 [`drop()`] 时，会自动将 [`Sem`] 从 [`Sel<M>`] 解绑。
///
/// [`Sel<M>`]: super::sel::Sel
/// [`drop()`]: https://doc.rust-lang.org/std/mem/fn.drop.html
pub struct SemSel<'a, const M: XwSz>
where
    [XwBmp; (M + XwBmp::BITS as usize - 1) / XwBmp::BITS as usize]: Sized
{
    sem: &'a Sem,
    sel: &'a Sel<M>,
    pos: XwSq,
}

unsafe impl<'a, const M: XwSz> Send for SemSel<'a, M>
where
    [XwBmp; (M + XwBmp::BITS as usize - 1) / XwBmp::BITS as usize]: Sized
{}

unsafe impl<'a, const M: XwSz> Sync for SemSel<'a, M>
where
    [XwBmp; (M + XwBmp::BITS as usize - 1) / XwBmp::BITS as usize]: Sized
{}

impl<'a, const M: XwSz> Drop for SemSel<'a, M>
where
    [XwBmp; (M + XwBmp::BITS as usize - 1) / XwBmp::BITS as usize]: Sized
{
    fn drop(&mut self) {
        unsafe {
            xwrustffi_sem_unbind(self.sem.sem.get(), self.sel.sel.get() as _);
            xwrustffi_sem_put(self.sem.sem.get());
        }
    }
}

impl<'a, const M: XwSz> SemSel<'a, M>
where
    [XwBmp; (M + XwBmp::BITS as usize - 1) / XwBmp::BITS as usize]: Sized
{
    /// 判断触发的 **选择信号** 是否包括此信号量
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
    ///                 if sem0sel.selected(&t) { // 信号量0被选择到
    ///                     sem0.trywait();
    ///                 }
    ///                 if sem1sel.selected(&t) { // 信号量1被选择到
    ///                     sem1.trywait();
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
