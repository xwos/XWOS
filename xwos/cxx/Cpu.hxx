/**
 * @file
 * @brief xwos::Cpu
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef __xwos_cxx_Cpu_hxx__
#define __xwos_cxx_Cpu_hxx__

#include <xwos/osal/skd.hxx>
#include <xwos/osal/irq.hxx>

namespace xwos {

/**
 * @defgroup xwos_cxx_Cpu CPU库
 * @ingroup xwos_cxx
 *
 * CPU、调度器、中断、上下文相关的库。
 *
 * ## C++ API
 *
 * 头文件： @ref xwos/cxx/Cpu.hxx
 *
 * @{
 */

class Cpu
{
  private:
    Cpu() {}
    ~Cpu() {}

  public:
    static xwer_t startLocalSchedule()
    {
        return xwos_skd_start_lc();
    } /**< 启动当前CPU的调度器 */
    static xwer_t pauseLocalScheduler()
    {
        return xwosdl_skd_pause_lc();
    } /**< 暂停本地CPU调度器 */
    static xwer_t continueLocalScheduler()
    {
        return xwos_skd_continue_lc();
    } /**< 继续运行本地CPU调度器 */
    static xwid_t getLocalCpuId()
    {
        return xwos_skd_get_cpuid_lc();
    } /**< 获取当前CPU的ID */
    static void getContext(xwsq_t * ctxbuf, xwirq_t * irqnbuf)
    {
        xwos_skd_get_context_lc(ctxbuf, irqnbuf);
    } /**< 获取当前代码的上下文 */
    static void disableLocalPreemption()
    {
        xwos_skd_dspmpt_lc();
    } /**< 关闭本地CPU调度器的抢占 */
    static void enableLocalPreemption()
    {
        xwos_skd_enpmpt_lc();
    } /**< 开启本地CPU调度器的抢占 */
    static void disableLocalBh() { xwos_skd_dsbh_lc(); } /**< 关闭本地CPU的中断底半部 */
    static void enableLocalBh() { xwos_skd_enbh_lc(); } /**< 开启本地CPU的中断底半部 */
    static void disableLocalIrq()
    {
        xwos_cpuirq_disable_lc();
    } /**< 关闭本地CPU的中断开关（不可嵌套使用） */
    static void enableLocalIrq()
    {
        xwos_cpuirq_enable_lc();
    } /**< 开启本地CPU的中断开关（不可嵌套使用） */
    static void suspendLocalIrq()
    {
        xwos_cpuirq_suspend_lc();
    } /**< 暂停本地CPU的中断（可嵌套使用） */
    static void resumeLocalIrq()
    {
        xwos_cpuirq_resume_lc();
    } /**< 恢复本地CPU的中断（可嵌套使用） */
    static void saveLocalIrq(xwreg_t * cpuirq)
    {
        xwos_cpuirq_save_lc(cpuirq);
    } /**< 保存本地CPU的中断开关然后关闭（可嵌套使用） */
    static void restoreLocalIrq(xwreg_t cpuirq)
    {
        xwos_cpuirq_restore_lc(cpuirq);
    } /**< 恢复本地CPU的中断（可嵌套使用） */
    static bool testLocalIrq()
    {
        return xwos_cpuirq_test_lc();
    } /**< 测试本地CPU的中断开关状态 */

    static void disableIrq(xwirq_t irqn)
    {
        xwos_irq_disable(irqn);
    } /**< 关闭某个外设中断 */
    static void enableIrq(xwirq_t irqn)
    {
        xwos_irq_enable(irqn);
    } /**< 开启某个外设中断 */
    static void saveIrq(xwirq_t irqn, xwreg_t * flag)
    {
        xwos_irq_save(irqn, flag);
    } /**< 保存某个外设中断的开关，然后将其关闭 */
    static void restoreIrq(xwirq_t irqn, xwreg_t flag)
    {
        xwos_irq_restore(irqn, flag);
    } /**< 恢复某个外设中断的开关 */
};

/**
 * @} xwos_cxx_Cpu
 */

} // namespace xwos

#endif /* xwos/cxx/Cpu.hxx */
