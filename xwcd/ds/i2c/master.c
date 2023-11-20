/**
 * @file
 * @brief 玄武设备栈：I2C：主机
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
#include <xwcd/ds/i2c/master.h>

static __xwds_vop
xwer_t xwds_i2cm_vop_probe(struct xwds_i2cm * i2cm);

static __xwds_vop
xwer_t xwds_i2cm_vop_remove(struct xwds_i2cm * i2cm);

static __xwds_vop
xwer_t xwds_i2cm_vop_start(struct xwds_i2cm * i2cm);

static __xwds_vop
xwer_t xwds_i2cm_vop_stop(struct xwds_i2cm * i2cm);

#if defined(XWCDCFG_ds_PM) && (1 == XWCDCFG_ds_PM)
static __xwds_vop
xwer_t xwds_i2cm_vop_suspend(struct xwds_i2cm * i2cm);

static __xwds_vop
xwer_t xwds_i2cm_vop_resume(struct xwds_i2cm * i2cm);
#endif

__xwds_rodata const struct xwds_virtual_operation xwds_i2cm_vop = {
        .probe = (void *)xwds_i2cm_vop_probe,
        .remove = (void *)xwds_i2cm_vop_remove,
        .start = (void *)xwds_i2cm_vop_start,
        .stop = (void *)xwds_i2cm_vop_stop,
#if defined(XWCDCFG_ds_PM) && (1 == XWCDCFG_ds_PM)
        .suspend = (void *)xwds_i2cm_vop_suspend,
        .resume = (void *)xwds_i2cm_vop_resume,
#endif
};

/******** ******** ******** constructor & destructor ******** ******** ********/
__xwds_api
void xwds_i2cm_construct(struct xwds_i2cm * i2cm)
{
        xwds_device_construct(&i2cm->dev);
        i2cm->dev.vop = &xwds_i2cm_vop;
}

__xwds_api
void xwds_i2cm_destruct(struct xwds_i2cm * i2cm)
{
        xwds_device_destruct(&i2cm->dev);
}

__xwds_api
xwer_t xwds_i2cm_grab(struct xwds_i2cm * i2cm)
{
        return xwds_device_grab(&i2cm->dev);
}

__xwds_api
xwer_t xwds_i2cm_put(struct xwds_i2cm * i2cm)
{
        return xwds_device_put(&i2cm->dev);
}

/******** ******** base virtual operations ******** ********/
/**
 * @brief XWDS VOP：探测I2C主机控制器
 * @param[in] i2cm: I2C主机控制器对象指针
 * @return 错误码
 */
static __xwds_vop
xwer_t xwds_i2cm_vop_probe(struct xwds_i2cm * i2cm)
{
        xwer_t rc;

        XWDS_VALIDATE(i2cm->xwccfg, "nullptr", -EFAULT);

        rc = xwos_mtx_init(&i2cm->xfer.apimtx, XWOS_SKD_PRIORITY_RT_MIN);
        if (rc < 0) {
                goto err_xfer_mtx_init;
        }
        rc = xwos_mtx_init(&i2cm->abort.apimtx, XWOS_SKD_PRIORITY_RT_MIN);
        if (rc < 0) {
                goto err_abort_mtx_init;
        }
        rc = xwds_device_vop_probe(&i2cm->dev);
        if (rc < 0) {
                goto err_dev_vop_probe;
        }
        return XWOK;

err_dev_vop_probe:
        xwos_mtx_fini(&i2cm->abort.apimtx);
err_abort_mtx_init:
        xwos_mtx_fini(&i2cm->xfer.apimtx);
err_xfer_mtx_init:
        return rc;
}

/**
 * @brief XWDS VOP：移除I2C主机控制器
 * @param[in] ds: 设备栈控制块指针
 * @param[in] i2cm: I2C主机控制器对象指针
 * @return 错误码
 */
static __xwds_vop
xwer_t xwds_i2cm_vop_remove(struct xwds_i2cm * i2cm)
{
        xwer_t rc;

        rc = xwds_device_vop_remove(&i2cm->dev);
        if (rc < 0) {
                goto err_dev_vop_remove;
        }
        xwos_mtx_fini(&i2cm->abort.apimtx);
        xwos_mtx_fini(&i2cm->xfer.apimtx);
        return XWOK;

err_dev_vop_remove:
        return rc;
}

/**
 * @brief XWDS VOP：启动I2C主机控制器
 * @param[in] i2cm: I2C主机控制器对象指针
 * @return 错误码
 */
static __xwds_vop
xwer_t xwds_i2cm_vop_start(struct xwds_i2cm * i2cm)
{
        xwer_t rc;

        rc = xwds_device_vop_start(&i2cm->dev);
        return rc;
}

/**
 * @brief XWDS VOP：停止I2C主机控制器
 * @param[in] i2cm: I2C主机控制器对象指针
 * @return 错误码
 */
static __xwds_vop
xwer_t xwds_i2cm_vop_stop(struct xwds_i2cm * i2cm)
{
        xwer_t rc;

        rc = xwds_device_vop_stop(&i2cm->dev);
        return rc;
}

/******** ******** pm ******** ********/
#if defined(XWCDCFG_ds_PM) && (1 == XWCDCFG_ds_PM)
/**
 * @brief XWDS VOP：暂停I2C主机控制器
 * @param[in] i2cm: I2C主机控制器对象指针
 * @return 错误码
 */
static __xwds_vop
xwer_t xwds_i2cm_vop_suspend(struct xwds_i2cm * i2cm)
{
        xwer_t rc;

        rc = xwds_device_vop_suspend(&i2cm->dev);
        return rc;
}

/**
 * @brief XWDS VOP：继续I2C主机控制器
 * @param[in] i2cm: I2C主机控制器对象指针
 * @return 错误码
 */
static __xwds_vop
xwer_t xwds_i2cm_vop_resume(struct xwds_i2cm * i2cm)
{
        xwer_t rc;

        rc = xwds_device_vop_resume(&i2cm->dev);
        return rc;
}
#endif

/******** ******** ******** APIs ******** ******** ********/
__xwds_api
xwer_t xwds_i2cm_xfer(struct xwds_i2cm * i2cm, struct xwds_i2c_msg * msg, xwtm_t to)
{
        xwer_t rc;
        const struct xwds_i2cm_driver * drv;

        XWDS_VALIDATE(i2cm, "nullptr", -EFAULT);
        XWDS_VALIDATE(msg, "nullptr", -EFAULT);
        XWDS_VALIDATE((msg->flag & XWDS_I2C_F_DIRMSK), "no-direction", -EINVAL);

        rc = xwds_i2cm_grab(i2cm);
        if (rc < 0) {
                goto err_i2cm_grab;
        }
        rc = xwos_mtx_lock_to(&i2cm->xfer.apimtx, to);
        if (rc < 0) {
                goto err_i2cm_lock;
        }
        drv = xwds_cast(const struct xwds_i2cm_driver *, i2cm->dev.drv);
        if ((drv) && (drv->xfer)) {
                rc = drv->xfer(i2cm, msg, to);
        } else {
                rc = -ENOSYS;
        }
        if (rc < 0) {
                goto err_drv_xfer;
        }
        xwos_mtx_unlock(&i2cm->xfer.apimtx);
        xwds_i2cm_put(i2cm);
        return XWOK;

err_drv_xfer:
        xwos_mtx_unlock(&i2cm->xfer.apimtx);
err_i2cm_lock:
        xwds_i2cm_put(i2cm);
err_i2cm_grab:
        return rc;
}

__xwds_api
xwer_t xwds_i2cm_abort(struct xwds_i2cm * i2cm,
                       xwu16_t address, xwu16_t addrmode,
                       xwtm_t to)
{
        xwer_t rc;
        const struct xwds_i2cm_driver * drv;

        XWDS_VALIDATE(i2cm, "nullptr", -EFAULT);

        rc = xwds_i2cm_grab(i2cm);
        if (rc < 0) {
                goto err_i2cm_grab;
        }
        rc = xwos_mtx_lock_to(&i2cm->abort.apimtx, to);
        if (rc < 0) {
                goto err_i2cm_lock;
        }
        drv = xwds_cast(const struct xwds_i2cm_driver *, i2cm->dev.drv);
        if ((drv) && (drv->abort)) {
                rc = drv->abort(i2cm, address, addrmode, to);
        } else {
                rc = -ENOSYS;
        }
        if (rc < 0) {
                goto err_drv_abort;
        }
        xwos_mtx_unlock(&i2cm->abort.apimtx);
        xwds_i2cm_put(i2cm);
        return XWOK;

err_drv_abort:
        xwos_mtx_unlock(&i2cm->abort.apimtx);
err_i2cm_lock:
        xwds_i2cm_put(i2cm);
err_i2cm_grab:
        return rc;
}
