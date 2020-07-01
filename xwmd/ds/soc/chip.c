/**
 * @file
 * @brief xwmd设备栈：SOC设备
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
#include <xwos/lib/string.h>
#include <xwmd/ds/soc/chip.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********     static function prototypes      ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
static __xwds_vop
xwer_t xwds_soc_cvop_probe(struct xwds_soc * soc);

static __xwds_vop
xwer_t xwds_soc_cvop_remove(struct xwds_soc * soc);

static __xwds_vop
xwer_t xwds_soc_cvop_start(struct xwds_soc * soc);

static __xwds_vop
xwer_t xwds_soc_cvop_stop(struct xwds_soc * soc);

#if defined(XWMDCFG_ds_PM) && (1 == XWMDCFG_ds_PM)
static __xwds_vop
xwer_t xwds_soc_cvop_suspend(struct xwds_soc * soc);

static __xwds_vop
xwer_t xwds_soc_cvop_resume(struct xwds_soc * soc);
#endif /* XWMDCFG_ds_PM */

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
__xwds_rodata const struct xwds_base_virtual_operations xwds_soc_cvops = {
        .probe = (void *)xwds_soc_cvop_probe,
        .remove = (void *)xwds_soc_cvop_remove,
        .start = (void *)xwds_soc_cvop_start,
        .stop = (void *)xwds_soc_cvop_stop,
#if defined(XWMDCFG_ds_PM) && (1 == XWMDCFG_ds_PM)
        .suspend = (void *)xwds_soc_cvop_suspend,
        .resume = (void *)xwds_soc_cvop_resume,
#endif /* XWMDCFG_ds_PM */
};

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/******** ******** ******** constructor & destructor ******** ******** ********/
/**
 * @brief XWDS API：SOC构造函数
 * @param soc: (I) SOC对象指针
 */
__xwds_api
void xwds_soc_construct(struct xwds_soc * soc)
{
        xwds_device_construct(&soc->dev);
        soc->dev.cvops = &xwds_soc_cvops;
}

/**
 * @brief XWDS API：SOC对象的析构函数
 * @param soc: (I) SOC对象指针
 */
__xwds_api
void xwds_soc_destruct(struct xwds_soc * soc)
{
        xwds_device_destruct(&soc->dev);
}

/******** ******** base virtual operations ******** ********/
/**
 * @brief XWDS VOP：探测SOC
 * @param soc: (I) SOC对象指针
 * @return 错误码
 */
static __xwds_vop
xwer_t xwds_soc_cvop_probe(struct xwds_soc * soc)
{
        xwer_t rc;

        XWDS_VALIDATE(soc->xwccfg, "nullptr", -EFAULT);

        rc = xwds_device_cvop_probe(&soc->dev);
        return rc;
}

/**
 * @brief XWDS VOP：移除SOC
 * @param soc: (I) SOC对象指针
 * @return 错误码
 */
static __xwds_vop
xwer_t xwds_soc_cvop_remove(struct xwds_soc * soc)
{
        xwer_t rc;

        rc = xwds_device_cvop_remove(&soc->dev);
        return rc;
}

/**
 * @brief XWDS VOP：启动SOC
 * @param soc: (I) SOC对象指针
 * @return 错误码
 */
static __xwds_vop
xwer_t xwds_soc_cvop_start(struct xwds_soc * soc)
{
        xwer_t rc;

        rc = xwds_device_cvop_start(&soc->dev);
        return rc;
}

/**
 * @brief XWDS VOP：停止SOC
 * @param soc: (I) SOC对象指针
 * @return 错误码
 */
static __xwds_vop
xwer_t xwds_soc_cvop_stop(struct xwds_soc * soc)
{
        xwer_t rc;

        rc = xwds_device_cvop_stop(&soc->dev);
        return rc;
}

#if (defined(XWMDCFG_ds_PM)) && (1 == XWMDCFG_ds_PM)
/******** ******** pm ******** ********/
/**
 * @brief XWDS VOP：暂停SOC
 * @param soc: (I) SOC对象指针
 * @return 错误码
 */
static __xwds_vop
xwer_t xwds_soc_cvop_suspend(struct xwds_soc * soc)
{
        xwer_t rc;

        rc = xwds_device_cvop_suspend(&soc->dev);
        return rc;
}

/**
 * @brief XWDS VOP：继续SOC
 * @param soc: (I) SOC对象指针
 * @return 错误码
 */
static __xwds_vop
xwer_t xwds_soc_cvop_resume(struct xwds_soc * soc)
{
        xwer_t rc;

        rc = xwds_device_cvop_resume(&soc->dev);
        return rc;
}
#endif /* XWMDCFG_ds_PM */

/******** ******** ******** APIs ******** ******** ********/
__xwds_api
xwer_t xwds_soc_ioctl(struct xwds_soc * soc, xwsq_t cmd, ...)
{
        const struct xwds_soc_driver * drv;
        va_list args;
        xwer_t rc;

        XWDS_VALIDATE(soc, "nullptr", -EFAULT);

        rc = xwds_soc_grab(soc);
        if (__unlikely(rc < 0)) {
                goto err_soc_grab;
        }

        va_start(args, cmd);
        drv = xwds_static_cast(const struct xwds_soc_driver *, soc->dev.drv);
        if ((drv) && (drv->ioctl)) {
                rc = drv->ioctl(soc, cmd, args);
        } else {
                rc = -ENOSYS;
        }
        va_end(args);
        if (__unlikely(rc < 0)) {
                goto err_drv_ioctl;
        }

        xwds_soc_put(soc);
        return OK;

err_drv_ioctl:
        xwds_soc_put(soc);
err_soc_grab:
        return rc;
}
