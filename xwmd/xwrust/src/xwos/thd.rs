//! XWOS RUST：线程库
//! ========
//!
//! # 线程模型
//!
//! XWOS RUST框架可让用户在Rust的代码中创建XWOS的线程，与C语言的线程一样，每个线程都有独立的名字
//! 和栈空间。
//!
//! XWOS RUST框架的线程库是仿照 [`std::thread`] 的来编写的，以降低熟悉Rust的朋友的学习代价。
//!
//! ```rust
//! use xwrust::xwos::thd;
//!
//! thd::spawn(|_| {
//!     // 线程代码;
//! });
//! ```
//!
//! 在此例子中，线程是自动“分离的(detached)”，此线程运行结束后，其资源会被操作系统自动回收。
//!
//! 线程也可以是“可连接的(joinable)”，另一线程可以获取创建线程时返回的 [`ThdHandle`] ，然后
//! 通过 [`join`] 方法等待线程运行结束：
//!
//! ```rust
//! use xwrust::xwos::thd;
//!
//! let handler = thd::spawn(|_| {
//!     // 线程代码;
//!     // 返回值
//! }).unwrap();
//!
//! let rc = handler.join().unwrap(); // 等待线程结束，并获取其返回值
//! ```
//!
//! [`join`] 方法会返回 [`thd::Result<R>`] ， `R`是返回值的类型，并放在 [`Ok`] 中。
//!
//! ## 与[`std::thread`]的区别
//!
//! [`std::thread`] 的闭包原型是 `FnOnce() -> R` ， `xwrust::xwos::thd` 的闭包原型是
//! `FnOnce(Arc<ThdElement>) -> R` ， [`ThdElement`] 用于存放与线程相关的元素。
//! XWOS RUST创建线程时也会创建 [`ThdElement`] ，并用作调用闭包的参数。
//!
//! + [`std::thread`] 通过 `thread::current()` 获取线程自己的`handle`，然后获取线程的名称：
//!
//! ```rust
//! use std::thread;
//!
//! let handler = thread::Builder::new()
//!     .name("named thread".into())
//!     .spawn(|| {
//!         let handle = thread::current();
//!         assert_eq!(handle.name(), Some("named thread"));
//!     }).unwrap();
//!
//! handler.join().unwrap();
//! ```
//!
//! + `xwrust::xwos::thd` 通过闭包参数获取 [`ThdElement`] ，然后通过 [`ele.name()`] 获取线程的名称：
//!
//! ```rust
//! use xwrust::xwos::thd;
//! use libc_print::std_name::println;
//!
//! let handler = thd::Builder::new()
//!     .name("foo".into())
//!     .spawn(|ele| {
//!         println!("Thread name: {}", ele.name().unwrap());
//!     }).unwrap();
//!
//! handler.join().unwrap();
//! ```
//!
//! [`ThdElement`] 充当了`handler`的角色。不使用时，使用`_`占位。
//!
//!
//! # 线程的工厂模式
//!
//! 可以通过线程工厂设置线程属性后，再创建线程：
//!
//! ```rust
//! use xwrust::xwos::thd;
//!
//! let builder = thd::Builder::new()
//!                            .name("foo".into()) // 设置线程的名称
//!                            .stack_size(8 * 1024) // 设置线程栈大小
//!                            .privileged(true); // 设置系统权限
//!
//! let handler = builder.spawn(|_| {
//!     // 线程代码;
//!     // 返回值
//! }).unwrap();
//!
//! let rc = handler.join().unwrap(); // 等待线程结束，并获取其返回值
//! ```
//!
//! ## 线程的名称
//!
//! 线程工厂可以为线程指定一个字符串名字，默认为空。线程的名称可用于日志输出。
//!
//! ## 线程的栈大小
//!
//! 线程工厂可以为线程指定栈内存大小，默认为系统配置 `XWMMCFG_STACK_SIZE_MIN` 。
//!
//! ## 线程的系统权限
//!
//! 某些SOC内部的寄存器，只有拥有系统权限的线程才可以访问，通常这些线程用于提供驱动功能。
//! 默认为有系统权限。
//!
//!
//! # 线程的元素
//!
//! 线程的元素 [`ThdElement`] 是存放与线程相关的某些数据的内存空间，例如线程的名称 。
//!
//!
//! # 线程的句柄
//!
//! 线程的句柄 [`ThdHandle`] 功能类似于 [`std::thread::JoinHandle`] ，可通过 [`join`]
//! 方法等待线程结束。
//!
//!
//! [`std::thread`]: https://doc.rust-lang.org/std/thread/index.html
//! [`std::thread::JoinHandle`]: http://localhost/std/thread/struct.JoinHandle.html
//! [`join`]: ThdHandle::join
//! [`thd::Result<R>`]: Result<R>
//! [`Ok`]: https://doc.rust-lang.org/core/result/enum.Result.html#variant.Ok
//! [`ele.name()`]: ThdElement::name
//!

extern crate core;
use core::ffi::*;
use core::option::Option;
use core::any::Any;
use core::cell::UnsafeCell;
use core::ptr;
use core::mem;
use core::str;

extern crate alloc;
use alloc::boxed::Box;
use alloc::string::String;
use alloc::sync::Arc;

use cstr_core::CString;
use cstr_core::CStr;

use crate::types::*;

/// XWOS线程的属性
///
/// 完全等价于C语言头文件 `xwos/osal/skd.h` 中的 `struct xwos_thd_attr`
#[repr(C)]
pub struct ThdAttr {
    /// 线程的名字
    name: *const cty::c_char,
    /// 线程栈的首地址
    stack: *mut XwStk,
    /// 线程栈的大小，以字节(byte)为单位
    stack_size: XwSz,
    /// 栈内存警戒线位置
    stack_guard_size: XwSz,
    /// 优先级
    priority: XwPr,
    /// 是否为分离态
    detached: bool,
    /// 是否为特权线程
    privileged: bool,
}

/// XWOS的线程
pub struct Thd {
    /// XWOS的线程的结构体的指针
    thd: *mut c_void,
    /// XWOS的线程的标签
    tik: XwSq,
}

unsafe impl Send for Thd {}
unsafe impl Sync for Thd {}

extern "C" {
    fn xwrustffi_skd_get_context_lc(
        ctxbuf: *mut XwSq,
        irqnbuf: *mut XwIrq);
    fn xwrustffi_skd_get_timetick_lc() -> XwTm;
    fn xwrustffi_skd_get_tickcount_lc() -> u64;
    fn xwrustffi_skd_get_timestamp_lc() -> XwTm;
    fn xwrustffi_skd_dspmpt_lc();
    fn xwrustffi_skd_enpmpt_lc();
    fn xwrustffi_thd_stack_size_default() -> XwSz;
    fn xwrustffi_thd_attr_init(attr: *mut ThdAttr);
    fn xwrustffi_thd_create(thd: *mut *mut c_void,
                            attr: *const ThdAttr,
                            mainfunc: extern "C" fn(*mut c_void) -> XwEr,
                            arg: *mut c_void) -> XwEr;
    fn xwrustffi_thd_gettik(thd: *mut c_void) -> XwSq;
    fn xwrustffi_thd_acquire(thd: *mut c_void,
                             tik: XwSq) -> XwEr;
    fn xwrustffi_thd_release(thd: *mut c_void,
                             tik: XwSq) -> XwEr;
    fn xwrustffi_thd_quit(thd: *mut c_void) -> XwEr;
    fn xwrustffi_thd_stop(thd: *mut c_void,
                          trc: *mut XwEr) -> XwEr;
    fn xwrustffi_thd_join(thd: *mut c_void,
                          trc: *mut XwEr) -> XwEr;
    fn xwrustffi_thd_detach(thd: *mut c_void) -> XwEr;
    fn xwrustffi_thd_migrate(thd: *mut c_void,
                             cpuid: XwId) -> XwEr;
    fn xwrustffi_cthd_self() -> *mut c_void;
    fn xwrustffi_cthd_yield();
    fn xwrustffi_cthd_exit(rc: XwEr);
    fn xwrustffi_cthd_shld_frz() -> bool;
    fn xwrustffi_cthd_shld_stop() -> bool;
    fn xwrustffi_cthd_frz_shld_stop(frozen: *mut bool) -> bool;
    fn xwrustffi_cthd_sleep(time: *mut XwTm) -> XwEr;
    fn xwrustffi_cthd_sleep_from(origin: *mut XwTm,
                                 inc: XwTm) -> XwEr;
    fn xwrustffi_cthd_freeze(time: *mut XwTm) -> XwEr;
}

impl Thd {
    pub unsafe fn new(attr: &ThdAttr, func: Box<dyn FnOnce()>) -> core::result::Result<Thd, XwEr> {
        let boxfunc = Box::new(func);
        let f = Box::into_raw(boxfunc);
        let mut thd: *mut c_void = ptr::null_mut();
        let rc = xwrustffi_thd_create(&mut thd, attr, xwrust_thd_entry, f as *mut _);
        return if rc < 0 {
            drop(Box::from_raw(f));
            Err(rc)
        } else {
            let xwostik = xwrustffi_thd_gettik(thd);
            Ok(Thd {thd: thd, tik: xwostik})
        };

        extern "C" fn xwrust_thd_entry(main: *mut c_void) -> XwEr {
            unsafe {
                Box::from_raw(main as *mut Box<dyn FnOnce()>)();
            }
            0
        }
    }

    pub fn join(self) {
        unsafe {
            let _rc = xwrustffi_thd_join(self.thd, ptr::null_mut());
            mem::forget(self);
        }
    }
}

impl Drop for Thd {
    fn drop(&mut self) {
        let _rc = unsafe { xwrustffi_thd_detach(self.thd) };
    }
}


////////////////////////////////////////////////////////////////////////////////
// 工厂模式
////////////////////////////////////////////////////////////////////////////////

/// 线程的工厂模式结构体，可用于配置新线程的属性
///
/// 有三种种配置可供选择：
///
/// - [`name`]: 设置[线程的名字][线程的名称]
/// - [`stack_size`]: 设置[线程的栈大小][线程的栈大小]
/// - [`privileged`]: 设置[线程的系统权限][线程的系统权限]
///
/// [`spawn`] 方法将获取构建器的所有权，并使用给定的配置创建线程，返回 [`core::result::Result`] 。
///
/// [`thd::spawn`] 独立的函数，并使用默认配置的 `Builder`创建线程，返回 [`core::result::Result`] 。
///
///
/// # 示例
///
/// ```rust
/// use xwrust::xwos::thd;
///
/// let builder = thd::Builder::new();
///
/// let handler = builder.spawn(|_| {
///     // 线程代码;
///     // 返回值
/// }).unwrap();
///
/// let rc = handler.join().unwrap(); // 等待线程结束，并获取其返回值
/// ```
///
/// [`name`]: Builder::name
/// [`stack_size`]: Builder::stack_size
/// [`privileged`]: Builder::privileged
/// [`spawn`]: Builder::spawn
/// [`thd::spawn`]: spawn
pub struct Builder {
    /// 线程的名字
    name: Option<String>,
    /// 线程栈的大小，以字节(byte)为单位
    stack_size: Option<usize>,
    /// 是否为特权线程
    privileged: Option<bool>,
}

impl Builder {
    /// 新建用于创建线程的工厂
    ///
    ///
    /// # 示例
    ///
    /// ```rust
    /// use xwrust::xwos::thd;
    ///
    /// let builder = thd::Builder::new()
    ///                            .name("foo".into()) // 设置线程的名称
    ///                            .stack_size(8 * 1024) // 设置线程栈大小
    ///                            .privileged(true); // 设置系统权限
    ///
    /// let handler = builder.spawn(|_| {
    ///     // 线程代码;
    ///     // 返回值
    /// }).unwrap();
    ///
    /// let rc = handler.join().unwrap(); // 等待线程结束，并获取其返回值
    /// ```
    pub fn new() -> Builder {
        Builder {
            name: None,
            stack_size: None,
            privileged: None,
        }
    }

    /// 设置线程的名称
    ///
    ///
    /// # 示例
    ///
    /// ```rust
    /// use xwrust::xwos::thd;
    /// use libc_print::std_name::println;
    ///
    /// let builder = thd::Builder::new()
    ///                            .name("foo".into()); // 设置线程的名称
    ///
    /// let handler = builder.spawn(|ele| {
    ///     println!("My name is {}.", ele.name().unwrap());
    ///     // 线程代码;
    ///     // 返回值
    /// }).unwrap();
    ///
    /// let rc = handler.join().unwrap(); // 等待线程结束，并获取其返回值
    /// ```
    pub fn name(mut self, name: String) -> Builder {
        self.name = Some(name);
        self
    }

    /// 设置线程栈的大小
    ///
    ///
    /// # 示例
    ///
    /// ```rust
    /// use xwrust::xwos::thd;
    ///
    /// let builder = thd::Builder::new()
    ///                            .stack_size(8 * 1024); // 设置线程栈大小
    ///
    /// let handler = builder.spawn(|_| {
    ///     // 线程代码;
    ///     // 返回值
    /// }).unwrap();
    ///
    /// let rc = handler.join().unwrap(); // 等待线程结束，并获取其返回值
    /// ```
    pub fn stack_size(mut self, size: usize) -> Builder {
        self.stack_size = Some(size);
        self
    }

    /// 设置线程栈的系统权限
    ///
    ///
    /// # 示例
    ///
    /// ```rust
    /// use xwrust::xwos::thd;
    ///
    /// let builder = thd::Builder::new()
    ///                            .privileged(true); // 设置系统权限
    ///
    /// let handler = builder.spawn(|_| {
    ///     // 线程代码;
    ///     // 返回值
    /// }).unwrap();
    ///
    /// let rc = handler.join().unwrap(); // 等待线程结束，并获取其返回值
    /// ```
    pub fn privileged(mut self, privileged: bool) -> Builder {
        self.privileged = Some(privileged);
        self
    }

    /// 转移 `Builder` 的所有权，并产生一个新线程
    ///
    /// + 创建线程成功，返回一个包含 [`ThdHandle`] 的 [`core::result::Result`] ；
    /// + 创建线程失败，返回一个包含 [`XwEr`] 的 [`core::result::Result`] ， [`XwEr`] 指示错误的原因。
    ///
    ///
    /// # 示例
    ///
    /// ```rust
    /// use xwrust::xwos::thd;
    ///
    /// let builder = thd::Builder::new()
    ///                            .name("foo".into()) // 设置线程的名称
    ///                            .stack_size(8 * 1024) // 设置线程栈大小
    ///                            .privileged(true); // 设置系统权限
    ///
    /// let handler = builder.spawn(|_| {
    ///     // 线程代码;
    ///     // 返回值
    /// }).unwrap();
    ///
    /// let rc = handler.join().unwrap(); // 等待线程结束，并获取其返回值
    /// ```
    pub fn spawn<F, R>(self, f: F) -> core::result::Result<ThdHandle<R>, XwEr>
    where
        F: FnOnce(Arc<ThdElement>) -> R,
        F: Send + 'static,
        R: Send + 'static,
    {
        unsafe { self.spawn_unchecked(f) }
    }

    /// 转移 `Builder` 的所有权，并产生一个新线程
    ///
    /// + 创建线程成功，返回一个包含 [`ThdHandle`] 的 [`core::result::Result`] ；
    /// + 创建线程失败，返回一个包含 [`XwEr`] 的 [`core::result::Result`] ， [`XwEr`] 指示错误的原因。
    ///
    /// 此方法只要求闭包 `F` 和 返回值 `R` 的生命周期一样长，然后不做其他限制，因此是 `unsafe`的。
    ///
    ///
    /// # 示例
    ///
    /// ```rust
    /// use xwrust::xwos::thd;
    ///
    /// let builder = thd::Builder::new()
    ///                            .name("foo".into()) // 设置线程的名称
    ///                            .stack_size(8 * 1024) // 设置线程栈大小
    ///                            .privileged(true); // 设置系统权限
    ///
    /// let handler = unsafe {
    ///     builder.spawn_unchecked(|| {
    ///         // 线程代码;
    ///         // 返回值
    ///     }).unwrap()
    /// };
    ///
    /// let rc = handler.join().unwrap(); // 等待线程结束，并获取其返回值
    /// ```
    pub unsafe fn spawn_unchecked<'a, F, R>(self, f: F) -> core::result::Result<ThdHandle<R>, XwEr>
    where
        F: FnOnce(Arc<ThdElement>) -> R,
        F: Send + 'a,
        R: Send + 'a,
    {
        Ok(ThdHandle(self.spawn_unchecked_(f)?))
    }

    unsafe fn spawn_unchecked_<'a, F, R>(
        self,
        f: F,
    ) -> core::result::Result<ThdHandleInner<R>, XwEr>
    where
        F: FnOnce(Arc<ThdElement>) -> R,
        F: Send + 'a,
        R: Send + 'a,
    {
        let mut attr: ThdAttr = mem::zeroed();
        xwrustffi_thd_attr_init(&mut attr);
        attr.privileged = self.privileged.unwrap_or(true);
        attr.stack_size = self.stack_size.unwrap_or(xwrustffi_thd_stack_size_default());
        let element: Arc<ThdElement> =
            Arc::new(ThdElement::new(self.name.map(|name| {
            match CString::new(name) {
                Ok(n) => n,
                Err(_e) => {
                    CString::new("InvalidName").unwrap()
                },
            }
            })));
        let thd_element = element.clone();
        attr.name = match element.cname() {
            Some(s) => s.as_ptr(),
            None => ptr::null(),
        };

        let retval: Arc<ThdReturnValue<R>> =
            Arc::new(ThdReturnValue { result: UnsafeCell::new(None) });
        let thd_retval = retval.clone();

        let main = move || {
            let result = f(thd_element);
            *thd_retval.result.get() = Some(Ok(result));
        };

        Ok(ThdHandleInner {
            thd: Thd::new(&attr,
                          mem::transmute::<Box<dyn FnOnce() + 'a>, Box<dyn FnOnce() + 'static>>(
                              Box::new(main)))?,
            element: element,
            rv: retval,
        })
    }
}

/// 产生一个新线程
///
/// + 创建线程成功，返回一个包含 [`ThdHandle`] 的 [`core::result::Result`] ；
/// + 创建线程失败，返回一个包含 [`XwEr`] 的 [`core::result::Result`] ， [`XwEr`] 指示错误的原因。
///
/// 此方法使用默认的线程工厂创建线程。
///
/// 此方法要求闭包 `F` 和 返回值 `R` 的具有 `'static` 生命周期，原因是新线程可能比旧线程运行时间更长。
///
/// # 示例
///
/// ```rust
/// use xwrust::xwos::thd;
///
/// let handler = thd::spawn(|_| {
///     // 线程代码;
///     // 返回值
/// }).unwrap();
///
/// let rc = handler.join().unwrap(); // 等待线程结束，并获取其返回值
/// ```
pub fn spawn<F, R>(f: F) -> core::result::Result<ThdHandle<R>, XwEr>
where
    F: FnOnce(Arc<ThdElement>) -> R,
    F: Send + 'static,
    R: Send + 'static,
{
    Builder::new().spawn(f)
}

/// 线程的元素
///
/// 线程的元素中的数据需跨线程共享，因此在定义时需要使用 [`Arc`] 封装
///
/// [`Arc`]: https://doc.rust-lang.org/alloc/sync/struct.Arc.html
pub struct ThdElement {
    /// 线程的名称
    name: Option<CString>,
}

impl ThdElement {
    pub(crate) fn new(name: Option<CString>) -> ThdElement {
        ThdElement { name }
    }

    /// 返回名字字符串的引用
    ///
    /// # 示例
    ///
    /// ```rust
    /// use xwrust::xwos::thd;
    /// use libc_print::std_name::println;
    ///
    /// let handler = thd::Builder::new()
    ///     .name("foo".into())
    ///     .spawn(|ele| {
    ///         println!("Thread name: {}", ele.name().unwrap());
    ///     }).unwrap();
    ///
    /// handler.join().unwrap();
    /// ```
    pub fn name(&self) -> Option<&str> {
        self.cname().map(|s| unsafe { str::from_utf8_unchecked(s.to_bytes()) })
    }

    fn cname(&self) -> Option<&CStr> {
        self.name.as_deref()
    }
}

/// 线程的返回值
///
/// 线程的返回值中的数据需跨线程共享，因此在定义时需要使用 [`Arc`] 封装
///
/// [`Arc`]: https://doc.rust-lang.org/alloc/sync/struct.Arc.html
pub type Result<R> = core::result::Result<R, Box<dyn Any + Send + 'static>>;

struct ThdReturnValue<R> {
    result: UnsafeCell<Option<Result<R>>>,
}

unsafe impl<R: Sync> Sync for ThdReturnValue<R> {}

struct ThdHandleInner<R> {
    thd: Thd,
    element: Arc<ThdElement>,
    rv: Arc<ThdReturnValue<R>>,
}

impl<R> ThdHandleInner<R> {
    fn join(mut self) -> Result<R> {
        self.thd.join();
        Arc::get_mut(&mut self.rv).unwrap().result.get_mut().take().unwrap()
    }
}

/// 线程的句柄
pub struct ThdHandle<R>(ThdHandleInner<R>);

unsafe impl<R> Send for ThdHandle<R> {}
unsafe impl<R> Sync for ThdHandle<R> {}

impl<R> ThdHandle<R> {
    /// 返回XWOS的线程对象的引用，用于与C语言交互
    pub fn thd(&self) -> &Thd {
        &self.0.thd
    }

    /// 返回线程的元素
    pub fn element(&self) -> &ThdElement {
        &self.0.element
    }

    /// 等待线程运行至退出，并返回线程的返回值
    ///
    /// 如果线程已经运行完成，此方法立即返回。
    ///
    /// # 示例
    ///
    /// ```rust
    /// use xwrust::xwos::thd;
    /// use libc_print::std_name::println;
    ///
    /// let builder = thd::Builder::new();
    /// let handler = builder.spawn(|_| {
    ///     8 // 返回值
    /// }).unwrap();
    ///
    /// let rc = handler.join().unwrap(); // 等待线程结束，并获取其返回值
    /// println!("Thread return: {}", rc);
    /// ```
    pub fn join(self) -> Result<R> {
        self.0.join()
    }

    /// 检查关联的线程是否运行结束
    ///
    /// 此方法不会阻塞调用者。
    pub fn is_finished(&self) -> bool {
        Arc::strong_count(&self.0.rv) == 1
    }
}
