/**
 * @file
 * @brief xwos::sync::Sem
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef __xwos_cxx_sync_Sem_hxx__
#define __xwos_cxx_sync_Sem_hxx__

#include <xwos/osal/sync/sem.hxx>

namespace xwos {
namespace sync {

/**
 * @defgroup xwos_cxx_sync_Sem 信号量基类
 * @ingroup xwos_cxx_sync
 *
 * ## C++ API
 *
 * 头文件： @ref xwos/cxx/sync/Sem.hxx
 *
 * @{
 */

/**
 * @brief 信号量基类
 */
class Sem
{
  public:
    /**
     * @brief 静态信号量的等待模式
     */
    enum WaitMode : xwu32_t {
        SemWait = 0, /**< 等待模式 */
        SemWaitTimed, /**< 定时等待模式 */
        SemWaitUninterruptable, /**< 不可中断等待模式 */
        SemTryWait, /**< 尝试模式（只测试不等待） */
    };

  protected:
    struct xwos_sem * mSemPtr;
  protected:
    explicit Sem() : mSemPtr(nullptr) {}
    ~Sem() { mSemPtr = nullptr; }

  public:
    /**
     * @brief 发布信号量
     * @return 错误码
     * @retval XWOK: 没有错误
     * @retval -EFAULT: 空指针
     * @retval -ENEGATIVE: 信号量对象已被冻结
     * @retval -ERANGE: 信号量对象的值已经最大
     * @note
     * + 上下文：任意
     * @details
     * + 此C++API只对未冻结的信号量对象起作用，已冻结的信号量对象将得到错误码 `-ENEGATIVE` 。
     */
    xwer_t post() { return xwos_sem_post(mSemPtr); }

    /**
     * @brief 等待并获取信号量
     * @param[in] mode: 锁模式，取值 @ref Sem::WaitMode
     *   @arg Sem::WaitMode::SemWait
     *   @arg Sem::WaitMode::SemWaitTimed
     *   @arg Sem::WaitMode::SemWaitUninterruptable
     *   @arg Sem::WaitMode::SemTryWait
     * @param[in] to: 期望唤醒的时间点，
     *                仅当 `mode == Sem::WaitMode::SemWaitTimed` 时有效。
     * @return 错误码
     * @retval XWOK: 没有错误
     * @retval -EFAULT: 空指针
     * @retval -ETIMEDOUT: 超时
     * @retval -EINTR: 等待被中断
     * @retval -ENOTTHDCTX: 不在线程上下文中
     * @retval -EDSPMPT: 抢占被关闭
     * @retval -EDSBH: 中断底半部被关闭
     * @note
     * + 上下文：
     *   + `mode == Sem::WaitMode::SemTryWait` : 任意
     *   + `mode != Sem::WaitMode::SemTryWait` : 线程
     * @details
     * + 等待分为下列几种模式：
     *   + `Sem::WaitMode::SemWait`: 调用线程会阻塞等待上锁。
     *   + `Sem::WaitMode::SemWaitTimed` 定时等待模式， `to` 表示等待超时的时间点：
     *     + `to` 通常是未来时间点，即 **当前系统时间** + `delta` ，
     *       可以使用 `xwtm_ft(delta)` 表示；
     *     + 如果 `to` 是过去的时间点，将直接返回 `-ETIMEDOUT` 。
     *   + `Sem::WaitMode::SemWaitUninterruptable` 不可中断等待模式。
     *   + `Sem::WaitMode::SemTryWait` 尝试模式（只测试不等待）。
     */
    xwer_t wait(enum WaitMode mode = WaitMode::SemWait, xwtm_t to = XWTM_MAX) {
        xwer_t rc;
        switch (mode) {
            case WaitMode::SemWait:
                rc = xwos_sem_wait(mSemPtr);
                break;
            case WaitMode::SemWaitTimed:
                rc = xwos_sem_wait_to(mSemPtr, to);
                break;
            case WaitMode::SemWaitUninterruptable:
                rc = xwos_sem_wait_unintr(mSemPtr);
                break;
            case WaitMode::SemTryWait:
                rc = xwos_sem_trywait(mSemPtr);
                break;
            default:
                rc = xwos_sem_wait(mSemPtr);
                break;
        }
        return rc;
    }

    /**
     * @brief 冻结信号量
     * @return 错误码
     * @retval XWOK: 没有错误
     * @retval -EFAULT: 空指针
     * @retval -EALREADY: 信号量对象已被冻结
     * @note
     * + 上下文：任意
     * @details
     * + 被冻结的信号量对象不允许增加(V操作)，但可以被测试(P操作)。
     *   测试信号量对象的线程会被加入到信号量对象的等待队列。
     * + 对已经冻结的信号量再次冻结，将返回 `-EALREADY` 。
     */
    xwer_t freeze() { return xwos_sem_freeze(mSemPtr); }

    /**
     * @brief 解冻信号量
     * @return 错误码
     * @retval XWOK: 没有错误
     * @retval -EFAULT: 无效的指针或空指针
     * @retval -EALREADY: 信号量对象未被冻结
     * @note
     * + 上下文：任意
     * @details
     * + 此C++API只对已冻结的信号量对象起作用，
     *   对未冻结的信号量对象调用此函数将返回错误码 `-EALREADY` 。
     */
    xwer_t thaw() { return xwos_sem_thaw(mSemPtr); }

    /**
     * @brief 获取信号量计数器的最大值
     * @return 最大值
     * @note
     * + 上下文：任意
     */
    xwssq_t max() {
        xwssq_t max;
        xwos_sem_get_max(mSemPtr, &max);
        return max;
    }

    /**
     * @brief 获取信号量计数器的值
     * @return 最大值
     * @note
     * + 上下文：任意
     */
    xwssq_t value() {
        xwssq_t value;
        xwos_sem_get_value(mSemPtr, &value);
        return value;
    }

    /**
     * @brief 获取XWOS对象指针
     */
    struct xwos_sem * getXwosObj() { return mSemPtr; }
};

/**
 * @} xwos_cxx_sync_Sem
 */

} // namespace sync
} // namespace xwos

#endif /* xwos/cxx/sync/Sem.hxx */
