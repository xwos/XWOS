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

#ifndef __xwcd_soc_arm_v8a_arch_gic2_h__
#define __xwcd_soc_arm_v8a_arch_gic2_h__

#include <xwos/standard.h>
#include <soc.h>

#define ARMV8A_IRQn_SGI(n)      (n) /**< Software Generated Interrupt, 0 <= n <= 15 */
#define ARMV8A_IRQn_CNTPS       29
#define ARMV8A_IRQn_CNTV        27
#define ARMV8A_IRQn_CNTHP       26
#define ARMV8A_IRQn_CNTP        30

#define ARMV8A_SGI_NUM          16U
#define ARMV8A_PPI_NUM          16U
#define ARMV8A_SPI_OFFSET       (ARMV8A_SGI_NUM + ARMV8A_PPI_NUM)

enum armv8a_irq_trigger_type_em {
        ARMV8A_IRQ_TRIGGER_TYPE_LEVEL = 0x0U,
        ARMV8A_IRQ_TRIGGER_TYPE_EDGE = 0x2U,
};

struct armv8a_gic2_distributor {
        union {
                xwu32_t u32;
                struct {
                        xwu32_t enable:1;
                } ns; /**< Non-secure Access */
                struct {
                        xwu32_t enable_grp0:1;
                        xwu32_t enable_grp1:1;
                } s; /**< Secure Access */
        } ctrl; /**< 0x0, Distributor Control Register */
        xwu32_t typer; /**< 0x4, Interrupt Controller Type Register */
        xwu32_t iidr; /**< 0x8, Distributor Implementer Identification Register */
        xwu32_t reserved0[29]; /**< 0xC:0x7C */
        xwu32_t igroupr[16]; /**< 0x80:0xBC, Interrupt Group Registers */
        xwu32_t reserved1[16]; /**< 0xC0:0xFC */
        xwu32_t isenabler[16]; /**< 0x100:0x13C, Interrupt Set-Enable Registers */
        xwu32_t reserved2[16]; /**< 0x140:0x17C */
        xwu32_t icenabler[16]; /**< 0x180:0x1BC, Interrupt Clear-Enable Registers */
        xwu32_t reserved3[16]; /**< 0x1C0:0x1FC */
        xwu32_t ispendr[16]; /**< 0x200:0x23C, Interrupt Set-Pending Registers */
        xwu32_t reserved4[16]; /**< 0x240:0x27C */
        xwu32_t icpendr[16]; /**< 0x280:0x2BC, Interrupt Clear-Pending Registers */
        xwu32_t reserved5[16]; /**< 0x2C0:0x2FC */
        xwu32_t isactiver[16]; /**< 0x300:0x33C, Interrupt Set-Active Registers */
        xwu32_t reserved6[16]; /**< 0x340:0x37C */
        xwu32_t icactiver[16]; /**< 0x380:0x3BC, Interrupt Clear-Active Registers */
        xwu32_t reserved7[16]; /**< 0x3C0:0x3FC */
        xwu32_t ipriorityr[128]; /**< 0x400:0x5FC, Interrupt Priority Registers */
        xwu32_t reserved8[128]; /**< 0x600:0x7FC */
        xwu32_t itargetsr[128]; /**< 0x800:0x9FC, Interrupt Processor Targets Registers */
        xwu32_t reserved9[128]; /**< 0xA00:0xBFC */
        xwu32_t icfgr[32]; /**< 0xC00:0xC7C, Interrupt Configuration Registers */
        xwu32_t reserved10[32]; /**< 0xC80:0xCFC */
        xwu32_t ppisr; /**< 0xD00, Private Peripheral Interrupt Status Register */
        xwu32_t spisr[15]; /**< 0xD04:0xD3C, Shared Peripheral Interrupt Status Registers */
        xwu32_t reserved11[112]; /**< 0xD40:0xEFC */
        xwu32_t sgir; /**< 0xF00, Software Generated Interrupt Register */
        xwu32_t reserved12[3]; /**< 0xF04:0xF0C */
        xwu32_t cpendsgir[4]; /**< 0xF10:0xF1C, SGI Clear-Pending Registers */
        xwu32_t spendsgir[4]; /**< 0xF20:0xF2C, SGI Set-Pending Registers */
        xwu32_t reserved13[40]; /**< 0xF30:0xFCC */
        xwu32_t pidr4; /**< 0xFD0, Peripheral ID 4 Register */
        xwu32_t pidr5; /**< 0xFD4, Peripheral ID 5 Register */
        xwu32_t pidr6; /**< 0xFD8, Peripheral ID 6 Register */
        xwu32_t pidr7; /**< 0xFDC, Peripheral ID 7 Register */
        xwu32_t pidr0; /**< 0xFE0, Peripheral ID 0 Register */
        xwu32_t pidr1; /**< 0xFE4, Peripheral ID 1 Register */
        xwu32_t pidr2; /**< 0xFE8, Peripheral ID 2 Register */
        xwu32_t pidr3; /**< 0xFEC, Peripheral ID 3 Register */
        xwu32_t cidr0; /**< 0xFF0, Component ID 0 Register */
        xwu32_t cidr1; /**< 0xFF4, Component ID 1 Register */
        xwu32_t cidr2; /**< 0xFF8, Component ID 2 Register */
        xwu32_t cidr3; /**< 0xFFC, Component ID 3 Register */
};

struct armv8a_gic2_cpu_intferface {
        union {
                xwu32_t u32;
                struct {
                        xwu32_t enable:1;
                } v1ns; /**< V1 Non-secure Access */
                struct {
                        xwu32_t enable_grp1:1;
                        xwu32_t res0_0:4;
                        xwu32_t fiq_byp_dis_grp1:1;
                        xwu32_t irq_byp_dis_grp1:1;
                        xwu32_t res0_1:2;
                        xwu32_t eoi_mode_ns:1;
                } ns; /**< Non-secure Access */
                struct {
                        xwu32_t enable_grp0:1;
                        xwu32_t enable_grp1:1;
                        xwu32_t ack_ctl:1;
                        xwu32_t fiq_en:1;
                        xwu32_t cbpr:1;
                        xwu32_t fiq_byp_dis_grp0:1;
                        xwu32_t irq_byp_dis_grp0:1;
                        xwu32_t fiq_byp_dis_grp1:1;
                        xwu32_t irq_byp_dis_grp1:1;
                        xwu32_t eoi_mode_s:1;
                        xwu32_t eoi_mode_ns:1;
                } s; /**< Secure Access */
        } ctrl; /**< 0x0, CPU Interface Control Register */
        xwu32_t pmr; /**< 0x4, Interrupt Priority Mask Register */
        xwu32_t bpr; /**< 0x8, Binary Point Register */
        xwu32_t iar; /**< 0xC, Interrupt Acknowledge Register */
        xwu32_t eoir; /**< 0x10, End of Interrupt Register */
        xwu32_t rpr; /**< 0x14, Running Priority Register */
        xwu32_t hppir; /**< 0x18, Highest Priority Pending Interrupt Register */
        xwu32_t abpr; /**< 0x1C, Aliased Binary Point Register */
        xwu32_t aiar; /**< 0x20, Aliased Interrupt Acknowledge Registerr */
        xwu32_t aeoir; /**< 0x24, Aliased End of Interrupt Register */
        xwu32_t ahppir; /**< 0x28, Aliased Highest Priority Pending Interrupt Register */
        xwu32_t reserved0[41]; /**< 0x2C:0xCC */
        xwu32_t apr0; /**< 0xD0, Active Priority Register */
        xwu32_t reserved1[3]; /**< 0xD4:0xDC */
        xwu32_t nsapr0; /**< 0xE0, Non-Secure Active Priority Register */
        xwu32_t reserved2[6]; /**< 0xE4:0xF8 */
        xwu32_t iidr; /**< 0xFC, CPU Interface Identification Register */
        xwu32_t reserved3[960]; /**< 0x100:0xFFC */
        xwu32_t dir; /**< 0x1000, Deactivate Interrupt Register */
};

struct armv8a_gic2 {
        xwu8_t offset[SOC_GIC2_D_OFFSET];
        __xwcc_aligned(SOC_GIC2_D_OFFSET) struct armv8a_gic2_distributor d;
        __xwcc_aligned(SOC_GIC2_C_OFFSET) struct armv8a_gic2_cpu_intferface c;
};

#define armv8a_gic2 (*((volatile struct armv8a_gic2 *)SOC_GIC2_REGBASE))

void armv8a_gic2_init_runtime(void);
void armv8a_gic2_init_distributor(void);
void armv8a_gic2_init_cpuif(void);

void armv8a_gic_isr(xwid_t cpuid);

xwu8_t armv8a_gic_get_max_priority(void);
xwu8_t armv8a_gic_get_min_priority(void);
xwer_t armv8a_gic_irq_get_id(xwirq_t * irqnbuf);
xwer_t armv8a_gic_irq_enable(xwirq_t irqn);
xwer_t armv8a_gic_irq_disable(xwirq_t irqn);
xwer_t armv8a_gic_irq_save(xwirq_t irqn, xwreg_t * flag);
xwer_t armv8a_gic_irq_restore(xwirq_t irqn, xwreg_t flag);
xwer_t armv8a_gic_irq_set_priority(xwirq_t irqn, xwu8_t pr);
xwer_t armv8a_gic_irq_get_priority(xwirq_t irqn, xwu8_t * pr);
xwu8_t armv8a_gic_raise_priority(xwu8_t pr, xwu8_t delta);
xwu8_t armv8a_gic_drop_priority(xwu8_t pr, xwu8_t delta);
xwer_t armv8a_gic_irq_set_trigger_type(xwirq_t irqn, xwu32_t type);
xwer_t armv8a_gic_irq_get_trigger_type(xwirq_t irqn, xwu32_t * type);
xwer_t armv8a_gic_irq_set_affinity(xwirq_t irqn, xwu8_t cpumask);
xwer_t armv8a_gic_irq_set_affinity_lc(xwirq_t irqn);
xwer_t armv8a_gic_irq_get_state(xwirq_t irqn,
                                bool * enable, bool * pending, bool * active);
void armv8a_gic_irq_dump(xwirq_t irqn);
xwer_t armv8a_gic_irq_set_isr(xwirq_t irqn, xwisr_f isr);

#endif /* xwcd/soc/arm64/v8a/arch_gic2.h */
