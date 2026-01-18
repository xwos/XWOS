/**
 * @file
 * @brief 玄武设备栈：IO扩展芯片
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

#ifndef __xwcd_ds_iochip_chip_h__
#define __xwcd_ds_iochip_chip_h__

#include <xwcd/ds/standard.h>
#include <xwcd/ds/device.h>
#include <xwcd/ds/i2c/peripheral.h>
#include <xwcd/ds/spi/peripheral.h>

/**
 * @defgroup xwcd_ds_iochip IO扩展芯片
 * @ingroup xwcd_ds
 * @{
 */

struct xwds_iochip;

/**
 * @brief 外部中断函数参数
 */
typedef void * xwds_iochip_eirq_arg_t;

/**
 * @brief 外部中断函数
 */
typedef void (* xwds_iochip_eirq_f)(struct xwds_iochip *,
                                    xwid_t,
                                    xwds_iochip_eirq_arg_t);

/**
 * @brief BSP中需要提供的IO扩展芯片设备驱动函数表
 */
struct xwds_iochip_driver {
        struct xwds_driver base; /**< C语言面向对象：继承struct xwds_driver */

        xwer_t (* gpio_req)(struct xwds_iochip * /*iochip*/,
                            xwid_t /*port*/,
                            xwsq_t /*pinmask*/,
                            xwtm_t /*to*/); /**< 申请GPIO */
        xwer_t (* gpio_rls)(struct xwds_iochip * /*iochip*/,
                            xwid_t /*port*/,
                            xwid_t /*pinmask*/,
                            xwtm_t /*to*/); /**< 释放GPIO */
        xwer_t (* gpio_cfg)(struct xwds_iochip * /*iochip*/,
                            xwid_t /*port*/,
                            xwsq_t /*pinmask*/,
                            void * /*cfg*/,
                            xwtm_t /*to*/); /**< 配置GPIO */
        xwer_t (* gpio_output)(struct xwds_iochip *,
                               xwid_t /*port*/,
                               xwsq_t /*pinmask*/,
                               xwsq_t /*out*/,
                               xwtm_t /*to*/); /**< 输出GPIO */
        xwer_t (* gpio_set)(struct xwds_iochip * /*iochip*/,
                            xwid_t /*port*/,
                            xwsq_t /*pinmask*/,
                            xwtm_t /*to*/); /**< 将GPIO置为高电平 */
        xwer_t (* gpio_reset)(struct xwds_iochip * /*iochip*/,
                              xwid_t /*port*/,
                              xwsq_t /*pinmask*/,
                              xwtm_t /*to*/); /**< 将GPIO置为低电平 */
        xwer_t (* gpio_toggle)(struct xwds_iochip * /*iochip*/,
                               xwid_t /*port*/,
                               xwsq_t /*pinmask*/,
                               xwtm_t /*to*/); /**< 反转GPIO电平*/
        xwer_t (* gpio_input)(struct xwds_iochip * /*iochip*/,
                              xwid_t /*port*/,
                              xwsq_t /*pinmask*/,
                              xwsq_t * /*inbuf*/,
                              xwtm_t /*to*/); /**< 读取GPIO上的输入 */
};

/**
 * @brief IO扩展芯片
 */
struct xwds_iochip {
        union {
                struct xwds_device dev; /**< 继承struct xwds_device */
#if defined(XWCDCFG_ds_I2C_PERIPHERAL) && (1 == XWCDCFG_ds_I2C_PERIPHERAL)
                struct xwds_i2cp i2cp; /**< 继承struct xwds_i2cp */
#endif
#if defined(XWCDCFG_ds_SPI_PERIPHERAL) && (1 == XWCDCFG_ds_SPI_PERIPHERAL)
                struct xwds_spip spip; /**< 继承struct xwds_spip */
#endif
        } bc; /**< 基类(base class) */

        /* attributes */
        struct {
                void * cfg; /**< GPIO配置 */
                atomic_xwsq_t * pins; /**< GPIO PIN状态图数组 */
                xwsz_t port_num; /**< GPIO端口数量 */
                xwsz_t pin_num; /**< 每个GPIO端口中的PIN数量 */
        } gpio; /**< GPIO描述 */
};

/**
 * @brief XWDS API：IOCHIP对象的构造函数
 * @param[in] iochip: IOCHIP对象指针
 */
void xwds_iochip_construct(struct xwds_iochip * iochip);

/**
 * @brief XWDS API：IOCHIP对象的析构函数
 * @param[in] iochip: IOCHIP对象指针
 */
void xwds_iochip_destruct(struct xwds_iochip * iochip);

/**
 * @brief XWDS API：增加对象的引用计数
 * @param[in] iochip: IOCHIP对象指针
 */
xwer_t xwds_iochip_grab(struct xwds_iochip * iochip);

/**
 * @brief XWDS API：减少对象的引用计数
 * @param[in] iochip: IOCHIP对象指针
 */
xwer_t xwds_iochip_put(struct xwds_iochip * iochip);

/**
 * @} xwcd_ds_iochip
 */

#endif /* xwcd/ds/iochip/chip.h */
