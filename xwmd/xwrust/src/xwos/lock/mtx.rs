//! XWOS RUST：互斥锁
//! ========
//!
//! # 静态互斥锁
//!
//! **静态互斥锁**是指在编译阶段可确定大小的互斥锁变量，即有 [`Sized`] 约束。
//! 构造变量的内存空间在编译期间分配，不依赖于内存分配器。
//! 静态互斥锁常用于定义 [`'static`] 约束的全局共享变量。
//!
//! ## 示例
//!
//! ```rust
//! use xwrust::xwos::lock::mtx::*;
//! static GLOBAL_LOCK: StaticMutex<u32> = StaticMutex::new(0); // 泛型T必须是Sized的，这里以u32为例
//!
//! pub unsafe extern "C" fn xwrust_main() {
//!     // ...省略...
//!     GLOBAL_LOCK.init();
//!     match GLOBAL_LOCK.lock() {
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
//!         .spawn(|_|) {
//!             // 子线程闭包
//!             match GLOBAL_LOCK.lock() {
//!                 Ok(mut guard) => { // 子线程上锁成功
//!                     *guard += 1;
//!                 }
//!                 Err(e) => { // 子线程上锁失败
//!                 }
//!             }
//!         }
//! }
//! ```
//!
//!
//! # 动态互斥锁
//!
//! **动态互斥锁**是指在**运行时**，依赖内存分配器分配内存创建的互斥锁对象。
//! 当内存不足时，内存分配可能会失败，此时创建动态互斥锁失败，将返回**空的**互斥锁对象描述符。
//!
//! 动态互斥锁在编译期不需要知道具体的大小，即具有 [`?Sized`] 约束。
//! 因为动态互斥锁的创建方法不是 **const fn** ，所以不可定义 [`'static`] 约束的全局互斥锁。
//! （不过，可以借助第三方的宏 [`lazy_static!`] 来巧妙定义。）
//!
//! ## 示例
//!
//! ```rust
//! use xwrust::xwos::lock::mtx::*;
//!
//! pub unsafe extern "C" fn xwrust_main() {
//!     // ...省略...
//!     let lock: Arc<DynamicMutex<u32>> = Arc::new(DynamicMutex::new(0));
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
//!         .spawn(|_|) {
//!             // 子线程闭包
//!             match lock_child.lock() {
//!                 Ok(mut guard) => { // 子线程上锁成功
//!                     *guard += 1;
//!                 }
//!                 Err(e) => { // 子线程上锁失败
//!                 }
//!             }
//!         }
//! }
//! ```
//!
//!
//! # 互斥锁对象描述符
//!
//! [对象描述符](https://xwos.tech/docs/user-manual/basic-promise/#对象描述符)是XWOS中用来管理动态对象生命周期的数据结构。
//! 主要用于解决**ABA问题**。
//!
//! 使用XWOS内核接口创建动态互斥锁时，将返回互斥锁对象描述符。
//!
//! [`'static`]: https://doc.rust-lang.org/std/keyword.static.html
//! [`lazy_static!`]: https://crates.io/crates/lazy_static
//! [`Sized`]: https://doc.rust-lang.org/std/marker/trait.Sized.html
//! [`?Sized`]: https://doc.rust-lang.org/std/marker/trait.Sized.html

extern crate core;
use core::ffi::*;
use core::cell::UnsafeCell;
use core::ptr;
use core::result::Result;
use core::default::Default;
use core::ops::Drop;
use core::ops::Deref;
use core::ops::DerefMut;
use core::sync::atomic::*;
use core::marker::PhantomData;
use core::cmp::PartialEq;

use crate::types::*;
use crate::errno::*;


extern "C" {
    fn xwrustffi_mtx_init(mtx: *mut XwosMtxMem) -> XwEr;
    fn xwrustffi_mtx_fini(mtx: *mut XwosMtxMem) -> XwEr;
    fn xwrustffi_mtx_grab(mtx: *mut XwosMtxMem) -> XwEr;
    fn xwrustffi_mtx_put(mtx: *mut XwosMtxMem) -> XwEr;
    fn xwrustffi_mtx_create(mtx: *mut *mut c_void, tik: *mut XwSq) -> XwEr;
    fn xwrustffi_mtx_delete(mtx: *mut c_void, tik: XwSq) -> XwEr;
    fn xwrustffi_mtx_gettik(mtx: *mut c_void) -> XwSq;
    fn xwrustffi_mtx_acquire(mtx: *mut c_void, tik: XwSq) -> XwEr;
    fn xwrustffi_mtx_release(mtx: *mut c_void, tik: XwSq) -> XwEr;
    fn xwrustffi_mtx_unlock(mtx: *mut c_void) -> XwEr;
    fn xwrustffi_mtx_lock(mtx: *mut c_void) -> XwEr;
    fn xwrustffi_mtx_trylock(mtx: *mut c_void) -> XwEr;
    fn xwrustffi_mtx_lock_to(mtx: *mut c_void, to: XwTm) -> XwEr;
    fn xwrustffi_mtx_lock_unintr(mtx: *mut c_void) -> XwEr;
}

/// 互斥锁的错误码
#[derive(Debug)]
pub enum MutexError {
    /// 互斥锁没有初始化
    NotInit,
    /// 空的互斥锁描述符
    NilMtxD,
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

////////////////////////////////////////////////////////////////
// 静态互斥锁
////////////////////////////////////////////////////////////////
/// XWOS互斥锁对象占用的内存大小
pub const SIZEOF_XWOS_MTX: usize = 80;

xwos_struct! {
    /// 用于构建静态互斥锁的内存数组类型
    pub struct XwosMtxMem {
        #[doc(hidden)]
        mem: [u8; SIZEOF_XWOS_MTX],
    }
}

/// 用于构建静态互斥锁的内存数组常量
///
/// 静态互斥锁是编译器在编译阶段就可安排内存空间的互斥锁对象，因此它必须是已知大小的(Sized约束)。
/// 此常量的作用是告诉编译器互斥锁对象需要多大的内存。
pub const XWOS_MTX_INITIALIZER: XwosMtxMem = XwosMtxMem {
    mem: [0; SIZEOF_XWOS_MTX],
};


/// 静态互斥锁对象结构体
pub struct StaticMutex<'a, T: Sized + 'a> {
    /// 用于初始化XWOS互斥锁对象的内存空间
    smtx: UnsafeCell<XwosMtxMem>,
    /// 初始化完成标记
    init: AtomicBool,
    /// 用户数据
    pub data: UnsafeCell<T>,
    /// 0-size的影子数据，用于保留生命周期标记
    phantom: PhantomData<&'a T>,
}

impl<'a, T: Sized + 'a> StaticMutex<'a, T> {
    /// 新建静态互斥锁对象。
    ///
    /// 此方法是编译期方法，可用于新建 [`'static`] 的全局变量。
    ///
    /// 调用此方法时，T必须是在编译期已知大小的类型。
    ///
    /// # 示例
    ///
    /// ```rust
    /// use xwrust::xwos::lock::mtx::*;
    ///
    /// static GLOBAL_MUTEX: StaticMutex<u32>  = StaticMutex::new(0);
    /// ```
    ///
    /// [`'static`]: https://doc.rust-lang.org/std/keyword.static.html
    pub const fn new(t: T) -> Self {
        Self {
            smtx: UnsafeCell::new(XWOS_MTX_INITIALIZER),
            init: AtomicBool::new(false),
            data: UnsafeCell::new(t),
            phantom: PhantomData,
        }
    }

    /// 初始化静态互斥锁对象。
    ///
    /// 静态互斥锁对象必须调用此方法一次，方可正常使用。
    ///
    /// # 示例
    ///
    /// ```rust
    /// use xwrust::xwos::lock::mtx::*;
    ///
    /// static GLOBAL_MUTEX: StaticMutex<u32>  = StaticMutex::new(0);
    ///
    /// pub unsafe extern "C" fn xwrust_main() {
    ///     // ...省略...
    ///     GLOBAL_LOCK.init();
    ///     // 从此处开始 GLOBAL_LOCK 可正常使用
    /// }
    /// ```
    pub fn init(&'a self) {
        match self.init.compare_exchange(false, true,
                                         Ordering::AcqRel, Ordering::Relaxed) {
            Ok(_) => unsafe {
                xwrustffi_mtx_init(self.smtx.get());
            }
            Err(_) => {}
        }
    }

    /// 获取互斥锁，若线程无法获取互斥锁，就阻塞等待，直到能获得锁为止。
    ///
    /// + 若成功获取互斥锁，将返回**RAII Guard**： [`StaticMutexGuard`] ，用于提供 **Scoped Lock** 机制。
    ///   + [`StaticMutexGuard`] 中包含 [`StaticMutex`] 的引用， 当 [`StaticMutexGuard`] 生命周期结束时，会在 [`drop()`] 方法中自动解锁互斥锁。
    /// + 若失败，将返回错误码：
    ///   + 线程的阻塞等待可能被中断，此时，API返回 **-EINTR** ；
    ///
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
    /// static GLOBAL_LOCK: StaticMutex<u32> = StaticMutex::new(0); // 泛型T必须是Sized的，这里以u32为例
    ///
    /// pub unsafe extern "C" fn xwrust_main() {
    ///     // ...省略...
    ///     GLOBAL_LOCK.init();
    ///     match GLOBAL_LOCK.lock() {
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
    pub fn lock(&'a self) -> Result<StaticMutexGuard<'a, T>, MutexError> {
        if self.init.load(Ordering::Acquire) {
            unsafe {
                let rc = xwrustffi_mtx_lock(self.smtx.get() as _);
                if 0 == rc {
                    Ok(StaticMutexGuard::new(self))
                } else if -EINTR == rc {
                    Err(MutexError::Interrupt)
                } else if -ENOTTHDCTX == rc {
                    Err(MutexError::NotThreadContext)
                } else {
                    Err(MutexError::Unknown(rc))
                }
            }
        } else {
            Err(MutexError::NotInit)
        }
    }

    /// 尝试获取互斥锁，若线程无法获取互斥锁，立即返回错误。
    ///
    /// + 若成功获取互斥锁，将返回**RAII Guard**： [`StaticMutexGuard`] ，用于提供 **Scoped Lock** 机制。
    ///   + [`StaticMutexGuard`] 中包含 [`StaticMutex`] 的引用， 当 [`StaticMutexGuard`] 生命周期结束时，会在 [`drop()`] 方法中自动解锁互斥锁。
    /// + 若失败，将返回错误码： **-ENODATA** 。
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
    /// static GLOBAL_LOCK: StaticMutex<u32> = StaticMutex::new(0); // 泛型T必须是Sized的，这里以u32为例
    ///
    /// pub unsafe extern "C" fn xwrust_main() {
    ///     // ...省略...
    ///     GLOBAL_LOCK.init();
    ///     match GLOBAL_LOCK.trylock() {
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
    pub fn trylock(&'a self) -> Result<StaticMutexGuard<'a, T>, MutexError> {
        if self.init.load(Ordering::Acquire) {
            unsafe {
                let rc = xwrustffi_mtx_trylock(self.smtx.get() as _);
                if 0 == rc {
                    Ok(StaticMutexGuard::new(self))
                } else if -EWOULDBLOCK == rc {
                    Err(MutexError::WouldBlock)
                } else if -ENOTTHDCTX == rc {
                    Err(MutexError::NotThreadContext)
                } else {
                    Err(MutexError::Unknown(rc))
                }
            }
        } else {
            Err(MutexError::NotInit)
        }
    }

    /// 获取互斥锁，若线程无法获取互斥锁，就限时阻塞等待。
    ///
    /// + 若成功获取互斥锁，将返回**RAII Guard**： [`StaticMutexGuard`] ，用于提供 **Scoped Lock** 机制。
    ///   + [`StaticMutexGuard`] 中包含 [`StaticMutex`] 的引用， 当 [`StaticMutexGuard`] 生命周期结束时，会在 [`drop()`] 方法中自动解锁互斥锁。
    /// + 若失败，将返回错误码：
    ///   + 线程的阻塞等待可能被中断，此时，API返回 **-EINTR** ；
    ///   + 线程的阻塞等待可能超时，此时，API返回 **-ETIMEDOUT** 。
    ///
    /// # 错误码
    ///
    /// + [`MutexError::NotInit`] 互斥锁未被初始化
    /// + [`MutexError::Interrupt`] 等待被中断
    /// + [`MutexError::Timedout`] 等待被中断
    /// + [`MutexError::NotThreadContext`] 不在线程上下文中
    ///
    /// # 示例
    /// ```rust
    /// use xwrust::xwtm;
    /// use xwrust::xwos::lock::mtx::*;
    /// static GLOBAL_LOCK: StaticMutex<u32> = StaticMutex::new(0); // 泛型T必须是Sized的，这里以u32为例
    ///
    /// pub unsafe extern "C" fn xwrust_main() {
    ///     // ...省略...
    ///     GLOBAL_LOCK.init();
    ///     match GLOBAL_LOCK.lock_to(xwtm::ft(xwtm::s(10))) { // 最多等待10s
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
    pub fn lock_to(&'a self, to: XwTm) -> Result<StaticMutexGuard<'a, T>, MutexError> {
        if self.init.load(Ordering::Acquire) {
            unsafe {
                let rc = xwrustffi_mtx_lock_to(self.smtx.get() as _, to);
                if 0 == rc {
                    Ok(StaticMutexGuard::new(self))
                } else if -EINTR == rc {
                    Err(MutexError::Interrupt)
                } else if -ETIMEDOUT == rc {
                    Err(MutexError::Timedout)
                } else if -ENOTTHDCTX == rc {
                    Err(MutexError::NotThreadContext)
                } else {
                    Err(MutexError::Unknown(rc))
                }
            }
        } else {
            Err(MutexError::NotInit)
        }
    }

    /// 获取互斥锁，若线程无法获取互斥锁，就阻塞等待，且不可中断，直到能获得锁为止。
    ///
    /// + 若成功获取互斥锁，将返回**RAII Guard**： [`StaticMutexGuard`] ，用于提供 **Scoped Lock** 机制；
    /// + 若失败，将返回错误码。
    ///
    /// [`StaticMutexGuard`] 中包含 [`StaticMutex`] 的引用， 当 [`StaticMutexGuard`] 生命周期结束时，会在 [`drop()`] 方法中自动解锁互斥锁。
    ///
    /// # 错误码
    ///
    /// + [`MutexError::NotInit`] 互斥锁未被初始化
    /// + [`MutexError::NotThreadContext`] 不在线程上下文中
    ///
    /// # 示例
    /// ```rust
    /// use xwrust::xwos::lock::mtx::*;
    /// static GLOBAL_LOCK: StaticMutex<u32> = StaticMutex::new(0); // 泛型T必须是Sized的，这里以u32为例
    ///
    /// pub unsafe extern "C" fn xwrust_main() {
    ///     // ...省略...
    ///     GLOBAL_LOCK.init();
    ///     match GLOBAL_LOCK.lock_unintr() {
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
    pub fn lock_unintr(&'a self) -> Result<StaticMutexGuard<'a, T>, MutexError> {
        if self.init.load(Ordering::Acquire) {
            unsafe {
                let rc = xwrustffi_mtx_lock_unintr(self.smtx.get() as _);
                if 0 == rc {
                    Ok(StaticMutexGuard::new(self))
                } else if -ENOTTHDCTX == rc {
                    Err(MutexError::NotThreadContext)
                } else {
                    Err(MutexError::Unknown(rc))
                }
            }
        } else {
            Err(MutexError::NotInit)
        }
    }

    /// 释放 [`StaticMutexGuard`]，并在 [`drop()`] 方法中解锁互斥锁。
    ///
    /// # 示例
    ///
    /// ```rust
    /// use xwrust::xwos::lock::mtx::*;
    /// static GLOBAL_LOCK: StaticMutex<u32> = StaticMutex::new(0); // 泛型T必须是Sized的，这里以u32为例
    ///
    /// pub unsafe extern "C" fn xwrust_main() {
    ///     // ...省略...
    ///     GLOBAL_LOCK.init();
    ///     match GLOBAL_LOCK.lock() {
    ///         Ok(mut guard) => { // 上锁成功
    ///             *guard = 1; // 访问共享变量
    ///             StaticMutex::unlock(guard); // 主动解锁
    ///         }
    ///         Err(e) => {
    ///             // 上锁失败
    ///         }
    ///     }
    /// }
    /// ```
    ///
    /// [`drop()`]: https://doc.rust-lang.org/std/mem/fn.drop.html
    pub fn unlock(guard: StaticMutexGuard<'a, T>) {
        drop(guard)
    }
}

// 定义静态生命周期的全局互斥锁只是静态互斥锁的主要用途，但不是唯一用途。当定义的是局部静态互斥锁时，
// 其应该是 [`Send`] + [`Sync`] 的。
unsafe impl<'a, T: Send + 'a> Send for StaticMutex<'a, T> {}
unsafe impl<'a, T: Send + 'a> Sync for StaticMutex<'a, T> {}

impl<'a, T: Sized + 'a> From<T> for StaticMutex<'a, T> {
    /// 从数据新建静态互斥锁对象
    ///
    /// 此方法会将数据所有权转移到静态互斥锁对象的内部
    ///
    /// 等价于 [`StaticMutex::new`]
    fn from(t: T) -> Self {
        StaticMutex::new(t)
    }
}

impl<'a, T: Default + 'a> Default for StaticMutex<'a, T> {
    fn default() -> StaticMutex<'a, T> {
        StaticMutex::new(Default::default())
    }
}

impl<T> Drop for StaticMutex<'_, T> {
    fn drop(&mut self) {
        unsafe {
            xwrustffi_mtx_fini(self.smtx.get());
        }
    }
}

/// 静态互斥锁对象的RAII Guard
///
/// **RAII Guard** 用于提供 **Scoped Lock** 机制。
///
/// + [`StaticMutexGuard`] 中包含 [`StaticMutex`] 的引用， 当 [`StaticMutexGuard`] 生命周期结束时，会在 [`drop()`] 方法中自动解锁互斥锁。
/// + [`StaticMutexGuard`] 不可在线程之间转移所有权，因为其 [`drop()`] 方法包含解锁的语义，上锁和解锁必须在同一线程；
/// + [`StaticMutexGuard`] 虽然可以在多线程中传递引用( [`Sync`] 约束)，但其实现中没有 **unlock()** 方法，意味着其他线程即便拿到引用也不能解锁。
///
/// [`drop()`]: https://doc.rust-lang.org/std/mem/fn.drop.html
/// [`Sync`]: https://doc.rust-lang.org/std/marker/trait.Send.html
pub struct StaticMutexGuard<'a, T: Sized + 'a> {
    /// 静态互斥锁的引用
    lock: &'a StaticMutex<'a, T>,
}

impl<T> !Send for StaticMutexGuard<'_, T> {}
unsafe impl<'a, T: Sync> Sync for StaticMutexGuard<'a, T> {}

impl<'a, T> StaticMutexGuard<'a, T> {
    fn new(lock: &'a StaticMutex<T>) -> StaticMutexGuard<'a, T> {
        StaticMutexGuard { lock: lock }
    }
}

impl<T> Deref for StaticMutexGuard<'_, T> {
    type Target = T;

    fn deref(&self) -> &T {
        unsafe { &*self.lock.data.get() }
    }
}

impl<T> DerefMut for StaticMutexGuard<'_, T> {
    fn deref_mut(&mut self) -> &mut T {
        unsafe { &mut *self.lock.data.get() }
    }
}

impl<T> Drop for StaticMutexGuard<'_, T> {
    #[inline]
    fn drop(&mut self) {
        unsafe {
            xwrustffi_mtx_unlock(self.lock.smtx.get() as _);
        }
    }
}


////////////////////////////////////////////////////////////////
// 动态互斥锁
////////////////////////////////////////////////////////////////

/// XWOS的互斥锁对象描述符
pub struct MtxD {
    /// XWOS互斥锁对象的指针
    mtx: *mut c_void,
    /// XWOS互斥锁对象的标签
    tik: XwSq,
}

unsafe impl Send for MtxD {}
unsafe impl Sync for MtxD {}

impl MtxD {
    fn new() -> MtxD {
        unsafe {
            let mut mtx: *mut c_void = ptr::null_mut();
            let mut tik: XwSq = 0;
            let rc = xwrustffi_mtx_create(&mut mtx, &mut tik);
            if rc < 0 {
                MtxD {mtx: ptr::null_mut(), tik: 0}
            } else {
                MtxD {mtx: mtx, tik: tik}
            }
        }
    }

    fn unlock(&self) -> XwEr {
        unsafe {
            let rc = xwrustffi_mtx_acquire(self.mtx, self.tik);
            if rc == 0 {
                let rc = xwrustffi_mtx_unlock(self.mtx);
                xwrustffi_mtx_put(self.mtx as _);
                rc
            } else {
                rc
            }
        }
    }

    fn lock(&self) -> XwEr {
        unsafe {
            let rc = xwrustffi_mtx_acquire(self.mtx, self.tik);
            if rc == 0 {
                let rc = xwrustffi_mtx_lock(self.mtx);
                xwrustffi_mtx_put(self.mtx as _);
                rc
            } else {
                rc
            }
        }
    }

    fn trylock(&self) -> XwEr {
        unsafe {
            let rc = xwrustffi_mtx_acquire(self.mtx, self.tik);
            if rc == 0 {
                let rc = xwrustffi_mtx_trylock(self.mtx);
                xwrustffi_mtx_put(self.mtx as _);
                rc
            } else {
                rc
            }
        }
    }

    fn lock_to(&self, to: XwTm) -> XwEr {
        unsafe {
            let rc = xwrustffi_mtx_acquire(self.mtx, self.tik);
            if rc == 0 {
                let rc = xwrustffi_mtx_lock_to(self.mtx, to);
                xwrustffi_mtx_put(self.mtx as _);
                rc
            } else {
                rc
            }
        }
    }

    fn lock_unintr(&self) -> XwEr {
        unsafe {
            let rc = xwrustffi_mtx_acquire(self.mtx, self.tik);
            if rc == 0 {
                let rc = xwrustffi_mtx_lock_unintr(self.mtx);
                xwrustffi_mtx_put(self.mtx as _);
                rc
            } else {
                rc
            }
        }
    }
}

impl PartialEq for MtxD {
    fn eq(&self, other: &Self) -> bool {
        (self.mtx == other.mtx) && (self.tik == other.tik)
    }
}

impl Default for MtxD {
    fn default() -> Self {
        MtxD {
            mtx: ptr::null_mut(),
            tik: 0
        }
    }
}

impl Drop for MtxD {
    fn drop(&mut self) {
        unsafe {
            xwrustffi_mtx_delete(self.mtx, self.tik);
        }
    }
}

/// 动态互斥锁结构体
pub struct DynamicMutex<T: ?Sized> {
    /// 互斥锁对象描述符
    mtxd: MtxD,
    /// 用户数据
    pub data: UnsafeCell<T>,
}

impl<T> DynamicMutex<T> {
    /// 新建动态互斥锁对象。
    ///
    /// # 示例
    ///
    /// ```rust
    /// use xwrust::xwos::thd;
    /// use xwrust::xwos::lock::mtx::*;
    ///
    /// pub unsafe extern "C" fn xwrust_main() {
    ///     // ...省略...
    ///     let lock: Arc<DynamicMutex<u32>> = Arc::new(DynamicMutex::new(0));
    ///     let lock_c = lock.clone();
    ///     thd::spawn(move |_| {
    ///         // 子线程
    ///         match lock_c.lock() { // lock_c移动到子线程
    ///             Ok(mut guard) => { // 上锁成功
    ///                 *guard = 1; // 访问共享变量
    ///             } // guard 生命周期结束，自动解锁
    ///             Err(e) => {
    ///                 // 上锁失败
    ///             }
    ///         }
    ///     });
    ///     // ...省略...
    /// }
    /// ```
    pub fn new(t: T) -> DynamicMutex<T> {
        DynamicMutex {
            mtxd: MtxD::new(),
            data: UnsafeCell::new(t),
        }
    }
}

unsafe impl<T: ?Sized + Send> Send for DynamicMutex<T> {}
unsafe impl<T: ?Sized + Send> Sync for DynamicMutex<T> {}

impl<T: ?Sized> DynamicMutex<T> {
    /// 获取互斥锁，若线程无法获取互斥锁，就阻塞等待，直到能获得锁为止。
    ///
    /// + 若成功获取互斥锁，将返回**RAII Guard**： [`DynamicMutexGuard`] ，用于提供 **Scoped Lock** 机制。
    ///   + [`DynamicMutexGuard`] 中包含 [`DynamicMutex`] 的引用， 当 [`DynamicMutexGuard`] 生命周期结束时，会在 [`drop()`] 方法中自动解锁互斥锁。
    /// + 若失败，将返回错误码：
    ///   + 线程的阻塞等待可能被中断，此时，API返回 **-EINTR** ；
    ///
    ///
    /// # 错误码
    ///
    /// + [`MutexError::NilMtxD`] 空的互斥锁描述符
    /// + [`MutexError::Interrupt`] 等待被中断
    /// + [`MutexError::NotThreadContext`] 不在线程上下文中
    ///
    /// # 示例
    /// ```rust
    /// use xwrust::xwos::lock::mtx::*;
    ///
    /// pub unsafe extern "C" fn xwrust_main() {
    ///     // ...省略...
    ///     let lock: Arc<DynamicMutex<u32>> = Arc::new(DynamicMutex::new(0));
    ///     match lock.lock() {
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
    pub fn lock(&self) -> Result<DynamicMutexGuard<'_, T>, MutexError> {
        let rc = self.mtxd.lock();
        if rc == 0 {
            Ok(DynamicMutexGuard::new(self))
        } else if -ENILOBJD == rc {
            Err(MutexError::NilMtxD)
        } else if -EINTR == rc {
            Err(MutexError::Interrupt)
        } else if -ENOTTHDCTX == rc {
            Err(MutexError::NotThreadContext)
        } else {
            Err(MutexError::Unknown(rc))
        }
    }

    /// 尝试获取互斥锁，若线程无法获取互斥锁，立即返回错误。
    ///
    /// + 若成功获取互斥锁，将返回**RAII Guard**： [`DynamicMutexGuard`] ，用于提供 **Scoped Lock** 机制。
    ///   + [`DynamicMutexGuard`] 中包含 [`DynamicMutex`] 的引用， 当 [`DynamicMutexGuard`] 生命周期结束时，会在 [`drop()`] 方法中自动解锁互斥锁。
    /// + 若失败，将返回错误码： **-ENODATA** 。
    ///
    /// # 错误码
    ///
    /// + [`MutexError::NilMtxD`] 空的互斥锁描述符
    /// + [`MutexError::WouldBlock`] 尝试获取锁失败
    /// + [`MutexError::NotThreadContext`] 不在线程上下文中
    ///
    /// # 示例
    /// ```rust
    /// use xwrust::xwos::lock::mtx::*;
    ///
    /// pub unsafe extern "C" fn xwrust_main() {
    ///     // ...省略...
    ///     let lock: Arc<DynamicMutex<u32>> = Arc::new(DynamicMutex::new(0));
    ///     match lock.trylock() {
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
    pub fn trylock(&self) -> Result<DynamicMutexGuard<'_, T>, MutexError> {
        let rc = self.mtxd.trylock();
        if rc == 0 {
            Ok(DynamicMutexGuard::new(self))
        } else if -ENILOBJD == rc {
            Err(MutexError::NilMtxD)
        } else if -EWOULDBLOCK == rc {
            Err(MutexError::WouldBlock)
        } else if -ENOTTHDCTX == rc {
            Err(MutexError::NotThreadContext)
        } else {
            Err(MutexError::Unknown(rc))
        }
    }

    /// 获取互斥锁，若线程无法获取互斥锁，就限时阻塞等待。
    ///
    /// + 若成功获取互斥锁，将返回**RAII Guard**： [`DynamicMutexGuard`] ，用于提供 **Scoped Lock** 机制。
    ///   + [`DynamicMutexGuard`] 中包含 [`DynamicMutex`] 的引用， 当 [`DynamicMutexGuard`] 生命周期结束时，会在 [`drop()`] 方法中自动解锁互斥锁。
    /// + 若失败，将返回错误码：
    ///   + 线程的阻塞等待可能被中断，此时，API返回 **-EINTR** ；
    ///   + 线程的阻塞等待可能超时，此时，API返回 **-ETIMEDOUT** 。
    ///
    /// # 错误码
    ///
    /// + [`MutexError::NilMtxD`] 空的互斥锁描述符
    /// + [`MutexError::Interrupt`] 等待被中断
    /// + [`MutexError::Timedout`] 等待被中断
    /// + [`MutexError::NotThreadContext`] 不在线程上下文中
    ///
    /// # 示例
    /// ```rust
    /// use xwrust::xwtm;
    /// use xwrust::xwos::lock::mtx::*;
    ///
    /// pub unsafe extern "C" fn xwrust_main() {
    ///     // ...省略...
    ///     let lock: Arc<DynamicMutex<u32>> = Arc::new(DynamicMutex::new(0));
    ///     match lock.lock_to(xwtm::ft(xwtm::s(10))) { // 最多等待10s
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
    pub fn lock_to(&self, to: XwTm) -> Result<DynamicMutexGuard<'_, T>, MutexError> {
        let rc = self.mtxd.lock_to(to);
        if rc == 0 {
            Ok(DynamicMutexGuard::new(self))
        } else if -ENILOBJD == rc {
            Err(MutexError::NilMtxD)
        } else if -EINTR == rc {
            Err(MutexError::Interrupt)
        } else if -ETIMEDOUT == rc {
            Err(MutexError::Timedout)
        } else if -ENOTTHDCTX == rc {
            Err(MutexError::NotThreadContext)
        } else {
            Err(MutexError::Unknown(rc))
        }
    }

    /// 获取互斥锁，若线程无法获取互斥锁，就阻塞等待，且不可中断，直到能获得锁为止。
    ///
    /// + 若成功获取互斥锁，将返回**RAII Guard**： [`DynamicMutexGuard`] ，用于提供 **Scoped Lock** 机制；
    /// + 若失败，将返回错误码。
    ///
    /// [`DynamicMutexGuard`] 中包含 [`DynamicMutex`] 的引用， 当 [`DynamicMutexGuard`] 生命周期结束时，会在 [`drop()`] 方法中自动解锁互斥锁。
    ///
    /// # 错误码
    ///
    /// + [`MutexError::NilMtxD`] 空的互斥锁描述符
    /// + [`MutexError::NotThreadContext`] 不在线程上下文中
    ///
    /// # 示例
    /// ```rust
    /// use xwrust::xwos::lock::mtx::*;
    ///
    /// pub unsafe extern "C" fn xwrust_main() {
    ///     // ...省略...
    ///     let lock: Arc<DynamicMutex<u32>> = Arc::new(DynamicMutex::new(0));
    ///     match lock.lock_unintr() {
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
    pub fn lock_unintr(&self) -> Result<DynamicMutexGuard<'_, T>, MutexError> {
        let rc = self.mtxd.lock_unintr();
        if rc == 0 {
            Ok(DynamicMutexGuard::new(self))
        } else if -ENILOBJD == rc {
            Err(MutexError::NilMtxD)
        } else if -ENOTTHDCTX == rc {
            Err(MutexError::NotThreadContext)
        } else {
            Err(MutexError::Unknown(rc))
        }
    }

    /// 释放 [`DynamicMutexGuard`]，并在 [`drop()`] 方法中解锁互斥锁。
    ///
    /// # 示例
    ///
    /// ```rust
    /// use xwrust::xwos::lock::mtx::*;
    ///
    /// pub unsafe extern "C" fn xwrust_main() {
    ///     // ...省略...
    ///     let lock: Arc<DynamicMutex<u32>> = Arc::new(DynamicMutex::new(0));
    ///     match lock.lock() {
    ///         Ok(mut guard) => { // 上锁成功
    ///             *guard = 1; // 访问共享变量
    ///             DynamicMutex::unlock(guard); // 主动解锁
    ///         }
    ///         Err(e) => {
    ///             // 上锁失败
    ///         }
    ///     }
    /// }
    /// ```
    ///
    /// [`drop()`]: https://doc.rust-lang.org/std/mem/fn.drop.html
    pub fn unlock(guard: DynamicMutexGuard<'_, T>) {
        drop(guard);
    }

    pub fn into_inner(self) -> T
    where
        T: Sized,
    {
        self.data.into_inner()
    }

    pub fn get_mut(&mut self) -> &mut T {
        self.data.get_mut()
    }
}

impl<T: ?Sized + Default> From<T> for DynamicMutex<T> {
    /// 从数据新建互斥锁对象
    ///
    /// 等价于 [`DynamicMutex::new`] 。
    fn from(t: T) -> Self {
        DynamicMutex::new(t)
    }
}

impl<T: ?Sized + Default> Default for DynamicMutex<T> {
    fn default() -> DynamicMutex<T> {
        DynamicMutex::new(Default::default())
    }
}

/// 动态互斥锁对象的RAII Guard
///
/// **RAII Guard** 用于提供 **Scoped Lock** 机制。
///
/// + [`DynamicMutexGuard`] 中包含 [`DynamicMutex`] 的引用， 当 [`DynamicMutexGuard`] 生命周期结束时，会在 [`drop()`] 方法中自动解锁互斥锁。
/// + [`DynamicMutexGuard`] 不可在线程之间转移所有权，因为其 [`drop()`] 方法包含解锁的语义，上锁和解锁必须在同一线程；
/// + [`DynamicMutexGuard`] 虽然可以在多线程中传递引用( [`Sync`] 约束)，但其实现中没有 **unlock()** 方法，意味着其他线程即便拿到引用也不能解锁。
///
/// [`drop()`]: https://doc.rust-lang.org/std/mem/fn.drop.html
/// [`Sync`]: https://doc.rust-lang.org/std/marker/trait.Send.html
pub struct DynamicMutexGuard<'a, T: ?Sized + 'a> {
    lock: &'a DynamicMutex<T>,
}

impl<T: ?Sized> !Send for DynamicMutexGuard<'_, T> {}
unsafe impl<T: ?Sized + Sync> Sync for DynamicMutexGuard<'_, T> {}

impl<'a, T: ?Sized> DynamicMutexGuard<'a, T> {
    fn new(lock: &'a DynamicMutex<T>) -> DynamicMutexGuard<'a, T> {
        DynamicMutexGuard { lock: lock }
    }
}

impl<T: ?Sized> Deref for DynamicMutexGuard<'_, T> {
    type Target = T;

    fn deref(&self) -> &T {
        unsafe { &*self.lock.data.get() }
    }
}

impl<T: ?Sized> DerefMut for DynamicMutexGuard<'_, T> {
    fn deref_mut(&mut self) -> &mut T {
        unsafe { &mut *self.lock.data.get() }
    }
}

impl<T: ?Sized> Drop for DynamicMutexGuard<'_, T> {
    #[inline]
    fn drop(&mut self) {
        self.lock.mtxd.unlock();
    }
}
