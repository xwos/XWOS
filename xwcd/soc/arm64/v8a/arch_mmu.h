/**
 * @file
 * @brief 架构描述层：ARMv8A MMU
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

#ifndef __xwcd_soc_arm_v8a_arch_mmu_h__
#define __xwcd_soc_arm_v8a_arch_mmu_h__

#include <xwos/standard.h>

/* TCR_EL3 bit field */
#define ARMV8A_TCR_EL3_T0SZ_SHIFT       (0UL)
#define ARMV8A_TCR_EL3_T0SZ_MASK        (0x3FUL)
#define ARMV8A_TCR_EL3_T0SZ(n)          ((n) & ARMV8A_TCR_EL3_T0SZ_MASK)

#define ARMV8A_TCR_EL3_IRGN0_SHIFT      (8UL)
#define ARMV8A_TCR_EL3_IRGN0_MASK       (3UL << ARMV8A_TCR_EL3_IRGN0_SHIFT)
#define ARMV8A_TCR_EL3_IRGN0_NC         (0UL << ARMV8A_TCR_EL3_IRGN0_SHIFT)
#define ARMV8A_TCR_EL3_IRGN0_WBWA       (1UL << ARMV8A_TCR_EL3_IRGN0_SHIFT)
#define ARMV8A_TCR_EL3_IRGN0_WT         (2UL << ARMV8A_TCR_EL3_IRGN0_SHIFT)
#define ARMV8A_TCR_EL3_IRGN0_WBNWA      (3UL << ARMV8A_TCR_EL3_IRGN0_SHIFT)

#define ARMV8A_TCR_EL3_ORGN0_SHIFT      (10UL)
#define ARMV8A_TCR_EL3_ORGN0_MASK       (3UL << ARMV8A_TCR_EL3_ORGN0_SHIFT)
#define ARMV8A_TCR_EL3_ORGN0_NC         (0UL << ARMV8A_TCR_EL3_ORGN0_SHIFT)
#define ARMV8A_TCR_EL3_ORGN0_WBWA       (1UL << ARMV8A_TCR_EL3_ORGN0_SHIFT)
#define ARMV8A_TCR_EL3_ORGN0_WT         (2UL << ARMV8A_TCR_EL3_ORGN0_SHIFT)
#define ARMV8A_TCR_EL3_ORGN0_WBNWA      (3UL << ARMV8A_TCR_EL3_ORGN0_SHIFT)
#define ARMV8A_TCR_EL3_ORGN0_MASK       (3UL << ARMV8A_TCR_EL3_ORGN0_SHIFT)

#define ARMV8A_TCR_EL3_SH0_SHIFT        (12UL)
#define ARMV8A_TCR_EL3_SH0_MASK         (3UL << ARMV8A_TCR_EL3_SH0_SHIFT)
#define ARMV8A_TCR_EL3_SH0_NON          (0UL << ARMV8A_TCR_EL3_SH0_SHIFT)
#define ARMV8A_TCR_EL3_SH0_OUTER        (2UL << ARMV8A_TCR_EL3_SH0_SHIFT)
#define ARMV8A_TCR_EL3_SH0_INNER        (3UL << ARMV8A_TCR_EL3_SH0_SHIFT)

#define ARMV8A_TCR_EL3_TG0_SHIFT        (14UL)
#define ARMV8A_TCR_EL3_TG0_MASK         (3UL << ARMV8A_TCR_EL3_TG0_SHIFT)
#define ARMV8A_TCR_EL3_TG0_4K           (0UL << ARMV8A_TCR_EL3_TG0_SHIFT)
#define ARMV8A_TCR_EL3_TG0_64K          (1UL << ARMV8A_TCR_EL3_TG0_SHIFT)
#define ARMV8A_TCR_EL3_TG0_16K          (2UL << ARMV8A_TCR_EL3_TG0_SHIFT)

#define ARMV8A_TCR_EL3_PS_SHIFT         (16UL)
#define ARMV8A_TCR_EL3_PS_MASK          (7UL << ARMV8A_TCR_EL3_PS_SHIFT)
#define ARMV8A_TCR_EL3_PS_32BIT         (0UL << ARMV8A_TCR_EL3_PS_SHIFT)
#define ARMV8A_TCR_EL3_PS_36BIT         (1UL << ARMV8A_TCR_EL3_PS_SHIFT)
#define ARMV8A_TCR_EL3_PS_40BIT         (2UL << ARMV8A_TCR_EL3_PS_SHIFT)
#define ARMV8A_TCR_EL3_PS_42BIT         (3UL << ARMV8A_TCR_EL3_PS_SHIFT)
#define ARMV8A_TCR_EL3_PS_44BIT         (4UL << ARMV8A_TCR_EL3_PS_SHIFT)
#define ARMV8A_TCR_EL3_PS_48BIT         (5UL << ARMV8A_TCR_EL3_PS_SHIFT)

#define ARMV8A_TCR_TBI_SHIFT            (20UL)
#define ARMV8A_TCR_TBI_MASK             (1UL << ARMV8A_TCR_TBI_SHIFT)

#define ARMV8A_TCR_EL3_RES1             (0x80800000UL)

/* TCR_EL2 bit field */
#define ARMV8A_TCR_EL2_T0SZ_SHIFT       (0UL)
#define ARMV8A_TCR_EL2_T0SZ_MASK        (0x3FUL)
#define ARMV8A_TCR_EL2_T0SZ(n)          ((n) & ARMV8A_TCR_EL2_T0SZ_MASK)

#define ARMV8A_TCR_EL2_IRGN0_SHIFT      (8UL)
#define ARMV8A_TCR_EL2_IRGN0_MASK       (3UL << ARMV8A_TCR_EL2_IRGN0_SHIFT)
#define ARMV8A_TCR_EL2_IRGN0_NC         (0UL << ARMV8A_TCR_EL2_IRGN0_SHIFT)
#define ARMV8A_TCR_EL2_IRGN0_WBWA       (1UL << ARMV8A_TCR_EL2_IRGN0_SHIFT)
#define ARMV8A_TCR_EL2_IRGN0_WT         (2UL << ARMV8A_TCR_EL2_IRGN0_SHIFT)
#define ARMV8A_TCR_EL2_IRGN0_WBNWA      (3UL << ARMV8A_TCR_EL2_IRGN0_SHIFT)

#define ARMV8A_TCR_EL2_ORGN0_SHIFT      (10UL)
#define ARMV8A_TCR_EL2_ORGN0_MASK       (3UL << ARMV8A_TCR_EL2_ORGN0_SHIFT)
#define ARMV8A_TCR_EL2_ORGN0_NC         (0UL << ARMV8A_TCR_EL2_ORGN0_SHIFT)
#define ARMV8A_TCR_EL2_ORGN0_WBWA       (1UL << ARMV8A_TCR_EL2_ORGN0_SHIFT)
#define ARMV8A_TCR_EL2_ORGN0_WT         (2UL << ARMV8A_TCR_EL2_ORGN0_SHIFT)
#define ARMV8A_TCR_EL2_ORGN0_WBNWA      (3UL << ARMV8A_TCR_EL2_ORGN0_SHIFT)
#define ARMV8A_TCR_EL2_ORGN0_MASK       (3UL << ARMV8A_TCR_EL2_ORGN0_SHIFT)

#define ARMV8A_TCR_EL2_SH0_SHIFT        (12UL)
#define ARMV8A_TCR_EL2_SH0_MASK         (3UL << ARMV8A_TCR_EL2_SH0_SHIFT)
#define ARMV8A_TCR_EL2_SH0_NON          (0UL << ARMV8A_TCR_EL2_SH0_SHIFT)
#define ARMV8A_TCR_EL2_SH0_OUTER        (2UL << ARMV8A_TCR_EL2_SH0_SHIFT)
#define ARMV8A_TCR_EL2_SH0_INNER        (3UL << ARMV8A_TCR_EL2_SH0_SHIFT)

#define ARMV8A_TCR_EL2_TG0_SHIFT        (14UL)
#define ARMV8A_TCR_EL2_TG0_MASK         (3UL << ARMV8A_TCR_EL2_TG0_SHIFT)
#define ARMV8A_TCR_EL2_TG0_4K           (0UL << ARMV8A_TCR_EL2_TG0_SHIFT)
#define ARMV8A_TCR_EL2_TG0_64K          (1UL << ARMV8A_TCR_EL2_TG0_SHIFT)
#define ARMV8A_TCR_EL2_TG0_16K          (2UL << ARMV8A_TCR_EL2_TG0_SHIFT)

#define ARMV8A_TCR_EL2_PS_SHIFT         (16UL)
#define ARMV8A_TCR_EL2_PS_MASK          (7UL << ARMV8A_TCR_EL2_PS_SHIFT)
#define ARMV8A_TCR_EL2_PS_32BIT         (0UL << ARMV8A_TCR_EL2_PS_SHIFT)
#define ARMV8A_TCR_EL2_PS_36BIT         (1UL << ARMV8A_TCR_EL2_PS_SHIFT)
#define ARMV8A_TCR_EL2_PS_40BIT         (2UL << ARMV8A_TCR_EL2_PS_SHIFT)
#define ARMV8A_TCR_EL2_PS_42BIT         (3UL << ARMV8A_TCR_EL2_PS_SHIFT)
#define ARMV8A_TCR_EL2_PS_44BIT         (4UL << ARMV8A_TCR_EL2_PS_SHIFT)
#define ARMV8A_TCR_EL2_PS_48BIT         (5UL << ARMV8A_TCR_EL2_PS_SHIFT)

#define ARMV8A_TCR_TBI_SHIFT            (20UL)
#define ARMV8A_TCR_TBI_MASK             (1UL << ARMV8A_TCR_TBI_SHIFT)

#define ARMV8A_TCR_EL2_RES1             (0x80800000UL)

/* TCR_EL1 bit field */
#define ARMV8A_TCR_EL1_T0SZ_SHIFT       (0UL)
#define ARMV8A_TCR_EL1_T0SZ_MASK        (0x3FUL)
#define ARMV8A_TCR_EL1_T0SZ(n)          ((n) & ARMV8A_TCR_EL1_T0SZ_MASK)

#define ARMV8A_TCR_EL1_EPD0_SHIFT       (7UL)
#define ARMV8A_TCR_EL1_EPD0_MASK        (1UL << ARMV8A_TCR_EL1_EPD0_SHIFT)

#define ARMV8A_TCR_EL1_IRGN0_SHIFT      (8UL)
#define ARMV8A_TCR_EL1_IRGN0_MASK       (3UL << ARMV8A_TCR_EL1_IRGN0_SHIFT)
#define ARMV8A_TCR_EL1_IRGN0_NC         (0UL << ARMV8A_TCR_EL1_IRGN0_SHIFT)
#define ARMV8A_TCR_EL1_IRGN0_WBWA       (1UL << ARMV8A_TCR_EL1_IRGN0_SHIFT)
#define ARMV8A_TCR_EL1_IRGN0_WT         (2UL << ARMV8A_TCR_EL1_IRGN0_SHIFT)
#define ARMV8A_TCR_EL1_IRGN0_WBNWA      (3UL << ARMV8A_TCR_EL1_IRGN0_SHIFT)

#define ARMV8A_TCR_EL1_ORGN0_SHIFT      (10UL)
#define ARMV8A_TCR_EL1_ORGN0_MASK       (3UL << ARMV8A_TCR_EL1_ORGN0_SHIFT)
#define ARMV8A_TCR_EL1_ORGN0_NC         (0UL << ARMV8A_TCR_EL1_ORGN0_SHIFT)
#define ARMV8A_TCR_EL1_ORGN0_WBWA       (1UL << ARMV8A_TCR_EL1_ORGN0_SHIFT)
#define ARMV8A_TCR_EL1_ORGN0_WT         (2UL << ARMV8A_TCR_EL1_ORGN0_SHIFT)
#define ARMV8A_TCR_EL1_ORGN0_WBNWA      (3UL << ARMV8A_TCR_EL1_ORGN0_SHIFT)
#define ARMV8A_TCR_EL1_ORGN0_MASK       (3UL << ARMV8A_TCR_EL1_ORGN0_SHIFT)

#define ARMV8A_TCR_EL1_SH0_SHIFT        (12UL)
#define ARMV8A_TCR_EL1_SH0_MASK         (3UL << ARMV8A_TCR_EL1_SH0_SHIFT)
#define ARMV8A_TCR_EL1_SH0_NON          (0UL << ARMV8A_TCR_EL1_SH0_SHIFT)
#define ARMV8A_TCR_EL1_SH0_OUTER        (2UL << ARMV8A_TCR_EL1_SH0_SHIFT)
#define ARMV8A_TCR_EL1_SH0_INNER        (3UL << ARMV8A_TCR_EL1_SH0_SHIFT)

#define ARMV8A_TCR_EL1_TG0_SHIFT        (14UL)
#define ARMV8A_TCR_EL1_TG0_MASK         (3UL << ARMV8A_TCR_EL1_TG0_SHIFT)
#define ARMV8A_TCR_EL1_TG0_4K           (0UL << ARMV8A_TCR_EL1_TG0_SHIFT)
#define ARMV8A_TCR_EL1_TG0_64K          (1UL << ARMV8A_TCR_EL1_TG0_SHIFT)
#define ARMV8A_TCR_EL1_TG0_16K          (2UL << ARMV8A_TCR_EL1_TG0_SHIFT)

#define ARMV8A_TCR_EL1_T1SZ_SHIFT       (16UL)
#define ARMV8A_TCR_EL1_T1SZ_MASK        (0x3FUL << ARMV8A_TCR_EL1_T1SZ_SHIFT)
#define ARMV8A_TCR_EL1_T1SZ(n)          ((n) & ARMV8A_TCR_T1SZ_MASK)

#define ARMV8A_TCR_EL1_A1_SHIFT         (22UL)
#define ARMV8A_TCR_EL1_A1_MASK          (1UL << ARMV8A_TCR_EL1_A1_SHIFT)

#define ARMV8A_TCR_EL1_EPD1_SHIFT       (23UL)
#define ARMV8A_TCR_EL1_EPD1_MASK        (1UL << ARMV8A_TCR_EL1_EPD1_SHIFT)

#define ARMV8A_TCR_EL1_IRGN1_SHIFT      (24UL)
#define ARMV8A_TCR_EL1_IRGN1_MASK       (3UL << ARMV8A_TCR_EL1_IRGN1_SHIFT)
#define ARMV8A_TCR_EL1_IRGN1_NC         (0UL << ARMV8A_TCR_EL1_IRGN1_SHIFT)
#define ARMV8A_TCR_EL1_IRGN1_WBWA       (1UL << ARMV8A_TCR_EL1_IRGN1_SHIFT)
#define ARMV8A_TCR_EL1_IRGN1_WT         (2UL << ARMV8A_TCR_EL1_IRGN1_SHIFT)
#define ARMV8A_TCR_EL1_IRGN1_WBNWA      (3UL << ARMV8A_TCR_EL1_IRGN1_SHIFT)

#define ARMV8A_TCR_EL1_ORGN1_SHIFT      (26UL)
#define ARMV8A_TCR_EL1_ORGN1_MASK       (3UL << ARMV8A_TCR_EL1_ORGN1_SHIFT)
#define ARMV8A_TCR_EL1_ORGN1_NC         (0UL << ARMV8A_TCR_EL1_ORGN1_SHIFT)
#define ARMV8A_TCR_EL1_ORGN1_WBWA       (1UL << ARMV8A_TCR_EL1_ORGN1_SHIFT)
#define ARMV8A_TCR_EL1_ORGN1_WT         (2UL << ARMV8A_TCR_EL1_ORGN1_SHIFT)
#define ARMV8A_TCR_EL1_ORGN1_WBNWA      (3UL << ARMV8A_TCR_EL1_ORGN1_SHIFT)
#define ARMV8A_TCR_EL1_ORGN1_MASK       (3UL << ARMV8A_TCR_EL1_ORGN1_SHIFT)

#define ARMV8A_TCR_EL1_SH1_SHIFT        (28UL)
#define ARMV8A_TCR_EL1_SH1_MASK         (3UL << ARMV8A_TCR_EL1_SH1_SHIFT)
#define ARMV8A_TCR_EL1_SH1_NON          (0UL << ARMV8A_TCR_EL1_SH1_SHIFT)
#define ARMV8A_TCR_EL1_SH1_OUTER        (2UL << ARMV8A_TCR_EL1_SH1_SHIFT)
#define ARMV8A_TCR_EL1_SH1_INNER        (3UL << ARMV8A_TCR_EL1_SH1_SHIFT)

#define ARMV8A_TCR_EL1_TG1_SHIFT        (30UL)
#define ARMV8A_TCR_EL1_TG1_MASK         (3UL << ARMV8A_TCR_EL1_TG1_SHIFT)
#define ARMV8A_TCR_EL1_TG1_4K           (0UL << ARMV8A_TCR_EL1_TG1_SHIFT)
#define ARMV8A_TCR_EL1_TG1_64K          (1UL << ARMV8A_TCR_EL1_TG1_SHIFT)
#define ARMV8A_TCR_EL1_TG1_16K          (2UL << ARMV8A_TCR_EL1_TG1_SHIFT)

#define ARMV8A_TCR_EL1_PS_SHIFT         (32UL)
#define ARMV8A_TCR_EL1_PS_MASK          (7UL << ARMV8A_TCR_EL1_PS_SHIFT)
#define ARMV8A_TCR_EL1_PS_32BIT         (0UL << ARMV8A_TCR_EL1_PS_SHIFT)
#define ARMV8A_TCR_EL1_PS_36BIT         (1UL << ARMV8A_TCR_EL1_PS_SHIFT)
#define ARMV8A_TCR_EL1_PS_40BIT         (2UL << ARMV8A_TCR_EL1_PS_SHIFT)
#define ARMV8A_TCR_EL1_PS_42BIT         (3UL << ARMV8A_TCR_EL1_PS_SHIFT)
#define ARMV8A_TCR_EL1_PS_44BIT         (4UL << ARMV8A_TCR_EL1_PS_SHIFT)
#define ARMV8A_TCR_EL1_PS_48BIT         (5UL << ARMV8A_TCR_EL1_PS_SHIFT)

#define ARMV8A_TCR_EL1_AS_SHIFT         (36UL)
#define ARMV8A_TCR_EL1_AS_MASK          (1UL << ARMV8A_TCR_EL1_AS_SHIFT)

#define ARMV8A_TCR_TBI0_AS_SHIFT        (37UL)
#define ARMV8A_TCR_TBI0_AS_MASK         (1UL << ARMV8A_TCR_TBI0_AS_SHIFT)

#define ARMV8A_TCR_TBI1_AS_SHIFT        (38UL)
#define ARMV8A_TCR_TBI1_AS_MASK         (1UL << ARMV8A_TCR_TBI1_AS_SHIFT)

/******** 64K granule translation ********/
/**
 * @brief 64K granule L2 block translation descriptor (For 512MiB Block)
 */
typedef union {
        struct {
                xwu64_t type:2; /**< IA[0:1] type, must be 1U */
                xwu64_t memattr:3; /**< IA[2:4] memory attribute selector */
                xwu64_t ns:1; /**< IA[5] Non-secure */
                xwu64_t ap:2; /**< IA[6:7] Data Access Permissions */
                xwu64_t sh:2; /**< IA[8:9] Shareability field */
                xwu64_t af:1; /**< IA[10] The Access flag bit */
                xwu64_t ng:1; /**< IA[11] The not global bit */
                xwu64_t res0_0:17; /**< IA[12:28] */
                xwu64_t oa:19; /**< IA[29:47] output address */
                xwu64_t res1_0:4; /**< IA[48:51] */
                xwu64_t contiguous:1; /**< IA[52] The contiguous bit */
                xwu64_t pxn:1; /**< IA[53] The privileged execute-never bit */
                xwu64_t xn:1; /**< IA[54] The execute-never bit */
                xwu64_t ignored:9; /**< IA[55:63] ignored bits */
        } bit;
        xwu64_t u64;
} armv8a_mmu_l2td_64k_block_t;

/**
 * @brief 64K granule table translation descriptor
 */
typedef union {
        struct {
                xwu64_t type:2; /**< IA[0:1] type, must be 3U */
                xwu64_t ignored0:14; /**< IA[2:15] */
                xwu64_t next_index:32; /**< IA[16:47] next-level table address */
                xwu64_t res0_0:4; /**< IA[48:51] */
                xwu64_t ignored1:7; /**< IA[52:58] */
                xwu64_t pxn:1; /**< IA[59] PXN */
                xwu64_t xn:1; /**< IA[60] XN */
                xwu64_t ap:2; /**< IA[61:62] AP */
                xwu64_t ns:1; /**< IA[63] NS */
        } bit;
        xwu64_t u64;
} armv8a_mmu_td_64k_table_t;

/**
 * @brief 64K granule L3 page translation descriptor (For 64KiB Page)
 */
typedef union {
        struct {
                xwu64_t type:2; /**< IA[0:1] type, must be 3U */
                xwu64_t memattr:3; /**< IA[2:4] memory attribute selector */
                xwu64_t ns:1; /**< IA[5] Non-secure */
                xwu64_t ap:2; /**< IA[6:7] Data Access Permissions */
                xwu64_t sh:2; /**< IA[8:9] Shareability field */
                xwu64_t af:1; /**< IA[10] The Access flag bit */
                xwu64_t ng:1; /**< IA[11] The not global bit */
                xwu64_t res0_0:4; /**< IA[12:15] */
                xwu64_t oa:32; /**< IA[16:47] output address */
                xwu64_t res1_0:4; /**< IA[48:51] */
                xwu64_t contiguous:1; /**< IA[52] The contiguous bit */
                xwu64_t pxn:1; /**< IA[53] The privileged execute-never bit */
                xwu64_t xn:1; /**< IA[54] The execute-never bit */
                xwu64_t ignored:9; /**< IA[55:63] ignored bits */
        } bit;
        xwu64_t u64;
} armv8a_mmu_l3td_64k_page_t;

typedef union {
        armv8a_mmu_td_64k_table_t table;
        armv8a_mmu_l2td_64k_block_t block;
        armv8a_mmu_l3td_64k_page_t page;
} armv8a_mmu_td_64k_t;

#define ARMV8A_MMU_64KTD_L2IDX_START_BIT                (29UL)
#define ARMV8A_MMU_64KTD_L2IDX_END_BIT                  (41UL)

#define ARMV8A_MMU_64KTD_TYPE_MASK                      (3UL)
#define ARMV8A_MMU_64KTD_TYPE_INVALID                   (0UL)
#define ARMV8A_MMU_64KTD_TYPE_TABLE                     (3UL) /**< L1/2 table */
#define ARMV8A_MMU_64KTD_TYPE_BLOCK                     (1UL) /**< L2 512MiB block */
#define ARMV8A_MMU_64KTD_TYPE_PAGE                      (3UL) /**< L3 64KiB page */

/* Memory Attribute */
#define ARMV8A_MAIR_ATTR_MASK(n)        (0xFFUL << ((n) * 8UL))
#define ARMV8A_MAIR_ATTR(n, attr)       ((attr) << ((n) * 8UL))

#define ARMV8A_MT_DEVICE_NGNRNE         0UL
#define ARMV8A_MT_DEVICE_NGNRE          1UL
#define ARMV8A_MT_DEVICE_GRE            2UL
#define ARMV8A_MT_MEMORY_NC             3UL
#define ARMV8A_MT_MEMORY                4UL

#define ARMV8A_MEMORY_ATTRIBUTES (ARMV8A_MAIR_ATTR(ARMV8A_MT_DEVICE_NGNRNE, 0x00UL) | \
                                  ARMV8A_MAIR_ATTR(ARMV8A_MT_DEVICE_NGNRE, 0x04UL) | \
                                  ARMV8A_MAIR_ATTR(ARMV8A_MT_DEVICE_GRE, 0x0CUL) | \
                                  ARMV8A_MAIR_ATTR(ARMV8A_MT_MEMORY_NC, 0x44UL) | \
                                  ARMV8A_MAIR_ATTR(ARMV8A_MT_MEMORY, 0xFFUL))

#define ARMV8A_MMU_64K_BTD_ATTR_NS                      (1UL << 5UL)

#define ARMV8A_MMU_64K_BTD_ATTR_AP_SHIFT                (6UL)
#define ARMV8A_MMU_64K_BTD_ATTR_AP_EL3_RW_EL2_RW        (1UL)
#define ARMV8A_MMU_64K_BTD_ATTR_AP_EL3_RO_EL2_RO        (3UL)
#define ARMV8A_MMU_64K_BTD_ATTR_AP_EL1_RW_EL0_NA        (0UL)
#define ARMV8A_MMU_64K_BTD_ATTR_AP_EL1_RW_EL0_RW        (1UL)
#define ARMV8A_MMU_64K_BTD_ATTR_AP_EL1_RO_EL0_NA        (2UL)
#define ARMV8A_MMU_64K_BTD_ATTR_AP_EL1_RO_EL0_RO        (3UL)

#define ARMV8A_MMU_64K_BTD_ATTR_SH_SHIFT                (8UL)
#define ARMV8A_MMU_64K_BTD_ATTR_SH_NONE                 (0UL)
#define ARMV8A_MMU_64K_BTD_ATTR_SH_OUTER                (2UL)
#define ARMV8A_MMU_64K_BTD_ATTR_SH_INNER                (3UL)

#define ARMV8A_MMU_64K_BTD_ATTR_AF                      (1UL << 10UL)
#define ARMV8A_MMU_64K_BTD_ATTR_NG                      (1UL << 11UL)
#define ARMV8A_MMU_64K_BTD_ATTR_CONTIGUOUS              (1UL << 52UL)
#define ARMV8A_MMU_64K_BTD_ATTR_PXN                     (1UL << 53UL)
#define ARMV8A_MMU_64K_BTD_ATTR_XN                      (1UL << 54UL)


void armv8a_invalidate_tlb_all(void);

#endif /* xwcd/soc/arm64/v8a/arch_mmu.h */
