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
 * @defgroup xwos_cxx_lock_SMtx 静态互斥锁
 * @ingroup xwos_cxx_lock
 *
 * 静态互斥锁是指互斥锁所需要的内存在编译期由编译器分配。
 *
 *
 * ## 静态互斥锁的RAII机制守卫
 *
 * `Mtx::Grd` 构造时会上锁互斥锁，析构时会自动解锁互斥锁。
 * `Mtx::Grd` 上锁模式分为：
 * + `Mtx::LockMode::MtxUnlock` 未上锁。
 * + `Mtx::LockMode::MtxLock` 如果无法上锁，调用线程会阻塞等待上锁。
 * + `Mtx::LockMode::MtxLockTimed` 定时等待模式，如果无法上锁，
 *   调用线程会定时阻塞等待上锁。
 * + `Mtx::LockMode::MtxLockUninterruptable` 不可中断等待模式，
 *   如果无法上锁，调用线程会阻塞等待上锁，且不可被中断。
 * + `Mtx::LockMode::MtxLockTry` 尝试上锁模式，如果无法上锁，
 *   调用线程不会阻塞等待，尝试失败立即返回。
 *
 * ### 示例
 *
 * ```Cpp
 * #include <xwos/cxx/lock/SMtx.hxx>
 *
 * xwos::lock::SMtx gSlock;
 *
 * void foo()
 * {
 *     { // 构造 `grd` 时会自动对互斥锁上锁
 *         xwos::lock::Mtx::Grd grd(gSlock, xwos::lock::Mtx::LockMode::MtxLock);
 *         if (XWOK == grd.getRc()) {
 *             // 上锁成功，进入临界区
 *         } else {
 *             // 上锁失败
 *         }
 *     } // 如果上锁成功，grd析构时会自动解锁互斥锁
 * }
 * ```
 *
 *
 * ## C++ API
 *
 * 头文件： @ref xwos/cxx/lock/SMtx.hxx
 *
 * @{
 */

/**
 * @brief 静态互斥锁
 */
class SMtx
    : public Mtx
{
  private:
    struct xwos_mtx mMtx; /**< 互斥锁结构体 */
    xwer_t mCtorRc; /**< 互斥锁构造的结果 */

  public:
    /**
     * @brief 构造函数
     * @param[in] pr: 互斥锁的天花板优先级
     */
    explicit SMtx(xwpr_t pr = XWOS_SKD_PRIORITY_RT_MAX) : Mtx() {
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
