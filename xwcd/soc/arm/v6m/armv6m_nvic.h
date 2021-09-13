/**
 * @file
 * @brief 架构描述层：ARMv6 Cortex-M NVIC
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

#ifndef __arch_armv6m_nvic_h__
#define __arch_armv6m_nvic_h__

#include <xwos/standard.h>
#include <armv6m_isa.h>

#ifndef SOCCFG_NVIC_PRIO_BITNUM
#  error "SOCCFG_NVIC_PRIO_BITNUM is not defined!"
#else
#  define CM_NVIC_PRIO_BITNUM SOCCFG_NVIC_PRIO_BITNUM
#endif

#define CM_NVIC_PRIO_MAXBITNUM  8
#define CM_NVIC_PRIO_SHIFT      (CM_NVIC_PRIO_MAXBITNUM - CM_NVIC_PRIO_BITNUM)
#define SOC_IRQ_PRIO_MSK        (((1 << CM_NVIC_PRIO_BITNUM) - 1) << CM_NVIC_PRIO_SHIFT)

#define SOC_IRQ_PRIO_HIGHEST    0x0
#define SOC_IRQ_PRIO_LOWEST     ((1 << CM_NVIC_PRIO_BITNUM) - 1)

enum soc_exc_em {
        SOC_SP_TOP = -16, /**< initial value of stack point */
        SOC_EXC_RESET = -15, /**< Reset Interrupt */
        SOC_EXC_NMI = -14, /**< 2 Non Maskable Interrupt */
        SOC_EXC_HARDFAULT = -13, /**< Cortex-M Hard Fault */
        SOC_EXC_MMFAULT = -12, /**< Cortex-M Memory Management Fault */
        SOC_EXC_BUSFAULT = -11, /**< Cortex-M Bus Fault */
        SOC_EXC_USGFAULT = -10, /**< Cortex-M Usage Fault */
        SOC_EXC_RSVN9 = -9,
        SOC_EXC_RSVN8 = -8,
        SOC_EXC_RSVN7 = -7,
        SOC_EXC_RSVN6 = -6,
        SOC_EXC_SVCALL = -5, /**< Cortex-M SV Call Interrupt */
        SOC_EXC_DBGMON = -4, /**< Cortex-M Debug Monitor Interrupt */
        SOC_EXC_RSVN3 = -3,
        SOC_EXC_PENDSV = -2, /**< Cortex-M Pend SV Interrupt */
        SOC_EXC_SYSTICK = -1, /**< Cortex-M System Tick Interrupt */
};

/******** SCB Interrupt Control State Register ********/
#define SCB_ICSR_NMIPENDSET_POS         (31U)
#define SCB_ICSR_NMIPENDSET_MSK         (1UL << SCB_ICSR_NMIPENDSET_POS)

#define SCB_ICSR_PENDSVSET_POS          (28U)
#define SCB_ICSR_PENDSVSET_MSK          (1UL << SCB_ICSR_PENDSVSET_POS)

#define SCB_ICSR_PENDSVCLR_POS          (27U)
#define SCB_ICSR_PENDSVCLR_MSK          (1UL << SCB_ICSR_PENDSVCLR_POS)

#define SCB_ICSR_PENDSTSET_POS          (26U)
#define SCB_ICSR_PENDSTSET_MSK          (1UL << SCB_ICSR_PENDSTSET_POS)

#define SCB_ICSR_PENDSTCLR_POS          (25U)
#define SCB_ICSR_PENDSTCLR_MSK          (1UL << SCB_ICSR_PENDSTCLR_POS)

#define SCB_ICSR_ISRPREEMPT_POS         (23U)
#define SCB_ICSR_ISRPREEMPT_MSK         (1UL << SCB_ICSR_ISRPREEMPT_POS)

#define SCB_ICSR_ISRPENDING_POS         (22U)
#define SCB_ICSR_ISRPENDING_MSK         (1UL << SCB_ICSR_ISRPENDING_POS)

#define SCB_ICSR_VECTPENDING_POS        (12U)
#define SCB_ICSR_VECTPENDING_MSK        (0x1FFUL << SCB_ICSR_VECTPENDING_POS)

#define SCB_ICSR_RETTOBASE_POS          (11U)
#define SCB_ICSR_RETTOBASE_MSK          (1UL << SCB_ICSR_RETTOBASE_POS)

#define SCB_ICSR_VECTACTIVE_POS         (0U)
#define SCB_ICSR_VECTACTIVE_MSK         (0x1FFUL << SCB_ICSR_VECTACTIVE_POS)

/******** SCB System Handler Control and State Register ********/
#define SCB_SHCSR_USGFAULTENA_POS       (18UL)
#define SCB_SHCSR_USGFAULTENA_MSK       (1UL << SCB_SHCSR_USGFAULTENA_POS)

#define SCB_SHCSR_BUSFAULTENA_POS       (17UL)
#define SCB_SHCSR_BUSFAULTENA_MSK       (1UL << SCB_SHCSR_BUSFAULTENA_POS)

#define SCB_SHCSR_MEMFAULTENA_POS       (16UL)
#define SCB_SHCSR_MEMFAULTENA_MSK       (1UL << SCB_SHCSR_MEMFAULTENA_POS)

#define SCB_SHCSR_SVCALLPENDED_POS      (15UL)
#define SCB_SHCSR_SVCALLPENDED_MSK      (1UL << SCB_SHCSR_SVCALLPENDED_POS)

#define SCB_SHCSR_BUSFAULTPENDED_POS    (14UL)
#define SCB_SHCSR_BUSFAULTPENDED_MSK    (1UL << SCB_SHCSR_BUSFAULTPENDED_POS)

#define SCB_SHCSR_MEMFAULTPENDED_POS    (13UL)
#define SCB_SHCSR_MEMFAULTPENDED_MSK    (1UL << SCB_SHCSR_MEMFAULTPENDED_POS)

#define SCB_SHCSR_USGFAULTPENDED_POS    (12UL)
#define SCB_SHCSR_USGFAULTPENDED_MSK    (1UL << SCB_SHCSR_USGFAULTPENDED_POS)

#define SCB_SHCSR_SYSTICKACT_POS        (11UL)
#define SCB_SHCSR_SYSTICKACT_MSK        (1UL << SCB_SHCSR_SYSTICKACT_POS)

#define SCB_SHCSR_PENDSVACT_POS         (10UL)
#define SCB_SHCSR_PENDSVACT_MSK         (1UL << SCB_SHCSR_PENDSVACT_POS)

#define SCB_SHCSR_MONITORACT_POS        (8UL)
#define SCB_SHCSR_MONITORACT_MSK        (1UL << SCB_SHCSR_MONITORACT_POS)

#define SCB_SHCSR_SVCALLACT_POS         (7UL)
#define SCB_SHCSR_SVCALLACT_MSK         (1UL << SCB_SHCSR_SVCALLACT_POS)

#define SCB_SHCSR_USGFAULTACT_POS       (3UL)
#define SCB_SHCSR_USGFAULTACT_MSK       (1UL << SCB_SHCSR_USGFAULTACT_POS)

#define SCB_SHCSR_BUSFAULTACT_POS       (1UL)
#define SCB_SHCSR_BUSFAULTACT_MSK       (1UL << SCB_SHCSR_BUSFAULTACT_POS)

#define SCB_SHCSR_MEMFAULTACT_POS       (0UL)
#define SCB_SHCSR_MEMFAULTACT_MSK       (1UL << SCB_SHCSR_MEMFAULTACT_POS)

/**
 * @brief irq configurations of NVIC
 */
struct cortexm_nvic_irq_cfg {
        xwpr_t priority; /**< priority of irq */
};

/**
 * @brief Enable all interrupts
 */
#define cm_nvic_enable_interrupts()     \
        __asm__ volatile(               \
        "       cpsie i"                \
        ::: "memory", "cc")

/**
 * @brief Disable all interrupts
 */
#define cm_nvic_disable_interrupts()    \
        __asm__ volatile(               \
        "       cpsid i"                \
        ::: "memory", "cc")

/**
 * @brief Enable all faults
 */
#define cm_nvic_enable_faults()         \
        __asm__ volatile(               \
        "       cpsie f"                \
        ::: "memory", "cc")

/**
 * @brief Disable all faults
 */
#define cm_nvic_disable_faults()        \
        __asm__ volatile(               \
        "       cpsid f"                \
        ::: "memory", "cc")

/**
 * @brief Set Priority Grouping
 * @param[in] prigroup: priority group is assigned to the field scb->aircr[10:8]
 *                      PRIGROUP field. Only values from 0~7 are used.
 * @note
 * * The function sets the priority grouping field using the required unlock
 *   sequence.
 */
static __xwbsp_inline
void cm_nvic_set_prioritygrouping(xwu32_t prigroup)
{
        xwu32_t val;

        val = cm_scs.scb.aircr.u32;
        val &= ~(SCB_AIRCR_VECTKEY_MSK | SCB_AIRCR_PRIGROUP_MSK);
        val = (val | ((xwu32_t)0x5FA << SCB_AIRCR_VECTKEY_POS) |
               ((prigroup & (xwu32_t)0x07) << SCB_AIRCR_PRIGROUP_POS));
        cm_scs.scb.aircr.u32 = val;
}

/**
 * @brief Get Priority Grouping
 * @return Priority grouping field (cm_scs.scb.aircr [10:8] PRIGROUP field).
 */
static __xwbsp_inline
xwu32_t cm_nvic_get_prioritygrouping(void)
{
        return ((cm_scs.scb.aircr.u32 & SCB_AIRCR_PRIGROUP_MSK) >>
                SCB_AIRCR_PRIGROUP_POS);
}

/**
 * @brief Get current irq number
 */
static __xwbsp_inline
xwu32_t cm_nvic_get_irq_num(void)
{
        return cm_scs.ictr.u32 << 5;
}

/******** ******** ******** external interrupts ******** ******** ********/
/**
 * @brief Enable interrupt
 * @param[in] irq: interrupt number
 */
static __xwbsp_inline
void cm_nvic_enable_irq(xwirq_t irq)
{
        cm_scs.nvic.iser[irq >> 5].u32 = (xwu32_t)(1U << ((xwu32_t)irq & 0x1FU));
}

/**
 * @brief Disable interrupt
 * @param[in] irq: interrupt number
 */
static __xwbsp_inline
void cm_nvic_disable_irq(xwirq_t irq)
{
        cm_scs.nvic.icer[irq >> 5].u32 = (xwu32_t)(1U << ((xwu32_t)irq & 0x1FU));
}

/**
 * @brief save interrupt flag
 * @param[in] irq: interrupt number
 * @param[out] flag: buffer to save flag
 */
static __xwbsp_inline
void cm_nvic_save_irq(xwirq_t irq, xwreg_t * flag)
{
        *flag = (cm_scs.nvic.iser[irq >> 5].u32) &
                 ((xwu32_t)(1U << ((xwu32_t)irq & 0x1FU)));
}

/**
 * @brief restore interrupt flag
 * @param[in] irqn: interrupt number
 * @param[in] flag: flag to restore
 */
static __xwbsp_inline
void cm_nvic_restore_irq(xwirq_t irq, xwreg_t flag)
{
        if (flag) {
                cm_scs.nvic.iser[irq >> 5].u32 = (xwu32_t)(1U <<
                                                           ((xwu32_t)irq & 0x1FU));
        } else {
                cm_scs.nvic.icer[irq >> 5].u32 = (xwu32_t)(1U <<
                                                           ((xwu32_t)irq & 0x1FU));
        }
}

/**
 * @brief Software trigger interrupt
 * @param[in] irq: interrupt number.
 */
static __xwbsp_inline
void cm_nvic_software_trigger_irq(xwirq_t irq)
{
        cm_scs.stir.u32 = ((xwu32_t)irq & 0x1FF);
}

/**
 * @brief Test interrupt pending state
 * @param[in] irq: interrupt number.
 * @retval false: Interrupt status is not pending.
 * @retval true: Interrupt status is pending.
 */
static __xwbsp_inline
bool cm_nvic_tst_irq(xwirq_t irq)
{
        return !!(cm_scs.nvic.ispr[irq >> 5].u32 >> ((xwu32_t)irq & 0x1F));
}

/**
 * @brief Pend interrupt
 * @param[in] irq: interrupt number.
 */
static __xwbsp_inline
void cm_nvic_pend_irq(xwirq_t irq)
{
        cm_scs.nvic.ispr[irq >> 5].u32 = (xwu32_t)(1 << ((xwu32_t)irq & 0x1F));
}

/**
 * @brief Clear interrupt pending state
 * @param[in] irq: interrupt number.
 */
static __xwbsp_inline
void cm_nvic_clear_irq(xwirq_t irq)
{
        cm_scs.nvic.icpr[irq >> 5].u32 = (xwu32_t)(1 << ((xwu32_t)irq & 0x1F));
}

/**
 * @brief Get interrupt active state
 * @param[in] irq: interrupt number
 * @retval false: Interrupt status is not active.
 * @retval true: Interrupt status is active.
 */
static __xwbsp_inline
bool cm_nvic_get_irq_active(xwirq_t irq)
{
        return (cm_scs.nvic.iabr[irq >> 5].u32 >> ((xwu32_t)irq & 0x1F));
}

/**
 * @brief Set interrupt priority
 * @param[in] irq: interrupt number.
 * @param[in] priority: priority to set.
 */
static __xwbsp_inline
void cm_nvic_set_irq_priority(xwirq_t irq, xwpr_t priority)
{
        cm_scs.nvic.ipr[irq].u8 = (xwu8_t)((priority << (8 - CM_NVIC_PRIO_BITNUM)) &
                                           0xFF);
}

/**
 * @brief Get interrupt priority
 * @param[in] irq: interrupt number
 * @return interrupt priority
 */
static __xwbsp_inline
xwpr_t cm_nvic_get_irq_priority(xwirq_t irq)
{
        return (xwpr_t)(cm_scs.nvic.ipr[(irq)].u8 >> (8 - CM_NVIC_PRIO_BITNUM));
}

/******** ******** ******** system irqs ******** ******** ********/
/******** nmi ********/
/**
 * @brief Pend NMI
 */
static __xwbsp_inline
void cm_nvic_pend_nmi(void)
{
        cm_scs.scb.icsr.bit.nmipend_set = 1;
}

/**
 * @brief Get NMI active-state
 */
static __xwbsp_inline
bool cm_nvic_get_nmi_active(void)
{
        return !!(cm_scs.scb.icsr.bit.nmipend_set);
}

/******** pendsv ********/
/**
 * @brief Pend pendsv
 */
static __xwbsp_inline
void cm_nvic_pend_pendsv(void)
{
        cm_scs.scb.icsr.bit.pendsv_set = 1;
}

/**
 * @brief Clear pendsv
 */
static __xwbsp_inline
void cm_nvic_clear_pendsv(void)
{
        cm_scs.scb.icsr.bit.pendsv_clr = 1;
}

/**
 * @brief Test pendsv pending-state
 */
static __xwbsp_inline
bool cm_nvic_tst_pendsv(void)
{
        return !!(cm_scs.scb.icsr.bit.pendsv_set);
}

/**
 * @brief Get pendsv active-state
 */
static __xwbsp_inline
bool cm_nvic_get_pendsv_active(void)
{
        return !!(cm_scs.scb.shcsr.bit.pendsv_act);
}

/******** systick ********/
/**
 * @brief Pend systick
 */
static __xwbsp_inline
void cm_nvic_pend_systick(void)
{
        cm_scs.scb.icsr.bit.pendst_set = 1;
}

/**
 * @brief Clear systick
 */
static __xwbsp_inline
void cm_nvic_clear_systick(void)
{
        cm_scs.scb.icsr.bit.pendst_clr = 1;
}

/**
 * @brief Test systick pending-state
 */
static __xwbsp_inline
bool cm_nvic_tst_systick(void)
{
        return !!(cm_scs.scb.icsr.bit.pendst_set);
}

/**
 * @brief Get systick active-state
 */
static __xwbsp_inline
bool cm_nvic_get_systick_active(void)
{
        return !!(cm_scs.scb.shcsr.bit.systick_act);
}

/******** usage fault ********/
/**
 * @brief Enable usage fault
 */
static __xwbsp_inline
void cm_nvic_enable_usgfault(void)
{
        cm_scs.scb.shcsr.bit.usgfault_en = 1;
}

/**
 * @brief Disable usage fault
 */
static __xwbsp_inline
void cm_nvic_disable_usgfault(void)
{
        cm_scs.scb.shcsr.bit.usgfault_en = 0;
}

/**
 * @brief Save usage fault flag and disable it
 */
static __xwbsp_inline
void cm_nvic_save_usgfault(xwreg_t * flag)
{
        *flag = cm_scs.scb.shcsr.u32 & SCB_SHCSR_USGFAULTENA_MSK;
}

/**
 * @brief restore usage fault flag
 */
static __xwbsp_inline
void cm_nvic_restore_usgfault(xwreg_t flag)
{
        cm_scs.scb.shcsr.u32 |= (flag & SCB_SHCSR_USGFAULTENA_MSK);
}

/**
 * @brief Test usagefault pending-state
 */
static __xwbsp_inline
bool cm_nvic_tst_usgfault(void)
{
        return !!(cm_scs.scb.shcsr.bit.usgfault_pended);
}

/**
 * @brief Get usagefault active-state
 */
static __xwbsp_inline
bool cm_nvic_get_usgfault_active(void)
{
        return !!(cm_scs.scb.shcsr.bit.usgfault_act);
}

/******** bus fault ********/
/**
 * @brief Enable bus fault
 */
static __xwbsp_inline
void cm_nvic_enable_busfault(void)
{
        cm_scs.scb.shcsr.bit.busfault_en = 1;
}

/**
 * @brief Disable bus fault
 */
static __xwbsp_inline
void cm_nvic_disable_busfault(void)
{
        cm_scs.scb.shcsr.bit.busfault_en = 0;
}

/**
 * @brief Save bus fault flag and disable it
 */
static __xwbsp_inline
void cm_nvic_save_busfault(xwreg_t * flag)
{
        *flag = cm_scs.scb.shcsr.u32 & SCB_SHCSR_USGFAULTENA_MSK;
}

/**
 * @brief restore bus fault flag
 */
static __xwbsp_inline
void cm_nvic_restore_busfault(xwreg_t flag)
{
        cm_scs.scb.shcsr.u32 |= (flag & SCB_SHCSR_USGFAULTENA_MSK);
}

/**
 * @brief Test busfault pending-state
 */
static __xwbsp_inline
bool cm_nvic_tst_busfault(void)
{
        return !!(cm_scs.scb.shcsr.bit.busfault_pended);
}

/**
 * @brief Get busfault active-state
 */
static __xwbsp_inline
bool cm_nvic_get_busfault_active(void)
{
        return !!(cm_scs.scb.shcsr.bit.busfault_act);
}

/******** mem fault ********/
/**
 * @brief Enable memfault
 */
static __xwbsp_inline
void cm_nvic_enable_memfault(void)
{
        cm_scs.scb.shcsr.bit.memfault_en = 1;
}

/**
 * @brief Disable memfault
 */
static __xwbsp_inline
void cm_nvic_disable_memfault(void)
{
        cm_scs.scb.shcsr.bit.memfault_en = 0;
}

/**
 * @brief Save memfault flag and disable it
 */
static __xwbsp_inline
void cm_nvic_save_memfault(xwreg_t * flag)
{
        *flag = cm_scs.scb.shcsr.u32 & SCB_SHCSR_MEMFAULTENA_MSK;
}

/**
 * @brief restore memfault flag
 */
static __xwbsp_inline
void cm_nvic_restore_memfault(xwreg_t flag)
{
        cm_scs.scb.shcsr.u32 |= (flag & SCB_SHCSR_MEMFAULTENA_MSK);
}

/**
 * @brief Test memfault pending-state
 */
static __xwbsp_inline
bool cm_nvic_tst_memfault(void)
{
        return !!(cm_scs.scb.shcsr.bit.memfault_pended);
}

/**
 * @brief Get memfault active-state
 */
static __xwbsp_inline
bool cm_nvic_get_memfault_active(void)
{
        return !!(cm_scs.scb.shcsr.bit.memfault_act);
}

/******** svc ********/
/**
 * @brief Test svccall pending-state
 */
static __xwbsp_inline
bool cm_nvic_tst_svccall(void)
{
        return !!(cm_scs.scb.shcsr.bit.svcall_pended);
}

/**
 * @brief Get svccall active-state
 */
static __xwbsp_inline
bool cm_nvic_get_svccall_active(void)
{
        return !!(cm_scs.scb.shcsr.bit.svcall_act);
}

/******** debug monitor ********/
/**
 * @brief Get monitor active-state
 */
static __xwbsp_inline
bool cm_nvic_get_monitor_active(void)
{
        return !!(cm_scs.scb.shcsr.bit.monitor_act);
}

/**
 * @brief Set system irq priority
 * @param[in] irq: irq number
 * @param[in] priority: priority to set.
 */
static __xwbsp_inline
void cm_nvic_set_sysirq_priority(xwirq_t irq, xwpr_t priority)
{
        xwu32_t idx;

        idx = (((xwu32_t)irq) & 0xF) - 4;
        cm_scs.scb.shpr[idx].u8 = (xwu8_t)((priority << (8 - CM_NVIC_PRIO_BITNUM)) &
                                           0xFF);
}

/**
 * @brief Get system irq priority
 * @param[in] irq: irq number
 * @return interrupt priority
 */
static __xwbsp_inline
xwpr_t cm_nvic_get_sysirq_priority(xwirq_t irq)
{
        xwu32_t idx;

        idx = (((xwu32_t)irq) & 0xF) - 4;
        return (xwpr_t)(cm_scs.scb.shpr[idx].u8 >> (8 - CM_NVIC_PRIO_BITNUM));
}

#endif /* armv6m_nvic.h */
