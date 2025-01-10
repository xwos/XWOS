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
#include <xwos/cxx/sync/Sel.hxx>

namespace xwos {
namespace sync {

/**
 * @defgroup xwos_cxx_sync_Sem 信号量
 * @ingroup xwos_cxx_sync
 *
 * ## 发布信号量
 *
 * + `Sem::post()` ：发布信号量
 *
 * ## 等待信号量
 *
 * + `Sem::wait()` ：等待信号量
 *
 * ## 获取信号量的状态
 *
 * + `Sem::max()` ：获取信号量中计数器的最大值
 * + `Sem::value()` ：获取信号量中计数器的值
 *
 * ## 冻结与解冻信号量
 *
 * + `Sem::freeze()` ：冻结
 * + `Sem::thaw()` ：解冻
 *
 * ## 使用信号选择器选择信号量
 *
 * + `Sem::bind()` ：绑定到 [信号选择器](group__xwos__cxx__sync__Sel.html) 上
 * + `Sem::unbind()` ：从 [信号选择器](group__xwos__cxx__sync__Sel.html) 上解绑
 *
 * ## 头文件
 *
 * @ref xwos/cxx/sync/Sem.hxx
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
    Sem() : mSemPtr(nullptr) {}
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
     * + 此C++API只对未冻结的信号量对象起作用，对已冻结的信号量对象调用此C++API将
     *   返回错误码 `-ENEGATIVE` 。
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
    xwer_t wait(enum WaitMode mode = WaitMode::SemWait, xwtm_t to = XWTM_MAX)
    {
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
    xwssq_t max()
    {
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
    xwssq_t value()
    {
        xwssq_t value;
        xwos_sem_get_value(mSemPtr, &value);
        return value;
    }

    /**
     * @brief 绑定信号量对象到信号选择器
     * @param[in] sel: 信号选择器的指针
     * @param[in] pos: 信号量对象映射到信号选择器位图中的位置
     * @return 错误码
     * @retval XWOK: 没有错误
     * @retval -EFAULT: 空指针
     * @retval -ECHRNG: 位置超出范围
     * @retval -EALREADY: 同步对象已经绑定到事件对象
     * @retval -EBUSY: 通道已经被其他同步对象独占
     * @note
     * + 上下文：任意
     * + 绑定方式：独占绑定
     */
    template<xwsz_t TSelNum>
    xwer_t bind(Sel<TSelNum> * sel, long pos)
    {
        return xwos_sem_bind(mSemPtr, sel->getXwosObj(), (xwsq_t)pos);
    }

    /**
     * @brief 绑定信号量对象到信号选择器
     * @param[in] sel: 信号选择器的引用
     * @param[in] pos: 信号量对象映射到信号选择器位图中的位置
     * @return 错误码
     * @retval XWOK: 没有错误
     * @retval -EFAULT: 空指针
     * @retval -ECHRNG: 位置超出范围
     * @retval -EALREADY: 同步对象已经绑定到事件对象
     * @retval -EBUSY: 通道已经被其他同步对象独占
     * @note
     * + 上下文：任意
     * + 绑定方式：独占绑定
     */
    template<xwsz_t TSelNum>
    xwer_t bind(Sel<TSelNum> & sel, long pos)
    {
        return xwos_sem_bind(mSemPtr, sel.getXwosObj(), (xwsq_t)pos);
    }

    /**
     * @brief 从信号选择器上解绑信号量对象
     * @param[in] sel: 信号选择器的引用
     * @return 错误码
     * @retval XWOK: 没有错误
     * @retval -EFAULT: 空指针
     * @retval -ENOTCONN: 同步对象没有绑定到事件对象上
     * @note
     * + 上下文：任意
     */
    template<xwsz_t TSelNum>
    xwer_t unbind(Sel<TSelNum> * sel)
    {
        return xwos_sem_unbind(mSemPtr, sel->getXwosObj());
    }

    /**
     * @brief 从信号选择器上解绑信号量对象
     * @param[in] sel: 信号选择器的引用
     * @return 错误码
     * @retval XWOK: 没有错误
     * @retval -EFAULT: 空指针
     * @retval -ENOTCONN: 同步对象没有绑定到事件对象上
     * @note
     * + 上下文：任意
     */
    template<xwsz_t TSelNum>
    xwer_t unbind(Sel<TSelNum> & sel)
    {
        return xwos_sem_unbind(mSemPtr, sel.getXwosObj());
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
