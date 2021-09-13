/**
 * @file
 * @brief 玄武OS移植实现层：中断
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
#include <xwos/ospl/irq.h>
#include <soc.h>

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

__evt __xwos_ivt_qualifier soc_esr_f soc_evt[SOC_ESRi_NUM] = {
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

__edvt __xwos_ivt_qualifier struct soc_irq_data soc_edvt[SOC_ESRi_NUM] = {
        [SOC_ESRi_InsUnalign] = {NULL,},
        [SOC_ESRi_InsAccFault] = {NULL,},
        [SOC_ESRi_IlleIns] = {NULL,},
        [SOC_ESRi_Break] = {NULL,},
        [SOC_ESRi_LdAddrUnalign] = {NULL,},
        [SOC_ESRi_LdFault] = {NULL,},
        [SOC_ESRi_StAddrUnalign] = {NULL,},
        [SOC_ESRi_StAccessFault] = {NULL,},
        [SOC_ESRi_UmodeEcall] = {NULL,},
        [SOC_ESRi_MmodeEcall] = {NULL,},
        [SOC_ESRi_NMI] = {NULL,},
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

void soc_cpuirq_enable_lc(void)
{
        __RV_CSR_SET(CSR_MSTATUS, MSTATUS_MIE);
}

void soc_cpuirq_disable_lc(void)
{
        __RV_CSR_CLEAR(CSR_MSTATUS, MSTATUS_MIE);
}

void soc_cpuirq_restore_lc(xwreg_t cpuirq)
{
        __RV_CSR_SET(CSR_MSTATUS, cpuirq);
}

void soc_cpuirq_save_lc(xwreg_t * cpuirq)
{
        *cpuirq = __RV_CSR_READ_CLEAR(CSR_MSTATUS, MSTATUS_MIE) &
        		  MSTATUS_MIE;
}

__xwbsp_code
xwer_t soc_irqc_init(void)
{
        ECLIC_SetMth(0);
        ECLIC_SetCfgNlbits(__ECLIC_INTCTLBITS);
        __RV_CSR_SET(CSR_MTVT2, MTVT2_MTVT2EN);
        __set_nonvec_entry((rv_csr_t)soc_isr_entry);
        __RV_CSR_WRITE(CSR_MTVT, xwospl_ivt.irq);
        return XWOK;
}

__xwbsp_code
xwer_t xwospl_irq_get_id(xwirq_t * irqnbuf)
{
        CSR_MCAUSE_Type mcause;
        xwirq_t code;

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
        return XWOK;
}

__xwbsp_code
xwer_t xwospl_irq_request(xwirq_t irqn, xwisr_f isrfunc, void * data,
                          const struct soc_irq_cfg * cfg)
{
        XWOS_UNUSED(isrfunc);
        XWOS_UNUSED(data);

#if !defined(SOCCFG_RO_IVT) || (1 != SOCCFG_RO_IVT)
        __xwos_ivt_qualifier struct soc_ivt * ivt;
        __xwos_ivt_qualifier struct soc_idvt * idvt;

        idvt = &xwospl_idvt;
        ivt = &xwospl_ivt;
        if (irqn >= 0) {
                ivt->irq[irqn] = isrfunc;
                if ((NULL != idvt) && (NULL != data)) {
                        idvt->irq[irqn] = data;
                }
        } else {
                xwirq_t esri = SOCCFG_EXC_NUM + irqn;
                ivt->exc[esri] = isrfunc;
                if ((NULL != idvt) && (NULL != data)) {
                        idvt->exc[esri] = data;
                }
        }
#endif
        if (cfg) {
                xwospl_irq_cfg(irqn, cfg);
        }
        return XWOK;
}

__xwbsp_code
xwer_t xwospl_irq_release(xwirq_t irqn)
{
        XWOS_UNUSED(irqn);

#if !defined(SOCCFG_RO_IVT) || (1 != SOCCFG_RO_IVT)
        __xwos_ivt_qualifier struct soc_ivt * ivt;
        __xwos_ivt_qualifier struct soc_idvt * idvt;

        idvt = &xwospl_idvt;
        ivt = &xwospl_ivt;
        if (irqn >= 0) {
                ivt->irq[irqn] = soc_isr_noop;
                if (NULL != idvt) {
                        idvt->irq[irqn] = NULL;
                }
        } else {
                xwirq_t esri = SOC_EXC_NUM + irqn;
                ivt->exc[esri] = NULL;
                if (NULL != idvt) {
                        idvt->exc[esri] = NULL;
                }
        }
#endif
        return XWOK;
}

__xwbsp_code
xwer_t xwospl_irq_enable(xwirq_t irqn)
{
        if (irqn >= 0) {
                ECLIC_EnableIRQ(irqn);
        }
        return XWOK;
}

__xwbsp_code
xwer_t xwospl_irq_disable(xwirq_t irqn)
{
        if (irqn >= 0) {
                ECLIC_DisableIRQ(irqn);
        }
        return XWOK;
}

__xwbsp_code
xwer_t xwospl_irq_save(xwirq_t irqn, xwreg_t * flag)
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
xwer_t xwospl_irq_restore(xwirq_t irqn, xwreg_t flag)
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
xwer_t xwospl_irq_pend(xwirq_t irqn)
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
xwer_t xwospl_irq_clear(xwirq_t irqn)
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
xwer_t xwospl_irq_tst(xwirq_t irqn, bool * pending)
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
xwer_t xwospl_irq_cfg(xwirq_t irqn, const struct soc_irq_cfg * cfg)
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
xwer_t xwospl_irq_get_cfg(xwirq_t irqn, struct soc_irq_cfg * cfgbuf)
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

__xwbsp_code
xwer_t xwospl_irq_get_data(xwirq_t irqn, struct soc_irq_data * databuf)
{
        xwer_t rc;
        __xwos_ivt_qualifier struct soc_idvt * idvt;

        idvt = &xwospl_idvt;
        if (irqn >= 0) {
                databuf->data = idvt->irq[irqn].data;
                rc = XWOK;
        } else {
                xwirq_t esri = SOCCFG_EXC_NUM + irqn;
                databuf->data = idvt->exc[esri].data;
                rc = XWOK;
        }
        return rc;
}
