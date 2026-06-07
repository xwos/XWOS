/**
 * @file
 * @brief xwos::lock::SMtx
 * @author
 * + 隐星曜 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef __xwos_cxx_lock_SMtx_hxx__
#define __xwos_cxx_lock_SMtx_hxx__

#include <xwos/osal/lock/mtx.hxx>
#include <xwos/cxx/lock/Mtx.hxx>
#include <xwos/cxx/sync/Cond.hxx>

namespace xwos {
namespace lock {

/**
 * @defgroup xwos_cxx_lock_Mtx_SMtx 静态互斥锁
 * @ingroup xwos_cxx_lock_Mtx
 *
 * @{
 */

/**
 * @brief 静态互斥锁
 */
class SMtx : public Mtx
{
  private:
    struct xwos_mtx mMtx; /**< 互斥锁结构体 */

  public:
    /**
     * @brief 构造函数
     * @param[in] pr: 互斥锁的天花板优先级
     */
    explicit SMtx(xwpr_t pr = XWOS_SKD_PRIORITY_RT_MAX)
        : Mtx()
    {
        xwos_mtx_init(&mMtx, pr);
        Mtx::mMtxPtr = &mMtx;
    }

    ~SMtx() { xwos_mtx_fini(&mMtx); } /**< 析构函数 */

    /* 生命周期管理 */
    xwer_t grab() { return xwos_mtx_grab(&mMtx); } /**< 增加引用计数 */
    xwer_t put() { return xwos_mtx_put(&mMtx); } /**< 减少引用计数 */
};

/**
 * @} xwos_cxx_lock_SMtx
 */

} // namespace lock
} // namespace xwos

#endif /* xwos/cxx/lock/SMtx.hxx */
