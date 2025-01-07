/**
 * @file
 * @brief xwos::sync::DSel
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef __xwos_cxx_sync_DSel_hxx__
#define __xwos_cxx_sync_DSel_hxx__

#include <xwos/osal/sync/sel.hxx>
#include <xwos/cxx/sync/Sel.hxx>

namespace xwos {
namespace sync {

/**
 * @defgroup xwos_cxx_sync_DSel 动态信号选择器
 * @ingroup xwos_cxx_sync
 *
 *
 * ## C++ API
 *
 * 头文件： @ref xwos/cxx/sync/DSel.hxx
 *
 * @{
 */

/**
 * @brief 动态信号选择器
 */
template<xwsz_t TNum>
class DSel
    : public Sel<TNum>
{
  private:
    xwos_sel_d mSelDesc; /**< 信号选择器对象描述符 */
    xwer_t mCtorRc; /**< 信号选择器构造的结果 */

  public:
    /**
     * @brief 构造函数
     */
    DSel() : Sel<TNum>() {
        mCtorRc = xwos_sel_create(&mSelDesc, TNum);
        if (XWOK == mCtorRc) {
            Sel<TNum>::mSelPtr = mSelDesc.sel;
        }
    }
    ~DSel() { xwos_sel_delete(mSelDesc); } /**< 析构函数 */
    xwer_t getCtorRc() { return mCtorRc; } /**< 获取动态信号选择器构造的结果 */

    /* 生命周期管理 */
    xwer_t acquire() { return xwos_sel_acquire(mSelDesc); } /**< 增加引用计数 */
    xwer_t release() { return xwos_sel_release(mSelDesc); } /**< 减少引用计数 */
};

/**
 * @} xwos_cxx_sync_DSel
 */

} // namespace sync
} // namespace xwos

#endif /* xwos/cxx/sync/DSel.hxx */
