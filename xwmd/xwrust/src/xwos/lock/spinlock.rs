//! XWOS RUST：自旋锁
//! ========
//!
//! 自旋锁是多核系统中为防止多个处理器同时访问临界区而引入的一种锁。
//! 当一个CPU获得自旋锁并访问临界区时，其他CPU只能 **自旋** 等待。
//! 所谓 **自旋** ，是指不断循环测试锁的是否可用。
//!
//! 自旋锁内的操作是不可被打断的。因此，自旋锁还伴随其他操作，
//! 例如关闭调度器的抢占，关闭中断底半部，关闭中断等。
//!
//! 在单核(UP)系统中，只有一个CPU，不需要自旋过程，单核系统为了软件接口与多核系统兼容，
//! 也会实现自旋锁，这种自旋锁只需关闭抢占、中断底半部或中断。
//!
//! 自旋锁可用在 **任何上下文（Context）** ：
//!
//! + 当临界区只被多个线程访问时，需使用关闭抢占的模式： [`SpinlockMode::Lock`]
//! + 当临界区被中断底半部访问时，需使用关闭中断底半部的模式： [`SpinlockMode::LockBh`]
//! + 当临界区被多个线程和单一中断访问时，需使用关闭全局中断的模式： [`SpinlockMode::LockCpuirq`]
//! + 当临界区被多个中断上下文访问时，需使用保存中断全局标志并关闭全局中断的模式： [`SpinlockMode::LockCpuirqSave(None)`]
//!
//!
//! # 创建
//!
//! XWOS RUST的自旋锁可使用 [`Spinlock::new()`] 创建：
//!
//! + 可以创建具有静态生命周期 [`static`] 约束的全局变量：
//!
//! ```rust
//! use xwrust::xwos::lock::spinlock::*;
//!
//! static GLOBAL_SPINLOCK: Spinlock<u32> = Spinlock::new(0);
//! ```
//!
//! + 也可以使用 [`alloc::sync::Arc`] 在heap中创建：
//!
//! ```rust
//! extern crate alloc;
//! use alloc::sync::Arc;
//!
//! use xwrust::xwos::lock::spinlock::*;
//!
//! pub fn xwrust_example_spinlock() {
//!     let spinlock: Arc<Spinlock<u32>> = Arc::new(Spinlock::new(0));
//! }
//! ```
//!
//!
//! # 解锁
//!
//! 上锁后返回的 [`SpinlockGuard`] 。 [`SpinlockGuard`] 的生命周期结束时，会自动解锁。
//! 也可调用 [`Spinlock::unlock()`] 主动消费掉 [`SpinlockGuard`] 来解锁。
//!
//!
//! # 上锁
//!
//! 上锁的方法 [`Spinlock::lock()`] 除了 **自旋** 等待，还会根据上锁模式增加额外操作：
//!
//! + 只关闭抢占的模式： [`SpinlockMode::Lock`]
//! + 关闭中断底半部的模式： [`SpinlockMode::LockBh`]
//! + 关闭全局中断的模式： [`SpinlockMode::LockCpuirq`]
//! + 保存中断全局标志并关闭全局中断的模式： [`SpinlockMode::LockCpuirqSave(None)`]
//!
//!
//! # 尝试上锁
//!
//! 尝试上锁的方法 [`Spinlock::trylock()`] 只会测试一下锁，不会 **自旋** 等待，上锁成功后还会根据上锁模式增加额外操作：
//!
//! + 只关闭抢占的模式： [`SpinlockMode::Lock`]
//! + 关闭中断底半部的模式： [`SpinlockMode::LockBh`]
//! + 关闭全局中断的模式： [`SpinlockMode::LockCpuirq`]
//! + 保存中断全局标志并关闭全局中断的模式： [`SpinlockMode::LockCpuirqSave(None)`]
//!
//!
//! # 示例
//!
//! [XWOS/xwam/xwrust-example/xwrust_example_spinlock](https://gitee.com/xwos/XWOS/blob/main/xwam/xwrust-example/xwrust_example_spinlock/src/lib.rs)
//!
//!
//! [`SpinlockMode::LockCpuirqSave(None)`]: SpinlockMode::LockCpuirqSave
//! [`static`]: https://doc.rust-lang.org/std/keyword.static.html
//! [`alloc::sync::Arc`]: <https://doc.rust-lang.org/alloc/sync/struct.Arc.html>

extern crate core;
use core::cell::UnsafeCell;
use core::result::Result;
use core::default::Default;
use core::option::Option;
use core::ops::Drop;
use core::ops::Deref;
use core::ops::DerefMut;
use core::ptr;

use crate::types::*;
use crate::errno::*;

use crate::xwos::sync::cond::*;

extern "C" {
    fn xwrustffi_splk_init(splk: *mut XwosSplk);
    fn xwrustffi_splk_unlock(splk: *mut XwosSplk);
    fn xwrustffi_splk_lock(splk: *mut XwosSplk);
    fn xwrustffi_splk_trylock(splk: *mut XwosSplk) -> XwEr;
    fn xwrustffi_splk_unlock_cpuirq(splk: *mut XwosSplk);
    fn xwrustffi_splk_lock_cpuirq(splk: *mut XwosSplk);
    fn xwrustffi_splk_trylock_cpuirq(splk: *mut XwosSplk) -> XwEr;
    fn xwrustffi_splk_unlock_cpuirqrs(splk: *mut XwosSplk, cpuirq: XwReg);
    fn xwrustffi_splk_lock_cpuirqsv(splk: *mut XwosSplk, cpuirq: *mut XwReg);
    fn xwrustffi_splk_trylock_cpuirqsv(splk: *mut XwosSplk, cpuirq: *mut XwReg) -> XwEr;
    fn xwrustffi_splk_unlock_bh(splk: *mut XwosSplk);
    fn xwrustffi_splk_lock_bh(splk: *mut XwosSplk);
    fn xwrustffi_splk_trylock_bh(splk: *mut XwosSplk) -> XwEr;
}

/// 自旋锁的错误码
#[derive(Debug)]
pub enum SpinlockError {
    /// 尝试上锁失败
    Again(XwEr),
    /// 未知错误
    Unknown(XwEr),
}

impl SpinlockError {
    /// 消费掉 `SpinlockError` 自身，返回内部的错误码。
    pub fn unwrap(self) -> XwEr {
        match self {
            Self::Again(rc) => rc,
            Self::Unknown(rc) => rc,
        }
    }
}

/// 自旋锁的上锁模式
pub enum SpinlockMode {
    /// 关闭抢占
    Lock,
    /// 关闭抢占、中断底半部
    LockBh,
    /// 关闭抢占、中断底半部和中断
    LockCpuirq,
    /// 关闭抢占、中断底半部和中断，并保存之前的中断标志
    LockCpuirqSave(Option<XwReg>),
}

/// XWOS自旋锁占用的内存大小
#[cfg(target_pointer_width = "32")]
pub const SIZEOF_XWOS_SPLK: usize = 8;

/// XWOS自旋锁占用的内存大小
#[cfg(target_pointer_width = "64")]
pub const SIZEOF_XWOS_SPLK: usize = 8;

/// 用于构建自旋锁的内存数组类型
#[repr(C)]
#[cfg_attr(target_pointer_width = "32", repr(align(8)))]
#[cfg_attr(target_pointer_width = "64", repr(align(16)))]
pub(crate) struct XwosSplk {
    pub(crate) obj: [u8; SIZEOF_XWOS_SPLK],
}

/// 用于构建自旋锁的内存数组常量
pub(crate) const XWOS_SPLK_INITIALIZER: XwosSplk = XwosSplk {
    obj: [0; SIZEOF_XWOS_SPLK],
};


/// 自旋锁结构体
pub struct Spinlock<T: ?Sized> {
    /// XWOS自旋锁
    pub(crate) splk: UnsafeCell<XwosSplk>,
    /// 上锁方式
    pub(crate) mode: UnsafeCell<SpinlockMode>,
    /// 用户数据
    pub data: UnsafeCell<T>,
}

impl<T> Spinlock<T> {
    /// 新建自旋锁
    ///
    /// 此方法是编译期方法，可用于新建 [`static`] 约束的全局变量。
    ///
    /// # 示例
    ///
    /// ```rust
    /// use xwrust::xwos::lock::spinlock::*;
    ///
    /// static GLOBAL_SPINLOCK: Spinlock<u32>  = Spinlock::new(0);
    /// ```
    ///
    /// [`static`]: https://doc.rust-lang.org/std/keyword.static.html
    pub const fn new(t: T) -> Self {
        Self {
            splk: UnsafeCell::new(XWOS_SPLK_INITIALIZER),
            mode: UnsafeCell::new(SpinlockMode::Lock),
            data: UnsafeCell::new(t),
        }
    }
}

impl<T: ?Sized> Spinlock<T> {
    /// 获取自旋锁，若线程无法获取自旋锁，就阻塞等待，直到能获得锁为止
    ///
    /// + 若成功获取自旋锁，将返回 **RAII Guard** ： [`SpinlockGuard`] ，用于提供 **Scoped Lock** 机制。
    ///   + [`SpinlockGuard`] 中包含 [`Spinlock`] 的引用， 当 [`SpinlockGuard`] 生命周期结束时，会在 [`drop()`] 方法中自动解锁自旋锁。
    /// + 若失败，将返回错误码 [`SpinlockError`] 。
    ///
    /// # 参数说明
    ///
    /// + mode: 锁模式
    ///   + [`SpinlockMode::Lock`] 关闭抢占
    ///   + [`SpinlockMode::LockBh`] 关闭抢占、中断底半部
    ///   + [`SpinlockMode::LockCpuirq`] 关闭抢占、中断底半部和中断
    ///   + [`SpinlockMode::LockCpuirqSave(None)`] 关闭抢占、中断底半部和中断，并保存之前的中断标志
    ///
    /// # 示例
    ///
    /// ```rust
    /// use xwrust::xwos::lock::spinlock::*;
    /// static GLOBAL_SPINLOCK: Spinlock<u32> = Spinlock::new(0);
    ///
    /// pub fn xwrust_example_spinlock() {
    ///     // ...省略...
    ///     let mut guard = GLOBAL_SPINLOCK.lock();
    ///     *guard = 1; // 访问共享变量
    ///     drop(guard); // 解锁
    /// }
    /// ```
    ///
    /// [`SpinlockMode::LockCpuirqSave(None)`]: SpinlockMode::LockCpuirqSave
    /// [`drop()`]: https://doc.rust-lang.org/std/mem/fn.drop.html
    pub fn lock(&self, mode: SpinlockMode) -> SpinlockGuard<'_, T> {
        unsafe {
            match mode {
                SpinlockMode::Lock => {
                    xwrustffi_splk_lock(self.splk.get());
                    *self.mode.get() = SpinlockMode::Lock;
                },
                SpinlockMode::LockCpuirq => {
                    xwrustffi_splk_lock_cpuirq(self.splk.get());
                    *self.mode.get() = SpinlockMode::LockCpuirq;
                },
                SpinlockMode::LockCpuirqSave(_) => {
                    let mut cpuirq: XwReg = 0;
                    xwrustffi_splk_lock_cpuirqsv(self.splk.get(), &mut cpuirq);
                    *self.mode.get() = SpinlockMode::LockCpuirqSave(Some(cpuirq));
                },
                SpinlockMode::LockBh => {
                    xwrustffi_splk_lock_bh(self.splk.get());
                    *self.mode.get() = SpinlockMode::LockBh;
                },
            }
            SpinlockGuard::new(self)
        }
    }

    /// 尝试获取自旋锁，若线程无法获取自旋锁，立即返回错误
    ///
    /// + 若成功获取自旋锁，将返回 **RAII Guard** ： [`SpinlockGuard`] ，用于提供 **Scoped Lock** 机制。
    ///   + [`SpinlockGuard`] 中包含 [`Spinlock`] 的引用， 当 [`SpinlockGuard`] 生命周期结束时，会在 [`drop()`] 方法中自动解锁自旋锁。
    /// + 若失败，将返回错误码 [`SpinlockError`] 。
    ///
    /// # 参数说明
    ///
    /// + mode: 锁模式
    ///   + [`SpinlockMode::Lock`] 关闭抢占
    ///   + [`SpinlockMode::LockBh`] 关闭抢占、中断底半部
    ///   + [`SpinlockMode::LockCpuirq`] 关闭抢占、中断底半部和中断
    ///   + [`SpinlockMode::LockCpuirqSave(None)`] 关闭抢占、中断底半部和中断，并保存之前的中断标志
    ///
    /// # 错误码
    ///
    /// + [`SpinlockError::Again`] 尝试获取锁失败
    ///
    /// # 示例
    /// ```rust
    /// use xwrust::xwos::lock::spinlock::*;
    /// static GLOBAL_SPINLOCK: Spinlock<u32> = Spinlock::new(0);
    ///
    /// pub fn xwrust_example_spinlock() {
    ///     // ...省略...
    ///     match GLOBAL_SPINLOCK.trylock() {
    ///         Ok(mut guard) => { // 上锁成功
    ///             *guard = 1; // 访问共享变量
    ///         } // guard 生命周期结束，自动解锁
    ///         Err(e) => {
    ///             // 上锁失败
    ///         }
    ///     }
    /// }
    /// ```
    ///
    /// [`SpinlockMode::LockCpuirqSave(None)`]: SpinlockMode::LockCpuirqSave
    /// [`drop()`]: https://doc.rust-lang.org/std/mem/fn.drop.html
    pub fn trylock(&self, mode: SpinlockMode) -> Result<SpinlockGuard<'_, T>, SpinlockError> {
        unsafe {
            let rc: XwEr;
            match mode {
                SpinlockMode::Lock => {
                    rc = xwrustffi_splk_trylock(self.splk.get());
                    if 0 == rc {
                        *self.mode.get() = SpinlockMode::Lock;
                        Ok(SpinlockGuard::new(self))
                    } else if -EAGAIN == rc {
                        Err(SpinlockError::Again(rc))
                    } else {
                        Err(SpinlockError::Unknown(rc))
                    }
                },
                SpinlockMode::LockCpuirq => {
                    rc = xwrustffi_splk_trylock_cpuirq(self.splk.get());
                    if 0 == rc {
                        *self.mode.get() = SpinlockMode::LockCpuirq;
                        Ok(SpinlockGuard::new(self))
                    } else if -EAGAIN == rc {
                        Err(SpinlockError::Again(rc))
                    } else {
                        Err(SpinlockError::Unknown(rc))
                    }
                },
                SpinlockMode::LockCpuirqSave(_) => {
                    let mut cpuirq: XwReg = 0;
                    rc = xwrustffi_splk_trylock_cpuirqsv(self.splk.get(), &mut cpuirq);
                    if 0 == rc {
                        *self.mode.get() = SpinlockMode::LockCpuirqSave(Some(cpuirq));
                        Ok(SpinlockGuard::new(self))
                    } else if -EAGAIN == rc {
                        Err(SpinlockError::Again(rc))
                    } else {
                        Err(SpinlockError::Unknown(rc))
                    }
                },
                SpinlockMode::LockBh => {
                    rc = xwrustffi_splk_trylock_bh(self.splk.get());
                    if 0 == rc {
                        *self.mode.get() = SpinlockMode::LockBh;
                        Ok(SpinlockGuard::new(self))
                    } else if -EAGAIN == rc {
                        Err(SpinlockError::Again(rc))
                    } else {
                        Err(SpinlockError::Unknown(rc))
                    }
                },
            }
        }
    }

    /// 解锁自旋锁，并释放 [`SpinlockGuard`]
    ///
    /// # 示例
    ///
    /// ```rust
    /// use xwrust::xwos::lock::spinlock::*;
    /// static GLOBAL_SPINLOCK: Spinlock<u32> = Spinlock::new(0);
    ///
    /// pub fn xwrust_example_spinlock() {
    ///     // ...省略...
    ///     let mut guard = GLOBAL_SPINLOCK.lock();
    ///     *guard = 1; // 访问共享变量
    ///     Spinlock::unlock(guard); // 主动解锁
    /// }
    /// ```
    ///
    /// [`drop()`]: https://doc.rust-lang.org/std/mem/fn.drop.html
    pub fn unlock(guard: SpinlockGuard<'_, T>) {
        drop(guard)
    }
}

unsafe impl<T: ?Sized + Send> Send for Spinlock<T> {}
unsafe impl<T: ?Sized + Send> Sync for Spinlock<T> {}

impl<T> From<T> for Spinlock<T> {
    /// 从数据新建自旋锁
    ///
    /// 此方法会将数据所有权转移到自旋锁的内部
    ///
    /// 等价于 [`Spinlock::new`]
    fn from(t: T) -> Self {
        Spinlock::new(t)
    }
}

impl<T: ?Sized + Default> Default for Spinlock<T> {
    fn default() -> Spinlock<T> {
        Spinlock::new(Default::default())
    }
}

impl<T: ?Sized> Drop for Spinlock<T> {
    fn drop(&mut self) {
    }
}

/// 自旋锁的RAII Guard
///
/// **RAII Guard** 用于提供 **Scoped Lock** 机制。
///
/// + [`SpinlockGuard`] 中包含 [`Spinlock`] 的引用， 当 [`SpinlockGuard`] 生命周期结束时，会在 [`drop()`] 方法中自动解锁自旋锁。
/// + [`SpinlockGuard`] 不可在线程之间转移所有权，因为其 [`drop()`] 方法包含解锁的语义，上锁和解锁必须在同一线程；
/// + [`SpinlockGuard`] 虽然可以在多线程中传递引用( [`Sync`] 约束)，但其实现中没有 **unlock()** 方法，意味着其他线程即便拿到引用也不能解锁。
///
/// [`drop()`]: https://doc.rust-lang.org/std/mem/fn.drop.html
/// [`Sync`]: https://doc.rust-lang.org/std/marker/trait.Send.html
pub struct SpinlockGuard<'a, T: ?Sized + 'a> {
    /// 自旋锁的引用
    lock: &'a Spinlock<T>,
}

impl<T: ?Sized> !Send for SpinlockGuard<'_, T> {}
unsafe impl<T: ?Sized + Sync> Sync for SpinlockGuard<'_, T> {}

impl<'a, T: ?Sized> SpinlockGuard<'a, T> {
    fn new(lock: &'a Spinlock<T>) -> SpinlockGuard<'a, T> {
        SpinlockGuard { lock: lock }
    }

    fn lock(&self) {
        unsafe {
            xwrustffi_splk_lock(self.lock.splk.get());
        }
    }

    fn unlock(&self) {
        unsafe {
            xwrustffi_splk_unlock(self.lock.splk.get());
        }
    }

    /// 阻塞当前线程，直到被条件量唤醒
    ///
    /// 此方法会消费自旋锁的守卫(Guard)，并当线程阻塞时，在条件量内部释放自旋锁。
    /// 当条件成立，线程被唤醒，会在条件量内部上锁自旋锁，并重新返回自旋锁的守卫(Guard)。
    ///
    /// + 当返回自旋锁的守卫 [`SpinlockGuard`] 时，自旋锁已经被重新上锁；
    /// + 当返回 [`Err`] 时，自旋锁未被上锁。
    ///
    /// # 参数说明
    ///
    /// + cond: 条件量的引用
    ///
    /// # 错误码
    ///
    /// + [`CondError::NotInit`] 条件量未被初始化
    /// + [`CondError::Interrupt`] 等待被中断
    /// + [`CondError::NotThreadContext`] 不在线程上下文中
    ///
    /// # 示例
    ///
    /// ```rust
    /// use xwrust::xwos::thd;
    /// use xwrust::xwos::lock::spinlock::*;
    /// use xwrust::xwos::sync::cond::*;
    /// extern crate alloc;
    /// use alloc::sync::Arc;
    ///
    /// pub fn xwrust_example_spinlock() {
    ///     let pair = Arc::new((Spinlock::new(true), Cond::new()));
    ///     pair.1.init();
    ///     let pair_c = pair.clone();
    ///
    ///     thd::Builder::new()
    ///         .name("child".into())
    ///         .spawn(move |_| { // 子线程闭包
    ///             cthd::sleep(xwtm::ms(500));
    ///             let (lock, cvar) = &*pair_c;
    ///             let mut guard = lock_child.lock();
    ///             *guard = false; // 设置共享数据
    ///             drop(guard); // 先解锁再触发条件可提高效率
    ///             cvar.broadcast();
    ///         });
    ///     let (lock, cvar) = &*pair;
    ///     {
    ///         let mut guard = lock.lock();
    ///         while *guard {
    ///             match guard.wait(cvar) {
    ///                 Ok(g) => { // 唤醒
    ///                     guard = g; // 重新捕获守卫
    ///                 },
    ///                 Err(e) => { // 等待条件量失败
    ///                     break;
    ///                 },
    ///             }
    ///         }
    ///     }
    /// }
    /// ```
    ///
    /// [`Err`]: <https://doc.rust-lang.org/core/result/enum.Result.html#variant.Err>
    pub fn wait(self, cond: &Cond) -> Result<SpinlockGuard<'a, T>, CondError> {
        unsafe {
            let mut rc = xwrustffi_cond_acquire(cond.cond.get(), *cond.tik.get());
            if rc == 0 {
                let mut lkst = 0;
                rc = xwrustffi_cond_wait(cond.cond.get(),
                                         self.lock.splk.get() as _, XWOS_LK_SPLK, ptr::null_mut(),
                                         &mut lkst);
                xwrustffi_cond_put(cond.cond.get());
                if 0 == rc {
                    Ok(self)
                } else {
                    if XWOS_LKST_LOCKED == lkst {
                        drop(self);
                    } else {
                        self.lock();
                        drop(self);
                    }
                    if -EINTR == rc {
                        Err(CondError::Interrupt(rc))
                    } else if -ENOTTHDCTX == rc {
                        Err(CondError::NotThreadContext(rc))
                    } else {
                        Err(CondError::Unknown(rc))
                    }
                }
            } else {
                drop(self);
                Err(CondError::NotInit(rc))
            }
        }
    }

    /// 限时阻塞当前线程，直到被条件量唤醒
    ///
    /// 此方法会消费自旋锁的守卫(Guard)，并当线程阻塞时，在条件量内部释放自旋锁。
    /// 当条件成立，线程被唤醒，会在条件量内部上锁自旋锁，并重新返回自旋锁的守卫(Guard)。
    /// 当超时后，将返回错误。
    ///
    /// + 当返回自旋锁的守卫 [`SpinlockGuard`] 时，自旋锁已经被重新上锁；
    /// + 当返回 [`Err`] 时，自旋锁未被上锁。
    ///
    /// # 参数说明
    ///
    /// + cond: 条件量的引用
    /// + to: 期望唤醒的时间点
    ///
    /// # 错误码
    ///
    /// + [`CondError::NotInit`] 条件量未被初始化
    /// + [`CondError::Interrupt`] 等待被中断
    /// + [`CondError::Timedout`] 等待超时
    /// + [`CondError::NotThreadContext`] 不在线程上下文中
    ///
    /// # 示例
    ///
    /// ```rust
    /// use xwrust::xwos::thd;
    /// use xwrust::xwos::lock::spinlock::*;
    /// use xwrust::xwos::sync::cond::*;
    /// extern crate alloc;
    /// use alloc::sync::Arc;
    ///
    /// pub fn xwrust_example_spinlock() {
    ///     let pair = Arc::new((Spinlock::new(true), Cond::new()));
    ///     pair.1.init();
    ///     let pair_c = pair.clone();
    ///
    ///     thd::Builder::new()
    ///         .name("child".into())
    ///         .spawn(move |_| { // 子线程闭包
    ///             cthd::sleep(xwtm::ms(500));
    ///             let (lock, cvar) = &*pair_c;
    ///             let mut guard = lock_child.lock();
    ///             *guard = false; // 设置共享数据
    ///             drop(guard); // 先解锁再触发条件可提高效率
    ///             cvar.broadcast();
    ///         });
    ///     let (lock, cvar) = &*pair;
    ///     {
    ///         let mut guard = lock.lock();
    ///         while *guard {
    ///             match guard.wait_to(cvar, xwtm::ft(xwtm::s(2))) {
    ///                 Ok(g) => { // 唤醒
    ///                     guard = g; // 重新捕获守卫
    ///                 },
    ///                 Err(e) => { // 等待条件量失败
    ///                     break;
    ///                 },
    ///             }
    ///         }
    ///     }
    /// }
    /// ```
    ///
    /// [`Err`]: <https://doc.rust-lang.org/core/result/enum.Result.html#variant.Err>
    pub fn wait_to(self, cond: &Cond, to: XwTm) -> Result<SpinlockGuard<'a, T>, CondError> {
        unsafe {
            let mut rc = xwrustffi_cond_acquire(cond.cond.get(), *cond.tik.get());
            if rc == 0 {
                let mut lkst = 0;
                rc = xwrustffi_cond_wait_to(cond.cond.get(),
                                            self.lock.splk.get() as _, XWOS_LK_SPLK, ptr::null_mut(),
                                            to, &mut lkst);
                xwrustffi_cond_put(cond.cond.get());
                if 0 == rc {
                    Ok(self)
                } else {
                    if XWOS_LKST_LOCKED == lkst {
                        drop(self);
                    } else {
                        self.lock();
                        drop(self);
                    }
                    if -EINTR == rc {
                        Err(CondError::Interrupt(rc))
                    } else if -ETIMEDOUT == rc {
                        Err(CondError::Timedout(rc))
                    } else if -ENOTTHDCTX == rc {
                        Err(CondError::NotThreadContext(rc))
                    } else {
                        Err(CondError::Unknown(rc))
                    }
                }
            } else {
                drop(self);
                Err(CondError::NotInit(rc))
            }
        }
    }

    /// 阻塞当前线程，直到被条件量唤醒，且阻塞不可被中断
    ///
    /// 此方法会消费自旋锁的守卫(Guard)，并当线程阻塞时，在条件量内部释放自旋锁。
    /// 当条件成立，线程被唤醒，会在条件量内部上锁自旋锁，并重新返回自旋锁的守卫(Guard)。
    ///
    /// + 当返回自旋锁的守卫 [`SpinlockGuard`] 时，自旋锁已经被重新上锁；
    /// + 当返回 [`Err`] 时，自旋锁未被上锁。
    ///
    /// # 参数说明
    ///
    /// + cond: 条件量的引用
    ///
    /// # 错误码
    ///
    /// + [`CondError::NotInit`] 条件量未被初始化
    /// + [`CondError::NotThreadContext`] 不在线程上下文中
    ///
    /// # 示例
    ///
    /// ```rust
    /// use xwrust::xwos::thd;
    /// use xwrust::xwos::lock::spinlock::*;
    /// use xwrust::xwos::sync::cond::*;
    /// extern crate alloc;
    /// use alloc::sync::Arc;
    ///
    /// pub fn xwrust_example_spinlock() {
    ///     let pair = Arc::new((Spinlock::new(true), Cond::new()));
    ///     pair.1.init();
    ///     let pair_c = pair.clone();
    ///
    ///     thd::Builder::new()
    ///         .name("child".into())
    ///         .spawn(move |_| { // 子线程闭包
    ///             cthd::sleep(xwtm::ms(500));
    ///             let (lock, cvar) = &*pair_c;
    ///             let mut guard = lock_child.lock();
    ///             *guard = false; // 设置共享数据
    ///             drop(guard); // 先解锁再触发条件可提高效率
    ///             cvar.broadcast();
    ///         });
    ///     let (lock, cvar) = &*pair;
    ///     {
    ///         let mut guard = lock.lock();
    ///         while *guard {
    ///             match guard.wait_unintr(cvar) {
    ///                 Ok(g) => { // 唤醒
    ///                     guard = g; // 重新捕获守卫
    ///                 },
    ///                 Err(e) => { // 等待条件量失败
    ///                     break;
    ///                 },
    ///             }
    ///         }
    ///     }
    /// }
    /// ```
    ///
    /// [`Err`]: <https://doc.rust-lang.org/core/result/enum.Result.html#variant.Err>
    pub fn wait_unintr(self, cond: &Cond) -> Result<SpinlockGuard<'a, T>, CondError> {
        unsafe {
            let mut rc = xwrustffi_cond_acquire(cond.cond.get(), *cond.tik.get());
            if rc == 0 {
                let mut lkst = 0;
                rc = xwrustffi_cond_wait_unintr(cond.cond.get(),
                                                self.lock.splk.get() as _, XWOS_LK_SPLK, ptr::null_mut(),
                                                &mut lkst);
                xwrustffi_cond_put(cond.cond.get());
                if 0 == rc {
                    Ok(self)
                } else {
                    if XWOS_LKST_LOCKED == lkst {
                        drop(self);
                    } else {
                        self.lock();
                        drop(self);
                    }
                    if -EINTR == rc {
                        Err(CondError::Interrupt(rc))
                    } else if -ENOTTHDCTX == rc {
                        Err(CondError::NotThreadContext(rc))
                    } else {
                        Err(CondError::Unknown(rc))
                    }
                }
            } else {
                drop(self);
                Err(CondError::NotInit(rc))
            }
        }
    }
}

impl<T: ?Sized> Deref for SpinlockGuard<'_, T> {
    type Target = T;

    fn deref(&self) -> &T {
        unsafe { &*self.lock.data.get() }
    }
}

impl<T: ?Sized> DerefMut for SpinlockGuard<'_, T> {
    fn deref_mut(&mut self) -> &mut T {
        unsafe { &mut *self.lock.data.get() }
    }
}

impl<T: ?Sized> Drop for SpinlockGuard<'_, T> {
    fn drop(&mut self) {
        unsafe {
            match *self.lock.mode.get() {
                SpinlockMode::Lock => {
                    xwrustffi_splk_unlock(self.lock.splk.get());
                },
                SpinlockMode::LockCpuirq => {
                    xwrustffi_splk_unlock_cpuirq(self.lock.splk.get());
                },
                SpinlockMode::LockCpuirqSave(cpuirq) => {
                    xwrustffi_splk_unlock_cpuirqrs(self.lock.splk.get(), cpuirq.unwrap_unchecked());
                },
                SpinlockMode::LockBh => {
                    xwrustffi_splk_unlock_bh(self.lock.splk.get());
                },
            }
        }
    }
}
