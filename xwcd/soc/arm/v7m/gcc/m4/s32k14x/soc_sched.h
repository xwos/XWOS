/**
 * @file
 * @brief SOC Adapter Code：Scheduler
 * @author
 * + 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <www.starsoul.tech>
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

#ifndef __soc_sched_h__
#define __soc_sched_h__

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <arch_sched.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
struct xwos_scheduler;
struct xwos_sdobj_stack_info;
struct xwos_tcb;

/******** ******** ******** ******** ******** ******** ******** ********
 ********            XWOS Scheduler Adapter Functions           ********
 ******** ******** ******** ******** ******** ******** ******** ********/
__xwbsp_init_code
xwer_t soc_scheduler_init(struct xwos_scheduler * xwsd);

__xwbsp_code
void soc_scheduler_init_sdobj_stack(struct xwos_sdobj_stack_info * stk,
                                    xwsq_t attr);

__xwbsp_code
xwer_t soc_scheduler_start_lc(struct xwos_scheduler * xwsd);

__xwbsp_code
void soc_scheduler_req_swcx(struct xwos_scheduler * xwsd);

__xwbsp_code
xwer_t soc_scheduler_suspend(struct xwos_scheduler * xwsd);

__xwbsp_code
xwer_t soc_scheduler_resume(struct xwos_scheduler * xwsd);

__xwbsp_code
void soc_thrd_exit_lc(struct xwos_tcb * tcb, xwer_t rc);

__xwbsp_code
xwer_t soc_thrd_freeze_lc(struct xwos_tcb * tcb);

__xwbsp_code
xwer_t soc_thrd_thaw(struct xwos_tcb * tcb);

#if defined(XuanWuOS_CFG_CORE__smp)
__xwbsp_code
xwer_t soc_thrd_outmigrate(struct xwos_tcb * tcb, xwid_t cpuid);

__xwbsp_code
void soc_thrd_immigrate(struct xwos_tcb * tcb, xwid_t cpuid);
#endif /* XuanWuOS_CFG_CORE__smp */

#endif /* soc_sched.h */
