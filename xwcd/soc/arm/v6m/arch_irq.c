/**
 * @file
 * @brief 架构描述层：中断
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
#include <armv6m_isa.h>
#include <armv6m_nvic.h>
#include <arch_irq.h>
#include <arch_skd.h>
#include <arch_nvic.h>

#define SOC_EXC_FAULT_PRIO (SOC_IRQ_PRIO_HIGHEST)
#define SOC_EXC_SVC_PRIO (SOC_IRQ_PRIO_HIGHEST)

/**
 * @brief Setup Architecture Fault
 */
__xwbsp_init_code
void arch_init_sysirqs(void)
{
        /* init faults */
        cm_nvic_set_sysirq_priority(SOC_EXC_MMFAULT, SOC_EXC_FAULT_PRIO);
        cm_nvic_enable_memfault();

        cm_nvic_set_sysirq_priority(SOC_EXC_BUSFAULT, SOC_EXC_FAULT_PRIO);
        cm_nvic_enable_busfault();

        cm_nvic_set_sysirq_priority(SOC_EXC_USGFAULT, SOC_EXC_FAULT_PRIO);
        cm_nvic_enable_usgfault();

        cm_nvic_enable_faults();

        cm_nvic_set_sysirq_priority(SOC_EXC_SVCALL, SOC_EXC_SVC_PRIO);
}

/**
 * @brief Reset ISR
 */
__xwbsp_isr __xwcc_naked
void arch_isr_reset(void)
{
        __asm__ volatile(".syntax       unified");
        __asm__ volatile("      cpsid   i");
        __asm__ volatile("      bl      xwos_preinit");
        __asm__ volatile("      bl      xwos_init");
        __asm__ volatile("      bl      xwos_postinit");
        __asm__ volatile("      bl      xwos_main");
}

/**
 * @brief NMI ISR
 */
__xwbsp_isr
void arch_isr_nmi(void)
{
        __xwcc_unused volatile struct cm_scs_reg * scs;

        scs = &cm_scs;
        while (true) {
        }
}

/**
 * @brief Hardfault ISR
 */
__xwbsp_isr
void arch_isr_hardfault(void)
{
        __xwcc_unused volatile struct cm_scs_reg * scs;

        scs = &cm_scs;
        arch_skd_chk_stk();
        while (true) {
        }
}

/**
 * @brief MM ISR
 */
__xwbsp_isr
void arch_isr_mm(void)
{
        __xwcc_unused volatile struct cm_scs_reg * scs;

        scs = &cm_scs;
        arch_skd_chk_stk();
        while (true) {
        }
}

/**
 * @brief busfault ISR
 */
__xwbsp_isr
void arch_isr_busfault(void)
{
        __xwcc_unused volatile struct cm_scs_reg * scs;

        scs = &cm_scs;
        arch_skd_chk_stk();
        while (true) {
        }
}

/**
 * @brief usagefault ISR
 */
__xwbsp_isr
void arch_isr_usagefault(void)
{
        __xwcc_unused volatile struct cm_scs_reg * scs;

        scs = &cm_scs;
        arch_skd_chk_stk();
        while (true) {
        }
}

/**
 * @brief debugmon ISR
 */
__xwbsp_isr
void arch_isr_dbgmon(void)
{
        __xwcc_unused volatile struct cm_scs_reg * scs;

        scs = &cm_scs;
        while (true) {
        }
}

/**
 * @brief SVC ISR
 * @note
 * - SVC handler中不能再次调用svc指令，否则会造成Hard Fault，且scs.scb.hfsr.bit.forced
 *   会被置位。PendSV则没有这个限制。参考ARMv7-M_RM.pdf第B1-585页。
 *   SVC的处理机制是Fault，当无法执行时会造成Hard Fault。
 */
__xwbsp_isr __xwcc_naked
void arch_isr_svc(void)
{
        __asm__ volatile(".syntax       unified");
        __asm__ volatile("      movs    r0, #4");
        __asm__ volatile("      mov     r1, lr");
        __asm__ volatile("      tst     r1, r0"); /* update Z flag in EPSR */
        __asm__ volatile("      bne     1f");
        __asm__ volatile("      mrs     r0, msp"); /* if Z == 1 (bit is 0), use MSP */
        __asm__ volatile("      b       2f");
        __asm__ volatile("1:");
        __asm__ volatile("      mrs     r0, psp"); /* if Z == 0 (bit is 1), use PSP */
        __asm__ volatile("2:");
        __asm__ volatile("      ldr     r1, [r0, #24]"); /* get old pc value */
        __asm__ volatile("      subs    r1, #2");
        __asm__ volatile("      ldrb    r2, [r1]"); /* get svc number */
        __asm__ volatile("      lsls    r2, r2, #2"); /* get offset */
        __asm__ volatile("      ldr     r1, =svc_table"); /* get base */
        __asm__ volatile("      ldr     r3, [r1, r2]"); /* get entry */
        __asm__ volatile("      bx      r3");
        __asm__ volatile(".align        4\n"
                         "svc_table:\n" /* Set the lsb to 1 because of the THUMB mode */
                         ".4byte        svc_0 + 1\n"
                         ".4byte        svc_1 + 1\n"
                         ".4byte        svc_2 + 1\n"
                         ".4byte        svc_3 + 1\n"
                         ".4byte        svc_4 + 1\n"
                         ".4byte        svc_5 + 1\n"
                         ".4byte        svc_6 + 1\n"
                         ".4byte        svc_7 + 1\n"
                         ".4byte        svc_8 + 1\n"
                         ".4byte        svc_9 + 1\n"
                         ".4byte        svc_10 + 1\n"
                         ".4byte        svc_11 + 1\n"
                         ".4byte        svc_12 + 1\n"
                         ".4byte        svc_13 + 1\n"
                         ".4byte        svc_14 + 1\n"
                         ".4byte        svc_15 + 1\n");
        __asm__ volatile("svc_0:"); /* case 0: */
        __asm__ volatile("      bx      lr");
        __asm__ volatile("svc_1:"); /* case 1: */
        __asm__ volatile("      bx      lr");
        __asm__ volatile("svc_2:"); /* case 2: */
        __asm__ volatile("      bx      lr");
        __asm__ volatile("svc_3:"); /* case 3: */
        __asm__ volatile("      bx      lr");
        __asm__ volatile("svc_4:"); /* case 4: */
        __asm__ volatile("      bx      lr");
        __asm__ volatile("svc_5:"); /* case 5: */
        __asm__ volatile("      bx      lr");
        __asm__ volatile("svc_6:"); /* case 6: */
        __asm__ volatile("      bx      lr");
        __asm__ volatile("svc_7:"); /* case 7: */
        __asm__ volatile("      bx      lr");
        __asm__ volatile("svc_8:"); /* case 8: XWSC */
        __asm__ volatile("      bx      lr");
        __asm__ volatile("svc_9:"); /* case 9: Start XWOS */
        __asm__ volatile("      ldr     r0, [r0, #0]"); /* get old r0 */
        __asm__ volatile("      b       arch_svc_skd_start");
        __asm__ volatile("svc_10:"); /* case 10: Freeze Thread */
        __asm__ volatile("      push    {r0, lr}");
        __asm__ volatile("      ldr     r0, [r0, #0]"); /* get old r0 value: cthd */
        __asm__ volatile("      bl      xwosplcb_thd_freeze_lic");
        __asm__ volatile("      mov     r2, r0");
        __asm__ volatile("      pop     {r0, r1}");
        __asm__ volatile("      str     r2, [r0, #0]"); /* save the return value */
        __asm__ volatile("      mov     lr, r1");
        __asm__ volatile("      bx      lr");
        __asm__ volatile("svc_11:"); /* case 11: thread exits */
        __asm__ volatile("      push    {r0, lr}");
        __asm__ volatile("      ldr     r1, [r0, #4]"); /* get old r1 value */
        __asm__ volatile("      ldr     r0, [r0, #0]"); /* get old r0 value */
        __asm__ volatile("      bl      xwosplcb_thd_exit_lic");
        __asm__ volatile("      mov     r2, r0");
        __asm__ volatile("      pop     {r0, r1}");
        __asm__ volatile("      str     r2, [r0, #0]"); /* save the return value */
        __asm__ volatile("      mov     lr, r1");
        __asm__ volatile("      bx      lr");
        __asm__ volatile("svc_12:"); /* case 12: Suspend scheduler */
        __asm__ volatile("      push    {r0, lr}");
        __asm__ volatile("      ldr     r0, [r0, #0]"); /* get old r0 value */
        __asm__ volatile("      bl      xwosplcb_skd_suspend_lic");
        __asm__ volatile("      mov     r2, r0");
        __asm__ volatile("      pop     {r0, r1}");
        __asm__ volatile("      str     r2, [r0, #0]"); /* save the return value */
        __asm__ volatile("      mov     lr, r1");
        __asm__ volatile("      bx      lr");
        __asm__ volatile("svc_13:"); /* case 13: Resume scheduler */
        __asm__ volatile("      push    {r0, lr}");
        __asm__ volatile("      ldr     r0, [r0, #0]"); /* get old r0 value */
        __asm__ volatile("      bl      xwosplcb_skd_resume_lic");
        __asm__ volatile("      mov     r2, r0");
        __asm__ volatile("      pop     {r0, r1}");
        __asm__ volatile("      str     r2, [r0, #0]"); /* save the return value */
        __asm__ volatile("      mov     lr, r1");
        __asm__ volatile("      bx      lr");
        __asm__ volatile("svc_14:"); /* case 14: */
        __asm__ volatile("      bx      lr");
        __asm__ volatile("svc_15:"); /* case 14: */
        __asm__ volatile("      bx      lr");
}

/**
 * @brief ARCH default ISR
 */
__xwbsp_isr
void arch_isr_noop(void)
{
        __xwcc_unused volatile struct cm_scs_reg * scs;
        __xwcc_unused xwer_t rc;
        __xwcc_unused xwirq_t irqn;

        rc = arch_nvic_irq_get_id(&irqn);
        scs = &cm_scs;
        while (true) {
        }
}
