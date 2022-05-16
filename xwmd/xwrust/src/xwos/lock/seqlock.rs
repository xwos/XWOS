//! XWOS RUST：顺序锁
//! ========
//!
//! 顺序锁是对自旋锁改良后的锁，顺序锁中包含一个自旋锁，并且带有一个顺序值。
//! 顺序锁将临界区分为三种：
//!
//! #### 独占写
//!
//! 任何对顺序锁临界区的 **写** 操作都是独占的，每次 **写** 操作时，会先上锁自旋锁，
//! 然后增加顺序值，离开临界区时也会再次增加顺序值。言外之意，顺序值增加两次，顺序值增加的次数是 **偶数** 。
//!
//! 锁的模式：
//!
//! + 当临界区只被多个线程访问时，需使用关闭抢占的模式： [`SeqlockMode::WriteLock`]
//! + 当临界区被中断底半部访问时，需使用关闭中断底半部的模式： [`SeqlockMode::WriteLockBh`]
//! + 当临界区被多个线程和单一中断访问时，需使用关闭中断的模式： [`SeqlockMode::WriteLockCpuirq`]
//! + 当临界区被多个中断上下文访问时，需使用保存中断标志的模式： [`SeqlockMode::WriteLockCpuirqSave(None)`]
//!
//! #### 非独占读
//!
//! 如果多个CPU进行的是 **只读** 操作，它们可以同时进入 **非独占读临界区** 。
//! CPU进入非独占读临界区无需获得自旋锁，但需要先测试顺序值是否为 **偶数**，并记录此时的顺序值。
//! 当退出读临界区时，需要再次读取顺序值，并与之前记录的值进行比较：
//!
//! + 如果相等，表示读的结果有效；
//! + 如果不相等，则表示读的过程中别的CPU进行了写操作，此次的读操作无效。
//!
//! #### 独占读
//!
//! 如果希望读临界区不会被写操作无效掉，可以使用独占读的方式，
//! 独占读会排斥其他CPU上的独占写和独占读操作，但不会排斥非独占读，
//! 其他CPU依然可以进入非独占读临界区。
//!
//! 锁的模式 ：
//!
//! + 当临界区只被多个线程访问时，需使用关闭抢占的模式： [`SeqlockMode::ReadExclusiveLock`]
//! + 当临界区被中断底半部访问时，需使用关闭中断底半部的模式： [`SeqlockMode::ReadExclusiveLockBh`]
//! + 当临界区被多个线程和单一中断访问时，需使用关闭中断的模式： [`SeqlockMode::ReadExclusiveLockCpuirq`]
//! + 当临界区被多个中断上下文访问时，需使用保存中断标志的模式： [`SeqlockMode::ReadExclusiveLockCpuirqSave(None)`]
//!
//!
//! # 创建
//!
//! XWOS RUST的互斥锁可使用 [`Seqlock::new()`] 创建。
//!
//! + 可以创建具有静态生命周期 [`'static`] 约束的全局变量：
//!
//! ```rust
//! use xwrust::xwos::lock::seqlock::*;
//!
//! static GLOBAL_SEQLOCK: Seqlock<u32> = Seqlock::new(0);
//! ```
//!
//! + 也可以使用 [`alloc::sync::Arc`] 在heap中创建：
//!
//! ```rust
//! extern crate alloc;
//! use alloc::sync::Arc;
//!
//! use xwrust::xwos::lock::seqlock::*;
//!
//! pub fn xwrust_example_seqlock() {
//!     let seqlock: Arc<Seqlock<u32>> = Arc::new(Seqlock::new(0));
//! }
//! ```
//!
//!
//! # 初始化
//!
//! 无论以何种方式创建的自旋锁，都必须在使用前调用 [`Seqlock::init()`] 进行初始化：
//!
//! ```rust
//! pub fn xwrust_example_seqlock() {
//!     GLOBAL_SEQLOCK.init();
//!     seqlock.init();
//! }
//! ```
//!
//!
//! # 解锁
//!
//! 上锁后返回的 [`SeqlockGuard`] 。 [`SeqlockGuard`] 的生命周期结束时，会自动解锁。
//! 也可调用 [`Seqlock::unlock()`] 主动消费掉 [`SeqlockGuard`] 来解锁。
//!
//!
//! # 上锁
//!
//! 上锁的方法 [`Seqlock::lock()`] 除了 **自旋** 等待，改变顺序值外，还会根据上锁模式增加额外操作：
//!
//! + 只关闭抢占的模式： [`SeqlockMode::WriteLock`] 以及 [`SeqlockMode::ReadExclusiveLock`]
//! + 关闭中断底半部的模式： [`SeqlockMode::WriteLockBh`] 以及 [`SeqlockMode::ReadExclusiveLockBh`]
//! + 关闭全局中断的模式： [`SeqlockMode::WriteLockCpuirq`] 以及 [`SeqlockMode::ReadExclusiveLockCpuirq`]
//! + 保存中断全局标志并关闭全局中断的模式： [`SeqlockMode::WriteLockCpuirqSave(None)`] 以及 [`SeqlockMode::ReadExclusiveLockCpuirqSave(None)`]
//!
//!
//! # 尝试上锁
//!
//! 尝试上锁的方法 [`Seqlock::trylock()`] 只会测试一下锁，不会 **自旋** 等待，上锁成功后还会根据上锁模式增加额外操作：
//!
//! + 只关闭抢占的模式： [`SeqlockMode::WriteLock`] 以及 [`SeqlockMode::ReadExclusiveLock`]
//! + 关闭中断底半部的模式： [`SeqlockMode::WriteLockBh`] 以及 [`SeqlockMode::ReadExclusiveLockBh`]
//! + 关闭全局中断的模式： [`SeqlockMode::WriteLockCpuirq`] 以及 [`SeqlockMode::ReadExclusiveLockCpuirq`]
//! + 保存中断全局标志并关闭全局中断的模式： [`SeqlockMode::WriteLockCpuirqSave(None)`] 以及 [`SeqlockMode::ReadExclusiveLockCpuirqSave(None)`]
//!
//!
//! # 示例
//!
//! [XWOS/xwam/xwrust-example/xwrust_example_seqlock](https://gitee.com/xwos/XWOS/blob/main/xwam/xwrust-example/xwrust_example_seqlock/src/lib.rs)
//!
//! [`SeqlockMode::WriteLockCpuirqSave(None)`]: SeqlockMode::WriteLockCpuirqSave
//! [`SeqlockMode::ReadExclusiveLockCpuirqSave(None)`]: SeqlockMode::ReadExclusiveLockCpuirqSave
//! [`'static`]: https://doc.rust-lang.org/std/keyword.static.html
//! [`alloc::sync::Arc`]: <https://doc.rust-lang.org/alloc/sync/struct.Arc.html>

extern crate core;
use core::cell::UnsafeCell;
use core::result::Result;
use core::default::Default;
use core::option::Option;
use core::ops::Drop;
use core::ops::Deref;
use core::ops::DerefMut;
use core::sync::atomic::*;
use core::ptr;

use crate::types::*;
use crate::errno::*;

use crate::xwos::sync::cond::*;

extern "C" {
    fn xwrustffi_sqlk_init(sqlk: *mut XwosSqlk);

    fn xwrustffi_sqlk_rd_begin(sqlk: *mut XwosSqlk) -> XwSq;
    fn xwrustffi_sqlk_rd_retry(sqlk: *mut XwosSqlk, start: XwSq) -> bool;

    fn xwrustffi_sqlk_rdex_unlock(sqlk: *mut XwosSqlk);
    fn xwrustffi_sqlk_rdex_lock(sqlk: *mut XwosSqlk);
    fn xwrustffi_sqlk_rdex_trylock(sqlk: *mut XwosSqlk) -> XwEr;
    fn xwrustffi_sqlk_rdex_unlock_cpuirq(sqlk: *mut XwosSqlk);
    fn xwrustffi_sqlk_rdex_lock_cpuirq(sqlk: *mut XwosSqlk);
    fn xwrustffi_sqlk_rdex_trylock_cpuirq(sqlk: *mut XwosSqlk) -> XwEr;
    fn xwrustffi_sqlk_rdex_unlock_cpuirqrs(sqlk: *mut XwosSqlk, cpuirq: XwReg);
    fn xwrustffi_sqlk_rdex_lock_cpuirqsv(sqlk: *mut XwosSqlk, cpuirq: *mut XwReg);
    fn xwrustffi_sqlk_rdex_trylock_cpuirqsv(sqlk: *mut XwosSqlk, cpuirq: *mut XwReg) -> XwEr;
    fn xwrustffi_sqlk_rdex_unlock_bh(sqlk: *mut XwosSqlk);
    fn xwrustffi_sqlk_rdex_lock_bh(sqlk: *mut XwosSqlk);
    fn xwrustffi_sqlk_rdex_trylock_bh(sqlk: *mut XwosSqlk) -> XwEr;

    fn xwrustffi_sqlk_wr_unlock(sqlk: *mut XwosSqlk);
    fn xwrustffi_sqlk_wr_lock(sqlk: *mut XwosSqlk);
    fn xwrustffi_sqlk_wr_trylock(sqlk: *mut XwosSqlk) -> XwEr;
    fn xwrustffi_sqlk_wr_unlock_cpuirq(sqlk: *mut XwosSqlk);
    fn xwrustffi_sqlk_wr_lock_cpuirq(sqlk: *mut XwosSqlk);
    fn xwrustffi_sqlk_wr_trylock_cpuirq(sqlk: *mut XwosSqlk) -> XwEr;
    fn xwrustffi_sqlk_wr_unlock_cpuirqrs(sqlk: *mut XwosSqlk, cpuirq: XwReg);
    fn xwrustffi_sqlk_wr_lock_cpuirqsv(sqlk: *mut XwosSqlk, cpuirq: *mut XwReg);
    fn xwrustffi_sqlk_wr_trylock_cpuirqsv(sqlk: *mut XwosSqlk, cpuirq: *mut XwReg) -> XwEr;
    fn xwrustffi_sqlk_wr_unlock_bh(sqlk: *mut XwosSqlk);
    fn xwrustffi_sqlk_wr_lock_bh(sqlk: *mut XwosSqlk);
    fn xwrustffi_sqlk_wr_trylock_bh(sqlk: *mut XwosSqlk) -> XwEr;
}

/// 顺序锁的错误码
#[derive(Debug)]
pub enum SeqlockError {
    /// 顺序锁没有初始化
    NotInit,
    /// 尝试上锁失败
    Again,
    /// 未知错误
    Unknown(XwEr),
}

/// 顺序锁的上锁模式
pub enum SeqlockMode {
    /// 写，关闭抢占
    WriteLock,
    /// 写，关闭抢占、中断底半部
    WriteLockBh,
    /// 写，关闭抢占、中断底半部和中断
    WriteLockCpuirq,
    /// 写，关闭抢占、中断底半部和中断，并保存之前的中断标志
    WriteLockCpuirqSave(Option<XwReg>),
    /// 独占读，关闭抢占
    ReadExclusiveLock,
    /// 独占读，关闭抢占、中断底半部
    ReadExclusiveLockBh,
    /// 独占读，关闭抢占、中断底半部和中断
    ReadExclusiveLockCpuirq,
    /// 独占读，关闭抢占、中断底半部和中断，并保存之前的中断标志
    ReadExclusiveLockCpuirqSave(Option<XwReg>),
}

////////////////////////////////////////////////////////////////
// 顺序锁
////////////////////////////////////////////////////////////////
/// XWOS顺序锁占用的内存大小
pub const SIZEOF_XWOS_SQLK: usize = 16;

xwos_struct! {
    /// 用于构建顺序锁的内存数组类型
    pub struct XwosSqlk {
        #[doc(hidden)]
        obj: [u8; SIZEOF_XWOS_SQLK],
    }
}

/// 用于构建顺序锁的内存数组常量
///
/// 此常量的作用是告诉编译器顺序锁需要多大的内存。
pub const XWOS_SQLK_INITIALIZER: XwosSqlk = XwosSqlk {
    obj: [0; SIZEOF_XWOS_SQLK],
};


/// 顺序锁结构体
pub struct Seqlock<T: ?Sized> {
    /// 用于初始化XWOS顺序锁的内存空间
    sqlk: UnsafeCell<XwosSqlk>,
    /// 初始化完成标记
    init: AtomicBool,
    /// 上锁方式
    mode: UnsafeCell<SeqlockMode>,
    /// 用户数据
    pub data: UnsafeCell<T>,
}

impl<T> Seqlock<T> {
    /// 新建顺序锁。
    ///
    /// 此方法是编译期方法，可用于新建 [`'static`] 约束的全局变量。
    ///
    /// # 示例
    ///
    /// ```rust
    /// use xwrust::xwos::lock::seqlock::*;
    ///
    /// static GLOBAL_SEQLOCK: Seqlock<u32>  = Seqlock::new(0);
    /// ```
    ///
    /// [`'static`]: https://doc.rust-lang.org/std/keyword.static.html
    pub const fn new(t: T) -> Self {
        Self {
            sqlk: UnsafeCell::new(XWOS_SQLK_INITIALIZER),
            init: AtomicBool::new(false),
            mode: UnsafeCell::new(SeqlockMode::WriteLock),
            data: UnsafeCell::new(t),
        }
    }
}

impl<T: ?Sized> Seqlock<T> {
    /// 初始化顺序锁。
    ///
    /// 顺序锁必须调用此方法一次，方可正常使用。
    ///
    /// # 示例
    ///
    /// ```rust
    /// use xwrust::xwos::lock::seqlock::*;
    ///
    /// static GLOBAL_SEQLOCK: Seqlock<u32>  = Seqlock::new(0);
    ///
    /// pub fn xwrust_example_seqlock() {
    ///     // ...省略...
    ///     GLOBAL_SEQLOCK.init();
    ///     // 从此处开始 GLOBAL_SEQLOCK 可正常使用
    /// }
    /// ```
    pub fn init(&self) {
        match self.init.compare_exchange(false, true,
                                         Ordering::AcqRel, Ordering::Relaxed) {
            Ok(_) => unsafe {
                xwrustffi_sqlk_init(self.sqlk.get());
            },
            Err(_) => {}
        }
    }

    /// 开启共享读临界区
    ///
    /// 若其他CPU正在访问写临界区，此方法会一直 **自旋** 测试是否可进入 **共享读临界区** 。
    ///
    /// # 示例
    ///
    /// ```rust
    /// loop {
    ///     let res = sqlk.read_begin();
    ///     match res {
    ///         Ok(guard) => {
    ///             let val = *guard; // 读值
    ///             if !guard.read_retry() {
    ///                 break;
    ///             }
    ///         },
    ///         Err(e) => {
    ///             break;
    ///         },
    ///     }
    /// }
    /// ```
    pub fn read_begin(&self) -> Result<SeqlockReadGuard<'_, T>, SeqlockError> {
        if self.init.load(Ordering::Acquire) {
            unsafe {
                let start = xwrustffi_sqlk_rd_begin(self.sqlk.get());
                Ok(SeqlockReadGuard::new(self, start))
            }
        } else {
            Err(SeqlockError::NotInit)
        }
    }

    fn read_retry(&self, start: XwSq) -> bool {
        unsafe {
            xwrustffi_sqlk_rd_retry(self.sqlk.get(), start)
        }
    }

    /// 获取顺序锁，若线程无法获取顺序锁，就阻塞等待，直到能获得锁为止。
    ///
    /// + 若成功获取顺序锁，将返回 **RAII Guard** ： [`SeqlockGuard`] ，用于提供 **Scoped Lock** 机制。
    ///   + [`SeqlockGuard`] 中包含 [`Seqlock`] 的引用， 当 [`SeqlockGuard`] 生命周期结束时，会在 [`drop()`] 方法中自动解锁顺序锁。
    /// + 若失败，将返回错误码 [`SeqlockError`] 。
    ///
    /// # 锁模式 **mode**
    ///
    /// + [`SeqlockMode::WriteLock`] 写，关闭抢占
    /// + [`SeqlockMode::WriteLockBh`] 写，关闭抢占、中断底半部
    /// + [`SeqlockMode::WriteLockCpuirq`] 写，关闭抢占、中断底半部和中断
    /// + [`SeqlockMode::WriteLockCpuirqSave(None)`] 写，关闭抢占、中断底半部和中断，并保存之前的中断标志
    /// + [`SeqlockMode::ReadExclusiveLock`] 独占读，关闭抢占
    /// + [`SeqlockMode::ReadExclusiveLockBh`] 独占读，关闭抢占、中断底半部
    /// + [`SeqlockMode::ReadExclusiveLockCpuirq`] 独占读，关闭抢占、中断底半部和中断
    /// + [`SeqlockMode::ReadExclusiveLockCpuirqSave(None)`] 独占读，关闭抢占、中断底半部和中断，并保存之前的中断标志
    ///
    /// # 错误码
    ///
    /// + [`SeqlockError::NotInit`] 顺序锁未被初始化
    ///
    /// # 示例
    ///
    /// ```rust
    /// use xwrust::xwos::lock::seqlock::*;
    /// static GLOBAL_SEQLOCK: Seqlock<u32> = Seqlock::new(0);
    ///
    /// pub fn xwrust_example_seqlock() {
    ///     // ...省略...
    ///     GLOBAL_SEQLOCK.init();
    ///     match GLOBAL_SEQLOCK.lock(SeqlockMode::ReadExclusiveLockCpuirqSave(None)) {
    ///         Ok(mut guard) => { // 上锁成功
    ///             *guard = 1; // 写共享变量
    ///         } // guard 生命周期结束，自动解锁，并恢复中断标志
    ///         Err(e) => {
    ///             // 上锁失败
    ///         }
    ///     }
    /// }
    /// ```
    ///
    /// [`SeqlockMode::WriteLockCpuirqSave(None)`]: SeqlockMode::WriteLockCpuirqSave
    /// [`SeqlockMode::ReadExclusiveLockCpuirqSave(None)`]: SeqlockMode::ReadExclusiveLockCpuirqSave
    /// [`drop()`]: https://doc.rust-lang.org/std/mem/fn.drop.html
    pub fn lock(&self, mode: SeqlockMode) -> Result<SeqlockGuard<'_, T>, SeqlockError> {
        if self.init.load(Ordering::Acquire) {
            unsafe {
                match mode {
                    SeqlockMode::ReadExclusiveLock => {
                        xwrustffi_sqlk_rdex_lock(self.sqlk.get());
                        *self.mode.get() = SeqlockMode::ReadExclusiveLock;
                    },
                    SeqlockMode::ReadExclusiveLockCpuirq => {
                        xwrustffi_sqlk_rdex_lock_cpuirq(self.sqlk.get());
                        *self.mode.get() = SeqlockMode::ReadExclusiveLockCpuirq;
                    },
                    SeqlockMode::ReadExclusiveLockCpuirqSave(_) => {
                        let mut cpuirq: XwReg = 0;
                        xwrustffi_sqlk_rdex_lock_cpuirqsv(self.sqlk.get(), &mut cpuirq);
                        *self.mode.get() = SeqlockMode::ReadExclusiveLockCpuirqSave(Some(cpuirq));
                    },
                    SeqlockMode::ReadExclusiveLockBh => {
                        xwrustffi_sqlk_rdex_lock_bh(self.sqlk.get());
                        *self.mode.get() = SeqlockMode::ReadExclusiveLockBh;
                    },


                    SeqlockMode::WriteLock => {
                        xwrustffi_sqlk_wr_lock(self.sqlk.get());
                        *self.mode.get() = SeqlockMode::WriteLock;
                    },
                    SeqlockMode::WriteLockCpuirq => {
                        xwrustffi_sqlk_wr_lock_cpuirq(self.sqlk.get());
                        *self.mode.get() = SeqlockMode::WriteLockCpuirq;
                    },
                    SeqlockMode::WriteLockCpuirqSave(_) => {
                        let mut cpuirq: XwReg = 0;
                        xwrustffi_sqlk_wr_lock_cpuirqsv(self.sqlk.get(), &mut cpuirq);
                        *self.mode.get() = SeqlockMode::WriteLockCpuirqSave(Some(cpuirq));
                    },
                    SeqlockMode::WriteLockBh => {
                        xwrustffi_sqlk_wr_lock_bh(self.sqlk.get());
                        *self.mode.get() = SeqlockMode::WriteLockBh;
                    },
                }
                Ok(SeqlockGuard::new(self))
            }
        } else {
            Err(SeqlockError::NotInit)
        }
    }

    /// 尝试获取顺序锁，若线程无法获取顺序锁，立即返回错误。
    ///
    /// + 若成功获取顺序锁，将返回 **RAII Guard** ： [`SeqlockGuard`] ，用于提供 **Scoped Lock** 机制。
    ///   + [`SeqlockGuard`] 中包含 [`Seqlock`] 的引用， 当 [`SeqlockGuard`] 生命周期结束时，会在 [`drop()`] 方法中自动解锁顺序锁。
    /// + 若失败，将返回错误码 [`SeqlockError`] 。
    ///
    /// # 锁模式 **mode**
    ///
    /// + [`SeqlockMode::WriteLock`] 写，关闭抢占
    /// + [`SeqlockMode::WriteLockBh`] 写，关闭抢占、中断底半部
    /// + [`SeqlockMode::WriteLockCpuirq`] 写，关闭抢占、中断底半部和中断
    /// + [`SeqlockMode::WriteLockCpuirqSave(None)`] 写，关闭抢占、中断底半部和中断，并保存之前的中断标志
    /// + [`SeqlockMode::ReadExclusiveLock`] 独占读，关闭抢占
    /// + [`SeqlockMode::ReadExclusiveLockBh`] 独占读，关闭抢占、中断底半部
    /// + [`SeqlockMode::ReadExclusiveLockCpuirq`] 独占读，关闭抢占、中断底半部和中断
    /// + [`SeqlockMode::ReadExclusiveLockCpuirqSave(None)`] 独占读，关闭抢占、中断底半部和中断，并保存之前的中断标志
    ///
    /// # 错误码
    ///
    /// + [`SeqlockError::NotInit`] 顺序锁未被初始化
    /// + [`SeqlockError::Again`] 尝试获取锁失败
    ///
    /// # 示例
    /// ```rust
    /// use xwrust::xwos::lock::seqlock::*;
    /// static GLOBAL_SEQLOCK: Seqlock<u32> = Seqlock::new(0);
    ///
    /// pub fn xwrust_example_seqlock() {
    ///     // ...省略...
    ///     GLOBAL_SEQLOCK.init();
    ///     match GLOBAL_SEQLOCK.trylock(SeqlockMode::ReadExclusiveLockCpuirq) {
    ///         Ok(mut guard) => { // 上锁成功
    ///             *guard = 1; // 写共享变量
    ///         } // guard 生命周期结束，自动解锁
    ///         Err(e) => {
    ///             // 上锁失败
    ///         }
    ///     }
    /// }
    /// ```
    ///
    /// [`SeqlockMode::WriteLockCpuirqSave(None)`]: SeqlockMode::WriteLockCpuirqSave
    /// [`SeqlockMode::ReadExclusiveLockCpuirqSave(None)`]: SeqlockMode::ReadExclusiveLockCpuirqSave
    /// [`drop()`]: https://doc.rust-lang.org/std/mem/fn.drop.html
    pub fn trylock(&self, mode: SeqlockMode) -> Result<SeqlockGuard<'_, T>, SeqlockError> {
        if self.init.load(Ordering::Acquire) {
            unsafe {
                let rc: XwEr;
                match mode {
                    SeqlockMode::ReadExclusiveLock => {
                        rc = xwrustffi_sqlk_rdex_trylock(self.sqlk.get());
                        if 0 == rc {
                            *self.mode.get() = SeqlockMode::ReadExclusiveLock;
                            Ok(SeqlockGuard::new(self))
                        } else if -EAGAIN == rc {
                            Err(SeqlockError::Again)
                        } else {
                            Err(SeqlockError::Unknown(rc))
                        }
                    },
                    SeqlockMode::ReadExclusiveLockCpuirq => {
                        rc = xwrustffi_sqlk_rdex_trylock_cpuirq(self.sqlk.get());
                        if 0 == rc {
                            *self.mode.get() = SeqlockMode::ReadExclusiveLockCpuirq;
                            Ok(SeqlockGuard::new(self))
                        } else if -EAGAIN == rc {
                            Err(SeqlockError::Again)
                        } else {
                            Err(SeqlockError::Unknown(rc))
                        }
                    },
                    SeqlockMode::ReadExclusiveLockCpuirqSave(_) => {
                        let mut cpuirq: XwReg = 0;
                        rc = xwrustffi_sqlk_rdex_trylock_cpuirqsv(self.sqlk.get(), &mut cpuirq);
                        if 0 == rc {
                            *self.mode.get() = SeqlockMode::ReadExclusiveLockCpuirqSave(Some(cpuirq));
                            Ok(SeqlockGuard::new(self))
                        } else if -EAGAIN == rc {
                            Err(SeqlockError::Again)
                        } else {
                            Err(SeqlockError::Unknown(rc))
                        }
                    },
                    SeqlockMode::ReadExclusiveLockBh => {
                        rc = xwrustffi_sqlk_rdex_trylock_bh(self.sqlk.get());
                        if 0 == rc {
                            *self.mode.get() = SeqlockMode::ReadExclusiveLockBh;
                            Ok(SeqlockGuard::new(self))
                        } else if -EAGAIN == rc {
                            Err(SeqlockError::Again)
                        } else {
                            Err(SeqlockError::Unknown(rc))
                        }
                    },

                    SeqlockMode::WriteLock => {
                        rc = xwrustffi_sqlk_wr_trylock(self.sqlk.get());
                        if 0 == rc {
                            *self.mode.get() = SeqlockMode::WriteLock;
                            Ok(SeqlockGuard::new(self))
                        } else if -EAGAIN == rc {
                            Err(SeqlockError::Again)
                        } else {
                            Err(SeqlockError::Unknown(rc))
                        }
                    },
                    SeqlockMode::WriteLockCpuirq => {
                        rc = xwrustffi_sqlk_wr_trylock_cpuirq(self.sqlk.get());
                        if 0 == rc {
                            *self.mode.get() = SeqlockMode::WriteLockCpuirq;
                            Ok(SeqlockGuard::new(self))
                        } else if -EAGAIN == rc {
                            Err(SeqlockError::Again)
                        } else {
                            Err(SeqlockError::Unknown(rc))
                        }
                    },
                    SeqlockMode::WriteLockCpuirqSave(_) => {
                        let mut cpuirq: XwReg = 0;
                        rc = xwrustffi_sqlk_wr_trylock_cpuirqsv(self.sqlk.get(), &mut cpuirq);
                        if 0 == rc {
                            *self.mode.get() = SeqlockMode::WriteLockCpuirqSave(Some(cpuirq));
                            Ok(SeqlockGuard::new(self))
                        } else if -EAGAIN == rc {
                            Err(SeqlockError::Again)
                        } else {
                            Err(SeqlockError::Unknown(rc))
                        }
                    },
                    SeqlockMode::WriteLockBh => {
                        rc = xwrustffi_sqlk_wr_trylock_bh(self.sqlk.get());
                        if 0 == rc {
                            *self.mode.get() = SeqlockMode::WriteLockBh;
                            Ok(SeqlockGuard::new(self))
                        } else if -EAGAIN == rc {
                            Err(SeqlockError::Again)
                        } else {
                            Err(SeqlockError::Unknown(rc))
                        }
                    },
                }
            }
        } else {
            Err(SeqlockError::NotInit)
        }
    }

    /// 释放 [`SeqlockGuard`]，并在 [`drop()`] 方法中解锁顺序锁。
    ///
    /// # 示例
    ///
    /// ```rust
    /// use xwrust::xwos::lock::seqlock::*;
    /// static GLOBAL_SEQLOCK: Seqlock<u32> = Seqlock::new(0);
    ///
    /// pub fn xwrust_example_seqlock() {
    ///     // ...省略...
    ///     GLOBAL_SEQLOCK.init();
    ///     match GLOBAL_SEQLOCK.lock() {
    ///         Ok(mut guard) => { // 上锁成功
    ///             *guard = 1; // 访问共享变量
    ///             Seqlock::unlock(guard); // 主动解锁
    ///         }
    ///         Err(e) => {
    ///             // 上锁失败
    ///         }
    ///     }
    /// }
    /// ```
    ///
    /// [`drop()`]: https://doc.rust-lang.org/std/mem/fn.drop.html
    pub fn unlock(guard: SeqlockGuard<'_, T>) {
        drop(guard)
    }
}

unsafe impl<T: ?Sized + Send> Send for Seqlock<T> {}
unsafe impl<T: ?Sized + Send> Sync for Seqlock<T> {}

impl<T> From<T> for Seqlock<T> {
    /// 从数据新建顺序锁
    ///
    /// 此方法会将数据所有权转移到顺序锁的内部
    ///
    /// 等价于 [`Seqlock::new`]
    fn from(t: T) -> Self {
        Seqlock::new(t)
    }
}

impl<T: ?Sized + Default> Default for Seqlock<T> {
    fn default() -> Seqlock<T> {
        Seqlock::new(Default::default())
    }
}

impl<T: ?Sized> Drop for Seqlock<T> {
    fn drop(&mut self) {
        self.init.store(false, Ordering::Release);
    }
}

/// 顺序锁共享读临界区的守卫
///
/// 共享读临界区的守卫只提供解引用到只读引用。
///
pub struct SeqlockReadGuard<'a, T: ?Sized + 'a> {
    /// 顺序锁的引用
    lock: &'a Seqlock<T>,
    start: XwSq,
}

impl<T: ?Sized> !Send for SeqlockReadGuard<'_, T> {}
unsafe impl<T: ?Sized + Sync> Sync for SeqlockReadGuard<'_, T> {}

impl<'a, T: ?Sized> SeqlockReadGuard<'a, T> {
    fn new(lock: &'a Seqlock<T>, start: XwSq) -> SeqlockReadGuard<'a, T> {
        SeqlockReadGuard {
            lock: lock,
            start: start,
        }
    }

    /// 关闭共享读临界区，并检查是否需要重试
    ///
    /// + 当顺序锁的顺序值没有发生变化，返回 [`false`] ;
    /// + 当顺序锁的顺序值发生了变化，返回 [`true`] 。
    ///
    /// # 示例
    ///
    /// ```rust
    /// loop {
    ///     match sqlk.read_begin() {
    ///         Ok(guard) => {
    ///             let val = *guard;
    ///             if !guard.read_retry() {
    ///                 break;
    ///             }
    ///         },
    ///         Err(e) => {
    ///             break;
    ///         },
    ///     }
    /// }
    /// ```
    pub fn read_retry(&self) -> bool {
        self.lock.read_retry(self.start)
    }
}

impl<T: ?Sized> Deref for SeqlockReadGuard<'_, T> {
    type Target = T;

    fn deref(&self) -> &T {
        unsafe { &*self.lock.data.get() }
    }
}

/// 顺序锁的RAII Guard
///
/// **RAII Guard** 用于提供 **Scoped Lock** 机制。
///
/// + [`SeqlockGuard`] 中包含 [`Seqlock`] 的引用， 当 [`SeqlockGuard`] 生命周期结束时，会在 [`drop()`] 方法中自动解锁顺序锁。
/// + [`SeqlockGuard`] 不可在线程之间转移所有权，因为其 [`drop()`] 方法包含解锁的语义，上锁和解锁必须在同一线程；
/// + [`SeqlockGuard`] 虽然可以在多线程中传递引用( [`Sync`] 约束)，但其实现中没有 **unlock()** 方法，意味着其他线程即便拿到引用也不能解锁。
///
/// [`drop()`]: https://doc.rust-lang.org/std/mem/fn.drop.html
/// [`Sync`]: https://doc.rust-lang.org/std/marker/trait.Send.html
pub struct SeqlockGuard<'a, T: ?Sized + 'a> {
    /// 顺序锁的引用
    lock: &'a Seqlock<T>,
}

impl<T: ?Sized> !Send for SeqlockGuard<'_, T> {}
unsafe impl<T: ?Sized + Sync> Sync for SeqlockGuard<'_, T> {}

impl<'a, T: ?Sized> SeqlockGuard<'a, T> {
    fn new(lock: &'a Seqlock<T>) -> SeqlockGuard<'a, T> {
        SeqlockGuard { lock: lock }
    }

    fn lock(&self) {
        unsafe {
            let mode = &*self.lock.mode.get();
            match mode {
                SeqlockMode::WriteLock |
                SeqlockMode::WriteLockBh |
                SeqlockMode::WriteLockCpuirq |
                SeqlockMode::WriteLockCpuirqSave(_) => {
                    xwrustffi_sqlk_wr_lock(self.lock.sqlk.get());
                },
                SeqlockMode::ReadExclusiveLock |
                SeqlockMode::ReadExclusiveLockBh |
                SeqlockMode::ReadExclusiveLockCpuirq |
                SeqlockMode::ReadExclusiveLockCpuirqSave(_) => {
                    xwrustffi_sqlk_rdex_lock(self.lock.sqlk.get());
                },
            }
        }
    }

    fn unlock(&self) {
        unsafe {
            let mode = &*self.lock.mode.get();
            match mode {
                SeqlockMode::WriteLock |
                SeqlockMode::WriteLockBh |
                SeqlockMode::WriteLockCpuirq |
                SeqlockMode::WriteLockCpuirqSave(_) => {
                    xwrustffi_sqlk_wr_unlock(self.lock.sqlk.get());
                },
                SeqlockMode::ReadExclusiveLock |
                SeqlockMode::ReadExclusiveLockBh |
                SeqlockMode::ReadExclusiveLockCpuirq |
                SeqlockMode::ReadExclusiveLockCpuirqSave(_) => {
                    xwrustffi_sqlk_rdex_unlock(self.lock.sqlk.get());
                },
            }
        }
    }

    /// 阻塞当前线程，直到被条件量唤醒。
    ///
    /// 此方法会消费顺序锁的守卫(Guard)，并当线程阻塞时，在条件量内部释放顺序锁。
    /// 当条件成立，线程被唤醒，会在条件量内部上锁顺序锁，并重新返回顺序锁的守卫(Guard)。
    ///
    /// + 当返回顺序锁的守卫 [`SeqlockGuard`] 时，顺序锁已经被重新上锁；
    /// + 当返回 [`Err`] 时，顺序锁未被上锁。
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
    /// use xwrust::xwos::lock::seqlock::*;
    /// use xwrust::xwos::sync::cond::*;
    /// extern crate alloc;
    /// use alloc::sync::Arc;
    ///
    /// pub fn xwrust_example_seqlock() {
    ///     let pair = Arc::new((Seqlock::new(true), Cond::new()));
    ///     pair.0.init();
    ///     pair.1.init();
    ///     let pair_c = pair.clone();
    ///
    ///     thd::Builder::new()
    ///         .name("child".into())
    ///         .spawn(move |_| { // 子线程闭包
    ///             cthd::sleep(xwtm::ms(500));
    ///             let (lock, cvar) = &*pair_c;
    ///             match lock_child.lock(SeqlockMode::WriteLockCpuirqSave(None)) {
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
    ///     match lock.lock(SeqlockMode::WriteLockCpuirqSave(None)) {
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
    pub fn wait(self, cond: &Cond) -> Result<SeqlockGuard<'a, T>, CondError> {
        unsafe {
            let mut rc = xwrustffi_cond_acquire(cond.cond.get(), *cond.tik.get());
            if rc == 0 {
                let lktype = match &*self.lock.mode.get() {
                    SeqlockMode::WriteLock |
                    SeqlockMode::WriteLockBh |
                    SeqlockMode::WriteLockCpuirq |
                    SeqlockMode::WriteLockCpuirqSave(_) => {
                        XWOS_LK_SQLK_WR
                    },
                    SeqlockMode::ReadExclusiveLock |
                    SeqlockMode::ReadExclusiveLockBh |
                    SeqlockMode::ReadExclusiveLockCpuirq |
                    SeqlockMode::ReadExclusiveLockCpuirqSave(_) => {
                        XWOS_LK_SQLK_RDEX
                    },
                };
                let mut lkst = 0;
                rc = xwrustffi_cond_wait(cond.cond.get(),
                                         self.lock.sqlk.get() as _, lktype, ptr::null_mut(),
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
    /// 此方法会消费顺序锁的守卫(Guard)，并当线程阻塞时，在条件量内部释放顺序锁。
    /// 当条件成立，线程被唤醒，会在条件量内部上锁顺序锁，并重新返回顺序锁的守卫(Guard)。
    /// 当超时后，将返回错误。
    ///
    /// + 当返回顺序锁的守卫 [`SeqlockGuard`] 时，顺序锁已经被重新上锁；
    /// + 当返回 [`Err`] 时，顺序锁未被上锁。
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
    /// use xwrust::xwos::lock::seqlock::*;
    /// use xwrust::xwos::sync::cond::*;
    /// extern crate alloc;
    /// use alloc::sync::Arc;
    ///
    /// pub fn xwrust_example_seqlock() {
    ///     let pair = Arc::new((Seqlock::new(true), Cond::new()));
    ///     pair.0.init();
    ///     pair.1.init();
    ///     let pair_c = pair.clone();
    ///
    ///     thd::Builder::new()
    ///         .name("child".into())
    ///         .spawn(move |_| { // 子线程闭包
    ///             cthd::sleep(xwtm::ms(500));
    ///             let (lock, cvar) = &*pair_c;
    ///             match lock_child.lock(SeqlockMode::WriteLockCpuirqSave(None)) {
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
    ///     match lock.lock(SeqlockMode::WriteLockCpuirqSave(None)) {
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
    pub fn wait_to(self, cond: &Cond, to: XwTm) -> Result<SeqlockGuard<'a, T>, CondError> {
        unsafe {
            let mut rc = xwrustffi_cond_acquire(cond.cond.get(), *cond.tik.get());
            if rc == 0 {
                let lktype = match &*self.lock.mode.get() {
                    SeqlockMode::WriteLock |
                    SeqlockMode::WriteLockBh |
                    SeqlockMode::WriteLockCpuirq |
                    SeqlockMode::WriteLockCpuirqSave(_) => {
                        XWOS_LK_SQLK_WR
                    },
                    SeqlockMode::ReadExclusiveLock |
                    SeqlockMode::ReadExclusiveLockBh |
                    SeqlockMode::ReadExclusiveLockCpuirq |
                    SeqlockMode::ReadExclusiveLockCpuirqSave(_) => {
                        XWOS_LK_SQLK_RDEX
                    },
                };
                let mut lkst = 0;
                rc = xwrustffi_cond_wait_to(cond.cond.get(),
                                            self.lock.sqlk.get() as _, lktype, ptr::null_mut(),
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
    /// 此方法会消费顺序锁的守卫(Guard)，并当线程阻塞时，在条件量内部释放顺序锁。
    /// 当条件成立，线程被唤醒，会在条件量内部上锁顺序锁，并重新返回顺序锁的守卫(Guard)。
    ///
    /// + 当返回顺序锁的守卫 [`SeqlockGuard`] 时，顺序锁已经被重新上锁；
    /// + 当返回 [`Err`] 时，顺序锁未被上锁。
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
    /// use xwrust::xwos::lock::seqlock::*;
    /// use xwrust::xwos::sync::cond::*;
    /// extern crate alloc;
    /// use alloc::sync::Arc;
    ///
    /// pub fn xwrust_example_seqlock() {
    ///     let pair = Arc::new((Seqlock::new(true), Cond::new()));
    ///     pair.0.init();
    ///     pair.1.init();
    ///     let pair_c = pair.clone();
    ///
    ///     thd::Builder::new()
    ///         .name("child".into())
    ///         .spawn(move |_| { // 子线程闭包
    ///             cthd::sleep(xwtm::ms(500));
    ///             let (lock, cvar) = &*pair_c;
    ///             match lock_child.lock(SeqlockMode::ReadExclusiveLockCpuirqSave(None)) {
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
    ///     match lock.lock(SeqlockMode::ReadExclusiveLockCpuirqSave(None)) {
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
    pub fn wait_unintr(self, cond: &Cond) -> Result<SeqlockGuard<'a, T>, CondError> {
        unsafe {
            let mut rc = xwrustffi_cond_acquire(cond.cond.get(), *cond.tik.get());
            if rc == 0 {
                let lktype = match &*self.lock.mode.get() {
                    SeqlockMode::WriteLock |
                    SeqlockMode::WriteLockBh |
                    SeqlockMode::WriteLockCpuirq |
                    SeqlockMode::WriteLockCpuirqSave(_) => {
                        XWOS_LK_SQLK_WR
                    },
                    SeqlockMode::ReadExclusiveLock |
                    SeqlockMode::ReadExclusiveLockBh |
                    SeqlockMode::ReadExclusiveLockCpuirq |
                    SeqlockMode::ReadExclusiveLockCpuirqSave(_) => {
                        XWOS_LK_SQLK_RDEX
                    },
                };
                let mut lkst = 0;
                rc = xwrustffi_cond_wait_unintr(cond.cond.get(),
                                                self.lock.sqlk.get() as _, lktype, ptr::null_mut(),
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
}

impl<T: ?Sized> Deref for SeqlockGuard<'_, T> {
    type Target = T;

    fn deref(&self) -> &T {
        unsafe { &*self.lock.data.get() }
    }
}

impl<T: ?Sized> DerefMut for SeqlockGuard<'_, T> {
    fn deref_mut(&mut self) -> &mut T {
        unsafe { &mut *self.lock.data.get() }
    }
}

impl<T: ?Sized> Drop for SeqlockGuard<'_, T> {
    fn drop(&mut self) {
        unsafe {
            match *self.lock.mode.get() {
                SeqlockMode::ReadExclusiveLock => {
                    xwrustffi_sqlk_rdex_unlock(self.lock.sqlk.get());
                },
                SeqlockMode::ReadExclusiveLockCpuirq => {
                    xwrustffi_sqlk_rdex_unlock_cpuirq(self.lock.sqlk.get());
                },
                SeqlockMode::ReadExclusiveLockCpuirqSave(cpuirq) => {
                    xwrustffi_sqlk_rdex_unlock_cpuirqrs(self.lock.sqlk.get(), cpuirq.unwrap_unchecked());
                },
                SeqlockMode::ReadExclusiveLockBh => {
                    xwrustffi_sqlk_rdex_unlock_bh(self.lock.sqlk.get());
                },

                SeqlockMode::WriteLock => {
                    xwrustffi_sqlk_wr_unlock(self.lock.sqlk.get());
                },
                SeqlockMode::WriteLockCpuirq => {
                    xwrustffi_sqlk_wr_unlock_cpuirq(self.lock.sqlk.get());
                },
                SeqlockMode::WriteLockCpuirqSave(cpuirq) => {
                    xwrustffi_sqlk_wr_unlock_cpuirqrs(self.lock.sqlk.get(), cpuirq.unwrap_unchecked());
                },
                SeqlockMode::WriteLockBh => {
                    xwrustffi_sqlk_wr_unlock_bh(self.lock.sqlk.get());
                },
            }
        }
    }
}
