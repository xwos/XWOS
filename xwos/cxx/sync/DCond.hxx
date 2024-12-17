/**
 * @file
 * @brief xwos::sync::DCond
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef __xwos_cxx_sync_DCond_hxx__
#define __xwos_cxx_sync_DCond_hxx__

#include <xwos/osal/sync/cond.hxx>
#include <xwos/cxx/sync/Cond.hxx>

namespace xwos {
namespace sync {

/**
 * @defgroup xwos_cxx_sync_DCond 动态条件量
 * @ingroup xwos_cxx_sync
 *
 *
 * ## C++ API
 *
 * 头文件： @ref xwos/cxx/sync/DCond.hxx
 *
 * @{
 */

/**
 * @brief 静态条件量
 */
class DCond
    : public Cond
{
  private:
    xwos_cond_d mCondDesc; /**< 条件量对象描述符 */
    xwer_t mCtorRc; /**< 条件量构造的结果 */

  public:
    /**
     * @brief 构造函数
     */
    DCond() : Cond(nullptr) {
        mCtorRc = xwos_cond_create(&mCondDesc);
        if (XWOK == mCtorRc) {
            Cond::mCondPtr = mCondDesc.cond;
        }
    }
    ~DCond() { xwos_cond_delete(mCondDesc); } /**< 析构函数 */
    xwer_t getCtorRc() { return mCtorRc; } /**< 获取静态条件量构造的结果 */

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
    xwer_t unicast() { return Cond::unicast(); }

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
    xwer_t broadcast() { return Cond::broadcast(); }

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
    xwer_t freeze() { return Cond::freeze(); }

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
    xwer_t thaw() { return Cond::thaw(); }

    /* 生命周期管理 */
    xwer_t acquire() { return xwos_cond_acquire(mCondDesc); } /**< 增加引用计数 */
    xwer_t release() { return xwos_cond_release(mCondDesc); } /**< 减少引用计数 */
};

/**
 * @} xwos_cxx_sync_DCond
 */

} // namespace sync
} // namespace xwos

#endif /* xwos/cxx/sync/DCond.hxx */
