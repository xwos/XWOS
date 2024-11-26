/**
 * @file
 * @brief xwos::DThd
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef __xwos_cxx_DThd_hxx__
#define __xwos_cxx_DThd_hxx__

#include <xwos/osal/thd.hxx>

/**
 * @defgroup xwos_DThd 静态线程
 * @ingroup xwos_cxx
 *
 * 动态线程是指线程所需要的内存空间是在运行时动态申请的。
 *
 *
 * ## C++ API
 *
 * 头文件： @ref xwos/cxx/DThd.hxx
 *
 * @{
 */

namespace xwos {

/**
 * @brief 动态线程
 */
class DThd
{
  private:
    xwos_thd_d mThdDesc; /**< 线程描述符 */
    xwer_t mCtorRc; /**< 线程构造的结果 */

  public:
    /**
     * @brief 动态线程构造函数
     * @param[in] name: 线程名
     * @param[in] stack_size: 线程栈的大小
     * @param[in] stack_guard_size: 栈内存警戒线位置
     * @param[in] priority: 优先级
     * @param[in] detached: 是否为分离态
     * @param[in] privileged: 是否为特权线程
     */
    DThd(const char * name, xwsz_t stack_size,
         xwsz_t stack_guard_size = XWOS_STACK_GUARD_SIZE_DEFAULT,
         xwpr_t priority = XWOS_SKD_PRIORITY_RT_MIN, bool detached = false,
         bool privileged = true);
    ~DThd(); /**< 动态线程析构函数 */
    xwer_t getCtorRc() { return mCtorRc; } /**< 获取动态线程构造的结果 */
    xwer_t intr() { return xwos_thd_intr(mThdDesc); } /**< 中断线程的阻塞态和睡眠态 */
    xwer_t quit() { return xwos_thd_quit(mThdDesc); } /**< 通知线程退出 */
    xwer_t join(xwer_t * trc) { return xwos_thd_join(mThdDesc, trc); } /**< 等待线程结束并获取它的返回值 */
    xwer_t stop(xwer_t * trc) { return xwos_thd_stop(mThdDesc, trc); } /**< 通知线程退出，等待线程结束并获取它的返回值 */
    xwer_t detach() { return xwos_thd_detach(mThdDesc); } /**< 终止线程并等待它退出 */

    /* 生命周期管理 */
    xwer_t acquire() { return xwos_thd_acquire(mThdDesc); } /**< 增加引用计数 */
    xwer_t release() { return xwos_thd_release(mThdDesc); } /**< 减少引用计数 */

  protected:
    virtual xwer_t thdMainFunction(); /**< 线程主函数，用户需要重新实现此函数 */
    /* 只能在当前线程函数中调用的API */
    void yield() { xwos_cthd_yield(); } /**< 当前线程通知调度器重新调度 */
    void exit(xwer_t rc) { xwos_cthd_exit(rc); } /**< 退出当前线程 */
    bool shouldFreeze() { return xwos_cthd_shld_frz(); } /**< 判断当前线程是否可被冻结 */
    bool shouldStop() { return xwos_cthd_shld_stop(); } /**< 判断当前线程是否可以退出 */
    xwer_t sleep(xwtm_t dur) { return xwos_cthd_sleep(dur); } /**< 当前线程睡眠一段时间 */
    xwer_t sleepTo(xwtm_t to) { return xwos_cthd_sleep_to(to); } /**< 当前线程睡眠到一个时间点 */
    xwer_t sleepFrom(xwtm_t * from, xwtm_t dur) { return xwos_cthd_sleep_from(from, dur); } /**< 当前线程从一个时间起点睡眠到另一个时间点 */
    xwer_t freeze(void) { return xwos_cthd_shld_stop(); } /**< 冻结当前线程 */

  private:
    static xwer_t sThdMainFunction(DThd * thd);
};

} // namespace xwos

/**
 * @} xwos_DThd
 */

#endif /* xwos/cxx/DThd.hxx */
