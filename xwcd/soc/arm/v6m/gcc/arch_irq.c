/**
 * @file
 * @brief architecture faults
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
#endif

#include <armv6m_core.h>
#include <armv6m_nvic.h>
#include <arch_irq.h>
#include <arch_sched.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      macros       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define ARCH_IRQ_FAULT_PRIO     (ARCH_IRQ_PRIO_HIGHEST)
#define ARCH_IRQ_SVC_PRIO       (ARCH_IRQ_PRIO_HIGHEST)

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief Setup Architecture Fault
 */
__xwbsp_init_code
void arch_init_sysirqs(void)
{
        /* init faults */
        cm_nvic_set_sysirq_priority(ARCH_IRQ_MMFAULT, ARCH_IRQ_FAULT_PRIO);
        cm_nvic_enable_memfault();

        cm_nvic_set_sysirq_priority(ARCH_IRQ_BUSFAULT, ARCH_IRQ_FAULT_PRIO);
        cm_nvic_enable_busfault();

        cm_nvic_set_sysirq_priority(ARCH_IRQ_USGFAULT, ARCH_IRQ_FAULT_PRIO);
        cm_nvic_enable_usgfault();

        cm_nvic_enable_faults();

        cm_nvic_set_sysirq_priority(ARCH_IRQ_SVCALL, ARCH_IRQ_SVC_PRIO);
}

/**
 * @brief Reset ISR
 */
__xwbsp_isr __naked
void arch_isr_reset(void)
{
        __asm__ volatile("      cpsid   i");
        cm_scs.scb.ccr.bit.stkalign = 1;
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
        __maybe_unused volatile struct cm_scs_reg * scs;

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
        __maybe_unused volatile struct cm_scs_reg * scs;

        scs = &cm_scs;
        arch_scheduler_lib_chkcstk();
        while (true) {
        }
}

/**
 * @brief MM ISR
 */
__xwbsp_isr
void arch_isr_mm(void)
{
        __maybe_unused volatile struct cm_scs_reg * scs;

        scs = &cm_scs;
        arch_scheduler_lib_chkcstk();
        while (true) {
        }
}

/**
 * @brief busfault ISR
 */
__xwbsp_isr
void arch_isr_busfault(void)
{
        __maybe_unused volatile struct cm_scs_reg * scs;

        scs = &cm_scs;
        arch_scheduler_lib_chkcstk();
        while (true) {
        }
}

/**
 * @brief usagefault ISR
 */
__xwbsp_isr
void arch_isr_usagefault(void)
{
        __maybe_unused volatile struct cm_scs_reg * scs;

        scs = &cm_scs;
        arch_scheduler_lib_chkcstk();
        while (true) {
        }
}

/**
 * @brief debugmon ISR
 */
__xwbsp_isr
void arch_isr_dbgmon(void)
{
        __maybe_unused volatile struct cm_scs_reg * scs;

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
 *   + svc_sysisr()运行在handler模式。ARM内核进入handler模式时会自动将r0 ~ r3, ip,
 *     lr, pc & xpsr压栈保存；返回thread模式时会自动从栈中弹出原始值恢复这些寄存器。
 *     返回的地址被存放在[SP+24]的地址处。可以将这个内存地址中的内容改为xwsc的真正
 *     入口@ref arch_xwsc_entry()，并将原始返回地址存放在原始LR的位置[SP+20]。原始
 *     LR的值又存放在原始R3的位置[SP+12]。原始R3在xwsc中没有使用到，因此不需要备份。
 *     当svc_sysisr()返回时，就会进入到@ref arch_xwsc_entry()函数，并且以r0 ~ r2作为
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
__xwbsp_isr __naked
void arch_isr_svc(void)
{
        __asm__ volatile("      mov     r0, #0x4");
        __asm__ volatile("      mov     r1, lr");
        __asm__ volatile("      tst     r1, r0"); /* update Z flag in EPSR */
        __asm__ volatile("      bne     1f");
        __asm__ volatile("      mrs     r0, msp"); /* if Z == 1 (bit is 0), use MSP */
        __asm__ volatile("      b       2f");
        __asm__ volatile("1:");
        __asm__ volatile("      mrs     r0, psp"); /* if Z == 0 (bit is 1), use PSP */
        __asm__ volatile("2:");
        __asm__ volatile("      ldr     r1, [r0, #24]"); /* get old pc value */
        __asm__ volatile("      sub     r1, #2");
        __asm__ volatile("      ldrb    r2, [r1]"); /* get svc number */
        __asm__ volatile("      lsl     r2, r2, #2"); /* get offset */
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
                         ".4byte        svc_11 + 1\n");
        __asm__ volatile("svc_0:"); /* case 0: */
        __asm__ volatile("      ldr     r0, [r0, #0]"); /* get old r0 */
        __asm__ volatile("      b       arch_scheduler_svcsr_start");
        __asm__ volatile("svc_1:"); /* case 1: privilege_start */
        __asm__ volatile("      mrs     r0, control");
        __asm__ volatile("      mov     r1, #1");
        __asm__ volatile("      orr     r0, r1");
        __asm__ volatile("      msr     control, r0");
        __asm__ volatile("      bx      lr");
        __asm__ volatile("svc_2:"); /* case 2: privilege_end */
        __asm__ volatile("      mrs     r0, control");
        __asm__ volatile("      mov     r1, #1");
        __asm__ volatile("      bic     r0, r1");
        __asm__ volatile("      msr     control, r0");
        __asm__ volatile("      bx      lr");
        __asm__ volatile("svc_3:"); /* case 3: */
        __asm__ volatile("      bx      lr");
        __asm__ volatile("svc_4:"); /* case 4: */
        __asm__ volatile("      bx      lr");
        __asm__ volatile("svc_5:"); /* case 5: freeze thread */
        __asm__ volatile("      push    {r0, lr}");
        __asm__ volatile("      ldr     r0, [r0, #0]"); /* get old r0 value: ctcb */
        __asm__ volatile("      bl      xwos_thrd_freeze_lic");
        __asm__ volatile("      mov     r2, r0");
        __asm__ volatile("      pop     {r0, r1}");
        __asm__ volatile("      str     r2, [r0, #0]"); /* save the return value */
        __asm__ volatile("      mov     lr, r1");
        __asm__ volatile("      bx      lr");
        __asm__ volatile("svc_6:"); /* case 6: */
        __asm__ volatile("      bx      lr");
        __asm__ volatile("svc_7:"); /* case 7: thread exits */
        __asm__ volatile("      push    {r0, lr}");
        __asm__ volatile("      ldr     r1, [r0, #4]"); /* get old r1 value */
        __asm__ volatile("      ldr     r0, [r0, #0]"); /* get old r0 value */
        __asm__ volatile("      bl      xwos_thrd_exit_lic");
        __asm__ volatile("      mov     r2, r0");
        __asm__ volatile("      pop     {r0, r1}");
        __asm__ volatile("      str     r2, [r0, #0]"); /* save the return value */
        __asm__ volatile("      mov     lr, r1");
        __asm__ volatile("      bx      lr");
        __asm__ volatile("svc_8:"); /* case 8: xwos_xwsc */
        __asm__ volatile("      bx      lr");
        __asm__ volatile("svc_9:"); /* case 9: */
        __asm__ volatile("      push    {r0, lr}");
        __asm__ volatile("      ldr     r0, [r0, #0]"); /* get old r0 value */
        __asm__ volatile("      bl      xwos_scheduler_suspend_lic");
        __asm__ volatile("      mov     r2, r0");
        __asm__ volatile("      pop     {r0, r1}");
        __asm__ volatile("      str     r2, [r0, #0]"); /* save the return value */
        __asm__ volatile("      mov     lr, r1");
        __asm__ volatile("      bx      lr");
        __asm__ volatile("svc_10:"); /* case 10: */
        __asm__ volatile("      push    {r0, lr}");
        __asm__ volatile("      ldr     r0, [r0, #0]"); /* get old r0 value */
        __asm__ volatile("      bl      xwos_scheduler_resume_lic");
        __asm__ volatile("      mov     r2, r0");
        __asm__ volatile("      pop     {r0, r1}");
        __asm__ volatile("      str     r2, [r0, #0]"); /* save the return value */
        __asm__ volatile("      mov     lr, r1");
        __asm__ volatile("      bx      lr");
        __asm__ volatile("svc_11:"); /* case 11 */
        __asm__ volatile("      bx      lr");
}

/**
 * @brief ARCH default ISR
 */
__xwbsp_isr
void arch_isr_noop(void)
{
        __maybe_unused volatile struct cm_scs_reg * scs;
        __maybe_unused xwer_t rc;
        __maybe_unused xwirq_t irqn;

        rc = arch_irq_get_id(&irqn);
        scs = &cm_scs;
        while (true) {
        }
}

__xwbsp_code
xwer_t arch_irq_get_id(xwirq_t * irqnbuf)
{
        xwirq_t curr;
        xwer_t rc;

        curr = cm_scs.scb.icsr.bit.vect_active;
        if (0 == curr) {
                rc = -ENOTINISR;
        } else {
                curr -= 16;
                rc = XWOK;
        }
        if (!is_err_or_null(irqnbuf)) {
                *irqnbuf = curr;
        }
        return rc;
}
