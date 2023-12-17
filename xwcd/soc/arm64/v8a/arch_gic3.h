/**
 * @file
 * @brief 架构描述层：ARMv8A Generic Interrupt Controller Version 3
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

#ifndef __xwcd_soc_arm_v8a_arch_gic3_h__
#define __xwcd_soc_arm_v8a_arch_gic3_h__

#include <xwos/standard.h>
#include <soc.h>

#define ARMV8A_IRQn_SGI(n)      (n) /**< Software Generated Interrupt, 0 <= n < 16 */
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

struct armv8a_gic3_d_regs {
        union {
                xwu32_t u32;
                struct {
                        xwu32_t enable_grp0:1;
                        xwu32_t enable_grp1ns:1;
                        xwu32_t enable_grp1s:1;
                        xwu32_t res0_0:1;
                        xwu32_t are_s:1;
                        xwu32_t are_ns:1;
                        xwu32_t ds:1;
                        xwu32_t e1nwf:1;
                        xwu32_t res1_0:23;
                        xwu32_t rwp:1;
                } s;
                struct {
                        xwu32_t enable_grp1:1;
                        xwu32_t enable_grp1a:1;
                        xwu32_t res0_0:2;
                        xwu32_t are_ns:1;
                        xwu32_t res0_1:26;
                        xwu32_t rwp:1;
                } ns;
        } ctrl; /**< 0x0, Distributor Control Register */
        union {
                xwu32_t u32;
                struct {
                        xwu32_t it_line_num:5;
                        xwu32_t cpu_number:3;
                        xwu32_t espi:1;
                        xwu32_t nmi:1;
                        xwu32_t security_extn:1;
                        xwu32_t num_lpis:5;
                        xwu32_t mbis:1;
                        xwu32_t lpis:1;
                        xwu32_t dvis:1;
                        xwu32_t id_bits:5;
                        xwu32_t a3v:1;
                        xwu32_t no_1n:1;
                        xwu32_t reserved3:6;
                } bit;
        } typer; /**< 0x4, Interrupt Controller Type Register */
        union {
                xwu32_t u32;
                struct {
                        xwu32_t implementer:12;
                        xwu32_t revision:4;
                        xwu32_t variant:4;
                        xwu32_t reserved0:4;
                        xwu32_t productID:8;
                } bit;
        } iidr; /**< 0x8, Distributor Implementer Identification Register */
        xwu32_t typer2; /**< 0xC, Interrupt controller Type Register 2 */
        xwu32_t statusr; /**< 0x10, Error Reporting Status Register */
        xwu32_t reserved0[3]; /**< 0x14:0x1C */
        xwu32_t fctlr; /**< 0x20, Function Control Register */
        xwu32_t sac; /**< 0x24, Secure Access Control register */
        xwu32_t reserved1[6]; /**< 0x28:0x3C */
        xwu32_t setspi_nsr; /**< 0x40, Set SPI Register */
        xwu32_t reserved2; /**< 0x44 */
        xwu32_t clrspi_nsr; /**< 0x48, Clear SPI Register */
        xwu32_t reserved3; /**< 0x4C */
        xwu32_t setspi_sr; /**< 0x50, Set SPI Secure Register */
        xwu32_t reserved4; /**< 0x54 */
        xwu32_t clrspi_sr; /**< 0x58, Clear SPI Secure Register */
        xwu32_t reserved5[9]; /**< 0x5C:0x7C */
        xwu32_t igroupr[32]; /**< 0x80:0xFC, Interrupt Group Registers */
        xwu32_t isenabler[32]; /**< 0x100:0x17C, Interrupt Set-Enable Registers */
        xwu32_t icenabler[32]; /**< 0x180:0x1FC, Interrupt Clear-Enable Registers */
        xwu32_t ispendr[32]; /**< 0x200:0x27C, Interrupt Set-Pending Registers */
        xwu32_t icpendr[32]; /**< 0x280:0x2FC, Interrupt Clear-Pending Registers */
        xwu32_t isactiver[32]; /**< 0x300:0x37C, Interrupt Set-Active Registers */
        xwu32_t icactiver[32]; /**< 0x380:0x3FC, Interrupt Clear-Active Registers */
        xwu32_t ipriorityr[256]; /**< 0x400:0x7FC, Interrupt Priority Registers */
        xwu32_t itargetsr[256]; /**< 0x800:0xBFC, Interrupt Processor Targets Registers */
        xwu32_t icfgr[64]; /**< 0xC00:0xCFC, Interrupt Configuration Registers */
        xwu32_t igrpmodr[32]; /**< 0xD00:0xD7C, Interrupt Group Modifier Registers */
        xwu32_t reserved6[32]; /**< 0xD80:0xDFC */
        xwu32_t nsacr[64]; /**< 0xE00:0xEFC, Non-secure Access Control Registers */
        xwu32_t sgir; /**< 0xF00 Software Generated Interrupt Register */
        xwu32_t reserved7[3]; /**< 0xF04:0xF0C */
        xwu32_t cpendsgir[4]; /**< 0xF10:0xF1C, SGI Clear-Pending Registers */
        xwu32_t spendsgir[4]; /**< 0xF20:0xF2C, SGI Set-Pending Registers */
        xwu32_t reserved8[20]; /**< 0xF30:0xF7C */
        xwu32_t inmir[32]; /**< 0xF80:0xFFC, Non-maskable Interrupt Registers */
        xwu32_t igroupre[32]; /**< 0x1000:0x107C, Interrupt Group Registers for extended SPI range */
        xwu32_t reserved9[96]; /**< 0x1080:0x11FC */
        xwu32_t isenablere[32]; /**< 0x1200:0x127C, Interrupt Set-Enable for extended SPI range */
        xwu32_t reserved10[96]; /**< 0x1280:0x13FC */
        xwu32_t icenablere[32]; /**< 0x1400:0x147C, Interrupt Clear-Enable for extended SPI range */
        xwu32_t reserved11[96]; /**< 0x1480:0x15FC */
        xwu32_t ispendre[32]; /**< 0x1600:0x167C, Interrupt Set-Pending Registers for extended SPI range */
        xwu32_t reserved12[96]; /**< 0x1680:0x17FC */
        xwu32_t icpendre[32]; /**< 0x1800:0x187C, Interrupt Clear-Pending Registers for extended SPI range */
        xwu32_t reserved13[96]; /**< 0x1880:0x19FC */
        xwu32_t isactivere[32]; /**< 0x1A00:0x1A7C, Interrupt Set-Active Registers for extended SPI range */
        xwu32_t reserved14[96]; /**< 0x1A80:0x1BFC */
        xwu32_t icactivere[32]; /**< 0x1C00:0x1C7C, Interrupt Clear-Active Registers for extended SPI range */
        xwu32_t reserved15[224]; /**< 0x1C80:0x1FFC */
        xwu32_t iprioritye[256]; /**< 0x2000:0x23FC, Interrupt Clear-Active Registers for extended SPI range */
        xwu32_t reserved16[768]; /**< 0x2400:0x2FFC */
        xwu32_t icfgre[64]; /**< 0x3000:0x30FC, Extended SPI Configuration Register */
        xwu32_t reserved17[192]; /**< 0x3100:0x33FC */
        xwu32_t igrpmodre[32]; /**< 0x3400:0x347C, Interrupt Group Modifier for extended SPI range */
        xwu32_t reserved18[96]; /**< 0x3480:0x35FC */
        xwu32_t nsacre[32]; /**< 0x3600:0x367C, Non-secure Access Control Registers for extended SPI range */
        xwu32_t reserved19[288]; /**< 0x3680:0x3AFC */
        xwu32_t inmire[32]; /**< 0x3B00:0x3B7C, Non-maskable Interrupt Registers for extended SPI range */
        xwu32_t reserved20[2400]; /**< 0x3B80:0x60FC */
        xwu64_t irouter[988]; /**< 0x6100:0x7FD8, Interrupt Routing Registers */
        xwu32_t reserved21[8]; /**< 0x7FE0:0x7FFC */
        xwu64_t iroutere[1024]; /**< 0x8000:0x9FFC, Interrupt Routing Registers for extended SPI range */
        xwu32_t reserved22[2048]; /**< 0xA000:0xBFFC */
        xwu32_t chipsr; /**< 0xC000, Chip Status Register */
        xwu32_t dchipr; /**< 0xC004, Default Chip Register */
        xwu64_t chipr[16]; /**< 0xC008:0xC080, Chip Registers */
        xwu32_t reserved23[2016]; /**< 0xC088:0xE004 */
        xwu32_t iclar[62]; /**< 0xE008:0xE0FC, Interrupt Class Registers */
        xwu32_t reserved24[2]; /**< 0xE100:0xE104 */
        xwu32_t ierrr[30]; /**< 0xE108:0xE17C, Interrupt Error Registers */
        xwu32_t reserved25[928]; /**< 0xE180:0xEFFC */
        xwu64_t cfgid; /**<  0xF000, Configuration ID Register */
        xwu32_t reserved26[1010]; /**< 0xF008:0xFFCC */
        xwu32_t idr[12]; /**< 0xFFD0:0xFFFC, Reserved for ID registers */
};
#define armv8a_gic3_d \
        (*((volatile struct armv8a_gic3_d_regs *)(SOC_GIC3_REGBASE + SOC_GIC3_D_OFFSET)))

struct armv8a_gic3_a_regs {
        xwu32_t reserved0[16]; /**< 0x0:0x3C */
        xwu32_t setspi_nsr; /**< 0x40, Aliased Non-secure SPI Set Register */
        xwu32_t reserved1; /**< 0x44 */
        xwu32_t clrspi_nsr; /**< 0x48, Aliased Non-secure SPI Clear Register */
        xwu32_t reserved2; /**< 0x4C */
        xwu32_t setspi_sr; /**< 0x50, Aliased Secure SPI Set Register */
        xwu32_t reserved3; /**< 0x54 */
        xwu32_t clrspi_sr; /**< 0x58, Aliased secure SPI Clear Register */
};
#define armv8a_gic3_a \
        (*((volatile struct armv8a_gic3_a_regs *)(SOC_GIC3_REGBASE + SOC_GIC3_A_OFFSET)))

struct armv8a_gic3_r_lpi_regs  {
        xwu32_t ctrl; /**< 0x0, Redistributor Control Register */
        xwu32_t iidr; /**< 0x4, Redistributor Implementer Identification Register */
        xwu64_t typer; /**< 0x8, Interrupt Controller Type Register */
        xwu32_t statusr; /**< 0x10, Error Reporting Status Register, optional */
        xwu32_t waker; /**< 0x14, Power Management Control Register */
        xwu32_t mpamidr; /**< 0x18,  Report maximum PARTID and PMG Register*/
        xwu32_t partidr; /**< 0x1C, Set PARTID and PMG Register*/
        xwu32_t fctlr; /**< 0x20, Function Control Register */
        xwu32_t pwrr; /**< 0x24, Power Register */
        xwu32_t class; /**< 0x28, [Secure-only] Class Register */
        xwu32_t reserved0[5]; /**< 0x2C:0x3C */
        xwu64_t setlpir; /**< 0x40, Set LPI Pending Register */
        xwu64_t clrlpir; /**< 0x48, Clear LPI Pending Register */
        xwu32_t reserved1[8]; /**< 0x50:0x6C */
        xwu64_t propbaser; /**< 0x70, Redistributor Properties Base Address Register */
        xwu64_t pendbaser; /**< 0x78, Redistributor LPI Pending Table Base Address Register */
        xwu32_t reserved2[8]; /**< 0x84:0x9C */
        xwu64_t invlpir; /**< 0xA0, Redistributor Invalidate LPI Register */
        xwu32_t reserved3[2]; /**< 0xA8:0xAC */
        xwu64_t invallr; /**< 0xB0, Redistributor Invalidate All Register */
        xwu32_t reserved4[2]; /**< 0xB8:0xBC */
        xwu32_t syncr; /**< 0xC0, Redistributor Synchronize Register */
        xwu32_t reserved5[16323]; /**< 0xC4:0xFFCC */
        xwu32_t idr[12]; /**< 0xFFD0:0xFFFC, Reserved for ID registers */
};
#define armv8a_gic3_r_lpi(n) \
        (*((volatile struct armv8a_gic3_r_lpi_regs *)(SOC_GIC3_REGBASE + SOC_GIC3_R_LPI_OFFSET(n))))

#define ARMV8A_GIC3_R_LPI(n) (SOC_GIC3_REGBASE + SOC_GIC3_R_LPI_OFFSET(n))

struct armv8a_gic3_r_sgi_regs {
        xwu32_t reserved0[32]; /**< 0x00:0x7C */
        xwu32_t igroupr0; /**< 0x80, Interrupt Group Register */
        xwu32_t reserved1[31]; /**< 0x84:0xFC */
        xwu32_t isenabler0; /**< 0x100, Interrupt Set-Enable Register */
        xwu32_t reserved2[31]; /**< 0x104:0x17C */
        xwu32_t icenabler0; /**< 0x180, Interrupt Clear-Enable Register */
        xwu32_t reserved3[31]; /**< 0x184:0x1FC */
        xwu32_t ispendr0; /**< 0x200, Interrupt Set-Pending Register */
        xwu32_t reserved4[31]; /**< 0x204:0x27C */
        xwu32_t icpendr0; /**< 0x280, Interrupt Clear-Pending Register */
        xwu32_t reserved5[31]; /**< 0x284:0x2FC */
        xwu32_t isactiver0; /**< 0x300, Interrupt Set-Active Register */
        xwu32_t reserved6[31]; /**< 0x304:0x37C */
        xwu32_t icactiver0; /**< 0x380, Interrupt Clear-Active Register */
        xwu32_t reserved7[31]; /**< 0x384:0x3FC */
        xwu32_t ipriorityr[8]; /**< 0x400:0x41C, Interrupt Priority Registers */
        xwu32_t reserved8[504]; /**< 0x420:0xBFC */
        xwu32_t icfgr[2]; /**< 0xC00:0xC04, Interrupt Configuration Registers */
        xwu32_t reserved9[62]; /**< 0xC08:0xCFC */
        xwu32_t igrpmodr0; /**< 0xD00, Interrupt Group Modifier Register */
        xwu32_t reserved10[63]; /**< 0xD04:0xDFC */
        xwu32_t nsacr; /**< 0xE00, Non-secure Access Control Register */
        xwu32_t reserved11[11391]; /**< 0xE04:0xBFFC */
        xwu32_t miscstatusr; /**< 0xC000, Miscellaneous Status Register */
        xwu32_t reserved12; /**< 0xC004 */
        xwu32_t ierrvr; /**< 0xC008, Interrupt Error Valid Register */
        xwu32_t reserved13; /**< 0xC00C */
        xwu64_t sgidr; /**< 0xC010, SGI Default Register */
        xwu32_t reserved14[3066]; /**< 0xC018:0xEFFC */
        xwu32_t cfgid0; /**< 0xF000, Configuration ID0 Register */
        xwu32_t cfgid1; /**< 0xF004, Configuration ID1 Register */
};
#define armv8a_gic3_r_sgi(n) \
        (*((volatile struct armv8a_gic3_r_sgi_regs *)(SOC_GIC3_REGBASE + SOC_GIC3_R_SGI_OFFSET(n))))

#define ARMV8A_GIC3_R_SGI(cpuid) (SOC_GIC3_REGBASE + SOC_GIC3_R_SGI_OFFSET(cpuid))

void armv8a_gic3_init_runtime(void);
void armv8a_gic3_init_distributor(void);
void armv8a_gic3_enable_distributor(void);
void armv8a_gic3_disable_distributor(void);
void armv8a_gic3_init_cpuif(void);

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
xwer_t armv8a_gic_irq_set_affinity_lc(xwirq_t irqn);
xwer_t armv8a_gic_irq_get_state(xwirq_t irqn,
                                bool * enable, bool * pending, bool * active);

void armv8a_gic_irq_dump(xwirq_t irqn);

xwer_t armv8a_gic_irq_set_isr(xwirq_t irqn, xwisr_f isr);

#endif /* xwcd/soc/arm64/v8a/arch_gic3.h */
