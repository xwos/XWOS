/**
 * @file
 * @brief 玄武设备栈：I2C IO扩展芯片：TCA9539
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

#ifndef __xwcd_peripheral_i2c_iochip_tca9539_device_h__
#define __xwcd_peripheral_i2c_iochip_tca9539_device_h__

#include <xwos/standard.h>
#include <xwcd/ds/i2c/peripheral.h>
#include <xwcd/ds/iochip/chip.h>

/**
 * @defgroup xwcd_peripheral_i2c_iochip_tca9539 TCA9539
 * @ingroup xwcd_peripheral_i2c_iochip
 * @{
 */

#define XWDS_TCA9539_REG_P0_INPUT       ((xwu8_t)0)
#define XWDS_TCA9539_REG_P1_INPUT       ((xwu8_t)1)
#define XWDS_TCA9539_REG_P0_OUTPUT      ((xwu8_t)2)
#define XWDS_TCA9539_REG_P1_OUTPUT      ((xwu8_t)3)
#define XWDS_TCA9539_REG_P0_POLINV      ((xwu8_t)4)
#define XWDS_TCA9539_REG_P1_POLINV      ((xwu8_t)5)
#define XWDS_TCA9539_REG_P0_CFG         ((xwu8_t)6)
#define XWDS_TCA9539_REG_P1_CFG         ((xwu8_t)7)

/**
 * @brief TCA9539 Pin方向枚举
 */
enum xwds_tca9539_pin_direction_em {
        XWDS_TCA9539_PIN_DIRECTION_OUT = 0U, /**< 输出 */
        XWDS_TCA9539_PIN_DIRECTION_IN, /**< 输入 */
};

/**
 * @brief TCA9539配置
 * @details
 * 用于填充 `xwds_tca9539.iochip.gpio.cfg` 成员。
 */
struct xwds_tca9539_cfg {
        struct {
                union {
                        struct {
                                xwu8_t p0:1;
                                xwu8_t p1:1;
                                xwu8_t p2:1;
                                xwu8_t p3:1;
                                xwu8_t p4:1;
                                xwu8_t p5:1;
                                xwu8_t p6:1;
                                xwu8_t p7:1;
                        } bit;
                        xwu8_t u8;
                } direction; /**< 方向 */
                union {
                        struct {
                                xwu8_t p0:1;
                                xwu8_t p1:1;
                                xwu8_t p2:1;
                                xwu8_t p3:1;
                                xwu8_t p4:1;
                                xwu8_t p5:1;
                                xwu8_t p6:1;
                                xwu8_t p7:1;
                        } bit;
                        xwu8_t u8;
                } out; /**< 初始值 */
        } p0; /**< P0的配置 */
        struct {
                union {
                        struct {
                                xwu8_t p0:1;
                                xwu8_t p1:1;
                                xwu8_t p2:1;
                                xwu8_t p3:1;
                                xwu8_t p4:1;
                                xwu8_t p5:1;
                                xwu8_t p6:1;
                                xwu8_t p7:1;
                        } bit;
                        xwu8_t u8;
                } direction; /**< 方向 */
                union {
                        struct {
                                xwu8_t p0:1;
                                xwu8_t p1:1;
                                xwu8_t p2:1;
                                xwu8_t p3:1;
                                xwu8_t p4:1;
                                xwu8_t p5:1;
                                xwu8_t p6:1;
                                xwu8_t p7:1;
                        } bit;
                        xwu8_t u8;
                } out; /**< 初始值 */
        } p1; /**< P1的配置 */
};

/**
 * @brief TCA9539 PIN配置
 * @details
 * 用于填充 `xwds_iochip_gpio_cfg()` 函数的参数 `cfg` 。
 */
struct xwds_tca9539_pincfg {
        xwu8_t direction;
};

/**
 * @brief TCA9539对象
 */
struct xwds_tca9539 {
        struct xwds_iochip iochip; /**< C语言面向对象：继承 `struct xwds_iochip` */
        struct {
                const struct xwds_resource_gpio * rst; /**< 复位的GPIO */
                const struct xwds_resource_gpio * irq; /**< 向上游SOC通知中断的GPIO */
        } gpiorsc; /**< GPIO资源 */
        xwid_t eirq; /**< 上游SOC的外部中断号 */
};

/**
 * @brief TCA9539 API：TCA9539对象的构造函数
 * @param[in] tca9539: TCA9539对象指针
 */
void xwds_tca9539_construct(struct xwds_tca9539 * tca9539);

/**
 * @brief TCA9539 API：TCA9539对象的析构函数
 * @param[in] tca9539: TCA9539对象指针
 */
void xwds_tca9539_destruct(struct xwds_tca9539 * tca9539);

/**
 * @brief TCA9539 API：增加对象的引用计数
 * @param[in] tca9539: I2C TCA9539对象指针
 * @return 错误码
 * @retval @ref xwds_i2cp_grab()
 */
static __xwds_inline
xwer_t xwds_tca9539_grab(struct xwds_tca9539 * tca9539)
{
        return xwds_i2cp_grab(&tca9539->iochip.bc.i2cp);
}

/**
 * @brief TCA9539 API：减少对象的引用计数
 * @param[in] tca9539: I2C TCA9539对象指针
 * @return 错误码
 * @retval @ref xwds_i2cp_put()
 */
static __xwds_inline
xwer_t xwds_tca9539_put(struct xwds_tca9539 * tca9539)
{
        return xwds_i2cp_put(&tca9539->iochip.bc.i2cp);
}

/**
 * @} xwcd_peripheral_i2c_iochip_tca9539
 */

#endif /* xwcd/peripheral/i2c/iochip/tca9539/device.h */
