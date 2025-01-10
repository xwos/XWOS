/**
 * @file
 * @brief xwos::sync::SCond
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef __xwos_cxx_sync_SCond_hxx__
#define __xwos_cxx_sync_SCond_hxx__

#include <xwos/osal/sync/cond.hxx>
#include <xwos/cxx/sync/Cond.hxx>

namespace xwos {
namespace sync {

/**
 * @defgroup xwos_cxx_sync_Cond_SCond 静态条件量
 * @ingroup xwos_cxx_sync_Cond
 *
 *
 * ## C++ API
 *
 * 头文件： @ref xwos/cxx/sync/SCond.hxx
 *
 * @{
 */

/**
 * @brief 静态条件量
 */
class SCond : public Cond
{
  private:
    struct xwos_cond mCond; /**< 条件量结构体 */
    xwer_t mCtorRc; /**< 条件量构造的结果 */

  public:
    /**
     * @brief 构造函数
     */
    SCond() : Cond()
    {
        mCtorRc = xwos_cond_init(&mCond);
        if (XWOK == mCtorRc) {
            Cond::mCondPtr = &mCond;
        }
    }
    ~SCond() { xwos_cond_fini(&mCond); } /**< 析构函数 */
    xwer_t getCtorRc() { return mCtorRc; } /**< 获取静态条件量构造的结果 */

    /* 生命周期管理 */
    xwer_t grab() { return xwos_cond_grab(&mCond); } /**< 增加引用计数 */
    xwer_t put() { return xwos_cond_put(&mCond); } /**< 减少引用计数 */

  private:
    static void * operator new(xwsz_t sz) = delete;
    void operator delete(void * obj) = delete;
};

/**
 * @} xwos_cxx_sync_SCond
 */

} // namespace sync
} // namespace xwos

#endif /* xwos/cxx/sync/SCond.hxx */
