/**
 * @file
 * @brief xwos::sync::DSem
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef __xwos_cxx_sync_DSem_hxx__
#define __xwos_cxx_sync_DSem_hxx__

#include <xwos/osal/sync/sem.hxx>
#include <xwos/cxx/sync/Sem.hxx>

namespace xwos {
namespace sync {

/**
 * @defgroup xwos_cxx_sync_Sem_DSem 动态信号量
 * @ingroup xwos_cxx_sync_Sem
 *
 *
 * ## C++ API
 *
 * 头文件： @ref xwos/cxx/sync/DSem.hxx
 *
 * @{
 */

/**
 * @brief 动态信号量
 */
class DSem : public Sem
{
  private:
    xwos_sem_d mSemDesc; /**< 信号量对象描述符 */
    xwer_t mCtorRc; /**< 信号量构造的结果 */

  public:
    /**
     * @brief 构造函数
     */
    DSem(xwssq_t val = 0, xwssq_t max = XWSSQ_MAX)
        : Sem()
    {
        mCtorRc = xwos_sem_create(&mSemDesc, val, max);
        if (XWOK == mCtorRc) {
            Sem::mSemPtr = mSemDesc.sem;
        }
    }
    ~DSem() { xwos_sem_delete(mSemDesc); } /**< 析构函数 */
    xwer_t getCtorRc() { return mCtorRc; } /**< 获取动态信号量构造的结果 */

    /* 生命周期管理 */
    xwer_t acquire() { return xwos_sem_acquire(mSemDesc); } /**< 增加引用计数 */
    xwer_t release() { return xwos_sem_release(mSemDesc); } /**< 减少引用计数 */
};

/**
 * @} xwos_cxx_sync_DSem
 */

} // namespace sync
} // namespace xwos

#endif /* xwos/cxx/sync/DSem.hxx */
