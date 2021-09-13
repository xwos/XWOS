/**
 * @file
 * @brief 架构描述层：中断
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

#include <xwos/standard.h>
#include <armv7m_core.h>
#include <armv7m_nvic.h>
#include <arch_nvic.h>
#include <arch_irq.h>
#include <arch_skd.h>

#define SOC_EXC_FAULT_PRIO      (SOC_IRQ_PRIO_HIGHEST | SOC_IRQ_SUBPRIO_HIGHEST)
#define SOC_EXC_SVC_PRIO        (SOC_IRQ_PRIO_HIGHEST | SOC_IRQ_SUBPRIO_LOWEST)

xws64_t soc_xwsc_entry(xwsc_f func, xwptr_t argnum, xwptr_t * args, xwreg_t old_lr);

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
        __asm__ volatile("      bl      arch_lowlevel_init");
        __asm__ volatile("      bl      cpu_lowlevel_init");
        __asm__ volatile("      bl      soc_lowlevel_init");
        __asm__ volatile("      bl      board_lowlevel_init");
        __asm__ volatile("      bl      xwos_init");
        __asm__ volatile("      bl      arch_relocate");
        __asm__ volatile("      bl      arch_init");
        __asm__ volatile("      bl      cpu_init");
        __asm__ volatile("      bl      soc_init");
        __asm__ volatile("      bl      board_init");
#if defined(ARCHCFG_CXX) && (1 == ARCHCFG_CXX)
        __asm__ volatile("      bl      cxx_init");
#endif /* ARCHCFG_CXX */
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
 * @note
 * - xwsc (svc 8)
 *   + ARM内核进入handler模式时会自动将r0 ~ r3, ip,lr, pc & xpsr压栈保存；
 *     返回thread模式时会自动从栈中弹出原始值恢复这些寄存器。
 *     返回的地址被存放在[SP+24]的地址处。可以将这个内存地址中的内容改为xwsc的真正
 *     入口@ref soc_xwsc_entry()，并将原始返回地址存放在原始LR的位置[SP+20]。原始
 *     LR的值又存放在原始R3的位置[SP+12]。原始R3在xwsc中没有使用到，因此不需要备份。
 *     当svc_sysisr()返回时，就会进入到@ref soc_xwsc_entry()函数，并且以r0 ~ r2作为
 *     参数，函数的第四个参数（通过R3的位置传递）作为LR的原始值。
 *         ------------------------------\n
 *         | stack      | change        |\n
 *         ------------------------------\n
 *   sp+28 | xpsr       |               |\n
 *   sp+24 | pc         | real entry    |\n
 *   sp+20 | lr         | old PC        |\n
 *   sp+16 | r12        |               |\n
 *   sp+12 | r3         | old lr        |\n
 *   sp+8  | r2         | args addr     |\n
 *   sp+4  | r1         | args number   |\n
 *   sp->  | r0         | function      |\n
 *         ------------------------------\n
 */
__xwbsp_isr __xwcc_naked
void arch_isr_svc(void)
{
        __asm__ volatile(".syntax       unified");
        __asm__ volatile("      clrex");
        __asm__ volatile("      tst     lr, #0x4"); /* update Z flag in EPSR */
        __asm__ volatile("      ite     eq\n"
                         "      mrseq   r0, msp\n" /* if Z == 1 (bit is 0), use MSP */
                         "      mrsne   r0, psp"); /* if Z == 0 (bit is 1), use PSP */
        __asm__ volatile("      ldr     r1, [r0, #24]"); /* get old pc value */
        __asm__ volatile("      ldrb    r2, [r1, #-2]"); /* get svc number */
        __asm__ volatile("      tbh     [pc, r2, lsl #1]"); /* switch (r2) */
        __asm__ volatile("svc_table:"); /* Number of entry must be even. */
        __asm__ volatile(".2byte        (svc_0 - svc_table) / 2");
        __asm__ volatile(".2byte        (svc_1 - svc_table) / 2");
        __asm__ volatile(".2byte        (svc_2 - svc_table) / 2");
        __asm__ volatile(".2byte        (svc_3 - svc_table) / 2");
        __asm__ volatile(".2byte        (svc_4 - svc_table) / 2");
        __asm__ volatile(".2byte        (svc_5 - svc_table) / 2");
        __asm__ volatile(".2byte        (svc_6 - svc_table) / 2");
        __asm__ volatile(".2byte        (svc_7 - svc_table) / 2");
        __asm__ volatile(".2byte        (svc_8 - svc_table) / 2");
        __asm__ volatile(".2byte        (svc_9 - svc_table) / 2");
        __asm__ volatile(".2byte        (svc_10 - svc_table) / 2");
        __asm__ volatile(".2byte        (svc_11 - svc_table) / 2");
        __asm__ volatile(".align        2");
        __asm__ volatile("svc_0:"); /* case 0: */
        __asm__ volatile("      ldr     r0, [r0, #0]"); /* get old r0 */
        __asm__ volatile("      b       arch_skd_svcsr_start");
        __asm__ volatile("svc_1:"); /* case 1: privilege_start */
        __asm__ volatile("      mrs     r0, control");
        __asm__ volatile("      orr     r0, #1");
        __asm__ volatile("      msr     control, r0");
        __asm__ volatile("      bx      lr");
        __asm__ volatile("svc_2:"); /* case 2: privilege_end */
        __asm__ volatile("      mrs     r0, control");
        __asm__ volatile("      bic     r0, #1");
        __asm__ volatile("      msr     control, r0");
        __asm__ volatile("      bx      lr");
        __asm__ volatile("svc_3:"); /* case 3: */
        __asm__ volatile("      bx      lr");
        __asm__ volatile("svc_4:"); /* case 4: */
        __asm__ volatile("      bx      lr");
        __asm__ volatile("svc_5:"); /* case 5: freeze thread */
        __asm__ volatile("      push    {r0, lr}");
        __asm__ volatile("      ldr     r0, [r0, #0]"); /* get old r0 value: cthd */
        __asm__ volatile("      bl      xwosplcb_thd_freeze_lic");
        __asm__ volatile("      mov     r1, r0");
        __asm__ volatile("      pop     {r0, lr}");
        __asm__ volatile("      str     r1, [r0, #0]"); /* save the return value */
        __asm__ volatile("      bx      lr");
        __asm__ volatile("svc_6:"); /* case 6: */
        __asm__ volatile("      bx      lr");
        __asm__ volatile("svc_7:"); /* case 7: thread exits */
        __asm__ volatile("      push    {r0, lr}");
        __asm__ volatile("      ldr     r1, [r0, #4]"); /* get old r1 value */
        __asm__ volatile("      ldr     r0, [r0, #0]"); /* get old r0 value */
        __asm__ volatile("      bl      xwosplcb_thd_exit_lic");
        __asm__ volatile("      mov     r1, r0");
        __asm__ volatile("      pop     {r0, lr}");
        __asm__ volatile("      str     r1, [r0, #0]"); /* save the return value */
        __asm__ volatile("      bx      lr");
        __asm__ volatile("svc_8:"); /* case 8: xwsc */
        __asm__ volatile("      ldr     r1, [r0, #20]"); /* get old lr value */
        __asm__ volatile("      str     r1, [r0, #12]"); /* save old lr value */
        __asm__ volatile("      ldr     r1, [r0, #24]"); /* get old pc value */
        __asm__ volatile("      orr     r1, #1"); /* set lsb to 1, Thumb mode */
        __asm__ volatile("      str     r1, [r0, #20]"); /* save old pc value */
        __asm__ volatile("      mov     r1, %[__soc_xwsc_entry]"
                         :
                         : [__soc_xwsc_entry] "r" ((xwptr_t)soc_xwsc_entry)
                         : "r0", "r1", "r2", "memory");
        __asm__ volatile("      str     r1, [r0, #24]"); /* setup xwsc entry */
        __asm__ volatile("      mrs     r0, control"); /* open privileged access */
        __asm__ volatile("      bic     r0, #1");
        __asm__ volatile("      msr     control, r0");
        __asm__ volatile("      bx      lr");
        __asm__ volatile("svc_9:"); /* case 9: */
        __asm__ volatile("      push    {r0, lr}");
        __asm__ volatile("      ldr     r0, [r0, #0]"); /* get old r0 value: xwskd */
        __asm__ volatile("      bl      xwosplcb_skd_suspend_lic");
        __asm__ volatile("      mov     r1, r0");
        __asm__ volatile("      pop     {r0, lr}");
        __asm__ volatile("      str     r1, [r0, #0]"); /* save the return value */
        __asm__ volatile("      bx      lr");
        __asm__ volatile("svc_10:"); /* case 10: */
        __asm__ volatile("      push    {r0, lr}");
        __asm__ volatile("      ldr     r0, [r0, #0]"); /* get old r0 value: xwskd */
        __asm__ volatile("      bl      xwosplcb_skd_resume_lic");
        __asm__ volatile("      mov     r1, r0");
        __asm__ volatile("      pop     {r0, lr}");
        __asm__ volatile("      str     r1, [r0, #0]"); /* save the return value */
        __asm__ volatile("      bx      lr");
        __asm__ volatile("svc_11:"); /* case 11: thread migrate */
#if defined(XuanWuOS_CFG_CORE__mp)
        __asm__ volatile("      push    {r0, lr}");
        __asm__ volatile("      ldr     r1, [r0, #4]"); /* get old r1 value */
        __asm__ volatile("      ldr     r0, [r0, #0]"); /* get old r0 value */
        __asm__ volatile("      bl      xwosplcb_thd_outmigrate_lic");
        __asm__ volatile("      mov     r1, r0");
        __asm__ volatile("      pop     {r0, lr}");
        __asm__ volatile("      str     r1, [r0, #0]"); /* save the return value */
#endif
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
