/**
 * @file
 * @brief SOC描述层：Mailbox
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

#ifndef __xwcd_soc_arm_v7m_m7_fc7300_soc_mb_h__
#define __xwcd_soc_arm_v7m_m7_fc7300_soc_mb_h__

#include <xwos/standard.h>

#define SOC_MB_CH_COUNT         (16)
#define SOC_MB_INT_COUNT        (5U)

struct soc_mb_regs {
        struct {
                const xwu32_t sema; /* SEMA, offset: 0x000 */
                xwu32_t sema_unlk; /* SEMA_UNLK, offset: 0x004 */
                xwu32_t request; /* REQUEST, offset: 0x008 */
                xwu32_t done; /* DONE, offset: 0x00C */
                xwu32_t done_mask; /* DONE_MASK, offset: 0x010 */
                xwu32_t data0; /* DATA0, offset: 0x014 */
                xwu32_t data1; /* DATA1, offset: 0x018 */
                const xwu32_t stat; /* STAT, offset: 0x01C */
                xwu32_t clr; /* CLR, offset: 0x020 */
                xwu8_t  reserved0[0xC]; /* offset: 0x024 */
        } channel[SOC_MB_CH_COUNT];
        xwu8_t  reserved1[0x500]; /* offset: 0x300 */
        struct {
                xwu32_t flg; /* FLG, offset: 0x800 */
                xwu32_t flg_mask; /* FLG_MASK, offset: 0x804 */
                xwu32_t inten; /* INTEN, offset: 0x808 */
                xwu32_t flg_stat; /* FLG_STAT, offset: 0x80C */
                xwu32_t ctrl; /* CTRL, offset: 0x810 */
                xwu8_t  reserved2[0xC]; /* offset: 0x814 */
        } intr[SOC_MB_INT_COUNT];
};

#define SOC_MB_BASE (0x40058000U)
#define soc_mb (*((volatile struct soc_mb_regs *)SOC_MB_BASE))

#define SOC_MB_MASTER_DONE_CODE                 (0xFC730000U)
#define SOC_MB_NESTED_LOCKCODE                  (0x00800000U)
#define SOC_MB_CH_XWOS_SPINLOCK                 (SOCCFG_MB_CH_XWOS_SPINLOCK)
#define SOC_MB_CH_XWAOP                         (SOCCFG_MB_CH_XWAOP)
#define SOC_MB_CH_ICC                           (SOCCFG_MB_CH_ICC)

#if defined(SOCCFG_CHIP_FC7300F8MDQ) && (1U == SOCCFG_CHIP_FC7300F8MDQ)
#  define SOC_MB_CORE_NUM                       (5U)
#else
#  define SOC_MB_CORE_NUM                       (4U)
#endif

extern const xwu8_t soc_mb_master_id_tab[SOC_MB_CORE_NUM];

#if defined(CPUCFG_CPU_NUM) && (4U == CPUCFG_CPU_NUM)
#  define SOC_MB_CPU2MB_IDX(c) ((xwu32_t)((c) == 3U ? 4U : (c)))
#  define SOC_MB_MB2CPU_IDX(m) ((xwu32_t)((m) == 4U ? 3U : (m)))
#else
#  define SOC_MB_CPU2MB_IDX(c) ((xwu32_t)(c))
#  define SOC_MB_MB2CPU_IDX(m) ((xwu32_t)(m))
#endif

/* MB.DONE_MASK Bit Fields */
#define SOC_MB_SEMA_LOCK_MASK                   (0x80000000U)
#define SOC_MB_SEMA_LOCK_SHIFT                  (31U)
#define SOC_MB_SEMA_LOCK_WIDTH                  (1U)
#define SOC_MB_SEMA_LOCK(x)                     \
        ((xwu32_t)((((xwu32_t)(x)) << SOC_MB_SEMA_LOCK_SHIFT) & SOC_MB_SEMA_LOCK_MASK))

#define SOC_MB_SEMA_MASTER_ID_MASK              (0xF0U)
#define SOC_MB_SEMA_MASTER_ID_SHIFT             (4U)
#define SOC_MB_SEMA_MASTER_ID_WIDTH             (4U)
#define SOC_MB_SEMA_MASTER_ID(x)                \
        ((xwu32_t)((((xwu32_t)(x)) << SOC_MB_SEMA_MASTER_ID_SHIFT) & \
                   SOC_MB_SEMA_MASTER_ID_MASK))

#define SOC_MB_SEMA_LOCK_MASTER_ID_MASK (0x800000F0U)

/* MB.SEMA_UNLK Bit Fields */
#define SOC_MB_SEMA_UNLK_AUTO_CLEAR_EN_MASK     (0x1F)
#define SOC_MB_SEMA_UNLK_AUTO_CLEAR_EN_SHIFT    (0U)
#define SOC_MB_SEMA_UNLK_AUTO_CLEAR_EN_WIDTH    (5U)
#define SOC_MB_SEMA_UNLK_AUTO_CLEAR_EN(x)       \
        ((xwu32_t)((((xwu32_t)(x)) << SOC_MB_SEMA_UNLK_AUTO_CLEAR_EN_SHIFT) & \
                   SOC_MB_SEMA_UNLK_AUTO_CLEAR_EN_MASK))

/* MB.REQUEST Bit Fields */
#define SOC_MB_REQUEST_REQ_MASK                 (0x1F)
#define SOC_MB_REQUEST_REQ_SHIFT                (0U)
#define SOC_MB_REQUEST_REQ_WIDTH                (5U)
#define SOC_MB_REQUEST_REQ(x)                   \
        ((xwu32_t)((((xwu32_t)(x)) << SOC_MB_REQUEST_REQ_SHIFT) & \
                   SOC_MB_REQUEST_REQ_MASK))

/* MB.DONE Bit Fields */
#define SOC_MB_DONE_DONE_MASK                   (0x1F)
#define SOC_MB_DONE_DONE_SHIFT                  (0U)
#define SOC_MB_DONE_DONE_WIDTH                  (5U)
#define SOC_MB_DONE_DONE(x)                     \
        ((xwu32_t)((((xwu32_t)(x)) << SOC_MB_DONE_DONE_SHIFT) & \
                   SOC_MB_DONE_DONE_MASK))

/* MB_DONE_MASK Bit Fields */
#define SOC_MB_DONE_MASK_DONE_MASTER_ID_MASK    (0xF0000U)
#define SOC_MB_DONE_MASK_DONE_MASTER_ID_SHIFT   (16U)
#define SOC_MB_DONE_MASK_DONE_MASTER_ID_WIDTH   (4U)
#define SOC_MB_DONE_MASK_DONE_MASTER_ID(x)      \
        ((xwu32_t)((((xwu32_t)(x)) << SOC_MB_DONE_MASK_DONE_MASTER_ID_SHIFT) & \
                   SOC_MB_DONE_MASK_DONE_MASTER_ID_MASK))

#define SOC_MB_DONE_MASK_DONE_MASK_MASK         (0x1FU)
#define SOC_MB_DONE_MASK_DONE_MASK_SHIFT        (0U)
#define SOC_MB_DONE_MASK_DONE_MASK_WIDTH        (5U)
#define SOC_MB_DONE_MASK_DONE_MASK(x)           \
        ((xwu32_t)((((xwu32_t)(x)) << SOC_MB_DONE_MASK_DONE_MASK_SHIFT) & \
                   SOC_MB_DONE_MASK_DONE_MASK0_MASK))

/* MB.STAT Bit Fields */
#define SOC_MB_STAT_CURRENT_LOCK_STATUS_MASK    (0x80000000U)
#define SOC_MB_STAT_CURRENT_LOCK_STATUS_SHIFT   (31U)
#define SOC_MB_STAT_CURRENT_LOCK_STATUS_WIDTH   (1U)
#define SOC_MB_STAT_CURRENT_LOCK_STATUS(x)      \
        ((xwu32_t)((((xwu32_t)(x)) << SOC_MB_STAT_CURRENT_LOCK_STATUS_SHIFT) & \
                   SOC_MB_STAT_CURRENT_LOCK_STATUS_MASK))

#define SOC_MB_STAT_CURRENT_CPU_STATUS_MASK     (0xFFFF00U)
#define SOC_MB_STAT_CURRENT_CPU_STATUS_SHIFT    (8U)
#define SOC_MB_STAT_CURRENT_CPU_STATUS_WIDTH    (16U)
#define SOC_MB_STAT_CURRENT_CPU_STATUS(x)       \
        ((xwu32_t)((((xwu32_t)(x)) << SOC_MB_STAT_CURRENT_CPU_STATUS_SHIFT) & \
                   SOC_MB_STAT_CURRENT_CPU_STATUS_MASK))

#define SOC_MB_STAT_CURRENT_LOCK_MASTER_ID_MASK         (0xF0U)
#define SOC_MB_STAT_CURRENT_LOCK_MASTER_ID_SHIFT        (4U)
#define SOC_MB_STAT_CURRENT_LOCK_MASTER_ID_WIDTH        (4U)
#define SOC_MB_STAT_CURRENT_LOCK_MASTER_ID(x)           \
        ((xwu32_t)((((xwu32_t)(x)) << SOC_MB_STAT_CURRENT_LOCK_MASTER_ID_SHIFT) & \
                   SOC_MB_STAT_CURRENT_LOCK_MASTER_ID_MASK))

#define SOC_MB_STAT_CURRENT_LOCK_MASTER_SEC_MASK        (0x2U)
#define SOC_MB_STAT_CURRENT_LOCK_MASTER_SEC_SHIFT       (1U)
#define SOC_MB_STAT_CURRENT_LOCK_MASTER_SEC_WIDTH       (1U)
#define SOC_MB_STAT_CURRENT_LOCK_MASTER_SEC(x)          \
        ((xwu32_t)((((xwu32_t)(x)) << SOC_MB_STAT_CURRENT_LOCK_MASTER_SEC_SHIFT) & \
                   SOC_MB_STAT_CURRENT_LOCK_MASTER_SEC_MASK))

#define SOC_MB_STAT_CURRENT_LOCK_MASTER_SUPERVISOR_MASK         (0x1U)
#define SOC_MB_STAT_CURRENT_LOCK_MASTER_SUPERVISOR_SHIFT        (0U)
#define SOC_MB_STAT_CURRENT_LOCK_MASTER_SUPERVISOR_WIDTH        (1U)
#define SOC_MB_STAT_CURRENT_LOCK_MASTER_SUPERVISOR(x)   \
        ((xwu32_t)((((xwu32_t)(x)) << SOC_MB_STAT_CURRENT_LOCK_MASTER_SUPERVISOR_SHIFT) & \
                   SOC_MB_STAT_CURRENT_LOCK_MASTER_SUPERVISOR_MASK))

/* MB functions */
xwu32_t soc_mb_lock(xwu32_t ch);
void soc_mb_unlock(xwu32_t ch, xwu32_t lockcode);

#endif /* xwcd/soc/arm/v7m/m7/fc7300/soc_mb.h */
