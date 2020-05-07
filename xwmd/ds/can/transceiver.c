/**
 * @file
 * @brief xwmd设备栈：CAN总线接收器
 * @author
 * + 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwmd/ds/standard.h>
#include <xwos/lib/string.h>
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
 * @brief SODS API：CAN接收器的构造函数
 * @param cantrcv: (I) CAN接收器对象指针
 */
__xwds_api
void xwds_cantrcv_construct(struct xwds_cantrcv * cantrcv)
{
        xwds_device_construct(&cantrcv->bc.dev);
        cantrcv->bc.dev.cvops = &xwds_cantrcv_cvops;
}

/**
 * @brief SODS API：CAN接收器对象的析构函数
 * @param cantrcv: (I) CAN接收器对象指针
 */
__xwds_api
void xwds_cantrcv_destruct(struct xwds_cantrcv * cantrcv)
{
        xwds_device_destruct(&cantrcv->bc.dev);
}

/******** ******** base virtual operations ******** ********/
/**
 * @brief SODS VOP：探测CAN接收器
 * @param cantrcv: (I) CAN接收器对象指针
 * @return 错误码
 */
static __xwds_vop
xwer_t xwds_cantrcv_cvop_probe(struct xwds_cantrcv * cantrcv)
{
        xwer_t rc;

        SODS_VALIDATE(cantrcv, "nullptr", -EFAULT);
        SODS_VALIDATE(cantrcv->cfg, "nullptr", -EFAULT);

        rc = xwds_device_cvop_probe(&cantrcv->bc.dev);
        return rc;
}

/**
 * @brief SODS VOP：移除CAN接收器
 * @param cantrcv: (I) CAN接收器对象指针
 * @return 错误码
 */
static __xwds_vop
xwer_t xwds_cantrcv_cvop_remove(struct xwds_cantrcv * cantrcv)
{
        xwer_t rc;

        SODS_VALIDATE(cantrcv, "nullptr", -EFAULT);
        SODS_VALIDATE(cantrcv->cfg, "nullptr", -EFAULT);

        rc = xwds_device_cvop_remove(&cantrcv->bc.dev);
        return rc;
}

/**
 * @brief SODS VOP：启动CAN接收器
 * @param cantrcv: (I) CAN接收器对象指针
 * @return 错误码
 */
__xwds_api
xwer_t xwds_cantrcv_cvop_start(struct xwds_cantrcv * cantrcv)
{
        xwer_t rc;

        SODS_VALIDATE(cantrcv, "nullptr", -EFAULT);
        SODS_VALIDATE(cantrcv->cfg, "nullptr", -EFAULT);

        rc = xwds_device_cvop_start(&cantrcv->bc.dev);
        if (__unlikely(rc < 0)) {
                goto err_dev_cvop_start;
        }
        cantrcv->wkuprs = SODS_CANTRCV_WKUPRS_NONE;
        rc = xwds_cantrcv_set_opmode(cantrcv, cantrcv->cfg->init_opmode);
        if (__unlikely(rc < 0)) {
                goto err_set_opmode;
        }
        return OK;

err_set_opmode:
        xwds_device_cvop_stop(&cantrcv->bc.dev);
err_dev_cvop_start:
        return rc;
}

/**
 * @brief SODS VOP：停止CAN接收器
 * @param cantrcv: (I) CAN接收器对象指针
 * @return 错误码
 */
__xwds_api
xwer_t xwds_cantrcv_cvop_stop(struct xwds_cantrcv * cantrcv)
{
        xwer_t rc;

        SODS_VALIDATE(cantrcv, "nullptr", -EFAULT);

        rc = xwds_device_cvop_stop(&cantrcv->bc.dev);
        return rc;
}

#if defined(XWMDCFG_ds_PM) && (1 == XWMDCFG_ds_PM)
/******** ******** pm ******** ********/
/**
 * @brief SODS VOP：暂停CAN接收器
 * @param cantrcv: (I) CAN接收器对象指针
 * @return 错误码
 */
__xwds_api
xwer_t xwds_cantrcv_cvop_suspend(struct xwds_cantrcv * cantrcv)
{
        xwer_t rc;

        SODS_VALIDATE(cantrcv, "nullptr", -EFAULT);

        rc = xwds_device_cvop_suspend(&cantrcv->bc.dev);
        return rc;
}

/**
 * @brief SODS VOP：继续CAN接收器
 * @param cantrcv: (I) CAN接收器对象指针
 * @return 错误码
 */
__xwds_api
xwer_t xwds_cantrcv_cvop_resume(struct xwds_cantrcv * cantrcv)
{
        xwer_t rc;

        SODS_VALIDATE(cantrcv, "nullptr", -EFAULT);

        rc = xwds_device_cvop_resume(&cantrcv->bc.dev);
        return rc;
}
#endif /* XWMDCFG_ds_PM */

/******** ******** ******** CAN operations ******** ******** ********/
/**
 * @brief SODS API：设置CAN接收器的运行模式
 * @param cantrcv: (I) CAN接收器对象指针
 * @param opmode: (I) 唤醒模式
 * @return 错误码
 * @retval OK: OK
 * @retval -EFAULT: 空指针
 * @retval -ERANGE: 不支持的模式
 * @note
 * - 同步/异步：同步
 * - 中断上下文：可以使用
 * - 中断底半部：可以使用
 * - 线程上下文：可以使用
 * - 重入性：对于同一个设备不可重入
 */
__xwds_api
xwer_t xwds_cantrcv_set_opmode(struct xwds_cantrcv * cantrcv, xwsq_t opmode)
{
        xwer_t rc;
        const struct xwds_cantrcv_driver * drv;

        SODS_VALIDATE(cantrcv, "nullptr", -EFAULT);
        SODS_VALIDATE((opmode < SODS_CANTRCV_OPMODE_NUM), "out-of-range", -ERANGE);

        drv = xwds_static_cast(const struct xwds_cantrcv_driver *,
                               cantrcv->bc.dev.drv);
        if ((drv) && (drv->set_opmode)) {
                rc = drv->set_opmode(cantrcv, opmode);
        } else {
                rc = -ENOSYS;
        }
        if (__unlikely(rc < 0)) {
                goto err_ops_set_opmode;
        }
        cantrcv->opmode = opmode;
        return OK;

err_ops_set_opmode:
        return rc;
}

/**
 * @brief SODS API：设置CAN接收器的模式
 * @param cantrcv: (I) CAN接收器对象指针
 * @param opmode: (O) 指向缓冲区的指针，通过次缓冲区返回模式
 * @note
 * - 同步/异步：同步
 * - 中断上下文：可以使用
 * - 中断底半部：可以使用
 * - 线程上下文：可以使用
 * - 重入性：可重入
 */
__xwds_api
void xwds_cantrcv_get_opmode(struct xwds_cantrcv * cantrcv, xwsq_t * opmode)
{
        SODS_VALIDATE(cantrcv, "nullptr");
        SODS_VALIDATE(opmode, "nullptr");

        *opmode = cantrcv->opmode;
}

/**
 * @brief SODS API：开启CAN接收器的唤醒
 * @param cantrcv: (I) CAN接收器对象指针
 * @return 错误码
 * @retval OK: OK
 * @retval -EFAULT: 空指针
 * @retval -EOPNOTSUPP: 不支持此API
 * @note
 * - 同步/异步：同步
 * - 中断上下文：可以使用
 * - 中断底半部：可以使用
 * - 线程上下文：可以使用
 * - 重入性：对于同一个设备不可重入
 */
__xwds_api
xwer_t xwds_cantrcv_enable_wkup(struct xwds_cantrcv * cantrcv)
{
        xwer_t rc;
        const struct xwds_cantrcv_driver * drv;
        const struct xwds_cantrcv_cfg * cfg;

        SODS_VALIDATE(cantrcv, "nullptr", -EFAULT);

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

/**
 * @brief SODS API：关闭CAN接收器的唤醒
 * @param cantrcv: (I) CAN接收器对象指针
 * @return 错误码
 * @retval OK: OK
 * @retval -EFAULT: 空指针
 * @retval -EOPNOTSUPP: 不支持此API
 * @note
 * - 同步/异步：同步
 * - 中断上下文：可以使用
 * - 中断底半部：可以使用
 * - 线程上下文：可以使用
 * - 重入性：对于同一个设备不可重入；对于不同设备可重入
 */
__xwds_api
xwer_t xwds_cantrcv_disable_wkup(struct xwds_cantrcv * cantrcv)
{
        xwer_t rc;
        const struct xwds_cantrcv_driver * drv;
        const struct xwds_cantrcv_cfg * cfg;

        SODS_VALIDATE(cantrcv, "nullptr", -EFAULT);

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

/**
 * @brief XWMD API：设置唤醒中断的回调函数
 * @param cantrcv: (I) CAN接收器对象指针
 * @param isr: (I) 回调函数
 * @note
 * - 同步/异步：同步
 * - 中断上下文：可以使用
 * - 中断底半部：可以使用
 * - 线程上下文：可以使用
 * - 重入性：不可重入，必须调用xwds_cantrcv_disable_wkup()后才可修改回调函数
 */
__xwds_api
void xwds_cantrcv_set_wkup_isr(struct xwds_cantrcv * cantrcv,
                               xwds_cantrcv_wkup_isr_f isr)
{
        cantrcv->wkup_isr = isr;
}

/**
 * @brief SODS API：获取CAN接收器的唤醒原因
 * @param cantrcv: (I) CAN接收器对象指针
 * @param wkuprs: (O) 指向缓冲区的指针，通过次缓冲区返回唤醒原因
 * @note
 * - 同步/异步：同步
 * - 中断上下文：可以使用
 * - 中断底半部：可以使用
 * - 线程上下文：可以使用
 * - 重入性：可重入
 */
__xwds_api
void xwds_cantrcv_get_wkuprs(struct xwds_cantrcv * cantrcv, xwsq_t * wkuprs)
{
        const struct xwds_cantrcv_cfg * cfg;

        SODS_VALIDATE(cantrcv, "nullptr");
        SODS_VALIDATE(wkuprs, "nullptr");

        cfg = cantrcv->cfg;
        if (cfg->wkup_supported) {
                *wkuprs = cantrcv->wkuprs;
        } else {
                *wkuprs = SODS_CANTRCV_WKUPRS_NOT_SUPPORTED;
        }
}

/**
 * @brief SODS API：清除CAN接收器的唤醒原因
 * @param cantrcv: (I) CAN接收器对象指针
 * @note
 * - 同步/异步：同步
 * - 中断上下文：可以使用
 * - 中断底半部：可以使用
 * - 线程上下文：可以使用
 * - 重入性：对于同一个设备不可重入；对于不同设备可重入
 */
__xwds_api
void xwds_cantrcv_clear_wkuprs(struct xwds_cantrcv * cantrcv)
{
        SODS_VALIDATE(cantrcv, "nullptr");

        cantrcv->wkuprs = SODS_CANTRCV_WKUPRS_NONE;
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
