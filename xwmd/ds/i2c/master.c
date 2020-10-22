/**
 * @file
 * @brief xwmd设备栈：I2C主机
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
#include <xwos/osal/lock/mutex.h>
#include <xwmd/ds/i2c/master.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********     static function prototypes      ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
static __xwds_vop
xwer_t xwds_i2cm_cvop_probe(struct xwds_i2cm * i2cm);

static __xwds_vop
xwer_t xwds_i2cm_cvop_remove(struct xwds_i2cm * i2cm);

static __xwds_vop
xwer_t xwds_i2cm_cvop_start(struct xwds_i2cm * i2cm);

static __xwds_vop
xwer_t xwds_i2cm_cvop_stop(struct xwds_i2cm * i2cm);

#if defined(XWMDCFG_ds_PM) && (1 == XWMDCFG_ds_PM)
static __xwds_vop
xwer_t xwds_i2cm_cvop_suspend(struct xwds_i2cm * i2cm);

static __xwds_vop
xwer_t xwds_i2cm_cvop_resume(struct xwds_i2cm * i2cm);
#endif /* XWMDCFG_ds_PM */

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
__xwds_rodata const struct xwds_base_virtual_operations xwds_i2cm_cvops = {
        .probe = (void *)xwds_i2cm_cvop_probe,
        .remove = (void *)xwds_i2cm_cvop_remove,
        .start = (void *)xwds_i2cm_cvop_start,
        .stop = (void *)xwds_i2cm_cvop_stop,
#if defined(XWMDCFG_ds_PM) && (1 == XWMDCFG_ds_PM)
        .suspend = (void *)xwds_i2cm_cvop_suspend,
        .resume = (void *)xwds_i2cm_cvop_resume,
#endif /* XWMDCFG_ds_PM */
};

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/******** ******** ******** constructor & destructor ******** ******** ********/
/**
 * @brief XWDS API：I2C主机控制器的构造函数
 * @param i2cm: (I) I2C主机控制器对象指针
 */
__xwds_api
void xwds_i2cm_construct(struct xwds_i2cm * i2cm)
{
        xwds_device_construct(&i2cm->dev);
        i2cm->dev.cvops = &xwds_i2cm_cvops;
}

/**
 * @brief XWDS API：I2C主机控制器对象的析构函数
 * @param i2cm: (I) I2C主机控制器对象指针
 */
__xwds_api
void xwds_i2cm_destruct(struct xwds_i2cm * i2cm)
{
        xwds_device_destruct(&i2cm->dev);
}

/******** ******** base virtual operations ******** ********/
/**
 * @brief XWDS VOP：探测I2C主机控制器
 * @param i2cm: (I) I2C主机控制器对象指针
 * @return 错误码
 */
static __xwds_vop
xwer_t xwds_i2cm_cvop_probe(struct xwds_i2cm * i2cm)
{
        xwer_t rc;

        XWDS_VALIDATE(i2cm->cfg, "nullptr", -EFAULT);

        rc = xwosal_mtx_init(&i2cm->xfer.lock, XWOSAL_SD_PRIORITY_RT_MIN);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_xfer_mtx_init;
        }
        rc = xwosal_mtx_init(&i2cm->abort.lock, XWOSAL_SD_PRIORITY_RT_MIN);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_abort_mtx_init;
        }
        rc = xwds_device_cvop_probe(&i2cm->dev);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_dev_cvop_probe;
        }
        return XWOK;

err_dev_cvop_probe:
        xwosal_mtx_destroy(&i2cm->abort.lock);
err_abort_mtx_init:
        xwosal_mtx_destroy(&i2cm->xfer.lock);
err_xfer_mtx_init:
        return rc;
}

/**
 * @brief XWDS VOP：移除I2C主机控制器
 * @param ds: (I) 设备栈控制块指针
 * @param i2cm: (I) I2C主机控制器对象指针
 * @return 错误码
 */
static __xwds_vop
xwer_t xwds_i2cm_cvop_remove(struct xwds_i2cm * i2cm)
{
        xwer_t rc;

        rc = xwds_device_cvop_remove(&i2cm->dev);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_dev_cvop_remove;
        }
        xwosal_mtx_destroy(&i2cm->abort.lock);
        xwosal_mtx_destroy(&i2cm->xfer.lock);
        return XWOK;

err_dev_cvop_remove:
        return rc;
}

/**
 * @brief XWDS VOP：启动I2C主机控制器
 * @param i2cm: (I) I2C主机控制器对象指针
 * @return 错误码
 */
static __xwds_vop
xwer_t xwds_i2cm_cvop_start(struct xwds_i2cm * i2cm)
{
        xwer_t rc;

        rc = xwds_device_cvop_start(&i2cm->dev);
        return rc;
}

/**
 * @brief XWDS VOP：停止I2C主机控制器
 * @param i2cm: (I) I2C主机控制器对象指针
 * @return 错误码
 */
static __xwds_vop
xwer_t xwds_i2cm_cvop_stop(struct xwds_i2cm * i2cm)
{
        xwer_t rc;

        rc = xwds_device_cvop_stop(&i2cm->dev);
        return rc;
}

/******** ******** pm ******** ********/
#if defined(XWMDCFG_ds_PM) && (1 == XWMDCFG_ds_PM)
/**
 * @brief XWDS VOP：暂停I2C主机控制器
 * @param i2cm: (I) I2C主机控制器对象指针
 * @return 错误码
 */
static __xwds_vop
xwer_t xwds_i2cm_cvop_suspend(struct xwds_i2cm * i2cm)
{
        xwer_t rc;

        rc = xwds_device_cvop_suspend(&i2cm->dev);
        return rc;
}

/**
 * @brief XWDS VOP：继续I2C主机控制器
 * @param i2cm: (I) I2C主机控制器对象指针
 * @return 错误码
 */
static __xwds_vop
xwer_t xwds_i2cm_cvop_resume(struct xwds_i2cm * i2cm)
{
        xwer_t rc;

        rc = xwds_device_cvop_resume(&i2cm->dev);
        return rc;
}
#endif /* XWMDCFG_ds_PM */

/******** ******** ******** APIs ******** ******** ********/
__xwds_api
xwer_t xwds_i2cm_xfer(struct xwds_i2cm * i2cm, struct xwds_i2c_msg * msg,
                      xwtm_t * xwtm)
{
        xwer_t rc;
        const struct xwds_i2cm_driver * drv;

        XWDS_VALIDATE(i2cm, "nullptr", -EFAULT);
        XWDS_VALIDATE(msg, "nullptr", -EFAULT);
        XWDS_VALIDATE((msg->flag & XWDS_I2C_F_DIRMSK), "no-direction", -EINVAL);
        XWDS_VALIDATE(xwtm, "nullptr", -EFAULT);

        rc = xwds_i2cm_grab(i2cm);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_i2cm_grab;
        }
        rc = xwds_i2cm_request(i2cm);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_i2cm_request;
        }

        rc = xwosal_mtx_timedlock(xwosal_mtx_get_id(&i2cm->xfer.lock), xwtm);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_i2cm_lock;
        }
        drv = xwds_static_cast(const struct xwds_i2cm_driver *, i2cm->dev.drv);
        if ((drv) && (drv->xfer)) {
                rc = drv->xfer(i2cm, msg, xwtm);
        } else {
                rc = -ENOSYS;
        }
        if (__xwcc_unlikely(rc < 0)) {
                goto err_drv_xfer;
        }
        xwosal_mtx_unlock(xwosal_mtx_get_id(&i2cm->xfer.lock));
        xwds_i2cm_release(i2cm);
        xwds_i2cm_put(i2cm);
        return XWOK;

err_drv_xfer:
        xwosal_mtx_unlock(xwosal_mtx_get_id(&i2cm->xfer.lock));
err_i2cm_lock:
        xwds_i2cm_release(i2cm);
err_i2cm_request:
        xwds_i2cm_put(i2cm);
err_i2cm_grab:
        return rc;
}

__xwds_api
xwer_t xwds_i2cm_abort(struct xwds_i2cm * i2cm,
                       xwu16_t address, xwu16_t addrmode,
                       xwtm_t * xwtm)
{
        xwer_t rc;
        const struct xwds_i2cm_driver * drv;

        XWDS_VALIDATE(i2cm, "nullptr", -EFAULT);
        XWDS_VALIDATE(xwtm, "nullptr", -EFAULT);

        rc = xwds_i2cm_grab(i2cm);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_i2cm_grab;
        }
        rc = xwds_i2cm_request(i2cm);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_i2cm_request;
        }

        rc = xwosal_mtx_timedlock(xwosal_mtx_get_id(&i2cm->abort.lock), xwtm);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_i2cm_lock;
        }
        drv = xwds_static_cast(const struct xwds_i2cm_driver *, i2cm->dev.drv);
        if ((drv) && (drv->abort)) {
                rc = drv->abort(i2cm, address, addrmode, xwtm);
        } else {
                rc = -ENOSYS;
        }
        if (__xwcc_unlikely(rc < 0)) {
                goto err_drv_abort;
        }
        xwosal_mtx_unlock(xwosal_mtx_get_id(&i2cm->abort.lock));
        xwds_i2cm_release(i2cm);
        xwds_i2cm_put(i2cm);
        return XWOK;

err_drv_abort:
        xwosal_mtx_unlock(xwosal_mtx_get_id(&i2cm->abort.lock));
err_i2cm_lock:
        xwds_i2cm_release(i2cm);
err_i2cm_request:
        xwds_i2cm_put(i2cm);
err_i2cm_grab:
        return rc;
}
