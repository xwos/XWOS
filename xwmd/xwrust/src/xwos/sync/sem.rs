//! XWOS RUST：XWOS的信号量
//! ========
//!
//!

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
    /// 此方法是编译期方法，可用于新建 [`'static`] 约束的全局变量。
    ///
    /// # 示例
    ///
    /// ```rust
    /// use xwrust::xwos::sync::sem::*;
    ///
    /// static GLOBAL_SEM: Sem  = Sem::new();
    /// ```
    ///
    /// [`'static`]: https://doc.rust-lang.org/std/keyword.static.html
    pub const fn new() -> Self {
        Self {
            sem: UnsafeCell::new(XWOS_SEM_INITIALIZER),
            tik: UnsafeCell::new(0),
        }
    }
}

impl Sem {
    /// 初始化信号量对象。
    ///
    /// 信号量对象必须调用此方法一次，方可正常使用。
    ///
    /// # 示例
    ///
    /// ```rust
    /// use xwrust::types::*;
    /// use xwrust::errno::*;
    /// use xwrust::xwos::sync::sem::*;
    ///
    /// static GLOBAL_SEM: Sem = Sem::new();
    ///
    /// pub unsafe extern "C" fn xwrust_main() {
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
    /// pub unsafe extern "C" fn xwrust_main() {
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
    /// pub unsafe extern "C" fn xwrust_main() {
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
    /// pub unsafe extern "C" fn xwrust_main() {
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
    /// pub unsafe extern "C" fn xwrust_main() {
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
    ///
    /// # 示例
    ///
    /// ```rust
    /// use xwrust::types::*;
    /// use xwrust::errno::*;
    /// use xwrust::xwos::sync::sem::*;
    ///
    /// pub unsafe extern "C" fn xwrust_main() {
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
    /// pub unsafe extern "C" fn xwrust_main() {
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
    ///
    /// # 示例
    ///
    /// ```rust
    /// use xwrust::types::*;
    /// use xwrust::errno::*;
    /// use xwrust::xwos::sync::sem::*;
    ///
    /// pub unsafe extern "C" fn xwrust_main() {
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
    ///
    /// # 示例
    ///
    /// ```rust
    /// use xwrust::types::*;
    /// use xwrust::errno::*;
    /// use xwrust::xwos::sync::sem::*;
    ///
    /// pub unsafe extern "C" fn xwrust_main() {
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