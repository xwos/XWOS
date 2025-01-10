/**
 * @file
 * @brief xwos::sync::Br
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef __xwos_cxx_sync_Br_hxx__
#define __xwos_cxx_sync_Br_hxx__

#include <xwos/cxx/Bmp.hxx>
#include <xwos/cxx/sync/Sel.hxx>

extern "C" {
#include <xwos/osal/sync/br.h>
}

namespace xwos {
namespace sync {

/**
 * @defgroup xwos_cxx_sync_Br 线程栅栏
 * @ingroup xwos_cxx_sync
 *
 * ## 获取事件的状态
 *
 * + `Br::num()` ：获取线程栅栏中线程槽数量
 *
 * ## 同步线程
 *
 * + `Br::wait()` ：等待所有线程到达栅栏
 *
 * ## 使用信号选择器选择条件量
 *
 * + `Br::bind()` ：绑定到 [信号选择器](group__xwos__cxx__sync__Sel.html) 上
 * + `Br::unbind()` ：从 [信号选择器](group__xwos__cxx__sync__Sel.html) 上解绑
 *
 * ## 头文件
 *
 * @ref xwos/cxx/sync/Br.hxx
 *
 * @{
 */

/**
 * @brief 线程栅栏基类
 */
template<xwsz_t TNum>
class Br
{
  public:
    /**
     * @brief 等待模式枚举
     */
    enum WaitMode : xwu32_t {
        BrWait = 0, /**< 等待模式 */
        BrWaitTimed, /**< 定时等待模式 */
        BrWaitUninterruptable, /**< 不可中断等待模式 */
    };

  protected:
    struct xwos_br * mBrPtr;

  protected:
    Br() : mBrPtr(nullptr) {}
    ~Br() { mBrPtr = nullptr; }

  public:
    /**
     * @brief 获取事件的最大数量
     */
    xwsz_t num() { return TNum; }

    /**
     * @brief 等待所有线程到达栅栏
     * @param[in] mode: 等待模式，取值 @ref Br::WaitMode
     *   @arg Br::WaitMode::BrWait
     *   @arg Br::WaitMode::BrWaitTimed
     *   @arg Br::WaitMode::BrWaitUninterruptable
     * @param[in] to: 期望唤醒的时间点，仅当 `mode == Br::WaitMode::BrWaitTimed` 时有效。
     * @return 错误码
     * @retval XWOK: 没有错误
     * @retval -EFAULT: 无效的指针或空指针
     * @retval -ECHRNG: 线程数量超出范围
     * @retval -ETIMEDOUT: 超时
     * @retval -EINTR: 等待被中断
     * @retval -ENOTTHDCTX：不在线程上下文中
     * @note
     * + 上下文：线程
     * @details
     * + 等待分为下列几种模式：
     *   + `Br::WaitMode::BrWait`: 调用线程会阻塞等待。
     *   + `Br::WaitMode::BrWaitTimed` 定时等待模式， `to` 表示等待超时的时间点：
     *     + `to` 通常是未来时间点，即 **当前系统时间** + `delta` ，
     *       可以使用 `xwtm_ft(delta)` 表示；
     *     + 如果 `to` 是过去的时间点，将直接返回 `-ETIMEDOUT` 。
     *   + `Br::WaitMode::BrWaitUninterruptable` 不可中断等待模式。
     */
    xwer_t wait(enum WaitMode mode, xwtm_t to)
    {
        xwer_t rc;
        switch (mode) {
            case WaitMode::BrWait:
                rc = xwos_br_wait(mBrPtr);
                break;
            case WaitMode::BrWaitTimed:
                rc = xwos_br_wait_to(mBrPtr, to);
                break;
            case WaitMode::BrWaitUninterruptable:
                rc = xwos_br_wait_unintr(mBrPtr);
                break;
            default:
                rc = xwos_br_wait(mBrPtr);
                break;
        }
        return rc;
    }

    /**
     * @brief 绑定线程栅栏对象到信号选择器
     * @param[in] sel: 信号选择器的指针
     * @param[in] pos: 线程栅栏对象映射到信号选择器位图中的位置
     * @return 错误码
     * @retval XWOK: 没有错误
     * @retval -EFAULT: 无效的指针或空指针
     * @retval -ECHRNG: 位置超出范围
     * @retval -EALREADY: 同步对象已经绑定到事件对象
     * @retval -EBUSY: 通道已经被其他同步对象独占
     * @note
     * + 上下文：任意
     * + 绑定方式：非独占绑定
     * @details
     * + 当所有线程到达线程栅栏时，线程栅栏将向信号选择器发送信号。
     */
    template<xwsz_t TSelNum>
    xwer_t bind(Sel<TSelNum> * sel, long pos)
    {
        return xwos_br_bind(mBrPtr, sel->getXwosObj(), (xwsq_t)pos);
    }

    /**
     * @brief 绑定线程栅栏对象到信号选择器
     * @param[in] sel: 信号选择器的引用
     * @param[in] pos: 线程栅栏对象映射到信号选择器位图中的位置
     * @return 错误码
     * @retval XWOK: 没有错误
     * @retval -EFAULT: 无效的指针或空指针
     * @retval -ECHRNG: 位置超出范围
     * @retval -EALREADY: 同步对象已经绑定到事件对象
     * @retval -EBUSY: 通道已经被其他同步对象独占
     * @note
     * + 上下文：任意
     * + 绑定方式：非独占绑定
     * @details
     * + 当所有线程到达线程栅栏时，线程栅栏将向信号选择器发送信号。
     */
    template<xwsz_t TSelNum>
    xwer_t bind(Sel<TSelNum> & sel, long pos)
    {
        return xwos_br_bind(mBrPtr, sel.getXwosObj(), (xwsq_t)pos);
    }

    /**
     * @brief 从信号选择器上解绑线程栅栏对象
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
        return xwos_br_unbind(mBrPtr, sel->getXwosObj());
    }

    /**
     * @brief 从信号选择器上解绑线程栅栏对象
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
        return xwos_br_unbind(mBrPtr, sel.getXwosObj());
    }

    /**
     * @brief 获取XWOS对象指针
     */
    struct xwos_br * getXwosObj() { return mBrPtr; }
};

/**
 * @} xwos_cxx_sync_Br
 */

} // namespace sync
} // namespace xwos

#endif /* xwos/cxx/sync/Br.hxx */
