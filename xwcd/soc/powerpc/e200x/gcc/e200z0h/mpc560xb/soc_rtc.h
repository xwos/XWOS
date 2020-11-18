/**
 * @file
 * @brief SOC描述层：RTC module
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

#ifndef __soc_rtc_h__
#define __soc_rtc_h__

#include <xwos/standard.h>
#include <soc.h>

/**
 * @brief Counter Enable enumerations
 */
enum soc_rtc_counter_em {
        SOC_RTC_CNTEN_DISABLED = 0,
        SOC_RTC_CNTEN_ENABLED,
};

/**
 * @brief RTC Interrupt Enable enumerations
 */
enum soc_rtc_interrupt_enable_em {
        SOC_RTC_RTCIE_DISABLED = 0,
        SOC_RTC_RTCIE_ENABLED,
};

/**
 * @brief Freeze Enable enumerations
 */
enum soc_rtc_freeze_em {
        SOC_RTC_FRZEN_DISABLED = 0, /**< Counter does not freeze in debug mode */
        SOC_RTC_FRZEN_ENABLED, /**< Counter freezes in debug mode */
};

/**
 * @brief Counter Roll Over Wakeup/Interrupt Enable enumerations
 */
enum soc_rtc_rollover_em {
        SOC_RTC_ROVREN_DISABLED = 0,
        SOC_RTC_ROVREN_ENABLED,
};

/**
 * @brief Autonomous Periodic Interrupt Enable enumerations
 */
enum soc_rtc_api_interrupt_enable_em {
        SOC_RTC_APIEN_DISABLED = 0,
        SOC_RTC_APIEN_ENABLED,
};

/**
 * @brief API Interrupt Enable enumerations
 */
enum soc_rtc_api_interrupt_em {
        SOC_RTC_APIIE_DISABLED = 0,
        SOC_RTC_APIIE_ENABLED,
};

/**
 * @brief Clock Select enumerations
 */
enum soc_rtc_clock_select_em {
        SOC_RTC_CLKSEL_SXOSC = 0,
        SOC_RTC_CLKSEL_SIRC = 1,
        SOC_RTC_CLKSEL_FIRC = 2,
};

/**
 * @brief Divide by 512 enable enumerations
 */
enum soc_rtc_divide512_em {
        SOC_RTC_DIV512EN_DISABLED = 0,
        SOC_RTC_DIV512EN_ENABLED,
};

/**
 * @brief Divide by 32 enable enumerations
 */
enum soc_rtc_divide32_em {
        SOC_RTC_DIV32EN_DISABLED = 0,
        SOC_RTC_DIV32EN_ENABLED,
};

/**
 * @brief mpc560xb RTC private configurations
 */
struct soc_rtc_private_cfg {
        union {
                struct {
                        xwu32_t reserved:1; /**< reserved (Counter Enable) */
                        xwu32_t rtcie:1; /**< RTC Interrupt Enable */
                        xwu32_t frzen:1; /**< Freeze Enable */
                        xwu32_t rovren:1; /**< Counter Roll Over Wakeup/IRQ Enable */
                        xwu32_t rtcval:12; /**< RTC Compare Value */
                        xwu32_t apien:1; /**< Autonomous Periodic Interrupt Enable */
                        xwu32_t apiie:1; /**< API Interrupt Enable */
                        xwu32_t clksel:2; /**< Clock Select */
                        xwu32_t div512en:1; /**< Divide by 512 enable */
                        xwu32_t div32en:1; /**< Divide by 32 enable */
                        xwu32_t apival:10; /**< API Compare Value */
                } bit; /**< bit access */
                xwu32_t u32; /**< 4-byte access */
        } u;
};

#endif /* soc_rtc.h */
