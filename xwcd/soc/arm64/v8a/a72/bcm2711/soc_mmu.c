/**
 * @file
 * @brief SOC描述层：MMU
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

#include <xwcd/soc/arm64/v8a/a72/bcm2711/soc_mmu.h>
#include <xwcd/soc/arm64/v8a/arch_isa.h>
#include <xwcd/soc/arm64/v8a/arch_regs.h>
#include <xwcd/soc/arm64/v8a/arch_mmu.h>

#define LOGTAG "MMU"
#include <xwcd/soc/arm64/v8a/a72/bcm2711/soc_debug.h>

#define SOC_PAGE_SHIFT  16UL
#define SOC_PAGE_SIZE   (1UL << ARMV8A_PAGE_SHIFT)
#define SOC_PAGE_MASK   (~(ARMV8A_PAGE_SIZE - 1UL))

#define SOC_BLOCK_SHIFT 29UL
#define SOC_BLOCK_SIZE  (1UL << SOC_BLOCK_SHIFT)

#define SOC_TCR_EL2_VALUE (ARMV8A_TCR_EL2_RES1 | \
                           ARMV8A_TCR_EL2_PS_36BIT | \
                           ARMV8A_TCR_EL2_TG0_64K | \
                           ARMV8A_TCR_EL2_SH0_INNER | \
                           ARMV8A_TCR_EL2_ORGN0_WBWA | \
                           ARMV8A_TCR_EL2_IRGN0_WBWA | \
                           ARMV8A_TCR_EL2_T0SZ(29U)) /* 64 - 29 = 35bit addr */

#define SOC_TCR_EL1_VALUE (ARMV8A_TCR_EL1_PS_36BIT | \
                           ARMV8A_TCR_EL1_TG0_64K | \
                           ARMV8A_TCR_EL1_SH0_INNER | \
                           ARMV8A_TCR_EL1_ORGN0_WBWA | \
                           ARMV8A_TCR_EL1_IRGN0_WBWA | \
                           ARMV8A_TCR_EL1_T0SZ(29U)) /* 64 - 29 = 35bit addr */


#define SOC_MMU_TD_END_BIT      (35UL)
#define SOC_MMU_L2TD_NUM        (1UL << (SOC_MMU_TD_END_BIT - ARMV8A_MMU_S1TD64K_L2TD_START_BIT + 1UL))

__xwcc_aligned(65536)
struct {
        armv8a_mmu_s1td64k_t l2[ARMV8A_MMU_S1TD64K_TD_NUM];
        armv8a_mmu_s1td64k_t l3[32][ARMV8A_MMU_S1TD64K_TD_NUM];
} soc_mmu_td = { 0UL };

extern xwu8_t ram_mr_origin[];
extern xwu8_t ram_mr_size[];

extern xwu8_t peri_mr_origin[];
extern xwu8_t peri_mr_size[];

extern xwu8_t pcie_mr_origin[];
extern xwu8_t pcie_mr_size[];

static
void soc_mmu_init_ram_td(void)
{
        xwu64_t i;
        xwu64_t start = ((xwu64_t)ram_mr_origin >> SOC_BLOCK_SHIFT);
        xwu64_t end = start + ((xwu64_t)ram_mr_size >> SOC_BLOCK_SHIFT);

        for (i = start; i < end; i++) {
                soc_mmu_td.l2[i].block.u64 = 0UL;
                soc_mmu_td.l2[i].block.sz512m.type = ARMV8A_MMU_TD_TYPE_BLOCK;
                soc_mmu_td.l2[i].block.sz512m.attridx = ARMV8A_MT_MEMORY;
                soc_mmu_td.l2[i].block.sz512m.ap = (ARMV8A_MMU_TD_ATTR_AP2_RW |
                                                    ARMV8A_MMU_TD_ATTR_AP1_PRIV);
                soc_mmu_td.l2[i].block.sz512m.sh = ARMV8A_MMU_TD_ATTR_SH_INNER;
                soc_mmu_td.l2[i].block.sz512m.af = 1;
                soc_mmu_td.l2[i].block.sz512m.oa29_47 = i;
        }
}

static
void soc_mmu_init_peri_td(void)
{
        xwu64_t i;
        xwu64_t start = ((xwu64_t)peri_mr_origin >> SOC_BLOCK_SHIFT);
        xwu64_t end = start + ((xwu64_t)peri_mr_size >> SOC_BLOCK_SHIFT);

        for (i = start; i < end; i++) {
                soc_mmu_td.l2[i].block.u64 = 0UL;
                soc_mmu_td.l2[i].block.sz512m.type = ARMV8A_MMU_TD_TYPE_BLOCK;
                soc_mmu_td.l2[i].block.sz512m.attridx = ARMV8A_MT_DEVICE_NGNRNE;
                soc_mmu_td.l2[i].block.sz512m.ap = (ARMV8A_MMU_TD_ATTR_AP2_RW |
                                                    ARMV8A_MMU_TD_ATTR_AP1_PRIV);
                soc_mmu_td.l2[i].block.sz512m.sh = ARMV8A_MMU_TD_ATTR_SH_OUTER;
                soc_mmu_td.l2[i].block.sz512m.af = 1;
                soc_mmu_td.l2[i].block.sz512m.oa29_47 = i;
                soc_mmu_td.l2[i].block.sz512m.pxn = 1;
                soc_mmu_td.l2[i].block.sz512m.uxn = 1;
        }
}

void soc_mmu_init(void)
{
        xwu64_t el;
        xwu64_t sctlr;

        armv8a_invalidate_tlb_all();
        soc_mmu_init_ram_td();
        soc_mmu_init_peri_td();

        armv8a_sysreg_read(&el, CurrentEL);
        el >>= 2UL;
        switch (el) {
        case 2UL:
                armv8a_sysreg_write(ttbr0_el2, &soc_mmu_td);
                armv8a_sysreg_write(tcr_el2, SOC_TCR_EL2_VALUE);
                armv8a_sysreg_write(mair_el2, ARMV8A_MEMORY_ATTRIBUTES);
                armv8a_dsb(sy);
                armv8a_isb();
                armv8a_sysreg_read(&sctlr, sctlr_el2);
                sctlr |= ARMV8A_SCTLR_M;
                armv8a_sysreg_write(sctlr_el2, sctlr);
                break;
        case 1UL:
                armv8a_sysreg_write(ttbr0_el1, &soc_mmu_td);
                armv8a_sysreg_write(tcr_el1, SOC_TCR_EL1_VALUE);
                armv8a_sysreg_write(mair_el1, ARMV8A_MEMORY_ATTRIBUTES);
                armv8a_dsb(sy);
                armv8a_isb();
                armv8a_sysreg_read(&sctlr, sctlr_el1);
                sctlr |= ARMV8A_SCTLR_M;
                armv8a_sysreg_write(sctlr_el1, sctlr);
                break;
        default:
                soc_errf(LOGTAG, "Not support EL%d\n\r", el);
                break;
        }
}
