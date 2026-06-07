/**
 * @file
 * @brief xwos::sync::SSel
 * @author
 * + 隐星曜 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef __xwos_cxx_sync_SSel_hxx__
#define __xwos_cxx_sync_SSel_hxx__

#include <xwos/cxx/sync/Sel.hxx>

namespace xwos {
namespace sync {

/**
 * @defgroup xwos_cxx_sync_Sel_SSel 静态信号选择器
 * @ingroup xwos_cxx_sync_Sel
 *
 * @{
 */

/**
 * @brief 静态信号选择器
 */
template<xwsz_t TNum>
class SSel : public Sel<TNum>
{
  private:
    struct xwos_sel mSel; /**< 信号选择器结构体 */
    xwos::Bmp<TNum> mObjBmp; /**< 同步对象位图 */
    xwos::Bmp<TNum> mObjMsk; /**< 同步对象掩码 */

  public:
    /**
     * @brief 构造函数
     */
    SSel() : Sel<TNum>()
    {
        xwos_sel_init(&mSel, TNum, mObjBmp.mData, mMask);
        Sel<TNum>::mSelPtr = &mSel;
    }
    ~SSel() { xwos_sel_fini(&mSel); } /**< 析构函数 */

    /* 生命周期管理 */
    xwer_t grab() { return xwos_sel_grab(&mSel); } /**< 增加引用计数 */
    xwer_t put() { return xwos_sel_put(&mSel); } /**< 减少引用计数 */
};

/**
 * @} xwos_cxx_sync_SSel
 */

} // namespace sync
} // namespace xwos

#endif /* xwos/cxx/sync/SSel.hxx */
