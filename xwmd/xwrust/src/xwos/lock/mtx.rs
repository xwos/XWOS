//! XWOS RUST：互斥锁
//! ========
//!
//! 互斥锁是用来保证不同线程正确访问共享数据的机制。访问共享数据的代码片段被称为临界区。
//!
//! 互斥锁 **只能** 用在 **线程上下文（Thread Context）** 。等待互斥锁的线程会被阻塞，并让出CPU的使用权。
//!
//! XWOS RUST框架的互斥锁是仿造 [`std::sync::Mutex`] 的来编写的，以降低熟悉Rust的朋友的学习代价。
//!
//! 互斥锁上锁后，可返回一个 **守卫** [`MutexGuard`] ，用于提供 **Scoped Lock** 机制：即只负责上锁，不用管解锁。
//! 解锁会由 [`MutexGuard`] 在其生命周期结束后自动触发。
//!
//! # 创建方法
//!
//! **互斥锁**具有编译期新建的方法，即其 [`Mutex::new()`] 方法具有 **const fn** 约束。
//!
//! + 可用于定义 [`'static`] 约束的全局共享变量：
//!
//! ```rust
//! use xwrust::xwos::thd;
//! use xwrust::xwos::lock::mtx::*;
//! static GLOBAL_MUTEX: Mutex<u32> = Mutex::new(0);
//!
//! pub unsafe extern "C" fn xwrust_main() {
//!     // ...省略...
//!     GLOBAL_MUTEX.init();
//!     match GLOBAL_MUTEX.lock() {
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
//!             match GLOBAL_MUTEX.lock() {
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
//! + 也可以通过 [`alloc::sync::Arc`] 在堆上动态创建互斥锁对象。
//!
//!
//! ```rust
//! use xwrust::xwos::thd;
//! use xwrust::xwos::lock::mtx::*;
//! extern crate alloc;
//! use alloc::sync::Arc;
//!
//! pub unsafe extern "C" fn xwrust_main() {
//!     // ...省略...
//!     let lock: Arc<Mutex<u32>> = Arc::new(Mutex::new(0));
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
//! # 与 [`std::sync::Mutex`] 的区别
//!
//! 对比 [`std::sync::Mutex`] 中的例子
//!
//! ```rust
//! use std::sync::{Arc, Mutex};
//! use std::thread;
//!
//! fn main() {
//!     // 创建互斥锁的方法类似
//!     let lock = Arc::new(Mutex::new(0_u32));
//!     let lock2 = Arc::clone(&lock);
//!
//!     let _ = thread::spawn(move || -> () {
//!         // 访问共享数据的方法类似：
//!         // 子线程中对互斥锁上锁， unwrap() 从 Ok() 中取出 guard，再对 guard 解可变引用可得数据的可变引用
//!         let guard = lock2.lock().unwrap();
//!         *guard = 1;
//!
//!         // std库特有的机制：持有锁时 panic!() 将导致锁变成 **中毒状态(poisoned)** 。
//!         panic!();
//!     }).join();
//!
//!     // std库特有的机制：处理中毒状态的锁
//!     let mut guard = match lock.lock() {
//!         Ok(guard) => guard,
//!         Err(poisoned) => poisoned.into_inner(),
//!     };
//!     *guard += 1;
//! }
//! ```
//!
//! ### 构建全局变量的方式
//!
//! + [`std::sync::Mutex`] 没有编译期构造函数，因此只能借助宏 [`lazy_static!`] 创建 [`'static`] 约束的全局变量；
//! + `xwrust::xwos::lock::mtx` 则可以直接在函数外部定义。
//!
//! ### 中毒锁机制
//!
//! + [`std::sync::Mutex`] 提供了 **Poisoned Mutex** 机制。
//! 当通过 [`std::thread::spawn()`] 创建的子线程在持有 **互斥锁** 时，发生了 [`panic!()`] ，
//! 此时 **互斥锁** 的状态被称为 **中毒(Poisoned)** 。 [`std::sync::Mutex`] 可在父线程中检测到此错误状态，并尝试恢复。
//! + `xwrust::xwos::lock::mtx` 不提供此机制，用户必须处理 [`Ok`] 与 [`Err`] ，不可使用 [`unwrap()`]。
//! 因为此机制需要依赖 **unwind** 机制 ，目前 **unwind** 在MCU上还不成熟：
//!   + Gcc可以在MCU C++中使用 **try-catch**；
//!   + LLVM（Clan++）还无法支持在MCU C++中使用 **try-catch**；
//!   + Rust目前在MCU上还无法使用 **panic_unwind** 的 feature。
//!
//! [`std::sync::Mutex`]: <https://doc.rust-lang.org/std/sync/struct.Mutex.html>
//! [`'static`]: <https://doc.rust-lang.org/std/keyword.static.html>
//! [`alloc::sync::Arc`]: <https://doc.rust-lang.org/alloc/sync/struct.Arc.html>
//! [`lazy_static!`]: <https://lib.rs/crates/lazy_static>
//! [`std::thread::spawn()`]: <https://doc.rust-lang.org/std/thread/fn.spawn.html>
//! [`panic!()`]: <https://doc.rust-lang.org/core/macro.panic.html>
//! [`Ok`]: <https://doc.rust-lang.org/core/result/enum.Result.html#variant.Ok>
//! [`Err`]: <https://doc.rust-lang.org/core/result/enum.Result.html#variant.Err>
//! [`unwrap()`]: <https://doc.rust-lang.org/core/result/enum.Result.html#method.unwrap>

extern crate core;
use core::cell::UnsafeCell;
use core::result::Result;
use core::default::Default;
use core::ops::Drop;
use core::ops::Deref;
use core::ops::DerefMut;
use core::ptr;
use core::mem;

use crate::types::*;
use crate::errno::*;
use crate::xwos::sync::cond::*;


extern "C" {
    fn xwrustffi_mtx_init(mtx: *mut XwosMtx) -> XwEr;
    fn xwrustffi_mtx_fini(mtx: *mut XwosMtx) -> XwEr;
    fn xwrustffi_mtx_grab(mtx: *mut XwosMtx) -> XwEr;
    fn xwrustffi_mtx_put(mtx: *mut XwosMtx) -> XwEr;
    fn xwrustffi_mtx_gettik(mtx: *mut XwosMtx) -> XwSq;
    fn xwrustffi_mtx_acquire(mtx: *mut XwosMtx, tik: XwSq) -> XwEr;
    fn xwrustffi_mtx_release(mtx: *mut XwosMtx, tik: XwSq) -> XwEr;
    fn xwrustffi_mtx_unlock(mtx: *mut XwosMtx) -> XwEr;
    fn xwrustffi_mtx_lock(mtx: *mut XwosMtx) -> XwEr;
    fn xwrustffi_mtx_trylock(mtx: *mut XwosMtx) -> XwEr;
    fn xwrustffi_mtx_lock_to(mtx: *mut XwosMtx, to: XwTm) -> XwEr;
    fn xwrustffi_mtx_lock_unintr(mtx: *mut XwosMtx) -> XwEr;
}

/// 互斥锁的错误码
#[derive(Debug)]
pub enum MutexError {
    /// 互斥锁没有初始化
    NotInit,
    /// 等待被中断
    Interrupt,
    /// 尝试上锁失败
    WouldBlock,
    /// 等待超时
    Timedout,
    /// 不在线程上下文内
    NotThreadContext,
    /// 未知错误
    Unknown(XwEr),
}

/// XWOS互斥锁对象占用的内存大小
pub const SIZEOF_XWOS_MTX: usize = 80;

xwos_struct! {
    /// 用于构建互斥锁的内存数组类型
    pub struct XwosMtx {
        #[doc(hidden)]
        mem: [u8; SIZEOF_XWOS_MTX],
    }
}

/// 用于构建互斥锁的内存数组常量
///
/// 此常量的作用是告诉编译器互斥锁对象需要多大的内存。
pub const XWOS_MTX_INITIALIZER: XwosMtx = XwosMtx {
    mem: [0; SIZEOF_XWOS_MTX],
};

/// 互斥锁对象结构体
pub struct Mutex<T: ?Sized> {
    /// 用于初始化XWOS互斥锁对象的内存空间
    mtx: UnsafeCell<XwosMtx>,
    /// 互斥锁对象的标签
    tik: UnsafeCell<XwSq>,
    /// 用户数据
    pub data: UnsafeCell<T>,
}

impl<T> Mutex<T> {
    /// 新建互斥锁对象。
    ///
    /// 此方法是编译期方法，可用于新建 [`'static`] 约束的全局变量。
    ///
    /// # 示例
    ///
    /// ```rust
    /// use xwrust::xwos::lock::mtx::*;
    ///
    /// static GLOBAL_MUTEX: Mutex<u32> = Mutex::new(0);
    /// ```
    ///
    /// [`'static`]: https://doc.rust-lang.org/std/keyword.static.html
    pub const fn new(t: T) -> Self {
        Self {
            mtx: UnsafeCell::new(XWOS_MTX_INITIALIZER),
            tik: UnsafeCell::new(0),
            data: UnsafeCell::new(t),
        }
    }
}

impl<T: ?Sized> Mutex<T> {
    /// 初始化互斥锁对象。
    ///
    /// 互斥锁对象必须调用此方法一次，方可正常使用。
    ///
    /// # 示例
    ///
    /// ```rust
    /// use xwrust::xwos::lock::mtx::*;
    ///
    /// static GLOBAL_MUTEX: Mutex<u32> = Mutex::new(0);
    ///
    /// pub unsafe extern "C" fn xwrust_main() {
    ///     // ...省略...
    ///     GLOBAL_MUTEX.init();
    ///     // 从此处开始 GLOBAL_MUTEX 可正常使用
    /// }
    /// ```
    pub fn init(&self) {
        unsafe {
            let rc = xwrustffi_mtx_acquire(self.mtx.get(), *self.tik.get());
            if rc == 0 {
                xwrustffi_mtx_put(self.mtx.get());
            } else {
                xwrustffi_mtx_init(self.mtx.get());
                *self.tik.get() = xwrustffi_mtx_gettik(self.mtx.get());
            }
        }
    }

    /// 获取互斥锁，若线程无法获取互斥锁，就阻塞等待，直到能获得锁为止。
    ///
    /// + 若成功获取互斥锁，将返回 **RAII Guard** ： [`MutexGuard`] ，用于提供 **Scoped Lock** 机制。
    ///   + [`MutexGuard`] 中包含 [`Mutex`] 的引用， 当 [`MutexGuard`] 生命周期结束时，会在 [`drop()`] 方法中自动解锁互斥锁。
    /// + 若失败，将返回错误码 [`MutexError`] 。
    ///
    /// # 错误码
    ///
    /// + [`MutexError::NotInit`] 互斥锁未被初始化
    /// + [`MutexError::Interrupt`] 等待被中断
    /// + [`MutexError::NotThreadContext`] 不在线程上下文中
    ///
    /// # 示例
    ///
    /// ```rust
    /// use xwrust::xwos::lock::mtx::*;
    /// static GLOBAL_MUTEX: Mutex<u32> = Mutex::new(0);
    ///
    /// pub unsafe extern "C" fn xwrust_main() {
    ///     // ...省略...
    ///     GLOBAL_MUTEX.init();
    ///     match GLOBAL_MUTEX.lock() {
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
    pub fn lock(&self) -> Result<MutexGuard<'_, T>, MutexError> {
        unsafe {
            let mut rc = xwrustffi_mtx_acquire(self.mtx.get(), *self.tik.get());
            if rc == 0 {
                rc = xwrustffi_mtx_lock(self.mtx.get());
                xwrustffi_mtx_put(self.mtx.get());
                if 0 == rc {
                    Ok(MutexGuard::new(self))
                } else if -EINTR == rc {
                    Err(MutexError::Interrupt)
                } else if -ENOTTHDCTX == rc {
                    Err(MutexError::NotThreadContext)
                } else {
                    Err(MutexError::Unknown(rc))
                }
            } else {
                Err(MutexError::NotInit)
            }
        }
    }

    /// 尝试获取互斥锁，若线程无法获取互斥锁，立即返回错误。
    ///
    /// + 若成功获取互斥锁，将返回 **RAII Guard** ： [`MutexGuard`] ，用于提供 **Scoped Lock** 机制。
    ///   + [`MutexGuard`] 中包含 [`Mutex`] 的引用， 当 [`MutexGuard`] 生命周期结束时，会在 [`drop()`] 方法中自动解锁互斥锁。
    /// + 若失败，将返回错误码 [`MutexError`] 。
    ///
    /// # 错误码
    ///
    /// + [`MutexError::NotInit`] 互斥锁未被初始化
    /// + [`MutexError::WouldBlock`] 尝试获取锁失败
    /// + [`MutexError::NotThreadContext`] 不在线程上下文中
    ///
    /// # 示例
    /// ```rust
    /// use xwrust::xwos::lock::mtx::*;
    /// static GLOBAL_MUTEX: Mutex<u32> = Mutex::new(0);
    ///
    /// pub unsafe extern "C" fn xwrust_main() {
    ///     // ...省略...
    ///     GLOBAL_MUTEX.init();
    ///     match GLOBAL_MUTEX.trylock() {
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
    pub fn trylock(&self) -> Result<MutexGuard<'_, T>, MutexError> {
        unsafe {
            let mut rc = xwrustffi_mtx_acquire(self.mtx.get(), *self.tik.get());
            if rc == 0 {
                rc = xwrustffi_mtx_trylock(self.mtx.get());
                xwrustffi_mtx_put(self.mtx.get());
                if 0 == rc {
                    Ok(MutexGuard::new(self))
                } else if -EWOULDBLOCK == rc {
                    Err(MutexError::WouldBlock)
                } else if -ENOTTHDCTX == rc {
                    Err(MutexError::NotThreadContext)
                } else {
                    Err(MutexError::Unknown(rc))
                }
            } else {
                Err(MutexError::NotInit)
            }
        }
    }

    /// 获取互斥锁，若线程无法获取互斥锁，就限时阻塞等待。
    ///
    /// + 若成功获取互斥锁，将返回 **RAII Guard** ： [`MutexGuard`] ，用于提供 **Scoped Lock** 机制。
    ///   + [`MutexGuard`] 中包含 [`Mutex`] 的引用， 当 [`MutexGuard`] 生命周期结束时，会在 [`drop()`] 方法中自动解锁互斥锁。
    /// + 若失败，将返回错误码 [`MutexError`] 。
    ///
    /// # 错误码
    ///
    /// + [`MutexError::NotInit`] 互斥锁未被初始化
    /// + [`MutexError::Interrupt`] 等待被中断
    /// + [`MutexError::Timedout`] 等待超时
    /// + [`MutexError::NotThreadContext`] 不在线程上下文中
    ///
    /// # 示例
    ///
    /// ```rust
    /// use xwrust::xwtm;
    /// use xwrust::xwos::lock::mtx::*;
    /// static GLOBAL_MUTEX: Mutex<u32> = Mutex::new(0);
    ///
    /// pub unsafe extern "C" fn xwrust_main() {
    ///     // ...省略...
    ///     GLOBAL_MUTEX.init();
    ///     match GLOBAL_MUTEX.lock_to(xwtm::ft(xwtm::s(10))) { // 最多等待10s
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
    pub fn lock_to(&self, to: XwTm) -> Result<MutexGuard<'_, T>, MutexError> {
        unsafe {
            let mut rc = xwrustffi_mtx_acquire(self.mtx.get(), *self.tik.get());
            if rc == 0 {
                rc = xwrustffi_mtx_lock_to(self.mtx.get(), to);
                xwrustffi_mtx_put(self.mtx.get());
                if 0 == rc {
                    Ok(MutexGuard::new(self))
                } else if -EINTR == rc {
                    Err(MutexError::Interrupt)
                } else if -ETIMEDOUT == rc {
                    Err(MutexError::Timedout)
                } else if -ENOTTHDCTX == rc {
                    Err(MutexError::NotThreadContext)
                } else {
                    Err(MutexError::Unknown(rc))
                }
            } else {
                Err(MutexError::NotInit)
            }
        }
    }

    /// 获取互斥锁，若线程无法获取互斥锁，就阻塞等待，且不可中断，直到能获得锁为止。
    ///
    /// + 若成功获取互斥锁，将返回 **RAII Guard** ： [`MutexGuard`] ，用于提供 **Scoped Lock** 机制。
    ///   + [`MutexGuard`] 中包含 [`Mutex`] 的引用， 当 [`MutexGuard`] 生命周期结束时，会在 [`drop()`] 方法中自动解锁互斥锁。
    /// + 若失败，将返回错误码 [`MutexError`] 。
    ///
    /// # 错误码
    ///
    /// + [`MutexError::NotInit`] 互斥锁未被初始化
    /// + [`MutexError::NotThreadContext`] 不在线程上下文中
    ///
    /// # 示例
    /// ```rust
    /// use xwrust::xwos::lock::mtx::*;
    /// static GLOBAL_MUTEX: Mutex<u32> = Mutex::new(0);
    ///
    /// pub unsafe extern "C" fn xwrust_main() {
    ///     // ...省略...
    ///     GLOBAL_MUTEX.init();
    ///     match GLOBAL_MUTEX.lock_unintr() {
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
    pub fn lock_unintr(&self) -> Result<MutexGuard<'_, T>, MutexError> {
        unsafe {
            let mut rc = xwrustffi_mtx_acquire(self.mtx.get(), *self.tik.get());
            if rc == 0 {
                rc = xwrustffi_mtx_lock_unintr(self.mtx.get());
                xwrustffi_mtx_put(self.mtx.get());
                if 0 == rc {
                    Ok(MutexGuard::new(self))
                } else if -EWOULDBLOCK == rc {
                    Err(MutexError::WouldBlock)
                } else if -ENOTTHDCTX == rc {
                    Err(MutexError::NotThreadContext)
                } else {
                    Err(MutexError::Unknown(rc))
                }
            } else {
                Err(MutexError::NotInit)
            }
        }
    }


    /// 释放 [`MutexGuard`]，并在 [`drop()`] 方法中解锁互斥锁。
    ///
    /// # 示例
    ///
    /// ```rust
    /// use xwrust::xwos::lock::mtx::*;
    /// static GLOBAL_MUTEX: Mutex<u32> = Mutex::new(0);
    ///
    /// pub unsafe extern "C" fn xwrust_main() {
    ///     // ...省略...
    ///     GLOBAL_MUTEX.init();
    ///     match GLOBAL_MUTEX.lock() {
    ///         Ok(mut guard) => { // 上锁成功
    ///             *guard = 1; // 访问共享变量
    ///             Mutex::unlock(guard); // 主动解锁
    ///         }
    ///         Err(e) => {
    ///             // 上锁失败
    ///         }
    ///     }
    /// }
    /// ```
    ///
    /// [`drop()`]: https://doc.rust-lang.org/std/mem/fn.drop.html
    pub fn unlock(guard: MutexGuard<'_, T>) {
        drop(guard)
    }
}

unsafe impl<T: ?Sized + Send> Send for Mutex<T> {}
unsafe impl<T: ?Sized + Send> Sync for Mutex<T> {}

impl<T> From<T> for Mutex<T> {
    /// 从数据新建互斥锁对象
    ///
    /// 此方法会将数据所有权转移到互斥锁对象的内部
    ///
    /// 等价于 [`Mutex::new`]
    fn from(t: T) -> Self {
        Mutex::new(t)
    }
}

impl<T: ?Sized + Default> Default for Mutex<T> {
    fn default() -> Mutex<T> {
        Mutex::new(Default::default())
    }
}

impl<T: ?Sized> Drop for Mutex<T> {
    fn drop(&mut self) {
        unsafe {
            xwrustffi_mtx_fini(self.mtx.get());
        }
    }
}

/// 互斥锁对象的RAII Guard
///
/// **RAII Guard** 用于提供 **Scoped Lock** 机制。
///
/// + [`MutexGuard`] 中包含 [`Mutex`] 的引用， 当 [`MutexGuard`] 生命周期结束时，会在 [`drop()`] 方法中自动解锁互斥锁。
/// + [`MutexGuard`] 不可在线程之间转移所有权，因为其 [`drop()`] 方法包含解锁的语义，上锁和解锁必须在同一线程；
/// + [`MutexGuard`] 虽然可以在多线程中传递引用( [`Sync`] 约束)，但其实现中没有 **unlock()** 方法，意味着其他线程即便拿到引用也不能解锁。
///
/// [`drop()`]: https://doc.rust-lang.org/std/mem/fn.drop.html
/// [`Sync`]: https://doc.rust-lang.org/std/marker/trait.Send.html
pub struct MutexGuard<'a, T: ?Sized + 'a> {
    /// 互斥锁的引用
    lock: &'a Mutex<T>,
}

impl<T: ?Sized> !Send for MutexGuard<'_, T> {}
unsafe impl<T: ?Sized + Sync> Sync for MutexGuard<'_, T> {}

impl<'a, T: ?Sized> MutexGuard<'a, T> {
    fn new(lock: &'a Mutex<T>) -> MutexGuard<'a, T> {
        MutexGuard { lock: lock }
    }

    /// 阻塞当前线程，直到被条件量唤醒。
    ///
    /// 此方法会消费互斥锁的守卫(Guard)，并当线程阻塞时，在条件量内部释放互斥锁。
    /// 当条件成立，线程被唤醒，会在条件量内部上锁互斥锁，并重新返回互斥锁的守卫(Guard)。
    ///
    /// + 当返回互斥锁的守卫 [`MutexGuard`] 时，互斥锁已经被重新上锁；
    /// + 当返回 [`Err`] 时，互斥锁未被上锁。
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
    /// use xwrust::xwos::lock::mtx::*;
    /// use xwrust::xwos::sync::cond::*;
    /// extern crate alloc;
    /// use alloc::sync::Arc;
    ///
    /// pub unsafe extern "C" fn xwrust_main() {
    ///     let pair = Arc::new((Mutex::new(true), Cond::new()));
    ///     pair.0.init();
    ///     pair.1.init();
    ///     let pair_c = pair.clone();
    ///
    ///     thd::Builder::new()
    ///         .name("child".into())
    ///         .spawn(move |_| { // 子线程闭包
    ///             cthd::sleep(xwtm::ms(500));
    ///             let (lock, cvar) = &*pair_c;
    ///             match lock_child.lock() {
    ///                 Ok(mut guard) => {
    ///                     *guard = false; // 设置共享数据
    ///                     drop(guard); // 先解锁再触发条件可提高效率
    ///                     cvar.broadcast();
    ///                 },
    ///                 Err(e) => { // 子线程上锁失败
    ///                 },
    ///             }
    ///         });
    ///     let (lock, cvar) = &*pair;
    ///     let mut guard;
    ///     match lock.lock() {
    ///         Ok(g) => {
    ///             guard = g;
    ///             while *guard {
    ///                 match guard.wait(cvar) {
    ///                     Ok(g) => { // 唤醒
    ///                         guard = g;
    ///                     },
    ///                     Err(e) => { // 等待条件量失败
    ///                         break;
    ///                     },
    ///                 }
    ///             }
    ///         },
    ///         Err(e) => { // 上锁失败
    ///         },
    ///     }
    /// }
    /// ```
    ///
    /// [`Err`]: <https://doc.rust-lang.org/core/result/enum.Result.html#variant.Err>
    pub fn wait(self, cond: &Cond) -> Result<MutexGuard<'a, T>, CondError> {
        unsafe {
            let mut rc = xwrustffi_cond_acquire(cond.cond.get(), *cond.tik.get());
            if rc == 0 {
                let mut lkst = 0;
                rc = xwrustffi_cond_wait(cond.cond.get(),
                                         self.lock.mtx.get() as _, XWOS_LK_MTX, ptr::null_mut(),
                                         &mut lkst);
                xwrustffi_cond_put(cond.cond.get());
                if 0 == rc {
                    Ok(self)
                } else {
                    if XWOS_LKST_LOCKED == lkst {
                        drop(self);
                    } else {
                        mem::forget(self);
                    }
                    if -EINTR == rc {
                        Err(CondError::Interrupt)
                    } else if -ENOTTHDCTX == rc {
                        Err(CondError::NotThreadContext)
                    } else {
                        Err(CondError::Unknown(rc))
                    }
                }
            } else {
                drop(self);
                Err(CondError::NotInit)
            }
        }
    }

    /// 限时阻塞当前线程，直到被条件量唤醒。
    ///
    /// 此方法会消费互斥锁的守卫(Guard)，并当线程阻塞时，在条件量内部释放互斥锁。
    /// 当条件成立，线程被唤醒，会在条件量内部上锁互斥锁，并重新返回互斥锁的守卫(Guard)。
    /// 当超时后，将返回错误。
    ///
    /// + 当返回互斥锁的守卫 [`MutexGuard`] 时，互斥锁已经被重新上锁；
    /// + 当返回 [`Err`] 时，互斥锁未被上锁。
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
    /// use xwrust::xwos::lock::mtx::*;
    /// use xwrust::xwos::sync::cond::*;
    /// extern crate alloc;
    /// use alloc::sync::Arc;
    ///
    /// pub unsafe extern "C" fn xwrust_main() {
    ///     let pair = Arc::new((Mutex::new(true), Cond::new()));
    ///     pair.0.init();
    ///     pair.1.init();
    ///     let pair_c = pair.clone();
    ///
    ///     thd::Builder::new()
    ///         .name("child".into())
    ///         .spawn(move |_| { // 子线程闭包
    ///             cthd::sleep(xwtm::ms(500));
    ///             let (lock, cvar) = &*pair_c;
    ///             match lock_child.lock() {
    ///                 Ok(mut guard) => {
    ///                     *guard = false; // 设置共享数据
    ///                     drop(guard); // 先解锁再触发条件可提高效率
    ///                     cvar.broadcast();
    ///                 },
    ///                 Err(e) => { // 子线程上锁失败
    ///                 },
    ///             }
    ///         });
    ///     let (lock, cvar) = &*pair;
    ///     let mut guard;
    ///     match lock.lock() {
    ///         Ok(g) => {
    ///             guard = g;
    ///             while *guard {
    ///                 match guard.wait_to(cvar, xwtm::ft(xwtm::s(2))) {
    ///                     Ok(g) => { // 唤醒
    ///                         guard = g;
    ///                     },
    ///                     Err(e) => { // 等待条件量失败
    ///                         break;
    ///                     },
    ///                 }
    ///             }
    ///         },
    ///         Err(e) => { // 上锁失败
    ///         },
    ///     }
    /// }
    /// ```
    ///
    /// [`Err`]: <https://doc.rust-lang.org/core/result/enum.Result.html#variant.Err>
    pub fn wait_to(self, cond: &Cond, to: XwTm) -> Result<MutexGuard<'a, T>, CondError> {
        unsafe {
            let mut rc = xwrustffi_cond_acquire(cond.cond.get(), *cond.tik.get());
            if rc == 0 {
                let mut lkst = 0;
                rc = xwrustffi_cond_wait_to(cond.cond.get(),
                                            self.lock.mtx.get() as _, XWOS_LK_MTX, ptr::null_mut(),
                                            to, &mut lkst);
                xwrustffi_cond_put(cond.cond.get());
                if 0 == rc {
                    Ok(self)
                } else {
                    if XWOS_LKST_LOCKED == lkst {
                        drop(self);
                    } else {
                        mem::forget(self);
                    }
                    if -EINTR == rc {
                        Err(CondError::Interrupt)
                    } else if -ETIMEDOUT == rc {
                        Err(CondError::Timedout)
                    } else if -ENOTTHDCTX == rc {
                        Err(CondError::NotThreadContext)
                    } else {
                        Err(CondError::Unknown(rc))
                    }
                }
            } else {
                drop(self);
                Err(CondError::NotInit)
            }
        }
    }

    /// 阻塞当前线程，直到被条件量唤醒，且阻塞不可被中断。
    ///
    /// 此方法会消费互斥锁的守卫(Guard)，并当线程阻塞时，在条件量内部释放互斥锁。
    /// 当条件成立，线程被唤醒，会在条件量内部上锁互斥锁，并重新返回互斥锁的守卫(Guard)。
    ///
    /// + 当返回互斥锁的守卫 [`MutexGuard`] 时，互斥锁已经被重新上锁；
    /// + 当返回 [`Err`] 时，互斥锁未被上锁。
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
    /// use xwrust::xwos::lock::mtx::*;
    /// use xwrust::xwos::sync::cond::*;
    /// extern crate alloc;
    /// use alloc::sync::Arc;
    ///
    /// pub unsafe extern "C" fn xwrust_main() {
    ///     let pair = Arc::new((Mutex::new(true), Cond::new()));
    ///     pair.0.init();
    ///     pair.1.init();
    ///     let pair_c = pair.clone();
    ///
    ///     thd::Builder::new()
    ///         .name("child".into())
    ///         .spawn(move |_| { // 子线程闭包
    ///             cthd::sleep(xwtm::ms(500));
    ///             let (lock, cvar) = &*pair_c;
    ///             match lock_child.lock() {
    ///                 Ok(mut guard) => {
    ///                     *guard = false; // 设置共享数据
    ///                     drop(guard); // 先解锁再触发条件可提高效率
    ///                     cvar.broadcast();
    ///                 },
    ///                 Err(e) => { // 子线程上锁失败
    ///                 },
    ///             }
    ///         });
    ///     let (lock, cvar) = &*pair;
    ///     let mut guard;
    ///     match lock.lock() {
    ///         Ok(g) => {
    ///             guard = g;
    ///             while *guard {
    ///                 match guard.wait_unintr(cvar) {
    ///                     Ok(g) => { // 唤醒
    ///                         guard = g;
    ///                     },
    ///                     Err(e) => { // 等待条件量失败
    ///                         break;
    ///                     },
    ///                 }
    ///             }
    ///         },
    ///         Err(e) => { // 上锁失败
    ///         },
    ///     }
    /// }
    /// ```
    ///
    /// [`Err`]: <https://doc.rust-lang.org/core/result/enum.Result.html#variant.Err>
    pub fn wait_unintr(self, cond: &Cond) -> Result<MutexGuard<'a, T>, CondError> {
        unsafe {
            let mut rc = xwrustffi_cond_acquire(cond.cond.get(), *cond.tik.get());
            if rc == 0 {
                let mut lkst = 0;
                rc = xwrustffi_cond_wait_unintr(cond.cond.get(),
                                                self.lock.mtx.get() as _, XWOS_LK_MTX, ptr::null_mut(),
                                                &mut lkst);
                xwrustffi_cond_put(cond.cond.get());
                if 0 == rc {
                    Ok(self)
                } else {
                    if XWOS_LKST_LOCKED == lkst {
                        drop(self);
                    } else {
                        mem::forget(self);
                    }
                    if -ENOTTHDCTX == rc {
                        Err(CondError::NotThreadContext)
                    } else {
                        Err(CondError::Unknown(rc))
                    }
                }
            } else {
                drop(self);
                Err(CondError::NotInit)
            }
        }
    }
}

impl<T: ?Sized> Deref for MutexGuard<'_, T> {
    type Target = T;

    fn deref(&self) -> &T {
        unsafe { &*self.lock.data.get() }
    }
}

impl<T: ?Sized> DerefMut for MutexGuard<'_, T> {
    fn deref_mut(&mut self) -> &mut T {
        unsafe { &mut *self.lock.data.get() }
    }
}

impl<T: ?Sized> Drop for MutexGuard<'_, T> {
    #[inline]
    fn drop(&mut self) {
        unsafe {
            xwrustffi_mtx_unlock(self.lock.mtx.get());
        }
    }
}
