/**
 * @file
 * @brief xwmd设备栈：MISC设备
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __xwmd_ds_misc_chip_h__
#define __xwmd_ds_misc_chip_h__

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwmd/ds/standard.h>
#include <xwmd/ds/device.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
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

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         function prototypes         ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/******** ******** ******** constructor & destructor ******** ******** ********/
__xwds_api
void xwds_misc_construct(struct xwds_misc * misc);

__xwds_api
void xwds_misc_destruct(struct xwds_misc * misc);

/******** ******** ******** APIs ******** ******** ********/
/**
 * @brief XWDS API：MISC设备输入、输出、控制
 * @param misc: (I) MISC设备对象指针
 * @param cmd: (I) 命令
 * @param ...: (I) 参数
 * @return 错误码
 * @retval OK: OK
 * @retval -EFAULT: 无效指针
 * @note
 * - 同步/异步：依赖于CMD的实现
 * - 上下文：依赖于CMD的实现
 * - 重入性：依赖于CMD的实现
 */
__xwds_api
xwer_t xwds_misc_ioctl(struct xwds_misc * misc, xwsq_t cmd, ...);

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********   inline function implementations   ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief 增加对象的引用计数
 * @param misc: (I) MISC设备对象指针
 * @return 错误码
 * @retval @ref xwds_device_grab()
 */
static __xwds_inline
xwer_t xwds_misc_grab(struct xwds_misc * misc)
{
        return xwds_device_grab(&misc->dev);
}

/**
 * @brief 减少对象的引用计数
 * @param misc: (I) MISC设备对象指针
 * @return 错误码
 * @retval @ref xwds_device_put()
 */
static __xwds_inline
xwer_t xwds_misc_put(struct xwds_misc * misc)
{
        return xwds_device_put(&misc->dev);
}

/**
 * @brief 增加设备运行状态计数器
 * @param misc: (I) MISC设备对象指针
 * @return 错误码
 * @retval @ref xwds_device_request()
 */
static __xwds_inline
xwer_t xwds_misc_request(struct xwds_misc * misc)
{
        return xwds_device_request(&misc->dev);
}

/**
 * @brief 减少设备运行状态计数器
 * @param misc: (I) MISC设备对象指针
 * @return 错误码
 * @retval @ref xwds_device_release()
 */
static __xwds_inline
xwer_t xwds_misc_release(struct xwds_misc * misc)
{
        return xwds_device_release(&misc->dev);
}

#endif /* xwmd/ds/misc/chip.h */
