/**
 * @file
 * @brief xwos::sync::SFlg
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef __xwos_cxx_sync_SFlg_hxx__
#define __xwos_cxx_sync_SFlg_hxx__

#include <xwos/cxx/sync/Flg.hxx>

namespace xwos {
namespace sync {

/**
 * @defgroup xwos_cxx_sync_SFlg 静态事件标志
 * @ingroup xwos_cxx_sync
 *
 *
 * ## C++ API
 *
 * 头文件： @ref xwos/cxx/sync/SFlg.hxx
 *
 * @{
 */

/**
 * @brief 静态事件标志
 */
template<xwsz_t TNum>
class SFlg
    : public Flg<TNum>
{
  public:


  private:
    struct xwos_flg mFlg; /**< 事件标志结构体 */
    typename Flg<TNum>::FlgBmp mBitmap; /**< 事件位图 */
    typename Flg<TNum>::FlgBmp mMask; /**< 事件掩码 */
    xwer_t mCtorRc; /**< 事件标志构造的结果 */

  public:
    /**
     * @brief 构造函数
     */
    SFlg() : Flg<TNum>() {
        mCtorRc = xwos_flg_init(&mFlg, TNum, mBitmap, mMask);
        if (XWOK == mCtorRc) {
            Flg<TNum>::mFlgPtr = &mFlg;
        }
    }
    ~SFlg() { xwos_flg_fini(&mFlg); } /**< 析构函数 */
    xwer_t getCtorRc() { return mCtorRc; } /**< 获取静态事件标志构造的结果 */

    /* 生命周期管理 */
    xwer_t grab() { return xwos_flg_grab(&mFlg); } /**< 增加引用计数 */
    xwer_t put() { return xwos_flg_put(&mFlg); } /**< 减少引用计数 */

  private:
    static void * operator new(xwsz_t sz) = delete;
    void operator delete(void * obj) = delete;
};

/**
 * @} xwos_cxx_sync_SFlg
 */

} // namespace sync
} // namespace xwos

#endif /* xwos/cxx/sync/SFlg.hxx */
