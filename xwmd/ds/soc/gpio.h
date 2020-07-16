/**
 * @file
 * @brief xwmd设备栈：SOC GPIO
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __xwmd_ds_soc_gpio_h__
#define __xwmd_ds_soc_gpio_h__

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwmd/ds/standard.h>
#include <xwos/lib/xwbop.h>
#include <xwmd/ds/soc/chip.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define XWDS_GPIO_PIN_MASK(nr)          (BIT(nr) - 1)

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
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
        XWDS_GPIO_PIN_0 = BIT(0), /**< Pin 0 */
        XWDS_GPIO_PIN_1 = BIT(1), /**< Pin 1 */
        XWDS_GPIO_PIN_2 = BIT(2), /**< Pin 2 */
        XWDS_GPIO_PIN_3 = BIT(3), /**< Pin 3 */
        XWDS_GPIO_PIN_4 = BIT(4), /**< Pin 4 */
        XWDS_GPIO_PIN_5 = BIT(5), /**< Pin 5 */
        XWDS_GPIO_PIN_6 = BIT(6), /**< Pin 6 */
        XWDS_GPIO_PIN_7 = BIT(7), /**< Pin 7 */
        XWDS_GPIO_PIN_8 = BIT(8), /**< Pin 8 */
        XWDS_GPIO_PIN_9 = BIT(9), /**< Pin 9 */
        XWDS_GPIO_PIN_10 = BIT(10), /**< Pin 10 */
        XWDS_GPIO_PIN_11 = BIT(11), /**< Pin 11 */
        XWDS_GPIO_PIN_12 = BIT(12), /**< Pin 12 */
        XWDS_GPIO_PIN_13 = BIT(13), /**< Pin 13 */
        XWDS_GPIO_PIN_14 = BIT(14), /**< Pin 14 */
        XWDS_GPIO_PIN_15 = BIT(15), /**< Pin 15 */
        XWDS_GPIO_PIN_16 = BIT(16), /**< Pin 16 */
        XWDS_GPIO_PIN_17 = BIT(17), /**< Pin 17 */
        XWDS_GPIO_PIN_18 = BIT(18), /**< Pin 18 */
        XWDS_GPIO_PIN_19 = BIT(19), /**< Pin 19 */
        XWDS_GPIO_PIN_20 = BIT(20), /**< Pin 20 */
        XWDS_GPIO_PIN_21 = BIT(21), /**< Pin 21 */
        XWDS_GPIO_PIN_22 = BIT(22), /**< Pin 22 */
        XWDS_GPIO_PIN_23 = BIT(23), /**< Pin 23 */
        XWDS_GPIO_PIN_24 = BIT(24), /**< Pin 24 */
        XWDS_GPIO_PIN_25 = BIT(25), /**< Pin 25 */
        XWDS_GPIO_PIN_26 = BIT(26), /**< Pin 26 */
        XWDS_GPIO_PIN_27 = BIT(27), /**< Pin 27 */
        XWDS_GPIO_PIN_28 = BIT(28), /**< Pin 28 */
        XWDS_GPIO_PIN_29 = BIT(29), /**< Pin 29 */
        XWDS_GPIO_PIN_30 = BIT(30), /**< Pin 30 */
        XWDS_GPIO_PIN_31 = BIT(31), /**< Pin 31 */
};

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         function prototypes         ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief XWDS API：申请GPIO
 * @param soc: (I) SOC对象指针
 * @param port: (I) GPIO端口，取值：@ref xwds_gpio_port_em中的一项
 * @param pinmask: (I) GPIO引脚，取值：@ref xwds_gpio_pin_em中的任意项的组合（或运算）
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效指针
 * @retval -ERANGE: GPIO PORT错误
 * @retval -EBUSY: GPIO PIN已被使用
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
__xwds_api
xwer_t xwds_gpio_req(struct xwds_soc * soc, xwid_t port, xwsq_t pinmask);

/**
 * @brief XWDS API：释放GPIO
 * @param soc: (I) SOC对象指针
 * @param port: (I) GPIO端口，取值：@ref xwds_gpio_port_em中的一项
 * @param pinmask: (I) GPIO引脚，取值：@ref xwds_gpio_pin_em中的任意项的组合（或运算）
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效指针
 * @retval -ERANGE: GPIO PORT错误
 * @retval -EPERM: GPIO PIN未被使用
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
__xwds_api
xwer_t xwds_gpio_rls(struct xwds_soc * soc, xwid_t port, xwsq_t pinmask);

/**
 * @brief XWDS API：配置GPIO
 * @param soc: (I) SOC对象指针
 * @param port: (I) GPIO端口，取值：@ref xwds_gpio_port_em中的一项
 * @param pinmask: (I) GPIO引脚，取值：@ref xwds_gpio_pin_em中的任意项的组合（或运算）
 * @param cfg: (I) GPIO配置，取值：依据不同SOC
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效指针
 * @retval -ERANGE: GPIO PORT错误
 * @retval -ENOSYS: 不支持的API
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
__xwds_api
xwer_t xwds_gpio_cfg(struct xwds_soc * soc, xwid_t port, xwsq_t pinmask, void * cfg);

/**
 * @brief XWDS API：将GPIO的PIN置为高电平
 * @param soc: (I) SOC对象指针
 * @param port: (I) GPIO端口，取值：@ref xwds_gpio_port_em中的一项
 * @param pinmask: (I) GPIO引脚，取值：@ref xwds_gpio_pin_em中的任意项的组合（或运算）
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效指针
 * @retval -ERANGE: GPIO PORT错误
 * @retval -EPERM: GPIO PIN未被使用
 * @retval -ENOSYS: 不支持的API
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
__xwds_api
xwer_t xwds_gpio_set(struct xwds_soc * soc, xwid_t port, xwsq_t pinmask);

/**
 * @brief XWDS API：将GPIO的PIN置为低电平
 * @param soc: (I) SOC对象指针
 * @param port: (I) GPIO端口，取值：@ref xwds_gpio_port_em中的一项
 * @param pinmask: (I) GPIO引脚，取值：@ref xwds_gpio_pin_em中的任意项的组合（或运算）
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效指针
 * @retval -ERANGE: GPIO PORT错误
 * @retval -EPERM: GPIO PIN未被使用
 * @retval -ENOSYS: 不支持的API
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
__xwds_api
xwer_t xwds_gpio_reset(struct xwds_soc * soc, xwid_t port, xwsq_t pinmask);

/**
 * @brief XWDS API：反转GPIO电平
 * @param soc: (I) SOC对象指针
 * @param port: (I) GPIO端口，取值：@ref xwds_gpio_port_em中的一项
 * @param pinmask: (I) GPIO引脚，取值：@ref xwds_gpio_pin_em中的任意项的组合（或运算）
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效指针
 * @retval -ERANGE: GPIO PORT错误
 * @retval -EPERM: GPIO PIN未被使用
 * @retval -ENOSYS: 不支持的API
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
__xwds_api
xwer_t xwds_gpio_toggle(struct xwds_soc * soc, xwid_t port, xwsq_t pinmask);

/**
 * @brief XWDS API：输出GPIO
 * @param soc: (I) SOC对象指针
 * @param port: (I) GPIO端口，取值：@ref xwds_gpio_port_em中的一项
 * @param pinmask: (I) GPIO引脚，取值：@ref xwds_gpio_pin_em中的任意项的组合（或运算）
 * @param out: (I) 输出值，取值：@ref xwds_gpio_pin_em中的任意项的组合（或运算），
 *                 只有被pinmask掩码覆盖的部分有效，未覆盖的pin输出不会发生改变。
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效指针
 * @retval -ERANGE: GPIO PORT错误
 * @retval -EPERM: GPIO PIN未被使用
 * @retval -ENOSYS: 不支持的API
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
__xwds_api
xwer_t xwds_gpio_output(struct xwds_soc * soc,
                        xwid_t port, xwsq_t pinmask,
                        xwsq_t out);

/**
 * @brief XWDS API：从GPIO端口读取输入值
 * @param soc: (I) SOC对象指针
 * @param port: (I) GPIO端口，取值：@ref xwds_gpio_port_em中的一项
 * @param pinmask: (I) GPIO引脚，取值：@ref xwds_gpio_pin_em中的任意项的组合（或运算）
 * @param inbuf: (O) 输入缓冲区
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效指针
 * @retval -ERANGE: GPIO PORT错误
 * @retval -EPERM: GPIO PIN未被使用
 * @retval -ENOSYS: 不支持的API
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
__xwds_api
xwer_t xwds_gpio_input(struct xwds_soc * soc,
                       xwid_t port, xwsq_t pinmask,
                       xwsq_t * inbuf);

__xwds_lib_code
xwer_t xwds_get_gpiorsc(const struct xwds_resource_gpio base[], xwsz_t num,
                        const char * descay[], xwsz_t descnum,
                        const struct xwds_resource_gpio ** ret);

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********  inline functions implementations   ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

#endif /* xwmd/ds/soc/gpio.h */
