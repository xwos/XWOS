/**
 * @file
 * @brief 玄武设备栈：LIN：控制器
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > Licensed under the Apache License, Version 2.0 (the "License");
 * > you may not use this file except in compliance with the License.
 * > You may obtain a copy of the License at
 * >
 * >         http://www.apache.org/licenses/LICENSE-2.0
 * >
 * > Unless required by applicable law or agreed to in writing, software
 * > distributed under the License is distributed on an "AS IS" BASIS,
 * > WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * > See the License for the specific language governing permissions and
 * > limitations under the License.
 */

#include <xwcd/ds/standard.h>
#include <xwos/osal/lock/mtx.h>
#include <xwcd/ds/lin/controller.h>

static __xwds_vop
xwer_t xwds_linc_vop_probe(struct xwds_linc * linc);

static __xwds_vop
xwer_t xwds_linc_vop_remove(struct xwds_linc * linc);

static __xwds_vop
xwer_t xwds_linc_vop_start(struct xwds_linc * linc);

static __xwds_vop
xwer_t xwds_linc_vop_stop(struct xwds_linc * linc);

#if defined(XWCDCFG_ds_PM) && (1 == XWCDCFG_ds_PM)
static __xwds_vop
xwer_t xwds_linc_vop_suspend(struct xwds_linc * linc);

static __xwds_vop
xwer_t xwds_linc_vop_resume(struct xwds_linc * linc);
#endif

__xwds_rodata const struct xwds_virtual_operation xwds_linc_vop = {
        .probe = (void *)xwds_linc_vop_probe,
        .remove = (void *)xwds_linc_vop_remove,
        .start = (void *)xwds_linc_vop_start,
        .stop = (void *)xwds_linc_vop_stop,
#if defined(XWCDCFG_ds_PM) && (1 == XWCDCFG_ds_PM)
        .suspend = (void *)xwds_linc_vop_suspend,
        .resume = (void *)xwds_linc_vop_resume,
#endif
};

/******** ******** ******** constructor & destructor ******** ******** ********/
__xwds_code
void xwds_linc_construct(struct xwds_linc * linc)
{
        xwds_device_construct(&linc->dev);
        linc->dev.vop = &xwds_linc_vop;
}

__xwds_code
void xwds_linc_destruct(struct xwds_linc * linc)
{
        xwds_device_destruct(&linc->dev);
}

__xwds_api
xwer_t xwds_linc_grab(struct xwds_linc * linc)
{
        return xwds_device_grab(&linc->dev);
}

__xwds_api
xwer_t xwds_linc_put(struct xwds_linc * linc)
{
        return xwds_device_put(&linc->dev);
}

/******** ******** base virtual operations ******** ********/
/**
 * @brief XWDS VOP：探测LIN控制器
 * @param[in] linc: LIN控制器对象指针
 */
static __xwds_vop
xwer_t xwds_linc_vop_probe(struct xwds_linc * linc)
{
        xwer_t rc;

        rc = xwos_mtx_init(&linc->txlock, XWOS_SKD_PRIORITY_RT_MIN);
        if (rc < 0) {
                goto err_mtx_init;
        }
        if (is_err_or_null(linc->get_msg_size)) {
                linc->get_msg_size = xwds_linc_get_msg_size;
        }
        rc = xwds_device_vop_probe(&linc->dev);
        if (rc < 0) {
                goto err_dev_vop_probe;
        }
        return XWOK;

err_dev_vop_probe:
        xwos_mtx_fini(&linc->txlock);
err_mtx_init:
        return rc;
}

/**
 * @brief XWDS VOP：移除LIN控制器
 * @param[in] linc: LIN控制器对象指针
 */
static __xwds_vop
xwer_t xwds_linc_vop_remove(struct xwds_linc * linc)
{
        xwer_t rc;

        rc = xwds_device_vop_remove(&linc->dev);
        if (rc < 0) {
                goto err_dev_vop_remove;
        }
        xwos_mtx_fini(&linc->txlock);
        return XWOK;

err_dev_vop_remove:
        return rc;
}

/**
 * @brief XWDS VOP：启动LIN控制器
 * @param[in] linc: LIN控制器对象指针
 */
static __xwds_vop
xwer_t xwds_linc_vop_start(struct xwds_linc * linc)
{
        xwer_t rc;

        rc = xwds_device_vop_start(&linc->dev);
        return rc;
}

/**
 * @brief XWDS VOP：停止LIN控制器
 * @param[in] linc: LIN控制器对象指针
 */
static __xwds_vop
xwer_t xwds_linc_vop_stop(struct xwds_linc * linc)
{
        xwer_t rc;

        rc = xwds_device_vop_stop(&linc->dev);
        return rc;
}

#if defined(XWCDCFG_ds_PM) && (1 == XWCDCFG_ds_PM)
/******** ******** pm ******** ********/
/**
 * @brief XWDS VOP：暂停LIN控制器
 * @param[in] linc: LIN控制器对象指针
 */
static __xwds_vop
xwer_t xwds_linc_vop_suspend(struct xwds_linc * linc)
{
        xwer_t rc;

        rc = xwds_device_vop_suspend(&linc->dev);
        return rc;
}

/**
 * @brief XWDS VOP：继续LIN控制器
 * @param[in] linc: LIN控制器对象指针
 */
static __xwds_vop
xwer_t xwds_linc_vop_resume(struct xwds_linc * linc)
{
        xwer_t rc;

        rc = xwds_device_vop_resume(&linc->dev);
        return rc;
}
#endif

/******** ******** ******** APIs ******** ******** ********/
__xwds_api
xwer_t xwds_linc_msttx(struct xwds_linc * linc,
                       xwu8_t id, struct xwds_lin_msg * msg,
                       xwtm_t to)
{
        xwer_t rc;
        const struct xwds_linc_driver * drv;

        XWDS_VALIDATE(linc, "nullptr", -EFAULT);
        XWDS_VALIDATE(msg, "nullptr", -EFAULT);

        rc = xwds_linc_grab(linc);
        if (rc < 0) {
                goto err_linc_grab;
        }
        rc = xwos_mtx_lock_to(&linc->txlock, to);
        if (rc < 0) {
                goto err_linc_txlock;
        }
        drv = xwds_cast(const struct xwds_linc_driver *, linc->dev.drv);
        if ((drv) && (drv->msttx)) {
                rc = drv->msttx(linc, id, msg, to);
        } else {
                rc = -ENOSYS;
        }
        if (rc < 0) {
                goto err_drv_msttx;
        }
        xwos_mtx_unlock(&linc->txlock);
        xwds_linc_put(linc);
        return XWOK;

err_drv_msttx:
        xwos_mtx_unlock(&linc->txlock);
err_linc_txlock:
        xwds_linc_put(linc);
err_linc_grab:
        return rc;
}

__xwds_api
xwer_t xwds_linc_slvtx(struct xwds_linc * linc,
                       struct xwds_lin_msg * msg,
                       xwtm_t to)
{
        xwer_t rc;
        const struct xwds_linc_driver * drv;

        XWDS_VALIDATE(linc, "nullptr", -EFAULT);
        XWDS_VALIDATE(msg, "nullptr", -EFAULT);

        rc = xwds_linc_grab(linc);
        if (rc < 0) {
                goto err_linc_grab;
        }
        rc = xwos_mtx_lock_to(&linc->txlock, to);
        if (rc < 0) {
                goto err_linc_txlock;
        }
        drv = xwds_cast(const struct xwds_linc_driver *, linc->dev.drv);
        if ((drv) && (drv->slvtx)) {
                rc = drv->slvtx(linc, msg, to);
        } else {
                rc = -ENOSYS;
        }
        if (rc < 0) {
                goto err_drv_slvtx;
        }
        xwos_mtx_unlock(&linc->txlock);
        xwds_linc_put(linc);
        return XWOK;

err_drv_slvtx:
        xwos_mtx_unlock(&linc->txlock);
err_linc_txlock:
        xwds_linc_put(linc);
err_linc_grab:
        return rc;
}

__xwds_api
xwer_t xwds_linc_rx(struct xwds_linc * linc,
                    struct xwds_lin_msg * msgbuf,
                    xwtm_t to)
{
        xwer_t rc;
        const struct xwds_linc_driver * drv;

        XWDS_VALIDATE(linc, "nullptr", -EFAULT);
        XWDS_VALIDATE(msgbuf, "nullptr", -EFAULT);

        rc = xwds_linc_grab(linc);
        if (rc < 0) {
                goto err_linc_grab;
        }
        drv = xwds_cast(const struct xwds_linc_driver *, linc->dev.drv);
        if ((drv) && (drv->rx)) {
                rc = drv->rx(linc, msgbuf, to);
        } else {
                rc = -ENOSYS;
        }
        if (rc < 0) {
                goto err_drv_rx;
        }
        xwds_linc_put(linc);
        return XWOK;

err_drv_rx:
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
                                rc = XWOK;
                                break;
                        }
                }
                if (-ENODEV == rc) {
                        *ret = XWDS_LIN_DEFAULT_DATA_SIZE;
                }
        } else {
                rc = XWOK;
                *ret = XWDS_LIN_DEFAULT_DATA_SIZE;
        }
        return rc;
}
