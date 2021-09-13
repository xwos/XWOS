/**
 * @file
 * @brief SOC描述层：GPIO
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

#ifndef __soc_gpio_h__
#define __soc_gpio_h__

#include <xwos/standard.h>
#include <xwos/lib/xwbop.h>
#include <soc.h>

#define SOC_GPIOA_0     (xwu16_t)0
#define SOC_GPIOA_1     (xwu16_t)1
#define SOC_GPIOA_2     (xwu16_t)2
#define SOC_GPIOA_3     (xwu16_t)3
#define SOC_GPIOA_4     (xwu16_t)4
#define SOC_GPIOA_5     (xwu16_t)5
#define SOC_GPIOA_6     (xwu16_t)6
#define SOC_GPIOA_7     (xwu16_t)7
#define SOC_GPIOA_8     (xwu16_t)8
#define SOC_GPIOA_9     (xwu16_t)9
#define SOC_GPIOA_10    (xwu16_t)10
#define SOC_GPIOA_11    (xwu16_t)11
#define SOC_GPIOA_12    (xwu16_t)12
#define SOC_GPIOA_13    (xwu16_t)13
#define SOC_GPIOA_14    (xwu16_t)14
#define SOC_GPIOA_15    (xwu16_t)15

#define SOC_GPIOB_0     (xwu16_t)16
#define SOC_GPIOB_1     (xwu16_t)17
#define SOC_GPIOB_2     (xwu16_t)18
#define SOC_GPIOB_3     (xwu16_t)19
#define SOC_GPIOB_4     (xwu16_t)20
#define SOC_GPIOB_5     (xwu16_t)21
#define SOC_GPIOB_6     (xwu16_t)22
#define SOC_GPIOB_7     (xwu16_t)23
#define SOC_GPIOB_8     (xwu16_t)24
#define SOC_GPIOB_9     (xwu16_t)25
#define SOC_GPIOB_10    (xwu16_t)26
#define SOC_GPIOB_11    (xwu16_t)27
#define SOC_GPIOB_12    (xwu16_t)28
#define SOC_GPIOB_13    (xwu16_t)29
#define SOC_GPIOB_14    (xwu16_t)30
#define SOC_GPIOB_15    (xwu16_t)31

#define SOC_GPIOC_0     (xwu16_t)32
#define SOC_GPIOC_1     (xwu16_t)33
#define SOC_GPIOC_2     (xwu16_t)34
#define SOC_GPIOC_3     (xwu16_t)35
#define SOC_GPIOC_4     (xwu16_t)36
#define SOC_GPIOC_5     (xwu16_t)37
#define SOC_GPIOC_6     (xwu16_t)38
#define SOC_GPIOC_7     (xwu16_t)39
#define SOC_GPIOC_8     (xwu16_t)40
#define SOC_GPIOC_9     (xwu16_t)41
#define SOC_GPIOC_10    (xwu16_t)42
#define SOC_GPIOC_11    (xwu16_t)43
#define SOC_GPIOC_12    (xwu16_t)44
#define SOC_GPIOC_13    (xwu16_t)45
#define SOC_GPIOC_14    (xwu16_t)46
#define SOC_GPIOC_15    (xwu16_t)47

#define SOC_GPIOD_0     (xwu16_t)48
#define SOC_GPIOD_1     (xwu16_t)49
#define SOC_GPIOD_2     (xwu16_t)50
#define SOC_GPIOD_3     (xwu16_t)51
#define SOC_GPIOD_4     (xwu16_t)52
#define SOC_GPIOD_5     (xwu16_t)53
#define SOC_GPIOD_6     (xwu16_t)54
#define SOC_GPIOD_7     (xwu16_t)55
#define SOC_GPIOD_8     (xwu16_t)56
#define SOC_GPIOD_9     (xwu16_t)57
#define SOC_GPIOD_10    (xwu16_t)58
#define SOC_GPIOD_11    (xwu16_t)59
#define SOC_GPIOD_12    (xwu16_t)60
#define SOC_GPIOD_13    (xwu16_t)61
#define SOC_GPIOD_14    (xwu16_t)62
#define SOC_GPIOD_15    (xwu16_t)63

#define SOC_GPIOE_0     (xwu16_t)64
#define SOC_GPIOE_1     (xwu16_t)65
#define SOC_GPIOE_2     (xwu16_t)66
#define SOC_GPIOE_3     (xwu16_t)67
#define SOC_GPIOE_4     (xwu16_t)68
#define SOC_GPIOE_5     (xwu16_t)69
#define SOC_GPIOE_6     (xwu16_t)70
#define SOC_GPIOE_7     (xwu16_t)71
#define SOC_GPIOE_8     (xwu16_t)72
#define SOC_GPIOE_9     (xwu16_t)73
#define SOC_GPIOE_10    (xwu16_t)74
#define SOC_GPIOE_11    (xwu16_t)75
#define SOC_GPIOE_12    (xwu16_t)76
#define SOC_GPIOE_13    (xwu16_t)77
#define SOC_GPIOE_14    (xwu16_t)78
#define SOC_GPIOE_15    (xwu16_t)79

#define SOC_GPIOF_0     (xwu16_t)80
#define SOC_GPIOF_1     (xwu16_t)81
#define SOC_GPIOF_2     (xwu16_t)82
#define SOC_GPIOF_3     (xwu16_t)83
#define SOC_GPIOF_4     (xwu16_t)84
#define SOC_GPIOF_5     (xwu16_t)85
#define SOC_GPIOF_6     (xwu16_t)86
#define SOC_GPIOF_7     (xwu16_t)87
#define SOC_GPIOF_8     (xwu16_t)88
#define SOC_GPIOF_9     (xwu16_t)89
#define SOC_GPIOF_10    (xwu16_t)90
#define SOC_GPIOF_11    (xwu16_t)91
#define SOC_GPIOF_12    (xwu16_t)92
#define SOC_GPIOF_13    (xwu16_t)93
#define SOC_GPIOF_14    (xwu16_t)94
#define SOC_GPIOF_15    (xwu16_t)95

#define SOC_GPIOG_0     (xwu16_t)96
#define SOC_GPIOG_1     (xwu16_t)97
#define SOC_GPIOG_2     (xwu16_t)98
#define SOC_GPIOG_3     (xwu16_t)99
#define SOC_GPIOG_4     (xwu16_t)100
#define SOC_GPIOG_5     (xwu16_t)101
#define SOC_GPIOG_6     (xwu16_t)102
#define SOC_GPIOG_7     (xwu16_t)103
#define SOC_GPIOG_8     (xwu16_t)104
#define SOC_GPIOG_9     (xwu16_t)105
#define SOC_GPIOG_10    (xwu16_t)106
#define SOC_GPIOG_11    (xwu16_t)107
#define SOC_GPIOG_12    (xwu16_t)108
#define SOC_GPIOG_13    (xwu16_t)109
#define SOC_GPIOG_14    (xwu16_t)110
#define SOC_GPIOG_15    (xwu16_t)111

#define SOC_GPIOH_0     (xwu16_t)112
#define SOC_GPIOH_1     (xwu16_t)113
#define SOC_GPIOH_2     (xwu16_t)114
#define SOC_GPIOH_3     (xwu16_t)115
#define SOC_GPIOH_4     (xwu16_t)116
#define SOC_GPIOH_5     (xwu16_t)117
#define SOC_GPIOH_6     (xwu16_t)118
#define SOC_GPIOH_7     (xwu16_t)119
#define SOC_GPIOH_8     (xwu16_t)120
#define SOC_GPIOH_9     (xwu16_t)121
#define SOC_GPIOH_10    (xwu16_t)122

#define SOC_GPIO_PIN_DCFG       ((xwu16_t)(0x0000))

/**
 * @brief soc gpio port enumerations
 */
enum soc_gpio_port_em {
        SOC_GPIO_PORT_A = 0,
        SOC_GPIO_PORT_B,
        SOC_GPIO_PORT_C,
        SOC_GPIO_PORT_D,
        SOC_GPIO_PORT_E,
        SOC_GPIO_PORT_F,
        SOC_GPIO_PORT_G,
        SOC_GPIO_PORT_H,
        SOC_GPIO_PORT_NUM,
};

/**
 * @brief soc gpio pin enumerations
 */
enum soc_gpio_pin_em {
        SOC_GPIO_PIN_0 = 0, /**< Pin 0 */
        SOC_GPIO_PIN_1, /**< Pin 1 */
        SOC_GPIO_PIN_2, /**< Pin 2 */
        SOC_GPIO_PIN_3, /**< Pin 3 */
        SOC_GPIO_PIN_4, /**< Pin 4 */
        SOC_GPIO_PIN_5, /**< Pin 5 */
        SOC_GPIO_PIN_6, /**< Pin 6 */
        SOC_GPIO_PIN_7, /**< Pin 7 */
        SOC_GPIO_PIN_8, /**< Pin 8 */
        SOC_GPIO_PIN_9, /**< Pin 9 */
        SOC_GPIO_PIN_10, /**< Pin 10 */
        SOC_GPIO_PIN_11, /**< Pin 11 */
        SOC_GPIO_PIN_12, /**< Pin 12 */
        SOC_GPIO_PIN_13, /**< Pin 13 */
        SOC_GPIO_PIN_14, /**< Pin 14 */
        SOC_GPIO_PIN_15, /**< Pin 15 */
        SOC_GPIO_PIN_NUM,
};

/**
 * @brief SOC_GPIO safe mode control enumeration
 */
enum soc_gpio_smc_em {
        SOC_GPIO_SMC_OBD = 0, /**< In safe mode, the output buffer of the pad is disabled */
        SOC_GPIO_SMC_OBE = 1, /**< In safe mode, the output buffer of the pad is enabled */
};

/**
 * @brief SOC_GPIO analog pad control enumeration
 */
enum soc_gpio_apc_em {
        SOC_GPIO_APC_DS = 0, /**< Analog input path from the pad is gated and cannot be used */
        SOC_GPIO_APC_EN = 1, /**< Analog input path switch can be enabled by the ADC */
};

/**
 * @brief SOC_GPIO pad output assignment enumeration
 */
enum soc_gpio_pa_em {
        SOC_GPIO_PA_GPIO = 0, /**< Alternative Mode 0: GPIO */
        SOC_GPIO_PA_AM1 = 1, /**< Alternative Mode 1 */
        SOC_GPIO_PA_AM2 = 2, /**< Alternative Mode 2 */
        SOC_GPIO_PA_AM3 = 3, /**< Alternative Mode 3 */
};

/**
 * @brief SOC_GPIO output buffer enable enumeration
 */
enum soc_gpio_obe_em {
        SOC_GPIO_OBE_DS = 0, /**< output buffer is disabled */
        SOC_GPIO_OBE_EN = 1, /**< output buffer is enabled */
};

/**
 * @brief SOC_GPIO input buffer enable enumeration
 */
enum soc_gpio_ibe_em {
        SOC_GPIO_IBE_DS = 0, /**< input buffer is disabled */
        SOC_GPIO_IBE_EN = 1, /**< input buffer is enabled */
};

/**
 * @brief SOC_GPIO open drain output enable enumeration
 */
enum soc_gpio_ode_em {
        SOC_GPIO_ODE_DS = 0, /**< open drain output is disabled */
        SOC_GPIO_ODE_EN = 1, /**< open drain output is enabled */
};

/**
 * @brief SOC_GPIO pad Slew Rate Control enumeration
 */
enum soc_gpio_src_em {
        SOC_GPIO_SRC_SLOW = 0, /**< Pad configured as slow */
        SOC_GPIO_SRC_FAST = 1, /**< Pad is configured as medium or fast */
};

/**
 * @brief SOC_GPIO weak pull up/down enable enumeration
 */
enum soc_gpio_wpe_em {
        SOC_GPIO_WPE_DS = 0, /**< weak pull up/down is disabled */
        SOC_GPIO_WPE_EN = 1, /**< weak pull up/down is enabled */
};

/**
 * @brief SOC_GPIO weak pull up/down selection enumeration
 */
enum soc_gpio_wps_em {
        SOC_GPIO_WPS_PD = 0, /**< weak pull down */
        SOC_GPIO_WPS_PU = 1, /**< weak pull up */
};

/**
 * @brief SOC_GPIO output level enumeration
 */
enum soc_gpio_out_em {
        SOC_GPIO_OUT_LOW = 0, /**< low level */
        SOC_GPIO_OUT_HIGH = 1, /**< high level */
};

/**
 * @brief SOC GPIO configurations
 */
struct soc_gpio_pincfg {
        union {
                struct {
                        xwu16_t reserved0:1;
                        xwu16_t smc:1; /**< Safe Mode Control */
                        xwu16_t apc:1; /**< Analog Pad Control */
                        xwu16_t reserved1:1;
                        xwu16_t pa:2; /**< Alternative Mode Control */
                        xwu16_t obe:1; /* Output Buffer Enable */
                        xwu16_t ibe:1; /* Input Buffer Enable */
                        xwu16_t reserved2:2;
                        xwu16_t ode:1; /* Open Drain Output Enable */
                        xwu16_t reserved3:2;
                        xwu16_t src:1; /* Slew Rate Control */
                        xwu16_t wpe:1; /* Weak Pull Up/Down Enable */
                        xwu16_t wps:1; /* Weak Pull Up/Down Select */
                } bit; /**< bit access */
                xwu16_t u16; /**< half-word access */
        } cfg;
        xwu8_t dout; /**< default output level */
};

#endif /* soc_gpio.h */
