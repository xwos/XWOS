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
//! + 当临界区被多个线程和单一中断访问时，需使用关闭中断的模式： [`SpinlockMode::LockCpuirq`]
//! + 当临界区被多个中断上下文访问时，需使用保存中断标志的模式： [`SpinlockMode::LockCpuirqSave`]
//!
//!
//! # 创建方法
//!
//! **自旋锁** 具有编译期新建的方法，即其 [`Spinlock::new()`] 方法具有 **const fn** 约束。
//!
//! + 可用于定义 [`'static`] 约束的全局共享变量：
//!
//! ```rust
//! use xwrust::xwos::lock::spinlock::*;
//! static GLOBAL_SPINLOCK: Spinlock<u32> = Spinlock::new(0);
//!
//! pub unsafe extern "C" fn xwrust_main() {
//!     // ...省略...
//!     GLOBAL_SPINLOCK.init();
//!     match GLOBAL_SPINLOCK.lock() {
//!         Ok(mut guard) => { // 主线程上锁成功
//!             *guard = 1; // 访问共享变量
//!         }
//!         Err(e) => {
//!             // 主线程上锁失败
//!         }
//!     }
//!     // ...省略...
//!     thd::Builder::new()
//!         .name("child".into())
//!         .spawn(move |_| {
//!             // 子线程闭包
//!             match GLOBAL_SPINLOCK.lock() {
//!                 Ok(mut guard) => { // 子线程上锁成功
//!                     *guard += 1;
//!                 }
//!                 Err(e) => { // 子线程上锁失败
//!                 }
//!             }
//!         });
//! }
//! ```
//!
//! + 也可以通过 [`alloc::sync::Arc`] 在堆上动态创建自旋锁。
//!
//!
//! ```rust
//! use xwrust::xwos::lock::spinlock::*;
//!
//! pub unsafe extern "C" fn xwrust_main() {
//!     // ...省略...
//!     let lock: Arc<Spinlock<u32>> = Arc::new(Spinlock::new(0));
//!     lock.init();
//!     let lock_child = lock.clone();
//!
//!     match lock.lock() {
//!         Ok(mut guard) => { // 主线程上锁成功
//!             *guard = 1; // 访问共享变量
//!         }
//!         Err(e) => {
//!             // 主线程上锁失败
//!         }
//!     }
//!     // ...省略...
//!     thd::Builder::new()
//!         .name("child".into())
//!         .spawn(move |_| {
//!             // 子线程闭包
//!             match lock_child.lock() {
//!                 Ok(mut guard) => { // 子线程上锁成功
//!                     *guard += 1;
//!                 }
//!                 Err(e) => { // 子线程上锁失败
//!                 }
//!             }
//!         });
//! }
//! ```
//!
//! [`'static`]: https://doc.rust-lang.org/std/keyword.static.html
//! [`alloc::sync::Arc`]: <https://doc.rust-lang.org/alloc/sync/struct.Arc.html>

extern crate core;
use core::cell::UnsafeCell;
use core::result::Result;
use core::default::Default;
use core::ops::Drop;
use core::ops::Deref;
use core::ops::DerefMut;
use core::sync::atomic::*;

use crate::types::*;
use crate::errno::*;


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
    /// 自旋锁没有初始化
    NotInit,
    /// 尝试上锁失败
    Again,
    /// 未知错误
    Unknown(XwEr),
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
    LockCpuirqSave(XwReg),
}

////////////////////////////////////////////////////////////////
// 自旋锁
////////////////////////////////////////////////////////////////
/// XWOS自旋锁占用的内存大小
pub const SIZEOF_XWOS_SPLK: usize = 8;

xwos_struct! {
    /// 用于构建自旋锁的内存数组类型
    pub struct XwosSplk {
        #[doc(hidden)]
        mem: [u8; SIZEOF_XWOS_SPLK],
    }
}

/// 用于构建自旋锁的内存数组常量
///
/// 此常量的作用是告诉编译器自旋锁需要多大的内存。
pub const XWOS_SPLK_INITIALIZER: XwosSplk = XwosSplk {
    mem: [0; SIZEOF_XWOS_SPLK],
};


/// 自旋锁结构体
pub struct Spinlock<T: ?Sized> {
    /// 用于初始化XWOS自旋锁的内存空间
    splk: UnsafeCell<XwosSplk>,
    /// 初始化完成标记
    init: AtomicBool,
    /// 上锁方式
    mode: UnsafeCell<SpinlockMode>,
    /// 用户数据
    pub data: UnsafeCell<T>,
}

impl<T> Spinlock<T> {
    /// 新建自旋锁。
    ///
    /// 此方法是编译期方法，可用于新建 [`'static`] 约束的全局变量。
    ///
    /// # 示例
    ///
    /// ```rust
    /// use xwrust::xwos::lock::spinlock::*;
    ///
    /// static GLOBAL_SPINLOCK: Spinlock<u32>  = Spinlock::new(0);
    /// ```
    ///
    /// [`'static`]: https://doc.rust-lang.org/std/keyword.static.html
    pub const fn new(t: T) -> Self {
        Self {
            splk: UnsafeCell::new(XWOS_SPLK_INITIALIZER),
            init: AtomicBool::new(false),
            mode: UnsafeCell::new(SpinlockMode::Lock),
            data: UnsafeCell::new(t),
        }
    }
}

impl<T: ?Sized> Spinlock<T> {
    /// 初始化自旋锁。
    ///
    /// 自旋锁必须调用此方法一次，方可正常使用。
    ///
    /// # 示例
    ///
    /// ```rust
    /// use xwrust::xwos::lock::spinlock::*;
    ///
    /// static GLOBAL_SPINLOCK: Spinlock<u32>  = Spinlock::new(0);
    ///
    /// pub unsafe extern "C" fn xwrust_main() {
    ///     // ...省略...
    ///     GLOBAL_SPINLOCK.init();
    ///     // 从此处开始 GLOBAL_SPINLOCK 可正常使用
    /// }
    /// ```
    pub fn init(&self) {
        match self.init.compare_exchange(false, true,
                                         Ordering::AcqRel, Ordering::Relaxed) {
            Ok(_) => unsafe {
                xwrustffi_splk_init(self.splk.get());
            },
            Err(_) => {}
        }
    }

    /// 获取自旋锁，若线程无法获取自旋锁，就阻塞等待，直到能获得锁为止。
    ///
    /// + 若成功获取自旋锁，将返回 **RAII Guard** ： [`SpinlockGuard`] ，用于提供 **Scoped Lock** 机制。
    ///   + [`SpinlockGuard`] 中包含 [`Spinlock`] 的引用， 当 [`SpinlockGuard`] 生命周期结束时，会在 [`drop()`] 方法中自动解锁自旋锁。
    /// + 若失败，将返回错误码 [`SpinlockError`] 。
    ///
    /// # 锁模式 **mode**
    ///
    /// + [`SpinlockMode::Lock`] 关闭抢占
    /// + [`SpinlockMode::LockBh`] 关闭抢占、中断底半部
    /// + [`SpinlockMode::LockCpuirq`] 关闭抢占、中断底半部和中断
    /// + [`SpinlockMode::LockCpuirqSave`] 关闭抢占、中断底半部和中断，并保存之前的中断标志
    ///
    /// # 错误码
    ///
    /// + [`SpinlockError::NotInit`] 自旋锁未被初始化
    ///
    /// # 示例
    ///
    /// ```rust
    /// use xwrust::xwos::lock::spinlock::*;
    /// static GLOBAL_SPINLOCK: Spinlock<u32> = Spinlock::new(0);
    ///
    /// pub unsafe extern "C" fn xwrust_main() {
    ///     // ...省略...
    ///     GLOBAL_SPINLOCK.init();
    ///     match GLOBAL_SPINLOCK.lock() {
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
    /// [`drop()`]: https://doc.rust-lang.org/std/mem/fn.drop.html
    pub fn lock(&self, mode: SpinlockMode) -> Result<SpinlockGuard<'_, T>, SpinlockError> {
        if self.init.load(Ordering::Acquire) {
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
                        *self.mode.get() = SpinlockMode::LockCpuirqSave(cpuirq);
                    },
                    SpinlockMode::LockBh => {
                        xwrustffi_splk_lock_bh(self.splk.get());
                        *self.mode.get() = SpinlockMode::LockBh;
                    },
                }
                Ok(SpinlockGuard::new(self))
            }
        } else {
            Err(SpinlockError::NotInit)
        }
    }

    /// 尝试获取自旋锁，若线程无法获取自旋锁，立即返回错误。
    ///
    /// + 若成功获取自旋锁，将返回 **RAII Guard** ： [`SpinlockGuard`] ，用于提供 **Scoped Lock** 机制。
    ///   + [`SpinlockGuard`] 中包含 [`Spinlock`] 的引用， 当 [`SpinlockGuard`] 生命周期结束时，会在 [`drop()`] 方法中自动解锁自旋锁。
    /// + 若失败，将返回错误码 [`SpinlockError`] 。
    ///
    /// # 锁模式 **mode**
    ///
    /// + [`SpinlockMode::Lock`] 关闭抢占
    /// + [`SpinlockMode::LockBh`] 关闭抢占、中断底半部
    /// + [`SpinlockMode::LockCpuirq`] 关闭抢占、中断底半部和中断
    /// + [`SpinlockMode::LockCpuirqSave`] 关闭抢占、中断底半部和中断，并保存之前的中断标志
    ///
    /// # 错误码
    ///
    /// + [`SpinlockError::NotInit`] 自旋锁未被初始化
    /// + [`SpinlockError::Again`] 尝试获取锁失败
    ///
    /// # 示例
    /// ```rust
    /// use xwrust::xwos::lock::spinlock::*;
    /// static GLOBAL_SPINLOCK: Spinlock<u32> = Spinlock::new(0);
    ///
    /// pub unsafe extern "C" fn xwrust_main() {
    ///     // ...省略...
    ///     GLOBAL_SPINLOCK.init();
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
    /// [`drop()`]: https://doc.rust-lang.org/std/mem/fn.drop.html
    pub fn trylock(&self, mode: SpinlockMode) -> Result<SpinlockGuard<'_, T>, SpinlockError> {
        if self.init.load(Ordering::Acquire) {
            unsafe {
                let rc: XwEr;
                match mode {
                    SpinlockMode::Lock => {
                        rc = xwrustffi_splk_trylock(self.splk.get());
                        if 0 == rc {
                            *self.mode.get() = SpinlockMode::Lock;
                            Ok(SpinlockGuard::new(self))
                        } else if -EAGAIN == rc {
                            Err(SpinlockError::Again)
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
                            Err(SpinlockError::Again)
                        } else {
                            Err(SpinlockError::Unknown(rc))
                        }
                    },
                    SpinlockMode::LockCpuirqSave(_) => {
                        let mut cpuirq: XwReg = 0;
                        rc = xwrustffi_splk_trylock_cpuirqsv(self.splk.get(), &mut cpuirq);
                        if 0 == rc {
                            *self.mode.get() = SpinlockMode::LockCpuirqSave(cpuirq);
                            Ok(SpinlockGuard::new(self))
                        } else if -EAGAIN == rc {
                            Err(SpinlockError::Again)
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
                            Err(SpinlockError::Again)
                        } else {
                            Err(SpinlockError::Unknown(rc))
                        }
                    },
                }
            }
        } else {
            Err(SpinlockError::NotInit)
        }
    }

    /// 释放 [`SpinlockGuard`]，并在 [`drop()`] 方法中解锁自旋锁。
    ///
    /// # 示例
    ///
    /// ```rust
    /// use xwrust::xwos::lock::spinlock::*;
    /// static GLOBAL_SPINLOCK: Spinlock<u32> = Spinlock::new(0);
    ///
    /// pub unsafe extern "C" fn xwrust_main() {
    ///     // ...省略...
    ///     GLOBAL_SPINLOCK.init();
    ///     match GLOBAL_SPINLOCK.lock() {
    ///         Ok(mut guard) => { // 上锁成功
    ///             *guard = 1; // 访问共享变量
    ///             Spinlock::unlock(guard); // 主动解锁
    ///         }
    ///         Err(e) => {
    ///             // 上锁失败
    ///         }
    ///     }
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
        self.init.store(false, Ordering::Release);
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
                    xwrustffi_splk_unlock_cpuirqrs(self.lock.splk.get(), cpuirq);
                },
                SpinlockMode::LockBh => {
                    xwrustffi_splk_unlock_bh(self.lock.splk.get());
                },
            }
        }
    }
}