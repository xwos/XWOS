/**
 * @file
 * @brief XWOS移植实现层：中断
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

#include <xwcd/soc/riscv/nuclei/bumblebee/gd32v/soc_irq.h>
#include <xwcd/soc/riscv/nuclei/bumblebee/gd32v/soc.h>

extern void soc_isr_entry(void);

__xwbsp_isr
void soc_esr_noop(xwreg_t sp)
{
        XWOS_UNUSED(sp);
}

__xwbsp_isr
void soc_isr_noop(void)
{
}

__xwbsp_isr
void soc_esr_nmi(void)
{
}

__xwbsp_isr
void soc_esr_ebreak(void)
{
        while (true) {
        }
}

__evt soc_esr_f soc_evt[SOC_ESRi_NUM] = {
        [SOC_ESRi_InsUnalign] = soc_esr_noop,
        [SOC_ESRi_InsAccFault] = soc_esr_noop,
        [SOC_ESRi_IlleIns] = soc_esr_noop,
        [SOC_ESRi_Break] = NULL, /* Not from here */
        [SOC_ESRi_LdAddrUnalign] = soc_esr_noop,
        [SOC_ESRi_LdFault] = soc_esr_noop,
        [SOC_ESRi_StAddrUnalign] = soc_esr_noop,
        [SOC_ESRi_StAccessFault] = soc_esr_noop,
        [SOC_ESRi_UmodeEcall] = NULL, /* Not from here */
        [SOC_ESRi_MmodeEcall] = NULL, /* Not from here */
        [SOC_ESRi_NMI] = NULL, /* Not from here */
};

__xwbsp_isr
void soc_esr_c_entry(CSR_MCAUSE_Type mcause, xwreg_t sp)
{
        xwirq_t exc = mcause.b.exccode;
        soc_esr_f esr = NULL;

        if ((exc <= SOC_EXCode_StAccessFault) && (SOC_EXCode_Break != exc)) {
                esr = soc_evt[exc];
        }
        if (esr) {
                esr(sp);
        }
}

__xwbsp_code
void soc_cpuirq_enable_lc(void)
{
        __RV_CSR_SET(CSR_MSTATUS, MSTATUS_MIE);
}

__xwbsp_code
void soc_cpuirq_disable_lc(void)
{
        __RV_CSR_CLEAR(CSR_MSTATUS, MSTATUS_MIE);
}

__xwbsp_code
void soc_cpuirq_restore_lc(xwreg_t cpuirq)
{
        __RV_CSR_SET(CSR_MSTATUS, cpuirq);
}

__xwbsp_code
void soc_cpuirq_save_lc(xwreg_t * cpuirq)
{
        *cpuirq = __RV_CSR_READ_CLEAR(CSR_MSTATUS, MSTATUS_MIE) & MSTATUS_MIE;
}

__xwbsp_code
bool soc_cpuirq_test_lc(void)
{
        xwreg_t cpuirq = __RV_CSR_READ(CSR_MSTATUS);
        return !!(cpuirq & MSTATUS_MIE);
}

__xwbsp_code
xwer_t soc_irqc_init(void)
{
        ECLIC_SetMth(0);
        ECLIC_SetCfgNlbits(__ECLIC_INTCTLBITS);
        __RV_CSR_SET(CSR_MTVT2, MTVT2_MTVT2EN);
        __set_nonvec_entry((rv_csr_t)soc_isr_entry);
        __RV_CSR_WRITE(CSR_MTVT, soc_ivt.irq);
        return XWOK;
}

__xwbsp_code
xwer_t soc_irq_get_id(xwirq_t * irqnbuf)
{
        CSR_MSUBM_Type msubm;
        CSR_MCAUSE_Type mcause;
        xwirq_t code;
        xwer_t rc;

        msubm.d = __RV_CSR_READ(CSR_MSUBM);
        if (msubm.b.typ) {
                mcause.d = __RV_CSR_READ(CSR_MCAUSE);
                code = mcause.b.exccode;
                if (0 == mcause.b.interrupt) {
                        if (SOC_EXCode_NMI == code) {
                                code = -1;
                        } else {
                                code -= SOCCFG_EXC_NUM;
                        }
                }
                *irqnbuf = code;
                rc = XWOK;
        } else {
                rc = -ENOTISRCTX;
        }
        return rc;
}

extern xwu8_t eclic_ivt_lma_base[];
extern xwu8_t eclic_ivt_vma_base[];
extern xwu8_t evt_lma_base[];
extern xwu8_t evt_vma_base[];

__xwbsp_code
xwer_t soc_irq_request(xwirq_t irqn, xwisr_f isrfunc)
{
        const struct soc_ivt * ivt;

        ivt = &soc_ivt;
        if ((irqn >= 0) && (&eclic_ivt_lma_base[0] != &eclic_ivt_vma_base[0])) {
                ivt->irq[irqn] = isrfunc;
        } else {
        }
        return XWOK;
}

__xwbsp_code
xwer_t soc_irq_release(xwirq_t irqn)
{
        const struct soc_ivt * ivt;

        ivt = &soc_ivt;
        if ((irqn >= 0) && (&eclic_ivt_lma_base[0] != &eclic_ivt_vma_base[0])) {
                ivt->irq[irqn] = soc_isr_noop;
        } else {
        }
        return XWOK;
}

__xwbsp_code
xwer_t soc_irq_enable(xwirq_t irqn)
{
        if (irqn >= 0) {
                ECLIC_EnableIRQ(irqn);
        }
        return XWOK;
}

__xwbsp_code
xwer_t soc_irq_disable(xwirq_t irqn)
{
        if (irqn >= 0) {
                ECLIC_DisableIRQ(irqn);
        }
        return XWOK;
}

__xwbsp_code
xwer_t soc_irq_save(xwirq_t irqn, xwreg_t * flag)
{
        xwer_t rc;

        if (irqn >= 0) {
                *flag = ECLIC_GetEnableIRQ(irqn);
                rc = XWOK;
        } else {
                rc = -EPERM;
        }
        return rc;
}

__xwbsp_code
xwer_t soc_irq_restore(xwirq_t irqn, xwreg_t flag)
{
        xwer_t rc;

        if (irqn >= 0) {
                if (flag) {
                        ECLIC_EnableIRQ(irqn);
                } else {
                        ECLIC_DisableIRQ(irqn);
                }
                rc = XWOK;
        } else {
                rc = -EPERM;
        }
        return rc;
}

__xwbsp_code
xwer_t soc_irq_pend(xwirq_t irqn)
{
        xwer_t rc;

        if (irqn >= 0) {
                ECLIC_SetPendingIRQ(irqn);
                rc = XWOK;
        } else {
                rc = -EPERM;
        }
        return rc;
}

__xwbsp_code
xwer_t soc_irq_clear(xwirq_t irqn)
{
        xwer_t rc;

        if (irqn >= 0) {
                ECLIC_ClearPendingIRQ(irqn);
                rc = XWOK;
        } else {
                rc = -EPERM;
        }
        return rc;
}

__xwbsp_code
xwer_t soc_irq_tst(xwirq_t irqn, bool * pending)
{
        xwer_t rc;

        if (irqn >= 0) {
                *pending = !!ECLIC_GetPendingIRQ(irqn);
                rc = XWOK;
        } else {
                rc = -EPERM;
        }
        return rc;
}

__xwbsp_code
xwer_t soc_irq_cfg(xwirq_t irqn, const struct soc_irq_cfg * cfg)
{
        xwer_t rc;

        if (irqn >= 0) {
                ECLIC_SetTrigIRQ(irqn, cfg->trig);
                ECLIC_SetShvIRQ(irqn, cfg->shv);
                ECLIC_SetLevelIRQ(irqn, cfg->level);
                ECLIC_SetPriorityIRQ(irqn, cfg->priority);
                rc = XWOK;
        } else {
                rc = -EPERM;
        }
        return rc;
}

__xwbsp_code
xwer_t soc_irq_get_cfg(xwirq_t irqn, struct soc_irq_cfg * cfgbuf)
{
        xwer_t rc;
        struct soc_irq_cfg cfg;

        if (irqn >= 0) {
                cfg.trig = ECLIC_GetTrigIRQ(irqn);
                cfg.shv = ECLIC_GetShvIRQ(irqn);
                cfg.level = ECLIC_GetLevelIRQ(irqn);
                cfg.priority = ECLIC_GetPriorityIRQ(irqn);
                *cfgbuf = cfg;
                rc = XWOK;
        } else {
                rc = -EPERM;
        }
        return rc;
}
