/**
 * @file
 * @brief xwmd设备栈：MISC设备
 * @author
 * + 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __xwmd_ds_misc_chip_h__
#define __xwmd_ds_misc_chip_h__

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
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
#if !defined(XWMDCFG_ds_NANO) || (1 != XWMDCFG_ds_NANO)
__xwds_code
void xwds_misc_construct(struct xwds_misc * misc);

__xwds_code
void xwds_misc_destruct(struct xwds_misc * misc);
#endif /* !XWMDCFG_ds_NANO */

/******** ******** ******** APIs ******** ******** ********/
#if defined(XWMDCFG_ds_NANO) && (1 == XWMDCFG_ds_NANO)
__xwds_api
xwer_t xwds_misc_probe(struct xwds_misc * misc);

__xwds_api
xwer_t xwds_misc_remove(struct xwds_misc * misc);

__xwds_api
xwer_t xwds_misc_start(struct xwds_misc * misc);

__xwds_api
xwer_t xwds_misc_stop(struct xwds_misc * misc);

#if defined(XWMDCFG_ds_LPM) && (1 == XWMDCFG_ds_LPM)
__xwds_api
xwer_t xwds_misc_suspend(struct xwds_misc * misc);

__xwds_api
xwer_t xwds_misc_resume(struct xwds_misc * misc);
#endif /* XWMDCFG_ds_LPM */
#endif /* XWMDCFG_ds_NANO */

__xwds_api
xwer_t xwds_misc_ioctl(struct xwds_misc * misc, xwsq_t cmd, ...);

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********   inline function implementations   ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#if !defined(XWMDCFG_ds_NANO) || (1 != XWMDCFG_ds_NANO)
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

#endif /* !XWMDCFG_ds_NANO */

#endif /* xwmd/ds/misc/chip.h */
