/**
 * @file
 * @brief xwmd设备栈：CAN总线接收器
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
#include <string.h>
#include <xwos/lib/xwlog.h>
#include <xwos/lib/xwbmpaop.h>
#include <xwmd/ds/can/transceiver.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********     static function prototypes      ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
static __xwds_vop
xwer_t xwds_cantrcv_cvop_probe(struct xwds_cantrcv * cantrcv);

static __xwds_vop
xwer_t xwds_cantrcv_cvop_remove(struct xwds_cantrcv * cantrcv);

static __xwds_vop
xwer_t xwds_cantrcv_cvop_start(struct xwds_cantrcv * cantrcv);

static __xwds_vop
xwer_t xwds_cantrcv_cvop_stop(struct xwds_cantrcv * cantrcv);

#if defined(XWMDCFG_ds_PM) && (1 == XWMDCFG_ds_PM)
static __xwds_vop
xwer_t xwds_cantrcv_cvop_suspend(struct xwds_cantrcv * cantrcv);

static __xwds_vop
xwer_t xwds_cantrcv_cvop_resume(struct xwds_cantrcv * cantrcv);
#endif /* XWMDCFG_ds_PM */

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
__xwds_rodata const struct xwds_base_virtual_operations xwds_cantrcv_cvops = {
        .probe = (void *)xwds_cantrcv_cvop_probe,
        .remove = (void *)xwds_cantrcv_cvop_remove,
        .start = (void *)xwds_cantrcv_cvop_start,
        .stop = (void *)xwds_cantrcv_cvop_stop,
#if defined(XWMDCFG_ds_PM) && (1 == XWMDCFG_ds_PM)
        .suspend = (void *)xwds_cantrcv_cvop_suspend,
        .resume = (void *)xwds_cantrcv_cvop_resume,
#endif /* XWMDCFG_ds_PM */
};

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/******** ******** ******** constructor & destructor ******** ******** ********/
/**
 * @brief XWDS API：CAN接收器的构造函数
 * @param cantrcv: (I) CAN接收器对象指针
 */
__xwds_api
void xwds_cantrcv_construct(struct xwds_cantrcv * cantrcv)
{
        xwds_device_construct(&cantrcv->bc.dev);
        cantrcv->bc.dev.cvops = &xwds_cantrcv_cvops;
}

/**
 * @brief XWDS API：CAN接收器对象的析构函数
 * @param cantrcv: (I) CAN接收器对象指针
 */
__xwds_api
void xwds_cantrcv_destruct(struct xwds_cantrcv * cantrcv)
{
        xwds_device_destruct(&cantrcv->bc.dev);
}

/******** ******** base virtual operations ******** ********/
/**
 * @brief XWDS VOP：探测CAN接收器
 * @param cantrcv: (I) CAN接收器对象指针
 * @return 错误码
 */
static __xwds_vop
xwer_t xwds_cantrcv_cvop_probe(struct xwds_cantrcv * cantrcv)
{
        xwer_t rc;

        XWDS_VALIDATE(cantrcv, "nullptr", -EFAULT);
        XWDS_VALIDATE(cantrcv->cfg, "nullptr", -EFAULT);

        rc = xwds_device_cvop_probe(&cantrcv->bc.dev);
        return rc;
}

/**
 * @brief XWDS VOP：移除CAN接收器
 * @param cantrcv: (I) CAN接收器对象指针
 * @return 错误码
 */
static __xwds_vop
xwer_t xwds_cantrcv_cvop_remove(struct xwds_cantrcv * cantrcv)
{
        xwer_t rc;

        XWDS_VALIDATE(cantrcv, "nullptr", -EFAULT);
        XWDS_VALIDATE(cantrcv->cfg, "nullptr", -EFAULT);

        rc = xwds_device_cvop_remove(&cantrcv->bc.dev);
        return rc;
}

/**
 * @brief XWDS VOP：启动CAN接收器
 * @param cantrcv: (I) CAN接收器对象指针
 * @return 错误码
 */
__xwds_api
xwer_t xwds_cantrcv_cvop_start(struct xwds_cantrcv * cantrcv)
{
        xwer_t rc;

        XWDS_VALIDATE(cantrcv, "nullptr", -EFAULT);
        XWDS_VALIDATE(cantrcv->cfg, "nullptr", -EFAULT);

        rc = xwds_device_cvop_start(&cantrcv->bc.dev);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_dev_cvop_start;
        }
        cantrcv->wkuprs = XWDS_CANTRCV_WKUPRS_NONE;
        rc = xwds_cantrcv_set_opmode(cantrcv, cantrcv->cfg->init_opmode);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_set_opmode;
        }
        return XWOK;

err_set_opmode:
        xwds_device_cvop_stop(&cantrcv->bc.dev);
err_dev_cvop_start:
        return rc;
}

/**
 * @brief XWDS VOP：停止CAN接收器
 * @param cantrcv: (I) CAN接收器对象指针
 * @return 错误码
 */
__xwds_api
xwer_t xwds_cantrcv_cvop_stop(struct xwds_cantrcv * cantrcv)
{
        xwer_t rc;

        XWDS_VALIDATE(cantrcv, "nullptr", -EFAULT);

        rc = xwds_device_cvop_stop(&cantrcv->bc.dev);
        return rc;
}

#if defined(XWMDCFG_ds_PM) && (1 == XWMDCFG_ds_PM)
/******** ******** pm ******** ********/
/**
 * @brief XWDS VOP：暂停CAN接收器
 * @param cantrcv: (I) CAN接收器对象指针
 * @return 错误码
 */
__xwds_api
xwer_t xwds_cantrcv_cvop_suspend(struct xwds_cantrcv * cantrcv)
{
        xwer_t rc;

        XWDS_VALIDATE(cantrcv, "nullptr", -EFAULT);

        rc = xwds_device_cvop_suspend(&cantrcv->bc.dev);
        return rc;
}

/**
 * @brief XWDS VOP：继续CAN接收器
 * @param cantrcv: (I) CAN接收器对象指针
 * @return 错误码
 */
__xwds_api
xwer_t xwds_cantrcv_cvop_resume(struct xwds_cantrcv * cantrcv)
{
        xwer_t rc;

        XWDS_VALIDATE(cantrcv, "nullptr", -EFAULT);

        rc = xwds_device_cvop_resume(&cantrcv->bc.dev);
        return rc;
}
#endif /* XWMDCFG_ds_PM */

/******** ******** ******** CAN operations ******** ******** ********/
__xwds_api
xwer_t xwds_cantrcv_set_opmode(struct xwds_cantrcv * cantrcv, xwsq_t opmode)
{
        xwer_t rc;
        const struct xwds_cantrcv_driver * drv;

        XWDS_VALIDATE(cantrcv, "nullptr", -EFAULT);
        XWDS_VALIDATE((opmode < XWDS_CANTRCV_OPMODE_NUM), "out-of-range", -ERANGE);

        drv = xwds_static_cast(const struct xwds_cantrcv_driver *,
                               cantrcv->bc.dev.drv);
        if ((drv) && (drv->set_opmode)) {
                rc = drv->set_opmode(cantrcv, opmode);
        } else {
                rc = -ENOSYS;
        }
        if (__xwcc_unlikely(rc < 0)) {
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
                drv = xwds_static_cast(const struct xwds_cantrcv_driver *,
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
                drv = xwds_static_cast(const struct xwds_cantrcv_driver *,
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

/**
 * @brief CAN接收器库函数：CAN接收器的唤醒通知
 * @param cantrcv: (I) CAN接收器对象指针
 * @note
 * - 这个回调函数在中断上下文中被调用，用于通知唤醒
 */
__xwds_code
void xwds_cantrcv_lib_wakeup_notification(struct xwds_cantrcv * cantrcv)
{
        if (cantrcv->wkup_isr) {
                cantrcv->wkup_isr(cantrcv, cantrcv->canc);
        }/* else {} */
}
