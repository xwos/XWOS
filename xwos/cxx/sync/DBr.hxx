/**
 * @file
 * @brief xwos::sync::DBr
 * @author
 * + 隐星曜 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef __xwos_cxx_sync_DBr_hxx__
#define __xwos_cxx_sync_DBr_hxx__

#include <xwos/osal/sync/br.hxx>
#include <xwos/cxx/sync/Br.hxx>

namespace xwos {
namespace sync {

/**
 * @defgroup xwos_cxx_sync_Br_DBr 动态线程栅栏
 * @ingroup xwos_cxx_sync_Br
 *
 * @{
 */

/**
 * @brief 动态线程栅栏
 */
template<xwsz_t TNum>
class DBr : public Br<TNum>
{
  private:
    xwos_br_d mBrDesc; /**< 线程栅栏对象描述符 */
    xwer_t mCtorRc; /**< 线程栅栏构造的结果 */

  public:
    /**
     * @brief 构造函数
     */
    DBr() : Br<TNum>()
    {
        mCtorRc = xwos_br_create(&mBrDesc, TNum);
        if (XWOK == mCtorRc) {
            Br<TNum>::mBrPtr = mBrDesc.br;
        }
    }
    /**
     * @brief 析构函数
     */
    ~DBr() { xwos_br_delete(mBrDesc); }
    /**
     * @brief 获取动态线程栅栏构造的结果
     * @note
     * 动态对象的创建可能会因为内存不足构造失败。
     */
    xwer_t getDBrCtorRc() { return mCtorRc; }

    /* 生命周期管理 */
    xwer_t acquire() { return xwos_br_acquire(mBrDesc); } /**< 增加引用计数 */
    xwer_t release() { return xwos_br_release(mBrDesc); } /**< 减少引用计数 */
};

/**
 * @} xwos_cxx_sync_DBr
 */

} // namespace sync
} // namespace xwos

#endif /* xwos/cxx/sync/DBr.hxx */
