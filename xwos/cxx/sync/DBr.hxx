/**
 * @file
 * @brief xwos::sync::DBr
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
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
 * @defgroup xwos_cxx_sync_DBr 动态事件标志
 * @ingroup xwos_cxx_sync
 *
 *
 * ## C++ API
 *
 * 头文件： @ref xwos/cxx/sync/DBr.hxx
 *
 * @{
 */

/**
 * @brief 动态事件标志
 */
template<xwsz_t TNum>
class DBr
    : public Br<TNum>
{
  private:
    xwos_br_d mBrDesc; /**< 事件标志对象描述符 */
    xwer_t mCtorRc; /**< 事件标志构造的结果 */

  public:
    /**
     * @brief 构造函数
     */
    DBr() : Br<TNum>() {
        mCtorRc = xwos_br_create(&mBrDesc, TNum);
        if (XWOK == mCtorRc) {
            Br<TNum>::mBrPtr = mBrDesc.br;
        }
    }
    ~DBr() { xwos_br_delete(mBrDesc); } /**< 析构函数 */
    xwer_t getCtorRc() { return mCtorRc; } /**< 获取动态事件标志构造的结果 */

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
