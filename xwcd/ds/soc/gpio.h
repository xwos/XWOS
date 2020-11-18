/**
 * @file
 * @brief 玄武设备栈：GPIO
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

#ifndef __xwcd_ds_soc_gpio_h__
#define __xwcd_ds_soc_gpio_h__

#include <xwcd/ds/standard.h>
#include <xwcd/ds/soc/chip.h>

#define XWDS_GPIO_PIN(n)                (1U << n)
#define XWDS_GPIO_PIN_MASK(n)           (XWDS_GPIO_PIN(n) - 1U)

/**
 * @brief XWDS GPIO Port 枚举
 */
enum xwds_gpio_port_em {
        XWDS_GPIO_PORT_A = 0, /**< GPIO A */
        XWDS_GPIO_PORT_B, /**< GPIO B */
        XWDS_GPIO_PORT_C, /**< GPIO C */
        XWDS_GPIO_PORT_D, /**< GPIO D */
        XWDS_GPIO_PORT_E, /**< GPIO E */
        XWDS_GPIO_PORT_F, /**< GPIO F */
        XWDS_GPIO_PORT_G, /**< GPIO G */
        XWDS_GPIO_PORT_H, /**< GPIO H */
        XWDS_GPIO_PORT_I, /**< GPIO I */
        XWDS_GPIO_PORT_J, /**< GPIO J */
        XWDS_GPIO_PORT_K, /**< GPIO K */
        XWDS_GPIO_PORT_L, /**< GPIO L */
        XWDS_GPIO_PORT_M, /**< GPIO M */
        XWDS_GPIO_PORT_N, /**< GPIO N */
        XWDS_GPIO_PORT_O, /**< GPIO O */
        XWDS_GPIO_PORT_P, /**< GPIO P */
        XWDS_GPIO_PORT_Q, /**< GPIO Q */
        XWDS_GPIO_PORT_R, /**< GPIO R */
        XWDS_GPIO_PORT_S, /**< GPIO S */
        XWDS_GPIO_PORT_T, /**< GPIO T */
        XWDS_GPIO_PORT_U, /**< GPIO U */
        XWDS_GPIO_PORT_V, /**< GPIO V */
        XWDS_GPIO_PORT_W, /**< GPIO W */
        XWDS_GPIO_PORT_X, /**< GPIO X */
        XWDS_GPIO_PORT_Y, /**< GPIO Y */
        XWDS_GPIO_PORT_Z, /**< GPIO Z */
};

/**
 * @brief XWDS GPIO Pin 枚举
 */
enum xwds_gpio_pin_em {
        XWDS_GPIO_PIN_0 = XWDS_GPIO_PIN(0), /**< Pin 0 */
        XWDS_GPIO_PIN_1 = XWDS_GPIO_PIN(1), /**< Pin 1 */
        XWDS_GPIO_PIN_2 = XWDS_GPIO_PIN(2), /**< Pin 2 */
        XWDS_GPIO_PIN_3 = XWDS_GPIO_PIN(3), /**< Pin 3 */
        XWDS_GPIO_PIN_4 = XWDS_GPIO_PIN(4), /**< Pin 4 */
        XWDS_GPIO_PIN_5 = XWDS_GPIO_PIN(5), /**< Pin 5 */
        XWDS_GPIO_PIN_6 = XWDS_GPIO_PIN(6), /**< Pin 6 */
        XWDS_GPIO_PIN_7 = XWDS_GPIO_PIN(7), /**< Pin 7 */
        XWDS_GPIO_PIN_8 = XWDS_GPIO_PIN(8), /**< Pin 8 */
        XWDS_GPIO_PIN_9 = XWDS_GPIO_PIN(9), /**< Pin 9 */
        XWDS_GPIO_PIN_10 = XWDS_GPIO_PIN(10), /**< Pin 10 */
        XWDS_GPIO_PIN_11 = XWDS_GPIO_PIN(11), /**< Pin 11 */
        XWDS_GPIO_PIN_12 = XWDS_GPIO_PIN(12), /**< Pin 12 */
        XWDS_GPIO_PIN_13 = XWDS_GPIO_PIN(13), /**< Pin 13 */
        XWDS_GPIO_PIN_14 = XWDS_GPIO_PIN(14), /**< Pin 14 */
        XWDS_GPIO_PIN_15 = XWDS_GPIO_PIN(15), /**< Pin 15 */
        XWDS_GPIO_PIN_16 = XWDS_GPIO_PIN(16), /**< Pin 16 */
        XWDS_GPIO_PIN_17 = XWDS_GPIO_PIN(17), /**< Pin 17 */
        XWDS_GPIO_PIN_18 = XWDS_GPIO_PIN(18), /**< Pin 18 */
        XWDS_GPIO_PIN_19 = XWDS_GPIO_PIN(19), /**< Pin 19 */
        XWDS_GPIO_PIN_20 = XWDS_GPIO_PIN(20), /**< Pin 20 */
        XWDS_GPIO_PIN_21 = XWDS_GPIO_PIN(21), /**< Pin 21 */
        XWDS_GPIO_PIN_22 = XWDS_GPIO_PIN(22), /**< Pin 22 */
        XWDS_GPIO_PIN_23 = XWDS_GPIO_PIN(23), /**< Pin 23 */
        XWDS_GPIO_PIN_24 = XWDS_GPIO_PIN(24), /**< Pin 24 */
        XWDS_GPIO_PIN_25 = XWDS_GPIO_PIN(25), /**< Pin 25 */
        XWDS_GPIO_PIN_26 = XWDS_GPIO_PIN(26), /**< Pin 26 */
        XWDS_GPIO_PIN_27 = XWDS_GPIO_PIN(27), /**< Pin 27 */
        XWDS_GPIO_PIN_28 = XWDS_GPIO_PIN(28), /**< Pin 28 */
        XWDS_GPIO_PIN_29 = XWDS_GPIO_PIN(29), /**< Pin 29 */
        XWDS_GPIO_PIN_30 = XWDS_GPIO_PIN(30), /**< Pin 30 */
        XWDS_GPIO_PIN_31 = XWDS_GPIO_PIN(31), /**< Pin 31 */
};

xwer_t xwds_gpio_req(struct xwds_soc * soc, xwid_t port, xwsq_t pinmask);
xwer_t xwds_gpio_rls(struct xwds_soc * soc, xwid_t port, xwsq_t pinmask);
xwer_t xwds_gpio_cfg(struct xwds_soc * soc, xwid_t port, xwsq_t pinmask, void * cfg);
xwer_t xwds_gpio_set(struct xwds_soc * soc, xwid_t port, xwsq_t pinmask);
xwer_t xwds_gpio_reset(struct xwds_soc * soc, xwid_t port, xwsq_t pinmask);
xwer_t xwds_gpio_toggle(struct xwds_soc * soc, xwid_t port, xwsq_t pinmask);
xwer_t xwds_gpio_output(struct xwds_soc * soc,
                        xwid_t port, xwsq_t pinmask,
                        xwsq_t out);
xwer_t xwds_gpio_input(struct xwds_soc * soc,
                       xwid_t port, xwsq_t pinmask,
                       xwsq_t * inbuf);

#endif /* xwcd/ds/soc/gpio.h */
