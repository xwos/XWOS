//! XWOS RUST：XWOS的条件量
//! ========
//!
//! 条件量是操作系统比较底层的同步机制，可以同时阻塞多个线程。当条件成立，条件量可以唤醒一个或所有正在等待的线程。
//!
//! XWOS RUST的条件量通常还需要一个伴生的锁一起工作。线程通常需要持有 **锁** 的情况下去等待条件量，
//! 条件量阻塞线程的同时会同步释放 **锁** 。当条件成立，线程被唤醒时，条件量会自动获得 **锁**。
//!
//! XWOS RUST的条件量机制，主要包括以下操作：
//!
//! + 线程A等待条件量的 **条件** 成立而阻塞；
//! + 另一个线程B或中断或其他任意上下文使 **条件** 成立，并唤醒条件量上阻塞的线程，可以 **单播** 也可以 **广播** 。
//!   为了防止两个上下文竞争 **条件** ，条件量需要和一个 **锁** 一起使用，锁用于保护 **条件** 不会被同时访问。
//! + XWOS的条件量的锁的类型包括：
//!   + 互斥锁
//!   + 自旋锁
//!   + 顺序锁的独占读锁
//!   + 顺序锁的写锁
//!

extern crate core;
use core::ffi::*;
use core::cell::UnsafeCell;

use crate::types::*;
use crate::errno::*;


extern "C" {
    pub(crate) fn xwrustffi_cond_init(cond: *mut XwosCond) -> XwEr;
    pub(crate) fn xwrustffi_cond_fini(cond: *mut XwosCond) -> XwEr;
    pub(crate) fn xwrustffi_cond_grab(cond: *mut XwosCond) -> XwEr;
    pub(crate) fn xwrustffi_cond_put(cond: *mut XwosCond) -> XwEr;
    pub(crate) fn xwrustffi_cond_gettik(cond: *mut XwosCond) -> XwSq;
    pub(crate) fn xwrustffi_cond_acquire(cond: *mut XwosCond, tik: XwSq) -> XwEr;
    pub(crate) fn xwrustffi_cond_release(cond: *mut XwosCond, tik: XwSq) -> XwEr;
    pub(crate) fn xwrustffi_cond_freeze(cond: *mut XwosCond) -> XwEr;
    pub(crate) fn xwrustffi_cond_thaw(cond: *mut XwosCond) -> XwEr;
    pub(crate) fn xwrustffi_cond_broadcast(cond: *mut XwosCond) -> XwEr;
    pub(crate) fn xwrustffi_cond_unicast(cond: *mut XwosCond) -> XwEr;
    pub(crate) fn xwrustffi_cond_wait(cond: *mut XwosCond,
                                      lock: *mut c_void, lktype: XwSq, lkdata: *mut c_void,
                                      lkst: *mut XwSq) -> XwEr;
    pub(crate) fn xwrustffi_cond_wait_to(cond: *mut XwosCond,
                                         lock: *mut c_void, lktype: XwSq, lkdata: *mut c_void,
                                         to: XwTm, lkst: *mut XwSq) -> XwEr;
    pub(crate) fn xwrustffi_cond_wait_unintr(cond: *mut XwosCond,
                                             lock: *mut c_void, lktype: XwSq, lkdata: *mut c_void,
                                             lkst: *mut XwSq) -> XwEr;
}

/// 条件量的错误码
#[derive(Debug)]
pub enum CondError {
    /// 没有错误
    Ok,
    /// 条件量没有初始化
    NotInit,
    /// 条件量已被冻结
    AlreadyFrozen,
    /// 条件量已解冻
    AlreadyThawed,
    /// 等待被中断
    Interrupt,
    /// 等待超时
    Timedout,
    /// 不在线程上下文内
    NotThreadContext,
    /// 未知错误
    Unknown(XwEr),
}

/// 锁类型：无
pub(crate) const XWOS_LK_NONE: XwSq = 0;
/// 锁类型：互斥锁
pub(crate) const XWOS_LK_MTX: XwSq = 1;
/// 锁类型：自旋锁
pub(crate) const XWOS_LK_SPLK: XwSq = 2;
/// 锁类型：顺序写锁
pub(crate) const XWOS_LK_SQLK_WR: XwSq = 3;
/// 锁类型：独占顺序读锁
pub(crate) const XWOS_LK_SQLK_RDEX: XwSq = 4;
/// 锁类型：抽象回调锁
pub(crate) const XWOS_LK_CALLBACK: XwSq = 5;


/// 锁状态：锁定
pub(crate) const XWOS_LKST_LOCKED: XwSq = 0;
/// 锁状态：未锁定
pub(crate) const XWOS_LKST_UNLOCKED: XwSq = 1;

/// XWOS条件量对象占用的内存大小
pub const SIZEOF_XWOS_COND: usize = 48;

xwos_struct! {
    /// 用于构建条件量的内存数组类型
    pub struct XwosCond {
        #[doc(hidden)]
        mem: [u8; SIZEOF_XWOS_COND],
    }
}

/// 用于构建条件量的内存数组常量
///
/// 此常量的作用是告诉编译器条件量对象需要多大的内存。
pub const XWOS_COND_INITIALIZER: XwosCond = XwosCond {
    mem: [0; SIZEOF_XWOS_COND],
};

/// 条件量对象结构体
pub struct Cond {
    /// 用于初始化XWOS条件量对象的内存空间
    pub(crate) cond: UnsafeCell<XwosCond>,
    /// 条件量对象的标签
    pub(crate) tik: UnsafeCell<XwSq>,
}

impl Cond {
    /// 新建条件量对象。
    ///
    /// 此方法是编译期方法，可用于新建 [`'static`] 约束的全局变量。
    ///
    /// # 示例
    ///
    /// ```rust
    /// use xwrust::xwos::sync::cond::*;
    ///
    /// static GLOBAL_COND: Cond  = Cond::new();
    /// ```
    ///
    /// [`'static`]: https://doc.rust-lang.org/std/keyword.static.html
    pub const fn new() -> Self {
        Self {
            cond: UnsafeCell::new(XWOS_COND_INITIALIZER),
            tik: UnsafeCell::new(0),
        }
    }
}

impl Cond {
    /// 初始化条件量对象。
    ///
    /// 条件量对象必须调用此方法一次，方可正常使用。
    ///
    /// # 示例
    ///
    /// ```rust
    /// use xwrust::xwos::sync::cond::*;
    ///
    /// static GLOBAL_COND: Cond = Cond::new();
    ///
    /// pub unsafe extern "C" fn xwrust_main() {
    ///     // ...省略...
    ///     GLOBAL_COND.init();
    ///     // 从此处开始 GLOBAL_COND 可正常使用
    /// }
    /// ```
    pub fn init(&self) {
        unsafe {
            let rc = xwrustffi_cond_acquire(self.cond.get(), *self.tik.get());
            if rc == 0 {
                xwrustffi_cond_put(self.cond.get());
            } else {
                xwrustffi_cond_init(self.cond.get());
                *self.tik.get() = xwrustffi_cond_gettik(self.cond.get());
            }
        }
    }

    /// 冻结条件量。
    ///
    /// 条件量被冻结后，可被线程等待，但被能被单播 [`Cond::unicast()`] 或广播 [`Cond::broadcast()`] 。
    ///
    /// # 错误码
    ///
    /// + [`CondError::Ok`] 没有错误
    /// + [`CondError::NotInit`] 条件量没有初始化
    /// + [`CondError::AlreadyFrozen`] 条件量已被冻结
    ///
    /// # 示例
    ///
    /// ```rust
    /// use xwrust::xwos::sync::cond::*;
    ///
    /// pub unsafe extern "C" fn xwrust_main() {
    ///     // ...省略...
    ///     condvar: Cond = Cond::new();
    ///     condvar.init();
    ///     condvar.freeze();
    /// }
    /// ```
    pub fn freeze(&self) -> CondError {
        unsafe {
            let mut rc = xwrustffi_cond_acquire(self.cond.get(), *self.tik.get());
            if rc == 0 {
                rc = xwrustffi_cond_freeze(self.cond.get());
                xwrustffi_cond_put(self.cond.get());
                if XWOK == rc {
                    CondError::Ok
                } else if -EALREADY == rc {
                    CondError::AlreadyFrozen
                } else {
                    CondError::Unknown(rc)
                }
            } else {
                CondError::NotInit
            }
        }
    }

    /// 解冻条件量。
    ///
    /// 被冻结的条件量解冻后，可被单播 [`Cond::unicast()`] 或广播 [`Cond::broadcast()`] 。
    ///
    /// # 错误码
    ///
    /// + [`CondError::Ok`] 没有错误
    /// + [`CondError::NotInit`] 条件量没有初始化
    /// + [`CondError::AlreadyThawed`] 条件量已解冻
    ///
    /// # 示例
    ///
    /// ```rust
    /// use xwrust::xwos::sync::cond::*;
    ///
    /// pub unsafe extern "C" fn xwrust_main() {
    ///     // ...省略...
    ///     condvar: Cond = Cond::new();
    ///     condvar.init();
    ///     condvar.freeze(); // 冻结
    ///     // ...省略...
    ///     condvar.thaw(); // 解冻
    /// }
    /// ```
    pub fn thaw(&self) -> CondError {
        unsafe {
            let mut rc = xwrustffi_cond_acquire(self.cond.get(), *self.tik.get());
            if rc == 0 {
                rc = xwrustffi_cond_thaw(self.cond.get());
                xwrustffi_cond_put(self.cond.get());
                if XWOK == rc {
                    CondError::Ok
                } else if -EALREADY == rc {
                    CondError::AlreadyThawed
                } else {
                    CondError::Unknown(rc)
                }
            } else {
                CondError::NotInit
            }
        }
    }

    /// 单播条件量对象。
    ///
    /// 只会唤醒第一个线程，阻塞线程的队列使用的是先进先出(FIFO)算法 。
    ///
    /// # 错误码
    ///
    /// + [`CondError::Ok`] 没有错误
    /// + [`CondError::NotInit`] 条件量没有初始化
    /// + [`CondError::AlreadyFrozen`] 条件量已被冻结
    ///
    /// # 示例
    ///
    /// ```rust
    /// use xwrust::xwos::sync::cond::*;
    ///
    /// pub unsafe extern "C" fn xwrust_main() {
    ///     // ...省略...
    ///     condvar: Cond = Cond::new();
    ///     condvar.init();
    ///     // ...省略...
    ///     condvar.unicast();
    /// }
    /// ```
    pub fn unicast(&self) -> CondError {
        unsafe {
            let mut rc = xwrustffi_cond_acquire(self.cond.get(), *self.tik.get());
            if rc == 0 {
                rc = xwrustffi_cond_unicast(self.cond.get());
                xwrustffi_cond_put(self.cond.get());
                if XWOK == rc {
                    CondError::Ok
                } else if -ENEGATIVE == rc {
                    CondError::AlreadyFrozen
                } else {
                    CondError::Unknown(rc)
                }
            } else {
                CondError::NotInit
            }
        }
    }

    /// 广播条件量。
    ///
    /// 阻塞队列中的线程会全部被唤醒。
    ///
    /// # 错误码
    ///
    /// + [`CondError::Ok`] 没有错误
    /// + [`CondError::NotInit`] 条件量没有初始化
    /// + [`CondError::AlreadyFrozen`] 条件量已被冻结
    ///
    /// # 示例
    ///
    /// ```rust
    /// use xwrust::xwos::sync::cond::*;
    ///
    /// pub unsafe extern "C" fn xwrust_main() {
    ///     // ...省略...
    ///     condvar: Cond = Cond::new();
    ///     condvar.init();
    ///     // ...省略...
    ///     condvar.broadcast();
    /// }
    /// ```
    pub fn broadcast(&self) -> CondError {
        unsafe {
            let mut rc = xwrustffi_cond_acquire(self.cond.get(), *self.tik.get());
            if rc == 0 {
                rc = xwrustffi_cond_broadcast(self.cond.get());
                xwrustffi_cond_put(self.cond.get());
                if XWOK == rc {
                    CondError::Ok
                } else if -ENEGATIVE == rc {
                    CondError::AlreadyFrozen
                } else {
                    CondError::Unknown(rc)
                }
            } else {
                CondError::NotInit
            }
        }
    }
}

unsafe impl Send for Cond {}
unsafe impl Sync for Cond {}
