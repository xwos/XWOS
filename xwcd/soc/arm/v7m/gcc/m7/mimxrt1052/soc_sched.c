/**
 * @file
 * @brief 玄武OS内核适配代码：调度器
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > Licensed under the Apache License, Version 2.0 (the "License");
 * > you may not use this file except in compliance with the License.
 * > You may obtain a copy of the License at
 * >
 * >         http://www.apache.org/licenses/LICENSE-2.0
 * >
 * > Unless required by applicable law or agreed to in writing, software
 * > distributed under the License is distributed on an "AS IS" BASIS,
 * > WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * > See the License for the specific language governing permissions and
 * > limitations under the License.
 */

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <xwos/irq.h>
#if defined(XuanWuOS_CFG_CORE__smp)
  #include <xwos/smp/scheduler.h>
  #include <xwos/smp/thread.h>
#elif defined(XuanWuOS_CFG_CORE__up)
  #include <xwos/up/scheduler.h>
  #include <xwos/up/thread.h>
#else
  #error "Can't find the configuration of XuanWuOS_CFG_CORE!"
#endif
#include <armv7m_core.h>
#include <soc_irq.h>
#include <arch_sched.h>
#include <soc_sched.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         function prototypes         ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief 玄武OS内核调度器适配函数：初始化调度调度器
 * @param xwsd: (I) 调度器的指针
 * @return 错误码
 */
__xwbsp_init_code
xwer_t soc_scheduler_init(struct xwos_scheduler * xwsd)
{
        return arch_scheduler_lib_init_pendsv(xwsd);
}

/**
 * @brief 玄武OS内核调度器适配函数：初始化调度对象的栈
 * @param stk: (I) 栈信息结构体指针
 * @param attr: (I) 标志
 */
__xwbsp_code
void soc_scheduler_init_sdobj_stack(struct xwos_sdobj_stack_info * stk,
                                    xwsq_t attr)
{
        arch_scheduler_lib_init_sdobj_stack(stk, attr);
}

/**
 * @brief 玄武OS内核调度器适配函数：启动调度器
 * @param xwsd: (I) XWOS调度器的指针
 */
__xwbsp_code __naked
xwer_t soc_scheduler_start_lc(__maybe_unused struct xwos_scheduler * xwsd)
{
        xwos_cpuirq_enable_lc();
        __asm__ volatile("      svc     0");
        return XWOK;
}

/**
 * @brief 玄武OS内核调度器适配函数：发起切换上下文的软中断
 * @param xwsd: (I) XWOS调度器的指针
 * @note
 * - XWOS的线程上下文切换过程中是开中断的，在切换的过程中（处理器正在执行
 *   @ref soc_scheduler_swcx_isr()函数）有可能因中断再一次发起切换请求。
 *   因为多次SVC会造成Hard Fault，所以不能通过SVC来实现切换，只能
 *   通过PendSV或SOC自带的外部软件中断来实现。
 */
__xwbsp_code
void soc_scheduler_req_swcx(struct xwos_scheduler * xwsd)
{
        XWOS_UNUSED(xwsd);
        cm_nvic_pend_pendsv();
}

/**
 * @brief 玄武OS内核调度器适配函数：暂停调度器
 * @param xwsd: (I) XWOS调度器的指针
 * @return 错误码
 */
__xwbsp_code __naked
xwer_t soc_scheduler_suspend(__maybe_unused struct xwos_scheduler * xwsd)
{
        __asm__ volatile(
        "       svc     9\n"
        "       bx      lr\n");
}

/**
 * @brief 玄武OS内核调度器适配函数：继续调度器
 * @param xwsd: (I) XWOS调度器的指针
 * @return 错误码
 */
__xwbsp_code __naked
xwer_t soc_scheduler_resume(__maybe_unused struct xwos_scheduler * xwsd)
{
        __asm__ volatile(
        "       svc     10\n"
        "       bx      lr\n");
}

/**
 * @brief 玄武OS内核调度器适配函数：本地CPU上的线程退出
 * @param tcb: (I) 线程控制块对象的指针
 * @param rc: (I) 线程退出抛出的返回值
 */
__xwbsp_code __naked
void soc_thrd_exit_lc(__maybe_unused struct xwos_tcb * tcb,
                      __maybe_unused xwer_t rc)
{
        __asm__ volatile(
        "       svc     7\n");
        while (true) {
        }
        __asm__ volatile(
        "       bx      lr\n");
}

/**
 * @brief 玄武OS内核调度器适配函数：冻结本地CPU中正在运行的线程
 * @param tcb: (I) 线程控制块对象的指针
 */
__xwbsp_code __naked
xwer_t soc_thrd_freeze_lc(__maybe_unused struct xwos_tcb * tcb)
{
        __asm__ volatile(
        "       svc     5\n"
        "       bx      lr\n");
}

#if defined(XuanWuOS_CFG_CORE__smp)
/**
 * @brief 玄武OS内核调度器适配函数：将线程迁出其他CPU，并准备迁入其他CPU
 * @param tcb: (I) 线程控制块对象的指针
 * @param cpuid: (I) 目的地CPU的ID
 * @return 错误码
 */
__xwbsp_code __naked
xwer_t soc_thrd_outmigrate(__maybe_unused struct xwos_tcb * tcb,
                           __maybe_unused xwid_t cpuid)
{
        __asm__ volatile(
        "       svc     11\n"
        "       bx      lr\n");
}

/**
 * @brief 玄武OS内核调度器适配函数：迁移线程至目标CPU
 * @param tcb: (I) 线程控制块对象的指针
 * @param cpuid: (I) 目的地CPU的ID
 */
__xwbsp_code
void soc_thrd_immigrate(struct xwos_tcb * tcb, xwid_t cpuid)
{
        XWOS_UNUSED(cpuid);
        xwos_thrd_immigrate_lic(tcb);
}
#endif /* XuanWuOS_CFG_CORE__smp */
