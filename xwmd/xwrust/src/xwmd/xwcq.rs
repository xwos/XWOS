//! XWOS RUST：循环队列
//! ========
//!
//! XWOS RUST的循环队列是不依赖任何动态内存申请与释放而实现的多线程间的通讯机制。
//!
//! 循环队列有 `N` 个数据槽，每个数据槽的大小固定为 `S` 。
//! 在创建循环队列时，需要指明类型常数 `S` 和 `N`，其内存完全由编译器进行分配。
//!
//! 发送时，可以将数据发送到队列后面（入队），也可以将数据发送到队列前面（插队）。
//! 如果循环队列已满，不会阻塞发送者，循环队列会丢弃一份数据，腾出数据槽，再将新数据放入队列。
//!
//! 发送时，数据类型是泛型 `T` ，`T` 必须是 [`Sized`] 的，数据是被逐字节地被拷贝到循环队列的数据槽内的。
//! 当发送的数据类型大小大于循环队列的数据槽大小 `S` ，发送会失败。
//!
//!
//! 接收时，可以从循环队列的前面取出数据（离队），也可以从循环队列的后面取出数据（反向离队）。
//! 还可以从循环队列的前面拷贝数据（前向偷窥），以及从循环队列的后面拷贝数据（后向偷窥）。
//!
//! 接收时，数据类型是泛型 `T` ， `T` 必须是 [`Sized`] 的，数据是被逐字节地从循环队列的数据槽中拷贝到出来的。
//! 循环队列不会对接收时的 `T` 与 发送时的 `T` 进行检查，因此用户必须确保两个类型是一样的，或能安全地进行转换。
//!
//!
//! 循环队列的 **入队** 与 **离队** ，**插队** 与 **反向离队** 分别组成了 **先进先出(FIFO)** 的关系。
//!
//!
//! 由于不使用动态内存申请，又无法预测各个线程的生命周期，因此循环队列只能被定义为 [`static`] 限定的全局变量。
//!
//!
//! 与 [`std::sync::mpsc`] 不同，循环队列不区分发送者与接收者，任何线程都可以发送与接收。
//!
//!
//! # 创建
//!
//! XWOS RUST的循环队列可使用 [`Xwcq::new()`] 创建。创建时需要指定两个常量参数：数据槽的大小 `S` 以及数量 `N` 。
//! 循环队列只能创建为具有静态生命周期 [`static`] 约束的全局变量：
//!
//! ```rust
//! use xwrust::xwmd::xwcq::*;
//!
//! static CQ: Xwcq<64, 16> = Xwcq::new(); // 数据槽大小 `S` = 64; 数量 `N` = 16
//! ```
//!
//!
//! # 初始化
//!
//! 使用前，必须调用 [`Xwcq::init()`] 对循环队列进行初始化。如果创建循环队列时的常量参数不合适，初始化将以错误码返回。
//!
//! ```rust
//! pub fn xwrust_example_xwcq() {
//!     let rc = CQ.init();
//! }
//! ```
//!
//!
//! # 发送
//!
//! 发送不会阻塞发送者，因此可在任意上下文进行发送。
//!
//! ### 入队
//!
//! **入队 (EnQueue)** [`Xwcq::eq`] 是指将数据发送到循环队列最后面。
//!
//! ### 插队
//!
//! **插队 (Jump the Queue)** [`Xwcq::jq`] 是指将数据发送到循环队列最前面。
//!
//!
//! # 接收
//!
//! ### 离队
//!
//! 离队是指从循环队列的最前面取出数据。取出数据后，数据槽被释放。
//!
//! + [`Xwcq::dq()`] 若循环队列中为空，就无限等待，直到循环队列中有数据可取出或被中断。
//! + [`Xwcq::dq_to()`] 若循环队列中为空，就限时等待，直到循环队列中有数据可取出或被中断或等待超时。
//! + [`Xwcq::trydq()`] 若循环队列中为空，就立即返回，可在中断中使用。
//!
//! ### 反向离队
//!
//! 反向离队是指从循环队列的最后面取出数据。取出数据后，数据槽被释放。
//!
//! + [`Xwcq::rq()`] 若循环队列中为空，就无限等待，直到循环队列中有数据可取出或被中断。
//! + [`Xwcq::rq_to()`] 若循环队列中为空，就限时等待，直到循环队列中有数据可取出或被中断或等待超时。
//! + [`Xwcq::tryrq()`] 若循环队列中为空，就立即返回，可在中断中使用。
//!
//! ### 从循环队列前面拷贝数据
//!
//! 从循环队列前面拷贝数据时，数据不会取走数据，数据槽不会被释放。因此命名为 **前向偷窥 (Peek at the Front of Queue)** 。
//!
//! + [`Xwcq::pfq()`] 若循环队列中为空，就无限等待，直到循环队列中有数据可取出或被中断。
//! + [`Xwcq::pfq_to()`] 若循环队列中为空，就限时等待，直到循环队列中有数据可取出或被中断或等待超时。
//! + [`Xwcq::trypfq()`] 若循环队列中为空，就立即返回，可在中断中使用。
//!
//! ### 从循环队列后面拷贝数据
//!
//! 从循环队列后面拷贝数据，数据不会取走数据，数据槽不会被释放。因此命名为 **后向偷窥 (Peek at the Rear of Queue)** 。
//!
//! + [`Xwcq::prq()`] 若循环队列中为空，就无限等待，直到循环队列中有数据可取出或被中断。
//! + [`Xwcq::prq_to()`] 若循环队列中为空，就限时等待，直到循环队列中有数据可取出或被中断或等待超时。
//! + [`Xwcq::tryprq()`] 若循环队列中为空，就立即返回，可在中断中使用。
//!
//!
//! # 清空循环队列
//!
//! [`Xwcq::flush()`] 可将循环队列的所有数据槽清空。
//!
//!
//! # 获取循环队列容量
//!
//! [`Xwcq::capacity()`] 可获取循环队列的数据槽的数量 `N` 。
//!
//!
//! # 获取循环队列数据槽的大小
//!
//! [`Xwcq::size()`] 可获取循环队列数据槽的大小 `S` ，若 `S` 不满足内存对齐的要求，会自动向上调整。
//!
//!
//! # 获取循环队列中可被接收的数据的数量
//!
//! [`Xwcq::availability()`] 可获取循环队列可被接收的数据的数量。
//!
//!
//! # 示例
//!
//! [XWOS/xwam/xwrust-example/xwrust_example_xwcq](https://gitee.com/xwos/XWOS/blob/main/xwam/xwrust-example/xwrust_example_xwcq/src/lib.rs)
//!
//! [`Sized`]: <https://doc.rust-lang.org/std/marker/trait.Sized.html>
//! [`static`]: <https://doc.rust-lang.org/std/keyword.static.html>
//! [`std::sync::mpsc`]: <https://doc.rust-lang.org/std/sync/mpsc/index.html>


extern crate core;
use core::ffi::*;
use core::result::Result;
use core::cell::UnsafeCell;
use core::mem;

use crate::cfg::*;
use crate::types::*;
use crate::errno::*;


extern "C" {
    fn xwcq_init(cq: *mut XwmdXwcq,
                 slotsize: XwSz, slotnum: XwSz,
                 txq: *mut c_void) -> XwEr;
    fn xwcq_fini(cq: *mut XwmdXwcq) -> XwEr;
    fn xwcq_grab(cq: *mut XwmdXwcq) -> XwEr;
    fn xwcq_put(cq: *mut XwmdXwcq) -> XwEr;
    fn xwcq_gettik(cq: *mut XwmdXwcq) -> XwSq;
    fn xwcq_acquire(cq: *mut XwmdXwcq, tik: XwSq) -> XwEr;
    fn xwcq_release(cq: *mut XwmdXwcq, tik: XwSq) -> XwEr;
    fn xwcq_eq(cq: *mut XwmdXwcq, data: *const u8, size: *mut XwSz) -> XwEr;
    fn xwcq_jq(cq: *mut XwmdXwcq, data: *const u8, size: *mut XwSz) -> XwEr;
    fn xwcq_dq(cq: *mut XwmdXwcq, data: *mut u8, size: *mut XwSz) -> XwEr;
    fn xwcq_dq_to(cq: *mut XwmdXwcq, data: *mut u8, size: *mut XwSz, to: XwTm) -> XwEr;
    fn xwcq_trydq(cq: *mut XwmdXwcq, data: *mut u8, size: *mut XwSz) -> XwEr;
    fn xwcq_rq(cq: *mut XwmdXwcq, data: *mut u8, size: *mut XwSz) -> XwEr;
    fn xwcq_rq_to(cq: *mut XwmdXwcq, data: *mut u8, size: *mut XwSz, to: XwTm) -> XwEr;
    fn xwcq_tryrq(cq: *mut XwmdXwcq, data: *mut u8, size: *mut XwSz) -> XwEr;
    fn xwcq_pfq(cq: *mut XwmdXwcq, data: *mut u8, size: *mut XwSz) -> XwEr;
    fn xwcq_pfq_to(cq: *mut XwmdXwcq, data: *mut u8, size: *mut XwSz, to: XwTm) -> XwEr;
    fn xwcq_trypfq(cq: *mut XwmdXwcq, data: *mut u8, size: *mut XwSz) -> XwEr;
    fn xwcq_prq(cq: *mut XwmdXwcq, data: *mut u8, size: *mut XwSz) -> XwEr;
    fn xwcq_prq_to(cq: *mut XwmdXwcq, data: *mut u8, size: *mut XwSz, to: XwTm) -> XwEr;
    fn xwcq_tryprq(cq: *mut XwmdXwcq, data: *mut u8, size: *mut XwSz) -> XwEr;
    fn xwcq_flush(cq: *mut XwmdXwcq) -> XwEr;
    fn xwcq_get_capacity(cq: *mut XwmdXwcq, capbuf: *mut XwSz) -> XwEr;
    fn xwcq_get_size(cq: *mut XwmdXwcq, szbuf: *mut XwSz) -> XwEr;
    fn xwcq_get_availability(cq: *mut XwmdXwcq, avbbuf: *mut XwSz) -> XwEr;
    fn xwcq_bind(cq: *mut XwmdXwcq, sel: *mut c_void, pos: XwSq) -> XwEr;
    fn xwcq_unbind(cq: *mut XwmdXwcq, sel: *mut c_void) -> XwEr;
}

/// 循环队列的错误码
#[derive(Debug)]
pub enum XwcqError {
    /// 没有错误
    Ok,
    /// 循环队列没有初始化
    NotInit,
    /// 循环队列内存大小错误
    PoolSize,
    /// 数据大小错误
    DataSize,
    /// 等待被中断
    Interrupt,
    /// 等待超时
    Timedout,
    /// 不在线程上下文内
    NotThreadContext,
    /// 抢占被关闭
    CannotPmpt,
    /// 中断底半部被关闭
    CannotBh,
    /// 循环队列中没有数据
    NoData,
    /// 未知错误
    Unknown(XwEr),
}

const XWCQ_MM_ALIGN: XwSz = XWMMCFG_ALIGNMENT;
const XWCQ_MM_MSK: XwSz = XWCQ_MM_ALIGN - 1;

/// 循环队列对象占用的内存大小
#[cfg(target_pointer_width = "32")]
pub const SIZEOF_XWCQ: usize = 96;

/// 循环队列对象占用的内存大小
#[cfg(target_pointer_width = "64")]
pub const SIZEOF_XWCQ: usize = 192;

/// 用于构建循环队列的内存数组类型
#[repr(C)]
#[cfg_attr(target_pointer_width = "32", repr(align(8)))]
#[cfg_attr(target_pointer_width = "64", repr(align(16)))]
pub(crate) struct XwmdXwcq {
    pub(crate) obj: [u8; SIZEOF_XWCQ],
}

/// 用于构建队列数据的内存池数组类型
#[repr(C)]
#[cfg_attr(target_pointer_width = "32", repr(align(8)))]
#[cfg_attr(target_pointer_width = "64", repr(align(16)))]
pub(crate) struct XwmdXwcqPool<const S: XwSz, const N: XwSz>
where
    [u8; ((S + XWCQ_MM_ALIGN - 1) & (!XWCQ_MM_MSK)) * N]: Sized
{
    pub(crate) mem: [u8; ((S + XWCQ_MM_ALIGN - 1) & (!XWCQ_MM_MSK)) * N],
}

/// 循环队列对象结构体
pub struct Xwcq<const S: XwSz, const N: XwSz>
where
    [u8; ((S + XWCQ_MM_ALIGN - 1) & (!XWCQ_MM_MSK)) * N]: Sized
{
    /// 用于初始化循环队列对象的内存空间
    pub(crate) cq: UnsafeCell<XwmdXwcq>,
    /// 用于初始化数据池的内存空间
    pub(crate) pool: UnsafeCell<XwmdXwcqPool<S, N>>,
    /// 循环队列对象的标签
    pub(crate) tik: UnsafeCell<XwSq>,
}

impl<const S: XwSz, const N: XwSz> !Send for Xwcq<S, N>
where
    [u8; ((S + XWCQ_MM_ALIGN - 1) & (!XWCQ_MM_MSK)) * N]: Sized
{}

unsafe impl<const S: XwSz, const N: XwSz> Sync for Xwcq<S, N>
where
    [u8; ((S + XWCQ_MM_ALIGN - 1) & (!XWCQ_MM_MSK)) * N]: Sized
{}

impl<const S: XwSz, const N: XwSz> Xwcq<S, N>
where
    [u8; ((S + XWCQ_MM_ALIGN - 1) & (!XWCQ_MM_MSK)) * N]: Sized
{
    /// 新建循环队列对象。
    ///
    /// 新建时需要指定类型常量参数。
    ///
    /// + `S` 是数据槽的大小，不能为0，若 `S` 不满足内存对齐的要求，会自动向上调整。
    /// + `N` 是数据槽的数量，不能为0。
    ///
    /// 此方法是编译期方法。循环队列只可被定义为具有 [`'static`] 约束的全局变量全局变量：
    ///
    /// ```rust
    /// use xwrust::xwmd::xwcq::*;
    ///
    /// static CQ: Xwcq<64, 8> = Xwcq::new(); // 64字节 * 8个槽
    /// ```
    ///
    /// [`'static`]: https://doc.rust-lang.org/std/keyword.static.html
    pub const fn new() -> Self {
        Self {
            cq: UnsafeCell::new(XwmdXwcq { obj: [0; SIZEOF_XWCQ] }),
            pool: UnsafeCell::new(XwmdXwcqPool {
                mem: [0; ((S + XWCQ_MM_ALIGN - 1) & (!XWCQ_MM_MSK)) * N]
            }),
            tik: UnsafeCell::new(0),
        }
    }

    /// 初始化循环队列对象。
    ///
    /// 循环队列对象必须调用此方法一次，方可正常使用。
    ///
    /// 如果在创建循环队列时，指定的 `S * N == 0` ，初始化时将返回错误 [`XwcqError::PoolSize`] 。
    ///
    /// # 错误码
    ///
    /// + [`XwcqError::Ok`] 没有错误
    /// + [`XwcqError::PoolSize`] 循环队列内存大小错误
    ///
    /// # 上下文
    ///
    /// + 任意
    ///
    /// # 示例
    ///
    /// ```rust
    /// use xwrust::xwmd::xwcq::*;
    ///
    /// static CQ: Xwcq<64, 8> = Xwcq::new(); // 64字节 * 8个槽
    ///
    /// pub fn xwrust_example_xwcq() {
    ///     let rc = CQ.init();
    /// }
    /// ```
    pub fn init(&'static self) -> XwcqError {
        unsafe {
            let slotsize: XwSz = (S + XWCQ_MM_ALIGN - 1) & (!XWCQ_MM_MSK);
            let rc = xwcq_init(self.cq.get(), slotsize, N, self.pool.get() as _);
            *self.tik.get() = xwcq_gettik(self.cq.get());
            if rc == 0 {
                XwcqError::Ok
            } else if rc == -ESIZE {
                XwcqError::PoolSize
            } else {
                XwcqError::Unknown(rc)
            }
        }
    }

    /// 将数据放入循环队列的后面。（入队 EnQueue）
    ///
    /// + 如果队列数据已满，不会阻塞发送者，循环队列会将最前面（最旧的）的数据丢弃，腾出空间将新数据放入队列。
    /// + 发送时，数据类型是泛型 `T` ，`T` 必须是 [`Sized`] 的，数据是被逐字节地被拷贝到循环队列的数据槽内的。
    /// + 当发送的数据类型大小大于循环队列的数据槽大小 `S` ，发送会失败，并返回错误码 [`XwcqError::DataSize`] 。
    ///
    /// # 错误码
    ///
    /// + [`XwcqError::Ok`] 没有错误
    /// + [`XwcqError::NotInit`] 循环队列没有初始化
    /// + [`XwcqError::DataSize`] 数据大小错误
    ///
    /// # 上下文
    ///
    /// + 任意
    ///
    /// # 示例
    ///
    /// ```rust
    /// use xwrust::xwos::thd::SThd;
    /// use xwrust::xwmd::xwcq::*;
    ///
    /// static CQ: Xwcq<64, 16> = Xwcq::new();
    /// static CHILD: SThd<1024, &str> = SThd::new("CHILD0", true);
    ///
    /// pub fn xwrust_example_xwcq() {
    ///     CQ.init();
    ///     CHILD.run(|_| { // 子线程0
    ///             // 将utf-8字符串放入 `[u8; 64]` 数组中后发送
    ///             let mut buffer: [u8; 64] = [0; 64];
    ///             let msg = "子线程数据".as_bytes();
    ///             let msgsize = msg.len();
    ///             buffer[..msgsize].copy_from_slice(msg);
    ///             let rc = CQ.eq(buffer);
    ///             match rc {
    ///                 Ok(sz) => { // 发送数据成功
    ///                 },
    ///                 Err(e) => { // 发送数据失败
    ///                 },
    ///             };
    ///             "OK"
    ///         });
    /// }
    /// ```
    ///
    /// [`Sized`]: <https://doc.rust-lang.org/std/marker/trait.Sized.html>
    pub fn eq<T>(&self, sdu: T) -> Result<XwSz, XwcqError>
    where
        T: Sized + Send
    {
        unsafe {
            let mut sdusz: XwSz = mem::size_of::<T>();
            if sdusz <= self.size() {
                let mut rc = xwcq_acquire(self.cq.get(), *self.tik.get());
                if rc == 0 {
                    let refsdu = &sdu as *const T;
                    rc = xwcq_eq(self.cq.get(), refsdu as *const u8, &mut sdusz as _);
                    xwcq_put(self.cq.get());
                    if XWOK == rc {
                        Ok(sdusz)
                    } else {
                        Err(XwcqError::Unknown(rc))
                    }
                } else {
                    Err(XwcqError::NotInit)
                }
            } else {
                Err(XwcqError::DataSize)
            }
        }
    }

    /// 将数据放入循环队列的前面。（插队 Jump the Queue）
    ///
    /// + 如果队列数据已满，不会阻塞发送者，循环队列会将最后面（最新的）的数据丢弃，腾出空间将新数据放入队列。
    /// + 发送时，数据类型是泛型 `T` ，`T` 必须是 [`Sized`] 的，数据是被逐字节地被拷贝到循环队列的数据槽内的。
    /// + 当发送的数据类型大小大于循环队列的数据槽大小 `S` ，发送会失败，并返回错误码 [`XwcqError::DataSize`] 。
    ///
    /// # 错误码
    ///
    /// + [`XwcqError::Ok`] 没有错误
    /// + [`XwcqError::NotInit`] 循环队列没有初始化
    /// + [`XwcqError::DataSize`] 数据大小错误
    ///
    /// # 上下文
    ///
    /// + 任意
    ///
    /// # 示例
    ///
    /// ```rust
    /// use xwrust::xwos::thd::SThd;
    /// use xwrust::xwmd::xwcq::*;
    ///
    /// static CQ: Xwcq<64, 16> = Xwcq::new();
    /// static CHILD: SThd<1024, &str> = SThd::new("CHILD0", true);
    ///
    /// pub fn xwrust_example_xwcq() {
    ///     CQ.init();
    ///     CHILD.run(|_| { // 子线程0
    ///             // 将utf-8字符串放入 `[u8; 64]` 数组中后发送
    ///             let mut buffer: [u8; 64] = [0; 64];
    ///             let msg = "子线程数据".as_bytes();
    ///             let msgsize = msg.len();
    ///             buffer[..msgsize].copy_from_slice(msg);
    ///             let rc = CQ.jq(buffer);
    ///             match rc {
    ///                 Ok(sz) => { // 发送数据成功
    ///                 },
    ///                 Err(e) => { // 发送数据失败
    ///                 },
    ///             };
    ///             "OK"
    ///         });
    /// }
    /// ```
    ///
    /// [`Sized`]: <https://doc.rust-lang.org/std/marker/trait.Sized.html>
    pub fn jq<T>(&self, sdu: T) -> Result<XwSz, XwcqError>
    where
        T: Sized + Send
    {
        unsafe {
            let mut sdusz: XwSz = mem::size_of::<T>();
            if sdusz <= self.size() {
                let mut rc = xwcq_acquire(self.cq.get(), *self.tik.get());
                if rc == 0 {
                    let refsdu = &sdu as *const T;
                    rc = xwcq_jq(self.cq.get(), refsdu as *const u8, &mut sdusz as _);
                    xwcq_put(self.cq.get());
                    if XWOK == rc {
                        Ok(sdusz)
                    } else {
                        Err(XwcqError::Unknown(rc))
                    }
                } else {
                    Err(XwcqError::NotInit)
                }
            } else {
                Err(XwcqError::DataSize)
            }
        }
    }

    /// 等待从循环队列的前面取出一份数据。（离队 DeQueue）
    ///
    /// + 当循环队列未初始化，返回错误码 [`XwcqError::NotInit`] 。
    /// + 若循环队列为空，线程会阻塞等待。
    /// + 接收时，数据类型是泛型 `T` ，`T` 必须是 [`Sized`] 的，数据是被逐字节地从循环队列的数据槽中拷贝到出来的。
    /// + 循环队列不会对接收时的 `T` 与 发送时的 `T` 进行检查，因此用户必须确保两个类型是一样的，或能安全地进行转换。
    /// + 当 `T` 大小大于循环队列的数据槽大小 `S` ，接收会失败，并返回错误码 [`XwcqError::DataSize`] 。
    /// + 当线程阻塞等待被中断时，返回错误码 [`XwcqError::Interrupt`] 。
    ///
    /// # 上下文
    ///
    /// + 线程
    ///
    /// # 示例
    ///
    /// ```rust
    /// extern crate core;
    /// use core::str;
    /// use core::slice::memchr::memchr;
    ///
    /// use xwrust::xwmd::xwcq::*;
    ///
    /// static CQ: Xwcq<64, 16> = Xwcq::new();
    ///
    /// pub fn xwrust_example_xwcq() {
    ///     CQ.init();
    ///     // ...省略...
    ///     let rc = CQ.dq::<[u8; 64]>();
    ///     match rc {
    ///         Ok(d) => { // 获取数据成功
    ///             // 从 `[u8; 64]` 数组中构建切片 `&str`
    ///             let totalslice = &d[0..64];
    ///             let nullpos = memchr(0, totalslice).unwrap_or(64);
    ///             let msgu8slice = &d[0..nullpos];
    ///             let msg = str::from_utf8(msgu8slice);
    ///         },
    ///         Err(e) => { // 获取数据失败
    ///             break;
    ///         },
    ///     };
    /// }
    /// ```
    ///
    /// [`Sized`]: <https://doc.rust-lang.org/std/marker/trait.Sized.html>
    pub fn dq<T>(&self) -> Result<T, XwcqError>
    where
        T: Sized + Send
    {
        unsafe {
            let mut sdusz: XwSz = mem::size_of::<T>();
            if sdusz <= self.size() {
                let mut rc = xwcq_acquire(self.cq.get(), *self.tik.get());
                if rc == 0 {
                    let mut sdu: T = mem::zeroed();
                    rc = xwcq_dq(self.cq.get(),
                                 &mut sdu as *mut T as *mut u8, &mut sdusz as _);
                    xwcq_put(self.cq.get());
                    if XWOK == rc {
                        Ok(sdu)
                    } else if  -EINTR == rc {
                        Err(XwcqError::Interrupt)
                    } else if -ENOTTHDCTX == rc {
                        Err(XwcqError::NotThreadContext)
                    } else if -ECANNOTPMPT == rc {
                        Err(XwcqError::CannotPmpt)
                    } else if -ECANNOTBH == rc {
                        Err(XwcqError::CannotBh)
                    } else {
                        Err(XwcqError::Unknown(rc))
                    }
                } else {
                    Err(XwcqError::NotInit)
                }
            } else {
                Err(XwcqError::DataSize)
            }
        }
    }

    /// 限时等待从循环队列的前面取出一份数据。（离队 DeQueue）
    ///
    /// + 当循环队列未初始化，返回错误码 [`XwcqError::NotInit`] 。
    /// + 若循环队列为空，线程会阻塞等待，等待时会指定一个唤醒时间点。
    /// + 接收时，数据类型是泛型 `T` ，`T` 必须是 [`Sized`] 的，数据是被逐字节地从循环队列的数据槽中拷贝到出来的。
    /// + 循环队列不会对接收时的 `T` 与 发送时的 `T` 进行检查，因此用户必须确保两个类型是一样的，或能安全地进行转换。
    /// + 当 `T` 大小大于循环队列的数据槽大小 `S` ，接收会失败，并返回错误码 [`XwcqError::DataSize`] 。
    /// + 当线程阻塞等待被中断时，返回错误码 [`XwcqError::Interrupt`] 。
    /// + 当到达指定的唤醒时间点，线程被唤醒，返回错误码 [`XwcqError::Timedout`] 。
    ///
    /// # 上下文
    ///
    /// + 线程
    ///
    /// # 示例
    ///
    /// ```rust
    /// extern crate core;
    /// use core::str;
    /// use core::slice::memchr::memchr;
    ///
    /// use xwrust::xwtm;
    /// use xwrust::xwmd::xwcq::*;
    ///
    /// static CQ: Xwcq<64, 16> = Xwcq::new();
    ///
    /// pub fn xwrust_example_xwcq() {
    ///     CQ.init();
    ///     // ...省略...
    ///     let rc = CQ.dq_to::<[u8; 64]>(xwtm::ft(xwtm::s(1)));
    ///     match rc {
    ///         Ok(d) => { // 获取数据成功
    ///             // 从 `[u8; 64]` 数组中构建切片 `&str`
    ///             let totalslice = &d[0..64];
    ///             let nullpos = memchr(0, totalslice).unwrap_or(64);
    ///             let msgu8slice = &d[0..nullpos];
    ///             let msg = str::from_utf8(msgu8slice);
    ///         },
    ///         Err(e) => { // 获取数据失败
    ///             break;
    ///         },
    ///     };
    /// }
    /// ```
    ///
    /// [`Sized`]: <https://doc.rust-lang.org/std/marker/trait.Sized.html>
    pub fn dq_to<T>(&self, to: XwTm) -> Result<T, XwcqError>
    where
        T: Sized + Send
    {
        unsafe {
            let mut sdusz: XwSz = mem::size_of::<T>();
            if sdusz <= self.size() {
                let mut rc = xwcq_acquire(self.cq.get(), *self.tik.get());
                if rc == 0 {
                    let mut sdu: T = mem::zeroed();
                    rc = xwcq_dq_to(self.cq.get(),
                                    &mut sdu as *mut T as *mut u8, &mut sdusz as _,
                                    to);
                    xwcq_put(self.cq.get());
                    if XWOK == rc {
                        Ok(sdu)
                    } else if  -EINTR == rc {
                        Err(XwcqError::Interrupt)
                    } else if -ETIMEDOUT == rc {
                        Err(XwcqError::Timedout)
                    } else if -ENOTTHDCTX == rc {
                        Err(XwcqError::NotThreadContext)
                    } else if -ECANNOTPMPT == rc {
                        Err(XwcqError::CannotPmpt)
                    } else if -ECANNOTBH == rc {
                        Err(XwcqError::CannotBh)
                    } else {
                        Err(XwcqError::Unknown(rc))
                    }
                } else {
                    Err(XwcqError::NotInit)
                }
            } else {
                Err(XwcqError::DataSize)
            }
        }
    }

    /// 尝试从循环队列的前面取出一份数据。（离队 DeQueue）
    ///
    /// + 当循环队列未初始化，返回错误码 [`XwcqError::NotInit`] 。
    /// + 若循环队列为空，立即返回 [`XwcqError::DataSize`] ，此方法不会阻塞。
    /// + 接收时，数据类型是泛型 `T` ，`T` 必须是 [`Sized`] 的，数据是被逐字节地从循环队列的数据槽中拷贝到出来的。
    /// + 循环队列不会对接收时的 `T` 与 发送时的 `T` 进行检查，因此用户必须确保两个类型是一样的，或能安全地进行转换。
    /// + 当 `T` 大小大于循环队列的数据槽大小 `S` ，接收会失败，并返回错误码 [`XwcqError::DataSize`] 。
    ///
    /// # 上下文
    ///
    /// + 任意
    ///
    /// # 示例
    ///
    /// ```rust
    /// extern crate core;
    /// use core::str;
    /// use core::slice::memchr::memchr;
    ///
    /// use xwrust::xwmd::xwcq::*;
    ///
    /// static CQ: Xwcq<64, 16> = Xwcq::new();
    ///
    /// pub fn xwrust_example_xwcq() {
    ///     CQ.init();
    ///     // ...省略...
    ///     let rc = CQ.trydq::<[u8; 64]>();
    ///     match rc {
    ///         Ok(d) => { // 获取数据成功
    ///             // 从 `[u8; 64]` 数组中构建切片 `&str`
    ///             let totalslice = &d[0..64];
    ///             let nullpos = memchr(0, totalslice).unwrap_or(64);
    ///             let msgu8slice = &d[0..nullpos];
    ///             let msg = str::from_utf8(msgu8slice);
    ///         },
    ///         Err(e) => { // 获取数据失败
    ///         },
    ///     };
    /// }
    /// ```
    ///
    /// [`Sized`]: <https://doc.rust-lang.org/std/marker/trait.Sized.html>
    pub fn trydq<T>(&self) -> Result<T, XwcqError>
    where
        T: Sized + Send
    {
        unsafe {
            let mut sdusz: XwSz = mem::size_of::<T>();
            if sdusz <= self.size() {
                let mut rc = xwcq_acquire(self.cq.get(), *self.tik.get());
                if rc == 0 {
                    let mut sdu: T = mem::zeroed();
                    rc = xwcq_trydq(self.cq.get(),
                                    &mut sdu as *mut T as *mut u8, &mut sdusz as _);
                    xwcq_put(self.cq.get());
                    if XWOK == rc {
                        Ok(sdu)
                    } else if -ENODATA == rc {
                        Err(XwcqError::NoData)
                    } else {
                        Err(XwcqError::Unknown(rc))
                    }
                } else {
                    Err(XwcqError::NotInit)
                }
            } else {
                Err(XwcqError::DataSize)
            }
        }
    }

    /// 等待从循环队列的后面取出一份数据。（反向离队 Reversely deQueue）
    ///
    /// + 当循环队列未初始化，返回错误码 [`XwcqError::NotInit`] 。
    /// + 若循环队列为空，线程会阻塞等待。
    /// + 接收时，数据类型是泛型 `T` ，`T` 必须是 [`Sized`] 的，数据是被逐字节地从循环队列的数据槽中拷贝到出来的。
    /// + 循环队列不会对接收时的 `T` 与 发送时的 `T` 进行检查，因此用户必须确保两个类型是一样的，或能安全地进行转换。
    /// + 当 `T` 大小大于循环队列的数据槽大小 `S` ，接收会失败，并返回错误码 [`XwcqError::DataSize`] 。
    /// + 当线程阻塞等待被中断时，返回错误码 [`XwcqError::Interrupt`] 。
    ///
    /// # 上下文
    ///
    /// + 线程
    ///
    /// # 示例
    ///
    /// ```rust
    /// extern crate core;
    /// use core::str;
    /// use core::slice::memchr::memchr;
    ///
    /// use xwrust::xwmd::xwcq::*;
    ///
    /// static CQ: Xwcq<64, 16> = Xwcq::new();
    ///
    /// pub fn xwrust_example_xwcq() {
    ///     CQ.init();
    ///     // ...省略...
    ///     let rc = CQ.rq::<[u8; 64]>();
    ///     match rc {
    ///         Ok(d) => { // 获取数据成功
    ///             // 从 `[u8; 64]` 数组中构建切片 `&str`
    ///             let totalslice = &d[0..64];
    ///             let nullpos = memchr(0, totalslice).unwrap_or(64);
    ///             let msgu8slice = &d[0..nullpos];
    ///             let msg = str::from_utf8(msgu8slice);
    ///         },
    ///         Err(e) => { // 获取数据失败
    ///             break;
    ///         },
    ///     };
    /// }
    /// ```
    ///
    /// [`Sized`]: <https://doc.rust-lang.org/std/marker/trait.Sized.html>
    pub fn rq<T>(&self) -> Result<T, XwcqError>
    where
        T: Sized + Send
    {
        unsafe {
            let mut sdusz: XwSz = mem::size_of::<T>();
            if sdusz <= self.size() {
                let mut rc = xwcq_acquire(self.cq.get(), *self.tik.get());
                if rc == 0 {
                    let mut sdu: T = mem::zeroed();
                    rc = xwcq_rq(self.cq.get(),
                                 &mut sdu as *mut T as *mut u8, &mut sdusz as _);
                    xwcq_put(self.cq.get());
                    if XWOK == rc {
                        Ok(sdu)
                    } else if  -EINTR == rc {
                        Err(XwcqError::Interrupt)
                    } else if -ENOTTHDCTX == rc {
                        Err(XwcqError::NotThreadContext)
                    } else if -ECANNOTPMPT == rc {
                        Err(XwcqError::CannotPmpt)
                    } else if -ECANNOTBH == rc {
                        Err(XwcqError::CannotBh)
                    } else {
                        Err(XwcqError::Unknown(rc))
                    }
                } else {
                    Err(XwcqError::NotInit)
                }
            } else {
                Err(XwcqError::DataSize)
            }
        }
    }

    /// 限时等待从循环队列的后面取出一份数据。（反向离队 Reversely deQueue）
    ///
    /// + 当循环队列未初始化，返回错误码 [`XwcqError::NotInit`] 。
    /// + 若循环队列为空，线程会阻塞等待，等待时会指定一个唤醒时间点。
    /// + 接收时，数据类型是泛型 `T` ，`T` 必须是 [`Sized`] 的，数据是被逐字节地从循环队列的数据槽中拷贝到出来的。
    /// + 循环队列不会对接收时的 `T` 与 发送时的 `T` 进行检查，因此用户必须确保两个类型是一样的，或能安全地进行转换。
    /// + 当 `T` 大小大于循环队列的数据槽大小 `S` ，接收会失败，并返回错误码 [`XwcqError::DataSize`] 。
    /// + 当线程阻塞等待被中断时，返回错误码 [`XwcqError::Interrupt`] 。
    /// + 当到达指定的唤醒时间点，线程被唤醒，返回错误码 [`XwcqError::Timedout`] 。
    ///
    /// # 上下文
    ///
    /// + 线程
    ///
    /// # 示例
    ///
    /// ```rust
    /// extern crate core;
    /// use core::str;
    /// use core::slice::memchr::memchr;
    ///
    /// use xwrust::xwtm;
    /// use xwrust::xwmd::xwcq::*;
    ///
    /// static CQ: Xwcq<64, 16> = Xwcq::new();
    ///
    /// pub fn xwrust_example_xwcq() {
    ///     CQ.init();
    ///     // ...省略...
    ///     let rc = CQ.rq_to::<[u8; 64]>(xwtm::ft(xwtm::s(1)));
    ///     match rc {
    ///         Ok(d) => { // 获取数据成功
    ///             // 从 `[u8; 64]` 数组中构建切片 `&str`
    ///             let totalslice = &d[0..64];
    ///             let nullpos = memchr(0, totalslice).unwrap_or(64);
    ///             let msgu8slice = &d[0..nullpos];
    ///             let msg = str::from_utf8(msgu8slice);
    ///         },
    ///         Err(e) => { // 获取数据失败
    ///             break;
    ///         },
    ///     };
    /// }
    /// ```
    ///
    /// [`Sized`]: <https://doc.rust-lang.org/std/marker/trait.Sized.html>
    pub fn rq_to<T>(&self, to: XwTm) -> Result<T, XwcqError>
    where
        T: Sized + Send
    {
        unsafe {
            let mut sdusz: XwSz = mem::size_of::<T>();
            if sdusz <= self.size() {
                let mut rc = xwcq_acquire(self.cq.get(), *self.tik.get());
                if rc == 0 {
                    let mut sdu: T = mem::zeroed();
                    rc = xwcq_rq_to(self.cq.get(),
                                    &mut sdu as *mut T as *mut u8, &mut sdusz as _,
                                    to);
                    xwcq_put(self.cq.get());
                    if XWOK == rc {
                        Ok(sdu)
                    } else if  -EINTR == rc {
                        Err(XwcqError::Interrupt)
                    } else if -ETIMEDOUT == rc {
                        Err(XwcqError::Timedout)
                    } else if -ENOTTHDCTX == rc {
                        Err(XwcqError::NotThreadContext)
                    } else if -ECANNOTPMPT == rc {
                        Err(XwcqError::CannotPmpt)
                    } else if -ECANNOTBH == rc {
                        Err(XwcqError::CannotBh)
                    } else {
                        Err(XwcqError::Unknown(rc))
                    }
                } else {
                    Err(XwcqError::NotInit)
                }
            } else {
                Err(XwcqError::DataSize)
            }
        }
    }

    /// 尝试从循环队列的后面取出一份数据。（反向离队 Reversely deQueue）
    ///
    /// + 当循环队列未初始化，返回错误码 [`XwcqError::NotInit`] 。
    /// + 若循环队列为空，立即返回 [`XwcqError::DataSize`] ，此方法不会阻塞。
    /// + 接收时，数据类型是泛型 `T` ，`T` 必须是 [`Sized`] 的，数据是被逐字节地从循环队列的数据槽中拷贝到出来的。
    /// + 循环队列不会对接收时的 `T` 与 发送时的 `T` 进行检查，因此用户必须确保两个类型是一样的，或能安全地进行转换。
    /// + 当 `T` 大小大于循环队列的数据槽大小 `S` ，接收会失败，并返回错误码 [`XwcqError::DataSize`] 。
    ///
    /// # 上下文
    ///
    /// + 任意
    ///
    /// # 示例
    ///
    /// ```rust
    /// extern crate core;
    /// use core::str;
    /// use core::slice::memchr::memchr;
    ///
    /// use xwrust::xwmd::xwcq::*;
    ///
    /// static CQ: Xwcq<64, 16> = Xwcq::new();
    ///
    /// pub fn xwrust_example_xwcq() {
    ///     CQ.init();
    ///     // ...省略...
    ///     let rc = CQ.tryrq::<[u8; 64]>();
    ///     match rc {
    ///         Ok(d) => { // 获取数据成功
    ///             // 从 `[u8; 64]` 数组中构建切片 `&str`
    ///             let totalslice = &d[0..64];
    ///             let nullpos = memchr(0, totalslice).unwrap_or(64);
    ///             let msgu8slice = &d[0..nullpos];
    ///             let msg = str::from_utf8(msgu8slice);
    ///         },
    ///         Err(e) => { // 获取数据失败
    ///         },
    ///     };
    /// }
    /// ```
    ///
    /// [`Sized`]: <https://doc.rust-lang.org/std/marker/trait.Sized.html>
    pub fn tryrq<T>(&self) -> Result<T, XwcqError>
    where
        T: Sized + Send
    {
        unsafe {
            let mut sdusz: XwSz = mem::size_of::<T>();
            if sdusz <= self.size() {
                let mut rc = xwcq_acquire(self.cq.get(), *self.tik.get());
                if rc == 0 {
                    let mut sdu: T = mem::zeroed();
                    rc = xwcq_tryrq(self.cq.get(),
                                    &mut sdu as *mut T as *mut u8, &mut sdusz as _);
                    xwcq_put(self.cq.get());
                    if XWOK == rc {
                        Ok(sdu)
                    } else if -ENODATA == rc {
                        Err(XwcqError::NoData)
                    } else {
                        Err(XwcqError::Unknown(rc))
                    }
                } else {
                    Err(XwcqError::NotInit)
                }
            } else {
                Err(XwcqError::DataSize)
            }
        }
    }

    /// 等待从循环队列前面拷贝一份数据。（Peek at the Front of Queue）
    ///
    /// + 当循环队列未初始化，返回错误码 [`XwcqError::NotInit`] 。
    /// + 从循环队列中拷贝数据时，数据不会取走数据，数据槽不会被释放。
    /// + 若循环队列为空，线程会阻塞等待。
    /// + 接收时，数据类型是泛型 `T` ，`T` 必须是 [`Sized`] 的，数据是被逐字节地从循环队列的数据槽中拷贝到出来的。
    /// + 循环队列不会对接收时的 `T` 与 发送时的 `T` 进行检查，因此用户必须确保两个类型是一样的，或能安全地进行转换。
    /// + 当 `T` 大小大于循环队列的数据槽大小 `S` ，接收会失败，并返回错误码 [`XwcqError::DataSize`] 。
    /// + 当线程阻塞等待被中断时，返回错误码 [`XwcqError::Interrupt`] 。
    ///
    /// # 上下文
    ///
    /// + 线程
    ///
    /// # 示例
    ///
    /// ```rust
    /// extern crate core;
    /// use core::str;
    /// use core::slice::memchr::memchr;
    ///
    /// use xwrust::xwmd::xwcq::*;
    ///
    /// static CQ: Xwcq<64, 16> = Xwcq::new();
    ///
    /// pub fn xwrust_example_xwcq() {
    ///     CQ.init();
    ///     // ...省略...
    ///     let rc = CQ.pfq::<[u8; 64]>();
    ///     match rc {
    ///         Ok(d) => { // 获取数据成功
    ///             // 从 `[u8; 64]` 数组中构建切片 `&str`
    ///             let totalslice = &d[0..64];
    ///             let nullpos = memchr(0, totalslice).unwrap_or(64);
    ///             let msgu8slice = &d[0..nullpos];
    ///             let msg = str::from_utf8(msgu8slice);
    ///         },
    ///         Err(e) => { // 获取数据失败
    ///             break;
    ///         },
    ///     };
    /// }
    /// ```
    ///
    /// [`Sized`]: <https://doc.rust-lang.org/std/marker/trait.Sized.html>
    pub fn pfq<T>(&self) -> Result<T, XwcqError>
    where
        T: Sized + Send
    {
        unsafe {
            let mut sdusz: XwSz = mem::size_of::<T>();
            if sdusz <= self.size() {
                let mut rc = xwcq_acquire(self.cq.get(), *self.tik.get());
                if rc == 0 {
                    let mut sdu: T = mem::zeroed();
                    rc = xwcq_pfq(self.cq.get(),
                                  &mut sdu as *mut T as *mut u8, &mut sdusz as _);
                    xwcq_put(self.cq.get());
                    if XWOK == rc {
                        Ok(sdu)
                    } else if  -EINTR == rc {
                        Err(XwcqError::Interrupt)
                    } else if -ENOTTHDCTX == rc {
                        Err(XwcqError::NotThreadContext)
                    } else if -ECANNOTPMPT == rc {
                        Err(XwcqError::CannotPmpt)
                    } else if -ECANNOTBH == rc {
                        Err(XwcqError::CannotBh)
                    } else {
                        Err(XwcqError::Unknown(rc))
                    }
                } else {
                    Err(XwcqError::NotInit)
                }
            } else {
                Err(XwcqError::DataSize)
            }
        }
    }

    /// 限时等待从循环队列前面拷贝一份数据。（Peek at the Front of Queue）
    ///
    /// + 当循环队列未初始化，返回错误码 [`XwcqError::NotInit`] 。
    /// + 从循环队列中拷贝数据时，数据不会取走数据，数据槽不会被释放。
    /// + 若循环队列为空，线程会阻塞等待，等待时会指定一个唤醒时间点。
    /// + 接收时，数据类型是泛型 `T` ，`T` 必须是 [`Sized`] 的，数据是被逐字节地从循环队列的数据槽中拷贝到出来的。
    /// + 循环队列不会对接收时的 `T` 与 发送时的 `T` 进行检查，因此用户必须确保两个类型是一样的，或能安全地进行转换。
    /// + 当 `T` 大小大于循环队列的数据槽大小 `S` ，接收会失败，并返回错误码 [`XwcqError::DataSize`] 。
    /// + 当线程阻塞等待被中断时，返回错误码 [`XwcqError::Interrupt`] 。
    /// + 当到达指定的唤醒时间点，线程被唤醒，返回错误码 [`XwcqError::Timedout`] 。
    ///
    /// # 上下文
    ///
    /// + 线程
    ///
    /// # 示例
    ///
    /// ```rust
    /// extern crate core;
    /// use core::str;
    /// use core::slice::memchr::memchr;
    ///
    /// use xwrust::xwtm;
    /// use xwrust::xwmd::xwcq::*;
    ///
    /// static CQ: Xwcq<64, 16> = Xwcq::new();
    ///
    /// pub fn xwrust_example_xwcq() {
    ///     CQ.init();
    ///     // ...省略...
    ///     let rc = CQ.pfq_to::<[u8; 64]>(xwtm::ft(xwtm::s(1)));
    ///     match rc {
    ///         Ok(d) => { // 获取数据成功
    ///             // 从 `[u8; 64]` 数组中构建切片 `&str`
    ///             let totalslice = &d[0..64];
    ///             let nullpos = memchr(0, totalslice).unwrap_or(64);
    ///             let msgu8slice = &d[0..nullpos];
    ///             let msg = str::from_utf8(msgu8slice);
    ///         },
    ///         Err(e) => { // 获取数据失败
    ///             break;
    ///         },
    ///     };
    /// }
    /// ```
    ///
    /// [`Sized`]: <https://doc.rust-lang.org/std/marker/trait.Sized.html>
    pub fn pfq_to<T>(&self, to: XwTm) -> Result<T, XwcqError>
    where
        T: Sized + Send
    {
        unsafe {
            let mut sdusz: XwSz = mem::size_of::<T>();
            if sdusz <= self.size() {
                let mut rc = xwcq_acquire(self.cq.get(), *self.tik.get());
                if rc == 0 {
                    let mut sdu: T = mem::zeroed();
                    rc = xwcq_pfq_to(self.cq.get(),
                                     &mut sdu as *mut T as *mut u8, &mut sdusz as _,
                                     to);
                    xwcq_put(self.cq.get());
                    if XWOK == rc {
                        Ok(sdu)
                    } else if  -EINTR == rc {
                        Err(XwcqError::Interrupt)
                    } else if -ETIMEDOUT == rc {
                        Err(XwcqError::Timedout)
                    } else if -ENOTTHDCTX == rc {
                        Err(XwcqError::NotThreadContext)
                    } else if -ECANNOTPMPT == rc {
                        Err(XwcqError::CannotPmpt)
                    } else if -ECANNOTBH == rc {
                        Err(XwcqError::CannotBh)
                    } else {
                        Err(XwcqError::Unknown(rc))
                    }
                } else {
                    Err(XwcqError::NotInit)
                }
            } else {
                Err(XwcqError::DataSize)
            }
        }
    }

    /// 尝试从循环队列前面拷贝一份数据。（Peek at the Front of Queue）
    ///
    /// + 当循环队列未初始化，返回错误码 [`XwcqError::NotInit`] 。
    /// + 从循环队列中拷贝数据时，数据不会取走数据，数据槽不会被释放。
    /// + 若循环队列为空，立即返回 [`XwcqError::DataSize`] ，此方法不会阻塞。
    /// + 接收时，数据类型是泛型 `T` ，`T` 必须是 [`Sized`] 的，数据是被逐字节地从循环队列的数据槽中拷贝到出来的。
    /// + 循环队列不会对接收时的 `T` 与 发送时的 `T` 进行检查，因此用户必须确保两个类型是一样的，或能安全地进行转换。
    /// + 当 `T` 大小大于循环队列的数据槽大小 `S` ，接收会失败，并返回错误码 [`XwcqError::DataSize`] 。
    ///
    /// # 上下文
    ///
    /// + 任意
    ///
    /// # 示例
    ///
    /// ```rust
    /// extern crate core;
    /// use core::str;
    /// use core::slice::memchr::memchr;
    ///
    /// use xwrust::xwmd::xwcq::*;
    ///
    /// static CQ: Xwcq<64, 16> = Xwcq::new();
    ///
    /// pub fn xwrust_example_xwcq() {
    ///     CQ.init();
    ///     // ...省略...
    ///     let rc = CQ.trypfq::<[u8; 64]>();
    ///     match rc {
    ///         Ok(d) => { // 获取数据成功
    ///             // 从 `[u8; 64]` 数组中构建切片 `&str`
    ///             let totalslice = &d[0..64];
    ///             let nullpos = memchr(0, totalslice).unwrap_or(64);
    ///             let msgu8slice = &d[0..nullpos];
    ///             let msg = str::from_utf8(msgu8slice);
    ///         },
    ///         Err(e) => { // 获取数据失败
    ///         },
    ///     };
    /// }
    /// ```
    ///
    /// [`Sized`]: <https://doc.rust-lang.org/std/marker/trait.Sized.html>
    pub fn trypfq<T>(&self) -> Result<T, XwcqError>
    where
        T: Sized + Send
    {
        unsafe {
            let mut sdusz: XwSz = mem::size_of::<T>();
            if sdusz <= self.size() {
                let mut rc = xwcq_acquire(self.cq.get(), *self.tik.get());
                if rc == 0 {
                    let mut sdu: T = mem::zeroed();
                    rc = xwcq_trypfq(self.cq.get(),
                                     &mut sdu as *mut T as *mut u8, &mut sdusz as _);
                    xwcq_put(self.cq.get());
                    if XWOK == rc {
                        Ok(sdu)
                    } else if -ENODATA == rc {
                        Err(XwcqError::NoData)
                    } else {
                        Err(XwcqError::Unknown(rc))
                    }
                } else {
                    Err(XwcqError::NotInit)
                }
            } else {
                Err(XwcqError::DataSize)
            }
        }
    }

    /// 等待从循环队列后面拷贝一份数据。（Peek at the Rear of Queue）
    ///
    /// + 当循环队列未初始化，返回错误码 [`XwcqError::NotInit`] 。
    /// + 从循环队列中拷贝数据时，数据不会取走数据，数据槽不会被释放。
    /// + 若循环队列为空，线程会阻塞等待。
    /// + 接收时，数据类型是泛型 `T` ，`T` 必须是 [`Sized`] 的，数据是被逐字节地从循环队列的数据槽中拷贝到出来的。
    /// + 循环队列不会对接收时的 `T` 与 发送时的 `T` 进行检查，因此用户必须确保两个类型是一样的，或能安全地进行转换。
    /// + 当 `T` 大小大于循环队列的数据槽大小 `S` ，接收会失败，并返回错误码 [`XwcqError::DataSize`] 。
    /// + 当线程阻塞等待被中断时，返回错误码 [`XwcqError::Interrupt`] 。
    ///
    /// # 上下文
    ///
    /// + 线程
    ///
    /// # 示例
    ///
    /// ```rust
    /// extern crate core;
    /// use core::str;
    /// use core::slice::memchr::memchr;
    ///
    /// use xwrust::xwmd::xwcq::*;
    ///
    /// static CQ: Xwcq<64, 16> = Xwcq::new();
    ///
    /// pub fn xwrust_example_xwcq() {
    ///     CQ.init();
    ///     // ...省略...
    ///     let rc = CQ.prq::<[u8; 64]>();
    ///     match rc {
    ///         Ok(d) => { // 获取数据成功
    ///             // 从 `[u8; 64]` 数组中构建切片 `&str`
    ///             let totalslice = &d[0..64];
    ///             let nullpos = memchr(0, totalslice).unwrap_or(64);
    ///             let msgu8slice = &d[0..nullpos];
    ///             let msg = str::from_utf8(msgu8slice);
    ///         },
    ///         Err(e) => { // 获取数据失败
    ///             break;
    ///         },
    ///     };
    /// }
    /// ```
    ///
    /// [`Sized`]: <https://doc.rust-lang.org/std/marker/trait.Sized.html>
    pub fn prq<T>(&self) -> Result<T, XwcqError>
    where
        T: Sized + Send
    {
        unsafe {
            let mut sdusz: XwSz = mem::size_of::<T>();
            if sdusz <= self.size() {
                let mut rc = xwcq_acquire(self.cq.get(), *self.tik.get());
                if rc == 0 {
                    let mut sdu: T = mem::zeroed();
                    rc = xwcq_prq(self.cq.get(),
                                  &mut sdu as *mut T as *mut u8, &mut sdusz as _);
                    xwcq_put(self.cq.get());
                    if XWOK == rc {
                        Ok(sdu)
                    } else if  -EINTR == rc {
                        Err(XwcqError::Interrupt)
                    } else if -ENOTTHDCTX == rc {
                        Err(XwcqError::NotThreadContext)
                    } else if -ECANNOTPMPT == rc {
                        Err(XwcqError::CannotPmpt)
                    } else if -ECANNOTBH == rc {
                        Err(XwcqError::CannotBh)
                    } else {
                        Err(XwcqError::Unknown(rc))
                    }
                } else {
                    Err(XwcqError::NotInit)
                }
            } else {
                Err(XwcqError::DataSize)
            }
        }
    }

    /// 限时等待从循环队列后面拷贝一份数据。（Peek at the Rear of Queue）
    ///
    /// + 当循环队列未初始化，返回错误码 [`XwcqError::NotInit`] 。
    /// + 从循环队列中拷贝数据时，数据不会取走数据，数据槽不会被释放。
    /// + 若循环队列为空，线程会阻塞等待，等待时会指定一个唤醒时间点。
    /// + 接收时，数据类型是泛型 `T` ，`T` 必须是 [`Sized`] 的，数据是被逐字节地从循环队列的数据槽中拷贝到出来的。
    /// + 循环队列不会对接收时的 `T` 与 发送时的 `T` 进行检查，因此用户必须确保两个类型是一样的，或能安全地进行转换。
    /// + 当 `T` 大小大于循环队列的数据槽大小 `S` ，接收会失败，并返回错误码 [`XwcqError::DataSize`] 。
    /// + 当线程阻塞等待被中断时，返回错误码 [`XwcqError::Interrupt`] 。
    /// + 当到达指定的唤醒时间点，线程被唤醒，返回错误码 [`XwcqError::Timedout`] 。
    ///
    /// # 上下文
    ///
    /// + 线程
    ///
    /// # 示例
    ///
    /// ```rust
    /// extern crate core;
    /// use core::str;
    /// use core::slice::memchr::memchr;
    ///
    /// use xwrust::xwtm;
    /// use xwrust::xwmd::xwcq::*;
    ///
    /// static CQ: Xwcq<64, 16> = Xwcq::new();
    ///
    /// pub fn xwrust_example_xwcq() {
    ///     CQ.init();
    ///     // ...省略...
    ///     let rc = CQ.prq_to::<[u8; 64]>(xwtm::ft(xwtm::s(1)));
    ///     match rc {
    ///         Ok(d) => { // 获取数据成功
    ///             // 从 `[u8; 64]` 数组中构建切片 `&str`
    ///             let totalslice = &d[0..64];
    ///             let nullpos = memchr(0, totalslice).unwrap_or(64);
    ///             let msgu8slice = &d[0..nullpos];
    ///             let msg = str::from_utf8(msgu8slice);
    ///         },
    ///         Err(e) => { // 获取数据失败
    ///             break;
    ///         },
    ///     };
    /// }
    /// ```
    ///
    /// [`Sized`]: <https://doc.rust-lang.org/std/marker/trait.Sized.html>
    pub fn prq_to<T>(&self, to: XwTm) -> Result<T, XwcqError>
    where
        T: Sized + Send
    {
        unsafe {
            let mut sdusz: XwSz = mem::size_of::<T>();
            if sdusz <= self.size() {
                let mut rc = xwcq_acquire(self.cq.get(), *self.tik.get());
                if rc == 0 {
                    let mut sdu: T = mem::zeroed();
                    rc = xwcq_prq_to(self.cq.get(),
                                     &mut sdu as *mut T as *mut u8, &mut sdusz as _,
                                     to);
                    xwcq_put(self.cq.get());
                    if XWOK == rc {
                        Ok(sdu)
                    } else if  -EINTR == rc {
                        Err(XwcqError::Interrupt)
                    } else if -ETIMEDOUT == rc {
                        Err(XwcqError::Timedout)
                    } else if -ENOTTHDCTX == rc {
                        Err(XwcqError::NotThreadContext)
                    } else if -ECANNOTPMPT == rc {
                        Err(XwcqError::CannotPmpt)
                    } else if -ECANNOTBH == rc {
                        Err(XwcqError::CannotBh)
                    } else {
                        Err(XwcqError::Unknown(rc))
                    }
                } else {
                    Err(XwcqError::NotInit)
                }
            } else {
                Err(XwcqError::DataSize)
            }
        }
    }

    /// 尝试从循环队列后面拷贝一份数据。（Peek at the Rear of Queue）
    ///
    /// + 当循环队列未初始化，返回错误码 [`XwcqError::NotInit`] 。
    /// + 从循环队列中拷贝数据时，数据不会取走数据，数据槽不会被释放。
    /// + 若循环队列为空，立即返回 [`XwcqError::DataSize`] ，此方法不会阻塞。
    /// + 接收时，数据类型是泛型 `T` ，`T` 必须是 [`Sized`] 的，数据是被逐字节地从循环队列的数据槽中拷贝到出来的。
    /// + 循环队列不会对接收时的 `T` 与 发送时的 `T` 进行检查，因此用户必须确保两个类型是一样的，或能安全地进行转换。
    /// + 当 `T` 大小大于循环队列的数据槽大小 `S` ，接收会失败，并返回错误码 [`XwcqError::DataSize`] 。
    ///
    /// # 上下文
    ///
    /// + 任意
    ///
    /// # 示例
    ///
    /// ```rust
    /// extern crate core;
    /// use core::str;
    /// use core::slice::memchr::memchr;
    ///
    /// use xwrust::xwmd::xwcq::*;
    ///
    /// static CQ: Xwcq<64, 16> = Xwcq::new();
    ///
    /// pub fn xwrust_example_xwcq() {
    ///     CQ.init();
    ///     // ...省略...
    ///     let rc = CQ.tryprq::<[u8; 64]>();
    ///     match rc {
    ///         Ok(d) => { // 获取数据成功
    ///             // 从 `[u8; 64]` 数组中构建切片 `&str`
    ///             let totalslice = &d[0..64];
    ///             let nullpos = memchr(0, totalslice).unwrap_or(64);
    ///             let msgu8slice = &d[0..nullpos];
    ///             let msg = str::from_utf8(msgu8slice);
    ///         },
    ///         Err(e) => { // 获取数据失败
    ///         },
    ///     };
    /// }
    /// ```
    ///
    /// [`Sized`]: <https://doc.rust-lang.org/std/marker/trait.Sized.html>
    pub fn tryprq<T>(&self) -> Result<T, XwcqError>
    where
        T: Sized + Send
    {
        unsafe {
            let mut sdusz: XwSz = mem::size_of::<T>();
            if sdusz <= self.size() {
                let mut rc = xwcq_acquire(self.cq.get(), *self.tik.get());
                if rc == 0 {
                    let mut sdu: T = mem::zeroed();
                    rc = xwcq_tryprq(self.cq.get(),
                                     &mut sdu as *mut T as *mut u8, &mut sdusz as _);
                    xwcq_put(self.cq.get());
                    if XWOK == rc {
                        Ok(sdu)
                    } else if -ENODATA == rc {
                        Err(XwcqError::NoData)
                    } else {
                        Err(XwcqError::Unknown(rc))
                    }
                } else {
                    Err(XwcqError::NotInit)
                }
            } else {
                Err(XwcqError::DataSize)
            }
        }
    }

    /// 清空循环队列。
    ///
    /// + 当循环队列未初始化，返回错误码 [`XwcqError::NotInit`] 。
    ///
    /// # 上下文
    ///
    /// + 任意
    pub fn flush(&self) -> XwcqError {
        unsafe {
            let mut rc = xwcq_acquire(self.cq.get(), *self.tik.get());
            if rc == 0 {
                rc = xwcq_flush(self.cq.get());
                xwcq_put(self.cq.get());
                if XWOK == rc {
                    XwcqError::Ok
                } else {
                    XwcqError::Unknown(rc)
                }
            } else {
                XwcqError::NotInit
            }
        }
    }

    /// 获取循环队列的容量。
    ///
    /// 循环队列的容量也即是数据槽的数量，是在创建循环队列时，指明的类型常数 `N` 。
    ///
    /// # 上下文
    ///
    /// + 任意
    pub const fn capacity(&self) -> XwSz {
        N
    }

    /// 获取循环队列的单个数据槽的大小。
    ///
    /// 循环队列的单个数据槽的大小是在创建循环队列时，指明的类型常数 `S` 。
    /// 若 `S` 不满足内存对齐的要求，会自动向上调整。
    ///
    /// # 上下文
    ///
    /// + 任意
    pub const fn size(&self) -> XwSz {
        (S + XWCQ_MM_ALIGN - 1) & (!XWCQ_MM_MSK)
    }

    /// 获取循环队列中有效数据的数量。
    ///
    /// 有效数据的数量是指可被取出的数据的数量。
    ///
    /// # 上下文
    ///
    /// + 任意
    pub fn availability(&self) -> Result<XwSz, XwcqError> {
        unsafe {
            let mut rc = xwcq_acquire(self.cq.get(), *self.tik.get());
            if rc == 0 {
                let mut avb: XwSz = 0;
                rc = xwcq_get_availability(self.cq.get(), &mut avb);
                xwcq_put(self.cq.get());
                if XWOK == rc {
                    Ok(avb)
                } else {
                    Err(XwcqError::Unknown(rc))
                }
            } else {
                Err(XwcqError::NotInit)
            }
        }
    }
}
