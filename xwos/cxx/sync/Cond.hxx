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

namespace xwos {
namespace sync {

/**
 * @defgroup xwos_cxx_sync_Cond 条件量基类
 * @ingroup xwos_cxx_sync
 *
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
