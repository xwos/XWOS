/**
 * @file
 * @brief 玄武设备栈：SPI：外设
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

#ifndef __xwcd_ds_spi_peripheral_h__
#define __xwcd_ds_spi_peripheral_h__

#include <xwcd/ds/standard.h>
#include <xwos/lib/xwbop.h>
#include <xwcd/ds/device.h>
#include <xwcd/ds/spi/master.h>

/**
 * @defgroup xwcd_ds_spi_peripheral SPI外设
 * @ingroup xwcd_ds_spi
 * @{
 */

struct xwds_spip;

/**
 * @brief BSP中需要提供的SPI外设控制器驱动函数表
 */
struct xwds_spip_driver {
        struct xwds_driver base; /**< C语言面向对象：继承struct xwds_driver */
};

/**
 * @brief SPI外设控制器
 */
struct xwds_spip {
        struct xwds_device dev; /**< 继承struct xwds_device */

        /* attributes */
        struct xwds_spim * bus; /**< SPI主机控制器总线 */
        xwid_t buscfgid; /**< 总线配置 */
};

/**
 * @brief XWDS API：SPI外设控制器对象的构造函数
 * @param[in] spip: SPI外设控制器对象指针
 */
void xwds_spip_construct(struct xwds_spip * spip);

/**
 * @brief XWDS API：SPI外设控制器对象的析构函数
 * @param[in] spip: SPI外设控制器对象指针
 */
void xwds_spip_destruct(struct xwds_spip * spip);

/**
 * @brief XWDS API：增加对象的引用计数
 * @param[in] spip: SPI外设控制器对象指针
 */
xwer_t xwds_spip_grab(struct xwds_spip * spip);

/**
 * @brief XWDS API：减少对象的引用计数
 * @param[in] spip: SPI外设控制器对象指针
 */
xwer_t xwds_spip_put(struct xwds_spip * spip);

/**
 * @} xwcd_ds_spi_peripheral
 */

#endif /* xwcd/ds/spi/peripheral.h */
