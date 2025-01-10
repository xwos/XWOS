/**
 * @file
 * @brief xwos::lock::Mtx
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef __xwos_cxx_lock_Mtx_hxx__
#define __xwos_cxx_lock_Mtx_hxx__

#include <xwos/osal/lock/mtx.hxx>
#include <xwos/cxx/sync/Cond.hxx>

namespace xwos {
namespace lock {

/**
 * @defgroup xwos_cxx_lock_Mtx 互斥锁基类
 * @ingroup xwos_cxx_lock
 *
 *
 * ## C++ API
 *
 * 头文件： @ref xwos/cxx/lock/Mtx.hxx
 *
 * @{
 */

/**
 * @brief 互斥锁基类
 */
class Mtx
{
  public:
    /**
     * @brief 互斥锁的锁模式
     */
    enum LockMode : xwu32_t {
        MtxUnlock = 0, /**< 未上锁 */
        MtxLock, /**< 如果无法上锁，调用线程会阻塞等待上锁。*/
        MtxLockTimed, /**< 定时等待模式，如果无法上锁，
                           调用线程会定时阻塞等待上锁。*/
        MtxLockUninterruptable, /**< 不可中断等待模式，如果无法上锁，
                                     调用线程会阻塞等待上锁，且不可被中断。*/
        MtxLockTry, /**< 尝试上锁模式，如果无法上锁，
                         调用线程不会阻塞等待，尝试失败立即返回。*/
    };

    /**
     * @brief 互斥锁的RAII机制守卫
     */
    class Grd
    {
      private:
        Mtx * mMtx;
        xwer_t mRc; /**< 内部上锁CAPI的返回值 */
        enum LockMode mLockMode;

      public:
        Grd() = delete;

        /**
         * @brief 构造互斥锁的RAII机制守卫并上锁互斥锁
         * @param[in] mtx: 互斥锁对象指针
         * @param[in] mode: 锁模式，取值 @ref Mtx::LockMode
         *   @arg Mtx::LockMode::MtxLock
         *   @arg Mtx::LockMode::MtxLockTimed
         *   @arg Mtx::LockMode::MtxLockUninterruptable
         *   @arg Mtx::LockMode::MtxLockTry
         * @param[in] to: 期望唤醒的时间点，
         *                仅当 `mode == Mtx::LockMode::MtxLockTimed` 时有效。
         * @note
         * + 上下文：线程
         * @details
         * + 上锁分为下列几种模式：
         *   + `Mtx::LockMode::MtxunLock` 未上锁。
         *   + `Mtx::LockMode::MtxLock` 如果无法上锁，调用线程会阻塞等待上锁。
         *   + `Mtx::LockMode::MtxLockTimed` 定时等待模式，如果无法上锁，
         *     调用线程会定时阻塞等待上锁。
         *   + `Mtx::LockMode::MtxLockUninterruptable` 不可中断等待模式，
         *     如果无法上锁，调用线程会阻塞等待上锁，且不可被中断。
         *   + `Mtx::LockMode::MtxLockTry` 尝试上锁模式，如果无法上锁，
         *     调用线程不会阻塞等待，尝试失败立即返回。
         * + 此C++API返回的是 `Mtx::Grd` 的对象。 `Mtx::Grd` 是
         *   互斥锁的RAII机制守卫。
         * + 如果上锁成功( `Mtx::Grd` 内部的 `mRc == XWOK` )， `Mtx::Grd`
         *   对象析构时会自动解锁互斥锁。
         * + 如果上锁失败，错误码存放在 `Mtx::Grd` 内部的 `mRc` ，
         *   可通过 `Mtx::Grd::getRc()` 获取。
         *   此时 `Mtx::Grd` 析构时不会解锁互斥锁。
         * + 当 `mode` 为 `Mtx::LockMode::MtxLock` 或
         *   `Mtx::LockMode::MtxLockTimed` ,等待被中断时， `mRc == -EINTR` 。
         * + 当 `mode` 为 `Mtx::LockMode::MtxLockTimed` ,等待超时时，
         *   `mRc == -ETIMEDOUT` 。
         * + 当 `mode` 为 `Mtx::LockMode::MtxLockTry` ,上锁失败时，
         *   `mRc == -EWOULDBLOCK` 。
         */
        Grd(Mtx * mtx, enum LockMode mode = LockMode::MtxLock, xwtm_t to = XWTM_MAX);

        /**
         * @brief 构造互斥锁的RAII机制守卫并上锁互斥锁
         * @param[in] mode: 锁模式，取值 @ref Mtx::LockMode
         *   @arg Mtx::LockMode::MtxLock
         *   @arg Mtx::LockMode::MtxLockTimed
         *   @arg Mtx::LockMode::MtxLockUninterruptable
         *   @arg Mtx::LockMode::MtxLockTry
         * @param[in] to: 期望唤醒的时间点，
         *                仅当 `mode == Mtx::LockMode::MtxLockTimed` 时有效。
         */
        Grd(Mtx & mtx, enum LockMode mode = LockMode::MtxLock, xwtm_t to = XWTM_MAX);

        /**
         * @brief 析构互斥锁的RAII机制守卫并解锁互斥锁
         */
        ~Grd();

        /**
         * @brief 解锁互斥锁
         * @details
         * + 主动解锁后，析构函数将不再解锁。
         */
        void unlock();

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
         * @brief 获取上锁模式
         */
        enum LockMode getLockMode() { return mLockMode; }

        /**
         * @brief 等待条件量
         * @param[in] cond: 条件量对象的指针
         * @return 错误码
         * @retval XWOK: 没有错误
         * @retval -EFAULT: 无效的指针或空指针
         * @retval -EINVAL: 参数无效
         * @retval -ENOTTHDCTX: 不在线程上下文中
         * @retval -ENOLCK: 未上锁
         * @retval -EINTR: 等待被中断
         * @note
         * + 上下文：线程
         * @details
         * + 调用此C++API的线程会阻塞等待条件量，等待的同时会解锁互斥锁。
         * + 如果构造 `Mtx::Grd` 时使用的模式是
         *   `Mtx::LockMode::MtxLockUninterruptable` ，那么等待模式也是不可中断的。
         * + 条件量被单播 `sync::Cond::unicast()` 或广播 `sync::Cond::broadcast()`
         *   时，会唤醒正在等待的线程。
         * + 线程被唤醒后，会重新上锁互斥锁，如果构造 `Mtx::Grd` 时使用的模式是
         *   `Mtx::LockMode::MtxLockUninterruptable` ，那么重新上锁的模式
         *   也是 `Mtx::LockMode::MtxLockUninterruptable` 。
         * + 重新上锁成功后将返回 `XWOK` 。
         * + 线程的等待被中断后，此C++API返回 `-EINTR` 。
         * + 如果此C++API返回 **非** `XWOK` 的错误码，应该
         *   使用 `Mtx::Grd::getLockMode()` 确认是否上锁成功。
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
         * @retval -ENOTTHDCTX: 不在线程上下文中
         * @retval -ENOLCK: 未上锁
         * @retval -EINTR: 等待被中断
         * @retval -ETIMEDOUT: 超时
         * @note
         * + 上下文：线程
         * @details
         * + 调用此C++API的线程会限时阻塞等待条件量，等待的同时会解锁互斥锁。
         * + 如果构造 `Mtx::Grd` 时使用的模式是
         *   `Mtx::LockMode::MtxLockUninterruptable` ，那么等待模式也是不可中断的，
         *   此时参数 `to` 无效，等待将不会超时。
         * + 条件量被单播 `sync::Cond::unicast()` 或广播 `sync::Cond::broadcast()`
         *   时，会唤醒正在等待的线程。
         * + 线程被唤醒后，会重新上锁互斥锁，如果构造 `Mtx::Grd` 时使用的模式是
         *   `Mtx::LockMode::MtxLockUninterruptable` ，那么重新上锁的模式
         *   也是 `Mtx::LockMode::MtxLockUninterruptable` 。
         * + 如果构造 `Mtx::Grd` 时使用的模式 **不是**
         *   `Mtx::LockMode::MtxLockUninterruptable` ，线程被唤醒时，若互斥锁被
         *   其他线程占用，线程也最多等待互斥锁到时间点 `to` 。
         * + 重新上锁成功后将返回 `XWOK` 。
         * + 线程的等待被中断后，此C++API返回 `-EINTR` 。
         * + 线程的等待超时后，此C++API返回 `-ETIMEDOUT` 。
         * + 如果此C++API返回 **非** `XWOK` 的错误码，应该
         *   使用 `Mtx::Grd::getLockMode()` 确认是否上锁成功。
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

  protected:
    struct xwos_mtx * mMtxPtr;

  protected:
    Mtx()
        : mMtxPtr(nullptr)
    {
    }
    ~Mtx() { mMtxPtr = nullptr; }

  public:
    /**
     * @brief 获取XWOS对象指针
     */
    struct xwos_mtx * getXwosObj() { return mMtxPtr; }
};

/**
 * @} xwos_cxx_lock_Mtx
 */

} // namespace lock
} // namespace xwos

#endif /* xwos/cxx/lock/Mtx.hxx */
