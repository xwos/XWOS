/**
 * @file
 * @brief xwos::sync::SSem
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef __xwos_cxx_sync_SSem_hxx__
#define __xwos_cxx_sync_SSem_hxx__

#include <xwos/osal/sync/sem.hxx>
#include <xwos/cxx/sync/Sem.hxx>

namespace xwos {
namespace sync {

/**
 * @defgroup xwos_cxx_sync_Sem_SSem 静态信号量
 * @ingroup xwos_cxx_sync_Sem
 *
 *
 * ## C++ API
 *
 * 头文件： @ref xwos/cxx/sync/SSem.hxx
 *
 * @{
 */

/**
 * @brief 静态信号量
 */
class SSem : public Sem
{
  private:
    struct xwos_sem mSem; /**< 信号量结构体 */
    xwer_t mCtorRc; /**< 信号量构造的结果 */

  public:
    /**
     * @brief 构造函数
     */
    SSem(xwssq_t val = 0, xwssq_t max = XWSSQ_MAX)
        : Sem()
    {
        mCtorRc = xwos_sem_init(&mSem, val, max);
        if (XWOK == mCtorRc) {
            Sem::mSemPtr = &mSem;
        }
    }
    ~SSem() { xwos_sem_fini(&mSem); } /**< 析构函数 */
    xwer_t getCtorRc() { return mCtorRc; } /**< 获取静态信号量构造的结果 */

    /* 生命周期管理 */
    xwer_t grab() { return xwos_sem_grab(&mSem); } /**< 增加引用计数 */
    xwer_t put() { return xwos_sem_put(&mSem); } /**< 减少引用计数 */

  private:
    static void * operator new(xwsz_t sz) = delete;
    void operator delete(void * obj) = delete;
};

/**
 * @} xwos_cxx_sync_SSem
 */

} // namespace sync
} // namespace xwos

#endif /* xwos/cxx/sync/SSem.hxx */
