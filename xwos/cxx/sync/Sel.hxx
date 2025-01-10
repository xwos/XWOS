/**
 * @file
 * @brief xwos::sync::Sel
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef __xwos_cxx_sync_Sel_hxx__
#define __xwos_cxx_sync_Sel_hxx__

#include <xwos/cxx/Bmp.hxx>

extern "C" {
#include <xwos/osal/sync/sel.h>
}

namespace xwos {
namespace sync {

/**
 * @defgroup xwos_cxx_sync_Sel 信号选择器
 * @ingroup xwos_cxx_sync
 *
 *
 * ## 获取事件的状态
 *
 * + `Sel::num()` ：获取信号选择器中的信号槽的数量
 *
 * ## 选择信号
 *
 * + `Sel::select()` ：等待信号选择器
 *
 * ## 信号选择器的级联
 *
 * + `Sel::bind()` ：绑定另一个到信号选择器上
 * + `Sel::unbind()` ：从另一个信号选择器上解绑
 *
 * ## 头文件
 *
 * @ref xwos/cxx/sync/Sel.hxx
 *
 * @{
 */

/**
 * @brief 信号选择器基类
 */
template<xwsz_t TNum>
class Sel
{
  public:
    /**
     * @brief 等待模式枚举
     */
    enum WaitMode : xwu32_t {
        SelWait = 0, /**< 等待模式 */
        SelWaitTimed, /**< 定时等待模式 */
        SelWaitUninterruptable, /**< 不可中断等待模式 */
        SelTryWait, /**< 尝试模式（只测试不等待） */
    };

  protected:
    struct xwos_sel * mSelPtr;

  protected:
    Sel()
        : mSelPtr(nullptr)
    {
    }
    ~Sel() { mSelPtr = nullptr; }

  public:
    /**
     * @brief 获取信号选择器中的信号槽的数量
     */
    xwsz_t num() { return TNum; }

    /**
     * @brief 等待信号选择器
     * @param[in] msk: 同步对象位图掩码，表示只关注掩码内的同步对象
     * @param[out] trigger: 指向缓冲区的指针，通过此缓冲区返回已触发的同步对象位图掩码
     * @param[in] mode: 等待模式，取值 @ref Sel::WaitMode
     *   @arg Sel::WaitMode::SelWait
     *   @arg Sel::WaitMode::SelWaitTimed
     *   @arg Sel::WaitMode::SelWaitUninterruptable
     *   @arg Sel::WaitMode::SelTryWait
     * @param[in] to: 期望唤醒的时间点，仅当 `mode == Sel::WaitMode::SelWaitTimed` 时有效。
     * @return 错误码
     * @retval XWOK: 没有错误
     * @retval -EFAULT: 空指针
     * @retval -ETIMEDOUT: 超时
     * @retval -ENODATA: 没有任何同步对象向信号选择器发送 **选择信号**
     * @retval -EINTR: 等待被中断
     * @retval -ENOTTHDCTX: 不在线程上下文中
     * @note
     * + 上下文：
     *   + `mode == Sel::WaitMode::SelTryWait` : 任意
     *   + `mode != Sel::WaitMode::SelTryWait` : 线程
     * @details
     * + 等待分为下列几种模式：
     *   + `Sel::WaitMode::SelWait`: 调用线程会阻塞等待。
     *   + `Sel::WaitMode::SelWaitTimed` 定时等待模式， `to` 表示等待超时的时间点：
     *     + `to` 通常是未来时间点，即 **当前系统时间** + `delta` ，
     *       可以使用 `xwtm_ft(delta)` 表示；
     *     + 如果 `to` 是过去的时间点，将直接返回 `-ETIMEDOUT` 。
     *   + `Sel::WaitMode::SelWaitUninterruptable` 不可中断等待模式。
     *   + `Sel::WaitMode::SelTryWait` 尝试模式（只测试不等待）。
     */
    xwer_t select(xwos::Bmp<TNum> * msk, xwos::Bmp<TNum> * trigger, enum WaitMode mode,
                  xwtm_t to)
    {
        xwer_t rc;
        switch (mode) {
            case WaitMode::SelWait:
                rc = xwos_sel_select(mSelPtr, msk->mData, trigger->mData);
                break;
            case WaitMode::SelWaitTimed:
                rc = xwos_sel_select_to(mSelPtr, msk->mData, trigger->mData, to);
                break;
            case WaitMode::SelWaitUninterruptable:
                rc = xwos_sel_select_unintr(mSelPtr, msk->mData, trigger->mData);
                break;
            case WaitMode::SelTryWait:
                rc = xwos_sel_tryselect(mSelPtr, msk->mData, trigger->mData);
                break;
            default:
                rc = xwos_sel_select(mSelPtr, msk->mData, trigger->mData);
                break;
        }
        return rc;
    }

    /**
     * @brief 绑定源信号选择器到本信号选择器
     * @param[in] src: 源信号选择器对象的指针
     * @param[in] pos: 源信号选择器对象映射到位图中的位置
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
     * + 多个信号选择器可以依次绑定，形成信号传递链。
     *   但不可循环绑定，否则会造成无限循环传递。
     */
    template<xwsz_t TSelNum>
    xwer_t bind(Sel<TSelNum> * src, long pos)
    {
        return xwos_sel_bind(src->getXwosObj(), mSelPtr, (xwsq_t)pos);
    }
    /**
     * @overload xwer_t bind(Sel<TSelNum> & sel, long pos)
     */
    template<xwsz_t TSelNum>
    xwer_t bind(Sel<TSelNum> & src, long pos)
    {
        return xwos_sel_bind(src.getXwosObj(), mSelPtr, (xwsq_t)pos);
    }

    /**
     * @brief 从目的信号选择器上解绑源信号选择器
     * @param[in] src: 源信号选择器对象的指针
     * @return 错误码
     * @retval XWOK: 没有错误
     * @retval -EFAULT: 空指针
     * @retval -ENOTCONN: 同步对象没有绑定到事件对象上
     * @note
     * + 上下文：任意
     */
    template<xwsz_t TSelNum>
    xwer_t unbind(Sel<TSelNum> * src)
    {
        return xwos_sel_unbind(src->getXwosObj(), mSelPtr);
    }
    /**
     * @overload xwer_t unbind(Sel<TSelNum> & sel)
     */
    template<xwsz_t TSelNum>
    xwer_t unbind(Sel<TSelNum> & src)
    {
        return xwos_sel_unbind(src.getXwosObj(), mSelPtr);
    }

    /**
     * @brief 获取XWOS对象指针
     */
    struct xwos_sel * getXwosObj() { return mSelPtr; }
};

/**
 * @} xwos_cxx_sync_Sel
 */

} // namespace sync
} // namespace xwos

#endif /* xwos/cxx/sync/Sel.hxx */
