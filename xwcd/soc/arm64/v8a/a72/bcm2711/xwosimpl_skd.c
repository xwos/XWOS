/**
 * @file
 * @brief XWOS移植实现层：调度
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
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

#include <xwos/standard.h>
#include <xwos/ospl/skd.h>
#include <xwcd/soc/arm64/v8a/arch_isa.h>
#include <xwcd/soc/arm64/v8a/arch_gic2.h>
#include <xwcd/soc/arm64/v8a/arch_skd.h>

__xwbsp_code
xwer_t xwospl_skd_init(__xwcc_unused struct xwospl_skd * xwskd)
{
        return XWOK;
}

__xwbsp_code
void xwospl_skd_init_stack(struct xwospl_skdobj_stack * stk,
                           void (* exit)(xwer_t))
{
        armv8a_skd_init_stack(stk, exit);
}

__xwbsp_code
xwid_t xwospl_skd_get_cpuid_lc(void)
{
        xwid_t cpuid;
        xwu64_t mpidr;

        armv8a_sysreg_read(&mpidr, mpidr_el1);
        cpuid = ((xwid_t)mpidr >> (xwid_t)8) & (xwid_t)0xFF;
        return cpuid;
}

extern xwu8_t soc_percpu_stack[CPUCFG_CPU_NUM][CPUCFG_CPU_STACK_SIZE];

__xwbsp_code
xwer_t xwospl_skd_start(struct xwospl_skd * xwskd)
{
        xwosplcb_skd_post_start_lic(xwskd);
        xwu8_t (* stack)[CPUCFG_CPU_STACK_SIZE] = &soc_percpu_stack[xwskd->id];
        armv8a_skd_start(xwskd, (xwstk_t *)stack);
        return XWOK;
}

__xwbsp_code
void xwospl_skd_req_swcx(struct xwospl_skd * xwskd)
{
        xwirq_t irqn;
        if (XWOK == armv8a_gic_irq_get_id(&irqn)) {
                armv8a_skd_swcx();
        } else {
                __asm__ volatile(
                        "       dsb     sy\n"
                        "       mov     x0, %[__xwskd]\n"
                        "       isb\n"
                        "       hvc     #8\n"
                        :
                        : [__xwskd] "r" (xwskd)
                        : "memory"
                                 );
        }
}

__xwbsp_code
void xwospl_thd_exit_lc(__xwcc_unused struct xwospl_thd * thd, __xwcc_unused xwer_t rc)
{
        __asm__ volatile(
        "       dsb     sy\n"
        "       isb\n"
        "       hvc     #2\n"
        :
        :
        : "memory"
        );
}

__xwbsp_code __xwcc_noreturn
xwer_t xwospl_thd_freeze_lc(__xwcc_unused struct xwospl_thd * thd)
{
        __asm__ volatile(
        "       dsb     sy\n"
        "       isb\n"
        "       hvc     #3\n"
        "       ret\n"
        :
        :
        : "x0", "memory"
        );
        /* 使用 `noreturn` 模拟 `naked` 属性，下列代码只为编译不报错，并不会执行到。*/
        while (true) {
        }
}

__xwbsp_code __xwcc_noreturn
xwer_t xwospl_skd_suspend(__xwcc_unused struct xwospl_skd * xwskd)
{
        __asm__ volatile(
        "       dsb     sy\n"
        "       isb\n"
        "       hvc     #4\n"
        "       ret\n"
        :
        :
        : "x0", "memory"
        );
        while (true) {
        }
}

__xwbsp_code __xwcc_noreturn
xwer_t xwospl_skd_resume(__xwcc_unused struct xwospl_skd * xwskd)
{
        __asm__ volatile(
        "       dsb     sy\n"
        "       isb\n"
        "       hvc     #5\n"
        "       ret\n"
        :
        :
        : "x0", "memory"
        );
        while (true) {
        }
}

#if (1 == XWOSRULE_THD_MIGRATION)
__xwbsp_code
xwer_t xwospl_thd_outmigrate(__xwcc_unused struct xwospl_thd * thd,
                             __xwcc_unused xwid_t cpuid)
{
        __asm__ volatile(
        "       dsb     sy\n"
        "       isb\n"
        "       hvc     #6\n"
        "       ret\n"
        :
        :
        : "x0", "memory"
        );
        while (true) {
        }
}

__xwbsp_code
void xwospl_thd_immigrate(struct xwospl_thd * thd, xwid_t cpuid)
{
        XWOS_UNUSED(cpuid);
        xwosplcb_thd_immigrate_lic(thd);
}
#endif
