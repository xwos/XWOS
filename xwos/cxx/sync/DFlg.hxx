/**
 * @file
 * @brief xwos::sync::DFlg
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef __xwos_cxx_sync_DFlg_hxx__
#define __xwos_cxx_sync_DFlg_hxx__

#include <xwos/osal/sync/flg.hxx>
#include <xwos/cxx/sync/Flg.hxx>

namespace xwos {
namespace sync {

/**
 * @defgroup xwos_cxx_sync_Flg_DFlg 动态事件标志
 * @ingroup xwos_cxx_sync_Flg
 *
 *
 * ## C++ API
 *
 * 头文件： @ref xwos/cxx/sync/DFlg.hxx
 *
 * @{
 */

/**
 * @brief 动态事件标志
 */
template<xwsz_t TNum>
class DFlg : public Flg<TNum>
{
  private:
    xwos_flg_d mFlgDesc; /**< 事件标志对象描述符 */
    xwer_t mCtorRc; /**< 事件标志构造的结果 */

  public:
    /**
     * @brief 构造函数
     */
    DFlg()
        : Flg<TNum>()
    {
        mCtorRc = xwos_flg_create(&mFlgDesc, TNum);
        if (XWOK == mCtorRc) {
            Flg<TNum>::mFlgPtr = mFlgDesc.flg;
        }
    }
    ~DFlg() { xwos_flg_delete(mFlgDesc); } /**< 析构函数 */
    xwer_t getCtorRc() { return mCtorRc; } /**< 获取动态事件标志构造的结果 */

    /* 生命周期管理 */
    xwer_t acquire() { return xwos_flg_acquire(mFlgDesc); } /**< 增加引用计数 */
    xwer_t release() { return xwos_flg_release(mFlgDesc); } /**< 减少引用计数 */
};

/**
 * @} xwos_cxx_sync_DFlg
 */

} // namespace sync
} // namespace xwos

#endif /* xwos/cxx/sync/DFlg.hxx */
