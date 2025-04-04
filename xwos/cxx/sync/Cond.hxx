/**
 * @file
 * @brief xwos::sync::Cond
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef __xwos_cxx_sync_Cond_hxx__
#define __xwos_cxx_sync_Cond_hxx__

#include <xwos/osal/sync/cond.hxx>
#include <xwos/cxx/sync/Sel.hxx>

namespace xwos {
namespace sync {

/**
 * @defgroup xwos_cxx_sync_Cond 条件量
 * @ingroup xwos_cxx_sync
 *
 * ## 触发条件量
 *
 * + `Cond::unicast()` ：单播条件量
 * + `Cond::broadcast()` ：广播条件量
 *
 * ## 等待条件量
 *
 * + 条件量的等待操作需在锁的临界区内进行。因此等待相关的C++API在锁的守卫类内定义。
 *
 * ## 冻结与解冻条件量
 *
 * + `Cond::freeze()` ：冻结
 * + `Cond::thaw()` ：解冻
 *
 * ## 使用信号选择器选择条件量
 *
 * + `Cond::bind()` ：绑定到 [信号选择器](group__xwos__cxx__sync__Sel.html) 上
 * + `Cond::unbind()` ：从 [信号选择器](group__xwos__cxx__sync__Sel.html) 上解绑
 *
 * ## 头文件
 *
 * @ref xwos/cxx/sync/Flg.hxx
 *
 * @{
 */

/**
 * @brief 条件量基类
 */
class Cond
{
  protected:
    struct xwos_cond * mCondPtr;

  protected:
    Cond() : mCondPtr(nullptr) {}
    ~Cond() { mCondPtr = nullptr; }

  public:
    /**
     * @brief 单播条件量
     * @return 错误码
     * @retval XWOK: 没有错误
     * @retval -EFAULT: 无效的指针或空指针
     * @retval -ENEGATIVE: 条件量对象已被冻结
     * @note
     * + 上下文：任意
     * @details
     * + 单播条件量会唤醒第一个等待的线程。
     * + 此C++API只对未冻结的条件量对象起作用，已冻结的条件量对象将返回
     *   错误码 `-ENEGATIVE` 。
     */
    xwer_t unicast() { return xwos_cond_unicast(mCondPtr); }

    /**
     * @brief 广播条件量
     * @return 错误码
     * @retval XWOK: 没有错误
     * @retval -EFAULT: 无效的指针或空指针
     * @retval -ENEGATIVE: 条件量对象已被冻结
     * @note
     * + 上下文：任意
     * @details
     * + 广播条件量会唤醒所有等待的线程。
     * + 此C++API只对未冻结的条件量对象起作用，已冻结的条件量对象将返回
     *   错误码 `-ENEGATIVE` 。
     */
    xwer_t broadcast() { return xwos_cond_broadcast(mCondPtr); }

    /**
     * @brief 冻结条件量
     * @return 错误码
     * @retval XWOK: 没有错误
     * @retval -EFAULT: 无效的指针或空指针
     * @retval -EALREADY: 条件量对象已被冻结
     * @note
     * + 上下文：任意
     * @details
     * + 已冻结的条件量不允许单播或广播，但可以被线程等待。
     * + 对已经冻结的条件量再次冻结，将返回 `-EALREADY` 。
     */
    xwer_t freeze() { return xwos_cond_freeze(mCondPtr); }

    /**
     * @brief 解冻条件量
     * @return 错误码
     * @retval XWOK: 没有错误
     * @retval -EFAULT: 无效的指针或空指针
     * @retval -EALREADY: 条件量对象未被冻结
     * @note
     * + 上下文：任意
     * @details
     * + 此函数只对已冻结的条件量对象起作用，
     *   对未冻结的条件量对象调用此函数将返回错误码 `-EALREADY` 。
     */
    xwer_t thaw() { return xwos_cond_thaw(mCondPtr); }

    /**
     * @brief 绑定条件量对象到信号选择器
     * @param[in] sel: 信号选择器的指针
     * @param[in] pos: 条件量对象映射到信号选择器位图中的位置
     * @return 错误码
     * @retval XWOK: 没有错误
     * @retval -EFAULT: 无效的指针或空指针
     * @retval -ECHRNG: 位置超出范围
     * @retval -EALREADY: 同步对象已经绑定到事件对象
     * @retval -EBUSY: 通道已经被其他同步对象独占
     * @note
     * + 上下文：任意
     * + 绑定方式：非独占绑定
     * + 绑定了信号选择器的条件量对象，只有 **广播** 才会向信号选择器发送信号。
     */
    template<xwsz_t TSelNum>
    xwer_t bind(Sel<TSelNum> * sel, long pos)
    {
        return xwos_cond_bind(mCondPtr, sel->getXwosObj(), (xwsq_t)pos);
    }
    /**
     * @overload xwer_t bind(Sel<TSelNum> & sel, long pos)
     */
    template<xwsz_t TSelNum>
    xwer_t bind(Sel<TSelNum> & sel, long pos)
    {
        return xwos_cond_bind(mCondPtr, sel.getXwosObj(), (xwsq_t)pos);
    }

    /**
     * @brief 从信号选择器上解绑条件量对象
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
        return xwos_cond_unbind(mCondPtr, sel->getXwosObj());
    }
    /**
     * @overload xwer_t unbind(Sel<TSelNum> & sel)
     */
    template<xwsz_t TSelNum>
    xwer_t unbind(Sel<TSelNum> & sel)
    {
        return xwos_cond_unbind(mCondPtr, sel.getXwosObj());
    }

    /**
     * @brief 获取XWOS对象指针
     */
    struct xwos_cond * getXwosObj() { return mCondPtr; }
};

/**
 * @} xwos_cxx_sync_Cond
 */

} // namespace sync
} // namespace xwos

#endif /* xwos/cxx/sync/Cond.hxx */
