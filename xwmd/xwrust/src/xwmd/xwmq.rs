//! XWOS RUST：消息队列
//! ========
//!
//! XWOS RUST的消息队列是仿照 [`std::sync::mpsc`]  编写的。可以有多个发送者，但只能有一个接收者。
//!
//! XWOS RUST的消息队列是一个先进先进（FIFO）队列。接收时，从队列的头部开始取走消息；发送时，可以将消息发送到队列尾部（入队 [`XwmqTx::eq()`] ），
//! 也可将消息发送到队列的头部（插队 [`XwmqTx::jq()`] ）。
//!
//!
//! # 创建
//!
//! XWOS RUST的消息队列可使用 [`Xwmq::new()`] 创建。创建时需要指定两个类型参数：发送消息槽的个数以及消息数据的类型。
//!
//! + 可以创建具有静态生命周期 [`'static`] 约束的全局变量：
//!
//! ```rust
//! use xwrust::xwmd::xwmq::*;
//!
//! pub static MQ: Xwmq<16, String> = Xwmq::new(); // 16个消息槽，数据类型为String
//! ```
//!
//! + 也可以使用 [`alloc::sync::Arc`] 在heap中创建：
//!
//! ```rust
//! extern crate alloc;
//! use alloc::sync::Arc;
//!
//! use xwrust::xwmd::xwmq::*;
//!
//! pub fn xwrust_example_xwmq() {
//!     let mq: Arc<Xwmq<16, String>> = Arc::new(Xwmq::new()); // 16个消息槽，数据类型为String
//! }
//! ```
//!
//!
//! # 初始化
//!
//! 无论以何种方式创建的消息队列，都必须在使用前调用 [`Xwmq::init()`] 进行初始化，
//! 初始化后会返回消息队列的发送者 [`XwmqTx`] 以及接收者 [`XwmqRx`] 。
//!
//! ```rust
//! pub fn xwrust_example_xwmq() {
//!     let (tx, rx) = MQ.init();
//! }
//! ```
//!
//!
//! # 发送
//!
//! [`Xwmq`] 的发送者 [`XwmqTx`] 具有 [`Send`] 约束，可在线程之间转移所有权，但不能在线程之间共享( [`!Sync`] )。
//!
//! 发送者 [`XwmqTx`] 可被 [`XwmqTx::clone()`] 为多个发送者，由不同线程持有，向接收者 [`XwmqRx`] 发送消息。
//!
//! ## 入队
//!
//! **入队** 方式是将消息发送到队列的 **尾部** ， **入队** 时，会通过 [`Box`] 将数据放入heap中，然后进行发送。
//!
//! ### 等待入队
//!
//! 方法 [`XwmqTx::eq()`] 只可在 **线程** 上下文中使用：
//!
//! + 当消息队列中没有空的消息槽可用时，发送线程会阻塞等待。
//! + 当消息队列中有可用的消息槽时，发送线程被唤醒，并取走一个消息槽，并发送数据。
//! + 当线程阻塞等待被中断时，返回 [`XwmqError::Interrupt`] 。
//!
//! ### 超时等待入队
//!
//! 方法 [`XwmqTx::eq_to()`] 只可在 **线程** 上下文中使用：
//!
//! + 当消息队列中没有空的消息槽可用时，发送线程会阻塞等待，等待时会指定一个唤醒时间点。
//! + 当消息队列中有可用的消息槽时，发送线程被唤醒，并取走一个消息槽，并发送数据。
//! + 当线程阻塞等待被中断时，返回 [`XwmqError::Interrupt`] 。
//! + 当到达指定的唤醒时间点，线程被唤醒，并返回 [`XwmqError::Timedout`] 。
//!
//! ### 尝试等待入队
//!
//! 方法 [`XwmqTx::tryeq()`] 方法可在 **任意** 上下文中使用，不会阻塞，只会检测信号量的值：
//!
//! + 当消息队列中没有空的消息槽可用时，立即返回 [`XwmqError::NoSlot`] 。
//! + 当消息队列中有可用的消息槽时，发送线程会取走一个消息槽，并发送数据。
//!
//! ## 插队
//!
//! **插队** 方式是将消息发送到队列的 **头部** ， **插队** 时，会通过 [`Box`] 将数据放入heap中，然后进行发送。
//!
//! ### 等待插队
//!
//! 方法 [`XwmqTx::eq()`] 只可在 **线程** 上下文中使用：
//!
//! + 当消息队列中没有空的消息槽可用时，发送线程会阻塞等待。
//! + 当消息队列中有可用的消息槽时，发送线程被唤醒，并取走一个消息槽，并发送数据。
//! + 当线程阻塞等待被中断时，返回 [`XwmqError::Interrupt`] 。
//!
//! ### 超时等待插队
//!
//! 方法 [`XwmqTx::eq_to()`] 只可在 **线程** 上下文中使用：
//!
//! + 当消息队列中没有空的消息槽可用时，发送线程会阻塞等待，等待时会指定一个唤醒时间点。
//! + 当消息队列中有可用的消息槽时，发送线程被唤醒，并取走一个消息槽，并发送数据。
//! + 当线程阻塞等待被中断时，返回 [`XwmqError::Interrupt`] 。
//! + 当到达指定的唤醒时间点，线程被唤醒，并返回 [`XwmqError::Timedout`] 。
//!
//! ### 尝试等待插队
//!
//! 方法 [`XwmqTx::tryeq()`] 方法可在 **任意** 上下文中使用，不会阻塞，只会检测信号量的值：
//!
//! + 当消息队列中没有空的消息槽可用时，立即返回 [`XwmqError::NoSlot`] 。
//! + 当消息队列中有可用的消息槽时，发送线程会取走一个消息槽，并发送数据。
//!
//!
//! # 接收
//!
//! [`Xwmq`] 的接收者 [`XwmqRx`] 具有 [`Send`] 约束，可在线程之间转移所有权，但不能在线程之间共享( [`!Sync`] )。
//!
//! 接收者 [`XwmqRx`] 不可被“克隆”，因为接收者只能有一个。
//!
//! **接收** 是指从接收队列的 **头部** 取出消息，称为 **离队** 。
//! 接收成功，消息 **离队** 时，会通过 [`Box::from_raw()`] 将数据重新放入 [`Box`] 中，然后通过 [`Ok()`] 返回。
//! 当 [`Box`] 被 [`drop()`] 时，会自动释放heap内存。
//!
//! ## 等待离队
//!
//! 方法 [`XwmqRx::dq()`] 只可在 **线程** 上下文中使用：
//!
//! + 当接收队列中没有消息时，接收线程会阻塞等待。
//! + 当接收队列中有可用的消息时，接收线程被唤醒，并取走一个消息，并释放消息槽。
//! + 当线程阻塞等待被中断时，通过 [`Err()`] 返回 [`XwmqError::Interrupt`] 。
//!
//! ### 超时等待离队
//!
//! 方法 [`XwmqRx::dq_to()`] 只可在 **线程** 上下文中使用：
//!
//! + 当消息接收队列中没有消息时，接收线程会阻塞等待，等待时会指定一个唤醒时间点。
//! + 当接收队列中有可用的消息时，接收线程被唤醒，并取走一个消息，释放消息槽。
//! + 当线程阻塞等待被中断时，通过 [`Err()`] 返回 [`XwmqError::Interrupt`] 。
//! + 当到达指定的唤醒时间点，线程被唤醒，并通过 [`Err()`] 返回 [`XwmqError::Timedout`] 。
//!
//! ### 尝试等待离队
//!
//! 方法 [`XwmqRx::trydq()`] 方法可在 **任意** 上下文中使用，不会阻塞，只会检测信号量的值：
//!
//! + 当接收队列中没有消息时，立即通过 [`Err()`] 返回 [`XwmqError::NoMsg`] 。
//! + 当接收队列中有可用的消息时，接收线程会取走一个消息，并释放消息槽。
//!
//!
//! # 示例
//!
//! [XWOS/xwam/xwrust-example/xwrust_example_xwmq](https://gitee.com/xwos/XWOS/blob/main/xwam/xwrust-example/xwrust_example_xwmq/src/lib.rs)
//!
//!
//! # 对比 [`std::sync::mpsc`]
//!
//!
//! + XWOS RUST:
//!
//! ```rust
//! #![no_std]
//!
//! extern crate alloc;
//!
//! use xwrust::xwos::thd;
//! use xwrust::xwmd::xwmq::*;
//!
//! pub static MQ: Xwmq<16, u32> = Xwmq::new();
//!
//! pub fn xwrust_example_xwmq() {
//!     println!("XWOS RUST Example: XWMQ");
//!     let (tx, rx) = MQ.init();
//!
//!     let _ = thd::Builder::new()
//!                 .spawn(move |_| { // 子线程闭包
//!                     tx.eq(53);
//!                     "OK"
//!                 });
//!
//!     let rc = rx.dq();
//!     match rc {
//!         Ok(d) => { // d为接收到的数据，并被放在智能指针Box中
//!         },
//!         Err(e) => {
//!         },
//!     };
//! }
//! ```
//!
//! + [`std::sync::mpsc`]
//!
//! ```rust
//! use std::thread;
//! use std::sync::mpsc::sync_channel;
//!
//! let (tx, rx) = sync_channel::<i32>(0);
//! thread::spawn(move|| {
//!     tx.send(53).unwrap();
//! });
//! rx.recv().unwrap();
//! ```
//!
//! ### 构建全局变量的方式
//!
//! + [`std::sync::mpsc`] 没有编译期构造函数，因此只能借助宏 [`lazy_static!`] 创建 [`'static`] 约束的全局变量；
//! + [`xwrust::xwmd::xwmq`] 则可以直接在函数外部定义。
//!
//!
//! ### 通道类型
//!
//! + [`std::sync::mpsc`] 还有异步的通道 [`std::sync::mpsc::channel()`] ；
//! + [`xwrust::xwmd::xwmq`] 通过 [`Xwmq::init()`] 返回的是类似于 [`std::sync::mpsc::sync_channel()`] 的通道， [`xwrust::xwmd::xwmq`] 没有异步的通道 。
//!
//! ### 接收消息返回方式不同
//!
//! + [`std::sync::mpsc`] 接收到消息后，直接返回类型为 **T** 的消息；
//! + [`xwrust::xwmd::xwmq`] 则返回 **Box\<T\>** 。
//!
//! [`std::sync::mpsc`]: <https://doc.rust-lang.org/std/sync/mpsc/index.html>
//! [`std::sync::mpsc::channel()`]: <https://doc.rust-lang.org/std/sync/mpsc/fn.channel.html>
//! [`std::sync::mpsc::sync_channel()`]: <https://doc.rust-lang.org/std/sync/mpsc/fn.sync_channel.html>
//! [`'static`]: <https://doc.rust-lang.org/std/keyword.static.html>
//! [`alloc::sync::Arc`]: <https://doc.rust-lang.org/alloc/sync/struct.Arc.html>
//! [`Send`]: <https://doc.rust-lang.org/core/marker/trait.Send.html>
//! [`Sync`]: <https://doc.rust-lang.org/core/marker/trait.Sync.html>
//! [`lazy_static!`]: <https://lib.rs/crates/lazy_static>
//! [`!Sync`]: <https://doc.rust-lang.org/core/marker/trait.Sync.html>
//! [`Box`]: <https://doc.rust-lang.org/alloc/boxed/struct.Box.html>
//! [`Box::from_raw()`]: <https://doc.rust-lang.org/alloc/boxed/struct.Box.html#method.from_raw>
//! [`drop()`]: <https://doc.rust-lang.org/alloc/boxed/struct.Box.html#method.drop>
//! [`Ok()`]: <https://doc.rust-lang.org/core/result/enum.Result.html#variant.Ok>
//! [`Err()`]: <https://doc.rust-lang.org/core/result/enum.Result.html#variant.Err>
//! [`xwrust::xwmd::xwmq`]: crate::xwmd::xwmq

extern crate core;
use core::ffi::*;
use core::result::Result;
use core::cell::UnsafeCell;
use core::ptr;
use core::marker::PhantomData;

extern crate alloc;
use alloc::boxed::Box;

use crate::types::*;
use crate::errno::*;

extern "C" {
    fn xwmq_init(mq: *mut XwmdXwmq, txq: *mut c_void, num: XwSz) -> XwEr;
    fn xwmq_fini(mq: *mut XwmdXwmq) -> XwEr;
    fn xwmq_grab(mq: *mut XwmdXwmq) -> XwEr;
    fn xwmq_put(mq: *mut XwmdXwmq) -> XwEr;
    fn xwmq_gettik(mq: *mut XwmdXwmq) -> XwSq;
    fn xwmq_acquire(mq: *mut XwmdXwmq, tik: XwSq) -> XwEr;
    fn xwmq_release(mq: *mut XwmdXwmq, tik: XwSq) -> XwEr;

    fn xwmq_eq(mq: *mut XwmdXwmq, topic: XwSq, data: *mut c_void) -> XwEr;
    fn xwmq_eq_to(mq: *mut XwmdXwmq, topic: XwSq, data: *mut c_void, to: XwTm) -> XwEr;
    fn xwmq_tryeq(mq: *mut XwmdXwmq, topic: XwSq, data: *mut c_void) -> XwEr;
    fn xwmq_jq(mq: *mut XwmdXwmq, topic: XwSq, data: *mut c_void) -> XwEr;
    fn xwmq_jq_to(mq: *mut XwmdXwmq, topic: XwSq, data: *mut c_void, to: XwTm) -> XwEr;
    fn xwmq_tryjq(mq: *mut XwmdXwmq, topic: XwSq, data: *mut c_void) -> XwEr;
    fn xwmq_dq(mq: *mut XwmdXwmq, topic: *mut XwSq, data: *mut *mut c_void) -> XwEr;
    fn xwmq_dq_to(mq: *mut XwmdXwmq, topic: *mut XwSq, data: *mut *mut c_void, to: XwTm) -> XwEr;
    fn xwmq_trydq(mq: *mut XwmdXwmq, topic: *mut XwSq, data: *mut *mut c_void) -> XwEr;
}

/// 消息队列的错误码
#[derive(Debug)]
pub enum XwmqError {
    /// 没有错误
    Ok,
    /// 消息队列没有初始化
    NotInit,
    /// 等待被中断
    Interrupt,
    /// 等待超时
    Timedout,
    /// 不在线程上下文内
    NotThreadContext,
    /// 消息队列中没有可用的消息槽
    NoSlot,
    /// 消息队列中没有消息
    NoMsg,
    /// 未知错误
    Unknown(XwEr),
}

/// 消息队列对象占用的内存大小
pub const SIZEOF_XWMQ: usize = 200;

/// 消息占用的内存大小
pub const SIZEOF_XWMQ_MSG: usize = 16;

xwos_struct! {
    /// 用于构建消息队列的内存数组类型
    pub struct XwmdXwmq {
        #[doc(hidden)]
        mem: [u8; SIZEOF_XWMQ],
    }
}

/// 用于构建消息槽的内存数组类型
#[repr(C)]
#[cfg_attr(target_pointer_width = "32", repr(align(8)))]
#[cfg_attr(target_pointer_width = "64", repr(align(16)))]
pub struct XwmdXwmqMsg<const N: XwSz>
where
    [u8; N * SIZEOF_XWMQ_MSG]: Sized
{
    #[doc(hidden)]
    mem: [u8; N * SIZEOF_XWMQ_MSG],
}

/// 用于构建消息队列的内存数组常量
///
/// 此常量的作用是告诉编译器消息队列对象需要多大的内存。
pub const XWMQ_INITIALIZER: XwmdXwmq = XwmdXwmq {
    mem: [0; SIZEOF_XWMQ],
};

/// 消息队列对象结构体
pub struct Xwmq<const N: XwSz, T>
where
    [u8; N * SIZEOF_XWMQ_MSG]: Sized
{
    /// 用于初始化消息队列对象的内存空间
    pub(crate) mq: UnsafeCell<XwmdXwmq>,
    /// 用于初始化消息池的内存空间
    pub(crate) pool: UnsafeCell<XwmdXwmqMsg<N>>,
    /// 消息队列对象的标签
    pub(crate) tik: UnsafeCell<XwSq>,
    phantom: PhantomData<T>,
}

unsafe impl<const N: XwSz, T> Send for Xwmq<N, T>
where
    [u8; N * SIZEOF_XWMQ_MSG]: Sized {}

unsafe impl<const N: XwSz, T> Sync for Xwmq<N, T>
where
    [u8; N * SIZEOF_XWMQ_MSG]: Sized {}

impl<const N: XwSz, T> Xwmq<N, T>
where
    [u8; N * SIZEOF_XWMQ_MSG]: Sized
{
    /// 新建消息队列对象。
    ///
    /// 此方法是编译期方法。
    ///
    /// # 示例
    ///
    /// + 具有 [`'static`] 约束的全局变量全局变量：
    ///
    /// ```rust
    /// use xwrust::xwmd::xwmq::*;
    ///
    /// pub static MQ: Xwmq<16, String> = Xwmq::new(); // 16个消息槽，数据类型为String
    /// ```
    ///
    /// + 在heap中创建：
    ///
    /// ```rust
    /// extern crate alloc;
    /// use alloc::sync::Arc;
    ///
    /// use xwrust::xwmd::xwmq::*;
    ///
    /// pub fn xwrust_example_xwmq() {
    ///     let mq: Arc<Xwmq<16, String>> = Arc::new(Xwmq::new()); // 16个消息槽，数据类型为String
    /// }
    /// ```
    ///
    /// [`'static`]: https://doc.rust-lang.org/std/keyword.static.html
    pub const fn new() -> Self {
        Self {
            mq: UnsafeCell::new(XWMQ_INITIALIZER),
            pool: UnsafeCell::new(XwmdXwmqMsg { mem: [0; N * SIZEOF_XWMQ_MSG] }),
            tik: UnsafeCell::new(0),
            phantom: PhantomData,
        }
    }

    /// 初始化消息队列对象。
    ///
    /// 消息队列对象必须调用此方法一次，方可正常使用。
    /// 初始化后会返回消息队列的发送者 [`XwmqTx`] 以及接收者 [`XwmqRx`] 。
    ///
    /// # 示例
    ///
    /// ```rust
    /// use xwrust::xwmd::xwmq::*;
    ///
    /// pub static MQ: Xwmq<16, String> = Xwmq::new();
    ///
    /// pub fn xwrust_example_xwmq() {
    ///     let (tx, rx) = MQ.init();
    /// }
    /// ```
    pub fn init(&self) -> (XwmqTx<N, T>, XwmqRx<N, T>) {
        unsafe {
            xwmq_init(self.mq.get(), self.pool.get() as _, N);
            *self.tik.get() = xwmq_gettik(self.mq.get());
        }
        (XwmqTx {xwmq: &self}, XwmqRx {xwmq: &self}, )
    }
}

#[derive(Clone)]
pub struct XwmqTx<'a, const N: XwSz, T>
where
    [u8; N * SIZEOF_XWMQ_MSG]: Sized
{
    xwmq: &'a Xwmq<N, T>,
}

pub struct XwmqRx<'a, const N: XwSz, T>
where
    [u8; N * SIZEOF_XWMQ_MSG]: Sized
{
    xwmq: &'a Xwmq<N, T>,
}

unsafe impl<'a, const N: XwSz, T> Send for XwmqTx<'a, N, T>
where
    [u8; N * SIZEOF_XWMQ_MSG]: Sized
{}

impl<'a, const N: XwSz, T> !Sync for XwmqTx<'a, N, T>
where
    [u8; N * SIZEOF_XWMQ_MSG]: Sized
{}

unsafe impl<'a, const N: XwSz, T> Send for XwmqRx<'a, N, T>
where
    [u8; N * SIZEOF_XWMQ_MSG]: Sized
{}

impl<'a, const N: XwSz, T> !Sync for XwmqRx<'a, N, T>
where
    [u8; N * SIZEOF_XWMQ_MSG]: Sized
{}

impl<'a, const N: XwSz, T> XwmqTx<'a, N, T>
where
    [u8; N * SIZEOF_XWMQ_MSG]: Sized
{
    /// 等待空的消息槽，通过消息槽将消息发送到队列的 **尾部** 。
    ///
    /// + 当消息队列中没有空的消息槽可用时，发送线程会阻塞等待。
    /// + 当消息队列中有可用的消息槽时，发送线程被唤醒，并取走一个消息槽，并发送数据。
    /// + 消息 **入队** 时，会通过 [`Box`] 将数据放入heap中，然后进行发送。
    /// + 当发送线程阻塞等待被中断时，返回 [`XwmqError::Interrupt`] 。
    ///
    /// # 上下文
    ///
    /// + 线程
    ///
    /// # 错误码
    ///
    /// + [`XwmqError::Ok`] 没有错误
    /// + [`XwmqError::NotInit`] 消息队列没有初始化
    /// + [`XwmqError::Interrupt`] 等待被中断
    /// + [`XwmqError::NotThreadContext`] 不在线程上下文内
    ///
    /// # 示例
    ///
    /// ```rust
    /// extern crate alloc;
    /// use alloc::string::String;
    /// use alloc::string::ToString;
    ///
    /// use xwrust::xwmd::xwmq::*;
    ///
    /// pub static MQ: Xwmq<16, String> = Xwmq::new();
    ///
    /// pub fn xwrust_example_xwmq() {
    ///     let (tx, rx) = MQ.init();
    ///     // ...省略...
    ///     tx.eq("message".to_string());
    ///     // ...省略...
    /// }
    /// ```
    ///
    /// [`Box`]: <https://doc.rust-lang.org/alloc/boxed/struct.Box.html>
    pub fn eq(&self, data: T) -> XwmqError {
        let boxdata = Box::new(data);
        let raw = Box::into_raw(boxdata);
        unsafe {
            let mut rc = xwmq_acquire(self.xwmq.mq.get(), *self.xwmq.tik.get());
            if rc == 0 {
                rc = xwmq_eq(self.xwmq.mq.get(), 0, raw as *mut c_void);
                xwmq_put(self.xwmq.mq.get());
                if XWOK == rc {
                    XwmqError::Ok
                } else if  -EINTR == rc {
                    XwmqError::Interrupt
                } else if -ENOTTHDCTX == rc {
                    XwmqError::NotThreadContext
                } else {
                    XwmqError::Unknown(rc)
                }
            } else {
                XwmqError::NotInit
            }
        }
    }

    /// 限时等待空的消息槽，通过消息槽将消息发送到队列的 **尾部** 。
    ///
    /// + 当消息队列中没有空的消息槽可用时，发送线程会阻塞等待，等待时会指定一个唤醒时间点。
    /// + 当消息队列中有可用的消息槽时，发送线程被唤醒，并取走一个消息槽，并发送数据。
    /// + 消息 **入队** 时，会通过 [`Box`] 将数据放入heap中，然后进行发送。
    /// + 当发送线程阻塞等待被中断时，返回 [`XwmqError::Interrupt`] 。
    /// + 当到达指定的唤醒时间点，线程被唤醒，并返回 [`XwmqError::Timedout`] 。
    ///
    /// # 上下文
    ///
    /// + 线程
    ///
    /// # 错误码
    ///
    /// + [`XwmqError::Ok`] 没有错误
    /// + [`XwmqError::NotInit`] 消息队列没有初始化
    /// + [`XwmqError::Interrupt`] 等待被中断
    /// + [`XwmqError::Timedout`] 等待超时
    /// + [`XwmqError::NotThreadContext`] 不在线程上下文内
    ///
    /// # 示例
    ///
    /// ```rust
    /// extern crate alloc;
    /// use alloc::string::String;
    /// use alloc::string::ToString;
    ///
    /// use xwrust::xwtm;
    /// use xwrust::xwmd::xwmq::*;
    ///
    /// pub static MQ: Xwmq<16, String> = Xwmq::new();
    ///
    /// pub fn xwrust_example_xwmq() {
    ///     let (tx, rx) = MQ.init();
    ///     // ...省略...
    ///     tx.eq_to("message".to_string(), xwtm::ft(xwtm::s(1)));
    ///     // ...省略...
    /// }
    /// ```
    ///
    /// [`Box`]: <https://doc.rust-lang.org/alloc/boxed/struct.Box.html>
    pub fn eq_to(&self, data: T, to: XwTm) -> XwmqError {
        let boxdata = Box::new(data);
        let raw = Box::into_raw(boxdata);
        unsafe {
            let mut rc = xwmq_acquire(self.xwmq.mq.get(), *self.xwmq.tik.get());
            if rc == 0 {
                rc = xwmq_eq_to(self.xwmq.mq.get(), 0, raw as *mut c_void, to);
                xwmq_put(self.xwmq.mq.get());
                if XWOK == rc {
                    XwmqError::Ok
                } else if  -EINTR == rc {
                    XwmqError::Interrupt
                } else if -ETIMEDOUT == rc {
                    XwmqError::Timedout
                } else if -ENOTTHDCTX == rc {
                    XwmqError::NotThreadContext
                } else {
                    XwmqError::Unknown(rc)
                }
            } else {
                XwmqError::NotInit
            }
        }
    }

    /// 尝试获取空的消息槽，并通过消息槽将消息发送到队列的 **尾部** 。
    ///
    /// + 当消息队列中没有空的消息槽可用时，立即返回 [`XwmqError::NoSlot`] 。
    /// + 当消息队列中有可用的消息槽时，发送线程会取走一个消息槽，并发送数据。
    /// + 消息 **入队** 时，会通过 [`Box`] 将数据放入heap中，然后进行发送。
    ///
    /// # 上下文
    ///
    /// + 线程
    /// + 中断
    /// + 中断底半部
    ///
    /// # 错误码
    ///
    /// + [`XwmqError::Ok`] 没有错误
    /// + [`XwmqError::NotInit`] 消息队列没有初始化
    /// + [`XwmqError::NoSlot`] 消息队列中没有可用的消息槽
    /// + [`XwmqError::NotThreadContext`] 不在线程上下文内
    ///
    /// # 示例
    ///
    /// ```rust
    /// extern crate alloc;
    /// use alloc::string::String;
    /// use alloc::string::ToString;
    ///
    /// use xwrust::xwmd::xwmq::*;
    ///
    /// pub static MQ: Xwmq<16, String> = Xwmq::new();
    ///
    /// pub fn xwrust_example_xwmq() {
    ///     let (tx, rx) = MQ.init();
    ///     // ...省略...
    ///     tx.tryeq("message".to_string());
    ///     // ...省略...
    /// }
    /// ```
    ///
    /// [`Box`]: <https://doc.rust-lang.org/alloc/boxed/struct.Box.html>
    pub fn tryeq(&self, data: T) -> XwmqError {
        let boxdata = Box::new(data);
        let raw = Box::into_raw(boxdata);
        unsafe {
            let mut rc = xwmq_acquire(self.xwmq.mq.get(), *self.xwmq.tik.get());
            if rc == 0 {
                rc = xwmq_tryeq(self.xwmq.mq.get(), 0, raw as *mut c_void);
                xwmq_put(self.xwmq.mq.get());
                if XWOK == rc {
                    XwmqError::Ok
                } else if  -ENODATA == rc {
                    XwmqError::NoSlot
                } else {
                    XwmqError::Unknown(rc)
                }
            } else {
                XwmqError::NotInit
            }
        }
    }

    /// 等待空的消息槽，通过消息槽将消息发送到队列的 **头部** 。
    ///
    /// + 当消息队列中没有空的消息槽可用时，发送线程会阻塞等待。
    /// + 当消息队列中有可用的消息槽时，发送线程被唤醒，并取走一个消息槽，并发送数据。
    /// + 消息 **入队** 时，会通过 [`Box`] 将数据放入heap中，然后进行发送。
    /// + 当发送线程阻塞等待被中断时，返回 [`XwmqError::Interrupt`] 。
    ///
    /// # 上下文
    ///
    /// + 线程
    ///
    /// # 错误码
    ///
    /// + [`XwmqError::Ok`] 没有错误
    /// + [`XwmqError::NotInit`] 消息队列没有初始化
    /// + [`XwmqError::Interrupt`] 等待被中断
    /// + [`XwmqError::NotThreadContext`] 不在线程上下文内
    ///
    /// # 示例
    ///
    /// ```rust
    /// extern crate alloc;
    /// use alloc::string::String;
    /// use alloc::string::ToString;
    ///
    /// use xwrust::xwmd::xwmq::*;
    ///
    /// pub static MQ: Xwmq<16, String> = Xwmq::new();
    ///
    /// pub fn xwrust_example_xwmq() {
    ///     let (tx, rx) = MQ.init();
    ///     // ...省略...
    ///     tx.jq("message".to_string());
    ///     // ...省略...
    /// }
    /// ```
    ///
    /// [`Box`]: <https://doc.rust-lang.org/alloc/boxed/struct.Box.html>
    pub fn jq(&self, data: T) -> XwmqError {
        let boxdata = Box::new(data);
        let raw = Box::into_raw(boxdata);
        unsafe {
            let mut rc = xwmq_acquire(self.xwmq.mq.get(), *self.xwmq.tik.get());
            if rc == 0 {
                rc = xwmq_jq(self.xwmq.mq.get(), 0, raw as *mut c_void);
                xwmq_put(self.xwmq.mq.get());
                if XWOK == rc {
                    XwmqError::Ok
                } else if  -EINTR == rc {
                    XwmqError::Interrupt
                } else if -ENOTTHDCTX == rc {
                    XwmqError::NotThreadContext
                } else {
                    XwmqError::Unknown(rc)
                }
            } else {
                XwmqError::NotInit
            }
        }
    }

    /// 限时等待空的消息槽，通过消息槽将消息发送到队列的 **头部** 。
    ///
    /// + 当消息队列中没有空的消息槽可用时，发送线程会阻塞等待，等待时会指定一个唤醒时间点。
    /// + 当消息队列中有可用的消息槽时，发送线程被唤醒，并取走一个消息槽，并发送数据。
    /// + 消息 **入队** 时，会通过 [`Box`] 将数据放入heap中，然后进行发送。
    /// + 当发送线程阻塞等待被中断时，返回 [`XwmqError::Interrupt`] 。
    /// + 当到达指定的唤醒时间点，线程被唤醒，并返回 [`XwmqError::Timedout`] 。
    ///
    /// # 上下文
    ///
    /// + 线程
    ///
    /// # 错误码
    ///
    /// + [`XwmqError::Ok`] 没有错误
    /// + [`XwmqError::NotInit`] 消息队列没有初始化
    /// + [`XwmqError::Interrupt`] 等待被中断
    /// + [`XwmqError::Timedout`] 等待超时
    /// + [`XwmqError::NotThreadContext`] 不在线程上下文内
    ///
    /// # 示例
    ///
    /// ```rust
    /// extern crate alloc;
    /// use alloc::string::String;
    /// use alloc::string::ToString;
    ///
    /// use xwrust::xwtm;
    /// use xwrust::xwmd::xwmq::*;
    ///
    /// pub static MQ: Xwmq<16, String> = Xwmq::new();
    ///
    /// pub fn xwrust_example_xwmq() {
    ///     let (tx, rx) = MQ.init();
    ///     // ...省略...
    ///     tx.jq_to("message".to_string(), xwtm::ft(xwtm::s(1)));
    ///     // ...省略...
    /// }
    /// ```
    ///
    /// [`Box`]: <https://doc.rust-lang.org/alloc/boxed/struct.Box.html>
    pub fn jq_to(&self, data: T, to: XwTm) -> XwmqError {
        let boxdata = Box::new(data);
        let raw = Box::into_raw(boxdata);
        unsafe {
            let mut rc = xwmq_acquire(self.xwmq.mq.get(), *self.xwmq.tik.get());
            if rc == 0 {
                rc = xwmq_jq_to(self.xwmq.mq.get(), 0, raw as *mut c_void, to);
                xwmq_put(self.xwmq.mq.get());
                if XWOK == rc {
                    XwmqError::Ok
                } else if  -EINTR == rc {
                    XwmqError::Interrupt
                } else if -ETIMEDOUT == rc {
                    XwmqError::Timedout
                } else if -ENOTTHDCTX == rc {
                    XwmqError::NotThreadContext
                } else {
                    XwmqError::Unknown(rc)
                }
            } else {
                XwmqError::NotInit
            }
        }
    }

    /// 尝试获取空的消息槽，并通过消息槽将消息发送到队列的 **头部** 。
    ///
    /// + 当消息队列中没有空的消息槽可用时，立即返回 [`XwmqError::NoSlot`] 。
    /// + 当消息队列中有可用的消息槽时，发送线程会取走一个消息槽，并发送数据。
    /// + 消息 **入队** 时，会通过 [`Box`] 将数据放入heap中，然后进行发送。
    ///
    /// # 上下文
    ///
    /// + 线程
    /// + 中断
    /// + 中断底半部
    ///
    /// # 错误码
    ///
    /// + [`XwmqError::Ok`] 没有错误
    /// + [`XwmqError::NotInit`] 消息队列没有初始化
    /// + [`XwmqError::NoSlot`] 消息队列中没有可用的消息槽
    /// + [`XwmqError::NotThreadContext`] 不在线程上下文内
    ///
    /// # 示例
    ///
    /// ```rust
    /// extern crate alloc;
    /// use alloc::string::String;
    /// use alloc::string::ToString;
    ///
    /// use xwrust::xwmd::xwmq::*;
    ///
    /// pub static MQ: Xwmq<16, String> = Xwmq::new();
    ///
    /// pub fn xwrust_example_xwmq() {
    ///     let (tx, rx) = MQ.init();
    ///     // ...省略...
    ///     tx.tryjq("message".to_string());
    ///     // ...省略...
    /// }
    /// ```
    ///
    /// [`Box`]: <https://doc.rust-lang.org/alloc/boxed/struct.Box.html>
    pub fn tryjq(&self, data: T) -> XwmqError {
        let boxdata = Box::new(data);
        let raw = Box::into_raw(boxdata);
        unsafe {
            let mut rc = xwmq_acquire(self.xwmq.mq.get(), *self.xwmq.tik.get());
            if rc == 0 {
                rc = xwmq_tryjq(self.xwmq.mq.get(), 0, raw as *mut c_void);
                xwmq_put(self.xwmq.mq.get());
                if XWOK == rc {
                    XwmqError::Ok
                } else if  -ENODATA == rc {
                    XwmqError::NoSlot
                } else {
                    XwmqError::Unknown(rc)
                }
            } else {
                XwmqError::NotInit
            }
        }
    }
}

impl<'a, const N: XwSz, T> XwmqRx<'a, N, T>
where
    [u8; N * SIZEOF_XWMQ_MSG]: Sized
{
    /// 等待接收消息。
    ///
    /// + **接收** 是指从接收队列的 **头部** 取出消息，称为 **离队** 。
    /// + 当接收队列中没有消息时，接收线程会阻塞等待。
    /// + 当接收队列中有可用的消息时，接收线程被唤醒，并取走一个消息，释放消息槽。
    /// + 消息 **离队** 时，会通过 [`Box::from_raw()`] 将数据重新放入 [`Box`] 中，然后通过 [`Ok()`] 返回。
    /// + 当线程阻塞等待被中断时，通过 [`Err()`] 返回 [`XwmqError::Interrupt`] 。
    ///
    /// # 上下文
    ///
    /// + 线程
    ///
    /// # 错误码
    ///
    /// + [`XwmqError::Ok`] 没有错误
    /// + [`XwmqError::NotInit`] 消息队列没有初始化
    /// + [`XwmqError::Interrupt`] 等待被中断
    /// + [`XwmqError::NotThreadContext`] 不在线程上下文内
    ///
    /// # 示例
    ///
    /// ```rust
    /// extern crate alloc;
    /// use alloc::string::String;
    /// use alloc::string::ToString;
    ///
    /// use xwrust::xwmd::xwmq::*;
    ///
    /// pub static MQ: Xwmq<16, String> = Xwmq::new();
    ///
    /// pub fn xwrust_example_xwmq() {
    ///     let (tx, rx) = MQ.init();
    ///     // ...省略...
    ///     let rc = rx.dq();
    ///         match rc {
    ///             Ok(d) => { // d是Box<String>
    ///             },
    ///             Err(e) => { // e是错误码
    ///             },
    ///         };
    ///     // ...省略...
    /// }
    /// ```
    ///
    /// [`Box`]: <https://doc.rust-lang.org/alloc/boxed/struct.Box.html>
    /// [`Box::from_raw()`]: <https://doc.rust-lang.org/alloc/boxed/struct.Box.html#method.from_raw>
    /// [`Ok()`]: <https://doc.rust-lang.org/core/result/enum.Result.html#variant.Ok>
    /// [`Err()`]: <https://doc.rust-lang.org/core/result/enum.Result.html#variant.Err>
    pub fn dq(&self) -> Result<Box<T>, XwmqError> {
        unsafe {
            let mut rc = xwmq_acquire(self.xwmq.mq.get(), *self.xwmq.tik.get());
            if rc == 0 {
                let mut topic = 0;
                let mut raw: *mut c_void = ptr::null_mut();
                rc = xwmq_dq(self.xwmq.mq.get(), &mut topic, &mut raw);
                xwmq_put(self.xwmq.mq.get());
                if XWOK == rc {
                    let boxdata = Box::from_raw(raw as *mut T);
                    Ok(boxdata)
                } else if -EINTR == rc {
                    Err(XwmqError::Interrupt)
                } else if -ENOTTHDCTX == rc {
                    Err(XwmqError::NotThreadContext)
                } else {
                    Err(XwmqError::Unknown(rc))
                }
            } else {
                Err(XwmqError::NotInit)
            }
        }
    }

    /// 限时等待接收消息。
    ///
    /// + **接收** 是指从接收队列的 **头部** 取出消息，称为 **离队** 。
    /// + 当接收队列中没有消息时，接收线程会阻塞等待，等待时会指定一个唤醒时间点。
    /// + 当接收队列中有可用的消息时，接收线程被唤醒，并取走一个消息，释放消息槽。
    /// + 消息 **离队** 时，会通过 [`Box::from_raw()`] 将数据重新放入 [`Box`] 中，然后通过 [`Ok()`] 返回。
    /// + 当线程阻塞等待被中断时，通过 [`Err()`] 返回 [`XwmqError::Interrupt`] 。
    /// + 当到达指定的唤醒时间点，线程被唤醒，并通过 [`Err()`] 返回 [`XwmqError::Timedout`] 。
    ///
    /// # 上下文
    ///
    /// + 线程
    ///
    /// # 错误码
    ///
    /// + [`XwmqError::Ok`] 没有错误
    /// + [`XwmqError::NotInit`] 消息队列没有初始化
    /// + [`XwmqError::Interrupt`] 等待被中断
    /// + [`XwmqError::Timedout`] 等待超时
    /// + [`XwmqError::NotThreadContext`] 不在线程上下文内
    ///
    /// # 示例
    ///
    /// ```rust
    /// extern crate alloc;
    /// use alloc::string::String;
    /// use alloc::string::ToString;
    ///
    /// use xwrust::xwtm;
    /// use xwrust::xwmd::xwmq::*;
    ///
    /// pub static MQ: Xwmq<16, String> = Xwmq::new();
    ///
    /// pub fn xwrust_example_xwmq() {
    ///     let (tx, rx) = MQ.init();
    ///     // ...省略...
    ///     let rc = rx.dq_to(xwtm::ft(xwtm::s(1)));
    ///         match rc {
    ///             Ok(d) => { // d是Box<String>
    ///             },
    ///             Err(e) => { // e是错误码
    ///             },
    ///         };
    ///     // ...省略...
    /// }
    /// ```
    ///
    /// [`Box`]: <https://doc.rust-lang.org/alloc/boxed/struct.Box.html>
    /// [`Box::from_raw()`]: <https://doc.rust-lang.org/alloc/boxed/struct.Box.html#method.from_raw>
    /// [`Ok()`]: <https://doc.rust-lang.org/core/result/enum.Result.html#variant.Ok>
    /// [`Err()`]: <https://doc.rust-lang.org/core/result/enum.Result.html#variant.Err>
    pub fn dq_to(&self, to: XwTm) -> Result<Box<T>, XwmqError> {
        unsafe {
            let mut rc = xwmq_acquire(self.xwmq.mq.get(), *self.xwmq.tik.get());
            if rc == 0 {
                let mut topic = 0;
                let mut raw: *mut c_void = ptr::null_mut();
                rc = xwmq_dq_to(self.xwmq.mq.get(), &mut topic, &mut raw, to);
                xwmq_put(self.xwmq.mq.get());
                if XWOK == rc {
                    let boxdata = Box::from_raw(raw as *mut T);
                    Ok(boxdata)
                } else if -EINTR == rc {
                    Err(XwmqError::Interrupt)
                } else if -ETIMEDOUT == rc {
                    Err(XwmqError::Timedout)
                } else if -ENOTTHDCTX == rc {
                    Err(XwmqError::NotThreadContext)
                } else {
                    Err(XwmqError::Unknown(rc))
                }
            } else {
                Err(XwmqError::NotInit)
            }
        }
    }

    /// 尝试接收消息。
    ///
    /// + **接收** 是指从接收队列的 **头部** 取出消息，称为 **离队** 。
    /// + 当接收队列中没有消息时，立即通过 [`Err()`] 返回 [`XwmqError::NoMsg`] 。
    /// + 当接收队列中有可用的消息时，接收线程会取走一个消息，并释放消息槽。
    /// + 消息 **离队** 时，会通过 [`Box::from_raw()`] 将数据重新放入 [`Box`] 中，然后通过 [`Ok()`] 返回。
    ///
    /// # 上下文
    ///
    /// + 线程
    /// + 中断
    /// + 中断底半部
    ///
    /// # 错误码
    ///
    /// + [`XwmqError::Ok`] 没有错误
    /// + [`XwmqError::NotInit`] 消息队列没有初始化
    /// + [`XwmqError::NoMsg`] 消息队列中没有消息
    /// + [`XwmqError::NotThreadContext`] 不在线程上下文内
    ///
    /// # 示例
    ///
    /// ```rust
    /// extern crate alloc;
    /// use alloc::string::String;
    /// use alloc::string::ToString;
    ///
    /// use xwrust::xwmd::xwmq::*;
    ///
    /// pub static MQ: Xwmq<16, String> = Xwmq::new();
    ///
    /// pub fn xwrust_example_xwmq() {
    ///     let (tx, rx) = MQ.init();
    ///     // ...省略...
    ///     let rc = rx.trydq();
    ///         match rc {
    ///             Ok(d) => { // d是Box<String>
    ///             },
    ///             Err(e) => { // e是错误码
    ///             },
    ///         };
    ///     // ...省略...
    /// }
    /// ```
    ///
    /// [`Box`]: <https://doc.rust-lang.org/alloc/boxed/struct.Box.html>
    /// [`Box::from_raw()`]: <https://doc.rust-lang.org/alloc/boxed/struct.Box.html#method.from_raw>
    /// [`Ok()`]: <https://doc.rust-lang.org/core/result/enum.Result.html#variant.Ok>
    /// [`Err()`]: <https://doc.rust-lang.org/core/result/enum.Result.html#variant.Err>
    pub fn trydq(&self) -> Result<Box<T>, XwmqError> {
        unsafe {
            let mut rc = xwmq_acquire(self.xwmq.mq.get(), *self.xwmq.tik.get());
            if rc == 0 {
                let mut topic = 0;
                let mut raw: *mut c_void = ptr::null_mut();
                rc = xwmq_trydq(self.xwmq.mq.get(), &mut topic, &mut raw);
                xwmq_put(self.xwmq.mq.get());
                if XWOK == rc {
                    let boxdata = Box::from_raw(raw as *mut T);
                    Ok(boxdata)
                } else if -ENODATA == rc {
                    Err(XwmqError::NoMsg)
                } else {
                    Err(XwmqError::Unknown(rc))
                }
            } else {
                Err(XwmqError::NotInit)
            }
        }
    }
}
