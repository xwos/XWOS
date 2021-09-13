/**
 * @file
 * @brief e200z0 core registers
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

#ifndef __arch_e200x_core_h__
#define __arch_e200x_core_h__

#ifndef __ASM__
#  include <xwos/standard.h>
#endif

#ifndef __ASM__
/**
 * @brief program status register
 * @note little-endian
 */
union msr_reg {
        struct {
                xwreg_t:5;
                xwreg_t ucle:1;
                xwreg_t Allocated:1;
                xwreg_t:6;
                xwreg_t we:1;
                xwreg_t ce:1;
                xwreg_t:1;
                xwreg_t ee:1;
                xwreg_t pr:1;
                xwreg_t fp:1;
                xwreg_t me:1;
                xwreg_t fe0:1;
                xwreg_t:1;
                xwreg_t de:1;
                xwreg_t fe1:1;
                xwreg_t:2;
                xwreg_t is:1;
                xwreg_t ds:1;
                xwreg_t:2;
                xwreg_t ri:1;
                xwreg_t:1;
        } b; /**< structure used for bit access */
        xwreg_t w; /**< Used for word access */
};

/**
 * @brief e200x context infomation
 */
struct e200x_context {
        xwstk_t * isr_sp;
        xwstk_t * thd_sp;
        xwptr_t irq_nesting_cnt;
        xwirq_t irqn;
};

xwu32_t lwarx(volatile void * addr);
xwer_t stwcx(volatile void * addr, xwu32_t value);
xwu32_t lwbrx(volatile xwu32_t * addr);
xwu16_t lhbrx(volatile xwu16_t * addr);

#endif

#endif /* e200x_core.h */
