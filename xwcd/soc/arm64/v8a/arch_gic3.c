/**
 * @file
 * @brief 架构描述层：ARMv8A Generic Interrupt Controller Version 2
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

#include <xwcd/soc/arm64/v8a/arch_gic3.h>
#include <xwos/lib/xwbop.h>
#include <xwos/ospl/skd.h>

/* #define SOC_DBGF */
#include <soc_debug.h>

struct __xwcc_alignl1cache armv8a_gic3_percpu_runtime_data {
        xwirq_t irqn; /**< CPU正在处理的中断号 */
        struct {
                xwu8_t max;
                xwu8_t min;
                xwu8_t shift;
        } priority; /**< 优先级 */
        xwisr_f pivt[ARMV8A_SGI_NUM + ARMV8A_PPI_NUM]; /**< 私有中断向量表 */
};

struct __xwcc_alignl1cache armv8a_gic3_runtime_data {
        xwu32_t it_lines_num; /**< 中断数量 */
        bool security; /**< 是否支持安全扩展 */
        xwisr_f sivt[SOCCFG_IRQ_NUM]; /**< 共享中断向量表 */
        struct armv8a_gic3_percpu_runtime_data cpu[CPUCFG_CPU_NUM];
};

__xwbsp_data
struct armv8a_gic3_runtime_data armv8a_gic3_runtime_data = {
        .it_lines_num = 512,
        .security = true,
        .cpu[0 ... (CPUCFG_CPU_NUM - 1)] = {
                .irqn = 0x3FF,
                .priority = {
                        .max = 0x0,
                        .min = 0xFF,
                },
        },
};

__xwbsp_code
void armv8a_gic3_init_runtime(void)
{
        xwu32_t i;

        armv8a_gic3_runtime_data.it_lines_num = (armv8a_gic3_d.typer.bit.it_line_num +
                                                 (xwu32_t)1) * (xwu32_t)32;
        armv8a_gic3_runtime_data.security = armv8a_gic3_d.typer.bit.security_extn;
        soc_dbgf("GIC3",
                 "Init: reg:0x%lX, gic3.it_lines_num:%d, gic3.security:%d\r\n",
                 SOC_GIC3_REGBASE,
                 armv8a_gic3_runtime_data.it_lines_num,
                 armv8a_gic3_runtime_data.security);

        for (i = 0; i < SOCCFG_IRQ_NUM; i += 8U) {
                armv8a_gic3_runtime_data.sivt[i + 0U] = NULL;
                armv8a_gic3_runtime_data.sivt[i + 1U] = NULL;
                armv8a_gic3_runtime_data.sivt[i + 2U] = NULL;
                armv8a_gic3_runtime_data.sivt[i + 3U] = NULL;
                armv8a_gic3_runtime_data.sivt[i + 4U] = NULL;
                armv8a_gic3_runtime_data.sivt[i + 5U] = NULL;
                armv8a_gic3_runtime_data.sivt[i + 6U] = NULL;
                armv8a_gic3_runtime_data.sivt[i + 7U] = NULL;
        }
}

__xwbsp_code
void armv8a_gic3_init_distributor(void)
{
        xwu32_t i;
	xwu32_t access = (xwu32_t)0xFFFFFFFF;
        xwu32_t regidx;

        armv8a_gic3_d.ctrl.ns.enable_grp1 = 0U;
        armv8a_gic3_d.ctrl.ns.enable_grp1a = 0U;
        while (armv8a_gic3_d.ctrl.ns.rwp) {
                /* wait for completion */
        }

        for (i = (ARMV8A_SGI_NUM + ARMV8A_PPI_NUM);
             i <  armv8a_gic3_runtime_data.it_lines_num;
             i += 32) {
		regidx = i >> 5U; /* i / 32 */
		access = armv8a_gic3_d.igroupr[regidx] | armv8a_gic3_d.igrpmodr[regidx];
                soc_dbgf("GIC3",
                         "IRQ[%d...%d] igroupr[%d]:0x%X, igrpmodr[%d]:0x%X, "
                         "access:0x%X\r\n",
                         i, i + 31,
                         regidx, armv8a_gic3_d.igroupr[regidx],
                         regidx, armv8a_gic3_d.igrpmodr[regidx],
                         access);

                /* Try to set interrupt to NS Group 1 */
                armv8a_gic3_d.igroupr[regidx] = access;
                armv8a_gic3_d.igrpmodr[regidx] = 0x0U;

                /* Try to clear interrupt */
                armv8a_gic3_d.icenabler[regidx] = (xwu32_t)0xFFFFFFFF;
                armv8a_gic3_d.icpendr[regidx] = (xwu32_t)0xFFFFFFFF;
        }
	/* Set all SPIs to min priority */
	for (i = (ARMV8A_SGI_NUM + ARMV8A_PPI_NUM);
             i < armv8a_gic3_runtime_data.it_lines_num;
             i += 4) {
                armv8a_gic3_d.ipriorityr[i >> 2U] = (xwu32_t)0xFFFFFFFF;
                soc_dbgf("GIC3", "Set ipriorityr[%d]:0x%X\r\n",
                         (i >> 2U), armv8a_gic3_d.ipriorityr[i >> 2U]);
	}

        /* enable G1S & G1NS */
        armv8a_gic3_d.ctrl.ns.enable_grp1 = 1U;
        armv8a_gic3_d.ctrl.ns.enable_grp1a = 1U;
        while (armv8a_gic3_d.ctrl.ns.rwp) {
                /* wait for completion */
        }
        soc_dbgf("GIC3", "armv8a_gic3_d.ctrl:0x%X\r\n", armv8a_gic3_d.ctrl.u32);
}

void armv8a_gic3_enable_distributor(void)
{
        armv8a_gic3_d.ctrl.ns.enable_grp1 = 1U;
        armv8a_gic3_d.ctrl.ns.enable_grp1a = 1U;
        while (armv8a_gic3_d.ctrl.ns.rwp) {
                /* wait for completion */
        }
        soc_dbgf("GIC3", "armv8a_gic3_d.ctrl:0x%X\r\n", armv8a_gic3_d.ctrl.u32);
}

void armv8a_gic3_disable_distributor(void)
{
        armv8a_gic3_d.ctrl.ns.enable_grp1 = 0U;
        armv8a_gic3_d.ctrl.ns.enable_grp1a = 0U;
        while (armv8a_gic3_d.ctrl.ns.rwp) {
                /* wait for completion */
        }
}

__xwbsp_code
void armv8a_gic3_init_cpuif(void)
{
        xwu32_t i;
	xwid_t cpuid = xwospl_skd_get_cpuid_lc();
	xwu32_t access = (xwu32_t)0xFFFFFFFF;
        xwu64_t icc_regval;
        xwu8_t ffs;
        xwu8_t step;

        for (i = 0; i < ARMV8A_SGI_NUM + ARMV8A_PPI_NUM; i += 8U) {
                armv8a_gic3_runtime_data.cpu[cpuid].pivt[i + 0U] = NULL;
                armv8a_gic3_runtime_data.cpu[cpuid].pivt[i + 1U] = NULL;
                armv8a_gic3_runtime_data.cpu[cpuid].pivt[i + 2U] = NULL;
                armv8a_gic3_runtime_data.cpu[cpuid].pivt[i + 3U] = NULL;
                armv8a_gic3_runtime_data.cpu[cpuid].pivt[i + 4U] = NULL;
                armv8a_gic3_runtime_data.cpu[cpuid].pivt[i + 5U] = NULL;
                armv8a_gic3_runtime_data.cpu[cpuid].pivt[i + 6U] = NULL;
                armv8a_gic3_runtime_data.cpu[cpuid].pivt[i + 7U] = NULL;
        }

        armv8a_gic3_runtime_data.cpu[cpuid].irqn = (xwirq_t)0x3FF;
        soc_dbgf("GIC3",
                 "Init CPU%d GICC, R.SGI:0x%lX\r\n",
                 cpuid, (xwu64_t)&armv8a_gic3_r_sgi(cpuid));

        /* Disable NS Group1 interrupts */
        armv8a_sysreg_read(&icc_regval, icc_igrpen1_el1);
        icc_regval &= ~1UL;
        armv8a_sysreg_write(icc_igrpen1_el1, icc_regval);

        access = armv8a_gic3_r_sgi(cpuid).igroupr0 | armv8a_gic3_r_sgi(cpuid).igrpmodr0;
        soc_dbgf("GIC3",
                 "IRQ[0...31] igroupr0:0x%X, igrpmodr0:0x%X, access:0x%X\r\n",
                 armv8a_gic3_r_sgi(cpuid).igroupr0,
                 armv8a_gic3_r_sgi(cpuid).igrpmodr0,
                 access);

        armv8a_gic3_r_sgi(cpuid).igroupr0 = access;
        armv8a_gic3_r_sgi(cpuid).igrpmodr0 = 0U;
        armv8a_gic3_r_sgi(cpuid).icenabler0 = (xwu32_t)0xFFFF0000 | access;
        armv8a_gic3_r_sgi(cpuid).icpendr0 = (xwu32_t)0xFFFF0000 | access;

        soc_dbgf("GIC3",
                 "IRQ[0...31] isenabler0:0x%X, icpendr0:0x%X\r\n",
                 armv8a_gic3_r_sgi(cpuid).isenabler0,
                 armv8a_gic3_r_sgi(cpuid).icpendr0);

	/* Set PPIs(16~31) to mask priority */
	for (i = 16U; i < (ARMV8A_SGI_NUM + ARMV8A_PPI_NUM); i += 4U) {
                armv8a_gic3_r_sgi(cpuid).ipriorityr[i >> 2U] = (xwu32_t)0xFFFFFFFF;
                soc_dbgf("GIC3", "Set ipriorityr[%d]:0x%X\r\n",
                         i >> 2U, armv8a_gic3_r_sgi(cpuid).ipriorityr[i >> 2U]);
	}

	/* unmask interrupts at all priority levels */
        icc_regval = (xwu64_t)0xFF;
        armv8a_sysreg_write(icc_pmr_el1, icc_regval);
        icc_regval = (xwu64_t)0;
        armv8a_sysreg_write(icc_bpr1_el1, icc_regval);
        armv8a_sysreg_read(&icc_regval, icc_pmr_el1);
        armv8a_gic3_runtime_data.cpu[cpuid].priority.max = (xwu8_t)0;
        armv8a_gic3_runtime_data.cpu[cpuid].priority.min = (xwu8_t)icc_regval;
        ffs = xwbop_ffs8(armv8a_gic3_runtime_data.cpu[cpuid].priority.min);
        armv8a_gic3_runtime_data.cpu[cpuid].priority.shift = ffs;
        step = (xwu8_t)1 << armv8a_gic3_runtime_data.cpu[cpuid].priority.shift;
        armv8a_gic3_runtime_data.cpu[cpuid].priority.min -= step;
        soc_dbgf("GIC3",
                 "soc.gic3.priority.max:0x%X, "
                 "soc.gic3.priority.min:0x%X, "
                 "soc.gic3.priority.shift:0x%X\r\n",
                 armv8a_gic3_runtime_data.cpu[cpuid].priority.max,
                 armv8a_gic3_runtime_data.cpu[cpuid].priority.min,
                 armv8a_gic3_runtime_data.cpu[cpuid].priority.shift);

	/* Enable NS Group1 interrupts */
        armv8a_sysreg_read(&icc_regval, icc_igrpen1_el1);
        icc_regval |= 1UL;
        armv8a_sysreg_write(icc_igrpen1_el1, icc_regval);
}

__xwbsp_isr
void armv8a_gic_isr(xwid_t cpuid)
{
        xwu64_t irqn;
        xwu64_t rpr;
        xwu64_t hppir1;
        bool en;

        armv8a_sysreg_read(&irqn, icc_iar1_el1);
        irqn &= (xwu64_t)0xFFFFFF;
        armv8a_gic3_runtime_data.cpu[cpuid].irqn = (xwirq_t)irqn;
        armv8a_sysreg_read(&rpr, icc_rpr_el1);
        rpr &= (xwu64_t)0xFFFFFF;
        armv8a_sysreg_read(&hppir1, icc_hppir1_el1);
        hppir1 &= (xwu64_t)0xFFFFFF;
        armv8a_isb();
        soc_dbgf("GIC3",
                 "CPU%d ISR[%d], RPR:0x%X, HPPIR1:0x%X\r\n",
                 cpuid, armv8a_gic3_runtime_data.cpu[cpuid].irqn, rpr, hppir1);

        if (irqn < (ARMV8A_SGI_NUM + ARMV8A_PPI_NUM)) {
                armv8a_gic_irq_get_state(irqn, &en, NULL, NULL);
                if (en && (NULL != armv8a_gic3_runtime_data.cpu[cpuid].pivt[irqn])) {
                        armv8a_gic3_runtime_data.cpu[cpuid].pivt[irqn]();
                }
        } else if (irqn < armv8a_gic3_runtime_data.it_lines_num) {
                armv8a_gic_irq_get_state(irqn, &en, NULL, NULL);
                if (en && (NULL != armv8a_gic3_runtime_data.sivt[irqn])) {
                        armv8a_gic3_runtime_data.sivt[irqn]();
                }
        } else {
        }
        armv8a_gic3_runtime_data.cpu[cpuid].irqn = (xwirq_t)0x3FF;
        armv8a_sysreg_write(icc_eoir1_el1, irqn);
}

xwu8_t armv8a_gic_get_max_priority(void)
{
	xwid_t cpuid = xwospl_skd_get_cpuid_lc();
        return armv8a_gic3_runtime_data.cpu[cpuid].priority.max;
}

xwu8_t armv8a_gic_get_min_priority(void)
{
	xwid_t cpuid = xwospl_skd_get_cpuid_lc();
        return armv8a_gic3_runtime_data.cpu[cpuid].priority.min;
}

xwu8_t armv8a_gic_raise_priority(xwu8_t pr, xwu8_t delta)
{
	xwid_t cpuid = xwospl_skd_get_cpuid_lc();
        xwu8_t max = armv8a_gic3_runtime_data.cpu[cpuid].priority.max;
        xwu8_t ffs = armv8a_gic3_runtime_data.cpu[cpuid].priority.shift;
        xwu8_t raise = delta << ffs;
        xwu8_t res = pr - raise;
        if (res > pr) {
                res = max;
        }
        return res;
}

xwu8_t armv8a_gic_drop_priority(xwu8_t pr, xwu8_t delta)
{
	xwid_t cpuid = xwospl_skd_get_cpuid_lc();
        xwu8_t min = armv8a_gic3_runtime_data.cpu[cpuid].priority.min;
        xwu8_t ffs = armv8a_gic3_runtime_data.cpu[cpuid].priority.shift;
        xwu8_t drop = delta << ffs;
        xwu8_t res = pr + drop;
        if (res > min) {
                res = min;
        }
        return res;
}

__xwbsp_code
xwer_t armv8a_gic_irq_get_id(xwirq_t * irqnbuf)
{
	xwid_t cpuid = xwospl_skd_get_cpuid_lc();
        xwer_t rc;

        *irqnbuf = armv8a_gic3_runtime_data.cpu[cpuid].irqn;
        if ((xwirq_t)0x3FF == *irqnbuf) {
                rc = -ENOTISRCTX;
        } else {
                rc = XWOK;
        }
        return rc;
}

__xwbsp_code
xwer_t armv8a_gic_irq_enable(xwirq_t irqn)
{
        xwer_t rc;
        xwu32_t ridx;
        xwu32_t bitmask;
        xwid_t cpuid;

        cpuid = xwospl_skd_get_cpuid_lc();
        if (irqn < (xwirq_t)0) {
                rc = -ENOSYS;
        } else if (irqn < (xwirq_t)(ARMV8A_SGI_NUM + ARMV8A_PPI_NUM)) {
                bitmask = (xwu32_t)1 << ((xwu32_t)irqn & (xwu32_t)0x1F);
                armv8a_gic3_r_sgi(cpuid).isenabler0 = bitmask;
                soc_dbgf("GIC3",
                         "CPU%d enables IRQ(%d) bitmask:0x%X, "
                         "gic3r_sgi(%d).isenabler0@0x%lX:0x%X\r\n",
                         cpuid, irqn, bitmask,
                         cpuid,
                         (xwu64_t)&armv8a_gic3_r_sgi(cpuid).isenabler0,
                         armv8a_gic3_r_sgi(cpuid).isenabler0);
                rc = XWOK;
        } else if ((xwu32_t)irqn < armv8a_gic3_runtime_data.it_lines_num) {
                ridx = irqn >> (xwu32_t)5;
                bitmask = (xwu32_t)1 << ((xwu32_t)irqn & (xwu32_t)0x1F);
                armv8a_gic3_d.isenabler[ridx] = bitmask;
                soc_dbgf("GIC3",
                         "CPU%d enables IRQ(%d) ridx:0x%X, bitmask:0x%X, "
                         "gic3d.isenabler[%d]@0x%lX:0x%X\r\n",
                         cpuid, irqn, ridx, bitmask,
                         ridx,
                         (xwu64_t)&armv8a_gic3_d.isenabler[ridx],
                         armv8a_gic3_d.isenabler[ridx]);
                rc = XWOK;
        } else {
                rc = -ERANGE;
        }
        return rc;
}

__xwbsp_code
xwer_t armv8a_gic_irq_disable(xwirq_t irqn)
{
        xwer_t rc;
        xwu32_t ridx;
        xwu32_t bitmask;
        xwid_t cpuid;

        cpuid = xwospl_skd_get_cpuid_lc();
        if (irqn < (xwirq_t)0) {
                rc = -ENOSYS;
        } else if (irqn < (xwirq_t)(ARMV8A_SGI_NUM + ARMV8A_PPI_NUM)) {
                bitmask = (xwu32_t)1 << ((xwu32_t)irqn & (xwu32_t)0x1F);
                armv8a_gic3_r_sgi(cpuid).icenabler0 = bitmask;
                soc_dbgf("GIC3",
                         "CPU%d disables IRQ(%d) bitmask:0x%X, "
                         "gic3r_sgi(%d).icenabler0@0x%lX:0x%X\r\n",
                         cpuid, irqn, bitmask,
                         cpuid,
                         (xwu64_t)&armv8a_gic3_r_sgi(cpuid).icenabler0,
                         armv8a_gic3_r_sgi(cpuid).icenabler0);
                rc = XWOK;
        } else if ((xwu32_t)irqn < armv8a_gic3_runtime_data.it_lines_num) {
                ridx = irqn >> (xwu32_t)5;
                bitmask = (xwu32_t)1 << ((xwu32_t)irqn & (xwu32_t)0x1F);
                armv8a_gic3_d.icenabler[ridx] = bitmask;
                soc_dbgf("GIC3",
                         "CPU%d disables IRQ(%d) ridx:0x%X, bitmask:0x%X "
                         "gic3d.icenabler[%d]@0x%lX:0x%X\r\n",
                         cpuid, irqn, ridx, bitmask,
                         ridx,
                         (xwu64_t)&armv8a_gic3_d.icenabler[ridx],
                         armv8a_gic3_d.icenabler[ridx]);
                rc = XWOK;
        } else {
                rc = -ERANGE;
        }
        return rc;
}

__xwbsp_code
xwer_t armv8a_gic_irq_save(xwirq_t irqn, xwreg_t * flag)
{
        xwer_t rc;
        xwu32_t ridx;
        xwu32_t bitmask;
        xwid_t cpuid;

        cpuid = xwospl_skd_get_cpuid_lc();
        if (irqn < (xwirq_t)0) {
                rc = -ENOSYS;
        } else if (irqn < (xwirq_t)(ARMV8A_SGI_NUM + ARMV8A_PPI_NUM)) {
                bitmask = (xwu32_t)1 << ((xwu32_t)irqn & (xwu32_t)0x1F);
                *flag = (armv8a_gic3_r_sgi(cpuid).isenabler0 & bitmask);
                armv8a_gic3_r_sgi(cpuid).icenabler0 = bitmask;
                soc_dbgf("GIC3",
                         "CPU%d save IRQ(%d) bitmask:0x%X, flag:0x%X, "
                         "gic3r_sgi(%d).icenabler0@0x%lX:0x%X\r\n",
                         cpuid, irqn, bitmask, *flag,
                         cpuid,
                         (xwu64_t)&armv8a_gic3_r_sgi(cpuid).icenabler0,
                         armv8a_gic3_r_sgi(cpuid).icenabler0);
                rc = XWOK;
        } else if ((xwu32_t)irqn < armv8a_gic3_runtime_data.it_lines_num) {
                ridx = irqn >> (xwu32_t)5;
                bitmask = (xwu32_t)1 << ((xwu32_t)irqn & (xwu32_t)0x1F);
                *flag = (armv8a_gic3_d.isenabler[ridx] & bitmask);
                armv8a_gic3_d.icenabler[ridx] |= bitmask;
                soc_dbgf("GIC3",
                         "CPU%d save IRQ(%d) ridx:0x%X, bitmask:0x%X, flag:0x%X, "
                         "gic3d.icenabler[%d]@0x%lX:0x%X\r\n",
                         cpuid, irqn, ridx, bitmask, *flag,
                         ridx,
                         (xwu64_t)&armv8a_gic3_d.icenabler[ridx],
                         armv8a_gic3_d.icenabler[ridx]);
                rc = XWOK;
        } else {
                rc = -ERANGE;
        }
        return rc;
}

__xwbsp_code
xwer_t armv8a_gic_irq_restore(xwirq_t irqn, xwreg_t flag)
{
        xwer_t rc;
        xwu32_t ridx;
        xwu32_t bitmask;
        xwid_t cpuid;

        cpuid = xwospl_skd_get_cpuid_lc();
        if (irqn < (xwirq_t)0) {
                rc = -ENOSYS;
        } else if (irqn < (xwirq_t)(ARMV8A_SGI_NUM + ARMV8A_PPI_NUM)) {
                bitmask = (xwu32_t)1 << ((xwu32_t)irqn & (xwu32_t)0x1F);
                if (flag & bitmask) {
                        armv8a_gic3_r_sgi(cpuid).isenabler0 = bitmask;
                } else {
                        armv8a_gic3_r_sgi(cpuid).icenabler0 = bitmask;
                }
                soc_dbgf("GIC3",
                         "CPU%d restore IRQ(%d) bitmask:0x%X, flag:0x%X "
                         "gic3r_sgi(%d).icenabler0@0x%lX:0x%X\r\n",
                         cpuid, irqn, bitmask, flag,
                         cpuid,
                         (xwu64_t)&armv8a_gic3_r_sgi(cpuid).icenabler0,
                         armv8a_gic3_r_sgi(cpuid).icenabler0);
                rc = XWOK;
        } else if ((xwu32_t)irqn < armv8a_gic3_runtime_data.it_lines_num) {
                ridx = irqn >> (xwu32_t)5;
                bitmask = (xwu32_t)1 << ((xwu32_t)irqn & (xwu32_t)0x1F);
                if (flag & bitmask) {
                        armv8a_gic3_d.isenabler[ridx] |= bitmask;
                } else {
                        armv8a_gic3_d.icenabler[ridx] |= bitmask;
                }
                soc_dbgf("GIC3",
                         "CPU%d restore IRQ(%d) ridx:0x%X, bitmask:0x%X, flag:0x%X "
                         "gic3d.isenabler[%d]@0x%lX:0x%X\r\n",
                         cpuid, irqn, ridx, bitmask, flag,
                         ridx,
                         (xwu64_t)&armv8a_gic3_d.icenabler[ridx],
                         armv8a_gic3_d.icenabler[ridx]);
                rc = XWOK;
        } else {
                rc = -ERANGE;
        }
        return rc;
}

__xwbsp_code
xwer_t armv8a_gic_irq_set_priority(xwirq_t irqn, xwu8_t pr)
{
        xwer_t rc;
        xwu32_t ridx;
        xwu32_t bidx;
        xwu32_t bitmask;
        xwid_t cpuid;

        cpuid = xwospl_skd_get_cpuid_lc();
        if (irqn < (xwirq_t)0) {
                rc = -ENOSYS;
        } else if (irqn < (xwirq_t)(ARMV8A_SGI_NUM + ARMV8A_PPI_NUM)) {
                ridx = irqn >> (xwu32_t)2; /* ridx = irqn / 4 */
                bidx = ((xwu32_t)irqn & (xwu32_t)0x3) << (xwu32_t)3; /* bidx = (irqn % 4) * 8 */
                bitmask = (xwu32_t)0xFF << bidx;
                armv8a_gic3_r_sgi(cpuid).ipriorityr[ridx] &= ~bitmask;
                armv8a_gic3_r_sgi(cpuid).ipriorityr[ridx] |= ((pr << bidx) & bitmask);
                soc_dbgf("GIC3",
                         "CPU%d set IRQ(%d) priority: ridx:0x%X, bidx:0x%X, "
                         "bitmask:0x%X, pr:0x%X "
                         "gic3r_sgi(%d).ipriorityr[%d]@0x%lX:0x%X\r\n",
                         cpuid, irqn, ridx, bidx, bitmask, pr,
                         cpuid, ridx,
                         &armv8a_gic3_r_sgi(cpuid).ipriorityr[ridx],
                         armv8a_gic3_r_sgi(cpuid).ipriorityr[ridx]);
                rc = XWOK;
        } else if ((xwu32_t)irqn < armv8a_gic3_runtime_data.it_lines_num) {
                ridx = irqn >> (xwu32_t)2; /* ridx = irqn / 4 */
                bidx = ((xwu32_t)irqn & (xwu32_t)0x3) << (xwu32_t)3; /* bidx = (irqn % 4) * 8 */
                bitmask = (xwu32_t)0xFF << bidx;
                armv8a_gic3_d.ipriorityr[ridx] &= ~bitmask;
                armv8a_gic3_d.ipriorityr[ridx] |= ((pr << bidx) & bitmask);
                soc_dbgf("GIC3",
                         "CPU%d set IRQ(%d) priority: ridx:0x%X, bidx:0x%X, "
                         "bitmask:0x%X, pr:0x%X, "
                         "gic3d.ipriorityr[%d]@0x%lX:0x%X\r\n",
                         cpuid, irqn, ridx, bidx, bitmask, pr,
                         ridx,
                         &armv8a_gic3_d.ipriorityr[ridx],
                         armv8a_gic3_d.ipriorityr[ridx]);
                rc = XWOK;
        } else {
                rc = -ERANGE;
        }
        return rc;
}

__xwbsp_code
xwer_t armv8a_gic_irq_get_priority(xwirq_t irqn, xwu8_t * pr)
{
        xwer_t rc;
        xwu32_t ridx;
        xwu32_t bidx;
        xwu32_t bitmask;
        xwu32_t rval;
        xwid_t cpuid;

        cpuid = xwospl_skd_get_cpuid_lc();
        if (irqn < (xwirq_t)0) {
                rc = -ENOSYS;
        } else if (irqn < (xwirq_t)(ARMV8A_SGI_NUM + ARMV8A_PPI_NUM)) {
                ridx = irqn >> (xwu32_t)2; /* ridx = irqn / 4 */
                bidx = ((xwu32_t)irqn & (xwu32_t)0x3) << (xwu32_t)3; /* bidx = (irqn % 4) * 8 */
                bitmask = (xwu32_t)0xFF << bidx;
                rval = armv8a_gic3_r_sgi(cpuid).ipriorityr[ridx] & bitmask;
                if (NULL != pr) {
                        *pr = rval >> bidx;
                }
                soc_dbgf("GIC3",
                         "CPU%d get IRQ(%d) priority: ridx:0x%X, bidx:0x%X, "
                         "bitmask:0x%X, rval:0x%X, "
                         "gic3r_sgi(%d).ipriorityr[%d]@0x%lX:0x%X\r\n",
                         cpuid, irqn, ridx, bidx, bitmask, rval,
                         cpuid, ridx,
                         &armv8a_gic3_r_sgi(cpuid).ipriorityr[ridx],
                         armv8a_gic3_r_sgi(cpuid).ipriorityr[ridx]);
                rc = XWOK;
        } else if ((xwu32_t)irqn < armv8a_gic3_runtime_data.it_lines_num) {
                ridx = irqn >> (xwu32_t)2; /* ridx = irqn / 4 */
                bidx = ((xwu32_t)irqn & (xwu32_t)0x3) << (xwu32_t)3; /* bidx = (irqn % 4) * 8 */
                bitmask = (xwu32_t)0xFF << bidx;
                rval = armv8a_gic3_d.ipriorityr[ridx] & bitmask;
                if (NULL != pr) {
                        *pr = rval >> bidx;
                }
                soc_dbgf("GIC3",
                         "CPU%d get IRQ(%d) priority: ridx:0x%X, bidx:0x%X, "
                         "bitmask:0x%X, rval:0x%X, "
                         "gic3d.ipriorityr[%d]@0x%lX:0x%X\r\n",
                         cpuid, irqn, ridx, bidx, bitmask, rval,
                         ridx,
                         &armv8a_gic3_d.ipriorityr[ridx],
                         armv8a_gic3_d.ipriorityr[ridx]);
                rc = XWOK;
        } else {
                rc = -ERANGE;
        }
        return rc;
}

__xwbsp_code
xwer_t armv8a_gic_irq_set_trigger_type(xwirq_t irqn, xwu32_t type)
{
        xwer_t rc;
        xwu32_t ridx;
        xwu32_t bidx;
        xwu32_t bitmask;

        if (irqn < (xwirq_t)0) {
                rc = -ENOSYS;
        } else if (irqn < (xwirq_t)(ARMV8A_SGI_NUM + ARMV8A_PPI_NUM)) {
                rc = -EACCES;
        } else if ((xwu32_t)irqn < armv8a_gic3_runtime_data.it_lines_num) {
                ridx = irqn >> (xwu32_t)4; /* ridx = irqn / 16 */
                bidx = ((xwu32_t)irqn & (xwu32_t)0xF) << (xwu32_t)1; /* bidx = (irqn % 16) * 2 */
                bitmask = (xwu32_t)0x3 << bidx;
                armv8a_gic3_d.icfgr[ridx] &= ~bitmask;
                armv8a_gic3_d.icfgr[ridx] |= ((type << bidx) & bitmask);
                soc_dbgf("GIC3",
                         "CPU%d set IRQ(%d) type: ridx:0x%X, bidx:0x%X, "
                         "bitmask:0x%X, type:0x%X, "
                         "gic3d.icfgr[%d]@0x%lX:0x%X\r\n",
                         xwospl_skd_get_cpuid_lc(), irqn, ridx, bidx, bitmask, type,
                         ridx,
                         &armv8a_gic3_d.icfgr[ridx],
                         armv8a_gic3_d.icfgr[ridx]);
                rc = XWOK;
        } else {
                rc = -ERANGE;
        }
        return rc;
}

__xwbsp_code
xwer_t armv8a_gic_irq_get_trigger_type(xwirq_t irqn, xwu32_t * type)
{
        xwer_t rc;
        xwu32_t ridx;
        xwu32_t bidx;
        xwu32_t bitmask;
        xwu32_t rval;
        xwid_t cpuid;

        cpuid = xwospl_skd_get_cpuid_lc();
        if (irqn < (xwirq_t)0) {
                rc = -ENOSYS;
        } else if (irqn < (xwirq_t)(ARMV8A_SGI_NUM + ARMV8A_PPI_NUM)) {
                ridx = irqn >> (xwu32_t)4; /* ridx = irqn / 16 */
                bidx = ((xwu32_t)irqn & (xwu32_t)0xF) << (xwu32_t)1; /* bidx = (irqn % 16) * 2 */
                bitmask = (xwu32_t)0x3 << bidx;
                rval = armv8a_gic3_r_sgi(cpuid).icfgr[ridx] & bitmask;
                if (NULL != type) {
                        *type = rval >> bidx;
                }
                soc_dbgf("GIC3",
                         "CPU%d get IRQ(%d) trigger type: ridx:0x%X, bidx:0x%X, "
                         "bitmask:0x%X, rval:0x%X, "
                         "gic3r_sgi(%d).icfgr[%d]@0x%lX:0x%X\r\n",
                         cpuid, irqn, ridx, bidx, bitmask, rval,
                         cpuid, ridx,
                         &armv8a_gic3_r_sgi(cpuid).icfgr[ridx],
                         armv8a_gic3_r_sgi(cpuid).icfgr[ridx]);
                rc = XWOK;
        } else if ((xwu32_t)irqn < armv8a_gic3_runtime_data.it_lines_num) {
                ridx = irqn >> (xwu32_t)4; /* ridx = irqn / 16 */
                bidx = ((xwu32_t)irqn & (xwu32_t)0xF) << (xwu32_t)1; /* bidx = (irqn % 16) * 2 */
                bitmask = (xwu32_t)0x3 << bidx;
                rval = armv8a_gic3_d.icfgr[ridx] & bitmask;
                if (NULL != type) {
                        *type = rval >> bidx;
                }
                soc_dbgf("GIC3",
                         "CPU%d get IRQ(%d) trigger type: ridx:0x%X, bidx:0x%X, "
                         "bitmask:0x%X, rval:0x%X, "
                         "gic3d.icfgr[%d]@0x%lX:0x%X\r\n",
                         cpuid, irqn, ridx, bidx, bitmask, rval,
                         ridx,
                         &armv8a_gic3_d.icfgr[ridx],
                         armv8a_gic3_d.icfgr[ridx]);
                rc = XWOK;
        } else {
                rc = -ERANGE;
        }
        return rc;
}

__xwbsp_code
xwer_t armv8a_gic_irq_set_affinity_lc(xwirq_t irqn)
{
        xwer_t rc;

        if (0 == armv8a_gic3_d.ctrl.ns.are_ns) {
                rc = -ENOSYS;
        } else if (irqn < (xwirq_t)0) {
                rc = -ENOSYS;
        } else if (irqn < (xwirq_t)(ARMV8A_SGI_NUM + ARMV8A_PPI_NUM)) {
                rc = -EALREADY;
        } else if ((xwu32_t)irqn < armv8a_gic3_runtime_data.it_lines_num) {
                xwu64_t mpidr;
                armv8a_sysreg_read(&mpidr, mpidr_el1);
                armv8a_gic3_d.irouter[irqn - 32] = mpidr & 0xFF00FFFFFFUL;
                soc_dbgf("GIC3",
                         "set IRQ(%d) affinity: mpidr:0x%lX, "
                         "gic3d.irouter[%d]@0x%lX:0x%X\r\n",
                         irqn, mpidr,
                         irqn - (xwirq_t)32,
                         &armv8a_gic3_d.irouter[irqn - (xwirq_t)32],
                         armv8a_gic3_d.irouter[irqn - (xwirq_t)32]);
                rc = XWOK;
        } else {
                rc = -ERANGE;
        }
        return rc;
}

__xwbsp_code
xwer_t armv8a_gic_irq_get_state(xwirq_t irqn,
                                bool * enable, bool * pending, bool * active)
{
        xwer_t rc;
        xwu32_t ridx;
        xwu32_t bitmask;
        xwid_t cpuid;
        xwu32_t enable_state;
        xwu32_t pending_state;
        xwu32_t active_state;

        cpuid = xwospl_skd_get_cpuid_lc();
        if (irqn < (xwirq_t)0) {
                rc = -ENOSYS;
        } else if (irqn < (xwirq_t)(ARMV8A_SGI_NUM + ARMV8A_PPI_NUM)) {
                bitmask = (xwu32_t)1 << ((xwu32_t)irqn & (xwu32_t)0x1F);
                enable_state = (armv8a_gic3_r_sgi(cpuid).isenabler0 & bitmask);
                pending_state = (armv8a_gic3_r_sgi(cpuid).ispendr0 & bitmask);
                active_state = (armv8a_gic3_r_sgi(cpuid).isactiver0 & bitmask);
                soc_dbgf("GIC3",
                         "CPU%d Local IRQ(%d) bitmask:0x%X, "
                         "enable:0x%X, pending:0x%X, active:0x%X\r\n",
                         cpuid, irqn, bitmask,
                         enable_state, pending_state, active_state);
                if (NULL != enable) {
                        *enable = !!enable_state;
                }
                if (NULL != pending) {
                        *pending = !!pending_state;
                }
                if (NULL != active) {
                        *active = !!active_state;
                }
                rc = XWOK;
        } else if ((xwu32_t)irqn < armv8a_gic3_runtime_data.it_lines_num) {
                ridx = irqn >> (xwu32_t)5; /* ridx = irqn / 32 */
                bitmask = (xwu32_t)1 << ((xwu32_t)irqn & (xwu32_t)0x1F); /* bitmask = 1 << (irqn % 32) */
                enable_state = (armv8a_gic3_d.isenabler[ridx] & bitmask);
                pending_state = (armv8a_gic3_d.ispendr[ridx] & bitmask);
                active_state = (armv8a_gic3_d.isactiver[ridx] & bitmask);
                soc_dbgf("GIC3",
                         "Peripheral IRQ(%d) bitmask:0x%X, "
                         "gic3d.isenabler[%d]@0x%lX:0x%X, "
                         "gic3d.ispendr[%d]@0x%lX:0x%X, "
                         "gic3d.isactiver[%d]@0x%lX:0x%X\r\n",
                         irqn, bitmask,
                         ridx, &armv8a_gic3_d.isenabler[ridx],
                         armv8a_gic3_d.isenabler[ridx],
                         ridx, &armv8a_gic3_d.ispendr[ridx],
                         armv8a_gic3_d.ispendr[ridx],
                         ridx, &armv8a_gic3_d.isactiver[ridx],
                         armv8a_gic3_d.isactiver[ridx]);
                if (NULL != enable) {
                        *enable = !!enable_state;
                }
                if (NULL != pending) {
                        *pending = !!pending_state;
                }
                if (NULL != active) {
                        *active = !!active_state;
                }
                rc = XWOK;
        } else {
                rc = -ERANGE;
        }
        return rc;
}

__xwbsp_code
void armv8a_gic_irq_dump(xwirq_t irqn)
{
        xwu32_t ridx;
        xwu32_t bitmask;
        xwid_t cpuid;

        cpuid = xwospl_skd_get_cpuid_lc();
        if (irqn < (xwirq_t)0) {
        } else if (irqn < (xwirq_t)(ARMV8A_SGI_NUM + ARMV8A_PPI_NUM)) {
                bitmask = (xwu32_t)1 << ((xwu32_t)irqn & (xwu32_t)0x1F);
                soc_infof("GIC3",
                          "CPU%d Private IRQ(%d) bitmask:0x%X, "
                          "gic3r_sgi(%d).isenabler0@0x%lX:0x%X, ",
                          "gic3r_sgi(%d).ispendr0@0x%lX:0x%X, ",
                          "gic3r_sgi(%d).isactiver0@0x%lX:0x%X\r\n",
                          cpuid, irqn, bitmask,
                          cpuid, (xwu64_t)&armv8a_gic3_r_sgi(cpuid).isenabler0,
                          armv8a_gic3_r_sgi(cpuid).isenabler0,
                          cpuid, (xwu64_t)&armv8a_gic3_r_sgi(cpuid).ispendr0,
                          armv8a_gic3_r_sgi(cpuid).ispendr0,
                          cpuid, (xwu64_t)&armv8a_gic3_r_sgi(cpuid).isactiver0,
                          armv8a_gic3_r_sgi(cpuid).isactiver0);
        } else if ((xwu32_t)irqn < armv8a_gic3_runtime_data.it_lines_num) {
                ridx = irqn >> (xwu32_t)5; /* ridx = irqn / 32 */
                bitmask = (xwu32_t)1 << ((xwu32_t)irqn & (xwu32_t)0x1F); /* bitmask = 1 << (irqn % 32) */
                soc_infof("GIC3",
                          "Peripheral IRQ(%d) bitmask:0x%X, "
                          "gic3d.isenabler[%d]@0x%lX:0x%X, "
                          "gic3d.ispendr[%d]@0x%lX:0x%X, "
                          "gic3d.isactiver[%d]@0x%lX:0x%X, "
                          "gic3d.irouter[%d]@0x%lX:0x%X\r\n",
                          irqn, bitmask,
                          ridx, &armv8a_gic3_d.isenabler[ridx],
                          armv8a_gic3_d.isenabler[ridx],
                          ridx, &armv8a_gic3_d.ispendr[ridx],
                          armv8a_gic3_d.ispendr[ridx],
                          ridx, &armv8a_gic3_d.isactiver[ridx],
                          armv8a_gic3_d.isactiver[ridx],
                          irqn - (xwirq_t)32,
                          &armv8a_gic3_d.irouter[irqn - (xwirq_t)32],
                          armv8a_gic3_d.irouter[irqn - (xwirq_t)32]);
        } else {
        }
}

__xwbsp_code
xwer_t armv8a_gic_irq_set_isr(xwirq_t irqn, xwisr_f isr)
{
        xwer_t rc;
        xwid_t cpuid;

        if (irqn < (xwirq_t)0) {
                rc = -ENOSYS;
        } else if (irqn < (xwirq_t)(ARMV8A_SGI_NUM + ARMV8A_PPI_NUM)) {
                cpuid = xwospl_skd_get_cpuid_lc();
                armv8a_gic3_runtime_data.cpu[cpuid].pivt[irqn] = isr;
                rc = XWOK;
        } else if ((xwu32_t)irqn < armv8a_gic3_runtime_data.it_lines_num) {
                armv8a_gic3_runtime_data.sivt[irqn] = isr;
                rc = XWOK;
        } else {
                rc = -ERANGE;
        }
        return rc;
}
