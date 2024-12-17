/**
 * @file
 * @brief xwos::lock::DMtx
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef __xwos_cxx_lock_DMtx_hxx__
#define __xwos_cxx_lock_DMtx_hxx__

#include <xwos/osal/lock/mtx.hxx>
#include <xwos/cxx/sync/Cond.hxx>

namespace xwos {
namespace lock {

/**
 * @defgroup xwos_cxx_lock_DMtx 动态互斥锁
 * @ingroup xwos_cxx_lock
 *
 * 动态互斥锁是指互斥锁所需要的内存在编译期由编译器分配。
 *
 *
 * ## 动态互斥锁的RAII机制守卫
 *
 * `DMtx::Grd` 构造时会上锁互斥锁，析构时会自动解锁互斥锁。
 * `DMtx::Grd` 上锁模式分为：
 * + `DMtx::LockMode::DMtxUnlock` 未上锁。
 * + `DMtx::LockMode::DMtxLock` 如果无法上锁，调用线程会阻塞等待上锁。
 * + `DMtx::LockMode::DMtxLockTimed` 定时等待模式，如果无法上锁，
 *   调用线程会定时阻塞等待上锁。
 * + `DMtx::LockMode::DMtxLockUninterruptable` 不可中断等待模式，
 *   如果无法上锁，调用线程会阻塞等待上锁，且不可被中断。
 * + `DMtx::LockMode::DMtxLockTry` 尝试上锁模式，如果无法上锁，
 *   调用线程不会阻塞等待，尝试失败立即返回。
 *
 * ### 示例
 *
 * ```Cpp
 * #include <xwos/cxx/lock/DMtx.hxx>
 *
 * void foo()
 * {
 *     xwos::lock::DMtx * dlock = new xwos::lock::DMtx(); // 创建动态互斥锁
 *     { // 构造 `grd` 时会自动对互斥锁上锁
 *         xwos::lock::DMtx::Grd grd(dlock, xwos::lock::DMtx::LockMode::DMtxLock);
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
 * 头文件： @ref xwos/cxx/lock/DMtx.hxx
 *
 * @{
 */

/**
 * @brief 动态互斥锁
 */
class DMtx
{
  public:
    /**
     * @brief 动态互斥锁的锁模式
     */
    enum LockMode : xwu32_t {
        DMtxUnlock = 0, /**< 未上锁 */
        DMtxLock, /**< 如果无法上锁，调用线程会阻塞等待上锁。*/
        DMtxLockTimed, /**< 定时等待模式，如果无法上锁，
                            调用线程会定时阻塞等待上锁。*/
        DMtxLockUninterruptable, /**< 不可中断等待模式，如果无法上锁，
                                      调用线程会阻塞等待上锁，且不可被中断。*/
        DMtxLockTry, /**< 尝试上锁模式，如果无法上锁，
                          调用线程不会阻塞等待，尝试失败立即返回。*/
    };

    /**
     * @brief 动态互斥锁的RAII机制守卫
     */
    class Grd
    {
      private:
        DMtx * mMtx;
        xwer_t mRc; /**< 内部上锁CAPI的返回值 */
        enum LockMode mLockMode;

      public:
        Grd() = delete;

        /**
         * @brief 构造动态互斥锁的RAII机制守卫并上锁互斥锁
         * @param[in] mtx: 动态互斥锁对象指针
         * @param[in] mode: 锁模式，取值 @ref DMtx::LockMode
         *   @arg DMtx::LockMode::DMtxLock
         *   @arg DMtx::LockMode::DMtxLockTimed
         *   @arg DMtx::LockMode::DMtxLockUninterruptable
         *   @arg DMtx::LockMode::DMtxLockTry
         * @param[in] to: 期望唤醒的时间点，
         *                仅当 `mode == DMtx::LockMode::DMtxLockTimed` 时有效。
         * @note
         * + 上下文：线程
         * @details
         * + 上锁分为下列几种模式：
         *   + `DMtx::LockMode::DMtxunLock` 未上锁。
         *   + `DMtx::LockMode::DMtxLock` 如果无法上锁，调用线程会阻塞等待上锁。
         *   + `DMtx::LockMode::DMtxLockTimed` 定时等待模式，如果无法上锁，
         *     调用线程会定时阻塞等待上锁。
         *   + `DMtx::LockMode::DMtxLockUninterruptable` 不可中断等待模式，
         *     如果无法上锁，调用线程会阻塞等待上锁，且不可被中断。
         *   + `DMtx::LockMode::DMtxLockTry` 尝试上锁模式，如果无法上锁，
         *     调用线程不会阻塞等待，尝试失败立即返回。
         * + 此C++API返回的是 `DMtx::Grd` 的对象。 `DMtx::Grd` 是
         *   动态互斥锁的RAII机制守卫。
         * + 如果上锁成功( `DMtx::Grd` 内部的 `mRc == XWOK` )， `DMtx::Grd`
         *   对象析构时会自动解锁互斥锁。
         * + 如果上锁失败，错误码存放在 `DMtx::Grd` 内部的 `mRc` ，
         *   可通过 `DMtx::Grd::getRc()` 获取。
         *   此时 `DMtx::Grd` 析构时不会解锁互斥锁。
         * + 当 `mode` 为 `DMtx::LockMode::DMtxLock` 或
         *   `DMtx::LockMode::DMtxLockTimed` ,等待被中断时， `mRc == -EINTR` 。
         * + 当 `mode` 为 `DMtx::LockMode::DMtxLockTimed` ,等待超时时，
         *   `mRc == -ETIMEDOUT` 。
         * + 当 `mode` 为 `DMtx::LockMode::DMtxLockTry` ,上锁失败时，
         *   `mRc == -EWOULDBLOCK` 。
         */
        Grd(DMtx * mtx, enum LockMode mode = LockMode::DMtxLock, xwtm_t to = XWTM_MAX);

        /**
         * @brief 构造动态互斥锁的RAII机制守卫并上锁互斥锁
         * @param[in] mode: 锁模式，取值 @ref DMtx::LockMode
         *   @arg DMtx::LockMode::DMtxLock
         *   @arg DMtx::LockMode::DMtxLockTimed
         *   @arg DMtx::LockMode::DMtxLockUninterruptable
         *   @arg DMtx::LockMode::DMtxLockTry
         * @param[in] to: 期望唤醒的时间点，
         *                仅当 `mode == DMtx::LockMode::DMtxLockTimed` 时有效。
         */
        Grd(DMtx & mtx, enum LockMode mode = LockMode::DMtxLock, xwtm_t to = XWTM_MAX);


        /**
         * @brief 析构动态互斥锁的RAII机制守卫并解锁互斥锁
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

        /**
         * @brief 等待条件量
         * @param[in] cond: 条件量对象的指针
         * @return 错误码
         * @retval XWOK: 没有错误
         * @retval -EFAULT: 无效的指针或空指针
         * @retval -EINVAL: 参数无效
         * @retval -EINTR: 等待被中断
         * @retval -ENOTTHDCTX: 不在线程上下文中
         * @note
         * + 上下文：线程
         * @details
         * + 调用此C++API的线程会阻塞等待条件量，等待的同时会解锁互斥锁。
         * + 如果构造 `DMtx::Grd` 时使用的模式是
         *   `DMtx::LockMode::DMtxLockUninterruptable` ，那么等待模式也是不可中断的。
         * + 条件量被单播 `sync::Cond::unicast()` 或广播 `sync::Cond::broadcast()`
         *   时，会唤醒正在等待的线程。
         * + 线程被唤醒后，会重新上锁互斥锁，如果构造 `DMtx::Grd` 时使用的模式是
         *   `DMtx::LockMode::DMtxLockUninterruptable` ，那么重新上锁的模式
         *   也是 `DMtx::LockMode::DMtxLockUninterruptable` 。
         * + 重新上锁成功后将返回 `XWOK` 。
         * + 线程的等待被中断后，此C++API返回 `-EINTR` 。
         * + 如果此C++API返回 **非** `XWOK` 的错误码，应该
         *   使用 `DMtx::Grd::getLockMode()` 确认是否上锁成功。
         */
        xwer_t wait(sync::Cond * cond);

        /**
         * @brief 等待条件量
         * @param[in] cond: 条件量对象的引用
         * @return 错误码
         * @note
         * + 上下文：线程
         * @details
         * + 同 `wait(sync::Cond * cond)` 。
         */
        xwer_t wait(sync::Cond & cond) { return wait(&cond); }

        /**
         * @brief 限时等待条件量
         * @param[in] cond: 条件量对象的指针
         * @param[in] to: 期望唤醒的时间点
         * @return 错误码
         * @retval XWOK: 没有错误
         * @retval -EFAULT: 无效的指针或空指针
         * @retval -EINVAL: 参数无效
         * @retval -ETIMEDOUT: 超时
         * @retval -EINTR: 等待被中断
         * @retval -ENOTTHDCTX: 不在线程上下文中
         * @note
         * + 上下文：线程
         * @details
         * + 调用此C++API的线程会限时阻塞等待条件量，等待的同时会解锁互斥锁。
         * + 如果构造 `DMtx::Grd` 时使用的模式是
         *   `DMtx::LockMode::DMtxLockUninterruptable` ，那么等待模式也是不可中断的，
         *   此时参数 `to` 无效，等待将不会超时。
         * + 条件量被单播 `sync::Cond::unicast()` 或广播 `sync::Cond::broadcast()`
         *   时，会唤醒正在等待的线程。
         * + 线程被唤醒后，会重新上锁互斥锁，如果构造 `DMtx::Grd` 时使用的模式是
         *   `DMtx::LockMode::DMtxLockUninterruptable` ，那么重新上锁的模式
         *   也是 `DMtx::LockMode::DMtxLockUninterruptable` 。
         * + 如果构造 `DMtx::Grd` 时使用的模式 **不是**
         *   `DMtx::LockMode::DMtxLockUninterruptable` ，线程被唤醒时，若互斥锁被
         *   其他线程占用，线程也最多等待互斥锁到时间点 `to` 。
         * + 重新上锁成功后将返回 `XWOK` 。
         * + 线程的等待被中断后，此C++API返回 `-EINTR` 。
         * + 线程的等待超时后，此C++API返回 `-ETIMEDOUT` 。
         * + 如果此C++API返回 **非** `XWOK` 的错误码，应该
         *   使用 `DMtx::Grd::getLockMode()` 确认是否上锁成功。
         */
        xwer_t wait(sync::Cond * cond, xwtm_t to);

        /**
         * @brief 限时等待条件量
         * @param[in] cond: 条件量对象的引用
         * @param[in] to: 期望唤醒的时间点
         * @return 错误码
         * @note
         * + 上下文：线程
         * @details
         * + 同 `wait(sync::Cond * cond, xwtm_t to)` 。
         */
        xwer_t wait(sync::Cond & cond, xwtm_t to) { return wait(&cond, to); }
    };

  private:
    xwos_mtx_d mMtxDesc; /**< 互斥锁对象描述符 */
    xwer_t mCtorRc; /**< 互斥锁构造的结果 */

  public:
    /**
     * @brief 构造函数
     * @param[in] pr: 互斥锁的天花板优先级
     */
    explicit DMtx(xwpr_t pr = XWOS_SKD_PRIORITY_RT_MAX);
    ~DMtx(); /**< 析构函数 */
    xwer_t getCtorRc() { return mCtorRc; } /**< 获取动态互斥锁构造的结果 */
    struct xwos_mtx * getXwosObj() { return mMtxDesc.mtx; } /**< 获取XWOS对象指针 */

    /* 生命周期管理 */
    xwer_t acquire() { return xwos_mtx_acquire(mMtxDesc); } /**< 增加引用计数 */
    xwer_t release() { return xwos_mtx_release(mMtxDesc); } /**< 减少引用计数 */
};

/**
 * @} xwos_cxx_lock_DMtx
 */

} // namespace lock
} // namespace xwos

#endif /* xwos/cxx/lock/DMtx.hxx */
