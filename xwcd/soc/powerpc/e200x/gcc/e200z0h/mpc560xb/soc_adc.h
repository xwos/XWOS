/**
 * @file
 * @brief SOC描述层：ADC
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

#ifndef __soc_adc_h__
#define __soc_adc_h__

#include <xwos/standard.h>
#include <soc.h>

/* Overwrite enable tags */
#define SOC_ADC_MCR_OVERWRITE_DS                0
#define SOC_ADC_MCR_OVERWRITE_EN                1

/* Write left/right-aligned tags */
#define SOC_ADC_MCR_WRITE_RIGHT                 0
#define SOC_ADC_MCR_WRITE_LEFT                  1

/* One Shot/Scan mode tags */
#define SOC_ADC_MCR_MODE_ONESHOT                0
#define SOC_ADC_MCR_MODE_SCAN                   1

/* Analog clock select tags */
#define SOC_ADC_MCR_CLKSEL_HALF                 0
#define SOC_ADC_MCR_CLKSEL_FULL                 1

/**
 * @brief mpc560xb ADC private configurations
 */
struct soc_adc_private_cfg {
        union {
                struct {
                        xwu32_t mode:1; /**< One Shot/Scan mode */
                        xwu32_t clock_select:1; /**< Analog clock select */
                        xwu32_t reserved:30;
                } bit; /**< bit access */
                xwu8_t u8; /**< byte access */
        } mcr;
        xwu32_t ncmr[3]; /* Normal Conversion Mask Registers */
        xwu32_t jcmr[3]; /* Injected Conversion Mask Registers */
        union {
                struct {
                        xwu32_t reserved0:16;
                        xwu32_t inplatch:1;
                        xwu32_t reserved1:1;
                        xwu32_t offshift:2;
                        xwu32_t reserved2:1;
                        xwu32_t inpcmp:2;
                        xwu32_t reserved3:1;
                        xwu32_t inpsamp:8;
                } bit; /**< bit access */
                xwu32_t u32; /**< word access */
        } ctr[3]; /* Conversion timing registers */
};

#endif /* soc_adc.h */
