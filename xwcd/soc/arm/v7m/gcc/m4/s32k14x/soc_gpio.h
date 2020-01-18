/**
 * @file
 * @brief S32K14x GPIO
 * @author
 * + 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <www.starsoul.tech>
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

#ifndef __soc_gpio_h__
#define __soc_gpio_h__

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <soc.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      macros       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define SOC_GPIO_PORT_REG_OFFSET                (0x1000U)
#define SOC_GPIO_PIN_REG_OFFSET                 (0x40U)

#define SOC_GPIO_PORT_PCR_DEFAULT_VALUE         (0x0U)

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief SOC GPIO Port Enumerations
 */
enum soc_gpio_port_em {
        SOC_GPIO_PORT_A = 0,
        SOC_GPIO_PORT_B,
        SOC_GPIO_PORT_C,
        SOC_GPIO_PORT_D,
        SOC_GPIO_PORT_E,
        SOC_GPIO_PORT_NUM,
};

/**
 * @brief SOC GPIO Pin Enumerations
 */
enum soc_gpio_pin_em {
        SOC_GPIO_PIN_0 = 0,
        SOC_GPIO_PIN_1,
        SOC_GPIO_PIN_2,
        SOC_GPIO_PIN_3,
        SOC_GPIO_PIN_4,
        SOC_GPIO_PIN_5,
        SOC_GPIO_PIN_6,
        SOC_GPIO_PIN_7,
        SOC_GPIO_PIN_8,
        SOC_GPIO_PIN_9,
        SOC_GPIO_PIN_10,
        SOC_GPIO_PIN_11,
        SOC_GPIO_PIN_12,
        SOC_GPIO_PIN_13,
        SOC_GPIO_PIN_14,
        SOC_GPIO_PIN_15,
        SOC_GPIO_PIN_16,
        SOC_GPIO_PIN_17,
        SOC_GPIO_PIN_18,
        SOC_GPIO_PIN_19,
        SOC_GPIO_PIN_20,
        SOC_GPIO_PIN_21,
        SOC_GPIO_PIN_22,
        SOC_GPIO_PIN_23,
        SOC_GPIO_PIN_24,
        SOC_GPIO_PIN_25,
        SOC_GPIO_PIN_26,
        SOC_GPIO_PIN_27,
        SOC_GPIO_PIN_28,
        SOC_GPIO_PIN_29,
        SOC_GPIO_PIN_30,
        SOC_GPIO_PIN_31,
        SOC_GPIO_PIN_NUM,
};

/**
 * @brief SOC GPIO Pull Select Enumeration
 */
enum soc_gpio_ps_em {
        SOC_GPIO_PS_PD = 0, /**< pull down */
        SOC_GPIO_PS_PU = 1, /**< pull up */
};

/**
 * @brief SOC GPIO Eull Ep/Down Enable Enumeration
 */
enum soc_gpio_pe_em {
        SOC_GPIO_PE_DS = 0, /**< pull up/down is disabled */
        SOC_GPIO_PE_EN = 1, /**< pull up/down is enabled */
};

/**
 * @brief SOC GPIO Passive Filter Enable Enumeration
 */
enum soc_gpio_pfe_em {
        SOC_GPIO_PFE_DS = 0, /**< Passive input filter is disabled */
        SOC_GPIO_PFE_EN = 1, /**< Passive input filter is enabled */
};

/**
 * @brief SOC GPIO Drive Strength Enable Enumeration
 */
enum soc_gpio_dse_em {
        SOC_GPIO_DSE_LOW = 0, /**< Low drive strength */
        SOC_GPIO_DSE_HIGH = 1, /**< High drive strength */
};

/**
 * @brief SOC GPIO Pin Mux Control Enumeration
 */
enum soc_gpio_mux_em {
        SOC_GPIO_MUX_ANALOG = 0, /**< Alternative Mode 0: Analog */
        SOC_GPIO_MUX_GPIO = 1, /**< Alternative Mode 1: GPIO */
        SOC_GPIO_MUX_AM2, /**< Alternative Mode 2 */
        SOC_GPIO_MUX_AM3, /**< Alternative Mode 3 */
        SOC_GPIO_MUX_AM4, /**< Alternative Mode 4 */
        SOC_GPIO_MUX_AM5, /**< Alternative Mode 5 */
        SOC_GPIO_MUX_AM6, /**< Alternative Mode 6 */
        SOC_GPIO_MUX_AM7, /**< Alternative Mode 7 */
};

/**
 * @brief SOC GPIO Digital Filter Clock Source Enumeration
 */
enum soc_gpio_dfcs_em {
        SOC_GPIO_DFCS_BUS = 0, /**< bus clock */
        SOC_GPIO_DFCS_LPO = 1, /**< LPO clock */
};

/**
 * @brief SOC GPIO Output Level Enumeration
 */
enum soc_gpio_out_em {
        SOC_GPIO_OUT_LOW = 0, /**< low level */
        SOC_GPIO_OUT_HIGH = 1, /**< high level */
};

/**
 * @brief SOC GPIO Pin Direction Enumeration
 */
enum soc_gpio_pdd_em {
        SOC_GPIO_PDD_INPUT = 0, /**< Pin is configured as general-purpose input */
        SOC_GPIO_PDD_OUTPUT = 1, /**< Pin is configured as general-purpose output */
};

/**
 * @brief SOC GPIO Port Input Disable Enumeration
 */
enum soc_gpio_pid_em {
        SOC_GPIO_PID_EN = 0, /**< Pin is configured for General Purpose Input */
        SOC_GPIO_PID_DS = 1, /**< Pin is not configured as General Purpose Input */
};

/**
 * @brief SOC GPIO Configurations
 */
struct soc_gpio_portcfg {
        struct {
                union {
                        struct {
                                xwu32_t ps:1; /**< Pull Select */
                                xwu32_t pe:1; /**< Pull Enable */
                                xwu32_t reserved2_3:2;
                                xwu32_t pfe:1; /**< Passive Filter Enable */
                                xwu32_t reserved5:1;
                                xwu32_t dse:1; /**< Drive Strength Enable */
                                xwu32_t reserved7:1;
                                xwu32_t mux:3; /**< Pin Mux Control */
                                xwu32_t reserved11_14:4;
                                xwu32_t lk:1; /**< Lock Register */
                                xwu32_t reserved3:16;
                        } bit; /**< bit access */
                        xwu32_t u32; /**< word access */
                } pcr;
                union {
                        struct {
                                xwu32_t dov:1; /**< Default Output value */
                                xwu32_t pdd:1; /**< Port Data Direction */
                                xwu32_t pid:1; /**< Port Input Disable */
                                xwu32_t reserved3_31:29;
                        } bit; /**< bit access */
                        xwu32_t u32; /**< word access */
                } io;
        } pincfg[SOC_GPIO_PIN_NUM]; /**< pin configurations */
        union {
                struct {
                        xwu32_t pin0:1;
                        xwu32_t pin1:1;
                        xwu32_t pin2:1;
                        xwu32_t pin3:1;
                        xwu32_t pin4:1;
                        xwu32_t pin5:1;
                        xwu32_t pin6:1;
                        xwu32_t pin7:1;
                        xwu32_t pin8:1;
                        xwu32_t pin9:1;
                        xwu32_t pin10:1;
                        xwu32_t pin11:1;
                        xwu32_t pin12:1;
                        xwu32_t pin13:1;
                        xwu32_t pin14:1;
                        xwu32_t pin15:1;
                        xwu32_t pin16:1;
                        xwu32_t pin17:1;
                        xwu32_t pin18:1;
                        xwu32_t pin19:1;
                        xwu32_t pin20:1;
                        xwu32_t pin21:1;
                        xwu32_t pin22:1;
                        xwu32_t pin23:1;
                        xwu32_t pin24:1;
                        xwu32_t pin25:1;
                        xwu32_t pin26:1;
                        xwu32_t pin27:1;
                        xwu32_t pin28:1;
                        xwu32_t pin29:1;
                        xwu32_t pin30:1;
                        xwu32_t pin31:1;
                } bit; /**< bit access */
                xwu32_t u32;
        } dfer; /**< Digital Filter Enable */
        union {
                struct {
                        xwu32_t cs:1; /**< Clock Source */
                } bit; /**< bit access */
                xwu32_t u32; /**< word access */
        } dfcr; /**< Digital Filter clock Register */
        union {
                struct {
                        xwu32_t filt:5; /**< Filter Length */
                        xwu32_t reserved0:27;
                } bit; /**< bit access */
                xwu32_t u32; /**< word access */
        } dfwr; /**< Digital Filter Width Register */
};

#endif /* soc_gpio.h */
