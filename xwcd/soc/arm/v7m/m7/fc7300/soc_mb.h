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

#define SOC_MB_SEMA_LOCK_MASK           (0x80000000U)
#define SOC_MB_SEMA_LOCK_SHIFT          (31U)
#define SOC_MB_SEMA_LOCK_WIDTH          (1U)
#define SOC_MB_SEMA_LOCK(x)             ((xwu32_t)((((xwu32_t)(x)) << \
                                                    SOC_MB_SEMA_LOCK_SHIFT) & \
                                                   SOC_MB_SEMA_LOCK_MASK))

#define SOC_MB_MASTER_DONE_CODE         (0xFC730000U)


static __xwbsp_inline
xwu32_t soc_mb_lock(xwu32_t ch)
{
        return soc_mb.channel[ch].sema & SOC_MB_SEMA_LOCK_MASK;
}

static __xwbsp_inline
void soc_mb_unlock(xwu32_t ch)
{
        soc_mb.channel[ch].done = SOC_MB_MASTER_DONE_CODE;
}

#endif /* xwcd/soc/arm/v7m/m7/fc7300/soc_mb.h */
