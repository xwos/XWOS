/**
 * @file
 * @brief SOC watchdog
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
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

#ifndef __soc_wdg_h__
#define __soc_wdg_h__

#include <xwos/standard.h>
#include <soc.h>

/* CS1 Bit Fields */
#define SOC_WDG_CS1_STOP_MASK           0x1U
#define SOC_WDG_CS1_STOP_SHIFT          0
#define SOC_WDG_CS1_WAIT_MASK           0x2U
#define SOC_WDG_CS1_WAIT_SHIFT          1
#define SOC_WDG_CS1_DBG_MASK            0x4U
#define SOC_WDG_CS1_DBG_SHIFT           2
#define SOC_WDG_CS1_TST_MASK            0x18U
#define SOC_WDG_CS1_TST_SHIFT           3
#define SOC_WDG_CS1_TST(x)              \
        (((xwu8_t)(((xwu8_t)(x)) << SOC_WDG_CS1_TST_SHIFT)) & SOC_WDG_CS1_TST_MASK)
#define SOC_WDG_CS1_UPDATE_MASK         0x20U
#define SOC_WDG_CS1_UPDATE_SHIFT        5
#define SOC_WDG_CS1_INT_MASK            0x40U
#define SOC_WDG_CS1_INT_SHIFT           6
#define SOC_WDG_CS1_EN_MASK             0x80U
#define SOC_WDG_CS1_EN_SHIFT            7
/* CS2 Bit Fields */
#define SOC_WDG_CS2_CLK_MASK            0x3U
#define SOC_WDG_CS2_CLK_SHIFT           0
#define SOC_WDG_CS2_CLK(x)              \
        (((xwu8_t)(((xwu8_t)(x)) << SOC_WDG_CS2_CLK_SHIFT)) & SOC_WDG_CS2_CLK_MASK)
#define SOC_WDG_CS2_PRES_MASK           0x10U
#define SOC_WDG_CS2_PRES_SHIFT          4
#define SOC_WDG_CS2_FLG_MASK            0x40U
#define SOC_WDG_CS2_FLG_SHIFT           6
#define SOC_WDG_CS2_WIN_MASK            0x80U
#define SOC_WDG_CS2_WIN_SHIFT           7

/**
 * @brief watchdog unlock routine.
 */
#define SOC_WDG_UNLOCK()                        \
        do {                                    \
                soc_wdg.cnt.dbyte = 0x20C5;     \
                soc_wdg.cnt.dbyte = 0x28D9;     \
        } while (0)

struct soc_wdg_reg {
        union {
                struct {
                        volatile xwu8_t stop:1;
                        volatile xwu8_t wait:1;
                        volatile xwu8_t dbg:1;
                        volatile xwu8_t tst:2;
                        volatile xwu8_t updata:1;
                        volatile xwu8_t irq:1;
                        volatile xwu8_t ent:1;
                } bit;
                volatile xwu8_t byte;
        } cs1;
        union {
                struct {
                        volatile xwu8_t clk:2;
                        volatile xwu8_t reserved0:2;
                        volatile xwu8_t pres:1;
                        volatile xwu8_t reserved1:1;
                        volatile xwu8_t flg:1;
                        volatile xwu8_t win:1;
                } bit;
                volatile xwu8_t byte;
        } cs2;
        union {
                volatile xwu16_t dbyte;
                struct {
                        volatile xwu8_t low;
                        volatile xwu8_t high;
                } byte;
        } cnt;
        union {
                volatile xwu16_t dbyte;
                struct {
                        volatile xwu8_t low;
                        volatile xwu8_t high;
                } byte;
        } toval;
        union {
                volatile xwu16_t dbyte;
                struct {
                        volatile xwu8_t low;
                        volatile xwu8_t high;
                } byte;
        } win;
};

#define SOC_WDG_BASE    (0x40052000U)
#define soc_wdg         (*((volatile struct soc_wdg_reg *)SOC_WDG_BASE))

#endif /* soc_wdg.h */
