/**
 * @file
 * @brief xwos::lock::SMtx
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef __xwos_cxx_lock_SMtx_hxx__
#define __xwos_cxx_lock_SMtx_hxx__

#include <xwos/osal/lock/mtx.hxx>

namespace xwos {
namespace lock {

/**
 * @defgroup xwos_cxx_lock_SMtx 静态互斥锁
 * @ingroup xwos_cxx_lock
 *
 * 静态互斥锁是指互斥锁所需要的内存在编译期由编译器分配。
 *
 *
 * ## 静态互斥锁的RAII机制守卫
 *
 * `SMtx::Grd` 构造时会上锁互斥锁，析构时会自动解锁互斥锁。
 * `SMtx::Grd` 上锁模式分为：
 * + `SMtx::LockMode::SMtxLock` 默认等待模式，如果无法上锁，
 *   调用线程会阻塞等待上锁。
 * + `SMtx::LockMode::SMtxLockTimed` 定时等待模式，如果无法上锁，
 *   调用线程会定时阻塞等待上锁。
 * + `SMtx::LockMode::SMtxLockUninterruptable` 不可中断等待模式，
 *   如果无法上锁，调用线程会阻塞等待上锁，且不可被中断。
 * + `SMtx::LockMode::SMtxLockTry` 尝试上锁模式，如果无法上锁，
 *   调用线程不会阻塞等待，尝试失败立即返回。
 *
 * ### 示例
 *
 * ```Cpp
 * #include <xwos/cxx/lock/SMtx.hxx>
 *
 * xwos::lock::SMtx gSlock;
 *
 * void foo()
 * {
 *     { // 构造 `grd` 时会自动对互斥锁上锁
 *         xwos::lock::SMtx::Grd grd(gSlock, xwos::lock::SMtx::LockMode::SMtxLock);
 *         if (XWOK == grd.getRc()) {
 *             // 上锁成功，进入临界区
 *         } else {
 *             // 上锁失败
 *         }
 *     } // 如果上锁成功，grd析构时会自动解锁互斥锁
 * }
 * ```
 *
 *
 * ## C++ API
 *
 * 头文件： @ref xwos/cxx/lock/SMtx.hxx
 *
 * @{
 */

/**
 * @brief 静态互斥锁
 */
class SMtx
{
  public:
    /**
     * @brief 静态互斥锁的锁模式
     */
    enum LockMode : xwu32_t {
        SMtxLock = 0, /**< 默认等待模式，如果无法上锁，
                           调用线程会阻塞等待上锁。*/
        SMtxLockTimed, /**< 定时等待模式，如果无法上锁，
                            调用线程会定时阻塞等待上锁。*/
        SMtxLockUninterruptable, /**< 不可中断等待模式，如果无法上锁，
                                      调用线程会阻塞等待上锁，且不可被中断。*/
        SMtxLockTry, /**< 尝试上锁模式，如果无法上锁，
                          调用线程不会阻塞等待，尝试失败立即返回。*/
    };

    /**
     * @brief 静态互斥锁的RAII机制守卫
     */
    class Grd
    {
      private:
        SMtx * mMtx;
        xwer_t mRc; /**< 内部上锁CAPI的返回值 */

      public:
        Grd() = delete;

        /**
         * @brief 构造静态互斥锁的RAII机制守卫并上锁互斥锁
         * @param[in] mtx: 静态互斥锁对象指针
         * @param[in] mode: 锁模式，取值 @ref SMtx::LockMode
         *   @arg SMtx::LockMode::SMtxLock
         *   @arg SMtx::LockMode::SMtxLockTimed
         *   @arg SMtx::LockMode::SMtxLockUninterruptable
         *   @arg SMtx::LockMode::SMtxLockTry
         * @param[in] to: 期望唤醒的时间点，
         *                仅当 `mode == SMtx::LockMode::SMtxLockTimed` 时有效。
         * @note
         * + 上下文：线程
         * @details
         * + 上锁分为下列几种模式：
         *   + `SMtx::LockMode::SMtxLock` 默认等待模式，如果无法上锁，
         *     调用线程会阻塞等待上锁。
         *   + `SMtx::LockMode::SMtxLockTimed` 定时等待模式，如果无法上锁，
         *     调用线程会定时阻塞等待上锁。
         *   + `SMtx::LockMode::SMtxLockUninterruptable` 不可中断等待模式，
         *     如果无法上锁，调用线程会阻塞等待上锁，且不可被中断。
         *   + `SMtx::LockMode::SMtxLockTry` 尝试上锁模式，如果无法上锁，
         *     调用线程不会阻塞等待，尝试失败立即返回。
         * + 此C++API返回的是 `SMtx::Grd` 的对象。 `SMtx::Grd` 是
         *   静态互斥锁的RAII机制守卫。
         * + 如果上锁成功( `SMtx::Grd` 内部的 `mRc == XWOK` )， `SMtx::Grd`
         *   对象析构时会自动解锁互斥锁。
         * + 如果上锁失败，错误码存放在 `SMtx::Grd` 内部的 `mRc` ，
         *   可通过 `SMtx::Grd::getRc()` 获取。
         *   此时 `SMtx::Grd` 析构时不会解锁互斥锁。
         * + 当 `mode` 为 `SMtx::LockMode::SMtxLock` 或
         *   `SMtx::LockMode::SMtxLockTimed` ,等待被中断时， `mRc == -EINTR` 。
         * + 当 `mode` 为 `SMtx::LockMode::SMtxLockTimed` ,等待超时时，
         *   `mRc == -ETIMEDOUT` 。
         * + 当 `mode` 为 `SMtx::LockMode::SMtxLockTry` ,上锁失败时，
         *   `mRc == -EWOULDBLOCK` 。
         */
        Grd(SMtx * mtx, enum LockMode mode = LockMode::SMtxLock, xwtm_t to = XWTM_MAX);

        /**
         * @brief 构造静态互斥锁的RAII机制守卫并上锁互斥锁
         * @param[in] mode: 锁模式，取值 @ref SMtx::LockMode
         *   @arg SMtx::LockMode::SMtxLock
         *   @arg SMtx::LockMode::SMtxLockTimed
         *   @arg SMtx::LockMode::SMtxLockUninterruptable
         *   @arg SMtx::LockMode::SMtxLockTry
         * @param[in] to: 期望唤醒的时间点，
         *                仅当 `mode == SMtx::LockMode::SMtxLockTimed` 时有效。
         */
        Grd(SMtx & mtx, enum LockMode mode = LockMode::SMtxLock, xwtm_t to = XWTM_MAX);

        /**
         * @brief 析构静态互斥锁的RAII机制守卫并解锁互斥锁
         */
        ~Grd();

        /**
         * @brief 获取上锁结果错误码
         * @return 错误码
         * @retval XWOK: 没有错误
         * @retval -EFAULT: 无效的指针或空指针
         * @retval -EOBJDEAD: 互斥锁对象无效
         * @retval -EINTR: 等待被中断
         * @retval -ETIMEDOUT: 超时
         * @retval -ENOTTHDCTX: 不在线程上下文中
         * @retval -EDSPMPT: 抢占被关闭
         * @retval -EDSBH: 中断底半部被关闭
         */
        xwer_t getRc() { return mRc; }
    };

    friend Grd;
  private:
    struct xwos_mtx mLock; /**< 互斥锁结构体 */
    xwer_t mCtorRc; /**< 互斥锁构造的结果 */

  public:
    /**
     * @brief 构造函数
     * @param[in] pr: 互斥锁的天花板优先级
     */
    explicit SMtx(xwpr_t pr = XWOS_SKD_PRIORITY_RT_MAX);
    ~SMtx(); /**< 析构函数 */
    xwer_t getCtorRc() { return mCtorRc; } /**< 获取静态互斥锁构造的结果 */

    /* 生命周期管理 */
    xwer_t grab() { return xwos_mtx_grab(&mLock); } /**< 增加引用计数 */
    xwer_t put() { return xwos_mtx_put(&mLock); } /**< 减少引用计数 */

  private:
    static void * operator new(xwsz_t sz) = delete;
    void operator delete(void * obj) = delete;
};

/**
 * @} xwos_cxx_lock_SMtx
 */

} // namespace lock
} // namespace xwos

#endif /* xwos/cxx/lock/SMtx.hxx */
