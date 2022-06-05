//! XWOS RUST：位图数组
//! ========
//!
//!

extern crate core;
use core::ffi::*;
use core::cmp::{PartialOrd, Ordering};
use core::cell::UnsafeCell;
use core::ops::*;
use core::fmt;

use crate::types::*;

extern "C" {
    pub(crate) fn xwbmpop_cmp(bmp: *mut c_void, opt: *mut c_void, num: XwSz) -> XwSsq;
    pub(crate) fn xwbmpop_s1all(bmp: *mut c_void, num: XwSz);
    pub(crate) fn xwbmpop_c0all(bmp: *mut c_void, num: XwSz);
    pub(crate) fn xwbmpop_s1i(bmp: *mut c_void, n: XwSq);
    pub(crate) fn xwbmpop_s1m(bmp: *mut c_void, msk: *mut c_void, num: XwSz);
    pub(crate) fn xwbmpop_c0i(bmp: *mut c_void, n: XwSq);
    pub(crate) fn xwbmpop_c0m(bmp: *mut c_void, msk: *mut c_void, num: XwSz);
    pub(crate) fn xwbmpop_x1i(bmp: *mut c_void, n: XwSq);
    pub(crate) fn xwbmpop_x1m(bmp: *mut c_void, msk: *mut c_void, num: XwSz);
    pub(crate) fn xwbmpop_t1i(bmp: *mut c_void, n: XwSq) -> bool;
    pub(crate) fn xwbmpop_t1ma(bmp: *mut c_void, msk: *mut c_void, num: XwSz) -> bool;
    pub(crate) fn xwbmpop_t1ma_then_c0m(bmp: *mut c_void, msk: *mut c_void, num: XwSz) -> bool;
    pub(crate) fn xwbmpop_t1mo(bmp: *mut c_void, msk: *mut c_void, num: XwSz) -> bool;
    pub(crate) fn xwbmpop_t1mo_then_c0m(bmp: *mut c_void, msk: *mut c_void, num: XwSz) -> bool;
    pub(crate) fn xwbmpop_t0ma(bmp: *mut c_void, msk: *mut c_void, num: XwSz) -> bool;
    pub(crate) fn xwbmpop_t0ma_then_s1m(bmp: *mut c_void, msk: *mut c_void, num: XwSz) -> bool;
    pub(crate) fn xwbmpop_t0mo(bmp: *mut c_void, msk: *mut c_void, num: XwSz) -> bool;
    pub(crate) fn xwbmpop_t0mo_then_s1m(bmp: *mut c_void, msk: *mut c_void, num: XwSz) -> bool;
    pub(crate) fn xwbmpop_not(bmp: *mut c_void, num: XwSz);
    pub(crate) fn xwbmpop_and(bmp: *mut c_void, msk: *mut c_void, num: XwSz);
    pub(crate) fn xwbmpop_or(bmp: *mut c_void, msk: *mut c_void, num: XwSz);
    pub(crate) fn xwbmpop_xor(bmp: *mut c_void, msk: *mut c_void, num: XwSz);
    pub(crate) fn xwbmpop_ffs(bmp: *mut c_void, num: XwSz) -> XwSsq;
    pub(crate) fn xwbmpop_ffz(bmp: *mut c_void, num: XwSz) -> XwSsq;
    pub(crate) fn xwbmpop_fls(bmp: *mut c_void, num: XwSz) -> XwSsq;
    pub(crate) fn xwbmpop_flz(bmp: *mut c_void, num: XwSz) -> XwSsq;
    pub(crate) fn xwbmpop_weight(bmp: *mut c_void, num: XwSz) -> XwSz;
}

#[repr(C)]
#[cfg_attr(target_pointer_width = "32", repr(align(8)))]
#[cfg_attr(target_pointer_width = "64", repr(align(16)))]
pub(crate) struct BmpArray<const N: XwSz>
where
    [XwBmp; ((N + XwBmp::BITS as usize - 1) / XwBmp::BITS as usize)]: Sized
{
    #[doc(hidden)]
    pub(crate) array: [XwBmp; ((N + XwBmp::BITS as usize - 1) / XwBmp::BITS as usize)],
}

/// 位图类型
///
/// 常数 `N` 表示有多少位。
pub struct Bmp<const N: XwSz>
where
    [XwBmp; ((N + XwBmp::BITS as usize - 1) / XwBmp::BITS as usize)]: Sized
{
    #[doc(hidden)]
    pub(crate) bmp: UnsafeCell<BmpArray<N>>,
}

unsafe impl<const N: XwSz> Send for Bmp<N>
where
    [XwBmp; ((N + XwBmp::BITS as usize - 1) / XwBmp::BITS as usize)]: Sized
{}

unsafe impl<const N: XwSz> Sync for Bmp<N>
where
    [XwBmp; ((N + XwBmp::BITS as usize - 1) / XwBmp::BITS as usize)]: Sized
{}

impl<const N: XwSz> Bmp<N>
where
    [XwBmp; ((N + XwBmp::BITS as usize - 1) / XwBmp::BITS as usize)]: Sized
{
    /// 新建位图
    ///
    /// 此方法是编译期方法。
    ///
    /// # 示例
    ///
    /// + 具有 [`'static`] 约束的全局变量全局变量：
    ///
    /// ```rust
    /// use xwrust::xwbmp::*;
    ///
    /// static GLOBAL_BMP = Bmp::<8>::new();
    /// ```
    ///
    /// + 在函数中创建：
    ///
    /// ```rust
    /// extern crate alloc;
    /// use alloc::sync::Arc;
    /// use xwrust::xwbmp::*;
    ///
    /// pub fn some_function() {
    ///     let msk = Bmp::<8>::new();
    /// }
    /// ```
    ///
    /// + 在heap上创建：
    ///
    /// ```rust
    /// extern crate alloc;
    /// use alloc::sync::Arc;
    /// use xwrust::xwbmp::*;
    ///
    /// pub fn some_function() {
    ///     let msk = Arc::new(Bmp::<8>::new());
    /// }
    /// ```
    ///
    /// [`'static`]: https://doc.rust-lang.org/std/keyword.static.html
    pub const fn new() -> Self {
        Self {
            bmp: UnsafeCell::new(BmpArray {
                array: [0; ((N + XwBmp::BITS as usize - 1) / XwBmp::BITS as usize)],
            }),
        }
    }

    /// 获取位图位的数量
    pub const fn bits(&self) -> XwSz {
        N
    }

    /// 在位图中统计被置 **1** 的位的个数
    pub fn weight(&self) -> XwSz {
        unsafe {
            xwbmpop_weight(self.bmp.get() as _ , N)
        }
    }

    /// 将位图中所有位置 **1**
    pub fn s1all(&self) {
        unsafe {
            xwbmpop_s1all(self.bmp.get() as _ , N);
        }
    }

    /// 将位图中所有位清 **0**
    pub fn c0all(&self) {
        unsafe {
            xwbmpop_c0all(self.bmp.get() as _ , N);
        }
    }

    /// 将位图中单个位置 **1**
    pub fn s1i(&self, n: XwSq) {
        unsafe {
            xwbmpop_s1i(self.bmp.get() as _ , n);
        }
    }

    /// 将位图中掩码部分置 **1**
    pub fn s1m(&self, msk: &Self) {
        unsafe {
            xwbmpop_s1m(self.bmp.get() as _ , msk.bmp.get() as _, N);
        }
    }

    /// 将位图中单个位清 **0**
    pub fn c0i(&self, n: XwSq) {
        unsafe {
            xwbmpop_c0i(self.bmp.get() as _ , n);
        }
    }

    /// 将位图中掩码部分清 **0**
    pub fn c0m(&self, msk: &Self) {
        unsafe {
            xwbmpop_c0m(self.bmp.get() as _ , msk.bmp.get() as _, N);
        }
    }

    /// 将位图中单个位翻转
    pub fn x1i(&self, n: XwSq) {
        unsafe {
            xwbmpop_x1i(self.bmp.get() as _ , n);
        }
    }

    /// 将位图中掩码部分翻转
    pub fn x1m(&self, msk: &Self) {
        unsafe {
            xwbmpop_x1m(self.bmp.get() as _ , msk.bmp.get() as _, N);
        }
    }

    /// 测试位图中的单个位是否为 **1**
    pub fn t1i(&self, n: XwSq) -> bool {
        unsafe {
            xwbmpop_t1i(self.bmp.get() as _ , n)
        }
    }

    /// 测试位图中掩码部分是否全部为 **1**
    pub fn t1ma(&self, msk: &Self) -> bool {
        unsafe {
            xwbmpop_t1ma(self.bmp.get() as _ , msk.bmp.get() as _, N)
        }
    }

    /// 测试位图中掩码部分是否至少有一位为 **1**
    pub fn t1mo(&self, msk: &Self) -> bool {
        unsafe {
            xwbmpop_t1mo(self.bmp.get() as _ , msk.bmp.get() as _, N)
        }
    }

    /// 测试位图中掩码部分是否全部为 **1** ，如果是，就将掩码部分全部清 **0**
    pub fn t1ma_then_c0m(&self, msk: &Self) -> bool {
        unsafe {
            xwbmpop_t1ma_then_c0m(self.bmp.get() as _ , msk.bmp.get() as _, N)
        }
    }

    /// 测试位图中掩码部分是否至少有一位为 **1** ，如果是，就将掩码部分全部清 **0**
    pub fn t1mo_then_c0m(&self, msk: &Self) -> bool {
        unsafe {
            xwbmpop_t1mo_then_c0m(self.bmp.get() as _ , msk.bmp.get() as _, N)
        }
    }

    /// 测试位图中掩码部分是否全部为 **0** ，如果是，就将掩码部分全部置 **1**
    pub fn t0ma_then_s1m(&self, msk: &Self) -> bool {
        unsafe {
            xwbmpop_t0ma_then_s1m(self.bmp.get() as _ , msk.bmp.get() as _, N)
        }
    }

    /// 测试位图中掩码部分是否至少有一位为 **0** ，如果是，就将掩码部分全部置 **1**
    pub fn t0mo_then_s1m(&self, msk: &Self) -> bool {
        unsafe {
            xwbmpop_t0mo_then_s1m(self.bmp.get() as _ , msk.bmp.get() as _, N)
        }
    }

    /// 在位图中从最低位起查找第一个被置 **1** 的位
    pub fn ffs(&self) -> XwSsq {
        unsafe {
            xwbmpop_ffs(self.bmp.get() as _ , N)
        }
    }

    /// 在位图中从最低位起查找第一个被清 **0** 的位
    pub fn ffz(&self) -> XwSsq {
        unsafe {
            xwbmpop_ffz(self.bmp.get() as _ , N)
        }
    }

    /// 在位图中从最高位起查找第一个被置 **1** 的位
    pub fn fls(&self) -> XwSsq {
        unsafe {
            xwbmpop_fls(self.bmp.get() as _ , N)
        }
    }

    /// 在位图中从最高位起查找第一个被清 **0** 的位
    pub fn flz(&self) -> XwSsq {
        unsafe {
            xwbmpop_flz(self.bmp.get() as _ , N)
        }
    }
}

impl<const N: XwSz> PartialEq for Bmp<N>
where
    [XwBmp; ((N + XwBmp::BITS as usize - 1) / XwBmp::BITS as usize)]: Sized
{
    fn eq(&self, other: &Self) -> bool {
        unsafe {
            let rc = xwbmpop_cmp(self.bmp.get() as _, other.bmp.get() as _, N);
            rc == 0
        }
    }
}

impl<const N: XwSz> PartialOrd for Bmp<N>
where
    [XwBmp; ((N + XwBmp::BITS as usize - 1) / XwBmp::BITS as usize)]: Sized
{
    fn partial_cmp(&self, other: &Self) -> Option<Ordering> {
        unsafe {
            let rc = xwbmpop_cmp(self.bmp.get() as _, other.bmp.get() as _, N);
            if rc < 0 {
                Some(Ordering::Less)
            } else if rc > 0 {
                Some(Ordering::Greater)
            } else {
                Some(Ordering::Equal)
            }
        }
    }
}

impl<const N: XwSz> BitAnd for Bmp<N>
where
    [XwBmp; ((N + XwBmp::BITS as usize - 1) / XwBmp::BITS as usize)]: Sized
{
    type Output = Self;

    fn bitand(self, rhs: Self) -> Self {
        unsafe {
            xwbmpop_and(self.bmp.get() as _, rhs.bmp.get() as _, N);
            self
        }
    }
}

impl<const N: XwSz> BitAndAssign for Bmp<N>
where
    [XwBmp; ((N + XwBmp::BITS as usize - 1) / XwBmp::BITS as usize)]: Sized
{
    fn bitand_assign(&mut self, rhs: Self){
        unsafe {
            xwbmpop_and(self.bmp.get() as _, rhs.bmp.get() as _, N);
        }
    }
}

impl<const N: XwSz> BitOr for Bmp<N>
where
    [XwBmp; ((N + XwBmp::BITS as usize - 1) / XwBmp::BITS as usize)]: Sized
{
    type Output = Self;

    fn bitor(self, rhs: Self) -> Self {
        unsafe {
            xwbmpop_or(self.bmp.get() as _, rhs.bmp.get() as _, N);
            self
        }
    }
}

impl<const N: XwSz> BitOrAssign for Bmp<N>
where
    [XwBmp; ((N + XwBmp::BITS as usize - 1) / XwBmp::BITS as usize)]: Sized
{
    fn bitor_assign(&mut self, rhs: Self){
        unsafe {
            xwbmpop_or(self.bmp.get() as _, rhs.bmp.get() as _, N);
        }
    }
}

impl<const N: XwSz> BitXor for Bmp<N>
where
    [XwBmp; ((N + XwBmp::BITS as usize - 1) / XwBmp::BITS as usize)]: Sized
{
    type Output = Self;

    fn bitxor(self, rhs: Self) -> Self {
        unsafe {
            xwbmpop_xor(self.bmp.get() as _, rhs.bmp.get() as _, N);
            self
        }
    }
}

impl<const N: XwSz> BitXorAssign for Bmp<N>
where
    [XwBmp; ((N + XwBmp::BITS as usize - 1) / XwBmp::BITS as usize)]: Sized
{
    fn bitxor_assign(&mut self, rhs: Self){
        unsafe {
            xwbmpop_xor(self.bmp.get() as _, rhs.bmp.get() as _, N);
        }
    }
}

impl<const N: XwSz> Neg for Bmp<N>
where
    [XwBmp; ((N + XwBmp::BITS as usize - 1) / XwBmp::BITS as usize)]: Sized
{
    type Output = Self;

    fn neg(self) -> Self{
        unsafe {
            xwbmpop_not(self.bmp.get() as _, N);
            self
        }
    }
}

impl<const N: XwSz> fmt::Debug for Bmp<N>
where
    [XwBmp; ((N + XwBmp::BITS as usize - 1) / XwBmp::BITS as usize)]: Sized
{
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        unsafe {
            f.debug_struct("Bmp")
                .field("bits", &self.bits())
                .field("array", &(*(self.bmp.get())).array)
                .finish()
        }
    }
}
