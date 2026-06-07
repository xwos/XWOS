/**
 * @file
 * @brief xwos::sync::SBr
 * @author
 * + 隐星曜 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef __xwos_cxx_sync_SBr_hxx__
#define __xwos_cxx_sync_SBr_hxx__

#include <xwos/cxx/sync/Br.hxx>

namespace xwos {
namespace sync {

/**
 * @defgroup xwos_cxx_sync_Br_SBr 静态事件标志
 * @ingroup xwos_cxx_sync_Br
 *
 * @{
 */

/**
 * @brief 静态线程栅栏
 */
template<xwsz_t TNum>
class SBr : public Br<TNum>
{
  private:
    struct xwos_br mBr; /**< 线程栅栏结构体 */
    xwos::Bmp<TNum> mThdBmp; /**< 线程位图 */
    xwos::Bmp<TNum> mThdMsk; /**< 线程掩码 */

  public:
    /**
     * @brief 构造函数
     */
    SBr() : Br<TNum>()
    {
        xwos_br_init(&mBr, TNum, mThdBmp.mData, mMask);
        Br<TNum>::mBrPtr = &mBr;
    }
    ~SBr() { xwos_br_fini(&mBr); } /**< 析构函数 */

    /* 生命周期管理 */
    xwer_t grab() { return xwos_br_grab(&mBr); } /**< 增加引用计数 */
    xwer_t put() { return xwos_br_put(&mBr); } /**< 减少引用计数 */
};

/**
 * @} xwos_cxx_sync_SBr
 */

} // namespace sync
} // namespace xwos

#endif /* xwos/cxx/sync/SBr.hxx */
