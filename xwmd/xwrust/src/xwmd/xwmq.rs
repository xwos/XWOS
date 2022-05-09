//! XWOS RUST：消息队列
//! ========
//!
//!

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
    NoData,
    /// 未知错误
    Unknown(XwEr),
}

/// XWOS消息队列对象占用的内存大小
pub const SIZEOF_XWMQ: usize = 200;
pub const SIZEOF_XWMQ_MSG: usize = 16;

xwos_struct! {
    /// 用于构建消息队列的内存数组类型
    pub struct XwmdXwmq {
        #[doc(hidden)]
        mem: [u8; SIZEOF_XWMQ],
    }
}

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
    pub const fn new() -> Self {
        Self {
            mq: UnsafeCell::new(XWMQ_INITIALIZER),
            pool: UnsafeCell::new(XwmdXwmqMsg { mem: [0; N * SIZEOF_XWMQ_MSG] }),
            tik: UnsafeCell::new(0),
            phantom: PhantomData,
        }
    }

    pub fn init(&self) -> (Sender<N, T>, Receiver<N, T>) {
        unsafe {
            xwmq_init(self.mq.get(), self.pool.get() as _, N);
            *self.tik.get() = xwmq_gettik(self.mq.get());
        }
        (Sender {xwmq: &self}, Receiver {xwmq: &self}, )
    }
}

#[derive(Clone)]
pub struct Sender<'a, const N: XwSz, T>
where
    [u8; N * SIZEOF_XWMQ_MSG]: Sized
{
    xwmq: &'a Xwmq<N, T>,
}

pub struct Receiver<'a, const N: XwSz, T>
where
    [u8; N * SIZEOF_XWMQ_MSG]: Sized
{
    xwmq: &'a Xwmq<N, T>,
}

unsafe impl<'a, const N: XwSz, T> Send for Sender<'a, N, T>
where
    [u8; N * SIZEOF_XWMQ_MSG]: Sized {}

impl<'a, const N: XwSz, T> !Sync for Sender<'a, N, T>
where
    [u8; N * SIZEOF_XWMQ_MSG]: Sized {}

unsafe impl<'a, const N: XwSz, T> Send for Receiver<'a, N, T>
where
    [u8; N * SIZEOF_XWMQ_MSG]: Sized {}

impl<'a, const N: XwSz, T> !Sync for Receiver<'a, N, T>
where
    [u8; N * SIZEOF_XWMQ_MSG]: Sized {}

impl<'a, const N: XwSz, T> Sender<'a, N, T>
where
    [u8; N * SIZEOF_XWMQ_MSG]: Sized
{
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

impl<'a, const N: XwSz, T> Receiver<'a, N, T>
where
    [u8; N * SIZEOF_XWMQ_MSG]: Sized
{
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
                    Err(XwmqError::NoSlot)
                } else {
                    Err(XwmqError::Unknown(rc))
                }
            } else {
                Err(XwmqError::NotInit)
            }
        }
    }
}
