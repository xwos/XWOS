/**
 * @file
 * @brief xwmd设备栈：SPI主机模式控制器
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
#include <xwos/lib/xwlog.h>
#include <xwos/osal/scheduler.h>
#include <xwos/osal/thread.h>
#include <xwos/osal/lock/mutex.h>
#include <xwmd/ds/spi/master.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********     static function prototypes      ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
static __xwds_vop
xwer_t xwds_spim_cvop_probe(struct xwds_spim * spim);

static __xwds_vop
xwer_t xwds_spim_cvop_remove(struct xwds_spim * spim);

static __xwds_vop
xwer_t xwds_spim_cvop_start(struct xwds_spim * spim);

static __xwds_vop
xwer_t xwds_spim_cvop_stop(struct xwds_spim * spim);

#if defined(XWMDCFG_ds_PM) && (1 == XWMDCFG_ds_PM)
static __xwds_vop
xwer_t xwds_spim_cvop_suspend(struct xwds_spim * spim);

static __xwds_vop
xwer_t xwds_spim_cvop_resume(struct xwds_spim * spim);
#endif /* XWMDCFG_ds_PM */

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
__xwds_rodata const struct xwds_base_virtual_operations xwds_spim_cvops = {
        .probe = (void *)xwds_spim_cvop_probe,
        .remove = (void *)xwds_spim_cvop_remove,
        .start = (void *)xwds_spim_cvop_start,
        .stop = (void *)xwds_spim_cvop_stop,
#if defined(XWMDCFG_ds_PM) && (1 == XWMDCFG_ds_PM)
        .suspend = (void *)xwds_spim_cvop_suspend,
        .resume = (void *)xwds_spim_cvop_resume,
#endif /* XWMDCFG_ds_PM */
};

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/******** ******** ******** constructor & destructor ******** ******** ********/
/**
 * @brief XWDS API：SPI主机模式控制器对象的构造函数
 * @param spim: (I) SPI主机模式控制器对象指针
 */
__xwds_api
void xwds_spim_construct(struct xwds_spim * spim)
{
        xwds_device_construct(&spim->dev);
        spim->dev.cvops = &xwds_spim_cvops;
}

/**
 * @brief XWDS API：SPI主机模式控制器对象的析构函数
 * @param spim: (I) SPI主机模式控制器对象指针
 */
__xwds_api
void xwds_spim_destruct(struct xwds_spim * spim)
{
        struct xwds_device * dev;

        dev = xwds_static_cast(struct xwds_device *, spim);
        xwds_device_destruct(dev);
}

/******** ******** base virtual operations ******** ********/
/**
 * @brief XWDS VOP：探测设备
 * @param spim: (I) SPI主机模式控制器对象指针
 * @return 错误码
 */
static __xwds_vop
xwer_t xwds_spim_cvop_probe(struct xwds_spim * spim)
{
        xwer_t rc;

        rc = xwosal_mtx_init(&spim->xfermtx, XWOSAL_SD_PRIORITY_RT_MIN);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_xfermtx_init;
        }
        rc = xwds_device_cvop_probe(&spim->dev);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_dev_probe;
        }
        return XWOK;

err_dev_probe:
        xwosal_mtx_destroy(&spim->xfermtx);
err_xfermtx_init:
        return rc;
}

/**
 * @brief XWDS VOP：删除设备
 * @param spim: (I) SPI主机模式控制器对象指针
 * @return 错误码
 */
static __xwds_vop
xwer_t xwds_spim_cvop_remove(struct xwds_spim * spim)
{
        xwer_t rc;

        rc = xwds_device_cvop_remove(&spim->dev);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_dev_cvop_remove;
        }

        xwosal_mtx_destroy(&spim->xfermtx);
        return XWOK;

err_dev_cvop_remove:
        return rc;
}

/**
 * @brief XWDS VOP：启动设备
 * @param spim: (I) SPI主机模式控制器对象指针
 * @return 错误码
 */
static __xwds_vop
xwer_t xwds_spim_cvop_start(struct xwds_spim * spim)
{
        xwer_t rc;

        rc = xwds_device_cvop_start(&spim->dev);
        return rc;
}

/**
 * @brief XWDS VOP：停止设备
 * @param spim: (I) SPI主机模式控制器对象指针
 * @return 错误码
 */
static __xwds_vop
xwer_t xwds_spim_cvop_stop(struct xwds_spim * spim)
{
        xwer_t rc;

        rc = xwds_device_cvop_stop(&spim->dev);
        return rc;
}

#if defined(XWMDCFG_ds_PM) && (1 == XWMDCFG_ds_PM)
/******** ******** pm ******** ********/
/**
 * @brief XWDS VOP：暂停设备
 * @param spim: (I) SPI主机模式控制器对象指针
 * @return 错误码
 */
static __xwds_vop
xwer_t xwds_spim_cvop_suspend(struct xwds_spim * spim)
{
        xwer_t rc;

        rc = xwds_device_cvop_suspend(&spim->dev);
        return rc;
}

/**
 * @brief XWDS VOP：继续设备
 * @param spim: (I) SPI主机模式控制器对象指针
 * @return 错误码
 */
static __xwds_vop
xwer_t xwds_spim_cvop_resume(struct xwds_spim * spim)
{
        xwer_t rc;

        rc = xwds_device_cvop_resume(&spim->dev);
        return rc;
}
#endif /* XWMDCFG_ds_PM */

/******** ******** ******** SPI Master Device APIs ******** ******** ********/
__xwds_api
xwer_t xwds_spim_xfer(struct xwds_spim * spim,
                      struct xwds_spim_msg * msg,
                      xwtm_t * xwtm)
{
        const struct xwds_spim_driver * drv;
        xwer_t rc;

        XWDS_VALIDATE(spim, "nullptr", -EFAULT);
        XWDS_VALIDATE(msg, "nullptr", -EFAULT);
        XWDS_VALIDATE(xwtm, "nullptr", -EFAULT);

        rc = xwds_spim_grab(spim);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_spim_grab;
        }
        rc = xwds_spim_request(spim);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_spim_request;
        }

        rc = xwosal_mtx_timedlock(xwosal_mtx_get_id(&spim->xfermtx), xwtm);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_spim_lock;
        }

        drv = xwds_static_cast(const struct xwds_spim_driver *, spim->dev.drv);
        if (__xwcc_likely((drv) && (drv->xfer))) {
                rc = drv->xfer(spim, msg, xwtm);
        } else {
                rc = -ENOSYS;
        }
        if (__xwcc_unlikely(rc < 0)) {
                goto err_drv_xfer;
        }
        xwosal_mtx_unlock(xwosal_mtx_get_id(&spim->xfermtx));

        xwds_spim_release(spim);
        xwds_spim_put(spim);
        return XWOK;

err_drv_xfer:
        xwosal_mtx_unlock(xwosal_mtx_get_id(&spim->xfermtx));
err_spim_lock:
        xwds_spim_release(spim);
err_spim_request:
        xwds_spim_put(spim);
err_spim_grab:
        return rc;
}
