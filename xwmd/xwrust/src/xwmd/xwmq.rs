//! XWOS RUST：消息队列
//! ========
//!
//! XWOS RUST的消息队列是仿照 [`std::sync::mpsc`]  编写的。可以有多个发送者，但只能有一个接收者。
//!
//! XWOS RUST的消息队列是一个先进先进（FIFO）队列。
//!
//! 消息队列在创建时，需要指定一个类型 `T` ，发送和接收的数据只能是类型 `T` 的数据。
//! 发送时，消息队列会通过 [`Box`] 将类型 `T` 的数据存入堆内存中。然后将 `Box<T>` 的裸指针发送到队列。
//! 可以发送到尾端（入队 [`XwmqTx::eq()`] ），也可将消息发送到队列的首端（插队 [`XwmqTx::jq()`] ）。
//!
//! 接收时，会从队列中取出 `T` 的裸指针，然后恢复成 `Box<T>` 后返回，由于 [`Box`] 能自动释放内存，因此用户不需要关心内存释放的问题。
//! 接收消息可以从首端接收，也可以从尾端接收。
//!
//!
//! # 创建
//!
//! XWOS RUST的消息队列可使用 [`Xwmq::new()`] 创建。创建时需要指定两个类型参数：发送消息槽的个数以及消息数据的类型。
//!
//! + 可以创建具有静态生命周期 [`static`] 约束的全局变量：
//!
//! ```rust
//! use xwrust::xwmd::xwmq::*;
//!
//! static MQ: Xwmq<16, String> = Xwmq::new(); // 16个消息槽，数据类型为String
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
//!     let mq: Arc<Xwmq<16, String>> = Arc::new(Xwmq::new()); // 16个消息槽，数据类型为 `String`
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
//! **入队** 方式是将消息发送到队列的 **尾端** ， **入队** 时，会通过 [`Box`] 将数据放入heap中，然后进行发送。
//!
//! ### 等待入队
//!
//! 方法 [`XwmqTx::eq()`] 只可在 **线程** 上下文中使用：
//!
//! + 当消息队列中没有可用的消息槽可用时，发送线程会阻塞等待。
//! + 当消息队列中有可用的消息槽时，发送线程被唤醒，并取走一个消息槽，并发送数据。
//! + 当线程阻塞等待被中断时，返回 [`XwmqError::Interrupt`] 。
//!
//! ### 超时等待入队
//!
//! 方法 [`XwmqTx::eq_to()`] 只可在 **线程** 上下文中使用：
//!
//! + 当消息队列中没有可用的消息槽可用时，发送线程会阻塞等待，等待时会指定一个唤醒时间点。
//! + 当消息队列中有可用的消息槽时，发送线程被唤醒，并取走一个消息槽，并发送数据。
//! + 当线程阻塞等待被中断时，返回 [`XwmqError::Interrupt`] 。
//! + 当到达指定的唤醒时间点，线程被唤醒，并返回 [`XwmqError::Timedout`] 。
//!
//! ### 等待入队，且等待不可不中断
//!
//! 方法 [`XwmqTx::eq_unintr()`] 只可在 **线程** 上下文中使用：
//!
//! + 当消息队列中没有可用的消息槽可用时，发送线程会阻塞等待。
//! + 发送线程的阻塞等待不可被中断。
//! + 当消息队列中有可用的消息槽时，发送线程被唤醒，并取走一个消息槽，并发送数据。
//!
//! ### 尝试等待入队
//!
//! 方法 [`XwmqTx::tryeq()`] 方法可在 **任意** 上下文中使用，不会阻塞，只会检测信号量的值：
//!
//! + 当消息队列中没有可用的消息槽可用时，立即返回 [`XwmqError::NoSlot`] 。
//! + 当消息队列中有可用的消息槽时，发送线程会取走一个消息槽，并发送数据。
//!
//! ## 插队
//!
//! **插队** 方式是将消息发送到队列的 **首端** ， **插队** 时，会通过 [`Box`] 将数据放入heap中，然后进行发送。
//!
//! ### 等待插队
//!
//! 方法 [`XwmqTx::jq()`] 只可在 **线程** 上下文中使用：
//!
//! + 当消息队列中没有可用的消息槽可用时，发送线程会阻塞等待。
//! + 当消息队列中有可用的消息槽时，发送线程被唤醒，并取走一个消息槽，并发送数据。
//! + 当线程阻塞等待被中断时，返回 [`XwmqError::Interrupt`] 。
//!
//! ### 超时等待插队
//!
//! 方法 [`XwmqTx::jq_to()`] 只可在 **线程** 上下文中使用：
//!
//! + 当消息队列中没有可用的消息槽可用时，发送线程会阻塞等待，等待时会指定一个唤醒时间点。
//! + 当消息队列中有可用的消息槽时，发送线程被唤醒，并取走一个消息槽，并发送数据。
//! + 当线程阻塞等待被中断时，返回 [`XwmqError::Interrupt`] 。
//! + 当到达指定的唤醒时间点，线程被唤醒，并返回 [`XwmqError::Timedout`] 。
//!
//! 方法 [`XwmqTx::jq_unintr()`] 只可在 **线程** 上下文中使用：
//!
//! + 当消息队列中没有可用的消息槽可用时，发送线程会阻塞等待。
//! + 发送线程的阻塞等待不可被中断。
//! + 当消息队列中有可用的消息槽时，发送线程被唤醒，并取走一个消息槽，并发送数据。
//!
//! ### 尝试等待插队
//!
//! 方法 [`XwmqTx::tryjq()`] 方法可在 **任意** 上下文中使用，不会阻塞，只会检测信号量的值：
//!
//! + 当消息队列中没有可用的消息槽可用时，立即返回 [`XwmqError::NoSlot`] 。
//! + 当消息队列中有可用的消息槽时，发送线程会取走一个消息槽，并发送数据。
//!
//!
//! # 接收
//!
//! [`Xwmq`] 的接收者 [`XwmqRx`] 具有 [`Send`] 约束，可在线程之间转移所有权，但不能在线程之间共享( [`!Sync`] )。
//!
//! 接收者 [`XwmqRx`] 不可被 ”**克隆**“，因为接收者只能有一个。
//!
//! 接收消息后，会释放消息槽。若有发送线程正在等待消息槽，将唤醒发送线程。
//!
//! ## 首端离队
//!
//! 从消息队列的 **首端** 取出消息，称为 **首端离队** 。
//! 接收成功，消息 **离队** 时，会通过 [`Box::from_raw()`] 将数据重新放入 [`Box`] 中，然后通过 [`Ok()`] 返回。
//! 当 [`Box`] 被 [`drop()`] 时，会自动释放heap内存。
//!
//! ### 等待离队
//!
//! 方法 [`XwmqRx::dq()`] 只可在 **线程** 上下文中使用：
//!
//! + 当消息队列中没有消息时，接收线程会阻塞等待。
//! + 当消息队列中有可用的消息时，接收线程被唤醒，并取走一个消息，并释放消息槽。
//! + 当线程阻塞等待被中断时，通过 [`Err()`] 返回 [`XwmqError::Interrupt`] 。
//!
//! ### 超时等待离队
//!
//! 方法 [`XwmqRx::dq_to()`] 只可在 **线程** 上下文中使用：
//!
//! + 当消息队列中没有消息时，接收线程会阻塞等待，等待时会指定一个唤醒时间点。
//! + 当消息队列中有可用的消息时，接收线程被唤醒，并取走一个消息，释放消息槽。
//! + 当线程阻塞等待被中断时，通过 [`Err()`] 返回 [`XwmqError::Interrupt`] 。
//! + 当到达指定的唤醒时间点，线程被唤醒，并通过 [`Err()`] 返回 [`XwmqError::Timedout`] 。
//!
//! 方法 [`XwmqRx::dq_unintr()`] 只可在 **线程** 上下文中使用：
//!
//! + 当消息队列中没有消息时，接收线程会阻塞等待。
//! + 接收线程的阻塞等待不可被中断。
//! + 当消息队列中有可用的消息时，接收线程被唤醒，并取走一个消息，并释放消息槽。
//!
//! ### 尝试等待离队
//!
//! 方法 [`XwmqRx::trydq()`] 方法可在 **任意** 上下文中使用，不会阻塞，只会检测信号量的值：
//!
//! + 当消息队列中没有消息时，立即通过 [`Err()`] 返回 [`XwmqError::NoMsg`] 。
//! + 当消息队列中有可用的消息时，接收线程会取走一个消息，并释放消息槽。
//!
//! ## 尾端离队
//!
//! 从消息队列的 **尾端** 取出消息，称为 **尾端离队** 。
//! 接收成功，消息 **离队** 时，会通过 [`Box::from_raw()`] 将数据重新放入 [`Box`] 中，
//! 然后通过 [`Ok()`] 返回。
//! 当 [`Box`] 被 [`drop()`] 时，会自动释放heap内存。
//!
//! ### 等待反向离队
//!
//! 方法 [`XwmqRx::rq()`] 只可在 **线程** 上下文中使用：
//!
//! + 当消息队列中没有消息时，接收线程会阻塞等待。
//! + 当消息队列中有可用的消息时，接收线程被唤醒，并取走一个消息，并释放消息槽。
//! + 当线程阻塞等待被中断时，通过 [`Err()`] 返回 [`XwmqError::Interrupt`] 。
//!
//! ### 超时等待反向离队
//!
//! 方法 [`XwmqRx::rq_to()`] 只可在 **线程** 上下文中使用：
//!
//! + 当消息队列中没有消息时，接收线程会阻塞等待，等待时会指定一个唤醒时间点。
//! + 当消息队列中有可用的消息时，接收线程被唤醒，并取走一个消息，释放消息槽。
//! + 当线程阻塞等待被中断时，通过 [`Err()`] 返回 [`XwmqError::Interrupt`] 。
//! + 当到达指定的唤醒时间点，线程被唤醒，并通过 [`Err()`] 返回 [`XwmqError::Timedout`] 。
//!
//! 方法 [`XwmqRx::rq_unintr()`] 只可在 **线程** 上下文中使用：
//!
//! + 当消息队列中没有消息时，接收线程会阻塞等待。
//! + 接收线程的阻塞等待不可被中断。
//! + 当消息队列中有可用的消息时，接收线程被唤醒，并取走一个消息，并释放消息槽。
//!
//! ### 尝试等待反向离队
//!
//! 方法 [`XwmqRx::tryrq()`] 方法可在 **任意** 上下文中使用，不会阻塞，只会检测信号量的值：
//!
//! + 当消息队列中没有消息时，立即通过 [`Err()`] 返回 [`XwmqError::NoMsg`] 。
//! + 当消息队列中有可用的消息时，接收线程会取走一个消息，并释放消息槽。
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
//! static MQ: Xwmq<16, u32> = Xwmq::new();
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
//! + [`std::sync::mpsc`] 没有编译期构造函数，因此只能借助
//!   宏 [`lazy_static!`] 创建全局变量；
//! + [`xwrust::xwmd::xwmq`] 则可以直接在函数外部定义。
//!
//!
//! ### 通道类型
//!
//! + [`std::sync::mpsc`] 还有异步的通道 [`std::sync::mpsc::channel()`] ；
//! + [`xwrust::xwmd::xwmq`] 通过 [`Xwmq::init()`] 返回的是类似于
//!   [`std::sync::mpsc::sync_channel()`] 的通道，
//!   [`xwrust::xwmd::xwmq`] 没有异步的通道 。
//!
//! ### 接收消息返回方式不同
//!
//! + [`std::sync::mpsc`] 接收到消息后，直接返回类型为 **T** 的消息；
//! + [`xwrust::xwmd::xwmq`] 则返回 `Box<T>` 。
//!
//! [`std::sync::mpsc`]: <https://doc.rust-lang.org/std/sync/mpsc/index.html>
//! [`std::sync::mpsc::channel()`]: <https://doc.rust-lang.org/std/sync/mpsc/fn.channel.html>
//! [`std::sync::mpsc::sync_channel()`]: <https://doc.rust-lang.org/std/sync/mpsc/fn.sync_channel.html>
//! [`static`]: <https://doc.rust-lang.org/std/keyword.static.html>
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
    fn xwmq_get_tik(mq: *mut XwmdXwmq) -> XwSq;
    fn xwrustffi_xwmq_acquire(mq: *mut XwmdXwmq, tik: XwSq) -> XwEr;
    fn xwrustffi_xwmq_release(mq: *mut XwmdXwmq, tik: XwSq) -> XwEr;

    fn xwmq_eq(mq: *mut XwmdXwmq, topic: XwSq, data: *mut c_void) -> XwEr;
    fn xwmq_eq_to(mq: *mut XwmdXwmq, topic: XwSq, data: *mut c_void, to: XwTm) -> XwEr;
    fn xwmq_tryeq(mq: *mut XwmdXwmq, topic: XwSq, data: *mut c_void) -> XwEr;
    fn xwmq_eq_unintr(mq: *mut XwmdXwmq, topic: XwSq, data: *mut c_void) -> XwEr;
    fn xwmq_jq(mq: *mut XwmdXwmq, topic: XwSq, data: *mut c_void) -> XwEr;
    fn xwmq_jq_to(mq: *mut XwmdXwmq, topic: XwSq, data: *mut c_void, to: XwTm) -> XwEr;
    fn xwmq_jq_unintr(mq: *mut XwmdXwmq, topic: XwSq, data: *mut c_void) -> XwEr;
    fn xwmq_tryjq(mq: *mut XwmdXwmq, topic: XwSq, data: *mut c_void) -> XwEr;
    fn xwmq_dq(mq: *mut XwmdXwmq, topic: *mut XwSq, data: *mut *mut c_void) -> XwEr;
    fn xwmq_dq_to(mq: *mut XwmdXwmq, topic: *mut XwSq, data: *mut *mut c_void, to: XwTm) -> XwEr;
    fn xwmq_dq_unintr(mq: *mut XwmdXwmq, topic: *mut XwSq, data: *mut *mut c_void) -> XwEr;
    fn xwmq_trydq(mq: *mut XwmdXwmq, topic: *mut XwSq, data: *mut *mut c_void) -> XwEr;
    fn xwmq_rq(mq: *mut XwmdXwmq, topic: *mut XwSq, data: *mut *mut c_void) -> XwEr;
    fn xwmq_rq_to(mq: *mut XwmdXwmq, topic: *mut XwSq, data: *mut *mut c_void, to: XwTm) -> XwEr;
    fn xwmq_rq_unintr(mq: *mut XwmdXwmq, topic: *mut XwSq, data: *mut *mut c_void) -> XwEr;
    fn xwmq_tryrq(mq: *mut XwmdXwmq, topic: *mut XwSq, data: *mut *mut c_void) -> XwEr;
}

/// 消息队列的错误码
#[derive(Debug)]
pub enum XwmqError {
    /// 没有错误
    Ok(XwEr),
    /// 消息队列没有初始化
    NotInit(XwEr),
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
    /// 消息队列中没有可用的消息槽
    NoSlot(XwEr),
    /// 消息队列中没有消息
    NoMsg(XwEr),
    /// 未知错误
    Unknown(XwEr),
}

impl XwmqError {
    /// 消费掉 `XwmqError` 自身，返回内部的错误码。
    pub fn unwrap(self) -> XwEr {
        match self {
            Self::Ok(rc) => rc,
            Self::NotInit(rc) => rc,
            Self::Interrupt(rc) => rc,
            Self::Timedout(rc) => rc,
            Self::NotThreadContext(rc) => rc,
            Self::DisPmpt(rc) => rc,
            Self::DisBh(rc) => rc,
            Self::DisIrq(rc) => rc,
            Self::NoSlot(rc) => rc,
            Self::NoMsg(rc) => rc,
            Self::Unknown(rc) => rc,
        }
    }

    /// 如果信号量的错误码是 [`XwmqError::Ok`] ，返回 `true` 。
    pub const fn is_ok(&self) -> bool {
        matches!(*self, Self::Ok(_))
    }

    /// 如果信号量的错误码不是 [`XwmqError::Ok`] ，返回 `true` 。
    pub const fn is_err(&self) -> bool {
        !self.is_ok()
    }
}

/// 消息队列对象占用的内存大小
#[cfg(target_pointer_width = "32")]
pub const SIZEOF_XWMQ: usize = 200;

/// 消息占用的内存大小
#[cfg(target_pointer_width = "32")]
pub const SIZEOF_XWMQ_MSG: usize = 16;

/// 消息队列对象占用的内存大小
#[cfg(target_pointer_width = "64")]
pub const SIZEOF_XWMQ: usize = 400;

/// 消息占用的内存大小
#[cfg(target_pointer_width = "64")]
pub const SIZEOF_XWMQ_MSG: usize = 32;

/// 用于构建消息队列的内存数组类型
#[repr(C)]
#[cfg_attr(target_pointer_width = "32", repr(align(8)))]
#[cfg_attr(target_pointer_width = "64", repr(align(16)))]
pub(crate) struct XwmdXwmq {
    pub(crate) obj: [u8; SIZEOF_XWMQ],
}

/// 用于构建消息槽的内存数组类型
#[repr(C)]
#[cfg_attr(target_pointer_width = "32", repr(align(8)))]
#[cfg_attr(target_pointer_width = "64", repr(align(16)))]
pub(crate) struct XwmdXwmqMsg<const N: XwSz>
where
    [u8; N * SIZEOF_XWMQ_MSG]: Sized
{
    pub(crate) obj: [u8; N * SIZEOF_XWMQ_MSG],
}

/// 用于构建消息队列的内存数组常量
///
/// 此常量的作用是告诉编译器消息队列对象需要多大的内存。
pub(crate) const XWMQ_INITIALIZER: XwmdXwmq = XwmdXwmq {
    obj: [0; SIZEOF_XWMQ],
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
    [u8; N * SIZEOF_XWMQ_MSG]: Sized
{}

unsafe impl<const N: XwSz, T> Sync for Xwmq<N, T>
where
    [u8; N * SIZEOF_XWMQ_MSG]: Sized
{}

impl<const N: XwSz, T> Xwmq<N, T>
where
    [u8; N * SIZEOF_XWMQ_MSG]: Sized
{
    /// 新建消息队列对象
    ///
    /// 此方法是编译期方法。
    ///
    /// # 示例
    ///
    /// + 具有 [`static`] 约束的全局变量全局变量：
    ///
    /// ```rust
    /// use xwrust::xwmd::xwmq::*;
    ///
    /// static MQ: Xwmq<16, String> = Xwmq::new(); // 16个消息槽，数据类型为String
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
    /// [`static`]: https://doc.rust-lang.org/std/keyword.static.html
    pub const fn new() -> Self {
        Self {
            mq: UnsafeCell::new(XWMQ_INITIALIZER),
            pool: UnsafeCell::new(XwmdXwmqMsg { obj: [0; N * SIZEOF_XWMQ_MSG] }),
            tik: UnsafeCell::new(0),
            phantom: PhantomData,
        }
    }

    /// 初始化消息队列对象
    ///
    /// 消息队列对象必须调用此方法一次，方可正常使用。
    /// 初始化后会返回消息队列的发送者 [`XwmqTx`] 以及接收者 [`XwmqRx`] 。
    ///
    /// # 示例
    ///
    /// ```rust
    /// use xwrust::xwmd::xwmq::*;
    ///
    /// static MQ: Xwmq<16, String> = Xwmq::new();
    ///
    /// pub fn xwrust_example_xwmq() {
    ///     let (tx, rx) = MQ.init();
    /// }
    /// ```
    pub fn init<'a>(&'a self) -> (XwmqTx<'a, N, T>, XwmqRx<'a, N, T>) {
        unsafe {
            xwmq_init(self.mq.get(), self.pool.get() as _, N);
            *self.tik.get() = xwmq_get_tik(self.mq.get());
        }
        (XwmqTx {xwmq: &self}, XwmqRx {xwmq: &self})
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
    /// 等待消息槽，成功后将消息发送到消息队列的 **尾端** （入队）
    ///
    /// + 当消息队列中没有可用的消息槽可用时，发送线程会阻塞等待。
    /// + 当消息队列中有可用的消息槽时，发送线程被唤醒，并取走一个消息槽，用于发送数据。
    /// + 消息 **入队** 时，会通过 [`Box`] 将数据放入heap中，然后进行发送。
    /// + 当发送线程阻塞等待被中断时，返回 [`XwmqError::Interrupt`] 。
    ///
    /// # 参数说明
    ///
    /// + data: 类型为 `T` 的数据
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
    /// + [`XwmqError::DisPmpt`] 抢占被关闭
    /// + [`XwmqError::DisBh`] 中断底半部被关闭
    /// + [`XwmqError::DisIrq`] 中断被关闭
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
    /// static MQ: Xwmq<16, String> = Xwmq::new();
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
            let mut rc = xwrustffi_xwmq_acquire(self.xwmq.mq.get(), *self.xwmq.tik.get());
            if rc == 0 {
                rc = xwmq_eq(self.xwmq.mq.get(), 0, raw as *mut c_void);
                xwmq_put(self.xwmq.mq.get());
                if XWOK == rc {
                    XwmqError::Ok(rc)
                } else if  -EINTR == rc {
                    XwmqError::Interrupt(rc)
                } else if -ENOTTHDCTX == rc {
                    XwmqError::NotThreadContext(rc)
                } else if -EDISPMPT == rc {
                    XwmqError::DisPmpt(rc)
                } else if -EDISBH == rc {
                    XwmqError::DisBh(rc)
                } else if -EDISIRQ == rc {
                    XwmqError::DisIrq(rc)
                } else {
                    XwmqError::Unknown(rc)
                }
            } else {
                XwmqError::NotInit(rc)
            }
        }
    }

    /// 限时等待消息槽，成功后将消息发送到消息队列的 **尾端** （入队）
    ///
    /// + 当消息队列中没有可用的消息槽可用时，发送线程会阻塞等待，等待时会指定一个唤醒时间点。
    /// + 当消息队列中有可用的消息槽时，发送线程被唤醒，并取走一个消息槽，用于发送数据。
    /// + 消息 **入队** 时，会通过 [`Box`] 将数据放入heap中，然后进行发送。
    /// + 当发送线程阻塞等待被中断时，返回 [`XwmqError::Interrupt`] 。
    /// + 当到达指定的唤醒时间点，线程被唤醒，并返回 [`XwmqError::Timedout`] 。
    ///
    /// # 参数说明
    ///
    /// + data: 类型为 `T` 的数据
    /// + to: 期望唤醒的时间点
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
    /// + [`XwmqError::DisPmpt`] 抢占被关闭
    /// + [`XwmqError::DisBh`] 中断底半部被关闭
    /// + [`XwmqError::DisIrq`] 中断被关闭
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
    /// static MQ: Xwmq<16, String> = Xwmq::new();
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
            let mut rc = xwrustffi_xwmq_acquire(self.xwmq.mq.get(), *self.xwmq.tik.get());
            if rc == 0 {
                rc = xwmq_eq_to(self.xwmq.mq.get(), 0, raw as *mut c_void, to);
                xwmq_put(self.xwmq.mq.get());
                if XWOK == rc {
                    XwmqError::Ok(rc)
                } else if  -EINTR == rc {
                    XwmqError::Interrupt(rc)
                } else if -ETIMEDOUT == rc {
                    XwmqError::Timedout(rc)
                } else if -ENOTTHDCTX == rc {
                    XwmqError::NotThreadContext(rc)
                } else if -EDISPMPT == rc {
                    XwmqError::DisPmpt(rc)
                } else if -EDISBH == rc {
                    XwmqError::DisBh(rc)
                } else if -EDISIRQ == rc {
                    XwmqError::DisIrq(rc)
                } else {
                    XwmqError::Unknown(rc)
                }
            } else {
                XwmqError::NotInit(rc)
            }
        }
    }

    /// 等待消息槽，且等待不可被中断，成功后将消息发送到消息队列的 **尾端** （入队）
    ///
    /// + 当消息队列中没有可用的消息槽可用时，发送线程会阻塞等待。
    /// + 发送线程的阻塞等待不可被中断。
    /// + 当消息队列中有可用的消息槽时，发送线程被唤醒，并取走一个消息槽，用于发送数据。
    /// + 消息 **入队** 时，会通过 [`Box`] 将数据放入heap中，然后进行发送。
    ///
    /// # 参数说明
    ///
    /// + data: 类型为 `T` 的数据
    ///
    /// # 上下文
    ///
    /// + 线程
    ///
    /// # 错误码
    ///
    /// + [`XwmqError::Ok`] 没有错误
    /// + [`XwmqError::NotInit`] 消息队列没有初始化
    /// + [`XwmqError::NotThreadContext`] 不在线程上下文内
    /// + [`XwmqError::DisPmpt`] 抢占被关闭
    /// + [`XwmqError::DisBh`] 中断底半部被关闭
    /// + [`XwmqError::DisIrq`] 中断被关闭
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
    /// static MQ: Xwmq<16, String> = Xwmq::new();
    ///
    /// pub fn xwrust_example_xwmq() {
    ///     let (tx, rx) = MQ.init();
    ///     // ...省略...
    ///     tx.eq_unintr("message".to_string());
    ///     // ...省略...
    /// }
    /// ```
    ///
    /// [`Box`]: <https://doc.rust-lang.org/alloc/boxed/struct.Box.html>
    pub fn eq_unintr(&self, data: T) -> XwmqError {
        let boxdata = Box::new(data);
        let raw = Box::into_raw(boxdata);
        unsafe {
            let mut rc = xwrustffi_xwmq_acquire(self.xwmq.mq.get(), *self.xwmq.tik.get());
            if rc == 0 {
                rc = xwmq_eq_unintr(self.xwmq.mq.get(), 0, raw as *mut c_void);
                xwmq_put(self.xwmq.mq.get());
                if XWOK == rc {
                    XwmqError::Ok(rc)
                } else if -ENOTTHDCTX == rc {
                    XwmqError::NotThreadContext(rc)
                } else if -EDISPMPT == rc {
                    XwmqError::DisPmpt(rc)
                } else if -EDISBH == rc {
                    XwmqError::DisBh(rc)
                } else if -EDISIRQ == rc {
                    XwmqError::DisIrq(rc)
                } else {
                    XwmqError::Unknown(rc)
                }
            } else {
                XwmqError::NotInit(rc)
            }
        }
    }

    /// 尝试获取消息槽，成功后将消息发送到消息队列的 **尾端** （入队）
    ///
    /// + 当消息队列中没有可用的消息槽可用时，立即返回 [`XwmqError::NoSlot`] 。
    /// + 当消息队列中有可用的消息槽时，取走一个消息槽，用于发送数据。
    /// + 消息 **入队** 时，会通过 [`Box`] 将数据放入heap中，然后进行发送。
    ///
    /// # 参数说明
    ///
    /// + data: 类型为 `T` 的数据
    ///
    /// # 上下文
    ///
    /// + 任意
    ///
    /// # 错误码
    ///
    /// + [`XwmqError::Ok`] 没有错误
    /// + [`XwmqError::NotInit`] 消息队列没有初始化
    /// + [`XwmqError::NoSlot`] 消息队列中没有可用的消息槽
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
    /// static MQ: Xwmq<16, String> = Xwmq::new();
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
            let mut rc = xwrustffi_xwmq_acquire(self.xwmq.mq.get(), *self.xwmq.tik.get());
            if rc == 0 {
                rc = xwmq_tryeq(self.xwmq.mq.get(), 0, raw as *mut c_void);
                xwmq_put(self.xwmq.mq.get());
                if XWOK == rc {
                    XwmqError::Ok(rc)
                } else if  -ENODATA == rc {
                    XwmqError::NoSlot(rc)
                } else {
                    XwmqError::Unknown(rc)
                }
            } else {
                XwmqError::NotInit(rc)
            }
        }
    }

    /// 等待消息槽，成功后将消息发送到消息队列的 **首端** （插队）
    ///
    /// + 当消息队列中没有可用的消息槽可用时，发送线程会阻塞等待。
    /// + 当消息队列中有可用的消息槽时，发送线程被唤醒，并取走一个消息槽，用于发送数据。
    /// + 消息 **入队** 时，会通过 [`Box`] 将数据放入heap中，然后进行发送。
    /// + 当发送线程阻塞等待被中断时，返回 [`XwmqError::Interrupt`] 。
    ///
    /// # 参数说明
    ///
    /// + data: 类型为 `T` 的数据
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
    /// + [`XwmqError::DisPmpt`] 抢占被关闭
    /// + [`XwmqError::DisBh`] 中断底半部被关闭
    /// + [`XwmqError::DisIrq`] 中断被关闭
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
    /// static MQ: Xwmq<16, String> = Xwmq::new();
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
            let mut rc = xwrustffi_xwmq_acquire(self.xwmq.mq.get(), *self.xwmq.tik.get());
            if rc == 0 {
                rc = xwmq_jq(self.xwmq.mq.get(), 0, raw as *mut c_void);
                xwmq_put(self.xwmq.mq.get());
                if XWOK == rc {
                    XwmqError::Ok(rc)
                } else if  -EINTR == rc {
                    XwmqError::Interrupt(rc)
                } else if -ENOTTHDCTX == rc {
                    XwmqError::NotThreadContext(rc)
                } else if -EDISPMPT == rc {
                    XwmqError::DisPmpt(rc)
                } else if -EDISBH == rc {
                    XwmqError::DisBh(rc)
                } else if -EDISIRQ == rc {
                    XwmqError::DisIrq(rc)
                } else {
                    XwmqError::Unknown(rc)
                }
            } else {
                XwmqError::NotInit(rc)
            }
        }
    }

    /// 限时等待消息槽，成功后将消息发送到消息队列的 **首端** （插队）
    ///
    /// + 当消息队列中没有可用的消息槽可用时，发送线程会阻塞等待，等待时会指定一个唤醒时间点。
    /// + 当消息队列中有可用的消息槽时，发送线程被唤醒，并取走一个消息槽，用于发送数据。
    /// + 消息 **入队** 时，会通过 [`Box`] 将数据放入heap中，然后进行发送。
    /// + 当发送线程阻塞等待被中断时，返回 [`XwmqError::Interrupt`] 。
    /// + 当到达指定的唤醒时间点，线程被唤醒，并返回 [`XwmqError::Timedout`] 。
    ///
    /// # 参数说明
    ///
    /// + data: 类型为 `T` 的数据
    /// + to: 期望唤醒的时间点
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
    /// + [`XwmqError::DisPmpt`] 抢占被关闭
    /// + [`XwmqError::DisBh`] 中断底半部被关闭
    /// + [`XwmqError::DisIrq`] 中断被关闭
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
    /// static MQ: Xwmq<16, String> = Xwmq::new();
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
            let mut rc = xwrustffi_xwmq_acquire(self.xwmq.mq.get(), *self.xwmq.tik.get());
            if rc == 0 {
                rc = xwmq_jq_to(self.xwmq.mq.get(), 0, raw as *mut c_void, to);
                xwmq_put(self.xwmq.mq.get());
                if XWOK == rc {
                    XwmqError::Ok(rc)
                } else if  -EINTR == rc {
                    XwmqError::Interrupt(rc)
                } else if -ETIMEDOUT == rc {
                    XwmqError::Timedout(rc)
                } else if -ENOTTHDCTX == rc {
                    XwmqError::NotThreadContext(rc)
                } else if -EDISPMPT == rc {
                    XwmqError::DisPmpt(rc)
                } else if -EDISBH == rc {
                    XwmqError::DisBh(rc)
                } else if -EDISIRQ == rc {
                    XwmqError::DisIrq(rc)
                } else {
                    XwmqError::Unknown(rc)
                }
            } else {
                XwmqError::NotInit(rc)
            }
        }
    }

    /// 等待消息槽，且等待不可被中断，成功后将消息发送到消息队列的 **首端** （插队）
    ///
    /// + 当消息队列中没有可用的消息槽可用时，发送线程会阻塞等待。
    /// + 发送线程的阻塞等待不可被中断。
    /// + 当消息队列中有可用的消息槽时，发送线程被唤醒，并取走一个消息槽，用于发送数据。
    /// + 消息 **入队** 时，会通过 [`Box`] 将数据放入heap中，然后进行发送。
    ///
    /// # 参数说明
    ///
    /// + data: 类型为 `T` 的数据
    ///
    /// # 上下文
    ///
    /// + 线程
    ///
    /// # 错误码
    ///
    /// + [`XwmqError::Ok`] 没有错误
    /// + [`XwmqError::NotInit`] 消息队列没有初始化
    /// + [`XwmqError::NotThreadContext`] 不在线程上下文内
    /// + [`XwmqError::DisPmpt`] 抢占被关闭
    /// + [`XwmqError::DisBh`] 中断底半部被关闭
    /// + [`XwmqError::DisIrq`] 中断被关闭
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
    /// static MQ: Xwmq<16, String> = Xwmq::new();
    ///
    /// pub fn xwrust_example_xwmq() {
    ///     let (tx, rx) = MQ.init();
    ///     // ...省略...
    ///     tx.jq_unintr("message".to_string());
    ///     // ...省略...
    /// }
    /// ```
    ///
    /// [`Box`]: <https://doc.rust-lang.org/alloc/boxed/struct.Box.html>
    pub fn jq_unintr(&self, data: T) -> XwmqError {
        let boxdata = Box::new(data);
        let raw = Box::into_raw(boxdata);
        unsafe {
            let mut rc = xwrustffi_xwmq_acquire(self.xwmq.mq.get(), *self.xwmq.tik.get());
            if rc == 0 {
                rc = xwmq_jq_unintr(self.xwmq.mq.get(), 0, raw as *mut c_void);
                xwmq_put(self.xwmq.mq.get());
                if XWOK == rc {
                    XwmqError::Ok(rc)
                } else if -ENOTTHDCTX == rc {
                    XwmqError::NotThreadContext(rc)
                } else if -EDISPMPT == rc {
                    XwmqError::DisPmpt(rc)
                } else if -EDISBH == rc {
                    XwmqError::DisBh(rc)
                } else if -EDISIRQ == rc {
                    XwmqError::DisIrq(rc)
                } else {
                    XwmqError::Unknown(rc)
                }
            } else {
                XwmqError::NotInit(rc)
            }
        }
    }

    /// 尝试获取消息槽，成功后将消息发送到消息队列的 **首端** （插队）
    ///
    /// + 当消息队列中没有可用的消息槽可用时，立即返回 [`XwmqError::NoSlot`] 。
    /// + 当消息队列中有可用的消息槽时，取走一个消息槽，用于发送数据。
    /// + 消息 **入队** 时，会通过 [`Box`] 将数据放入heap中，然后进行发送。
    ///
    /// # 参数说明
    ///
    /// + data: 类型为 `T` 的数据
    ///
    /// # 上下文
    ///
    /// + 任意
    ///
    /// # 错误码
    ///
    /// + [`XwmqError::Ok`] 没有错误
    /// + [`XwmqError::NotInit`] 消息队列没有初始化
    /// + [`XwmqError::NoSlot`] 消息队列中没有可用的消息槽
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
    /// static MQ: Xwmq<16, String> = Xwmq::new();
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
            let mut rc = xwrustffi_xwmq_acquire(self.xwmq.mq.get(), *self.xwmq.tik.get());
            if rc == 0 {
                rc = xwmq_tryjq(self.xwmq.mq.get(), 0, raw as *mut c_void);
                xwmq_put(self.xwmq.mq.get());
                if XWOK == rc {
                    XwmqError::Ok(rc)
                } else if  -ENODATA == rc {
                    XwmqError::NoSlot(rc)
                } else {
                    XwmqError::Unknown(rc)
                }
            } else {
                XwmqError::NotInit(rc)
            }
        }
    }
}

impl<'a, const N: XwSz, T> XwmqRx<'a, N, T>
where
    [u8; N * SIZEOF_XWMQ_MSG]: Sized
{
    /// 等待从消息队列 **首端** 接收一条消息
    ///
    /// + 从消息队列的 **首端** 取出消息，称为 **离队** 。
    /// + 当消息队列中没有消息时，接收线程会阻塞等待。
    /// + 当消息队列中有可用的消息时，接收线程被唤醒，并取走一个消息，释放消息槽。
    ///   若有发送线程正在等待消息槽，将唤醒发送线程。
    /// + 消息 **离队** 时，会通过 [`Box::from_raw()`] 将数据重新放入 [`Box`] 中，然后通过 [`Ok()`] 返回。
    /// + 当线程阻塞等待被中断时，通过 [`Err()`] 返回 [`XwmqError::Interrupt`] 。
    ///
    /// # 上下文
    ///
    /// + 线程
    ///
    /// # 错误码
    ///
    /// + [`XwmqError::NotInit`] 消息队列没有初始化
    /// + [`XwmqError::Interrupt`] 等待被中断
    /// + [`XwmqError::NotThreadContext`] 不在线程上下文内
    /// + [`XwmqError::DisPmpt`] 抢占被关闭
    /// + [`XwmqError::DisBh`] 中断底半部被关闭
    /// + [`XwmqError::DisIrq`] 中断被关闭
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
    /// static MQ: Xwmq<16, String> = Xwmq::new();
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
            let mut rc = xwrustffi_xwmq_acquire(self.xwmq.mq.get(), *self.xwmq.tik.get());
            if rc == 0 {
                let mut topic = 0;
                let mut raw: *mut c_void = ptr::null_mut();
                rc = xwmq_dq(self.xwmq.mq.get(), &mut topic, &mut raw);
                xwmq_put(self.xwmq.mq.get());
                if XWOK == rc {
                    let boxdata = Box::from_raw(raw as *mut T);
                    Ok(boxdata)
                } else if -EINTR == rc {
                    Err(XwmqError::Interrupt(rc))
                } else if -ENOTTHDCTX == rc {
                    Err(XwmqError::NotThreadContext(rc))
                } else if -EDISPMPT == rc {
                    Err(XwmqError::DisPmpt(rc))
                } else if -EDISBH == rc {
                    Err(XwmqError::DisBh(rc))
                } else if -EDISIRQ == rc {
                    Err(XwmqError::DisIrq(rc))
                } else {
                    Err(XwmqError::Unknown(rc))
                }
            } else {
                Err(XwmqError::NotInit(rc))
            }
        }
    }

    /// 限时等待从消息队列 **首端** 接收一条消息
    ///
    /// + 从消息队列的 **首端** 取出消息，称为 **离队** 。
    /// + 当消息队列中没有消息时，接收线程会阻塞等待，等待时会指定一个唤醒时间点。
    /// + 当消息队列中有可用的消息时，接收线程被唤醒，并取走一个消息，释放消息槽。
    ///   若有发送线程正在等待消息槽，将唤醒发送线程。
    /// + 消息 **离队** 时，会通过 [`Box::from_raw()`] 将数据重新放入 [`Box`] 中，然后通过 [`Ok()`] 返回。
    /// + 当线程阻塞等待被中断时，通过 [`Err()`] 返回 [`XwmqError::Interrupt`] 。
    /// + 当到达指定的唤醒时间点，线程被唤醒，并通过 [`Err()`] 返回 [`XwmqError::Timedout`] 。
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
    /// + [`XwmqError::NotInit`] 消息队列没有初始化
    /// + [`XwmqError::Interrupt`] 等待被中断
    /// + [`XwmqError::Timedout`] 等待超时
    /// + [`XwmqError::NotThreadContext`] 不在线程上下文内
    /// + [`XwmqError::DisPmpt`] 抢占被关闭
    /// + [`XwmqError::DisBh`] 中断底半部被关闭
    /// + [`XwmqError::DisIrq`] 中断被关闭
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
    /// static MQ: Xwmq<16, String> = Xwmq::new();
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
            let mut rc = xwrustffi_xwmq_acquire(self.xwmq.mq.get(), *self.xwmq.tik.get());
            if rc == 0 {
                let mut topic = 0;
                let mut raw: *mut c_void = ptr::null_mut();
                rc = xwmq_dq_to(self.xwmq.mq.get(), &mut topic, &mut raw, to);
                xwmq_put(self.xwmq.mq.get());
                if XWOK == rc {
                    let boxdata = Box::from_raw(raw as *mut T);
                    Ok(boxdata)
                } else if -EINTR == rc {
                    Err(XwmqError::Interrupt(rc))
                } else if -ETIMEDOUT == rc {
                    Err(XwmqError::Timedout(rc))
                } else if -ENOTTHDCTX == rc {
                    Err(XwmqError::NotThreadContext(rc))
                } else if -EDISPMPT == rc {
                    Err(XwmqError::DisPmpt(rc))
                } else if -EDISBH == rc {
                    Err(XwmqError::DisBh(rc))
                } else if -EDISIRQ == rc {
                    Err(XwmqError::DisIrq(rc))
                } else {
                    Err(XwmqError::Unknown(rc))
                }
            } else {
                Err(XwmqError::NotInit(rc))
            }
        }
    }

    /// 等待从消息队列 **首端** 接收一条消息，且等待不可被中断
    ///
    /// + 从消息队列的 **首端** 取出消息，称为 **离队** 。
    /// + 当消息队列中没有消息时，接收线程会阻塞等待。
    /// + 接收线程的阻塞等待不可被中断。
    /// + 当消息队列中有可用的消息时，接收线程被唤醒，并取走一个消息，释放消息槽。
    ///   若有发送线程正在等待消息槽，将唤醒发送线程。
    /// + 消息 **离队** 时，会通过 [`Box::from_raw()`] 将数据重新放入 [`Box`] 中，然后通过 [`Ok()`] 返回。
    ///
    /// # 上下文
    ///
    /// + 线程
    ///
    /// # 错误码
    ///
    /// + [`XwmqError::NotInit`] 消息队列没有初始化
    /// + [`XwmqError::NotThreadContext`] 不在线程上下文内
    /// + [`XwmqError::DisPmpt`] 抢占被关闭
    /// + [`XwmqError::DisBh`] 中断底半部被关闭
    /// + [`XwmqError::DisIrq`] 中断被关闭
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
    /// static MQ: Xwmq<16, String> = Xwmq::new();
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
    pub fn dq_unintr(&self) -> Result<Box<T>, XwmqError> {
        unsafe {
            let mut rc = xwrustffi_xwmq_acquire(self.xwmq.mq.get(), *self.xwmq.tik.get());
            if rc == 0 {
                let mut topic = 0;
                let mut raw: *mut c_void = ptr::null_mut();
                rc = xwmq_dq_unintr(self.xwmq.mq.get(), &mut topic, &mut raw);
                xwmq_put(self.xwmq.mq.get());
                if XWOK == rc {
                    let boxdata = Box::from_raw(raw as *mut T);
                    Ok(boxdata)
                } else if -ENOTTHDCTX == rc {
                    Err(XwmqError::NotThreadContext(rc))
                } else if -EDISPMPT == rc {
                    Err(XwmqError::DisPmpt(rc))
                } else if -EDISBH == rc {
                    Err(XwmqError::DisBh(rc))
                } else if -EDISIRQ == rc {
                    Err(XwmqError::DisIrq(rc))
                } else {
                    Err(XwmqError::Unknown(rc))
                }
            } else {
                Err(XwmqError::NotInit(rc))
            }
        }
    }

    /// 尝试从消息队列 **首端** 接收一条消息
    ///
    /// + 从消息队列的 **首端** 取出消息，称为 **离队** 。
    /// + 当消息队列中没有消息时，立即通过 [`Err()`] 返回 [`XwmqError::NoMsg`] 。
    /// + 当消息队列中有可用的消息时，会取走一个消息，并释放消息槽。
    ///   若有发送线程正在等待消息槽，将唤醒发送线程。
    /// + 消息 **离队** 时，会通过 [`Box::from_raw()`] 将数据重新放入 [`Box`] 中，然后通过 [`Ok()`] 返回。
    ///
    /// # 上下文
    ///
    /// + 任意
    ///
    /// # 错误码
    ///
    /// + [`XwmqError::NotInit`] 消息队列没有初始化
    /// + [`XwmqError::NoMsg`] 消息队列中没有消息
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
    /// static MQ: Xwmq<16, String> = Xwmq::new();
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
            let mut rc = xwrustffi_xwmq_acquire(self.xwmq.mq.get(), *self.xwmq.tik.get());
            if rc == 0 {
                let mut topic = 0;
                let mut raw: *mut c_void = ptr::null_mut();
                rc = xwmq_trydq(self.xwmq.mq.get(), &mut topic, &mut raw);
                xwmq_put(self.xwmq.mq.get());
                if XWOK == rc {
                    let boxdata = Box::from_raw(raw as *mut T);
                    Ok(boxdata)
                } else if -ENODATA == rc {
                    Err(XwmqError::NoMsg(rc))
                } else {
                    Err(XwmqError::Unknown(rc))
                }
            } else {
                Err(XwmqError::NotInit(rc))
            }
        }
    }

    /// 等待从消息队列 **尾端** 接收一条消息
    ///
    /// + 从消息队列的 **尾端** 取出消息，称为 **反向离队** 。
    /// + 当消息队列中没有消息时，接收线程会阻塞等待。
    /// + 当消息队列中有可用的消息时，接收线程被唤醒，并取走一个消息，释放消息槽。
    ///   若有发送线程正在等待消息槽，将唤醒发送线程。
    /// + 消息 **离队** 时，会通过 [`Box::from_raw()`] 将数据重新放入 [`Box`] 中，然后通过 [`Ok()`] 返回。
    /// + 当线程阻塞等待被中断时，通过 [`Err()`] 返回 [`XwmqError::Interrupt`] 。
    ///
    /// # 上下文
    ///
    /// + 线程
    ///
    /// # 错误码
    ///
    /// + [`XwmqError::NotInit`] 消息队列没有初始化
    /// + [`XwmqError::Interrupt`] 等待被中断
    /// + [`XwmqError::NotThreadContext`] 不在线程上下文内
    /// + [`XwmqError::DisPmpt`] 抢占被关闭
    /// + [`XwmqError::DisBh`] 中断底半部被关闭
    /// + [`XwmqError::DisIrq`] 中断被关闭
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
    /// static MQ: Xwmq<16, String> = Xwmq::new();
    ///
    /// pub fn xwrust_example_xwmq() {
    ///     let (tx, rx) = MQ.init();
    ///     // ...省略...
    ///     let rc = rx.rq();
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
    pub fn rq(&self) -> Result<Box<T>, XwmqError> {
        unsafe {
            let mut rc = xwrustffi_xwmq_acquire(self.xwmq.mq.get(), *self.xwmq.tik.get());
            if rc == 0 {
                let mut topic = 0;
                let mut raw: *mut c_void = ptr::null_mut();
                rc = xwmq_rq(self.xwmq.mq.get(), &mut topic, &mut raw);
                xwmq_put(self.xwmq.mq.get());
                if XWOK == rc {
                    let boxdata = Box::from_raw(raw as *mut T);
                    Ok(boxdata)
                } else if -EINTR == rc {
                    Err(XwmqError::Interrupt(rc))
                } else if -ENOTTHDCTX == rc {
                    Err(XwmqError::NotThreadContext(rc))
                } else if -EDISPMPT == rc {
                    Err(XwmqError::DisPmpt(rc))
                } else if -EDISBH == rc {
                    Err(XwmqError::DisBh(rc))
                } else if -EDISIRQ == rc {
                    Err(XwmqError::DisIrq(rc))
                } else {
                    Err(XwmqError::Unknown(rc))
                }
            } else {
                Err(XwmqError::NotInit(rc))
            }
        }
    }

    /// 限时等待从消息队列 **尾端** 接收一条消息
    ///
    /// + 从消息队列的 **尾端** 取出消息，称为 **反向离队** 。
    /// + 当消息队列中没有消息时，接收线程会阻塞等待，等待时会指定一个唤醒时间点。
    /// + 当消息队列中有可用的消息时，接收线程被唤醒，并取走一个消息，释放消息槽。
    ///   若有发送线程正在等待消息槽，将唤醒发送线程。
    /// + 消息 **离队** 时，会通过 [`Box::from_raw()`] 将数据重新放入 [`Box`] 中，然后通过 [`Ok()`] 返回。
    /// + 当线程阻塞等待被中断时，通过 [`Err()`] 返回 [`XwmqError::Interrupt`] 。
    /// + 当到达指定的唤醒时间点，线程被唤醒，并通过 [`Err()`] 返回 [`XwmqError::Timedout`] 。
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
    /// + [`XwmqError::NotInit`] 消息队列没有初始化
    /// + [`XwmqError::Interrupt`] 等待被中断
    /// + [`XwmqError::Timedout`] 等待超时
    /// + [`XwmqError::NotThreadContext`] 不在线程上下文内
    /// + [`XwmqError::DisPmpt`] 抢占被关闭
    /// + [`XwmqError::DisBh`] 中断底半部被关闭
    /// + [`XwmqError::DisIrq`] 中断被关闭
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
    /// static MQ: Xwmq<16, String> = Xwmq::new();
    ///
    /// pub fn xwrust_example_xwmq() {
    ///     let (tx, rx) = MQ.init();
    ///     // ...省略...
    ///     let rc = rx.rq_to(xwtm::ft(xwtm::s(1)));
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
    pub fn rq_to(&self, to: XwTm) -> Result<Box<T>, XwmqError> {
        unsafe {
            let mut rc = xwrustffi_xwmq_acquire(self.xwmq.mq.get(), *self.xwmq.tik.get());
            if rc == 0 {
                let mut topic = 0;
                let mut raw: *mut c_void = ptr::null_mut();
                rc = xwmq_rq_to(self.xwmq.mq.get(), &mut topic, &mut raw, to);
                xwmq_put(self.xwmq.mq.get());
                if XWOK == rc {
                    let boxdata = Box::from_raw(raw as *mut T);
                    Ok(boxdata)
                } else if -EINTR == rc {
                    Err(XwmqError::Interrupt(rc))
                } else if -ETIMEDOUT == rc {
                    Err(XwmqError::Timedout(rc))
                } else if -ENOTTHDCTX == rc {
                    Err(XwmqError::NotThreadContext(rc))
                } else if -EDISPMPT == rc {
                    Err(XwmqError::DisPmpt(rc))
                } else if -EDISBH == rc {
                    Err(XwmqError::DisBh(rc))
                } else if -EDISIRQ == rc {
                    Err(XwmqError::DisIrq(rc))
                } else {
                    Err(XwmqError::Unknown(rc))
                }
            } else {
                Err(XwmqError::NotInit(rc))
            }
        }
    }

    /// 等待从消息队列 **尾端** 接收一条消息，且等待不可被中断
    ///
    /// + 从消息队列的 **尾端** 取出消息，称为 **反向离队** 。
    /// + 当消息队列中没有消息时，接收线程会阻塞等待。
    /// + 接收线程的阻塞等待不可被中断。
    /// + 当消息队列中有可用的消息时，接收线程被唤醒，并取走一个消息，释放消息槽。
    ///   若有发送线程正在等待消息槽，将唤醒发送线程。
    /// + 消息 **离队** 时，会通过 [`Box::from_raw()`] 将数据重新放入 [`Box`] 中，然后通过 [`Ok()`] 返回。
    ///
    /// # 上下文
    ///
    /// + 线程
    ///
    /// # 错误码
    ///
    /// + [`XwmqError::NotInit`] 消息队列没有初始化
    /// + [`XwmqError::NotThreadContext`] 不在线程上下文内
    /// + [`XwmqError::DisPmpt`] 抢占被关闭
    /// + [`XwmqError::DisBh`] 中断底半部被关闭
    /// + [`XwmqError::DisIrq`] 中断被关闭
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
    /// static MQ: Xwmq<16, String> = Xwmq::new();
    ///
    /// pub fn xwrust_example_xwmq() {
    ///     let (tx, rx) = MQ.init();
    ///     // ...省略...
    ///     let rc = rx.rq();
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
    pub fn rq_unintr(&self) -> Result<Box<T>, XwmqError> {
        unsafe {
            let mut rc = xwrustffi_xwmq_acquire(self.xwmq.mq.get(), *self.xwmq.tik.get());
            if rc == 0 {
                let mut topic = 0;
                let mut raw: *mut c_void = ptr::null_mut();
                rc = xwmq_rq_unintr(self.xwmq.mq.get(), &mut topic, &mut raw);
                xwmq_put(self.xwmq.mq.get());
                if XWOK == rc {
                    let boxdata = Box::from_raw(raw as *mut T);
                    Ok(boxdata)
                } else if -ENOTTHDCTX == rc {
                    Err(XwmqError::NotThreadContext(rc))
                } else if -EDISPMPT == rc {
                    Err(XwmqError::DisPmpt(rc))
                } else if -EDISBH == rc {
                    Err(XwmqError::DisBh(rc))
                } else if -EDISIRQ == rc {
                    Err(XwmqError::DisIrq(rc))
                } else {
                    Err(XwmqError::Unknown(rc))
                }
            } else {
                Err(XwmqError::NotInit(rc))
            }
        }
    }

    /// 尝试从消息队列 **尾端** 接收一条消息
    ///
    /// + 从消息队列的 **尾端** 取出消息，称为 **反向离队** 。
    /// + 当消息队列中没有消息时，立即通过 [`Err()`] 返回 [`XwmqError::NoMsg`] 。
    /// + 当消息队列中有可用的消息时，会取走一个消息，并释放消息槽。
    ///   若有发送线程正在等待消息槽，将唤醒发送线程。
    /// + 消息 **离队** 时，会通过 [`Box::from_raw()`] 将数据重新放入 [`Box`] 中，然后通过 [`Ok()`] 返回。
    ///
    /// # 上下文
    ///
    /// + 任意
    ///
    /// # 错误码
    ///
    /// + [`XwmqError::NotInit`] 消息队列没有初始化
    /// + [`XwmqError::NoMsg`] 消息队列中没有消息
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
    /// static MQ: Xwmq<16, String> = Xwmq::new();
    ///
    /// pub fn xwrust_example_xwmq() {
    ///     let (tx, rx) = MQ.init();
    ///     // ...省略...
    ///     let rc = rx.tryrq();
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
    pub fn tryrq(&self) -> Result<Box<T>, XwmqError> {
        unsafe {
            let mut rc = xwrustffi_xwmq_acquire(self.xwmq.mq.get(), *self.xwmq.tik.get());
            if rc == 0 {
                let mut topic = 0;
                let mut raw: *mut c_void = ptr::null_mut();
                rc = xwmq_tryrq(self.xwmq.mq.get(), &mut topic, &mut raw);
                xwmq_put(self.xwmq.mq.get());
                if XWOK == rc {
                    let boxdata = Box::from_raw(raw as *mut T);
                    Ok(boxdata)
                } else if -ENODATA == rc {
                    Err(XwmqError::NoMsg(rc))
                } else {
                    Err(XwmqError::Unknown(rc))
                }
            } else {
                Err(XwmqError::NotInit(rc))
            }
        }
    }
}
