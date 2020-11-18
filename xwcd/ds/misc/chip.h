/**
 * @file
 * @brief 玄武设备栈：MISC设备
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

#ifndef __xwcd_ds_misc_chip_h__
#define __xwcd_ds_misc_chip_h__

#include <xwcd/ds/standard.h>
#include <xwcd/ds/device.h>

struct xwds_misc;

/**
 * @brief BSP中需要提供的MISC设备驱动函数表
 */
struct xwds_misc_driver {
        struct xwds_driver base; /**< C语言面向对象：继承struct xwds_driver */
        xwer_t (* ioctl)(struct xwds_misc *, xwsq_t, va_list);
};

/**
 * @brief MISC设备
 */
struct xwds_misc {
        struct xwds_device dev; /**< 继承struct xwds_device */

        /* attributes */
        const void * misccfg; /**< 配置 */
};

void xwds_misc_construct(struct xwds_misc * misc);
void xwds_misc_destruct(struct xwds_misc * misc);
xwer_t xwds_misc_grab(struct xwds_misc * misc);
xwer_t xwds_misc_put(struct xwds_misc * misc);
xwer_t xwds_misc_request(struct xwds_misc * misc);
xwer_t xwds_misc_release(struct xwds_misc * misc);
xwer_t xwds_misc_ioctl(struct xwds_misc * misc, xwsq_t cmd, ...);

#endif /* xwcd/ds/misc/chip.h */
