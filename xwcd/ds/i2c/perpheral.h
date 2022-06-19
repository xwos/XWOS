/**
 * @file
 * @brief 玄武设备栈：I2C外设
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
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

#ifndef __xwcd_ds_i2c_perpheral_h__
#define __xwcd_ds_i2c_perpheral_h__

#include <xwcd/ds/standard.h>
#include <xwos/lib/xwbop.h>
#include <xwcd/ds/device.h>
#include <xwcd/ds/i2c/common.h>
#include <xwcd/ds/i2c/master.h>

struct xwds_i2cp;

/**
 * @brief BSP中需要提供的I2C外设驱动函数表
 */
struct xwds_i2cp_driver {
        struct xwds_driver base; /**< C语言面向对象：继承struct xwds_driver */
};

/**
 * @brief I2C外设
 */
struct xwds_i2cp {
        struct xwds_device dev; /**< 继承struct xwds_device */

        /* attributes */
        struct xwds_i2cm * bus; /**< I2C总线 */
        xwu16_t addr; /**< 外设地址 */
};

void xwds_i2cp_construct(struct xwds_i2cp * i2cp);
void xwds_i2cp_destruct(struct xwds_i2cp * i2cp);
xwer_t xwds_i2cp_grab(struct xwds_i2cp * i2cp);
xwer_t xwds_i2cp_put(struct xwds_i2cp * i2cp);

#endif /* xwcd/ds/i2c/perpheral.h */
