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
    explicit Cond(xwos_cond * cond) : mCondPtr(cond) {}
    ~Cond() { mCondPtr = nullptr; }

    /* XWOS CAPI 能处理 `nullptr` 的情况。 */
    xwer_t unicast() { return xwos_cond_unicast(mCondPtr); }
    xwer_t broadcast() { return xwos_cond_broadcast(mCondPtr); }
    xwer_t freeze() { return xwos_cond_freeze(mCondPtr); }
    xwer_t thaw() { return xwos_cond_thaw(mCondPtr); }

  public:
    struct xwos_cond * getXwosObj() { return mCondPtr; }
};

/**
 * @} xwos_cxx_sync_Cond
 */

} // namespace sync
} // namespace xwos

#endif /* xwos/cxx/sync/Cond.hxx */
