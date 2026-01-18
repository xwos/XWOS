/**
 * @file
 * @brief 架构描述层：ARMv8A Generic Interrupt Controller Version 2
 * @author
 * + 隐星曜 (Roy Sun) <xwos@xwos.tech>
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

#include <xwcd/soc/arm64/v8a/arch_gic2.h>
#include <xwos/lib/xwbop.h>
#include <xwos/ospl/skd.h>

/* #define SOC_DBGF */
#include <soc_debug.h>

struct __xwcc_alignl1cache armv8a_gic2_percpu_runtime_data {
        xwirq_t irqn; /**< CPU正在处理的中断号 */
        struct {
                xwu8_t max;
                xwu8_t min;
                xwu8_t shift;
        } priority; /**< 优先级 */
        xwisr_f pivt[ARMV8A_SGI_NUM + ARMV8A_PPI_NUM]; /**< 私有中断向量表 */
};

struct __xwcc_alignl1cache armv8a_gic2_runtime_data {
        xwu32_t it_lines_num; /**< 中断数量 */
        xwu32_t cpu_num; /**< CPU数量 */
        bool security; /**< 是否支持安全扩展 */
        xwisr_f sivt[SOCCFG_IRQ_NUM]; /**< 共享中断向量表 */
        struct armv8a_gic2_percpu_runtime_data cpu[CPUCFG_CPU_NUM];
};

__xwbsp_data
struct armv8a_gic2_runtime_data armv8a_gic2_runtime_data = {
        .it_lines_num = 512,
        .cpu_num = 4,
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
void armv8a_gic2_init_runtime(void)
{
        xwu32_t i;

        armv8a_gic2_runtime_data.it_lines_num = ((armv8a_gic2.d.typer & (xwu32_t)0x1F) +
                                                 (xwu32_t)1) * (xwu32_t)32;
        armv8a_gic2_runtime_data.cpu_num = ((armv8a_gic2.d.typer >> (xwu32_t)5) &
                                            (xwu32_t)0x7) + (xwu32_t)1;
        armv8a_gic2_runtime_data.security = ((armv8a_gic2.d.typer >> (xwu32_t)10) &
                                             (xwu32_t)0x1);
        soc_dbgf("GIC2", "Init GIC2: D@0x%lX,C@0x%lX\r\n",
                 SOC_GIC2_REGBASE + SOC_GIC2_D_OFFSET,
                 SOC_GIC2_REGBASE + SOC_GIC2_C_OFFSET);
        soc_dbgf("GIC2",
                 "soc.gic2.it_lines_num:%d, "
                 "soc.gic2.cpu_num:%d, "
                 "soc.gic2.security:%d\r\n",
                 armv8a_gic2_runtime_data.it_lines_num,
                 armv8a_gic2_runtime_data.cpu_num,
                 armv8a_gic2_runtime_data.security);

        for (i = 0; i < SOCCFG_IRQ_NUM; i += 8U) {
                armv8a_gic2_runtime_data.sivt[i + 0U] = NULL;
                armv8a_gic2_runtime_data.sivt[i + 1U] = NULL;
                armv8a_gic2_runtime_data.sivt[i + 2U] = NULL;
                armv8a_gic2_runtime_data.sivt[i + 3U] = NULL;
                armv8a_gic2_runtime_data.sivt[i + 4U] = NULL;
                armv8a_gic2_runtime_data.sivt[i + 5U] = NULL;
                armv8a_gic2_runtime_data.sivt[i + 6U] = NULL;
                armv8a_gic2_runtime_data.sivt[i + 7U] = NULL;
        }
}

__xwbsp_code
void armv8a_gic2_init_distributor(void)
{
        xwu32_t i;

        armv8a_gic2.d.ctrl.u32 = 0U;
        /* Distribute all SGI to cpu 0 */
        for (i = 32; i < armv8a_gic2_runtime_data.it_lines_num; i += 4U) {
                armv8a_gic2.d.itargetsr[i >> 2U] = (xwu32_t)0x01010101;
        }
	/* Set all SGI to be level triggered, active low */
        for (i = 32; i < armv8a_gic2_runtime_data.it_lines_num; i += 16U) {
                armv8a_gic2.d.icfgr[i >> 4U] = (xwu32_t)0x0;
        }
	/* Set all SGI, PPI, SPI to non-security group 1 */
	for (i = 0; i < armv8a_gic2_runtime_data.it_lines_num; i += 32U) {
                armv8a_gic2.d.igroupr[i >> 5U] = (xwu32_t)0xFFFFFFFF;
	}
	/* Set default priority */
	for (i = 0; i < armv8a_gic2_runtime_data.it_lines_num; i += 4U) {
                armv8a_gic2.d.ipriorityr[i >> 2U] = (xwu32_t)0xA0A0A0A0;
	}
	/* Deactivate and disable all SGI, PPI, SPI */
	for (i = 0; i < armv8a_gic2_runtime_data.it_lines_num; i += 32U) {
                armv8a_gic2.d.icactiver[i >> 5U] = (xwu32_t)0xFFFFFFFF;
                armv8a_gic2.d.icpendr[i >> 5U] = (xwu32_t)0xFFFFFFFF;
	}
        armv8a_gic2.d.ctrl.u32 = 3U;
}

__xwbsp_code
void armv8a_gic2_init_cpuif(void)
{
        xwu32_t i;
        xwid_t cpuid = xwospl_skd_get_cpuid_lc();
        xwu8_t ffs;
        xwu8_t step;

        for (i = 0; i < ARMV8A_SGI_NUM + ARMV8A_PPI_NUM; i += 8U) {
                armv8a_gic2_runtime_data.cpu[cpuid].pivt[i + 0U] = NULL;
                armv8a_gic2_runtime_data.cpu[cpuid].pivt[i + 1U] = NULL;
                armv8a_gic2_runtime_data.cpu[cpuid].pivt[i + 2U] = NULL;
                armv8a_gic2_runtime_data.cpu[cpuid].pivt[i + 3U] = NULL;
                armv8a_gic2_runtime_data.cpu[cpuid].pivt[i + 4U] = NULL;
                armv8a_gic2_runtime_data.cpu[cpuid].pivt[i + 5U] = NULL;
                armv8a_gic2_runtime_data.cpu[cpuid].pivt[i + 6U] = NULL;
                armv8a_gic2_runtime_data.cpu[cpuid].pivt[i + 7U] = NULL;
        }

        armv8a_gic2_runtime_data.cpu[cpuid].irqn = (xwirq_t)0x3FF;
        armv8a_gic2.c.pmr = (xwu8_t)0xFF;
        armv8a_gic2.c.bpr = (xwu8_t)0;
        armv8a_gic2.c.abpr = (xwu8_t)0;
        armv8a_gic2_runtime_data.cpu[cpuid].priority.max = (xwu8_t)0;
        armv8a_gic2_runtime_data.cpu[cpuid].priority.min = armv8a_gic2.c.pmr;
        ffs = xwbop_ffs8(armv8a_gic2_runtime_data.cpu[cpuid].priority.min);
        armv8a_gic2_runtime_data.cpu[cpuid].priority.shift = ffs;
        step = (xwu8_t)1 << armv8a_gic2_runtime_data.cpu[cpuid].priority.shift;
        armv8a_gic2_runtime_data.cpu[cpuid].priority.min -= step;
        soc_dbgf("GIC2",
                 "soc.gic2.priority.max:0x%X, "
                 "soc.gic2.priority.min:0x%X, "
                 "soc.gic2.priority.shift:0x%X\r\n",
                 armv8a_gic2_runtime_data.cpu[cpuid].priority.max,
                 armv8a_gic2_runtime_data.cpu[cpuid].priority.min,
                 armv8a_gic2_runtime_data.cpu[cpuid].priority.shift);
        armv8a_gic2.c.ctrl.u32 = 0x63U;
}

__xwbsp_isr
void armv8a_gic_isr(xwid_t cpuid)
{
        bool en;
        xwu32_t irqn;

        irqn = armv8a_gic2.c.iar & (xwu32_t)0x3FF;
        armv8a_gic2_runtime_data.cpu[cpuid].irqn = (xwirq_t)irqn;
        if (irqn < (ARMV8A_SGI_NUM + ARMV8A_PPI_NUM)) {
                armv8a_gic_irq_get_state(irqn, &en, NULL, NULL);
                if (en && (NULL != armv8a_gic2_runtime_data.cpu[cpuid].pivt[irqn])) {
                        armv8a_gic2_runtime_data.cpu[cpuid].pivt[irqn]();
                }
        } else if (irqn < armv8a_gic2_runtime_data.it_lines_num) {
                armv8a_gic_irq_get_state(irqn, &en, NULL, NULL);
                if (en && (NULL != armv8a_gic2_runtime_data.sivt[irqn])) {
                        armv8a_gic2_runtime_data.sivt[irqn]();
                }
        } else {
        }
        armv8a_gic2_runtime_data.cpu[cpuid].irqn = (xwirq_t)0x3FF;
        armv8a_gic2.c.eoir = irqn;
}

xwu8_t armv8a_gic_get_max_priority(void)
{
	xwid_t cpuid = xwospl_skd_get_cpuid_lc();
        return armv8a_gic2_runtime_data.cpu[cpuid].priority.max;
}

xwu8_t armv8a_gic_get_min_priority(void)
{
	xwid_t cpuid = xwospl_skd_get_cpuid_lc();
        return armv8a_gic2_runtime_data.cpu[cpuid].priority.min;
}

xwu8_t armv8a_gic_raise_priority(xwu8_t pr, xwu8_t delta)
{
	xwid_t cpuid = xwospl_skd_get_cpuid_lc();
        xwu8_t max = armv8a_gic2_runtime_data.cpu[cpuid].priority.max;
        xwu8_t ffs = armv8a_gic2_runtime_data.cpu[cpuid].priority.shift;
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
        xwu8_t min = armv8a_gic2_runtime_data.cpu[cpuid].priority.min;
        xwu8_t ffs = armv8a_gic2_runtime_data.cpu[cpuid].priority.shift;
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

        *irqnbuf = armv8a_gic2_runtime_data.cpu[cpuid].irqn;
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

        if (irqn < (xwirq_t)0) {
                rc = -ENOSYS;
        } else if ((xwu32_t)irqn < armv8a_gic2_runtime_data.it_lines_num) {
                ridx = irqn >> (xwu32_t)5;
                bitmask = (xwu32_t)1 << ((xwu32_t)irqn & (xwu32_t)0x1F);
                armv8a_gic2.d.isenabler[ridx] = bitmask;
                soc_dbgf("GIC2",
                         "CPU%d enables IRQ(%d) ridx:0x%X, bitmask:0x%X, "
                         "gic2d.isenabler[%d]@0x%lX:0x%X\r\n",
                         xwospl_skd_get_cpuid_lc(), irqn, ridx, bitmask,
                         ridx,
                         (xwu64_t)&armv8a_gic2.d.isenabler[ridx],
                         armv8a_gic2.d.isenabler[ridx]);
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

        if (irqn < (xwirq_t)0) {
                rc = -ENOSYS;
        } else if ((xwu32_t)irqn < armv8a_gic2_runtime_data.it_lines_num) {
                ridx = irqn >> (xwu32_t)5;
                bitmask = (xwu32_t)1 << ((xwu32_t)irqn & (xwu32_t)0x1F);
                armv8a_gic2.d.icenabler[ridx] = bitmask;
                soc_dbgf("GIC2",
                         "CPU%d disables IRQ(%d) ridx:0x%X, bitmask:0x%X "
                         "gic2d.icenabler[%d]@0x%lX:0x%X\r\n",
                         xwospl_skd_get_cpuid_lc(), irqn, ridx, bitmask,
                         ridx,
                         (xwu64_t)&armv8a_gic2.d.icenabler[ridx],
                         armv8a_gic2.d.icenabler[ridx]);
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

        if (irqn < (xwirq_t)0) {
                rc = -ENOSYS;
        } else if ((xwu32_t)irqn < armv8a_gic2_runtime_data.it_lines_num) {
                ridx = irqn >> (xwu32_t)5;
                bitmask = (xwu32_t)1 << ((xwu32_t)irqn & (xwu32_t)0x1F);
                if (NULL != flag) {
                        *flag = (armv8a_gic2.d.isenabler[ridx] & bitmask);
                }
                armv8a_gic2.d.icenabler[ridx] = bitmask;
                soc_dbgf("GIC2",
                         "CPU%d save IRQ(%d) ridx:0x%X, bitmask:0x%X, flag:0x%X, "
                         "gic2d.icenabler[%d]@0x%lX:0x%X\r\n",
                         xwospl_skd_get_cpuid_lc(), irqn, ridx, bitmask, *flag,
                         ridx,
                         (xwu64_t)&armv8a_gic2.d.icenabler[ridx],
                         armv8a_gic2.d.icenabler[ridx]);
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

        if (irqn < (xwirq_t)0) {
                rc = -ENOSYS;
        } else if ((xwu32_t)irqn < armv8a_gic2_runtime_data.it_lines_num) {
                ridx = irqn >> (xwu32_t)5;
                bitmask = (xwu32_t)1 << ((xwu32_t)irqn & (xwu32_t)0x1F);
                if (flag & bitmask) {
                        armv8a_gic2.d.isenabler[ridx] = bitmask;
                } else {
                        armv8a_gic2.d.icenabler[ridx] = bitmask;
                }
                soc_dbgf("GIC2",
                         "CPU%d restore IRQ(%d) ridx:0x%X, bitmask:0x%X, flag:0x%X "
                         "gic2d.isenabler[%d]@0x%lX:0x%X\r\n",
                         xwospl_skd_get_cpuid_lc(), irqn, ridx, bitmask, flag,
                         ridx,
                         (xwu64_t)&armv8a_gic2.d.icenabler[ridx],
                         armv8a_gic2.d.icenabler[ridx]);
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

        if (irqn < (xwirq_t)0) {
                rc = -ENOSYS;
        } else if ((xwu32_t)irqn < armv8a_gic2_runtime_data.it_lines_num) {
                ridx = irqn >> (xwu32_t)2; /* ridx = irqn / 4 */
                bidx = ((xwu32_t)irqn & (xwu32_t)0x3) << (xwu32_t)3; /* bidx = (irqn & 3) * 8 */
                bitmask = (xwu32_t)0xFF << bidx;
                armv8a_gic2.d.ipriorityr[ridx] &= ~bitmask;
                armv8a_gic2.d.ipriorityr[ridx] |= ((pr << bidx) & bitmask);
                soc_dbgf("GIC2",
                         "CPU%d set IRQ(%d) priority: ridx:0x%X, bidx:0x%X, "
                         "bitmask:0x%X, pr:0x%X, "
                         "gic2d.ipriorityr[%d]@0x%lX:0x%X\r\n",
                         xwospl_skd_get_cpuid_lc(), irqn, ridx, bidx, bitmask, pr,
                         ridx,
                         &armv8a_gic2.d.ipriorityr[ridx],
                         armv8a_gic2.d.ipriorityr[ridx]);
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

        if (irqn < (xwirq_t)0) {
                rc = -ENOSYS;
        } else if ((xwu32_t)irqn < armv8a_gic2_runtime_data.it_lines_num) {
                ridx = irqn >> (xwu32_t)2;
                bidx = (xwu32_t)irqn & (xwu32_t)0x3;
                bitmask = (xwu32_t)0xFF << bidx;
                rval = armv8a_gic2.d.ipriorityr[ridx] & bitmask;
                if (NULL != pr) {
                        *pr = rval >> bidx;
                }
                soc_dbgf("GIC2",
                         "CPU%d get IRQ(%d) priority: ridx:0x%X, bidx:0x%X, "
                         "bitmask:0x%X, rval:0x%X, "
                         "gic2d.ipriorityr[%d]@0x%lX:0x%X\r\n",
                         xwospl_skd_get_cpuid_lc(), irqn, ridx, bidx, bitmask, rval,
                         ridx,
                         &armv8a_gic2.d.ipriorityr[ridx],
                         armv8a_gic2.d.ipriorityr[ridx]);
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
        } else if ((xwu32_t)irqn < armv8a_gic2_runtime_data.it_lines_num) {
                ridx = irqn >> (xwu32_t)4; /* ridx = irqn / 16 */
                bidx = ((xwu32_t)irqn & (xwu32_t)0xF) << (xwu32_t)1; /* bidx = (irqn % 16) * 2 */
                bitmask = (xwu32_t)0x3 << bidx;
                armv8a_gic2.d.icfgr[ridx] &= ~bitmask;
                armv8a_gic2.d.icfgr[ridx] |= ((type << bidx) & bitmask);
                soc_dbgf("GIC2",
                         "CPU%d set IRQ(%d) type: ridx:0x%X, bidx:0x%X, "
                         "bitmask:0x%X, type:0x%X, "
                         "gic2d.icfgr[%d]@0x%lX:0x%X\r\n",
                         xwospl_skd_get_cpuid_lc(), irqn, ridx, bidx, bitmask, type,
                         ridx,
                         &armv8a_gic2.d.icfgr[ridx],
                         armv8a_gic2.d.icfgr[ridx]);
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

        if (irqn < (xwirq_t)0) {
                rc = -ENOSYS;
        } else if ((xwu32_t)irqn < armv8a_gic2_runtime_data.it_lines_num) {
                ridx = irqn >> (xwu32_t)4; /* ridx = irqn / 16 */
                bidx = ((xwu32_t)irqn & (xwu32_t)0xF) << (xwu32_t)1; /* bidx = (irqn % 16) * 2 */
                bitmask = (xwu32_t)0x3 << bidx;
                rval = armv8a_gic2.d.icfgr[ridx] & bitmask;
                if (NULL != type) {
                        *type = rval >> bidx;
                }
                soc_dbgf("GIC2",
                         "CPU%d get IRQ(%d) trigger type: ridx:0x%X, bidx:0x%X, "
                         "bitmask:0x%X, rval:0x%X, "
                         "gic2d.icfgr[%d]@0x%lX:0x%X\r\n",
                         xwospl_skd_get_cpuid_lc(), irqn, ridx, bidx, bitmask, rval,
                         ridx,
                         &armv8a_gic2.d.icfgr[ridx],
                         armv8a_gic2.d.icfgr[ridx]);
                rc = XWOK;
        } else {
                rc = -ERANGE;
        }
        return rc;
}

__xwbsp_code
xwer_t armv8a_gic_irq_set_affinity(xwirq_t irqn, xwu8_t cpumask)
{
        xwer_t rc;
        xwu32_t ridx;
        xwu32_t bidx;
        xwu32_t bitmask;

        if (irqn < (xwirq_t)32) {
                rc = -ENOSYS;
        } else if ((xwu32_t)irqn < armv8a_gic2_runtime_data.it_lines_num) {
                ridx = irqn >> (xwu32_t)2; /* ridx = irqn / 4 */
                bidx = ((xwu32_t)irqn & (xwu32_t)0x3) << (xwu32_t)3; /* bidx = (irqn & 3) * 8 */
                bitmask = (xwu32_t)0xFF << bidx;
                armv8a_gic2.d.itargetsr[ridx] &= ~bitmask;
                armv8a_gic2.d.itargetsr[ridx] |= ((cpumask << bidx) & bitmask);
                soc_dbgf("GIC2",
                         "CPU%d set IRQ(%d) target: ridx:0x%X, bidx:0x%X, "
                         "bitmask:0x%X, cpumask:0x%X, "
                         "gic2d.itargetsr[%d]@0x%lX:0x%X\r\n",
                         xwospl_skd_get_cpuid_lc(), irqn, ridx, bidx, bitmask, cpumask,
                         ridx,
                         &armv8a_gic2.d.itargetsr[ridx],
                         armv8a_gic2.d.itargetsr[ridx]);
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
        xwu32_t ridx;
        xwu32_t bidx;
        xwid_t cpuid;
        xwu32_t bitmask;
        xwu8_t cpumask;

        if (irqn < (xwirq_t)32) {
                rc = -ENOSYS;
        } else if ((xwu32_t)irqn < armv8a_gic2_runtime_data.it_lines_num) {
                cpuid = xwospl_skd_get_cpuid_lc();
                cpumask = ((xwu8_t)1 << (xwu8_t)cpuid);
                ridx = irqn >> (xwu32_t)2; /* ridx = irqn / 4 */
                bidx = ((xwu32_t)irqn & (xwu32_t)0x3) << (xwu32_t)3; /* bidx = (irqn & 3) * 8 */
                bitmask = (xwu32_t)0xFF << bidx;
                armv8a_gic2.d.itargetsr[ridx] &= ~bitmask;
                armv8a_gic2.d.itargetsr[ridx] |= ((cpumask << bidx) & bitmask);
                soc_dbgf("GIC2",
                         "CPU%d set IRQ(%d) target: ridx:0x%X, bidx:0x%X, "
                         "bitmask:0x%X, cpumask:0x%X, "
                         "gic2d.itargetsr[%d]@0x%lX:0x%X\r\n",
                         xwospl_skd_get_cpuid_lc(), irqn, ridx, bidx, bitmask, cpumask,
                         ridx,
                         &armv8a_gic2.d.itargetsr[ridx],
                         armv8a_gic2.d.itargetsr[ridx]);
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
        xwu32_t enable_state;
        xwu32_t pending_state;
        xwu32_t active_state;

        if (irqn < (xwirq_t)0) {
                rc = -ENOSYS;
        } else if ((xwu32_t)irqn < armv8a_gic2_runtime_data.it_lines_num) {
                ridx = irqn >> (xwu32_t)5; /* ridx = irqn / 32 */
                bitmask = (xwu32_t)1 << ((xwu32_t)irqn & (xwu32_t)0x1F); /* bitmask = 1 << (irqn % 32) */
                enable_state = (armv8a_gic2.d.isenabler[ridx] & bitmask);
                pending_state = (armv8a_gic2.d.ispendr[ridx] & bitmask);
                active_state = (armv8a_gic2.d.isactiver[ridx] & bitmask);
                soc_dbgf("GIC2",
                         "IRQ(%d) bitmask:0x%X, "
                         "gic2d.isenabler[%d]@0x%lX:0x%X, "
                         "gic2d.ispendr[%d]@0x%lX:0x%X, "
                         "gic2d.isactiver[%d]@0x%lX:0x%X\r\n",
                         irqn, bitmask,
                         ridx, &armv8a_gic2.d.isenabler[ridx], armv8a_gic2.d.isenabler[ridx],
                         ridx, &armv8a_gic2.d.ispendr[ridx], armv8a_gic2.d.ispendr[ridx],
                         ridx, &armv8a_gic2.d.isactiver[ridx], armv8a_gic2.d.isactiver[ridx]);
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
        xwu32_t aidx;
        xwu32_t ridx;
        xwu32_t bitmask;

        if (irqn < (xwirq_t)0) {
        } else if ((xwu32_t)irqn < armv8a_gic2_runtime_data.it_lines_num) {
                aidx = irqn >> (xwu32_t)2; /* aidx = irqn / 4 */
                ridx = irqn >> (xwu32_t)5; /* ridx = irqn / 32 */
                bitmask = (xwu32_t)1 << ((xwu32_t)irqn & (xwu32_t)0x1F); /* bitmask = 1 << (irqn % 32) */
                soc_infof("GIC2",
                          "IRQ(%d) bitmask:0x%X, "
                          "gic2d.isenabler[%d]@0x%lX:0x%X, "
                          "gic2d.ispendr[%d]@0x%lX:0x%X, "
                          "gic2d.isactiver[%d]@0x%lX:0x%X, "
                          "gic2d.itargetsr[%d]@0x%lX:0x%X\r\n",
                          irqn, bitmask,
                          ridx, &armv8a_gic2.d.isenabler[ridx], armv8a_gic2.d.isenabler[ridx],
                          ridx, &armv8a_gic2.d.ispendr[ridx], armv8a_gic2.d.ispendr[ridx],
                          ridx, &armv8a_gic2.d.isactiver[ridx], armv8a_gic2.d.isactiver[ridx],
                          aidx, &armv8a_gic2.d.itargetsr[aidx], armv8a_gic2.d.itargetsr[aidx]);
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
                armv8a_gic2_runtime_data.cpu[cpuid].pivt[irqn] = isr;
                rc = XWOK;
        } else if ((xwu32_t)irqn < armv8a_gic2_runtime_data.it_lines_num) {
                armv8a_gic2_runtime_data.sivt[irqn] = isr;
                rc = XWOK;
        } else {
                rc = -ERANGE;
        }
        return rc;
}
