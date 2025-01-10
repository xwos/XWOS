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
class DCond : public Cond
{
  private:
    xwos_cond_d mCondDesc; /**< 条件量对象描述符 */
    xwer_t mCtorRc; /**< 条件量构造的结果 */

  public:
    /**
     * @brief 构造函数
     */
    DCond() : Cond()
    {
        mCtorRc = xwos_cond_create(&mCondDesc);
        if (XWOK == mCtorRc) {
            Cond::mCondPtr = mCondDesc.cond;
        }
    }
    ~DCond() { xwos_cond_delete(mCondDesc); } /**< 析构函数 */
    xwer_t getCtorRc() { return mCtorRc; } /**< 获取静态条件量构造的结果 */

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
