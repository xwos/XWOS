/**
 * @file
 * @brief xwos::SThd
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef __xwos_cxx_SThd_hxx__
#define __xwos_cxx_SThd_hxx__

#include <xwos/osal/thd.hxx>

namespace xwos {

/**
 * @defgroup xwos_cxx_Thd_SThd 静态线程
 * @ingroup xwos_cxx_Thd
 *
 * 静态线程是指线程所需要的内存在编译期由编译器分配。
 *
 *
 * ## 头文件
 *
 * @ref xwos/cxx/SThd.hxx
 *
 * @{
 */

/**
 * @brief 静态线程
 */
class SThd
{
  private:
    xwos_thd_d mThdDesc; /**< 线程描述符 */
    struct xwos_thd mThd; /**< 线程结构体 */
    xwer_t mCtorRc; /**< 线程构造的结果 */

  public:
    /**
     * @brief 静态线程构造函数
     * @param[in] name: 线程名
     * @param[in] stack: 线程栈内存
     * @param[in] stack_size: 线程栈的大小
     * @param[in] stack_guard_size: 栈内存警戒线位置
     * @param[in] priority: 优先级
     * @param[in] detached: 是否为分离态
     * @param[in] privileged: 是否为特权线程
     */
    SThd(const char * name, xwstk_t * stack, xwsz_t stack_size,
         xwsz_t stack_guard_size = XWOS_STACK_GUARD_SIZE_DEFAULT,
         xwpr_t priority = XWOS_SKD_PRIORITY_RT_MIN, bool detached = false,
         bool privileged = true);
    ~SThd(); /**< 静态线程析构函数 */
    xwer_t getCtorRc() { return mCtorRc; } /**< 获取静态线程构造的结果 */

    /**
     * @brief 中断线程的阻塞态和睡眠态
     */
    xwer_t intr() { return xwos_thd_intr(mThdDesc); }
    /**
     * @brief通知线程退出
     */
    xwer_t quit() { return xwos_thd_quit(mThdDesc); }
    /**
     * @brief 等待线程结束并获取它的返回值
     */
    xwer_t join(xwer_t * trc)
    {
        return xwos_thd_join(mThdDesc, trc);
    }
    /**
     * @brief 通知线程退出，等待线程结束并获取它的返回值
     */
    xwer_t stop(xwer_t * trc)
    {
        return xwos_thd_stop(mThdDesc, trc);
    }
    /**
     * @brief 终止线程并等待它退出
     */
    xwer_t detach() { return xwos_thd_detach(mThdDesc); }
    /**
     * @brief 获取XWOS对象指针
     */
    struct xwos_thd * getXwosObj() { return mThdDesc.thd; }

    /* 生命周期管理 */
    xwer_t grab() { return xwos_thd_grab(mThdDesc); } /**< 增加引用计数 */
    xwer_t put() { return xwos_thd_put(mThdDesc); } /**< 减少引用计数 */

  protected:
    /* 线程主函数 */
    /**
     * @brief 线程主函数，用户需要重新实现此函数
     */
    virtual xwer_t thdMainFunction();

    /* 只能在当前线程函数中调用的API */
    /**
     * @brief 当前线程通知调度器重新调度
     */
    void yield()
    {
        xwos_cthd_yield();
    }
    /**
     * @brief 退出当前线程
     * @param[in] rc: 线程退出时抛出的返回值
     */
    void exit(xwer_t rc)
    {
        xwos_cthd_exit(rc);
    }
    /**
     * @brief 判断当前线程是否可被冻结
     */
    bool shouldFreeze()
    {
        return xwos_cthd_shld_frz();
    }
    /**
     * @brief 判断当前线程是否可以退出
     */
    bool shouldStop()
    {
        return xwos_cthd_shld_stop();
    }
    /**
     * @brief 当前线程睡眠一段时间
     * @param[in] dur: 期望睡眠的时间
     */
    xwer_t sleep(xwtm_t dur)
    {
        return xwos_cthd_sleep(dur);
    }
    /**
     * @brief 当前线程睡眠到一个时间点
     * @param[in] to: 期望唤醒的时间点
     */
    xwer_t sleepTo(xwtm_t to)
    {
        return xwos_cthd_sleep_to(to);
    }
    /**
     * @brief 当前线程从一个时间起点睡眠到另一个时间点
     * @param[in,out] from: 指向缓冲区的指针，此缓冲区：
     * + (I) 输入时，作为时间起点
     * + (O) 输出时，返回线程被唤醒的时间（可作为下一次时间起点）
     * @param[in] dur: 期望被唤醒的时间增量（相对于时间原点）
     */
    xwer_t sleepFrom(xwtm_t * from, xwtm_t dur)
    {
        return xwos_cthd_sleep_from(from, dur);
    }
    /**
     * @brief 冻结当前线程
     */
    xwer_t freeze(void)
    {
        return xwos_cthd_freeze();
    }

  private:
    static xwer_t sThdMainFunction(SThd * thd);
    static void * operator new(xwsz_t sz) = delete;
    void operator delete(void * obj) = delete;
};

/**
 * @} xwos_cxx_SThd
 */

} // namespace xwos

#endif /* xwos/cxx/SThd.hxx */
