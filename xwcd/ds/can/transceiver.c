/**
 * @file
 * @brief 玄武设备栈：CAN：总线接收器
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
#include <string.h>
#include <xwos/lib/xwlog.h>
#include <xwos/lib/xwaop.h>
#include <xwcd/ds/can/transceiver.h>

static __xwds_vop
xwer_t xwds_cantrcv_vop_probe(struct xwds_cantrcv * cantrcv);

static __xwds_vop
xwer_t xwds_cantrcv_vop_remove(struct xwds_cantrcv * cantrcv);

static __xwds_vop
xwer_t xwds_cantrcv_vop_start(struct xwds_cantrcv * cantrcv);

static __xwds_vop
xwer_t xwds_cantrcv_vop_stop(struct xwds_cantrcv * cantrcv);

#if defined(XWCDCFG_ds_PM) && (1 == XWCDCFG_ds_PM)
static __xwds_vop
xwer_t xwds_cantrcv_vop_suspend(struct xwds_cantrcv * cantrcv);

static __xwds_vop
xwer_t xwds_cantrcv_vop_resume(struct xwds_cantrcv * cantrcv);
#endif

__xwds_rodata const struct xwds_virtual_operation xwds_cantrcv_vop = {
        .probe = (void *)xwds_cantrcv_vop_probe,
        .remove = (void *)xwds_cantrcv_vop_remove,
        .start = (void *)xwds_cantrcv_vop_start,
        .stop = (void *)xwds_cantrcv_vop_stop,
#if defined(XWCDCFG_ds_PM) && (1 == XWCDCFG_ds_PM)
        .suspend = (void *)xwds_cantrcv_vop_suspend,
        .resume = (void *)xwds_cantrcv_vop_resume,
#endif
};

/******** ******** ******** constructor & destructor ******** ******** ********/
__xwds_api
void xwds_cantrcv_construct(struct xwds_cantrcv * cantrcv)
{
        xwds_device_construct(&cantrcv->bc.dev);
        cantrcv->bc.dev.vop = &xwds_cantrcv_vop;
}

__xwds_api
void xwds_cantrcv_destruct(struct xwds_cantrcv * cantrcv)
{
        xwds_device_destruct(&cantrcv->bc.dev);
}

__xwds_api
xwer_t xwds_cantrcv_grab(struct xwds_cantrcv * cantrcv)
{
        return xwds_device_grab(&cantrcv->bc.dev);
}

__xwds_api
xwer_t xwds_cantrcv_put(struct xwds_cantrcv * cantrcv)
{
        return xwds_device_put(&cantrcv->bc.dev);
}

/******** ******** base virtual operations ******** ********/
/**
 * @brief XWDS VOP：探测CAN接收器
 * @param[in] cantrcv: CAN接收器对象指针
 */
static __xwds_vop
xwer_t xwds_cantrcv_vop_probe(struct xwds_cantrcv * cantrcv)
{
        xwer_t rc;

        XWDS_VALIDATE(cantrcv, "nullptr", -EFAULT);
        XWDS_VALIDATE(cantrcv->cfg, "nullptr", -EFAULT);

        rc = xwds_device_vop_probe(&cantrcv->bc.dev);
        return rc;
}

/**
 * @brief XWDS VOP：移除CAN接收器
 * @param[in] cantrcv: CAN接收器对象指针
 */
static __xwds_vop
xwer_t xwds_cantrcv_vop_remove(struct xwds_cantrcv * cantrcv)
{
        xwer_t rc;

        XWDS_VALIDATE(cantrcv, "nullptr", -EFAULT);
        XWDS_VALIDATE(cantrcv->cfg, "nullptr", -EFAULT);

        rc = xwds_device_vop_remove(&cantrcv->bc.dev);
        return rc;
}

/**
 * @brief XWDS VOP：启动CAN接收器
 * @param[in] cantrcv: CAN接收器对象指针
 */
static __xwds_vop
xwer_t xwds_cantrcv_vop_start(struct xwds_cantrcv * cantrcv)
{
        xwer_t rc;

        XWDS_VALIDATE(cantrcv, "nullptr", -EFAULT);
        XWDS_VALIDATE(cantrcv->cfg, "nullptr", -EFAULT);

        rc = xwds_device_vop_start(&cantrcv->bc.dev);
        if (rc < 0) {
                goto err_dev_vop_start;
        }
        cantrcv->wkuprs = XWDS_CANTRCV_WKUPRS_NONE;
        rc = xwds_cantrcv_set_opmode(cantrcv, cantrcv->cfg->init_opmode);
        if (rc < 0) {
                goto err_set_opmode;
        }
        return XWOK;

err_set_opmode:
        xwds_device_vop_stop(&cantrcv->bc.dev);
err_dev_vop_start:
        return rc;
}

/**
 * @brief XWDS VOP：停止CAN接收器
 * @param[in] cantrcv: CAN接收器对象指针
 */
static __xwds_vop
xwer_t xwds_cantrcv_vop_stop(struct xwds_cantrcv * cantrcv)
{
        xwer_t rc;

        XWDS_VALIDATE(cantrcv, "nullptr", -EFAULT);

        rc = xwds_device_vop_stop(&cantrcv->bc.dev);
        return rc;
}

#if defined(XWCDCFG_ds_PM) && (1 == XWCDCFG_ds_PM)
/******** ******** pm ******** ********/
/**
 * @brief XWDS VOP：暂停CAN接收器
 * @param[in] cantrcv: CAN接收器对象指针
 */
static __xwds_vop
xwer_t xwds_cantrcv_vop_suspend(struct xwds_cantrcv * cantrcv)
{
        xwer_t rc;

        XWDS_VALIDATE(cantrcv, "nullptr", -EFAULT);

        rc = xwds_device_vop_suspend(&cantrcv->bc.dev);
        return rc;
}

/**
 * @brief XWDS VOP：继续CAN接收器
 * @param[in] cantrcv: CAN接收器对象指针
 */
static __xwds_vop
xwer_t xwds_cantrcv_vop_resume(struct xwds_cantrcv * cantrcv)
{
        xwer_t rc;

        XWDS_VALIDATE(cantrcv, "nullptr", -EFAULT);

        rc = xwds_device_vop_resume(&cantrcv->bc.dev);
        return rc;
}
#endif

/******** ******** ******** CAN operations ******** ******** ********/
__xwds_api
xwer_t xwds_cantrcv_set_opmode(struct xwds_cantrcv * cantrcv, xwsq_t opmode)
{
        xwer_t rc;
        const struct xwds_cantrcv_driver * drv;

        XWDS_VALIDATE(cantrcv, "nullptr", -EFAULT);
        XWDS_VALIDATE((opmode < XWDS_CANTRCV_OPMODE_NUM), "out-of-range", -ERANGE);

        drv = xwds_cast(const struct xwds_cantrcv_driver *, cantrcv->bc.dev.drv);
        if ((drv) && (drv->set_opmode)) {
                rc = drv->set_opmode(cantrcv, opmode);
        } else {
                rc = -ENOSYS;
        }
        if (rc < 0) {
                goto err_ops_set_opmode;
        }
        cantrcv->opmode = opmode;
        return XWOK;

err_ops_set_opmode:
        return rc;
}

__xwds_api
void xwds_cantrcv_get_opmode(struct xwds_cantrcv * cantrcv, xwsq_t * opmode)
{
        XWDS_VALIDATE(cantrcv, "nullptr");
        XWDS_VALIDATE(opmode, "nullptr");

        *opmode = cantrcv->opmode;
}

__xwds_api
xwer_t xwds_cantrcv_enable_wkup(struct xwds_cantrcv * cantrcv)
{
        xwer_t rc;
        const struct xwds_cantrcv_driver * drv;
        const struct xwds_cantrcv_cfg * cfg;

        XWDS_VALIDATE(cantrcv, "nullptr", -EFAULT);

        cfg = cantrcv->cfg;
        if (cfg->wkup_supported) {
                drv = xwds_cast(const struct xwds_cantrcv_driver *,
                                cantrcv->bc.dev.drv);
                if ((drv) && (drv->enable_wkup)) {
                        rc = drv->enable_wkup(cantrcv);
                } else {
                        rc = -ENOSYS;
                }
        } else {
                rc = -EOPNOTSUPP;
        }
        return rc;
}

__xwds_api
xwer_t xwds_cantrcv_disable_wkup(struct xwds_cantrcv * cantrcv)
{
        xwer_t rc;
        const struct xwds_cantrcv_driver * drv;
        const struct xwds_cantrcv_cfg * cfg;

        XWDS_VALIDATE(cantrcv, "nullptr", -EFAULT);

        cfg = cantrcv->cfg;
        if (cfg->wkup_supported) {
                drv = xwds_cast(const struct xwds_cantrcv_driver *,
                                cantrcv->bc.dev.drv);
                if ((drv) && (drv->disable_wkup)) {
                        rc = drv->disable_wkup(cantrcv);
                } else {
                        rc = -ENOSYS;
                }
        } else {
                rc = -EOPNOTSUPP;
        }
        return rc;
}

__xwds_api
void xwds_cantrcv_set_wkup_isr(struct xwds_cantrcv * cantrcv,
                               xwds_cantrcv_wkup_isr_f isr)
{
        cantrcv->wkup_isr = isr;
}

__xwds_api
void xwds_cantrcv_get_wkuprs(struct xwds_cantrcv * cantrcv, xwsq_t * wkuprs)
{
        const struct xwds_cantrcv_cfg * cfg;

        XWDS_VALIDATE(cantrcv, "nullptr");
        XWDS_VALIDATE(wkuprs, "nullptr");

        cfg = cantrcv->cfg;
        if (cfg->wkup_supported) {
                *wkuprs = cantrcv->wkuprs;
        } else {
                *wkuprs = XWDS_CANTRCV_WKUPRS_NOT_SUPPORTED;
        }
}

__xwds_api
void xwds_cantrcv_clear_wkuprs(struct xwds_cantrcv * cantrcv)
{
        XWDS_VALIDATE(cantrcv, "nullptr");

        cantrcv->wkuprs = XWDS_CANTRCV_WKUPRS_NONE;
}

__xwds_code
void xwds_cantrcv_drvcb_wakeup_notification(struct xwds_cantrcv * cantrcv)
{
        if (cantrcv->wkup_isr) {
                cantrcv->wkup_isr(cantrcv, cantrcv->canc);
        }/* else {} */
}
