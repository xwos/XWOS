/**
 * @file
 * @brief xwmd设备栈：LIN控制器
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
#include <xwmd/ds/lin/controller.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********     static function prototypes      ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
static __xwds_vop
xwer_t xwds_linc_cvop_probe(struct xwds_linc * linc);

static __xwds_vop
xwer_t xwds_linc_cvop_remove(struct xwds_linc * linc);

static __xwds_vop
xwer_t xwds_linc_cvop_start(struct xwds_linc * linc);

static __xwds_vop
xwer_t xwds_linc_cvop_stop(struct xwds_linc * linc);

#if defined(XWMDCFG_ds_PM) && (1 == XWMDCFG_ds_PM)
static __xwds_vop
xwer_t xwds_linc_cvop_suspend(struct xwds_linc * linc);

static __xwds_vop
xwer_t xwds_linc_cvop_resume(struct xwds_linc * linc);
#endif /* XWMDCFG_ds_PM */

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
__xwds_rodata const struct xwds_base_virtual_operations xwds_linc_cvops = {
        .probe = (void *)xwds_linc_cvop_probe,
        .remove = (void *)xwds_linc_cvop_remove,
        .start = (void *)xwds_linc_cvop_start,
        .stop = (void *)xwds_linc_cvop_stop,
#if defined(XWMDCFG_ds_PM) && (1 == XWMDCFG_ds_PM)
        .suspend = (void *)xwds_linc_cvop_suspend,
        .resume = (void *)xwds_linc_cvop_resume,
#endif /* XWMDCFG_ds_PM */
};

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/******** ******** ******** constructor & destructor ******** ******** ********/
/**
 * @brief XWDS API：LIN控制器的构造函数
 * @param linc: (I) LIN控制器对象指针
 */
__xwds_code
void xwds_linc_construct(struct xwds_linc * linc)
{
        xwds_device_construct(&linc->dev);
        linc->dev.cvops = &xwds_linc_cvops;
}

/**
 * @brief XWDS API：LIN控制器对象的析构函数
 * @param linc: (I) LIN控制器对象指针
 */
__xwds_code
void xwds_linc_destruct(struct xwds_linc * linc)
{
        xwds_device_destruct(&linc->dev);
}

/******** ******** base virtual operations ******** ********/
/**
 * @brief XWDS VOP：探测LIN控制器
 * @param linc: (I) LIN控制器对象指针
 * @return 错误码
 */
static __xwds_vop
xwer_t xwds_linc_cvop_probe(struct xwds_linc * linc)
{
        xwer_t rc;

        rc = xwosal_mtx_init(&linc->txlock, XWOSAL_SD_PRIORITY_RT_MIN);
        if (__unlikely(rc < 0)) {
                goto err_mtx_init;
        }
        if (is_err_or_null(linc->get_msg_size)) {
                linc->get_msg_size = xwds_linc_get_msg_size;
        }
        rc = xwds_device_cvop_probe(&linc->dev);
        if (__unlikely(rc < 0)) {
                goto err_dev_cvop_probe;
        }
        return OK;

err_dev_cvop_probe:
        xwosal_mtx_destroy(&linc->txlock);
err_mtx_init:
        return rc;
}

/**
 * @brief XWDS VOP：移除LIN控制器
 * @param linc: (I) LIN控制器对象指针
 * @return 错误码
 */
static __xwds_vop
xwer_t xwds_linc_cvop_remove(struct xwds_linc * linc)
{
        xwer_t rc;

        rc = xwds_device_cvop_remove(&linc->dev);
        if (__unlikely(rc < 0)) {
                goto err_dev_cvop_remove;
        }
        xwosal_mtx_destroy(&linc->txlock);
        return OK;

err_dev_cvop_remove:
        return rc;
}

/**
 * @brief XWDS VOP：启动LIN控制器
 * @param linc: (I) LIN控制器对象指针
 * @return 错误码
 */
static __xwds_vop
xwer_t xwds_linc_cvop_start(struct xwds_linc * linc)
{
        xwer_t rc;

        rc = xwds_device_cvop_start(&linc->dev);
        return rc;
}

/**
 * @brief XWDS VOP：停止LIN控制器
 * @param linc: (I) LIN控制器对象指针
 * @return 错误码
 */
static __xwds_vop
xwer_t xwds_linc_cvop_stop(struct xwds_linc * linc)
{
        xwer_t rc;

        rc = xwds_device_cvop_stop(&linc->dev);
        return rc;
}

#if defined(XWMDCFG_ds_PM) && (1 == XWMDCFG_ds_PM)
/******** ******** pm ******** ********/
/**
 * @brief XWDS VOP：暂停LIN控制器
 * @param linc: (I) LIN控制器对象指针
 * @return 错误码
 */
static __xwds_vop
xwer_t xwds_linc_cvop_suspend(struct xwds_linc * linc)
{
        xwer_t rc;

        rc = xwds_device_cvop_suspend(&linc->dev);
        return rc;
}

/**
 * @brief XWDS VOP：继续LIN控制器
 * @param linc: (I) LIN控制器对象指针
 * @return 错误码
 */
static __xwds_vop
xwer_t xwds_linc_cvop_resume(struct xwds_linc * linc)
{
        xwer_t rc;

        rc = xwds_device_cvop_resume(&linc->dev);
        return rc;
}
#endif /* XWMDCFG_ds_PM */

/******** ******** ******** APIs ******** ******** ********/
__xwds_api
xwer_t xwds_linc_msttx(struct xwds_linc * linc,
                       xwu8_t id, struct xwds_lin_msg * msg,
                       xwtm_t * xwtm)
{
        xwer_t rc;
        const struct xwds_linc_driver * drv;

        XWDS_VALIDATE(linc, "nullptr", -EFAULT);
        XWDS_VALIDATE(msg, "nullptr", -EFAULT);
        XWDS_VALIDATE(xwtm, "nullptr", -EFAULT);

        rc = xwds_linc_grab(linc);
        if (__unlikely(rc < 0)) {
                goto err_linc_grab;
        }

        rc = xwds_linc_request(linc);
        if (__unlikely(rc < 0)) {
                goto err_linc_request;
        }

        rc = xwosal_mtx_timedlock(xwosal_mtx_get_id(&linc->txlock), xwtm);
        if (__unlikely(rc < 0)) {
                goto err_linc_txlock;
        }
        drv = xwds_static_cast(const struct xwds_linc_driver *, linc->dev.drv);
        if ((drv) && (drv->msttx)) {
                rc = drv->msttx(linc, id, msg, xwtm);
        } else {
                rc = -ENOSYS;
        }
        if (__unlikely(rc < 0)) {
                goto err_drv_msttx;
        }
        xwosal_mtx_unlock(xwosal_mtx_get_id(&linc->txlock));

        xwds_linc_release(linc);
        xwds_linc_put(linc);
        return OK;

err_drv_msttx:
        xwosal_mtx_unlock(xwosal_mtx_get_id(&linc->txlock));
err_linc_txlock:
        xwds_linc_release(linc);
err_linc_request:
        xwds_linc_put(linc);
err_linc_grab:
        return rc;
}

__xwds_api
xwer_t xwds_linc_slvtx(struct xwds_linc * linc,
                       struct xwds_lin_msg * msg,
                       xwtm_t * xwtm)
{
        xwer_t rc;
        const struct xwds_linc_driver * drv;

        XWDS_VALIDATE(linc, "nullptr", -EFAULT);
        XWDS_VALIDATE(msg, "nullptr", -EFAULT);
        XWDS_VALIDATE(xwtm, "nullptr", -EFAULT);

        rc = xwds_linc_grab(linc);
        if (__unlikely(rc < 0)) {
                goto err_linc_grab;
        }
        rc = xwds_linc_request(linc);
        if (__unlikely(rc < 0)) {
                goto err_linc_request;
        }

        rc = xwosal_mtx_timedlock(xwosal_mtx_get_id(&linc->txlock), xwtm);
        if (__unlikely(rc < 0)) {
                goto err_linc_txlock;
        }
        drv = xwds_static_cast(const struct xwds_linc_driver *, linc->dev.drv);
        if (__likely((drv) && (drv->slvtx))) {
                rc = drv->slvtx(linc, msg, xwtm);
        } else {
                rc = -ENOSYS;
        }
        if (__unlikely(rc < 0)) {
                goto err_drv_slvtx;
        }
        xwosal_mtx_unlock(xwosal_mtx_get_id(&linc->txlock));

        xwds_linc_release(linc);
        xwds_linc_put(linc);
        return OK;

err_drv_slvtx:
        xwosal_mtx_unlock(xwosal_mtx_get_id(&linc->txlock));
err_linc_txlock:
        xwds_linc_release(linc);
err_linc_request:
        xwds_linc_put(linc);
err_linc_grab:
        return rc;
}

__xwds_api
xwer_t xwds_linc_rx(struct xwds_linc * linc,
                    struct xwds_lin_msg * msgbuf,
                    xwtm_t * xwtm)
{
        xwer_t rc;
        const struct xwds_linc_driver * drv;

        XWDS_VALIDATE(linc, "nullptr", -EFAULT);
        XWDS_VALIDATE(msgbuf, "nullptr", -EFAULT);
        XWDS_VALIDATE(xwtm, "nullptr", -EFAULT);

        rc = xwds_linc_grab(linc);
        if (__unlikely(rc < 0)) {
                goto err_linc_grab;
        }
        rc = xwds_linc_request(linc);
        if (__unlikely(rc < 0)) {
                goto err_linc_request;
        }

        drv = xwds_static_cast(const struct xwds_linc_driver *, linc->dev.drv);
        if (__likely((drv) && (drv->rx))) {
                rc = drv->rx(linc, msgbuf, xwtm);
        } else {
                rc = -ENOSYS;
        }
        if (__unlikely(rc < 0)) {
                goto err_drv_rx;
        }

        xwds_linc_release(linc);
        xwds_linc_put(linc);
        return OK;

err_drv_rx:
        xwds_linc_release(linc);
err_linc_request:
        xwds_linc_put(linc);
err_linc_grab:
        return rc;
}

__xwds_api
xwer_t xwds_linc_get_msg_size(struct xwds_linc * linc,
                              xwu8_t protected_id,
                              xwu8_t * ret)
{
        const struct xwds_lin_msg_info * msgitbl;
        xwsz_t itemnum;
        xwer_t rc;
        xwsq_t i;

        XWDS_VALIDATE(linc, "nullptr", -EFAULT);
        XWDS_VALIDATE(ret, "nullptr", -EFAULT);

        msgitbl = linc->msgitable;
        itemnum = linc->msgitable_itemnum;
        rc = -ENODEV;
        if (!is_err_or_null(msgitbl)) {
                for (i = 0; i < itemnum; i++) {
                        if (protected_id == msgitbl[i].protected_id) {
                                *ret = msgitbl[i].size;
                                rc = OK;
                                break;
                        }
                }
                if (-ENODEV == rc) {
                        *ret = XWDS_LIN_DEFAULT_DATA_SIZE;
                }
        } else {
                rc = OK;
                *ret = XWDS_LIN_DEFAULT_DATA_SIZE;
        }
        return rc;
}
