//! XWOS RUST：XWOS的信号量
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
//! + 可以创建具有静态生命周期 [`'static`] 约束的全局变量：
//!
//! ```rust
//! use xwrust::xwos::sync::sem::*;
//!
//! static GLOBAL_SEM: Sem  = Sem::new();
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
//!     let sema = Arc::new(Sem::new());
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
//!     sema.init(0, XwSsq::MAX);
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
//! 当信号量的值大于0时，可以直接取走一个，此时信号量的值减1；
//! 当信号量的值等于0时，获取信号量的线程就只能阻塞等待，XWOS提供四种方式：
//!
//! ## 等待并获取信号量
//!
//! [`Sem::wait()`] 方法只可在 **线程** 上下文中使用：
//!
//! + 若信号量的值小于等于0，线程会阻塞等待。
//! + 当信号量的值大于0时，线程被唤醒，并取走一个值（信号量的值减少1），然后返回 [`SemError::Ok`] 。
//! + 当线程阻塞等待被中断时，返回 [`SemError::Interrupt`] 。
//!
//! ## 限时等待并获取信号量
//!
//! [`Sem::wait_to()`] 方法只可在 **线程** 上下文中使用：
//!
//! + 若信号量的值小于等于0，线程会阻塞等待，等待时会指定一个唤醒时间点。
//! + 当信号量的值大于0时，线程被唤醒，并取走一个值（信号量的值减少1），然后返回 [`SemError::Ok`] 。
//! + 当线程阻塞等待被中断时，返回 [`SemError::Interrupt`] 。
//! + 当到达指定的唤醒时间点，线程被唤醒，并返回 [`SemError::Timedout`] 。
//!
//! ## 不可中断等待并获取信号量
//!
//! [`Sem::wait_unintr()`] 方法只可在 **线程** 上下文中使用：
//!
//! + 若信号量的值小于等于0，线程会阻塞等待，且不可被中断，也不会超时。
//! + 当信号量的值大于0时，线程被唤醒，并取走一个值（信号量的值减少1），然后返回 [`SemError::Ok`] 。
//!
//! ## 尝试获取信号量
//!
//! [`Sem::trywait()`] 方法可在 **任意** 上下文中使用，不会阻塞，只会检测信号量的值：
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
//! # 获取信号量值
//!
//! 可以通过方法 [`Sem::getvalue()`] 获取信号量的值，此方法只是读取值，不会 **消费** 信号量。
//!
//!
//! # 示例
//!
//! [XWOS/xwam/xwrust-example/xwrust_example_sem](https://gitee.com/xwos/XWOS/blob/main/xwam/xwrust-example/xwrust_example_sem/src/lib.rs)
//!
//!
//! [`'static`]: <https://doc.rust-lang.org/std/keyword.static.html>
//! [`alloc::sync::Arc`]: <https://doc.rust-lang.org/alloc/sync/struct.Arc.html>

extern crate core;
use core::result::Result;
use core::cell::UnsafeCell;

use crate::types::*;
use crate::errno::*;


extern "C" {
    fn xwrustffi_sem_init(sem: *mut XwosSem, val: XwSsq, max: XwSsq) -> XwEr;
    fn xwrustffi_sem_fini(sem: *mut XwosSem) -> XwEr;
    fn xwrustffi_sem_grab(sem: *mut XwosSem) -> XwEr;
    fn xwrustffi_sem_put(sem: *mut XwosSem) -> XwEr;
    fn xwrustffi_sem_gettik(sem: *mut XwosSem) -> XwSq;
    fn xwrustffi_sem_acquire(sem: *mut XwosSem, tik: XwSq) -> XwEr;
    fn xwrustffi_sem_release(sem: *mut XwosSem, tik: XwSq) -> XwEr;
    fn xwrustffi_sem_freeze(sem: *mut XwosSem) -> XwEr;
    fn xwrustffi_sem_thaw(sem: *mut XwosSem) -> XwEr;
    fn xwrustffi_sem_post(sem: *mut XwosSem) -> XwEr;
    fn xwrustffi_sem_wait(sem: *mut XwosSem) -> XwEr;
    fn xwrustffi_sem_trywait(sem: *mut XwosSem) -> XwEr;
    fn xwrustffi_sem_wait_to(sem: *mut XwosSem, to: XwTm) -> XwEr;
    fn xwrustffi_sem_wait_unintr(sem: *mut XwosSem) -> XwEr;
    fn xwrustffi_sem_getvalue(sem: *mut XwosSem, sval: *mut XwSsq) -> XwEr;
}

/// 信号量的错误码
#[derive(Debug)]
pub enum SemError {
    /// 没有错误
    Ok,
    /// 信号量没有初始化
    NotInit,
    /// 信号量已被冻结
    AlreadyFrozen,
    /// 信号量已解冻
    AlreadyThawed,
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
    /// 信号量不可用
    NoData,
    /// 未知错误
    Unknown(XwEr),
}

/// XWOS信号量对象占用的内存大小
pub const SIZEOF_XWOS_SEM: usize = 64;

xwos_struct! {
    /// 用于构建信号量的内存数组类型
    pub struct XwosSem {
        #[doc(hidden)]
        mem: [u8; SIZEOF_XWOS_SEM],
    }
}

/// 用于构建信号量的内存数组常量
///
/// 此常量的作用是告诉编译器信号量对象需要多大的内存。
pub const XWOS_SEM_INITIALIZER: XwosSem = XwosSem {
    mem: [0; SIZEOF_XWOS_SEM],
};

/// 信号量对象结构体
pub struct Sem {
    /// 用于初始化XWOS信号量对象的内存空间
    pub(crate) sem: UnsafeCell<XwosSem>,
    /// 信号量对象的标签
    pub(crate) tik: UnsafeCell<XwSq>,
}

impl Sem {
    /// 新建信号量对象。
    ///
    /// 此方法是编译期方法。
    ///
    /// # 示例
    ///
    /// + 具有 [`'static`] 约束的全局变量全局变量：
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
    /// use alloc::sync::Arc;
    ///
    /// pub fn xwrust_example_sem() {
    ///     let sema = Arc::new(Sem::new());
    /// }
    /// ```
    ///
    /// [`'static`]: https://doc.rust-lang.org/std/keyword.static.html
    pub const fn new() -> Self {
        Self {
            sem: UnsafeCell::new(XWOS_SEM_INITIALIZER),
            tik: UnsafeCell::new(0),
        }
    }

    /// 初始化信号量对象。
    ///
    /// 信号量对象必须调用此方法一次，方可正常使用。
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
                *self.tik.get() = xwrustffi_sem_gettik(self.sem.get());
            }
        }
    }

    /// 冻结信号量。
    ///
    /// 信号量被冻结后，可被线程等待，但被能被单播 [`Sem::post()`] 。
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
    ///     sema: Sem = Sem::new();
    ///     sema.init(0, XwSsq::max);
    ///     sema.freeze();
    /// }
    /// ```
    pub fn freeze(&self) -> SemError {
        unsafe {
            let mut rc = xwrustffi_sem_acquire(self.sem.get(), *self.tik.get());
            if rc == 0 {
                rc = xwrustffi_sem_freeze(self.sem.get());
                xwrustffi_sem_put(self.sem.get());
                if XWOK == rc {
                    SemError::Ok
                } else if -EALREADY == rc {
                    SemError::AlreadyFrozen
                } else {
                    SemError::Unknown(rc)
                }
            } else {
                SemError::NotInit
            }
        }
    }

    /// 解冻信号量。
    ///
    /// 被冻结的信号量解冻后，可被单播 [`Sem::post()`] 。
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
    ///     sema: Sem = Sem::new();
    ///     sema.init(0, XwSsq::max);
    ///     sema.freeze(); // 冻结
    ///     // ...省略...
    ///     sema.thaw(); // 解冻
    /// }
    /// ```
    pub fn thaw(&self) -> SemError {
        unsafe {
            let mut rc = xwrustffi_sem_acquire(self.sem.get(), *self.tik.get());
            if rc == 0 {
                rc = xwrustffi_sem_thaw(self.sem.get());
                xwrustffi_sem_put(self.sem.get());
                if XWOK == rc {
                    SemError::Ok
                } else if -EALREADY == rc {
                    SemError::AlreadyThawed
                } else {
                    SemError::Unknown(rc)
                }
            } else {
                SemError::NotInit
            }
        }
    }

    /// 发布信号量。
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
    ///     sem: Sem = Sem::new();
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
                    SemError::Ok
                } else if -ENEGATIVE == rc {
                    SemError::AlreadyFrozen
                } else {
                    SemError::Unknown(rc)
                }
            } else {
                SemError::NotInit
            }
        }
    }

    /// 获取信号量对象计数器的值。
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
    ///     sem: Sem = Sem::new();
    ///     sem.init(0, XwSsq::MAX);
    ///     // ...省略...
    ///     let res = sem.getvalue();
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
    pub fn getvalue(&self) -> Result<XwSsq, SemError> {
        unsafe {
            let rc = xwrustffi_sem_acquire(self.sem.get(), *self.tik.get());
            if rc == 0 {
                let mut val: XwSsq = 0;
                xwrustffi_sem_getvalue(self.sem.get(), &mut val);
                xwrustffi_sem_put(self.sem.get());
                Ok(val)
            } else {
                Err(SemError::NotInit)
            }
        }
    }

    /// 等待并获取信号。
    ///
    /// 若信号量不可用，调用的线程会阻塞等待，且可被中断。
    ///
    /// # 错误码
    ///
    /// + [`SemError::Ok`] 没有错误
    /// + [`SemError::NotInit`] 信号量没有初始化
    /// + [`SemError::Interrupt`] 等待被中断
    /// + [`SemError::NotThreadContext`] 不在线程上下文内
    /// + [`SemError::CannotPmpt`] 抢占被关闭
    /// + [`SemError::CannotBh`] 中断底半部被关闭
    ///
    /// # 示例
    ///
    /// ```rust
    /// use xwrust::types::*;
    /// use xwrust::xwos::sync::sem::*;
    ///
    /// pub fn xwrust_example_sem() {
    ///     // ...省略...
    ///     sem: Sem = Sem::new();
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
                    SemError::Ok
                } else if -EINTR == rc {
                    SemError::Interrupt
                } else if -ENOTTHDCTX == rc {
                    SemError::NotThreadContext
                } else if -ECANNOTPMPT == rc {
                    SemError::CannotPmpt
                } else if -ECANNOTBH == rc {
                    SemError::CannotBh
                } else {
                    SemError::Unknown(rc)
                }
            } else {
                SemError::NotInit
            }
        }
    }

    /// 检查信号量对象。
    ///
    /// 若没有检测到信号，立即返回，不会阻塞调用线程。
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
    ///     sem: Sem = Sem::new();
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
                    SemError::Ok
                } else if -ENODATA == rc {
                    SemError::NoData
                } else {
                    SemError::Unknown(rc)
                }
            } else {
                SemError::NotInit
            }
        }
    }

    /// 限时等待并获取信号量。
    ///
    /// 若信号量不可用，调用的线程会阻塞等待，且可被中断，超时亦会返回。
    ///
    /// # 错误码
    ///
    /// + [`SemError::Ok`] 没有错误
    /// + [`SemError::NotInit`] 信号量没有初始化
    /// + [`SemError::Interrupt`] 等待被中断
    /// + [`SemError::Timedout`] 等待超时
    /// + [`SemError::NotThreadContext`] 不在线程上下文内
    /// + [`SemError::CannotPmpt`] 抢占被关闭
    /// + [`SemError::CannotBh`] 中断底半部被关闭
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
    ///     sem: Sem = Sem::new();
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
                    SemError::Ok
                } else if -EINTR == rc {
                    SemError::Interrupt
                } else if -ETIMEDOUT == rc {
                    SemError::Timedout
                } else if -ENOTTHDCTX == rc {
                    SemError::NotThreadContext
                } else if -ECANNOTPMPT == rc {
                    SemError::CannotPmpt
                } else if -ECANNOTBH == rc {
                    SemError::CannotBh
                } else {
                    SemError::Unknown(rc)
                }
            } else {
                SemError::NotInit
            }
        }
    }

    /// 等待并获取信号量，且不可被中断。
    ///
    /// 若信号量不可用，调用的线程会阻塞等待，且不可被中断。
    ///
    /// # 错误码
    ///
    /// + [`SemError::Ok`] 没有错误
    /// + [`SemError::NotInit`] 信号量没有初始化
    /// + [`SemError::NotThreadContext`] 不在线程上下文内
    /// + [`SemError::CannotPmpt`] 抢占被关闭
    /// + [`SemError::CannotBh`] 中断底半部被关闭
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
    ///     sem: Sem = Sem::new();
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
                    SemError::Ok
                } else if -ENOTTHDCTX == rc {
                    SemError::NotThreadContext
                } else if -ECANNOTPMPT == rc {
                    SemError::CannotPmpt
                } else if -ECANNOTBH == rc {
                    SemError::CannotBh
                } else {
                    SemError::Unknown(rc)
                }
            } else {
                SemError::NotInit
            }
        }
    }
}

unsafe impl Send for Sem {}
unsafe impl Sync for Sem {}
