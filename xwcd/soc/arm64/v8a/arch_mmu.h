/**
 * @file
 * @brief 架构描述层：ARMv8A MMU
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

#ifndef __xwcd_soc_arm_v8a_arch_mmu_h__
#define __xwcd_soc_arm_v8a_arch_mmu_h__

#include <xwos/standard.h>

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

#define ARMV8A_TCR_EL2_TBI_SHIFT        (20UL)
#define ARMV8A_TCR_EL2_TBI_MASK         (1UL << ARMV8A_TCR_EL2_TBI_SHIFT)

#define ARMV8A_TCR_EL2_HA_SHIFT         (21UL)
#define ARMV8A_TCR_EL2_HA_MASK          (1UL << ARMV8A_TCR_EL2_HA_SHIFT)

#define ARMV8A_TCR_EL2_HD_SHIFT         (22UL)
#define ARMV8A_TCR_EL2_HD_MASK          (1UL << ARMV8A_TCR_EL2_HD_SHIFT)

#define ARMV8A_TCR_EL2_HPD_SHIFT        (24UL)
#define ARMV8A_TCR_EL2_HPD_MASK         (1UL << ARMV8A_TCR_EL2_HPD_SHIFT)

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

/* Memory Attribute */
#define ARMV8A_MAIR_ATTR_MASK(n)        (0xFFUL << ((n) * 8UL))
#define ARMV8A_MAIR_ATTR(n, attr)       ((attr) << ((n) * 8UL))

#define ARMV8A_MT_DEVICE_NGNRNE         0UL
#define ARMV8A_MT_DEVICE_NGNRE          1UL
#define ARMV8A_MT_DEVICE_GRE            2UL
#define ARMV8A_MT_MEMORY_NC             3UL
#define ARMV8A_MT_MEMORY                4UL
#define ARMV8A_MT_MEMORY_WT             5UL

#define ARMV8A_MEMORY_ATTRIBUTES (ARMV8A_MAIR_ATTR(ARMV8A_MT_DEVICE_NGNRNE, 0x00UL) | \
                                  ARMV8A_MAIR_ATTR(ARMV8A_MT_DEVICE_NGNRE, 0x04UL) | \
                                  ARMV8A_MAIR_ATTR(ARMV8A_MT_DEVICE_GRE, 0x0CUL) | \
                                  ARMV8A_MAIR_ATTR(ARMV8A_MT_MEMORY_NC, 0x44UL) | \
                                  ARMV8A_MAIR_ATTR(ARMV8A_MT_MEMORY, 0xFFUL) | \
                                  ARMV8A_MAIR_ATTR(ARMV8A_MT_MEMORY_WT, 0xBBUL))

/******** ******** translation descriptor ******** ********/
/******** Type ********/
#define ARMV8A_MMU_TD_TYPE_INVALID              (0UL)
#define ARMV8A_MMU_TD_TYPE_TABLE                (3UL) /**< L0/L1/2 table */
#define ARMV8A_MMU_TD_TYPE_BLOCK                (1UL) /**< L2 block */
#define ARMV8A_MMU_TD_TYPE_PAGE                 (3UL) /**< L3 page */

/******** AP[2:1] ********/
#define ARMV8A_MMU_TD_ATTR_AP2_RW               (0U) /**< Read, Write */
#define ARMV8A_MMU_TD_ATTR_AP2_RO               (2U) /**< Read Only */
#define ARMV8A_MMU_TD_ATTR_AP1_UNPRIV           (1U) /**< Unprivileged enabled */
#define ARMV8A_MMU_TD_ATTR_AP1_PRIV             (0U) /**< Unprivileged disabled */

/******** SH[1:0] ********/
#define ARMV8A_MMU_TD_ATTR_SH_NONE              (0UL)
#define ARMV8A_MMU_TD_ATTR_SH_OUTER             (2UL)
#define ARMV8A_MMU_TD_ATTR_SH_INNER             (3UL)

/******** APTable[1:0] ********/
#define ARMV8A_MMU_TD_ATTR_APTAB1_RW            (0U) /**< Read, Write */
#define ARMV8A_MMU_TD_ATTR_APTAB1_RO            (2U) /**< Read Only */
#define ARMV8A_MMU_TD_ATTR_APTAB0_UNPRIV        (0U) /**< Unprivileged enabled */
#define ARMV8A_MMU_TD_ATTR_APTAB0_PRIV          (1U) /**< Unprivileged disabled */

/******** 64K granule translation descriptor ********/
/**
 * @brief 64K granule table translation descriptor
 */
typedef union {
        struct {
                xwu64_t type:2; /**< IA[0:1] type, must be 3U */
                xwu64_t res0_0:10; /**< IA[2:11] */
                xwu64_t nlta48_51:4; /**< IA[12:15] next-level address[48:51] */
                xwu64_t nlta16_47:32; /**< IA[16:47] next-level address[16:47] */
                xwu64_t res0_1:3; /**< IA[48:50] */
                xwu64_t res0_2:8; /**< IA[51:58] */
                xwu64_t pxn:1; /**< IA[59] The privileged execute-never bit */
                xwu64_t uxn:1; /**< IA[60] The Unprivileged execute-never bit */
                xwu64_t ap:2; /**< IA[61:62] AP */
                xwu64_t ns:1; /**< IA[63] NS */
        } b;
        xwu64_t u64;
} armv8a_mmu_s1td64k_table_t;

/**
 * @brief 64K granule block translation descriptor
 */
typedef union {
        struct {
                xwu64_t type:2; /**< IA[0:1] type, must be 1U */
                xwu64_t attridx:3; /**< IA[2:4] memory attribute selector */
                xwu64_t ns:1; /**< IA[5] Non-secure */
                xwu64_t ap:2; /**< IA[6:7] Data Access Permissions */
                xwu64_t sh:2; /**< IA[8:9] Shareability field */
                xwu64_t af:1; /**< IA[10] The Access flag bit */
                xwu64_t ng:1; /**< IA[11] The not global bit */
                xwu64_t oa48_51:4; /**< IA[12:15] output address[48:51] */
                xwu64_t nt:1; /**< IA[16] ARMv8.3 FEAT_BBML1 */
                xwu64_t res0_0:12; /**< IA[17:28] */
                xwu64_t oa29_47:19; /**< IA[29:47] output address[29:47] */
                xwu64_t res0_1:2; /**< IA[48:49] */
                xwu64_t gp:1; /**< IA[50] Guarded Page */
                xwu64_t dbm:1; /**< IA[51] Dirty bit modifier */
                xwu64_t contiguous:1; /**< IA[52] The contiguous bit */
                xwu64_t pxn:1; /**< IA[53] The privileged execute-never bit */
                xwu64_t uxn:1; /**< IA[54] The Unprivileged execute-never bit */
                xwu64_t res0_2:9; /**< IA[55:63] */
        } sz512m;
        struct {
                xwu64_t type:2; /**< IA[0:1] type, must be 1U */
                xwu64_t attridx:3; /**< IA[2:4] memory attribute selector */
                xwu64_t ns:1; /**< IA[5] Non-secure */
                xwu64_t ap:2; /**< IA[6:7] Data Access Permissions */
                xwu64_t sh:2; /**< IA[8:9] Shareability field */
                xwu64_t af:1; /**< IA[10] The Access flag bit */
                xwu64_t ng:1; /**< IA[11] The not global bit */
                xwu64_t oa48_51:4; /**< IA[12:15] output address[48:51] */
                xwu64_t nt:1; /**< IA[16] ARMv8.3 FEAT_BBML1 */
                xwu64_t res0_0:25; /**< IA[17:41] */
                xwu64_t oa42_47:6; /**< IA[42:47] output address[42:47] */
                xwu64_t res0_1:2; /**< IA[48:49] */
                xwu64_t gp:1; /**< IA[50] Guarded Page */
                xwu64_t dbm:1; /**< IA[51] Dirty bit modifier */
                xwu64_t contiguous:1; /**< IA[52] The contiguous bit */
                xwu64_t pxn:1; /**< IA[53] The privileged execute-never bit */
                xwu64_t uxn:1; /**< IA[54] The Unprivileged execute-never bit */
                xwu64_t res0_2:9; /**< IA[55:63] */
        } sz4t;
        xwu64_t u64;
} armv8a_mmu_s1td64k_block_t;

/**
 * @brief 64K granule page translation descriptor
 */
typedef union {
        struct {
                xwu64_t type:2; /**< IA[0:1] type, must be 3U */
                xwu64_t attridx:3; /**< IA[2:4] memory attribute selector */
                xwu64_t ns:1; /**< IA[5] Non-secure */
                xwu64_t ap:2; /**< IA[6:7] Data Access Permissions */
                xwu64_t sh:2; /**< IA[8:9] Shareability field */
                xwu64_t af:1; /**< IA[10] The Access flag bit */
                xwu64_t ng:1; /**< IA[11] The not global bit */
                xwu64_t oa48_51:4; /**< IA[12:15] */
                xwu64_t oa16_47:32; /**< IA[16:47] output address[16:47] */
                xwu64_t res0_0:2; /**< IA[48:49] */
                xwu64_t gp:1; /**< IA[50] Guarded Page */
                xwu64_t dbm:1; /**< IA[51] Dirty bit modifier */
                xwu64_t contiguous:1; /**< IA[52] The contiguous bit */
                xwu64_t pxn:1; /**< IA[53] The privileged execute-never bit */
                xwu64_t uxn:1; /**< IA[54] The Unprivileged execute-never bit */
                xwu64_t res0_1:9; /**< IA[55:63] */
        } sz64k;
        xwu64_t u64;
} armv8a_mmu_s1td64k_page_t;

typedef union {
        armv8a_mmu_s1td64k_table_t table;
        armv8a_mmu_s1td64k_block_t block;
        armv8a_mmu_s1td64k_page_t page;
} armv8a_mmu_s1td64k_t;

#define ARMV8A_MMU_S1TD64K_L1TD_START_BIT               (42UL)
#define ARMV8A_MMU_S1TD64K_L2TD_START_BIT               (29UL)
#define ARMV8A_MMU_S1TD64K_L3TD_START_BIT               (16UL)
#define ARMV8A_MMU_S1TD64K_TABLE_SHIFT                  (16UL)
#define ARMV8A_MMU_S1TD64K_TD_NUM                       (8192UL)

/******** 16K granule translation descriptor ********/
/**
 * @brief 16K granule table translation descriptor
 */
typedef union {
        struct {
                xwu64_t type:2; /**< IA[0:1] type, must be 3U */
                xwu64_t res0_0:6; /**< IA[2:7] */
                xwu64_t nlta50_51:2; /**< IA[8:9] next-level address[50:51] */
                xwu64_t res0_1:4; /**< IA[10:13] */
                xwu64_t nlta14_49:36; /**< IA[14:49] next-level address[14:49] */
                xwu64_t res0_2:1; /**< IA[50] */
                xwu64_t res0_3:8; /**< IA[51:58] */
                xwu64_t pxn:1; /**< IA[59] The privileged execute-never bit */
                xwu64_t uxn:1; /**< IA[60] The Unprivileged execute-never bit */
                xwu64_t ap:2; /**< IA[61:62] AP */
                xwu64_t ns:1; /**< IA[63] NS */
        } b;
        xwu64_t u64;
} armv8a_mmu_s1td16k_table_t;

/**
 * @brief 16K granule block translation descriptor
 */
typedef union {
        struct {
                xwu64_t type:2; /**< IA[0:1] type, must be 1U */
                xwu64_t attridx:3; /**< IA[2:4] memory attribute selector */
                xwu64_t ns:1; /**< IA[5] Non-secure */
                xwu64_t ap:2; /**< IA[6:7] Data Access Permissions */
                xwu64_t sh:2; /**< IA[8:9] Shareability field */
                xwu64_t af:1; /**< IA[10] The Access flag bit */
                xwu64_t ng:1; /**< IA[11] The not global bit */
                xwu64_t res0_0:4; /**< IA[12:15] output address[48:51] */
                xwu64_t nt:1; /**< IA[16] ARMv8.3 FEAT_BBML1 */
                xwu64_t res0_1:19; /**< IA[17:35] */
                xwu64_t oa36_47:12; /**< IA[36:47] output address[36:47] */
                xwu64_t res0_2:2; /**< IA[48:49] */
                xwu64_t gp:1; /**< IA[50] Guarded Page */
                xwu64_t dbm:1; /**< IA[51] Dirty bit modifier */
                xwu64_t contiguous:1; /**< IA[52] The contiguous bit */
                xwu64_t pxn:1; /**< IA[53] The privileged execute-never bit */
                xwu64_t uxn:1; /**< IA[54] The Unprivileged execute-never bit */
                xwu64_t res0_3:9; /**< IA[55:63] */
        } sz64g;
        struct {
                xwu64_t type:2; /**< IA[0:1] type, must be 1U */
                xwu64_t attridx:3; /**< IA[2:4] memory attribute selector */
                xwu64_t ns:1; /**< IA[5] Non-secure */
                xwu64_t ap:2; /**< IA[6:7] Data Access Permissions */
                xwu64_t sh:2; /**< IA[8:9] Shareability field */
                xwu64_t af:1; /**< IA[10] The Access flag bit */
                xwu64_t ng:1; /**< IA[11] The not global bit */
                xwu64_t res0_0:4; /**< IA[12:15] output address[48:51] */
                xwu64_t nt:1; /**< IA[16] ARMv8.3 FEAT_BBML1 */
                xwu64_t res0_1:8; /**< IA[17:24] */
                xwu64_t oa25_47:23; /**< IA[25:47] output address[25:47] */
                xwu64_t res0_2:2; /**< IA[48:49] */
                xwu64_t gp:1; /**< IA[50] Guarded Page */
                xwu64_t dbm:1; /**< IA[51] Dirty bit modifier */
                xwu64_t contiguous:1; /**< IA[52] The contiguous bit */
                xwu64_t pxn:1; /**< IA[53] The privileged execute-never bit */
                xwu64_t uxn:1; /**< IA[54] The Unprivileged execute-never bit */
                xwu64_t res0_3:9; /**< IA[55:63] */
        } sz32m;
        xwu64_t u64;
} armv8a_mmu_s1td16k_block_t;

/**
 * @brief 16K granule page translation descriptor
 */
typedef union {
        struct {
                xwu64_t type:2; /**< IA[0:1] type, must be 1U */
                xwu64_t attridx:3; /**< IA[2:4] memory attribute selector */
                xwu64_t ns:1; /**< IA[5] Non-secure */
                xwu64_t ap:2; /**< IA[6:7] Data Access Permissions */
                xwu64_t sh:2; /**< IA[8:9] Shareability field */
                xwu64_t af:1; /**< IA[10] The Access flag bit */
                xwu64_t ng:1; /**< IA[11] The not global bit */
                xwu64_t res0_0:2; /**< IA[12:13] */
                xwu64_t oa14_47:34; /**< IA[14:47] output address[14:47] */
                xwu64_t res0_1:2; /**< IA[48:49] */
                xwu64_t gp:1; /**< IA[50] Guarded Page */
                xwu64_t dbm:1; /**< IA[51] Dirty bit modifier */
                xwu64_t contiguous:1; /**< IA[52] The contiguous bit */
                xwu64_t pxn:1; /**< IA[53] The privileged execute-never bit */
                xwu64_t uxn:1; /**< IA[54] The Unprivileged execute-never bit */
                xwu64_t res0_2:9; /**< IA[55:63] */
        } sz4k;
        xwu64_t u64;
} armv8a_mmu_s1td16k_page_t;

typedef union {
        armv8a_mmu_s1td16k_table_t table;
        armv8a_mmu_s1td16k_block_t block;
        armv8a_mmu_s1td16k_page_t page;
} armv8a_mmu_s1td16k_t;

#define ARMV8A_MMU_S1TD16K_L0TD_START_BIT               (47UL)
#define ARMV8A_MMU_S1TD16K_L1TD_START_BIT               (36UL)
#define ARMV8A_MMU_S1TD16K_L2TD_START_BIT               (25UL)
#define ARMV8A_MMU_S1TD16K_L3TD_START_BIT               (14UL)
#define ARMV8A_MMU_S1TD16K_TABLE_SHIFT                  (14UL)
#define ARMV8A_MMU_S1TD16K_TD_NUM                       (2048UL)

/******** 4K granule translation descriptor ********/
/**
 * @brief 4K granule table translation descriptor
 */
typedef union {
        struct {
                xwu64_t type:2; /**< IA[0:1] type, must be 3U */
                xwu64_t res0_0:6; /**< IA[2:7] */
                xwu64_t nlta50_51:2; /**< IA[8:9] next-level address[50:51] */
                xwu64_t res0_1:2; /**< IA[10:11] */
                xwu64_t nlta12_49:38; /**< IA[12:49] next-level address[12:49] */
                xwu64_t res0_2:1; /**< IA[50] */
                xwu64_t res0_3:8; /**< IA[51:58] */
                xwu64_t pxn:1; /**< IA[59] The privileged execute-never bit */
                xwu64_t uxn:1; /**< IA[60] The Unprivileged execute-never bit */
                xwu64_t ap:2; /**< IA[61:62] AP */
                xwu64_t ns:1; /**< IA[63] NS */
        } b;
        xwu64_t u64;
} armv8a_mmu_s1td4k_table_t;

/**
 * @brief 4K granule block translation descriptor
 */
typedef union {
        struct {
                xwu64_t type:2; /**< IA[0:1] type, must be 1U */
                xwu64_t attridx:3; /**< IA[2:4] memory attribute selector */
                xwu64_t ns:1; /**< IA[5] Non-secure */
                xwu64_t ap:2; /**< IA[6:7] Data Access Permissions */
                xwu64_t sh:2; /**< IA[8:9] Shareability field */
                xwu64_t af:1; /**< IA[10] The Access flag bit */
                xwu64_t ng:1; /**< IA[11] The not global bit */
                xwu64_t res0_0:4; /**< IA[12:15] output address[48:51] */
                xwu64_t nt:1; /**< IA[16] ARMv8.3 FEAT_BBML1 */
                xwu64_t res0_1:13; /**< IA[17:29] */
                xwu64_t oa30_47:18; /**< IA[30:47] output address[30:47] */
                xwu64_t res0_2:2; /**< IA[48:49] */
                xwu64_t gp:1; /**< IA[50] Guarded Page */
                xwu64_t dbm:1; /**< IA[51] Dirty bit modifier */
                xwu64_t contiguous:1; /**< IA[52] The contiguous bit */
                xwu64_t pxn:1; /**< IA[53] The privileged execute-never bit */
                xwu64_t uxn:1; /**< IA[54] The Unprivileged execute-never bit */
                xwu64_t res0_3:9; /**< IA[55:63] */
        } sz1g;
        struct {
                xwu64_t type:2; /**< IA[0:1] type, must be 1U */
                xwu64_t attridx:3; /**< IA[2:4] memory attribute selector */
                xwu64_t ns:1; /**< IA[5] Non-secure */
                xwu64_t ap:2; /**< IA[6:7] Data Access Permissions */
                xwu64_t sh:2; /**< IA[8:9] Shareability field */
                xwu64_t af:1; /**< IA[10] The Access flag bit */
                xwu64_t ng:1; /**< IA[11] The not global bit */
                xwu64_t res0_0:4; /**< IA[12:15] output address[48:51] */
                xwu64_t nt:1; /**< IA[16] ARMv8.3 FEAT_BBML1 */
                xwu64_t res0_1:4; /**< IA[17:20] */
                xwu64_t oa21_47:27; /**< IA[21:47] output address[21:47] */
                xwu64_t res0_2:2; /**< IA[48:49] */
                xwu64_t gp:1; /**< IA[50] Guarded Page */
                xwu64_t dbm:1; /**< IA[51] Dirty bit modifier */
                xwu64_t contiguous:1; /**< IA[52] The contiguous bit */
                xwu64_t pxn:1; /**< IA[53] The privileged execute-never bit */
                xwu64_t uxn:1; /**< IA[54] The Unprivileged execute-never bit */
                xwu64_t res0_3:9; /**< IA[55:63] */
        } sz2m;
        xwu64_t u64;
} armv8a_mmu_s1td4k_block_t;

/**
 * @brief 4K granule page translation descriptor
 */
typedef union {
        struct {
                xwu64_t type:2; /**< IA[0:1] type, must be 1U */
                xwu64_t attridx:3; /**< IA[2:4] memory attribute selector */
                xwu64_t ns:1; /**< IA[5] Non-secure */
                xwu64_t ap:2; /**< IA[6:7] Data Access Permissions */
                xwu64_t sh:2; /**< IA[8:9] Shareability field */
                xwu64_t af:1; /**< IA[10] The Access flag bit */
                xwu64_t ng:1; /**< IA[11] The not global bit */
                xwu64_t oa12_47:36; /**< IA[12:47] output address[12:47] */
                xwu64_t res0_0:2; /**< IA[48:49] */
                xwu64_t gp:1; /**< IA[50] Guarded Page */
                xwu64_t dbm:1; /**< IA[51] Dirty bit modifier */
                xwu64_t contiguous:1; /**< IA[52] The contiguous bit */
                xwu64_t pxn:1; /**< IA[53] The privileged execute-never bit */
                xwu64_t uxn:1; /**< IA[54] The Unprivileged execute-never bit */
                xwu64_t res0_1:9; /**< IA[55:63] */
        } sz4k;
        xwu64_t u64;
} armv8a_mmu_s1td4k_page_t;

typedef union {
        armv8a_mmu_s1td4k_table_t table;
        armv8a_mmu_s1td4k_block_t block;
        armv8a_mmu_s1td4k_page_t page;
} armv8a_mmu_s1td4k_t;

#define ARMV8A_MMU_S1TD4K_L0TD_START_BIT                (39UL)
#define ARMV8A_MMU_S1TD4K_L1TD_START_BIT                (30UL)
#define ARMV8A_MMU_S1TD4K_L2TD_START_BIT                (21UL)
#define ARMV8A_MMU_S1TD4K_L3TD_START_BIT                (12UL)
#define ARMV8A_MMU_S1TD4K_TABLE_SHIFT                   (12UL)
#define ARMV8A_MMU_S1TD4K_TD_NUM                        (512UL)

/******** APIs ********/
void armv8a_invalidate_tlb_all(void);

#endif /* xwcd/soc/arm64/v8a/arch_mmu.h */
