/**
 * @file
 * @brief xwmd设备栈：SPI外设控制器
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __xwmd_ds_spi_perpheral_h__
#define __xwmd_ds_spi_perpheral_h__

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwmd/ds/standard.h>
#include <xwos/lib/xwbop.h>
#include <xwmd/ds/device.h>
#include <xwmd/ds/spi/common.h>
#include <xwmd/ds/spi/master.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
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
        xwsq_t cs_pin_msk; /**< 片选信号掩码 */
        xwsq_t msg_flags; /**< 消息标志 */
};

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         function prototypes         ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/******** ******** ******** constructor & destructor ******** ******** ********/
__xwds_api
void xwds_spip_construct(struct xwds_spip * spip);

__xwds_api
void xwds_spip_destruct(struct xwds_spip * spip);

/******** ******** ******** APIs ******** ******** ********/
/**
 * @brief XWDS API：SPI外设输入、输出、控制
 * @param spip: (I) SPI外设
 * @param cmd: (I) 命令
 * @param ...: (I) 参数
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效指针
 * @retval -ENOSYS: 不支持的API
 * @note
 * - 同步/异步：依赖于CMD的实现
 * - 上下文：依赖于CMD的实现
 * - 重入性：依赖于CMD的实现
 */
__xwds_api
xwer_t xwds_spip_ioctl(struct xwds_spip * spip, xwsq_t cmd, ...);

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********   inline function implementations   ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief 增加对象的引用计数
 * @param spip: (I) SPI外设控制器对象指针
 * @return 错误码
 * @retval @ref xwds_device_grab()
 */
static __xwds_inline
xwer_t xwds_spip_grab(struct xwds_spip * spip)
{
        return xwds_device_grab(&spip->dev);
}

/**
 * @brief 减少对象的引用计数
 * @param spip: (I) SPI外设控制器对象指针
 * @return 错误码
 * @retval @ref xwds_device_put()
 */
static __xwds_inline
xwer_t xwds_spip_put(struct xwds_spip * spip)
{
        return xwds_device_put(&spip->dev);
}

/**
 * @brief 增加设备运行状态计数器
 * @param spip: (I) SPI外设控制器对象指针
 * @return 错误码
 * @retval @ref xwds_device_request()
 */
static __xwds_inline
xwer_t xwds_spip_request(struct xwds_spip * spip)
{
        return xwds_device_request(&spip->dev);
}

/**
 * @brief 减少设备运行状态计数器
 * @param spip: (I) SPI外设控制器对象指针
 * @return 错误码
 * @retval @ref xwds_device_release()
 */
static __xwds_inline
xwer_t xwds_spip_release(struct xwds_spip * spip)
{
        return xwds_device_release(&spip->dev);
}

#endif /* xwmd/ds/spi/perpheral.h */
