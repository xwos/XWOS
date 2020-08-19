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

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwmd/ds/standard.h>
#include <xwmd/ds/misc/chip.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********     static function prototypes      ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
static __xwds_vop
xwer_t xwds_misc_cvop_probe(struct xwds_misc * misc);

static __xwds_vop
xwer_t xwds_misc_cvop_remove(struct xwds_misc * misc);

static __xwds_vop
xwer_t xwds_misc_cvop_start(struct xwds_misc * misc);

static __xwds_vop
xwer_t xwds_misc_cvop_stop(struct xwds_misc * misc);

#if defined(XWMDCFG_ds_PM) && (1 == XWMDCFG_ds_PM)
static __xwds_vop
xwer_t xwds_misc_cvop_suspend(struct xwds_misc * misc);

static __xwds_vop
xwer_t xwds_misc_cvop_resume(struct xwds_misc * misc);
#endif /* XWMDCFG_ds_PM */

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
__xwds_rodata const struct xwds_base_virtual_operations xwds_misc_cvops = {
        .probe = (void *)xwds_misc_cvop_probe,
        .remove = (void *)xwds_misc_cvop_remove,
        .start = (void *)xwds_misc_cvop_start,
        .stop = (void *)xwds_misc_cvop_stop,
#if defined(XWMDCFG_ds_PM) && (1 == XWMDCFG_ds_PM)
        .suspend = (void *)xwds_misc_cvop_suspend,
        .resume = (void *)xwds_misc_cvop_resume,
#endif /* XWMDCFG_ds_PM */
};

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/******** ******** ******** constructor & destructor ******** ******** ********/
/**
 * @brief XWDS API：MISC设备的构造函数
 * @param misc: (I) MISC设备对象指针
 */
__xwds_api
void xwds_misc_construct(struct xwds_misc * misc)
{
        xwds_device_construct(&misc->dev);
        misc->dev.cvops = &xwds_misc_cvops;
}

/**
 * @brief XWDS API：MISC设备对象的析构函数
 * @param misc: (I) MISC设备对象指针
 */
__xwds_api
void xwds_misc_destruct(struct xwds_misc * misc)
{
        xwds_device_destruct(&misc->dev);
}

/******** ******** base virtual operations ******** ********/
/**
 * @brief XWDS VOP：探测MISC设备
 * @param misc: (I) MISC设备对象指针
 * @return 错误码
 */
static __xwds_vop
xwer_t xwds_misc_cvop_probe(struct xwds_misc * misc)
{
        xwer_t rc;

        rc = xwds_device_cvop_probe(&misc->dev);
        return rc;
}

/**
 * @brief XWDS VOP：移除MISC设备
 * @param ds: (I) 设备栈控制块指针
 * @param misc: (I) MISC设备对象指针
 * @return 错误码
 */
static __xwds_vop
xwer_t xwds_misc_cvop_remove(struct xwds_misc * misc)
{
        xwer_t rc;

        rc = xwds_device_cvop_remove(&misc->dev);
        return rc;
}

/**
 * @brief XWDS VOP：启动MISC设备
 * @param misc: (I) MISC设备对象指针
 * @return 错误码
 */
static __xwds_vop
xwer_t xwds_misc_cvop_start(struct xwds_misc * misc)
{
        xwer_t rc;

        rc = xwds_device_cvop_start(&misc->dev);
        return rc;
}

/**
 * @brief XWDS VOP：停止MISC设备
 * @param misc: (I) MISC设备对象指针
 * @return 错误码
 */
static __xwds_vop
xwer_t xwds_misc_cvop_stop(struct xwds_misc * misc)
{
        xwer_t rc;

        rc = xwds_device_cvop_stop(&misc->dev);
        return rc;
}

#if defined(XWMDCFG_ds_PM) && (1 == XWMDCFG_ds_PM)
/******** ******** pm ******** ********/
/**
 * @brief XWDS VOP：暂停MISC设备
 * @param misc: (I) MISC设备对象指针
 * @return 错误码
 */
static __xwds_vop
xwer_t xwds_misc_cvop_suspend(struct xwds_misc * misc)
{
        xwer_t rc;

        rc = xwds_device_cvop_suspend(&misc->dev);
        return rc;
}

/**
 * @brief XWDS VOP：继续MISC设备
 * @param misc: (I) MISC设备对象指针
 * @return 错误码
 */
static __xwds_vop
xwer_t xwds_misc_cvop_resume(struct xwds_misc * misc)
{
        xwer_t rc;

        rc = xwds_device_cvop_resume(&misc->dev);
        return rc;
}
#endif /* XWMDCFG_ds_PM */

/******** ******** ******** APIs ******** ******** ********/
__xwds_api
xwer_t xwds_misc_ioctl(struct xwds_misc * misc, xwsq_t cmd, ...)
{
        const struct xwds_misc_driver * drv;
        va_list args;
        xwer_t rc;

        XWDS_VALIDATE(misc, "nullptr", -EFAULT);

        rc = xwds_misc_grab(misc);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_misc_grab;
        }

        va_start(args, cmd);
        drv = xwds_static_cast(const struct xwds_misc_driver *, misc->dev.drv);
        if ((drv) && (drv->ioctl)) {
                rc = drv->ioctl(misc, cmd, args);
        } else {
                rc = -ENOSYS;
        }
        va_end(args);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_drv_ioctl;
        }

        xwds_misc_put(misc);
        return XWOK;

err_drv_ioctl:
        xwds_misc_put(misc);
err_misc_grab:
        return rc;
}
