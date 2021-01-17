/**
 * @file
 * @brief 玄武设备栈：SPI外设控制器
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

#ifndef __xwcd_ds_spi_perpheral_h__
#define __xwcd_ds_spi_perpheral_h__

#include <xwcd/ds/standard.h>
#include <xwos/lib/xwbop.h>
#include <xwcd/ds/device.h>
#include <xwcd/ds/spi/master.h>

struct xwds_spip;

/**
 * @breif BSP中需要提供的SPI外设控制器驱动函数表
 */
struct xwds_spip_driver {
        struct xwds_driver base; /**< C语言面向对象：继承struct xwds_driver */
        xwer_t (* ioctl)(struct xwds_spip *, xwsq_t, va_list); /**< 输入/输出/控制 */
};

/**
 * @breif SPI外设控制器
 */
struct xwds_spip {
        struct xwds_device dev; /**< 继承struct xwds_device */

        /* attributes */
        struct xwds_spim * bus; /**< SPI主机控制器总线 */
        xwid_t buscfgid; /**< 总线配置 */
};

void xwds_spip_construct(struct xwds_spip * spip);
void xwds_spip_destruct(struct xwds_spip * spip);
xwer_t xwds_spip_ioctl(struct xwds_spip * spip, xwsq_t cmd, ...);

/**
 * @brief XWDS API：增加对象的引用计数
 * @param spip: (I) SPI外设控制器对象指针
 * @return 错误码
 * @retval @ref xwds_device_grab()
 */
static __xwds_inline_api
xwer_t xwds_spip_grab(struct xwds_spip * spip)
{
        return xwds_device_grab(&spip->dev);
}

/**
 * @brief XWDS API：减少对象的引用计数
 * @param spip: (I) SPI外设控制器对象指针
 * @return 错误码
 * @retval @ref xwds_device_put()
 */
static __xwds_inline_api
xwer_t xwds_spip_put(struct xwds_spip * spip)
{
        return xwds_device_put(&spip->dev);
}

#endif /* xwcd/ds/spi/perpheral.h */
