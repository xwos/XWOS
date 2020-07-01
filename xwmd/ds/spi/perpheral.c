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

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwmd/ds/standard.h>
#include <xwos/osal/scheduler.h>
#include <xwmd/ds/spi/perpheral.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********     static function prototypes      ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
static __xwds_vop
xwer_t xwds_spip_cvop_probe(struct xwds_spip * spip);

static __xwds_vop
xwer_t xwds_spip_cvop_remove(struct xwds_spip * spip);

static __xwds_vop
xwer_t xwds_spip_cvop_start(struct xwds_spip * spip);

static __xwds_vop
xwer_t xwds_spip_cvop_stop(struct xwds_spip * spip);

#if defined(XWMDCFG_ds_PM) && (1 == XWMDCFG_ds_PM)
static __xwds_vop
xwer_t xwds_spip_cvop_suspend(struct xwds_spip * spip);

static __xwds_vop
xwer_t xwds_spip_cvop_resume(struct xwds_spip * spip);
#endif /* XWMDCFG_ds_PM */

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
__xwds_rodata const struct xwds_base_virtual_operations xwds_spip_cvops = {
        .probe = (void *)xwds_spip_cvop_probe,
        .remove = (void *)xwds_spip_cvop_remove,
        .start = (void *)xwds_spip_cvop_start,
        .stop = (void *)xwds_spip_cvop_stop,
#if defined(XWMDCFG_ds_PM) && (1 == XWMDCFG_ds_PM)
        .suspend = (void *)xwds_spip_cvop_suspend,
        .resume = (void *)xwds_spip_cvop_resume,
#endif /* XWMDCFG_ds_PM */
};

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/******** ******** ******** constructor & destructor ******** ******** ********/
/**
 * @brief XWDS API：SPI外设控制器对象的构造函数
 * @param spip: (I) SPI外设控制器对象指针
 */
__xwds_api
void xwds_spip_construct(struct xwds_spip * spip)
{
        xwds_device_construct(&spip->dev);
        spip->dev.cvops = &xwds_spip_cvops;
}

/**
 * @brief XWDS API：SPI外设控制器对象的析构函数
 * @param spip: (I) SPI外设控制器对象指针
 */
__xwds_api
void xwds_spip_destruct(struct xwds_spip * spip)
{
        xwds_device_destruct(&spip->dev);
}

/******** ******** base virtual operations ******** ********/
/**
 * @brief XWDS VOP：探测设备
 * @param spip: (I) SPI外设控制器对象指针
 * @return 错误码
 */
static __xwds_vop
xwer_t xwds_spip_cvop_probe(struct xwds_spip * spip)
{
        xwer_t rc;

        if (!is_err_or_null(spip->bus)) {
                rc = xwds_spim_grab(spip->bus);
        } else {
                rc = -EHOSTUNREACH;
        }
        if (__unlikely(rc < 0)) {
                goto err_spim_grab;
        }

        rc = xwds_device_cvop_probe(&spip->dev);
        if (__unlikely(rc < 0)) {
                goto err_dev_cvop_probe;
        }
        return OK;

err_dev_cvop_probe:
        xwds_spim_put(spip->bus);
err_spim_grab:
        return rc;
}

/**
 * @brief XWDS VOP：删除设备
 * @param spip: (I) SPI外设控制器对象指针
 * @return 错误码
 */
static __xwds_vop
xwer_t xwds_spip_cvop_remove(struct xwds_spip * spip)
{
        xwer_t rc;

        XWDS_VALIDATE(spip->bus, "nullptr", -EHOSTUNREACH);

        rc = xwds_device_cvop_remove(&spip->dev);
        if (__unlikely(rc < 0)) {
                goto err_dev_cvop_remove;
        }

        xwds_spim_put(spip->bus);
        return OK;

err_dev_cvop_remove:
        return rc;
}

/**
 * @brief XWDS VOP：启动设备
 * @param spip: (I) SPI外设控制器对象指针
 * @return 错误码
 */
static __xwds_vop
xwer_t xwds_spip_cvop_start(struct xwds_spip * spip)
{
        xwer_t rc;

        if (spip->bus) {
                rc = xwds_spim_request(spip->bus);
        } else {
                rc = -EHOSTUNREACH;
        }
        if (__unlikely(rc < 0)) {
                goto err_spim_request;
        }

        rc = xwds_device_cvop_start(&spip->dev);
        if (__unlikely(rc < 0)) {
                goto err_dev_cvop_start;
        }
        return OK;

err_dev_cvop_start:
        xwds_spim_release(spip->bus);
err_spim_request:
        return rc;
}

/**
 * @brief XWDS VOP：停止设备
 * @param spip: (I) SPI外设控制器对象指针
 * @return 错误码
 */
static __xwds_vop
xwer_t xwds_spip_cvop_stop(struct xwds_spip * spip)
{
        xwer_t rc;

        XWDS_VALIDATE(spip->bus, "nullptr", -EHOSTUNREACH);

        rc = xwds_device_cvop_stop(&spip->dev);
        if (__unlikely(rc < 0)) {
                goto err_dev_cvop_stop;
        }

        xwds_spim_release(spip->bus);
        return OK;

err_dev_cvop_stop:
        return rc;
}

#if (defined(XWMDCFG_ds_PM)) && (1 == XWMDCFG_ds_PM)
/******** ******** pm ******** ********/
/**
 * @brief XWDS VOP：暂停设备
 * @param spip: (I) SPI外设控制器对象指针
 * @return 错误码
 */
static __xwds_vop
xwer_t xwds_spip_cvop_suspend(struct xwds_spip * spip)
{
        xwer_t rc;

        XWDS_VALIDATE(spip->bus, "nullptr", -EHOSTUNREACH);

        rc = xwds_device_cvop_suspend(&spip->dev);
        if (__unlikely(rc < 0)) {
                goto err_dev_cvop_suspend;
        }

        xwds_spim_release(spip->bus);
        return OK;

err_dev_cvop_suspend:
        return rc;
}

/**
 * @brief XWDS VOP：继续设备
 * @param spip: (I) SPI外设控制器对象指针
 * @return 错误码
 */
static __xwds_vop
xwer_t xwds_spip_cvop_resume(struct xwds_spip * spip)
{
        xwer_t rc;

        if (spip->bus) {
                rc = xwds_spim_request(spip->bus);
        } else {
                rc = -EHOSTUNREACH;
        }
        if (__unlikely(rc < 0)) {
                goto err_spim_request;
        }

        rc = xwds_device_cvop_resume(&spip->dev);
        if (__unlikely(rc < 0)) {
                goto err_dev_cvop_resume;
        }
        return OK;

err_dev_cvop_resume:
        xwds_spim_release(spip->bus);
err_spim_request:
        return rc;
}
#endif /* XWMDCFG_ds_PM */

/******** ******** ******** SPI Perpheral APIs ******** ******** ********/
__xwds_api
xwer_t xwds_spip_ioctl(struct xwds_spip * spip, xwsq_t cmd, ...)
{
        xwer_t rc;
        va_list args;
        const struct xwds_spip_driver * drv;

        XWDS_VALIDATE(spip, "nullptr", -EFAULT);

        rc = xwds_spip_grab(spip);
        if (__unlikely(rc < 0)) {
                goto err_spip_grab;
        }
        rc = xwds_spip_request(spip);
        if (__unlikely(rc < 0)) {
                goto err_spip_request;
        }

        va_start(args, cmd);
        drv = xwds_static_cast(const struct xwds_spip_driver *, spip->dev.drv);
        if (drv->ioctl) {
                rc = drv->ioctl(spip, cmd, args);
        } else {
                rc = -ENOSYS;
        }
        va_end(args);
        if (__unlikely(rc < 0)) {
                goto err_drv_ioctl;
        }

        xwds_spip_release(spip);
        xwds_spip_put(spip);

        return OK;

err_drv_ioctl:
        xwds_spip_release(spip);
err_spip_request:
        xwds_spip_put(spip);
err_spip_grab:
        return rc;
}
