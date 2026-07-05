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

#include <xwos/ospl/skd.h>
#include <xwcd/soc/arm64/v8a/arch_isa.h>
#include <xwcd/soc/arm64/v8a/arch_regs.h>
#include <xwcd/soc/arm64/v8a/arch_mmu.h>
#include <xwcd/soc/arm64/v8a/a72/bcm2711/soc_mmu.h>

/* #define SOC_DBGF */
#define LOGTAG "MMU"
#include <xwcd/soc/arm64/v8a/a72/bcm2711/soc_debug.h>

#define SOC_TCR_EL2_VALUE (ARMV8A_TCR_EL2_RES1 | \
                           ARMV8A_TCR_EL2_PS_36BIT | \
                           ARMV8A_TCR_EL2_TG0_4K | \
                           ARMV8A_TCR_EL2_SH0_INNER | \
                           ARMV8A_TCR_EL2_ORGN0_WBWA | \
                           ARMV8A_TCR_EL2_IRGN0_WBWA | \
                           ARMV8A_TCR_EL2_T0SZ(29U)) /* 64 - 29 = 35bit addr */

#define SOC_MMU_TD_END_BIT      (34UL) /* Only Map 32GiB */

#define SOC_MMU_L1TD_1G_SHIFT   ARMV8A_MMU_S1TD4K_L1TD_START_BIT
#define SOC_MMU_L1TD_1G_SIZE    (1UL << SOC_MMU_L1TD_1G_SHIFT)
#define SOC_MMU_L1TD_1G_MASK    (SOC_MMU_L1TD_1G_SIZE - 1UL)
#define SOC_MMU_L1TD_1G_NUM     (1UL << (SOC_MMU_TD_END_BIT - SOC_MMU_L1TD_1G_SHIFT + 1UL))
#define SOC_MMU_L1TD_ARR        (1U)

#define SOC_MMU_L2TD_2M_SHIFT   ARMV8A_MMU_S1TD4K_L2TD_START_BIT
#define SOC_MMU_L2TD_2M_SIZE    (1UL << SOC_MMU_L2TD_2M_SHIFT)
#define SOC_MMU_L2TD_2M_MASK    (SOC_MMU_L2TD_2M_SIZE - 1UL)
#define SOC_MMU_L2TD_2M_NUM     (1UL << (SOC_MMU_TD_END_BIT - SOC_MMU_L2TD_2M_SHIFT + 1UL))
#define SOC_MMU_L2TD_ARR        (SOC_MMU_L2TD_2M_NUM / ARMV8A_MMU_S1TD4K_TD_NUM)

#define SOC_MMU_L3TD_4K_SHIFT   ARMV8A_MMU_S1TD4K_L3TD_START_BIT
#define SOC_MMU_L3TD_4K_SIZE    (1UL << SOC_MMU_L3TD_4K_SHIFT)
#define SOC_MMU_L3TD_4K_MASK    (SOC_MMU_L3TD_4K_SIZE - 1UL)
#define SOC_MMU_L3TD_4K_NUM     (1UL << (SOC_MMU_TD_END_BIT - SOC_MMU_L3TD_4K_SHIFT + 1UL))
#define SOC_MMU_L3TD_ARR        (SOC_MMU_L3TD_4K_NUM / ARMV8A_MMU_S1TD4K_TD_NUM)

typedef struct {
        armv8a_mmu_s1td4k_t l1[SOC_MMU_L1TD_ARR][ARMV8A_MMU_S1TD4K_TD_NUM];
        armv8a_mmu_s1td4k_t l2[SOC_MMU_L2TD_ARR][ARMV8A_MMU_S1TD4K_TD_NUM];
        armv8a_mmu_s1td4k_t l3[SOC_MMU_L3TD_ARR][ARMV8A_MMU_S1TD4K_TD_NUM];
} soc_mmu_td_s;

extern xwu8_t ram_mr_origin[]; /* 0GiB */
extern xwu8_t ram_mr_size[]; /* 8GiB */

extern xwu8_t periph_mr_origin[]; /* 16GiB */
extern xwu8_t periph_mr_size[]; /* 16GiB */

extern xwu8_t mmu_mr_origin[];
extern xwu8_t mmu_mr_size[];

#define soc_mmu_td (*(soc_mmu_td_s *)mmu_mr_origin)

void soc_mmu_init_l1td(void)
{
        xwu64_t m;

        /* 初始化L1TD，每个TD覆盖1GiB范围，全部设置为nGnRnE的Device */
        for (m = 0; m < ARMV8A_MMU_S1TD4K_TD_NUM; m++) {
                soc_mmu_td.l1[0][m].block.u64 = 0UL;
                if (m < SOC_MMU_L1TD_1G_NUM) {
                        soc_mmu_td.l1[0][m].block.l1td1g.type = ARMV8A_MMU_TD_TYPE_BLOCK;
                        soc_mmu_td.l1[0][m].block.l1td1g.attridx = ARMV8A_MT_DEVICE_NGNRNE;
                        soc_mmu_td.l1[0][m].block.l1td1g.ap = (ARMV8A_MMU_TD_ATTR_AP2_RW | ARMV8A_MMU_TD_ATTR_AP1_PRIV);
                        soc_mmu_td.l1[0][m].block.l1td1g.sh = ARMV8A_MMU_TD_ATTR_SH_OUTER;
                        soc_mmu_td.l1[0][m].block.l1td1g.af = 1;
                        soc_mmu_td.l1[0][m].block.l1td1g.oa30_47 = m;
                        soc_mmu_td.l1[0][m].block.l1td1g.pxn = 1;
                        soc_mmu_td.l1[0][m].block.l1td1g.uxn = 1;
                }
        }
}

void soc_mmu_init_ram_td(void)
{
        xwu64_t m;
        xwu64_t n;
        xwu64_t k;
        xwu64_t ram_td_origin;
        xwu64_t ram_td_num;
        xwu64_t ram_td_end;

        /* 更改Ram区的L1TD为Table */
        ram_td_origin = (xwu64_t)ram_mr_origin >> SOC_MMU_L1TD_1G_SHIFT;
        ram_td_num = (xwu64_t)ram_mr_size >> SOC_MMU_L1TD_1G_SHIFT;
        ram_td_end = ram_td_origin + ram_td_num;
        for (m = ram_td_origin; m < ram_td_end; m++) {
                soc_mmu_td.l1[0][m].table.u64 = 0UL;
                soc_mmu_td.l1[0][m].table.td.type = ARMV8A_MMU_TD_TYPE_TABLE;
                soc_mmu_td.l1[0][m].table.td.nlta12_47 = ((xwu64_t)soc_mmu_td.l2[m] >> ARMV8A_MMU_S1TD4K_TABLE_SHIFT);
                soc_mmu_td.l1[0][m].table.td.pxn = 0;
                soc_mmu_td.l1[0][m].table.td.uxn = 0;
                soc_mmu_td.l1[0][m].table.td.ap = (ARMV8A_MMU_TD_ATTR_APTAB1_RW | ARMV8A_MMU_TD_ATTR_APTAB0_PRIV);
        }

        /* 初始化L2TD为Table */
        ram_td_origin = (xwu64_t)ram_mr_origin >> SOC_MMU_L2TD_2M_SHIFT;
        ram_td_num = (xwu64_t)ram_mr_size >> SOC_MMU_L2TD_2M_SHIFT;
        ram_td_end = ram_td_origin + ram_td_num;
        for (m = ram_td_origin, n = ram_td_origin / ARMV8A_MMU_S1TD4K_TD_NUM;
             m < ram_td_end;
             m += ARMV8A_MMU_S1TD4K_TD_NUM, n++) {
                for (k = 0; k < ARMV8A_MMU_S1TD4K_TD_NUM; k++) {
                        soc_mmu_td.l2[n][k].table.u64 = 0UL;
                        soc_mmu_td.l2[n][k].table.td.type = ARMV8A_MMU_TD_TYPE_TABLE;
                        soc_mmu_td.l2[n][k].table.td.nlta12_47 = ((xwu64_t)soc_mmu_td.l3[m + k] >> ARMV8A_MMU_S1TD4K_TABLE_SHIFT);
                        soc_mmu_td.l2[n][k].table.td.pxn = 0;
                        soc_mmu_td.l2[n][k].table.td.uxn = 0;
                        soc_mmu_td.l2[n][k].table.td.ap = (ARMV8A_MMU_TD_ATTR_APTAB1_RW | ARMV8A_MMU_TD_ATTR_APTAB0_PRIV);
                }
        }

        /* 初始化L3TD为Page */
        ram_td_origin = (xwu64_t)ram_mr_origin >> SOC_MMU_L3TD_4K_SHIFT;
        ram_td_num = (xwu64_t)ram_mr_size >> SOC_MMU_L3TD_4K_SHIFT;
        ram_td_end = ram_td_origin + ram_td_num;
        for (m = ram_td_origin, n = ram_td_origin / ARMV8A_MMU_S1TD4K_TD_NUM;
             m < ram_td_end;
             m += ARMV8A_MMU_S1TD4K_TD_NUM, n++) {
                for (k = 0; k < ARMV8A_MMU_S1TD4K_TD_NUM; k++) {
                        soc_mmu_td.l3[n][k].page.u64 = 0UL;
                        soc_mmu_td.l3[n][k].page.l3td4k.type = ARMV8A_MMU_TD_TYPE_PAGE;
                        soc_mmu_td.l3[n][k].page.l3td4k.attridx = ARMV8A_MT_MEMORY;
                        soc_mmu_td.l3[n][k].page.l3td4k.ap = (ARMV8A_MMU_TD_ATTR_AP2_RW | ARMV8A_MMU_TD_ATTR_AP1_PRIV);
                        soc_mmu_td.l3[n][k].page.l3td4k.sh = ARMV8A_MMU_TD_ATTR_SH_INNER;
                        soc_mmu_td.l3[n][k].page.l3td4k.af = 1;
                        soc_mmu_td.l3[n][k].page.l3td4k.oa12_47 = m + k;
                }
        }

        /* 修改前4KiB的内存为Write-Through + Shareability
         * 多核启动的Spin-table位于0xC8, 0xE0, 0xE8, 0xF0，
         * 对于还未开启MMU+DCache的CPU，这个地址的内存必须保持一致，
         * 否则会出现从核无法启动的问题。
         */
        soc_mmu_td.l3[0][0].page.l3td4k.attridx = ARMV8A_MT_MEMORY_WT;
        soc_mmu_td.l3[0][0].page.l3td4k.sh = ARMV8A_MMU_TD_ATTR_SH_INNER;
}

void soc_mmu_init_periph_td(void)
{
        xwu64_t m;
        xwu64_t n;
        xwu64_t k;
        xwu64_t periph_td_origin;
        xwu64_t periph_td_num;
        xwu64_t periph_td_end;

#if !defined(SOCCFG_PERI_HIGH) || (0 == SOCCFG_PERI_HIGH)
        /* 当使用低地址模式，外设地址：[0xFC000000:64M]，修改内存属性为nGnRnE Device */
        /* 修改L2TD为Block */
        periph_td_origin = (xwu64_t)0xFC000000 >> SOC_MMU_L2TD_2M_SHIFT;
        periph_td_num = (xwu64_t)0x04000000 >> SOC_MMU_L2TD_2M_SHIFT;
        periph_td_end = periph_td_origin + periph_td_num;
        m = periph_td_origin & ((xwu64_t)(~(ARMV8A_MMU_S1TD4K_TD_NUM - 1UL)));
        for (n = m / ARMV8A_MMU_S1TD4K_TD_NUM;
             m < periph_td_end;
             m += ARMV8A_MMU_S1TD4K_TD_NUM, n++) {
                for (k = periph_td_origin % ARMV8A_MMU_S1TD4K_TD_NUM;
                     k < ARMV8A_MMU_S1TD4K_TD_NUM;
                     k++) {
                        soc_mmu_td.l2[n][k].block.u64 = 0UL;
                        soc_mmu_td.l2[n][k].block.l2td2m.type = ARMV8A_MMU_TD_TYPE_BLOCK;
                        soc_mmu_td.l2[n][k].block.l2td2m.attridx = ARMV8A_MT_DEVICE_NGNRNE;
                        soc_mmu_td.l2[n][k].block.l2td2m.ap = (ARMV8A_MMU_TD_ATTR_AP2_RW | ARMV8A_MMU_TD_ATTR_AP1_PRIV);
                        soc_mmu_td.l2[n][k].block.l2td2m.sh = ARMV8A_MMU_TD_ATTR_SH_OUTER;
                        soc_mmu_td.l2[n][k].block.l2td2m.af = 1;
                        soc_mmu_td.l2[n][k].block.l2td2m.oa21_47 = m + k;
                        soc_mmu_td.l2[n][k].block.l2td2m.pxn = 1;
                        soc_mmu_td.l2[n][k].block.l2td2m.uxn = 1;
                }
        }
#endif
}

void soc_mmu_init(void)
{
        xwu64_t el;
        xwu64_t sctlr;
        xwid_t cpuid;

        cpuid = xwospl_skd_get_cpuid_lc();
        armv8a_invalidate_tlb_all();
        if (0U == cpuid) {
                soc_mmu_init_l1td();
                soc_mmu_init_ram_td();
                soc_mmu_init_periph_td();
        }

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
        default:
                soc_errf(LOGTAG, "Not support EL%d\n\r", el);
                break;
        }
}
