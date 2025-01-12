/**
 * @file
 * @brief xwos::CThd
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef __xwos_cxx_CThd_hxx__
#define __xwos_cxx_CThd_hxx__

#include <xwos/osal/thd.hxx>

namespace xwos {

/**
 * @defgroup xwos_cxx_Thd_CThd 只作用于调用线程的函数集合
 * @ingroup xwos_cxx_Thd
 *
 *
 * ## 头文件
 *
 * @ref xwos/cxx/CThd.hxx
 *
 * @{
 */

/**
 * @brief 只作用于调用线程的函数集合
 */
class CThd
{
  public:
    /**
     * @brief 当前线程通知调度器重新调度
     */
    static void yield() { xwos_cthd_yield(); }
    /**
     * @brief 退出当前线程
     * @param[in] rc: 线程退出时抛出的返回值
     */
    static void exit(xwer_t rc) { xwos_cthd_exit(rc); }
    /**
     * @brief 判断当前线程是否可被冻结
     */
    static bool shouldFreeze() { return xwos_cthd_shld_frz(); }
    /**
     * @brief 判断当前线程是否可以退出
     */
    static bool shouldStop() { return xwos_cthd_shld_stop(); }
    /**
     * @brief 当前线程睡眠一段时间
     * @param[in] dur: 期望睡眠的时间
     */
    static xwer_t sleep(xwtm_t dur) { return xwos_cthd_sleep(dur); }
    /**
     * @brief 当前线程睡眠到一个时间点
     * @param[in] to: 期望唤醒的时间点
     */
    static xwer_t sleepTo(xwtm_t to) { return xwos_cthd_sleep_to(to); }
    /**
     * @brief 当前线程从一个时间起点睡眠到另一个时间点
     * @param[in,out] from: 指向缓冲区的指针，此缓冲区：
     * + (I) 输入时，作为时间起点
     * + (O) 输出时，返回线程被唤醒的时间（可作为下一次时间起点）
     * @param[in] dur: 期望被唤醒的时间增量（相对于时间原点）
     */
    static xwer_t sleepFrom(xwtm_t * from, xwtm_t dur)
    {
        return xwos_cthd_sleep_from(from, dur);
    }
    /**
     * @brief 冻结当前线程
     */
    static xwer_t freeze(void) { return xwos_cthd_freeze(); }

  private:
    CThd() {}
    ~CThd() {}
};

/**
 * @} xwos_cxx_CThd
 */

} // namespace xwos

#endif /* xwos/cxx/CThd.hxx */
