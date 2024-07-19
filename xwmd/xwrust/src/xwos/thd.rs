//! XWOS RUST：线程
//! ========
//!
//! XWOS的线程分为 **分离态** 或 **连接态** ：
//!
//! + **分离态（detached）** ：分离态的线程退出后由操作系统自动回收其内存资源；
//! + **连接态（joinable）** ：连接态的线程退出后需要被其他线程 `join()` ，之后才会被操作系统回收内存资源。
//!
//! XWOS RUST的线程分为 **动态线程** 与 **静态线程** 。无论哪种线程创建时，都是 **连接态（joinable）** 的。
//! 线程创建后，将返回线程的 **Handle** 。如果未将线程的 **Handle** 绑定到变量上，线程将自动转变为 **分离态（detached）** 。
//!
//!
//! # 动态线程
//!
//! **动态线程** 是指通过动态内存分配器创建的线程。
//!
//! 动态线程的对象结构体、栈、线程闭包等资源都是通过内存申请的接口动态创建的。
//!
//! XWOS RUST的动态线程库是仿照 [`std::thread`] 来编写的。
//!
//! ```rust
//! use xwrust::xwos::thd;
//!
//! thd::spawn(|_| {
//!     // 线程代码;
//! });
//! ```
//!
//! 在上述代码中，[`spawn()`] 方法会返回 [`DThdHandle`] ，但由于 [`DThdHandle`] 没有被绑定到任何变量名上，
//! 其生命周期结束后的 [`drop()`] 方法会自动将动态线程转变为 **分离态(detached)** 。此线程运行结束后，其资源会被系统自动回收。
//!
//!
//! 动态线程也可以是 **可连接的(joinable)** ，可以将返回的 [`DThdHandle`] 绑定在变量上。
//! 然后通过 [`DThdHandle::join()`] 方法等待线程运行结束。
//!
//! ```rust
//! use xwrust::xwos::thd;
//!
//! let res = thd::spawn(|_| {
//!     // 线程代码;
//!     // 返回值
//! });
//! match res {
//!     Ok(handler) => {
//!         match handler.join() {
//!             Ok(r) => {
//!                 // `r` 是线程闭包的返回值。
//!             },
//!             Err(e) => {
//!                 // `join()` 失败时的错误码可通过 `e.state()` 获取。
//!                 // `e` 是 `DThdHandle<R>` ，重新被返回。
//!             },
//!         };
//!     },
//!     Err(rc) => {
//!         // `rc` 是 `spawn()` 失败时的错误码。
//!     },
//! };
//! ```
//!
//! [`DThdHandle::join()`] 方法会返回 [`Result<R, Self>`] ， `R` 是返回值的类型，并放在 [`Ok()`] 中，
//! 方法调用失败时，会将 [`DThdHandle`] 放在 [`Err()`] 中重新返回。此时，用户可以通过 [`DThdHandle::state()`] 获取失败原因，
//! 并且在合适的时机重新调用方法 [`DThdHandle::join()`] 。
//!
//!
//! ## 动态线程的线程函数
//!
//! 动态线程可以使用 [`FnOnce()`] 闭包作为线程函数。其原型是： `FnOnce(Arc<DThdElement>) -> R` ，
//! 动态线程函数运行时，参数是动态线程的元素 [`DThdElement`] ，返回值为泛型 `R` 。
//!
//!
//! ## 动态线程的工厂模式
//!
//! 可以通过线程工厂 [`DThdBuilder`] 设置线程属性后，再创建动态线程：
//!
//! ```rust
//! use xwrust::xwos::thd;
//!
//! let builder = thd::DThdBuilder::new()
//!                                .name("foo".into()) // 设置线程的名称
//!                                .stack_size(8 * 1024) // 设置线程栈大小
//!                                .privileged(true); // 设置系统权限
//!
//! builder.spawn(|_| {
//!     // 线程代码;
//!     // 返回值
//! });
//! ```
//!
//! #### 线程的名称
//!
//! 线程工厂可以通过 [`DThdBuilder::name()`] 为线程指定一个字符串名字。可以为空，默认为 `"anon"` 。
//!
//! #### 线程的栈大小
//!
//! 线程工厂可以通过 [`DThdBuilder::stack_size()`] 为线程指定栈内存大小，默认为XWOS的内核配置 `XWMMCFG_STACK_SIZE_MIN` 。
//!
//! #### 线程的系统权限
//!
//! 某些SOC内部的寄存器，只有拥有系统权限的线程才可以访问。线程工厂可以通过 [`DThdBuilder::privileged()`] 为线程指定是否具有系统特权，默认为拥有系统权限。
//!
//!
//! ## 动态线程的元素
//!
//! 动态线程的元素 [`DThdElement`] 是存放与线程相关的信息的内存空间。
//! 动态线程工厂 [`DThdBuilder`] 中设置的信息会被转移到 [`DThdElement`] 中。
//! 动态线程运行时， `Arc<DThdElement>` 作为参数被传递到闭包内。以 [`Arc<T>`] 进行封装是因为要将其放在堆上。
//!
//!
//! ## 动态线程的句柄
//!
//! 静态线程的句柄 [`DThdHandle`] 功能类似于 [`std::thread::JoinHandle`] ，可用于控制动态子线程的退出。
//!
//! #### 中断动态线程的阻塞态和睡眠态
//!
//! 方法 [`DThdHandle::intr()`] 可用于中断线程的 **阻塞态** 和 **睡眠态** 。
//! 阻塞和睡眠的方法将以返回值 **负** 的 [`EINTR`] 退出。错误码 [`EINTR`] 会被转换为各个可阻塞线程的操作系统对象的错误码：
//!
//! + [`MutexError::Interrupt`]
//! + [`SemError::Interrupt`]
//! + [`CondError::Interrupt`]
//! + [`FlgError::Interrupt`]
//! + [`BrError::Interrupt`]
//! + [`SelError::Interrupt`]
//!
//! 方法 [`DThdHandle::intr()`] 是基于 [`ThdD::intr()`] 实现的，调用后者与前者在功能上没有区别。
//! 动态线程自身的 [`ThdD`] 可通过 [`cthd::i()`] 获取。
//!
//! #### 通知动态线程退出
//!
//! 方法 [`DThdHandle::quit()`] 可用于父线程通知动态子线程退出。此方法不会等待动态子线程退出。
//!
//! 方法 [`DThdHandle::quit()`] 会为动态子线程设置 **退出状态** ，并中断 **阻塞状态** 和 **睡眠状态** 。
//! 阻塞和睡眠的方法将以返回值负的 [`EINTR`] 退出。错误码 [`EINTR`] 会被转换为各个可阻塞线程的操作系统对象的错误码：
//!
//! + [`MutexError::Interrupt`]
//! + [`SemError::Interrupt`]
//! + [`CondError::Interrupt`]
//! + [`FlgError::Interrupt`]
//! + [`BrError::Interrupt`]
//! + [`SelError::Interrupt`]
//!
//! 但是，当动态子线程的 **阻塞状态** 是不可被中断的，方法 [`DThdHandle::quit()`] 只会为动态子线程设置 **退出状态** ，不会发生中断。
//!
//! 动态子线程可以通过 [`cthd::shld_stop()`] 判断是否被设置了 **退出状态** ，可以作为结束线程循环的条件。
//!
//! 方法 [`DThdHandle::quit()`] 是基于 [`ThdD::quit()`] 实现的，调用后者与前者在功能上没有区别。
//! 动态线程自身的 [`ThdD`] 可通过 [`cthd::i()`] 获取。
//!
//! #### 等待动态线程退出
//!
//! 当父线程需要等待动态子线程退出，并捕获其返回值时，需要使用方法 [`DThdHandle::join()`] 。
//!
//! + 如果动态子线程还在运行，此方法会阻塞父线程直到动态子线程退出。
//! + 如果动态子线程已经提前运行至退出，此方法可立即返回动态子线程的返回值。
//!
//! 此方法会消费 [`DThdHandle`] ：
//!
//! + 如果此方法执行成功，会消费掉 [`DThdHandle`] ，并将动态子线程的返回值放在 [`Ok()`] 中返回，因为线程已经结束，其 [`DThdHandle`] 的生命周期也应该结束；
//! + 如果此方法执行失败，会重新在 [`Err()`] 中返回 [`DThdHandle`] ，用户可通过 [`DThdHandle::state()`] 方法获取失败的原因，并且在合适的重新调用 [`DThdHandle::join()`] 方法。
//!
//! #### 通知并等待动态线程退出
//!
//! 方法 [`DThdHandle::stop()`] 等价于 [`DThdHandle::quit()`] + [`DThdHandle::join()`]
//!
//!
//! ## 动态线程的示例
//!
//! [XWOS/xwam/xwrust-example/xwrust_example_dthd](https://gitee.com/xwos/XWOS/blob/main/xwam/xwrust-example/xwrust_example_dthd/src/lib.rs)
//!
//!
//! ## 对比 [`std::thread`]
//!
//! #### 闭包原型不同
//!
//! + [`std::thread`] 的闭包原型是 `FnOnce() -> R` 。
//! + `xwrust::xwos::thd` 的闭包原型是 `FnOnce(Arc<DThdElement>) -> R` 。
//! + 【举例】 [`std::thread`] 通过 `thread::current()` 获取线程自己的`handle`，然后获取线程的名称：
//!
//! ```rust
//! use std::thread;
//!
//! let handler = thread::DThdBuilder::new()
//!     .name("named thread".into())
//!     .spawn(|| {
//!         let handle = thread::current();
//!         assert_eq!(handle.name(), Some("named thread"));
//!     }).unwrap();
//!
//! handler.join().unwrap();
//! ```
//!
//! + 【举例】 `xwrust::xwos::thd` 通过闭包参数 [`ele.name()`] 获取线程的名称：
//!
//! ```rust
//! use xwrust::xwos::thd;
//! use libc_print::std_name::println;
//!
//! thd::DThdBuilder::new()
//!     .name("foo".into())
//!     .spawn(|ele| {
//!         println!("Thread name: {}", ele.name());
//!     });
//! ```
//!
//! 若不需要使用 `Arc<DThdElement>` ，可使用 `_` 占位。
//!
//! ```rust
//! use xwrust::xwos::thd;
//! use libc_print::std_name::println;
//!
//! thd::DThdBuilder::new()
//!     .name("foo".into())
//!     .spawn(|_| {
//!         // 线程闭包
//!     });
//! ```
//!
//! #### **spawn()** 失败时的处理方式不同
//!
//! + [`std::thread`] **spawn()** 失败时，会直接 [`panic!()`] ；
//! + XWOS是RTOS，运行环境是 `#![no_std]` ， [`panic!()`] 意味着死机，因此需要处理 [`Err()`] ；
//!
//! ```rust
//! use xwrust::xwos::thd;
//!
//! match thd::DThdBuilder::new()
//!     .name("child".into())
//!     .spawn(|ele| {
//!         // 子线程闭包
//!     }) {
//!         Ok(h) => { // 新建子线程成功
//!         },
//!         Err(rc) => { // 新建子线程失败
//!         },
//!     };
//! ```
//!
//! #### 子线程 [`panic!()`] 的处理方式不同
//!
//! + [`std::thread`] 可捕获子线程的 [`panic!()`] 。
//! + `xwrust::xwos::thd` 的子线程 [`panic!()`] 后会导致整个系统 **halt** 。
//! 目前 `#![no_std]` 环境的 **unwind** 支持还不完善，暂时无法实现类似于 [`std::thread`] 的捕获机制。
//!
//!
//! # 静态线程
//!
//! **静态线程** 是指 **不** 通过动态内存分配器创建的线程，所需内存全部由编译器在编译链接阶段分配。
//! 设计 **静态线程** 的目的是为了避免使用动态内存分配。在功能安全的场合，动态内存分配是禁止使用的。
//!
//! 因为无法确定线程会运行多长时间，只能将 **静态线程** 定义为 **静态生命周期** 的全局变量。
//!
//!
//! ```rust
//! use xwrust::xwos::thd::*;
//!
//! static STHD: SThd<1024, &str> = SThd::new("SThd", true);
//! pub fn xwrust_example_sthd() {
//!     let h = STHD.run(|sthd| { // 子线程
//!         // 线程功能
//!         "OK" // 返回值
//!     });
//!     let res = h.join();
//!     match res {
//!         Ok(r) => {
//!             // `r` 是线程的返回值。
//!         },
//!         Err(e) => {
//!             h = e;
//!             // `join()` 失败时的错误码可通过 `e.state()` 获取。
//!             // `e` 是 `SThdHandle` ，重新被返回。
//!         },
//!     };
//!
//! }
//! ```
//!
//! 静态线程可以是 **可连接的(joinable)** 。当另一线程通过 [`SThd::run()`] 启动静态线程后，
//! 可将返回的 [`SThdHandle`] 绑定在变量上，然后通过 [`SThdHandle::join()`] 方法等待静态线程运行结束并获取返回值。
//!
//!
//! 当 [`SThdHandle`] 生命周期结束前， [`SThdHandle::join()`] 未被调用过，其 [`drop()`] 方法会将静态线程自动变成 **分离的(detached)** 。
//!
//!
//! ## 静态线程的线程函数
//!
//! 定义RUST闭包时，实际上会生成一个隐藏的结构体，隐藏结构体中包括捕获的外部变量以及闭包函数。
//! 闭包其实是这个隐藏结构体类型的变量，并且是在栈上建立的，其生命周期只限于函数局部。
//! 因此，不能将闭包直接转移到静态线程的线程函数内部。
//!
//! 如何延长闭包生命周期？RUST语言的常用技巧是借助于 [`Box<T>`] 或 [`Arc<T>`] 将其放入堆中。
//! 但若使用了 [`Box<T>`] 或 [`Arc<T>`] 就违背了 **不使用动态内存分配** 的初衷。
//!
//! 综上，静态线程的线程函数只能是 **普通函数** 或 **不捕获任何环境的闭包** 。
//!
//!
//! 静态线程只能访问 **静态生命周期的全局变量** 。
//! 其原型是 `fn(&Self) -> R` 。其中 `&Self` 是指向静态线程自身的引用， `R` 是泛型返回值。
//!
//!
//! ## 静态线程的句柄
//!
//! 静态线程的句柄 [`SThdHandle`] 由方法 [`SThd::run()`] 返回，功能类似于 [`std::thread::JoinHandle`] ，可用于控制静态子线程的退出。
//!
//! #### 中断静态线程的阻塞态和睡眠态
//!
//! 方法 [`SThdHandle::intr()`] 可用于中断线程的 **阻塞态** 和 **睡眠态** 。
//! 阻塞和睡眠的方法将以返回值 **负** 的 [`EINTR`] 退出。错误码 [`EINTR`] 会被转换为各个可阻塞线程的操作系统对象的错误码：
//!
//! + [`MutexError::Interrupt`]
//! + [`SemError::Interrupt`]
//! + [`CondError::Interrupt`]
//! + [`FlgError::Interrupt`]
//! + [`BrError::Interrupt`]
//! + [`SelError::Interrupt`]
//!
//! 方法 [`SThdHandle::intr()`] 是基于 [`ThdD::intr()`] 实现的，调用后者与前者在功能上没有区别。
//! 动态线程自身的 [`ThdD`] 可通过 [`cthd::i()`] 获取。
//!
//! #### 通知动态线程退出
//!
//! 方法 [`SThdHandle::quit()`] 可用于父线程通知静态子线程退出。此方法不会等待静态子线程退出。
//!
//! 方法 [`SThdHandle::quit()`] 是基于 [`SThd::quit()`] 实现的，调用后者与前者在功能上没有区别。后者可以由静态现在自己调用，自己通知自己退出。
//!
//! 方法 [`ThdD::quit()`] 也可达到同样的效果。静态态线程自身的 [`ThdD`] 可通过 [`cthd::i()`] 获取。
//!
//!
//! 通知静态线程退出的方法会为静态子线程设置 **退出状态** ，并中断 **阻塞状态** 和 **睡眠状态** 。
//! 阻塞和睡眠的方法将以返回值负的 [`EINTR`] 退出。错误码 [`EINTR`] 会被转换为各个可阻塞线程的操作系统对象的错误码：
//!
//! + [`MutexError::Interrupt`]
//! + [`SemError::Interrupt`]
//! + [`CondError::Interrupt`]
//! + [`FlgError::Interrupt`]
//! + [`BrError::Interrupt`]
//! + [`SelError::Interrupt`]
//!
//! 但是，当静态子线程的 **阻塞状态** 是不可被中断的，方法 [`SThdHandle::quit()`] 只会为静态子线程设置 **退出状态** ，不会发生中断。
//!
//! 静态子线程可以通过 [`cthd::shld_stop()`] 判断是否被设置了 **退出状态** ，可以作为结束线程循环的条件。
//!
//! #### 等待动态线程退出
//!
//! 当父线程需要等待静态子线程退出，并捕获其返回值，需要使用方法 [`SThdHandle::join()`] 。
//!
//! + 如果静态子线程还在运行，此方法会阻塞父线程直到静态子线程退出。父线程的阻塞状态可被中断；
//! + 如果静态子线程已经提前运行至退出，此方法可立即返回静态子线程的返回值。
//!
//! 此方法会消费 [`SThdHandle`] ：
//!
//! + 如果此方法执行成功，会消费掉 [`SThdHandle`] ，并将静态子线程的返回值放在 [`Ok()`] 中返回，因为线程已经结束，其 [`SThdHandle`] 的生命周期也应该结束；
//! + 如果此方法执行失败，会重新在 [`Err()`] 中返回 [`SThdHandle`] ，用户可通过 [`SThdHandle::state()`] 方法获取失败的原因，
//!   并且在合适的时机重新调用 [`SThdHandle::join()`] 方法。
//!
//! #### 通知并等待动态线程退出
//!
//! 方法 [`SThdHandle::stop()`] 等价于 [`SThdHandle::quit()`] + [`SThdHandle::join()`]
//!
//!
//! ## 静态线程的示例
//!
//! [XWOS/xwam/xwrust-example/xwrust_example_sthd](https://gitee.com/xwos/XWOS/blob/main/xwam/xwrust-example/xwrust_example_sthd/src/lib.rs)
//!
//!
//! # 只可在线程自身函数内部调用的方法
//!
//! XWOS RUST有一组方法，只会对调用线程自身起作用，被实现在 [`xwrust::xwos::cthd`] 。命名中的 **c** 是 **current** 的意思。
//!
//!
//! [`drop()`]: <https://doc.rust-lang.org/core/ops/trait.Drop.html#tymethod.drop>
//! [`std::thread`]: <https://doc.rust-lang.org/std/thread/index.html>
//! [`std::thread::JoinHandle`]: <https://doc.rust-lang.org/std/thread/struct.JoinHandle.html>
//! [`panic!()`]: <https://doc.rust-lang.org/std/macro.panic.html>
//! [`Ok()`]: <https://doc.rust-lang.org/core/result/enum.Result.html#variant.Ok>
//! [`Err()`]: <https://doc.rust-lang.org/core/result/enum.Result.html#variant.Err>
//! [`FnOnce()`]: <https://doc.rust-lang.org/core/ops/trait.FnOnce.html>
//! [`ele.name()`]: DThdElement::name
//! [`EINTR`]: crate::errno::EINTR
//! [`MutexError::Interrupt`]: crate::xwos::lock::mtx::MutexError::Interrupt
//! [`SemError::Interrupt`]: crate::xwos::sync::sem::SemError::Interrupt
//! [`CondError::Interrupt`]: crate::xwos::sync::cond::CondError::Interrupt
//! [`FlgError::Interrupt`]: crate::xwos::sync::flg::FlgError::Interrupt
//! [`BrError::Interrupt`]: crate::xwos::sync::br::BrError::Interrupt
//! [`SelError::Interrupt`]: crate::xwos::sync::sel::SelError::Interrupt
//! [`cthd::i()`]: crate::xwos::cthd::i
//! [`cthd::shld_stop()`]: crate::xwos::cthd::shld_stop
//! [`xwrust::xwos::cthd`]: crate::xwos::cthd
//! [`Box<T>`]: <https://doc.rust-lang.org/alloc/boxed/struct.Box.html>
//! [`Arc<T>`]: <https://doc.rust-lang.org/alloc/sync/struct.Arc.html>

extern crate core;
use core::ffi::*;
use core::option::Option;
use core::result::Result;
use core::cell::UnsafeCell;
use core::ptr;
use core::mem;
use core::str;
use core::fmt;

extern crate alloc;
use alloc::boxed::Box;
use alloc::string::String;
use alloc::sync::Arc;

use crate::types::*;

extern "C" {
    fn xwrustffi_thd_stack_size_default() -> XwSz;
    fn xwrustffi_thd_attr_init(attr: *mut ThdAttr);
    fn xwrustffi_thd_create(thd: *mut *mut c_void, tik: *mut XwSq,
                            attr: *mut ThdAttr,
                            mainfunc: extern "C" fn(*mut c_void) -> XwEr,
                            arg: *mut c_void) -> XwEr;
    fn xwrustffi_thd_init(thd: *mut c_void, tik: *mut XwSq,
                          attr: *const ThdAttr,
                          mainfunc: extern "C" fn(*mut c_void) -> XwEr,
                          arg: *mut c_void) -> XwEr;
    fn xwrustffi_thd_acquire(thd: *mut c_void, tik: XwSq) -> XwEr;
    fn xwrustffi_thd_release(thd: *mut c_void, tik: XwSq) -> XwEr;
    fn xwrustffi_thd_intr(thd: *mut c_void, tik: XwSq) -> XwEr;
    fn xwrustffi_thd_quit(thd: *mut c_void, tik: XwSq) -> XwEr;
    fn xwrustffi_thd_join(thd: *mut c_void, tik: XwSq, trc: *mut XwEr) -> XwEr;
    fn xwrustffi_thd_stop(thd: *mut c_void, tik: XwSq, trc: *mut XwEr) -> XwEr;
    fn xwrustffi_thd_detach(thd: *mut c_void, tik: XwSq) -> XwEr;
    fn xwrustffi_thd_migrate(thd: *mut c_void, tik: XwSq, cpuid: XwId) -> XwEr;
}

/// XWOS线程的属性
///
/// 完全等价于C语言头文件 `xwos/osal/thd.h` 中的 `struct xwos_thd_attr`
#[repr(C)]
pub(crate) struct ThdAttr {
    /// 线程的名字
    pub(crate) name: *const c_char,
    /// 线程栈的首地址
    pub(crate) stack: *mut XwStk,
    /// 线程栈的大小，以字节(byte)为单位
    pub(crate) stack_size: XwSz,
    /// 栈内存警戒线位置
    pub(crate) stack_guard_size: XwSz,
    /// 优先级
    pub(crate) priority: XwPr,
    /// 是否为分离态
    pub(crate) detached: bool,
    /// 是否为特权线程
    pub(crate) privileged: bool,
}

/// XWOS的线程对象描述符
///
/// 用于调用XWOS-CAPI
pub struct ThdD {
    /// XWOS线程对象的指针
    pub thd: *mut c_void,
    /// XWOS线程对象的标签
    pub tik: XwSq,
}

unsafe impl Send for ThdD {}
unsafe impl Sync for ThdD {}

impl ThdD {
    unsafe fn new(attr: &mut ThdAttr, func: Box<dyn FnOnce()>) -> Result<ThdD, XwEr> {
        let boxboxfunc = Box::new(func);
        let rawboxfunc = Box::into_raw(boxboxfunc);
        let mut thd: *mut c_void = ptr::null_mut();
        let mut tik: XwSq = 0;
        let rc = xwrustffi_thd_create(&mut thd, &mut tik, attr,
                                      ThdD::xwrust_dthd_entry, rawboxfunc as *mut _);
        return if rc < 0 {
            drop(Box::from_raw(rawboxfunc)); // 创建失败，需要释放f
            Err(rc)
        } else {
            Ok(ThdD {thd: thd, tik: tik})
        };
    }

    extern "C" fn xwrust_dthd_entry(main: *mut c_void) -> XwEr {
        unsafe {
            let func = Box::from_raw(main as *mut Box<dyn FnOnce()>);
            func();
        }
        0
    }

    /// 中断线程的阻塞态和睡眠态
    pub fn intr(&self) -> XwEr {
        unsafe {
            xwrustffi_thd_intr(self.thd, self.tik)
        }
    }

    /// 通知线程退出
    pub fn quit(&self) -> XwEr {
        unsafe {
            xwrustffi_thd_quit(self.thd, self.tik)
        }
    }

    fn join(&mut self) ->XwEr {
        unsafe {
            xwrustffi_thd_join(self.thd, self.tik, ptr::null_mut())
        }
    }

    fn stop(&mut self) -> XwEr {
        unsafe {
            xwrustffi_thd_stop(self.thd, self.tik, ptr::null_mut())
        }
    }

    fn detach(&mut self) -> XwEr {
        unsafe {
            xwrustffi_thd_detach(self.thd, self.tik)
        }
    }

    /// 将线程迁移到目标CPU
    ///
    /// # 参数说明
    ///
    /// + cpuid: 目标CPU的ID
    pub fn migrate(&self, cpuid: XwId) -> XwEr {
        unsafe {
            xwrustffi_thd_migrate(self.thd, self.tik, cpuid)
        }
    }
}

impl fmt::Debug for ThdD {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        f.debug_struct("ThdD")
            .field("thd", &self.thd)
            .field("tik", &self.tik)
            .finish()
    }
}

/// 线程的 `join()/stop()` 状态
#[derive(Debug)]
pub enum ThdJoinState {
    /// 已经被连接
    Joined,
    /// 可被连接的
    Joinable,
    /// 连接错误
    JoinErr(XwEr),
}


////////////////////////////////////////////////////////////////////////////////
// 动态线程
////////////////////////////////////////////////////////////////////////////////

/// 动态线程的工厂模式结构体，可用于配置新线程的属性
///
/// + [`name`] 设置线程的名字
/// + [`stack_size`] 设置线程的栈大小
/// + [`privileged`] 设置线程的系统权限
///
/// [`spawn`] 方法将获取构建器的所有权，并使用给定的配置创建线程，返回 [`Result`] 。
///
/// [`thd::spawn`] 独立的函数，并使用默认配置的 `DThdBuilder`创建线程，返回 [`Result`] 。
///
///
/// # 示例
///
/// ```rust
/// use xwrust::xwos::thd;
///
/// let builder = thd::DThdBuilder::new();
///
/// builder.spawn(|_| {
///     // 线程代码;
///     // 返回值
/// });
/// ```
///
/// [`name`]: DThdBuilder::name
/// [`stack_size`]: DThdBuilder::stack_size
/// [`privileged`]: DThdBuilder::privileged
/// [`spawn`]: DThdBuilder::spawn
/// [`thd::spawn`]: spawn
/// [`Result`]: <https://doc.rust-lang.org/core/result/enum.Result.html>
pub struct DThdBuilder {
    /// 线程的名字
    name: Option<String>,
    /// 线程栈的大小，以字节(byte)为单位
    stack_size: Option<XwSz>,
    /// 是否为特权线程
    privileged: Option<bool>,
}

impl DThdBuilder {
    /// 新建用于创建动态线程的工厂
    ///
    /// # 示例
    ///
    /// ```rust
    /// use xwrust::xwos::thd;
    ///
    /// let builder = thd::DThdBuilder::new()
    ///                                .name("foo".into()) // 设置线程的名称
    ///                                .stack_size(8 * 1024) // 设置线程栈大小
    ///                                .privileged(true); // 设置系统权限
    ///
    /// builder.spawn(|_| {
    ///     // 线程代码;
    ///     // 返回值
    /// });
    /// ```
    pub fn new() -> DThdBuilder {
        DThdBuilder {
            name: None,
            stack_size: None,
            privileged: None,
        }
    }

    /// 设置动态线程的名称
    ///
    /// # 示例
    ///
    /// ```rust
    /// use xwrust::xwos::thd;
    /// use libc_print::std_name::println;
    ///
    /// let builder = thd::DThdBuilder::new()
    ///                                .name("foo".into()); // 设置线程的名称
    ///
    /// builder.spawn(|ele| {
    ///     println!("My name is {}.", ele.name().unwrap());
    ///     // 线程代码;
    ///     // 返回值
    /// });
    /// ```
    pub fn name(mut self, name: String) -> DThdBuilder {
        self.name = Some(name);
        self
    }

    /// 设置动态线程栈的大小
    ///
    /// # 示例
    ///
    /// ```rust
    /// use xwrust::xwos::thd;
    ///
    /// let builder = thd::DThdBuilder::new()
    ///                                .stack_size(8 * 1024); // 设置线程栈大小
    ///
    /// builder.spawn(|_| {
    ///     // 线程代码;
    ///     // 返回值
    /// });
    /// ```
    pub fn stack_size(mut self, size: XwSz) -> DThdBuilder {
        self.stack_size = Some(size);
        self
    }

    /// 设置动态线程栈的系统权限
    ///
    /// # 示例
    ///
    /// ```rust
    /// use xwrust::xwos::thd;
    ///
    /// let builder = thd::DThdBuilder::new()
    ///                                .privileged(true); // 设置系统权限
    ///
    /// builder.spawn(|_| {
    ///     // 线程代码;
    ///     // 返回值
    /// });
    /// ```
    pub fn privileged(mut self, privileged: bool) -> DThdBuilder {
        self.privileged = Some(privileged);
        self
    }

    /// 消费 `DThdBuilder` ，并新建一个动态线程
    ///
    /// + 创建线程成功，返回一个包含 [`DThdHandle`] 的 [`Result`] ；
    /// + 创建线程失败，返回一个包含 [`XwEr`] 的 [`Result`] ， [`XwEr`] 指示错误的原因。
    ///
    /// 方法的签名：
    ///
    /// + [`'static`] 约束是因为新建的线程可能比调用者的生命周期更长，因此线程的闭包和返回值的生命周期限定为静态生命周期；
    /// + [`Send`] 约束是因为闭包和返回值需要在线程之间进行转移。并且被移动到 [`Send`] 约束的闭包的变量也必须是 [`Send`] 的，否则编译器会报错。
    ///   RUSTC是通过 [`Send`] 约束来区分闭包是不是另一线程的代码的。
    ///
    /// # 参数说明
    ///
    /// + f: 线程的闭包
    ///
    /// # 示例
    ///
    /// ```rust
    /// use xwrust::xwos::thd;
    ///
    /// let builder = thd::DThdBuilder::new()
    ///     .name("foo".into()) // 设置线程的名称
    ///     .stack_size(8 * 1024) // 设置线程栈大小
    ///     .privileged(true); // 设置系统权限
    /// match builder.spawn(|_| {
    ///     // 线程代码;
    ///     // 返回值
    /// }) {
    ///     Ok(handler) => {
    ///         match handler.join() {
    ///             Ok(r) => {
    ///                 // r 是线程闭包的返回值。
    ///             },
    ///             Err(e) => {
    ///                 // `join()` 失败的错误码可通过 `e.state()` 获取。
    ///                 // `e` 是 `DThdHandle<R>` ，重新被返回。
    ///             },
    ///         };
    ///     },
    ///     Err(rc) => {
    ///         // `rc` 是 `spawn()` 失败时的错误码。
    ///     }
    /// };
    /// ```
    /// [`Result`]: <https://doc.rust-lang.org/core/result/enum.Result.html>
    /// [`'static`]: <https://doc.rust-lang.org/std/keyword.static.html>
    /// [`Send`]: <https://doc.rust-lang.org/core/marker/trait.Send.html>
    pub fn spawn<F, R>(self, f: F) -> Result<DThdHandle<R>, XwEr>
    where
        F: FnOnce(Arc<DThdElement>) -> R,
        F: Send + 'static,
        R: Send + 'static,
    {
        unsafe { self.spawn_unchecked(f) }
    }

    /// 消费 `DThdBuilder` ，并产生一个新的动态线程
    ///
    /// + 创建线程成功，返回一个包含 [`DThdHandle`] 的 [`Result`] ；
    /// + 创建线程失败，返回一个包含 [`XwEr`] 的 [`Result`] ， [`XwEr`] 指示错误的原因。
    ///
    /// 此方法只要求闭包 `F` 和 返回值 `R` 的生命周期一样长，然后不做其他限制，因此是 `unsafe` 的。
    ///
    /// # 参数说明
    ///
    /// + f: 线程的闭包
    ///
    /// # 示例
    ///
    /// ```rust
    /// use xwrust::xwos::thd;
    ///
    /// let builder = thd::DThdBuilder::new()
    ///     .name("foo".into()) // 设置线程的名称
    ///     .stack_size(8 * 1024) // 设置线程栈大小
    ///     .privileged(true); // 设置系统权限
    /// match unsafe { builder.spawn_unchecked(|_| {
    ///     // 线程代码;
    ///     // 返回值
    /// })} {
    ///     Ok(handler) => {
    ///         match handler.join() {
    ///             Ok(r) => {
    ///                 // r 是线程闭包的返回值。
    ///             },
    ///             Err(e) => {
    ///                 // join() 失败时的错误码可通过 e.state() 获取。
    ///                 // e 是 DThdHandle<R> ，重新被返回。
    ///             },
    ///         };
    ///     },
    ///     Err(rc) => {
    ///         // rc 是 spawn() 失败时的错误码。
    ///     }
    /// };
    /// ```
    /// [`Result`]: <https://doc.rust-lang.org/core/result/enum.Result.html>
    pub unsafe fn spawn_unchecked<'a, F, R>(self, f: F) -> Result<DThdHandle<R>, XwEr>
    where
        F: FnOnce(Arc<DThdElement>) -> R,
        F: Send + 'a,
        R: Send + 'a,
    {
        Ok(DThdHandle{inner: self.spawn_unchecked_(f)?})
    }

    unsafe fn spawn_unchecked_<'a, F, R>(self, f: F) -> Result<DThdHandleInner<R>, XwEr>
    where
        F: FnOnce(Arc<DThdElement>) -> R,
        F: Send + 'a,
        R: Send + 'a,
    {
        let mut attr: ThdAttr = mem::zeroed();
        xwrustffi_thd_attr_init(&mut attr);
        let name = self.name.unwrap_or("anon".into());
        attr.stack_size = self.stack_size.unwrap_or(xwrustffi_thd_stack_size_default());
        attr.privileged = self.privileged.unwrap_or(true);
        let element: Arc<DThdElement> =
            Arc::new(DThdElement::new(name, attr.stack_size, attr.privileged));
        let thd_element = element.clone();

        let retval: Arc<DThdReturnValue<R>> =
            Arc::new(DThdReturnValue { result: UnsafeCell::new(None) });
        let thd_retval = retval.clone();

        let main = move || {
            let result = f(thd_element);
            *thd_retval.result.get() = Some(result);
        };

        Ok(DThdHandleInner {
            thdd: ThdD::new(&mut attr,
                            mem::transmute::<Box<dyn FnOnce() + 'a>, Box<dyn FnOnce() + 'static>>(Box::new(main)))?,
            state: ThdJoinState::Joinable,
            element: element,
            rv: retval,
        })
    }
}

/// 新建一个动态线程
///
/// + 创建线程成功，返回一个包含 [`DThdHandle`] 的 [`Result`] ；
/// + 创建线程失败，返回一个包含 [`XwEr`] 的 [`Result`] ， [`XwEr`] 指示错误的原因。
///
/// 此方法使用默认的线程工厂创建线程。
///
/// 当 [`DThdHandle`] 被 [`drop()`] 时，新建的线程会变成 **detached（分离的）** 。此时，新建的线程不能再被 [`join()`] 。
///
/// 方法的签名：
///
/// + [`'static`] 约束是因为新建的线程可能比调用者的生命周期更长，因此线程的闭包和返回值的生命周期限定为静态生命周期；
/// + [`Send`] 约束是因为闭包和返回值需要在线程之间进行转移。并且被移动到 [`Send`] 约束的闭包的变量也必须是 [`Send`] 的，否则编译器会报错。
///   RUSTC是通过 [`Send`] 约束来区分闭包是不是另一线程的代码的。
///
/// # 参数说明
///
/// + f: 线程的闭包
///
/// # 示例
///
/// ```rust
/// use xwrust::xwos::thd;
///
/// match thd::spawn(|_| {
///     // 线程代码;
///     // 返回值
/// }) {
///     Ok(handler) => {
///         match handler.join() {
///             Ok(r) => {
///                 // r 是线程闭包的返回值。
///             },
///             Err(e) => {
///                 // join() 失败时的错误码可通过 e.state() 获取。
///                 // e 是 DThdHandle<R> ，重新被返回。
///             },
///         };
///     },
///     Err(rc) => {
///         // rc 是 spawn() 失败时的错误码。
///     },
/// };
/// ```
/// [`Result`]: <https://doc.rust-lang.org/core/result/enum.Result.html>
/// [`drop()`]: <https://doc.rust-lang.org/core/ops/trait.Drop.html#tymethod.drop>
/// [`join()`]: DThdHandle::join
/// [`'static`]: <https://doc.rust-lang.org/std/keyword.static.html>
/// [`Send`]: <https://doc.rust-lang.org/core/marker/trait.Send.html>
pub fn spawn<F, R>(f: F) -> Result<DThdHandle<R>, XwEr>
where
    F: FnOnce(Arc<DThdElement>) -> R,
    F: Send + 'static,
    R: Send + 'static,
{
    DThdBuilder::new().spawn(f)
}

/// 动态线程的元素
///
/// 动态线程的元素中的数据需跨线程共享，因此在定义时需要使用 [`Arc`] 进行封装。
///
/// [`Arc`]: <https://doc.rust-lang.org/alloc/sync/struct.Arc.html>
pub struct DThdElement {
    /// 线程的名字
    name: String,
    /// 线程栈的大小，以字节(byte)为单位
    stack_size: XwSz,
    /// 是否为特权线程
    privileged: bool,
}

impl DThdElement {
    pub(crate) fn new(name: String,
                      stack_size: XwSz,
                      privileged: bool) -> DThdElement {
        DThdElement {
            name,
            stack_size,
            privileged
        }
    }

    /// 返回动态线程名字的引用
    ///
    /// # 示例
    ///
    /// ```rust
    /// use xwrust::xwos::thd;
    /// use libc_print::std_name::println;
    ///
    /// let handler = thd::DThdBuilder::new()
    ///     .name("foo".into())
    ///     .spawn(|ele| {
    ///         println!("Thread name: {}", ele.name());
    ///     });
    /// ```
    pub fn name(&self) -> &str {
        &self.name
    }

    /// 返回动态线程的栈大小
    ///
    /// # 示例
    ///
    /// ```rust
    /// use xwrust::xwos::thd;
    /// use libc_print::std_name::println;
    ///
    /// let handler = thd::DThdBuilder::new()
    ///     .spawn(|ele| {
    ///         println!("Thread stack size: {}", ele.stack_size()); // 将返回默认线程大小
    ///     });
    /// ```
    pub fn stack_size(&self) -> XwSz {
        self.stack_size
    }

    /// 返回动态线程是否具有特权
    ///
    /// # 示例
    ///
    /// ```rust
    /// use xwrust::xwos::thd;
    /// use libc_print::std_name::println;
    ///
    /// let handler = thd::DThdBuilder::new()
    ///     .privileged(true);
    ///     .spawn(|ele| {
    ///         println!("Thread is privileged: {} .", ele.privileged());
    ///     });
    /// ```
    pub fn privileged(&self) -> bool {
        self.privileged
    }
}

impl fmt::Debug for DThdElement {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        f.debug_struct("DThdElement")
            .field("name", &self.name)
            .finish()
    }
}

/// 动态线程的返回值
///
/// 动态线程的返回值中的数据需跨线程共享，因此在定义时需要使用 [`Arc`] 进行封装。
///
/// [`Arc`]: <https://doc.rust-lang.org/alloc/sync/struct.Arc.html>
struct DThdReturnValue<R> {
    result: UnsafeCell<Option<R>>,
}

unsafe impl<R: Sync> Sync for DThdReturnValue<R> {}


struct DThdHandleInner<R> {
    /// XWOS线程的描述符
    thdd: ThdD,
    /// 线程的 `join()/stop()` 状态
    state: ThdJoinState,
    /// 线程的元素
    element: Arc<DThdElement>,
    /// 线程的返回值
    rv: Arc<DThdReturnValue<R>>,
}

impl<R> DThdHandleInner<R> {
    fn intr(&self) -> XwEr {
        self.thdd.intr()
    }

    fn quit(&self) -> XwEr {
        self.thdd.quit()
    }

    fn join(mut self) -> Result<R, Self> {
        let rc = self.thdd.join();
        if rc == 0 {
            self.state = ThdJoinState::Joined;
            Ok(Arc::get_mut(&mut self.rv).unwrap().result.get_mut().take().unwrap())
        } else {
            self.state = ThdJoinState::JoinErr(rc);
            Err(self)
        }
    }

    fn stop(mut self) -> Result<R, Self> {
        let rc = self.thdd.stop();
        if rc == 0 {
            self.state = ThdJoinState::Joined;
            Ok(Arc::get_mut(&mut self.rv).unwrap().result.get_mut().take().unwrap())
        } else {
            self.state = ThdJoinState::JoinErr(rc);
            Err(self)
        }
    }
}

impl<R> Drop for DThdHandleInner<R> {
    fn drop(&mut self) {
        match self.state {
            ThdJoinState::Joined => {
            },
            _ => {
                self.thdd.detach();
            },
        };
    }
}

impl<R> fmt::Debug for DThdHandleInner<R> {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        f.debug_struct("DThdHandleInner")
            .field("thdd", &self.thdd)
            .field("state", &self.state)
            .field("element", &self.element)
            .finish_non_exhaustive()
    }
}

/// 动态线程的句柄
pub struct DThdHandle<R> {
    inner: DThdHandleInner<R>
}

unsafe impl<R> Send for DThdHandle<R> {}
unsafe impl<R> Sync for DThdHandle<R> {}

impl<R> DThdHandle<R> {
    /// 返回XWOS的线程对象描述符
    ///
    /// 线程对象描述符用于与C语言交互。
    pub fn thdd(&self) -> &ThdD {
        &self.inner.thdd
    }

    /// 返回线程的元素
    pub fn element(&self) -> &DThdElement {
        &self.inner.element
    }

    /// 中断线程的阻塞态和睡眠态
    ///
    /// 此方法用于中断线程的 **阻塞态** 和 **睡眠态** 。
    ///
    /// # 上下文
    ///
    /// + 线程、中断、中断底半部、空闲任务
    pub fn intr(&self) -> XwEr {
        self.inner.intr()
    }

    /// 通知动态线程退出
    ///
    /// 此方法用于向线程设置 **退出状态** 。
    ///
    /// 调用此方法的线程不会等待被设置 **退出状态** 的线程退出。
    ///
    /// 此方法可被重复调用，线程的 **退出状态** 一旦被设置，不可被清除。
    ///
    /// # 上下文
    ///
    /// + 线程、中断、中断底半部、空闲任务
    pub fn quit(&self) -> XwEr {
        self.inner.quit()
    }

    /// 等待动态线程运行至退出，并返回线程的返回值
    ///
    /// + 如果动态子线程还在运行，此方法会阻塞父线程直到动态子线程退出。父线程的阻塞状态可被中断；
    /// + 如果动态子线程已经提前运行至退出，此方法可立即返回动态子线程的返回值。
    ///
    /// 此方法会消费 [`self`] ：
    ///
    /// + 如果此方法执行成功，会消费掉 [`self`] ，并将动态子线程的返回值放在 [`Ok()`] 中返回，因为线程已经结束，其 [`DThdHandle`] 的生命周期也应该结束；
    /// + 如果此方法执行失败，会重新在 [`Err()`] 中返回 [`self`] ，并可通过 [`DThdHandle::state()`] 方法获取失败的原因。
    ///
    /// # 上下文
    ///
    /// + 线程
    ///
    /// # 示例
    ///
    /// ```rust
    /// use xwrust::xwos::thd;
    ///
    /// match thd::spawn(|_| {
    ///     // 线程代码;
    ///     // 返回值
    /// }) {
    ///     Ok(handler) => {
    ///         match handler.join() {
    ///             Ok(r) => {
    ///                 // r 是线程闭包的返回值。
    ///             },
    ///             Err(e) => {
    ///                 // `join()` 失败时的错误码可通过 `e.state()` 获取。
    ///                 // `e` 是 `DThdHandle<R>` ，重新被返回。
    ///             },
    ///         };
    ///     },
    ///     Err(rc) => {
    ///         // rc 是 spawn() 失败时的错误码。
    ///     },
    /// };
    /// ```
    ///
    /// [`Ok()`]: <https://doc.rust-lang.org/core/result/enum.Result.html#variant.Ok>
    /// [`Err()`]: <https://doc.rust-lang.org/core/result/enum.Result.html#variant.Err>
    pub fn join(self) -> Result<R, Self> {
        match self.inner.join() {
            Ok(r) => Ok(r),
            Err(e) => Err(Self{inner: e}),
        }
    }

    /// 终止动态线程并等待线程运行至退出，并返回线程的返回值
    ///
    /// + 如果动态子线程还在运行，此方法会阻塞父线程直到动态子线程退出。父线程的阻塞状态可被中断；
    /// + 如果动态子线程已经提前运行至退出，此方法可立即返回动态子线程的返回值。
    ///
    /// 此方法 = [`DThdHandle::quit()`] + [`DThdHandle::join()`]
    ///
    ///
    /// 此方法会消费 [`self`] ：
    ///
    /// + 如果此方法执行成功，会消费掉 [`self`] ，并将动态子线程的返回值放在 [`Ok()`] 中返回，因为线程已经结束，其 [`DThdHandle`] 的生命周期也应该结束；
    /// + 如果此方法执行失败，会重新在 [`Err()`] 中返回 [`self`] ，并可通过 [`DThdHandle::state()`] 方法获取失败的原因。
    ///
    /// # 上下文
    ///
    /// + 线程
    ///
    /// # 示例
    ///
    /// ```rust
    /// use xwrust::xwos::thd;
    ///
    /// match thd::spawn(|_| {
    ///     // 线程代码;
    ///     // 返回值
    /// }) {
    ///     Ok(handler) => {
    ///         match handler.stop() {
    ///             Ok(r) => {
    ///                 // r 是线程闭包的返回值。
    ///             },
    ///             Err(e) => {
    ///                 // `stop()` 失败时的错误码可通过 `e.state()` 获取。
    ///                 // `e` 是 `DThdHandle<R>` ，重新被返回。
    ///             },
    ///         };
    ///     },
    ///     Err(rc) => {
    ///         // rc 是 spawn() 失败时的错误码。
    ///     },
    /// };
    /// ```
    ///
    /// [`Ok()`]: <https://doc.rust-lang.org/core/result/enum.Result.html#variant.Ok>
    /// [`Err()`]: <https://doc.rust-lang.org/core/result/enum.Result.html#variant.Err>
    pub fn stop(self) -> Result<R, Self> {
        match self.inner.stop() {
            Ok(r) => Ok(r),
            Err(e) => Err(Self{inner: e}),
        }
    }

    /// 返回动态线程的 `join()/stop()` 状态
    pub fn state<'a>(&'a self) -> &'a ThdJoinState {
        &self.inner.state
    }

    /// 检查关联的动态线程是否运行结束
    ///
    /// 此方法不会阻塞调用者。
    pub fn finished(&self) -> bool {
        Arc::strong_count(&self.inner.rv) == 1
    }
}

impl<R> fmt::Debug for DThdHandle<R> {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        f.debug_struct("DThdHandle")
            .field("inner", &self.inner)
            .finish_non_exhaustive()
    }
}


////////////////////////////////////////////////////////////////////////////////
// 静态线程
////////////////////////////////////////////////////////////////////////////////
/// XWOS线程对象占用的内存大小
#[cfg(target_pointer_width = "32")]
pub const SIZEOF_XWOS_THD: usize = 280;

/// XWOS线程占用的内存大小
#[cfg(target_pointer_width = "64")]
pub const SIZEOF_XWOS_THD: usize = 560;

/// 用于构建线程对象的内存数组类型
#[repr(C)]
#[cfg_attr(target_pointer_width = "32", repr(align(8)))]
#[cfg_attr(target_pointer_width = "64", repr(align(16)))]
pub(crate) struct XwosThd
{
    pub(crate) obj: [u8; SIZEOF_XWOS_THD],
}

/// 用于构建线程栈的内存数组类型
#[repr(C)]
#[cfg_attr(target_pointer_width = "32", repr(align(8)))]
#[cfg_attr(target_pointer_width = "64", repr(align(16)))]
pub(crate) struct XwosThdStack<const N: XwSz>
where
    [XwStk; N]: Sized
{
    pub(crate) stk: [XwStk; N],
}

/// 静态线程对象结构体
///
/// 此结构体用于创建具有 [`'static`] 生命周期的线程对象。
///
/// [`'static`]: <https://doc.rust-lang.org/std/keyword.static.html>
pub struct SThd<const N: XwSz, R>
where
    [XwStk; N]: Sized,
    R: Send
{
    /// 线程对象
    pub(crate) thd: UnsafeCell<XwosThd>,
    /// 线程的栈
    pub(crate) stk: UnsafeCell<XwosThdStack<N>>,
    /// 线程的名字
    pub(crate) name: &'static str,
    /// 线程是否为特权线程
    pub(crate) privileged: UnsafeCell<bool>,
    /// 线程的函数
    pub(crate) func: UnsafeCell<Option<fn(&Self) -> R>>,
    /// 线程对象的标签
    pub(crate) tik: UnsafeCell<XwSq>,
    /// 线程的执行结果
    pub(crate) result: UnsafeCell<Option<R>>,
}

impl<const N: XwSz, R> SThd<N, R>
where
    [XwStk; N]: Sized,
    R: Send
{
    /// 新建静态线程对象
    ///
    /// 此方法是编译期方法。
    ///
    /// # 参数说明
    ///
    /// + name: 线程的名字
    /// + privileged: 线程的是否具有系统特权
    ///
    /// # 示例
    ///
    /// ```rust
    /// use xwrust::xwos::thd::*;
    ///
    /// // 线程栈：1024 * 4； 线程返回值类型：&str；线程名字："SThd"；是否为特权线程：true；
    /// static STHD: SThd<1024, &str> = SThd::new("SThd", true);
    /// ```
    pub const fn new(name: &'static str, privileged: bool) -> Self {
        Self {
            thd: UnsafeCell::new(XwosThd{obj: [0; SIZEOF_XWOS_THD]}),
            stk: UnsafeCell::new(XwosThdStack{stk: [0; N]}),
            name: name,
            privileged: UnsafeCell::new(privileged),
            func: UnsafeCell::new(None),
            tik: UnsafeCell::new(0),
            result: UnsafeCell::new(None),
        }
    }

    /// 以线程函数 `f` 启动静态线程
    ///
    /// 此方法限定了参数的 [`&'static self`] ，因此若试图在函数局部定义静态线程，
    /// 调用此方法时，编译器会提示错误：生命周期不够长。
    ///
    /// 此约束的目的是让用户只能在函数外部定义静态线程对象。
    ///
    ///
    /// # 线程函数
    ///
    /// 线程函数的原型为 `fn(&Self) -> R` ，线程运行时，系统会以静态线程自身的引用作为参数调用线程函数 `f` 。
    ///
    /// 用户可通过此引用调用线程的方法或访问线程的名字，栈大小等信息。
    ///
    /// # 返回值
    ///
    /// 此方法会返回静态线程的句柄 [`SThdHandle<'a, N, R>`] ，通过此句柄，其他线程可通知此线程退出并获取返回值。
    ///
    ///
    /// # 上下文
    ///
    /// + 线程、中断、中断底半部、空闲任务
    ///
    ///
    /// # 示例
    ///
    /// ```rust
    /// use xwrust::xwos::thd::*;
    ///
    /// static STHD: SThd<1024, &str> = SThd::new("SThd", true);
    /// pub fn xwrust_example_sthd() {
    ///     STHD.run(|sthd| { // 子线程
    ///         // 线程功能
    ///         "OK" // 返回值
    ///     });
    /// }
    /// ```
    pub fn run<'a>(&'static self, f: fn(&Self) -> R) -> SThdHandle<'a, N, R> {
        unsafe {
            let mut attr: ThdAttr = mem::zeroed();
            xwrustffi_thd_attr_init(&mut attr);
            attr.stack = self.stk.get() as _;
            attr.stack_size = N * (XwStk::BITS / 8) as XwSz;
            attr.privileged = *self.privileged.get();
            (*self.func.get()) = Some(f);
            xwrustffi_thd_init(self.thd.get() as _, self.tik.get(), &attr,
                               SThd::<N, R>::xwrust_sthd_entry,
                               self as *const Self as *mut c_void);

            SThdHandle {
                sthd: UnsafeCell::new(self),
                state: ThdJoinState::Joinable,
            }
        }
    }

    extern "C" fn xwrust_sthd_entry(arg: *mut c_void) -> XwEr {
        unsafe {
            let thd: &Self = &*(arg as *const Self);
            let func = (*thd.func.get()).unwrap_or_else(|| unreachable!());
            *(thd.result.get()) = Some(func(thd));
        }
        0
    }

    /// 返回静态线程名字的引用
    ///
    /// # 上下文
    ///
    /// + 任意
    pub fn name(&self) -> &str {
        self.name
    }

    /// 返回静态线程是否具有特权
    ///
    /// # 上下文
    ///
    /// + 任意
    pub fn privileged(&self) -> bool {
        unsafe {
            *self.privileged.get()
        }
    }

    /// 返回静态线程的栈大小
    ///
    /// # 上下文
    ///
    /// + 任意
    pub const fn stack_size(&self) -> XwSz {
        N * (XwStk::BITS / 8) as XwSz
    }

    /// 中断线程的阻塞态和睡眠态
    ///
    /// 此方法用于中断线程的 **阻塞状态** 和 **睡眠状态** 。
    ///
    /// # 上下文
    ///
    /// + 线程、中断、中断底半部、空闲任务
    pub fn intr(&self) -> XwEr {
        unsafe {
            xwrustffi_thd_intr(self.thd.get() as _, *self.tik.get())
        }
    }

    /// 通知静态线程退出
    ///
    /// 此方法用于向线程设置 **退出状态** 。
    ///
    /// 调用此方法的线程不会等待被设置 **退出状态** 的线程退出。
    ///
    /// 此方法可被重复调用，线程的 **退出状态** 一旦被设置，不可被清除。
    ///
    /// # 上下文
    ///
    /// + 线程、中断、中断底半部、空闲任务
    pub fn quit(&self) -> XwEr {
        unsafe {
            xwrustffi_thd_quit(self.thd.get() as _, *self.tik.get())
        }
    }

    fn join(&self) ->XwEr {
        unsafe {
            xwrustffi_thd_join(self.thd.get() as _, *self.tik.get(), ptr::null_mut())
        }
    }

    fn stop(&self) -> XwEr {
        unsafe {
            xwrustffi_thd_stop(self.thd.get() as _, *self.tik.get(), ptr::null_mut())
        }
    }

    fn detach(&self) -> XwEr {
        unsafe {
            xwrustffi_thd_detach(self.thd.get() as _, *self.tik.get())
        }
    }

    /// 将线程迁移到目标CPU。
    pub fn migrate(&self, cpuid: XwId) -> XwEr {
        unsafe {
            xwrustffi_thd_migrate(self.thd.get() as _, *self.tik.get(), cpuid)
        }
    }
}

impl<const N: XwSz, R> !Send for SThd<N, R>
where
    [XwStk; N]: Sized,
    R: Send
{}

unsafe impl<const N: XwSz, R> Sync for SThd<N, R>
where
    [XwStk; N]: Sized,
    R: Send
{}

/// 静态线程的句柄
pub struct SThdHandle<'a, const N: XwSz, R>
where
    [XwStk; N]: Sized,
    R: Send
{
    /// 线程的引用
    sthd: UnsafeCell<&'a SThd<N, R>>,
    /// 静态线程的 `join()/stop()` 状态
    state: ThdJoinState,
}

unsafe impl<'a, const N: XwSz, R> Send for SThdHandle<'a, N, R>
where
    [XwStk; N]: Sized,
    R: Send
{}

unsafe impl<'a, const N: XwSz, R> Sync for SThdHandle<'a, N, R>
where
    [XwStk; N]: Sized,
    R: Send
{}

impl<'a, const N: XwSz, R> SThdHandle<'a, N, R>
where
    [XwStk; N]: Sized,
    R: Send
{
    /// 中断线程的阻塞态和睡眠态
    ///
    /// 此方法用于中断线程的 **阻塞状态** 和 **睡眠状态** 。
    ///
    /// # 上下文
    ///
    /// + 线程、中断、中断底半部、空闲任务
    pub fn intr(&self) -> XwEr {
        unsafe {
            (*(self.sthd.get())).intr()
        }
    }

    /// 通知静态线程退出
    ///
    /// 此方法用于向线程设置 **退出状态** 。
    ///
    /// 调用此方法的线程不会等待被设置 **退出状态** 的线程退出。
    ///
    /// 此方法可被重复调用，线程的 **退出状态** 一旦被设置，不可被清除。
    ///
    /// # 上下文
    ///
    /// + 线程、中断、中断底半部、空闲任务
    pub fn quit(&self) -> XwEr {
        unsafe {
            (*(self.sthd.get())).quit()
        }
    }

    /// 等待静态线程运行至退出，并返回线程的返回值
    ///
    /// + 如果静态子线程还在运行，此方法会阻塞父线程直到静态子线程退出。父线程的阻塞状态可被中断；
    /// + 如果静态子线程已经提前运行至退出，此方法可立即返回静态子线程的返回值。
    ///
    /// 此方法会消费 [`self`] ：
    ///
    /// + 如果此方法执行成功，会消费掉 [`self`] ，并将静态子线程的返回值放在 [`Ok()`] 中返回，因为线程已经结束，其 [`SThdHandle`] 的生命周期也应该结束；
    /// + 如果此方法执行失败，会重新在 [`Err()`] 中返回 [`self`] ，并可通过 [`SThdHandle::state()`] 方法获取失败的原因。
    ///
    /// # 上下文
    ///
    /// + 线程
    ///
    /// # 示例
    ///
    /// ```rust
    /// use xwrust::xwos::thd::*;
    ///
    /// static STHD: SThd<1024, &str> = SThd::new("SThd", true);
    ///
    /// pub fn xwrust_example_sthd() {
    ///     let mut h = STHD.run(|_| {
    ///         // 线程功能
    ///         "Ok" // 返回值
    ///     });
    ///     let res = h.join();
    ///     match res {
    ///         Ok(r) => {
    ///             // `r` 是线程的返回值。
    ///         },
    ///         Err(e) => {
    ///             h = e;
    ///             // `join()` 失败时的错误码可通过 `e.state()` 获取。
    ///             // `e` 是 `SThdHandle<'a, N, R>` ，重新被返回。
    ///         },
    ///     };
    /// }
    /// ```
    ///
    /// [`Ok()`]: <https://doc.rust-lang.org/core/result/enum.Result.html#variant.Ok>
    /// [`Err()`]: <https://doc.rust-lang.org/core/result/enum.Result.html#variant.Err>
    pub fn join(mut self) -> Result<R, Self> {
        unsafe {
            let rc = (*(self.sthd.get())).join();
            if rc == 0 {
                self.state = ThdJoinState::Joined;
                Ok((*(self.sthd.get_mut().result.get())).take().unwrap())
            } else {
                self.state = ThdJoinState::JoinErr(rc);
                Err(self)
            }
        }
    }

    /// 终止静态线程并等待线程运行至退出，并返回线程的返回值
    ///
    /// + 如果静态子线程还在运行，此方法会阻塞父线程直到静态子线程退出。父线程的阻塞状态可被中断；
    /// + 如果静态子线程已经提前运行至退出，此方法可立即返回静态子线程的返回值。
    ///
    /// 此方法 = [`SThdHandle::quit()`] + [`SThdHandle::join()`]
    ///
    ///
    /// 此方法会消费 [`self`] ：
    ///
    /// + 如果此方法执行成功，会消费掉 [`self`] ，并将静态子线程的返回值放在 [`Ok()`] 中返回，因为线程已经结束，其 [`SThdHandle`] 的生命周期也应该结束；
    /// + 如果此方法执行失败，会重新在 [`Err()`] 中返回 [`self`] ，并可通过 [`SThdHandle::state()`] 方法获取失败的原因。
    ///
    /// # 上下文
    ///
    /// + 线程
    ///
    /// # 示例
    ///
    /// ```rust
    /// use xwrust::xwos::thd::*;
    ///
    /// static STHD: SThd<1024, &str> = SThd::new("SThd", true);
    ///
    /// pub fn xwrust_example_sthd() {
    ///     let mut h = STHD.run(|_| {
    ///         // 线程代码;
    ///         "Ok"
    ///     });
    ///     let res = h.stop();
    ///     match res {
    ///         Ok(r) => {
    ///             // `r` 是线程的返回值。
    ///         },
    ///         Err(e) => {
    ///             h = e;
    ///             // `stop()` 失败时的错误码可通过 `e.state()` 获取。
    ///             // `e` 是 `SThdHandle<'a, N, R>` ，重新被返回。
    ///         },
    ///     };
    /// }
    /// ```
    ///
    /// [`Ok()`]: <https://doc.rust-lang.org/core/result/enum.Result.html#variant.Ok>
    /// [`Err()`]: <https://doc.rust-lang.org/core/result/enum.Result.html#variant.Err>
    pub fn stop(mut self) -> Result<R, Self> {
        unsafe {
            let rc = (*(self.sthd.get())).stop();
            if rc == 0 {
                self.state = ThdJoinState::Joined;
                Ok((*(self.sthd.get_mut().result.get())).take().unwrap())
            } else {
                self.state = ThdJoinState::JoinErr(rc);
                Err(self)
            }
        }
    }

    /// 返回线程的 `join()/stop()` 状态
    pub fn state(&self) -> &ThdJoinState {
        &self.state
    }
}

impl<'a, const N: XwSz, R>Drop for SThdHandle<'a, N, R>
where
    [XwStk; N]: Sized,
    R: Send
{
    fn drop(&mut self) {
        match self.state {
            ThdJoinState::Joined => {
            },
            _ => {
                unsafe {
                    (*(self.sthd.get())).detach();
                }
            },
        };
    }
}
