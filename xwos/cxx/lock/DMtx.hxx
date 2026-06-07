/**
 * @file
 * @brief xwos::lock::DMtx
 * @author
 * + 隐星曜 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef __xwos_cxx_lock_DMtx_hxx__
#define __xwos_cxx_lock_DMtx_hxx__

#include <xwos/osal/lock/mtx.hxx>
#include <xwos/cxx/lock/Mtx.hxx>
#include <xwos/cxx/sync/Cond.hxx>

namespace xwos {
namespace lock {

/**
 * @defgroup xwos_cxx_lock_Mtx_DMtx 动态互斥锁
 * @ingroup xwos_cxx_lock_Mtx
 *
 * @{
 */

/**
 * @brief 动态互斥锁
 */
class DMtx : public Mtx
{
  private:
    xwos_mtx_d mMtxDesc; /**< 互斥锁对象描述符 */
    xwer_t mCtorRc; /**< 互斥锁构造的结果 */

  public:
    /**
     * @brief 构造函数
     * @param[in] pr: 互斥锁的天花板优先级
     */
    explicit DMtx(xwpr_t pr = XWOS_SKD_PRIORITY_RT_MAX)
    {
        mCtorRc = xwos_mtx_create(&mMtxDesc, pr);
        if (XWOK == mCtorRc) {
            Mtx::mMtxPtr = mMtxDesc.mtx;
        }
    }
    /**
     * @brief 析构函数
     */
    ~DMtx() { xwos_mtx_delete(mMtxDesc); }
    /**
     * @brief 获取动态互斥锁构造的结果
     * @note
     * 动态对象的创建可能会因为内存不足构造失败。
     */
    xwer_t getDMtxCtorRc() { return mCtorRc; }

    /* 生命周期管理 */
    xwer_t acquire() { return xwos_mtx_acquire(mMtxDesc); } /**< 增加引用计数 */
    xwer_t release() { return xwos_mtx_release(mMtxDesc); } /**< 减少引用计数 */
};

/**
 * @} xwos_cxx_lock_DMtx
 */

} // namespace lock
} // namespace xwos

#endif /* xwos/cxx/lock/DMtx.hxx */
