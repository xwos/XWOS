/**
 * @file
 * @brief xwos::sync::Flg
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef __xwos_cxx_sync_Flg_hxx__
#define __xwos_cxx_sync_Flg_hxx__

#include <xwos/cxx/Bmp.hxx>
#include <xwos/cxx/sync/Sel.hxx>

extern "C" {
#include <xwos/osal/sync/flg.h>
}

namespace xwos {
namespace sync {

/**
 * @defgroup xwos_cxx_sync_Flg 事件标志基类
 * @ingroup xwos_cxx_sync
 *
 * ## C++ API
 *
 * 头文件： @ref xwos/cxx/sync/Flg.hxx
 *
 * @{
 */

/**
 * @brief 事件标志基类
 */
template<xwsz_t TNum>
class Flg
{
  public:
    /**
     * @brief 触发条件枚举
     */
    enum Trigger: xwu32_t {
        FlgSetAll = XWOS_FLG_TRIGGER_SET_ALL, /**< 掩码中所有位同时为1 */
        FlgSetAny = XWOS_FLG_TRIGGER_SET_ANY, /**< 掩码中的任意位为1 */
        FlgClearAll = XWOS_FLG_TRIGGER_CLR_ALL, /**< 掩码中的所有位同时为0 */
        FlgClearAny = XWOS_FLG_TRIGGER_CLR_ANY, /**< 掩码中的任意位为0 */
        FlgToggleAll = XWOS_FLG_TRIGGER_TGL_ALL, /**< 掩码中所有位翻转 */
        FlgToggleAny = XWOS_FLG_TRIGGER_TGL_ANY, /**< 掩码中任意位翻转 */
    };

    /**
     * @brief 等待模式枚举
     */
    enum WaitMode : xwu32_t {
        FlgWait = 0, /**< 等待模式 */
        FlgWaitTimed, /**< 定时等待模式 */
        FlgWaitUninterruptable, /**< 不可中断等待模式 */
        FlgTryWait, /**< 尝试模式（只测试不等待） */
    };

  protected:
    struct xwos_flg * mFlgPtr;
  protected:
    Flg() : mFlgPtr(nullptr) {}
    ~Flg() { mFlgPtr = nullptr; }

  public:
    /**
     * @brief 获取事件的最大数量
     */
    xwsz_t num() { return TNum; }

    /**
     * @brief 读取事件标志位图的值
     * @param[out] out: 指向缓冲区的指针，通过此缓冲区返回事件标志对象位图的值
     * @note
     * + 上下文：任意
     */
    void read(xwos::Bmp<TNum> * out) { xwos_flg_read(mFlgPtr, out->mData); }

    /**
     * @brief 同时设置多个事件标志位
     * @param[in] msk: 事件掩码
     * @return 错误码
     * @retval XWOK: 没有错误
     * @retval -EFAULT: 无效的指针或空指针
     * @note
     * + 上下文：任意
     * @details
     * + 此C++API会将事件标志位图中，被掩码 `msk` 覆盖的位全部设置为 **1** ，
     *   然后 **广播** 所有正在等待的线程。
     * + 线程唤醒后通过比对位图状态，确定事件是否已经满足触发条件，
     *   若满足触发条件，就退出等待；若未满足触发条件，重新进入阻塞等待状态。
     */
    xwer_t set(xwos::Bmp<TNum> * msk) { return xwos_flg_s1m(mFlgPtr, msk->mData); }
    /**
     * @brief 同时设置多个事件标志位
     */
    xwer_t set(xwos::Bmp<TNum> & msk) { return xwos_flg_s1m(mFlgPtr, msk.mData); }
    /**
     * @brief 设置单个事件标志位
     * @param[in] pos: 事件的序号
     * @return 错误码
     * @retval XWOK: 没有错误
     * @retval -EFAULT: 无效的指针或空指针
     * @retval -ECHRNG: 位置超出范围
     * @note
     * + 上下文：任意
     * @details
     * + 此CAPI会将事件标志位图中，序号为 `pos` 的单个位设置为 **1** ，
     *   然后 **广播** 所有正在等待的线程。
     * + 线程唤醒后通过比对位图状态，确定事件是否已经满足触发条件，
     *   若满足触发条件，就退出等待；若未满足触发条件，重新进入阻塞等待状态。
     */
    xwer_t set(unsigned long pos) { return xwos_flg_s1i(mFlgPtr, pos); }
    /**
     * @brief 设置单个事件标志位
     */
    xwer_t set(long pos) { return xwos_flg_s1i(mFlgPtr, (xwsq_t)pos); }

    /**
     * @brief 同时清除多个事件标志位
     * @param[in] msk: 事件的位图掩码
     * @return 错误码
     * @retval XWOK: 没有错误
     * @retval -EFAULT: 无效的指针或空指针
     * @note
     * + 上下文：任意
     * @details
     * + 此CAPI会将事件标志位图中，被掩码 `msk` 覆盖的位全部清 **0** ，
     *   然后 **广播** 所有正在等待的线程。
     * + 线程唤醒后通过比对位图状态，确定事件是否已经满足触发条件，
     *   若满足触发条件，就退出等待；若未满足触发条件，重新进入阻塞等待状态。
     */
    xwer_t clear(xwos::Bmp<TNum> * msk) { return xwos_flg_c0m(mFlgPtr, msk->mData); }
    /**
     * @brief 清除单个事件标志位
     * @param[in] pos: 事件的序号
     * @return 错误码
     * @retval XWOK: 没有错误
     * @retval -EFAULT: 无效的指针或空指针
     * @retval -ECHRNG: 位置超出范围
     * @note
     * + 上下文：任意
     * @details
     * + 此CAPI会将事件标志位图中，序号为 `pos` 的单个位清 **0** ，
     *   然后 **广播** 所有正在等待的线程。
     * + 线程唤醒后通过比对位图状态，确定事件是否已经满足触发条件，
     *   若满足触发条件，就退出等待；若未满足触发条件，重新进入阻塞等待状态。
     */
    xwer_t clear(unsigned long pos) { return xwos_flg_c0i(mFlgPtr, (xwsq_t)pos); }
    /**
     * @brief 清除单个事件标志位
     */
    xwer_t clear(long pos) { return xwos_flg_c0i(mFlgPtr, (xwsq_t)pos); }


    /**
     * @brief 同时翻转多个事件标志位
     * @param[in] msk: 事件的位图掩码
     * @return 错误码
     * @retval XWOK: 没有错误
     * @retval -EFAULT: 无效的指针或空指针
     * @note
     * + 上下文：任意
     * @details
     * + 此CAPI会将事件标志位图中，被掩码 `msk` 覆盖的位全部翻转，
     *   然后 **广播** 所有正在等待的线程。
     * + 线程唤醒后通过比对位图状态，确定事件是否已经满足触发条件，
     *   若满足触发条件，就退出等待；若未满足触发条件，重新进入阻塞等待状态。
     */
    xwer_t toggle(xwos::Bmp<TNum> * msk) { return xwos_flg_x1m(mFlgPtr, msk->mData); }

    /**
     * @brief 翻转单个事件标志位
     * @param[in] pos: 事件的序号
     * @return 错误码
     * @retval XWOK: 没有错误
     * @retval -EFAULT: 无效的指针或空指针
     * @retval -ECHRNG: 位置超出范围
     * @note
     * + 上下文：任意
     * @details
     * + 此CAPI会将事件标志位图中，序号为 `pos` 的单个位翻转，
     *   然后 **广播** 所有正在等待的线程。
     * + 线程唤醒后通过比对位图状态，确定事件是否已经满足触发条件，
     *   若满足触发条件，就退出等待；若未满足触发条件，重新进入阻塞等待状态。
     */
    xwer_t toggle(unsigned long pos) { return xwos_flg_x1i(mFlgPtr, (xwsq_t)pos); }
    /**
     * @brief 翻转单个事件标志位
     */
    xwer_t toggle(long pos) { return xwos_flg_x1i(mFlgPtr, (xwsq_t)pos); }

    /**
     * @brief 等待事件标志
     * @param[in] trigger: 事件触发条件，取值：
     * + **电平触发**
     *   @arg @ref Trigger::FlgSetAll : 掩码中的所有位同时为1
     *   @arg @ref Trigger::FlgSetAny : 掩码中的任意位为1
     *   @arg @ref Trigger::FlgClearAll : 掩码中的所有位同时为0
     *   @arg @ref Trigger::FlgClearAny : 掩码中的任意位为0
     * + **边沿触发**
     *   @arg @ref Trigger::FlgToggleAll : 掩码中的所有位同时发生翻转
     *   @arg @ref Trigger::FlgToggleAny : 掩码中的任意位发生翻转
     * @param[in] consumption: 事件触发后是否清除事件：
     * + 当 `trigger` 为 **电平触发** 时，
     *   + `true` 表示清除事件标志
     *   + `false` 表示保留事件标志
     * + 当 `trigger` 为 **边沿触发** 时，此参数没有意义
     * @param[in,out] origin: 指向缓冲区的指针：
     * + 当 `trigger` 为 **电平触发** 时
     *   + (O) 返回事件触发 **之前** 的位图状态
     * + 当 `trigger` 为 **边沿触发** 时
     *   + (I) 输入时，作为评估事件位图是否发生变化的初始状态
     *   + (O) 输出时，返回触发后的事件标志位图状态（可作为下一次调用的初始值）
     * @param[in] msk: 事件的位图掩码，表示只关注掩码部分的事件
     * @param[in] mode: 等待模式，取值 @ref Flg::WaitMode
     *   @arg Flg::WaitMode::FlgWait
     *   @arg Flg::WaitMode::FlgWaitTimed
     *   @arg Flg::WaitMode::FlgWaitUninterruptable
     *   @arg Flg::WaitMode::FlgTryWait
     * @param[in] to: 期望唤醒的时间点，
     *                仅当 `mode == Flg::WaitMode::FlgWaitTimed` 时有效。
     * @return 错误码
     * @retval XWOK: 没有错误
     * @retval -EFAULT: 空指针
     * @retval -ETIMEDOUT: 超时
     * @retval -EINTR: 等待被中断
     * @retval -ENOTTHDCTX: 不在线程上下文中
     * @note
     * + 上下文：
     *   + `mode == Flg::WaitMode::FlgTryWait` : 任意
     *   + `mode != Flg::WaitMode::FlgTryWait` : 线程
     * @details
     * + 等待分为下列几种模式：
     *   + `Flg::WaitMode::FlgWait`: 调用线程会阻塞等待。
     *   + `Flg::WaitMode::FlgWaitTimed` 定时等待模式， `to` 表示等待超时的时间点：
     *     + `to` 通常是未来时间点，即 **当前系统时间** + `delta` ，
     *       可以使用 `xwtm_ft(delta)` 表示；
     *     + 如果 `to` 是过去的时间点，将直接返回 `-ETIMEDOUT` 。
     *   + `Flg::WaitMode::FlgWaitUninterruptable` 不可中断等待模式。
     *   + `Flg::WaitMode::FlgTryWait` 尝试模式（只测试不等待）。
     */
    xwer_t wait(enum Trigger trigger, bool consumption,
                xwos::Bmp<TNum> * origin, xwos::Bmp<TNum> * msk,
                enum WaitMode mode = WaitMode::FlgWait, xwtm_t to = XWTM_MAX) {
        xwer_t rc;
        switch (mode) {
            case WaitMode::FlgWait:
                rc = xwos_flg_wait(mFlgPtr, (enum xwos_flg_trigger_em)trigger,
                                   consumption, origin->mData, msk->mData);
                break;
            case WaitMode::FlgWaitTimed:
                rc = xwos_flg_wait_to(mFlgPtr, (enum xwos_flg_trigger_em)trigger,
                                      consumption, origin->mData, msk->mData, to);
                break;
            case WaitMode::FlgWaitUninterruptable:
                rc = xwos_flg_wait_unintr(mFlgPtr, (enum xwos_flg_trigger_em)trigger,
                                          consumption, origin->mData, msk->mData);
                break;
            case WaitMode::FlgTryWait:
                rc = xwos_flg_trywait(mFlgPtr, (enum xwos_flg_trigger_em)trigger,
                                      consumption, origin->mData, msk->mData);
                break;
            default:
                rc = xwos_flg_wait(mFlgPtr, (enum xwos_flg_trigger_em)trigger,
                                   consumption, origin->mData, msk->mData);
                break;
        }
        return rc;
    }

    /**
     * @brief 绑定事件标志对象到信号选择器
     * @param[in] sel: 信号选择器的指针
     * @param[in] pos: 事件标志对象映射到信号选择器位图中的位置
     * @return 错误码
     * @retval XWOK: 没有错误
     * @retval -EFAULT: 无效的指针或空指针
     * @retval -ECHRNG: 位置超出范围
     * @retval -EALREADY: 同步对象已经绑定到事件对象
     * @retval -EBUSY: 通道已经被其他同步对象独占
     * @note
     * + 上下文：任意
     * + 绑定方式：非独占绑定
     */
    template<xwsz_t TSelNum>
    xwer_t bind(Sel<TSelNum> * sel, long pos) {
        return xwos_flg_bind(mFlgPtr, sel->getXwosObj(), (xwsq_t)pos);
    }

    /**
     * @brief 绑定事件标志对象到信号选择器
     * @param[in] sel: 信号选择器的引用
     * @param[in] pos: 事件标志对象映射到信号选择器位图中的位置
     * @return 错误码
     * @retval XWOK: 没有错误
     * @retval -EFAULT: 无效的指针或空指针
     * @retval -ECHRNG: 位置超出范围
     * @retval -EALREADY: 同步对象已经绑定到事件对象
     * @retval -EBUSY: 通道已经被其他同步对象独占
     * @note
     * + 上下文：任意
     * + 绑定方式：非独占绑定
     */
    template<xwsz_t TSelNum>
    xwer_t bind(Sel<TSelNum> & sel, long pos) {
        return xwos_flg_bind(mFlgPtr, sel.getXwosObj(), (xwsq_t)pos);
    }

    /**
     * @brief 从信号选择器上解绑事件标志对象
     * @param[in] sel: 信号选择器的引用
     * @return 错误码
     * @retval XWOK: 没有错误
     * @retval -EFAULT: 空指针
     * @retval -ENOTCONN: 同步对象没有绑定到事件对象上
     * @note
     * + 上下文：任意
     */
    template<xwsz_t TSelNum>
    xwer_t unbind(Sel<TSelNum> * sel) {
        return xwos_flg_unbind(mFlgPtr, sel->getXwosObj());
    }

    /**
     * @brief 从信号选择器上解绑事件标志对象
     * @param[in] sel: 信号选择器的引用
     * @return 错误码
     * @retval XWOK: 没有错误
     * @retval -EFAULT: 空指针
     * @retval -ENOTCONN: 同步对象没有绑定到事件对象上
     * @note
     * + 上下文：任意
     */
    template<xwsz_t TSelNum>
    xwer_t unbind(Sel<TSelNum> & sel) {
        return xwos_flg_unbind(mFlgPtr, sel.getXwosObj());
    }

    /**
     * @brief 获取XWOS对象指针
     */
    struct xwos_flg * getXwosObj() { return mFlgPtr; }
};

/**
 * @} xwos_cxx_sync_Flg
 */

} // namespace sync
} // namespace xwos

#endif /* xwos/cxx/sync/Flg.hxx */
