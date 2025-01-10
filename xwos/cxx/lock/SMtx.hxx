/**
 * @file
 * @brief xwos::lock::SMtx
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef __xwos_cxx_lock_SMtx_hxx__
#define __xwos_cxx_lock_SMtx_hxx__

#include <xwos/osal/lock/mtx.hxx>
#include <xwos/cxx/lock/Mtx.hxx>
#include <xwos/cxx/sync/Cond.hxx>

namespace xwos {
namespace lock {

/**
 * @defgroup xwos_cxx_lock_Mtx_SMtx 静态互斥锁
 * @ingroup xwos_cxx_lock_Mtx
 *
 * 静态互斥锁是指互斥锁所需要的内存在编译期由编译器分配。
 *
 * ## 头文件
 *
 * @ref xwos/cxx/lock/SMtx.hxx
 *
 * @{
 */

/**
 * @brief 静态互斥锁
 */
class SMtx : public Mtx
{
  private:
    struct xwos_mtx mMtx; /**< 互斥锁结构体 */
    xwer_t mCtorRc; /**< 互斥锁构造的结果 */

  public:
    /**
     * @brief 构造函数
     * @param[in] pr: 互斥锁的天花板优先级
     */
    explicit SMtx(xwpr_t pr = XWOS_SKD_PRIORITY_RT_MAX)
        : Mtx()
    {
        mCtorRc = xwos_mtx_init(&mMtx, pr);
        if (XWOK == mCtorRc) {
            Mtx::mMtxPtr = &mMtx;
        }
    }

    ~SMtx() { xwos_mtx_fini(&mMtx); } /**< 析构函数 */
    xwer_t getCtorRc() { return mCtorRc; } /**< 获取静态互斥锁构造的结果 */

    /* 生命周期管理 */
    xwer_t grab() { return xwos_mtx_grab(&mMtx); } /**< 增加引用计数 */
    xwer_t put() { return xwos_mtx_put(&mMtx); } /**< 减少引用计数 */

  private:
    static void * operator new(xwsz_t sz) = delete;
    void operator delete(void * obj) = delete;
};

/**
 * @} xwos_cxx_lock_SMtx
 */

} // namespace lock
} // namespace xwos

#endif /* xwos/cxx/lock/SMtx.hxx */
