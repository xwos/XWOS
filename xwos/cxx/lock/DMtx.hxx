/**
 * @file
 * @brief xwos::lock::DMtx
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef __xwos_cxx_lock_DMtx_hxx__
#define __xwos_cxx_lock_DMtx_hxx__

#include <xwos/osal/lock/mtx.hxx>
#include <xwos/cxx/lock/Mtx.hxx>
#include <xwos/cxx/sync/Cond.hxx>

namespace xwos {
namespace lock {

/**
 * @defgroup xwos_cxx_lock_DMtx 动态互斥锁
 * @ingroup xwos_cxx_lock
 *
 * 动态互斥锁是指互斥锁所需要的内存在编译期由编译器分配。
 *
 *
 * ## 动态互斥锁的RAII机制守卫
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
 * #include <xwos/cxx/lock/DMtx.hxx>
 *
 * void foo()
 * {
 *     xwos::lock::DMtx * dlock = new xwos::lock::DMtx(); // 创建动态互斥锁
 *     { // 构造 `grd` 时会自动对互斥锁上锁
 *         xwos::lock::Mtx::Grd grd(dlock, xwos::lock::Mtx::LockMode::MtxLock);
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
 * 头文件： @ref xwos/cxx/lock/DMtx.hxx
 *
 * @{
 */

/**
 * @brief 动态互斥锁
 */
class DMtx
    : public Mtx
{
  private:
    xwos_mtx_d mMtxDesc; /**< 互斥锁对象描述符 */
    xwer_t mCtorRc; /**< 互斥锁构造的结果 */

  public:
    /**
     * @brief 构造函数
     * @param[in] pr: 互斥锁的天花板优先级
     */
    explicit DMtx(xwpr_t pr = XWOS_SKD_PRIORITY_RT_MAX) {
        mCtorRc = xwos_mtx_create(&mMtxDesc, pr);
        if (XWOK == mCtorRc) {
            Mtx::mMtxPtr = mMtxDesc.mtx;
        }
    }

    ~DMtx() { xwos_mtx_delete(mMtxDesc); } /**< 析构函数 */
    xwer_t getCtorRc() { return mCtorRc; } /**< 获取动态互斥锁构造的结果 */

    /* 生命周期管理 */
    xwer_t acquire() { return xwos_mtx_acquire(mMtxDesc); } /**< 增加引用计数 */
    xwer_t release() { return xwos_mtx_release(mMtxDesc); } /**< 减少引用计数 */
};

/**
 * @} xwos_cxx_lock_DMtx
 */

} // namespace lock
} // namespace xwos

#endif /* xwos/cxx/lock/DMtx.hxx */
