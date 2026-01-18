/**
 * @file
 * @brief 玄武设备栈：IO扩展芯片：GPIO
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

#ifndef __xwcd_ds_iochip_gpio_h__
#define __xwcd_ds_iochip_gpio_h__

#include <xwcd/ds/standard.h>
#include <xwcd/ds/iochip/chip.h>

/**
 * @ingroup xwcd_ds_iochip
 * @{
 */

#define XWDS_IOCHIP_GPIO_PORT(p)        (p)

#define XWDS_IOCHIP_GPIO_PIN(n)         (1U << ((xwu32_t)(n)))
#define XWDS_IOCHIP_GPIO_PIN_MASK(n)    (XWDS_IOCHIP_GPIO_PIN(n) - 1U)

/**
 * @brief XWDS IOCHIP GPIO PIN 枚举
 */
enum xwds_iochip_gpio_pin_em {
        XWDS_IOCHIP_GPIO_PIN_0 = XWDS_IOCHIP_GPIO_PIN(0), /**< Pin 0 */
        XWDS_IOCHIP_GPIO_PIN_1 = XWDS_IOCHIP_GPIO_PIN(1), /**< Pin 1 */
        XWDS_IOCHIP_GPIO_PIN_2 = XWDS_IOCHIP_GPIO_PIN(2), /**< Pin 2 */
        XWDS_IOCHIP_GPIO_PIN_3 = XWDS_IOCHIP_GPIO_PIN(3), /**< Pin 3 */
        XWDS_IOCHIP_GPIO_PIN_4 = XWDS_IOCHIP_GPIO_PIN(4), /**< Pin 4 */
        XWDS_IOCHIP_GPIO_PIN_5 = XWDS_IOCHIP_GPIO_PIN(5), /**< Pin 5 */
        XWDS_IOCHIP_GPIO_PIN_6 = XWDS_IOCHIP_GPIO_PIN(6), /**< Pin 6 */
        XWDS_IOCHIP_GPIO_PIN_7 = XWDS_IOCHIP_GPIO_PIN(7), /**< Pin 7 */
        XWDS_IOCHIP_GPIO_PIN_8 = XWDS_IOCHIP_GPIO_PIN(8), /**< Pin 8 */
        XWDS_IOCHIP_GPIO_PIN_9 = XWDS_IOCHIP_GPIO_PIN(9), /**< Pin 9 */
        XWDS_IOCHIP_GPIO_PIN_10 = XWDS_IOCHIP_GPIO_PIN(10), /**< Pin 10 */
        XWDS_IOCHIP_GPIO_PIN_11 = XWDS_IOCHIP_GPIO_PIN(11), /**< Pin 11 */
        XWDS_IOCHIP_GPIO_PIN_12 = XWDS_IOCHIP_GPIO_PIN(12), /**< Pin 12 */
        XWDS_IOCHIP_GPIO_PIN_13 = XWDS_IOCHIP_GPIO_PIN(13), /**< Pin 13 */
        XWDS_IOCHIP_GPIO_PIN_14 = XWDS_IOCHIP_GPIO_PIN(14), /**< Pin 14 */
        XWDS_IOCHIP_GPIO_PIN_15 = XWDS_IOCHIP_GPIO_PIN(15), /**< Pin 15 */
        XWDS_IOCHIP_GPIO_PIN_16 = XWDS_IOCHIP_GPIO_PIN(16), /**< Pin 16 */
        XWDS_IOCHIP_GPIO_PIN_17 = XWDS_IOCHIP_GPIO_PIN(17), /**< Pin 17 */
        XWDS_IOCHIP_GPIO_PIN_18 = XWDS_IOCHIP_GPIO_PIN(18), /**< Pin 18 */
        XWDS_IOCHIP_GPIO_PIN_19 = XWDS_IOCHIP_GPIO_PIN(19), /**< Pin 19 */
        XWDS_IOCHIP_GPIO_PIN_20 = XWDS_IOCHIP_GPIO_PIN(20), /**< Pin 20 */
        XWDS_IOCHIP_GPIO_PIN_21 = XWDS_IOCHIP_GPIO_PIN(21), /**< Pin 21 */
        XWDS_IOCHIP_GPIO_PIN_22 = XWDS_IOCHIP_GPIO_PIN(22), /**< Pin 22 */
        XWDS_IOCHIP_GPIO_PIN_23 = XWDS_IOCHIP_GPIO_PIN(23), /**< Pin 23 */
        XWDS_IOCHIP_GPIO_PIN_24 = XWDS_IOCHIP_GPIO_PIN(24), /**< Pin 24 */
        XWDS_IOCHIP_GPIO_PIN_25 = XWDS_IOCHIP_GPIO_PIN(25), /**< Pin 25 */
        XWDS_IOCHIP_GPIO_PIN_26 = XWDS_IOCHIP_GPIO_PIN(26), /**< Pin 26 */
        XWDS_IOCHIP_GPIO_PIN_27 = XWDS_IOCHIP_GPIO_PIN(27), /**< Pin 27 */
        XWDS_IOCHIP_GPIO_PIN_28 = XWDS_IOCHIP_GPIO_PIN(28), /**< Pin 28 */
        XWDS_IOCHIP_GPIO_PIN_29 = XWDS_IOCHIP_GPIO_PIN(29), /**< Pin 29 */
        XWDS_IOCHIP_GPIO_PIN_30 = XWDS_IOCHIP_GPIO_PIN(30), /**< Pin 30 */
        XWDS_IOCHIP_GPIO_PIN_31 = XWDS_IOCHIP_GPIO_PIN(31), /**< Pin 31 */
};

/**
 * @brief XWDS API：申请IO扩展芯片的GPIO
 * @param[in] iochip: IOCHIP对象指针
 * @param[in] port: GPIO端口
 * @param[in] pinmask: 引脚的掩码，取值 @ref xwds_iochip_gpio_pin_em 中的任意项的或运算
 * @param[in] to: 期望唤醒的时间点
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效指针
 * @retval -ERANGE: GPIO PORT错误
 * @retval -EBUSY: GPIO PIN已被使用
 * @note
 * + 上下文：线程
 */
xwer_t xwds_iochip_gpio_req(struct xwds_iochip * iochip,
                            xwid_t port, xwsq_t pinmask,
                            xwtm_t to);

/**
 * @brief XWDS API：释放IO扩展芯片的GPIO
 * @param[in] iochip: IOCHIP对象指针
 * @param[in] port: GPIO端口
 * @param[in] pinmask: 引脚的掩码，取值 @ref xwds_iochip_gpio_pin_em 中的任意项的或运算
 * @param[in] to: 期望唤醒的时间点
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效指针
 * @retval -ERANGE: GPIO PORT错误
 * @retval -EPERM: GPIO PIN未被申请
 * @note
 * + 上下文：线程
 */
xwer_t xwds_iochip_gpio_rls(struct xwds_iochip * iochip,
                            xwid_t port, xwsq_t pinmask,
                            xwtm_t to);

/**
 * @brief XWDS API：配置IO扩展芯片的GPIO
 * @param[in] iochip: IOCHIP对象指针
 * @param[in] port: GPIO端口
 * @param[in] pinmask: 引脚的掩码，取值 @ref xwds_iochip_gpio_pin_em 中的任意项的或运算
 * @param[in] cfg: GPIO配置，取值依据不同IOCHIP
 * @param[in] to: 期望唤醒的时间点
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效指针
 * @retval -ERANGE: GPIO PORT错误
 * @retval -ENOSYS: 不支持的API
 * @note
 * + 上下文：线程
 */
xwer_t xwds_iochip_gpio_cfg(struct xwds_iochip * iochip,
                            xwid_t port, xwsq_t pinmask, void * cfg,
                            xwtm_t to);

/**
 * @brief XWDS API：将IO扩展芯片的GPIO设置为高电平
 * @param[in] iochip: IOCHIP对象指针
 * @param[in] port: GPIO端口
 * @param[in] pinmask: 引脚的掩码，取值 @ref xwds_iochip_gpio_pin_em 中的任意项的或运算
 * @param[in] to: 期望唤醒的时间点
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效指针
 * @retval -ERANGE: GPIO PORT错误
 * @retval -EPERM: GPIO PIN未被申请
 * @retval -ENOSYS: 不支持的API
 * @note
 * + 上下文：线程
 */
xwer_t xwds_iochip_gpio_set(struct xwds_iochip * iochip,
                            xwid_t port, xwsq_t pinmask,
                            xwtm_t to);

/**
 * @brief XWDS API：将IO扩展芯片的GPIO设置为低电平
 * @param[in] iochip: IOCHIP对象指针
 * @param[in] port: GPIO端口
 * @param[in] pinmask: 引脚的掩码，取值 @ref xwds_iochip_gpio_pin_em 中的任意项的或运算
 * @param[in] to: 期望唤醒的时间点
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效指针
 * @retval -ERANGE: GPIO PORT错误
 * @retval -EPERM: GPIO PIN未被申请
 * @retval -ENOSYS: 不支持的API
 * @note
 * + 上下文：线程
 */
xwer_t xwds_iochip_gpio_reset(struct xwds_iochip * iochip,
                              xwid_t port, xwsq_t pinmask,
                              xwtm_t to);

/**
 * @brief XWDS API：翻转IO扩展芯片的GPIO电平
 * @param[in] iochip: IOCHIP对象指针
 * @param[in] port: GPIO端口
 * @param[in] pinmask: 引脚的掩码，取值 @ref xwds_iochip_gpio_pin_em 中的任意项的或运算
 * @param[in] to: 期望唤醒的时间点
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效指针
 * @retval -ERANGE: GPIO PORT错误
 * @retval -EPERM: GPIO PIN未被申请
 * @retval -ENOSYS: 不支持的API
 * @note
 * + 上下文：线程
 */
xwer_t xwds_iochip_gpio_toggle(struct xwds_iochip * iochip,
                               xwid_t port, xwsq_t pinmask,
                               xwtm_t to);

/**
 * @brief XWDS API：并行输出多个IO扩展芯片的GPIO
 * @param[in] iochip: IOCHIP对象指针
 * @param[in] port: GPIO端口
 * @param[in] pinmask: 引脚的掩码，取值 @ref xwds_iochip_gpio_pin_em 中的任意项的或运算
 * @param[in] out: 输出值，取值 @ref xwds_iochip_gpio_pin_em 中的任意项的或运算，
 *                 只有被pinmask掩码覆盖的部分有效，未覆盖的pin输出不会发生改变。
 * @param[in] to: 期望唤醒的时间点
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效指针
 * @retval -ERANGE: GPIO PORT错误
 * @retval -EPERM: GPIO PIN未被申请
 * @retval -ENOSYS: 不支持的API
 * @note
 * + 上下文：线程
 */
xwer_t xwds_iochip_gpio_output(struct xwds_iochip * iochip,
                               xwid_t port, xwsq_t pinmask, xwsq_t out,
                               xwtm_t to);

/**
 * @brief XWDS API：并行读取多个IO扩展芯片的GPIO
 * @param[in] iochip: IOCHIP对象指针
 * @param[in] port: GPIO端口
 * @param[in] pinmask: 引脚的掩码，取值 @ref xwds_iochip_gpio_pin_em 中的任意项的或运算
 * @param[out] inbuf: 输入缓冲区
 * @param[in] to: 期望唤醒的时间点
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效指针
 * @retval -ERANGE: GPIO PORT错误
 * @retval -EPERM: GPIO PIN未被申请
 * @retval -ENOSYS: 不支持的API
 * @note
 * + 上下文：线程
 */
xwer_t xwds_iochip_gpio_input(struct xwds_iochip * iochip,
                              xwid_t port, xwsq_t pinmask, xwsq_t * inbuf,
                              xwtm_t to);

/**
 * @} xwcd_ds_iochip
 */

#endif /* xwcd/ds/iochip/gpio.h */
