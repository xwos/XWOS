/**
 * @file
 * @brief xwmd设备栈：I2C主机
 * @author
 * + 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __xwmd_ds_i2c_master_h__
#define __xwmd_ds_i2c_master_h__

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <xwos/lib/xwbop.h>
#include <xwos/osal/lock/mutex.h>
#include <xwmd/ds/device.h>
#include <xwmd/ds/i2c/common.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief I2C主机配置
 */
struct xwds_i2cm_cfg {
        xwu32_t speed; /**< 总线速度（单位：HZ） */
        void * xwccfg; /**< SOC芯片私有配置 */
};

struct xwds_i2cm;

/**
 * @brief BSP中需要提供的I2C主机驱动函数表
 */
struct xwds_i2cm_driver {
        struct xwds_driver base; /**< C语言面向对象：继承struct xwds_driver */
        xwer_t (* xfer)(struct xwds_i2cm *,
                        struct xwds_i2c_msg *,
                        xwtm_t *); /**< 传输 */
        xwer_t (* reset)(struct xwds_i2cm *, xwtm_t *); /**< 复位 */
};

/**
 * @brief I2C主机
 */
struct xwds_i2cm {
        struct xwds_device dev; /**< C语言面向对象：继承struct xwds_device */

        /* attributes */
        const struct xwds_i2cm_cfg * cfg; /**< 配置 */

        /* private */
        struct xwosal_mtx xferlock; /**< 多线程安全访问的互斥锁 */
};

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         function prototypes         ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/******** ******** ******** constructor & destructor ******** ******** ********/
#if !defined(XWMDCFG_ds_NANO) || (1 != XWMDCFG_ds_NANO)
__xwds_code
void xwds_i2cm_construct(struct xwds_i2cm * i2cm);

__xwds_code
void xwds_i2cm_destruct(struct xwds_i2cm * i2cm);
#endif /* !XWMDCFG_ds_NANO */

/******** ******** ******** APIs ******** ******** ********/
#if defined(XWMDCFG_ds_NANO) && (1 == XWMDCFG_ds_NANO)
__xwds_api
xwer_t xwds_i2cm_probe(struct xwds_i2cm * i2cm);

__xwds_api
xwer_t xwds_i2cm_remove(struct xwds_i2cm * i2cm);

__xwds_api
xwer_t xwds_i2cm_start(struct xwds_i2cm * i2cm);

__xwds_api
xwer_t xwds_i2cm_stop(struct xwds_i2cm * i2cm);

#if defined(XWMDCFG_ds_LPM) && (1 == XWMDCFG_ds_LPM)
__xwds_api
xwer_t xwds_i2cm_suspend(struct xwds_i2cm * i2cm);

__xwds_api
xwer_t xwds_i2cm_resume(struct xwds_i2cm * i2cm);
#endif /* XWMDCFG_ds_LPM */
#endif /* XWMDCFG_ds_NANO */

__xwds_api
xwer_t xwds_i2cm_xfer(struct xwds_i2cm * i2cm, struct xwds_i2c_msg * msg,
                      xwtm_t * xwtm);

__xwds_api
xwer_t xwds_i2cm_reset(struct xwds_i2cm * i2cm, xwtm_t * xwtm);

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********   inline function implementations   ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#if !defined(XWMDCFG_ds_NANO) || (1 != XWMDCFG_ds_NANO)
/**
 * @brief 增加对象的引用计数
 * @param i2cm: (I) I2C主机控制器对象指针
 * @return 错误码
 * @retval @ref xwds_device_grab()
 */
static __xwds_inline
xwer_t xwds_i2cm_grab(struct xwds_i2cm * i2cm)
{
        return xwds_device_grab(&i2cm->dev);
}

/**
 * @brief 减少对象的引用计数
 * @param i2cm: (I) I2C主机控制器对象指针
 * @return 错误码
 * @retval @ref xwds_device_put()
 */
static __xwds_inline
xwer_t xwds_i2cm_put(struct xwds_i2cm * i2cm)
{
        return xwds_device_put(&i2cm->dev);
}

/**
 * @brief 增加设备运行状态计数器
 * @param i2cm: (I) I2C主机控制器对象指针
 * @return 错误码
 * @retval @ref xwds_device_request()
 */
static __xwds_inline
xwer_t xwds_i2cm_request(struct xwds_i2cm * i2cm)
{
        return xwds_device_request(&i2cm->dev);
}

/**
 * @brief 减少设备运行状态计数器
 * @param i2cm: (I) I2C主机控制器对象指针
 * @return 错误码
 * @retval @ref xwds_device_release()
 */
static __xwds_inline
xwer_t xwds_i2cm_release(struct xwds_i2cm * i2cm)
{
        return xwds_device_release(&i2cm->dev);
}
#endif /* !XWMDCFG_ds_NANO */

#endif /* xwmd/ds/i2c/master.h */
