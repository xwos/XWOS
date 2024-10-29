/**
 * @file
 * @brief XWOS移植实现层：SOC中断
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

#ifndef __xwcd_soc_riscv_nuclei_bumblebee_gd32v_soc_irq_h__
#define __xwcd_soc_riscv_nuclei_bumblebee_gd32v_soc_irq_h__

#include <xwos/standard.h>

/**
 * @brief IRQ Number Enumeration (For NMSIS)
 */
enum soc_irq_em {
        SOC_IRQn_Reserved0 = 0, /**< Internal reserved */
        SOC_IRQn_Reserved1 = 1, /**< Internal reserved */
        SOC_IRQn_Reserved2 = 2, /**< Internal reserved */
        SOC_IRQn_SysTimerSW = 3, /**< System Timer SW interrupt */
        SOC_IRQn_Reserved3 = 4, /**< Internal reserved */
        SOC_IRQn_Reserved4 = 5, /**< Internal reserved */
        SOC_IRQn_Reserved5 = 6, /**< Internal reserved */
        SOC_IRQn_SysTimer = 7, /**< System Timer Interrupt */
        SOC_IRQn_Reserved6 = 8, /**< Internal reserved */
        SOC_IRQn_Reserved7 = 9, /**< Internal reserved */
        SOC_IRQn_Reserved8 = 10, /**< Internal reserved */
        SOC_IRQn_Reserved9 = 11, /**< Internal reserved */
        SOC_IRQn_Reserved10 = 12, /**< Internal reserved */
        SOC_IRQn_Reserved11 = 13, /**< Internal reserved */
        SOC_IRQn_Reserved12 = 14, /**< Internal reserved */
        SOC_IRQn_Reserved13 = 15, /**< Internal reserved */
        SOC_IRQn_Reserved14 = 16, /**< Internal reserved */
        SOC_IRQn_BusError = 17, /**< Bus Error interrupt */
        SOC_IRQn_PerfMon = 18, /**< Performance Monitor */
};

/**
 * @brief Exception Code Enumeration (For XWOS)
 */
enum soc_exception_code_em {
        SOC_EXCode_InsUnalign = 0, /**< Instruction address misaligned */
        SOC_EXCode_InsAccFault = 1, /**< Instruction access fault */
        SOC_EXCode_IlleIns = 2, /**< Illegal instruction */
        SOC_EXCode_Break = 3, /**< Beakpoint */
        SOC_EXCode_LdAddrUnalign = 4, /**< Load address misaligned */
        SOC_EXCode_LdFault = 5, /**< Load access fault */
        SOC_EXCode_StAddrUnalign = 6, /**< Store or AMO address misaligned */
        SOC_EXCode_StAccessFault = 7, /**< Store or AMO access fault */
        SOC_EXCode_UmodeEcall = 8, /**< Environment call from User mode */
        SOC_EXCode_MmodeEcall = 11, /**< Environment call from Machine mode */
        SOC_EXCode_NMI = 0xFFF, /**< NMI interrupt */
};

/**
 * @brief Exception Service Routine Index Enumeration (For XWOS)
 */
enum soc_exception_service_routine_index_em {
        SOC_ESRi_InsUnalign = 0, /**< Instruction address misaligned */
        SOC_ESRi_InsAccFault = 1, /**< Instruction access fault */
        SOC_ESRi_IlleIns = 2, /**< Illegal instruction */
        SOC_ESRi_Break = 3, /**< Beakpoint */
        SOC_ESRi_LdAddrUnalign = 4, /**< Load address misaligned */
        SOC_ESRi_LdFault = 5, /**< Load access fault */
        SOC_ESRi_StAddrUnalign = 6, /**< Store or AMO address misaligned */
        SOC_ESRi_StAccessFault = 7, /**< Store or AMO access fault */
        SOC_ESRi_UmodeEcall = 8, /**< Environment call from User mode */
        SOC_ESRi_MmodeEcall = 11, /**< Environment call from Machine mode */
        SOC_ESRi_NMI = 12, /**< NMI interrupt */
        SOC_ESRi_NUM = 16, /**< ESR Index number (roundup to 16 bytes) */
};

/**
 * @brief Exception Number Enumeration (For XWOS)
 */
enum soc_exception_em {
        SOC_EXC_InsUnalign = -13, /**< Instruction address misaligned */
        SOC_EXC_InsAccFault = -12, /**< Instruction access fault */
        SOC_EXC_IlleIns = -11, /**< Illegal instruction */
        SOC_EXC_Break = -10, /**< Beakpoint */
        SOC_EXC_LdAddrUnalign = -9, /**< Load address misaligned */
        SOC_EXC_LdFault = -8, /**< Load access fault */
        SOC_EXC_StAddrUnalign = -7, /**< Store or AMO address misaligned */
        SOC_EXC_StAccessFault = -6, /**< Store or AMO access fault */
        SOC_EXC_UmodeEcall = -5, /**< Environment call from User mode */
        SOC_EXC_MmodeEcall = -2, /**< Environment call from Machine mode */
        SOC_EXC_NMI = -1, /**< NMI interrupt */
};

struct soc_irq_cfg {
        xwu8_t level;
        xwu8_t priority;
        xwu8_t trig;
        xwu8_t shv;
};

struct soc_irq_data {
        void * data;
};

typedef void (* soc_esr_f)(xwreg_t /*sp*/);

struct soc_ivt {
        soc_esr_f * exc;
        xwisr_f * irq;
};

extern soc_esr_f soc_evt[SOC_ESRi_NUM];
extern const struct soc_ivt soc_ivt;

void soc_esr_noop(xwreg_t);
void soc_isr_noop(void);
void soc_cpuirq_enable_lc(void);
void soc_cpuirq_disable_lc(void);
void soc_cpuirq_restore_lc(xwreg_t cpuirq);
void soc_cpuirq_save_lc(xwreg_t * cpuirq);
bool soc_cpuirq_test_lc(void);
xwer_t soc_irqc_init(void);
xwer_t soc_irq_get_id(xwirq_t * irqnbuf);
xwer_t soc_irq_request(xwirq_t irqn, xwisr_f isrfunc);
xwer_t soc_irq_release(xwirq_t irqn);
xwer_t soc_irq_enable(xwirq_t irqn);
xwer_t soc_irq_disable(xwirq_t irqn);
xwer_t soc_irq_save(xwirq_t irqn, xwreg_t * flag);
xwer_t soc_irq_restore(xwirq_t irqn, xwreg_t flag);
xwer_t soc_irq_pend(xwirq_t irqn);
xwer_t soc_irq_clear(xwirq_t irqn);
xwer_t soc_irq_tst(xwirq_t irqn, bool * pending);
xwer_t soc_irq_cfg(xwirq_t irqn, const struct soc_irq_cfg * cfg);
xwer_t soc_irq_get_cfg(xwirq_t irqn, struct soc_irq_cfg * cfgbuf);

#endif /* xwcd/soc/riscv/nuclei/bumblebee/gd32v/soc_irq.h */
