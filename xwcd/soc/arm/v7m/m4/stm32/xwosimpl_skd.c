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
#include <xwcd/soc/arm/v7m/arch_irq.h>
#include <xwcd/soc/arm/v7m/arch_skd.h>

__xwbsp_code
xwer_t xwospl_skd_init(__unused struct xwospl_skd * xwskd)
{
        arch_skd_init_pendsv();
        return XWOK;
}

__xwbsp_code
void xwospl_skd_init_stack(struct xwospl_skdobj_stack * stk,
                           void (* exit)(xwer_t))
{
        arch_skd_init_stack(stk, exit);
}

__xwbsp_code
xwid_t xwospl_skd_get_cpuid_lc(void)
{
        return 0;
}

__xwbsp_code __xwcc_naked
xwer_t xwospl_skd_start(__xwcc_unused struct xwospl_skd * xwskd)
{
        __asm__ volatile(
                ".syntax        unified");
        armv7m_dsb();
        armv7m_isb();
        __asm__ volatile(
                "       cpsie   i\n"
                "       svc     9\n"
                "       bx      lr\n");
}

__xwbsp_code __xwcc_naked
xwer_t xwospl_thd_freeze_lc(__xwcc_unused struct xwospl_thd * thd)
{
        __asm__ volatile(
                ".syntax        unified");
        __asm__ volatile(
                "       svc     10\n"
                "       bx      lr\n");
}

__xwbsp_code __xwcc_naked
void xwospl_thd_exit_lc(__xwcc_unused struct xwospl_thd * thd,
                        __xwcc_unused xwer_t rc)
{
        __asm__ volatile(
                ".syntax        unified");
        __asm__ volatile(
                "       svc     11\n"
                "       b       .\n"
                "       bx      lr\n");
}

__xwbsp_code __xwcc_naked
xwer_t xwospl_skd_suspend(__xwcc_unused struct xwospl_skd * xwskd)
{
        __asm__ volatile(
                ".syntax        unified");
        __asm__ volatile(
                "       svc     12\n"
                "       bx      lr\n");
}

__xwbsp_code __xwcc_naked
xwer_t xwospl_skd_resume(__xwcc_unused struct xwospl_skd * xwskd)
{
        __asm__ volatile(
                ".syntax        unified");
        __asm__ volatile(
                "       svc     13\n"
                "       bx      lr\n");
}

#if (1 == XWOSRULE_THD_MIGRATION)
__xwbsp_code __xwcc_naked
xwer_t xwospl_thd_outmigrate(__xwcc_unused struct xwospl_thd * thd,
                             __xwcc_unused xwid_t cpuid)
{
        __asm__ volatile(
                ".syntax        unified");
        __asm__ volatile(
                "       svc     14\n"
                "       bx      lr\n");
}

__xwbsp_code
void xwospl_thd_immigrate(struct xwospl_thd * thd, xwid_t cpuid)
{
        XWOS_UNUSED(cpuid);
        xwosplcb_thd_immigrate_lic(thd);
}
#endif
