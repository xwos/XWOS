/**
 * @file
 * @brief xwmd设备栈：CAN总线控制器
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
#include <xwos/lib/xwlog.h>
#include <xwos/lib/xwbmpaop.h>
#include <xwos/osal/lock/spinlock.h>
#include <xwos/osal/sync/semaphore.h>
#include <xwmd/ds/can/controller.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********     static function prototypes      ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
static __xwds_vop
xwer_t xwds_canc_cvop_probe(struct xwds_canc * canc);

static __xwds_vop
xwer_t xwds_canc_cvop_remove(struct xwds_canc * canc);

static __xwds_vop
xwer_t xwds_canc_cvop_start(struct xwds_canc * canc);

static __xwds_vop
xwer_t xwds_canc_cvop_stop(struct xwds_canc * canc);

#if defined(XWMDCFG_ds_PM) && (1 == XWMDCFG_ds_PM)
static __xwds_vop
xwer_t xwds_canc_cvop_suspend(struct xwds_canc * canc);

static __xwds_vop
xwer_t xwds_canc_cvop_resume(struct xwds_canc * canc);
#endif /* XWMDCFG_ds_PM */

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
__xwds_rodata const struct xwds_base_virtual_operations xwds_canc_cvops = {
        .probe = (void *)xwds_canc_cvop_probe,
        .remove = (void *)xwds_canc_cvop_remove,
        .start = (void *)xwds_canc_cvop_start,
        .stop = (void *)xwds_canc_cvop_stop,
#if defined(XWMDCFG_ds_PM) && (1 == XWMDCFG_ds_PM)
        .suspend = (void *)xwds_canc_cvop_suspend,
        .resume = (void *)xwds_canc_cvop_resume,
#endif /* XWMDCFG_ds_PM */
};

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/******** ******** ******** constructor & destructor ******** ******** ********/
/**
 * @brief XWDS API：CAN控制器的构造函数
 * @param canc: (I) CAN控制器对象指针
 */
__xwds_api
void xwds_canc_construct(struct xwds_canc * canc)
{
        xwds_device_construct(&canc->bc.dev);
        canc->bc.dev.cvops = &xwds_canc_cvops;
}

/**
 * @brief XWDS API：CAN控制器对象的析构函数
 * @param canc: (I) CAN控制器对象指针
 */
__xwds_api
void xwds_canc_destruct(struct xwds_canc * canc)
{
        xwds_device_destruct(&canc->bc.dev);
}

/******** ******** base virtual operations ******** ********/
/**
 * @brief XWDS VOP：探测CAN控制器
 * @param canc: (I) CAN控制器对象指针
 * @return 错误码
 */
static __xwds_vop
xwer_t xwds_canc_cvop_probe(struct xwds_canc * canc)
{
        xwer_t rc;

        XWDS_VALIDATE(canc->cfg, "nullptr", -EFAULT);

        canc->mode = XWDS_CANC_MODE_UNINIT;
        rc = xwds_device_cvop_probe(&canc->bc.dev);
        return rc;
}

/**
 * @brief XWDS VOP：移除CAN控制器
 * @param canc: (I) CAN控制器对象指针
 * @return 错误码
 */
static __xwds_vop
xwer_t xwds_canc_cvop_remove(struct xwds_canc * canc)
{
        xwer_t rc;

        rc = xwds_device_cvop_remove(&canc->bc.dev);
        return rc;
}

/**
 * @brief XWDS VOP：启动CAN控制器
 * @param canc: (I) CAN控制器对象指针
 * @return 错误码
 */
static __xwds_vop
xwer_t xwds_canc_cvop_start(struct xwds_canc * canc)
{
        xwer_t rc;

        rc = xwds_device_cvop_start(&canc->bc.dev);
        return rc;
}

/**
 * @brief XWDS VOP：停止CAN控制器
 * @param canc: (I) CAN控制器对象指针
 * @return 错误码
 */
static __xwds_vop
xwer_t xwds_canc_cvop_stop(struct xwds_canc * canc)
{
        xwer_t rc;

        rc = xwds_device_cvop_stop(&canc->bc.dev);
        return rc;
}

#if defined(XWMDCFG_ds_PM) && (1 == XWMDCFG_ds_PM)
/******** ******** pm ******** ********/
/**
 * @brief XWDS VOP：暂停CAN控制器
 * @param canc: (I) CAN控制器对象指针
 * @return 错误码
 */
static __xwds_vop
xwer_t xwds_canc_cvop_suspend(struct xwds_canc * canc)
{
        xwer_t rc;

        rc = xwds_device_cvop_suspend(&canc->bc.dev);
        return rc;
}

/**
 * @brief XWDS VOP：继续CAN控制器
 * @param canc: (I) CAN控制器对象指针
 * @return 错误码
 */
static __xwds_vop
xwer_t xwds_canc_cvop_resume(struct xwds_canc * canc)
{
        xwer_t rc;

        rc = xwds_device_cvop_resume(&canc->bc.dev);
        return rc;
}
#endif /* XWMDCFG_ds_PM */

/******** ******** ******** APIs ******** ******** ********/
__xwds_api
xwer_t xwds_canc_write(struct xwds_canc * canc, xwid_t txobjid,
                       struct xwds_can_msg * msg)
{
        const struct xwds_canc_txobj_cfg * txobjcfg;
        const struct xwds_canc_driver * drv;
        xwer_t rc;

        XWDS_VALIDATE(canc, "nullptr", -EFAULT);
        XWDS_VALIDATE(msg, "nullptr", -EFAULT);

        rc = xwds_canc_grab(canc);
        if (__unlikely(rc < 0)) {
                goto err_canc_grab;
        }

        if (txobjid >= canc->cfg->txobjs_num) {
                rc = -ERANGE;
                goto err_txobjid;
        }

        rc = xwds_canc_request(canc);
        if (__unlikely(rc < 0)) {
                goto err_canc_request;
        }

        txobjcfg = &canc->cfg->txobjs[txobjid];
        if (XWDS_CAN_MSG_F_EXID & msg->flag) {
                if (!(XWDS_CANC_HWOBJ_T_ID_EXT & txobjcfg->type)) {
                        rc = -EBADSLT;
                        goto err_badtxobj;
                }
        } else {
                if (!(XWDS_CANC_HWOBJ_T_ID_STD & txobjcfg->type)) {
                        rc = -EBADSLT;
                        goto err_badtxobj;
                }
        }
        drv = xwds_static_cast(const struct xwds_canc_driver *, canc->bc.dev.drv);
        if ((drv) && (drv->write)) {
                rc = drv->write(canc, txobjcfg, msg);
        } else {
                rc = -ENOSYS;
        }
        if (__unlikely(rc < 0)) {
                goto err_drv_write;
        }

        xwds_canc_release(canc);
        xwds_canc_put(canc);

        return XWOK;

err_drv_write:
err_badtxobj:
        xwds_canc_release(canc);
err_canc_request:
err_txobjid:
        xwds_canc_put(canc);
err_canc_grab:
        return rc;
}

__xwds_api
xwer_t xwds_canc_set_mode(struct xwds_canc * canc, xwsq_t mode)
{
        const struct xwds_canc_driver * drv;
        xwer_t rc;

        XWDS_VALIDATE(canc, "nullptr", -EFAULT);
        XWDS_VALIDATE((mode < XWDS_CANC_MODE_NUM), "out-of-range", -ERANGE);

        rc = xwds_canc_grab(canc);
        if (__unlikely(rc < 0)) {
                goto err_canc_grab;
        }
        rc = xwds_canc_request(canc);
        if (__unlikely(rc < 0)) {
                goto err_canc_request;
        }

        if (canc->mode == mode) {
                rc = -EALREADY;
                goto err_same_mode;
        }
        drv = xwds_static_cast(const struct xwds_canc_driver *, canc->bc.dev.drv);
        if ((drv) && (drv->set_mode)) {
                rc = drv->set_mode(canc, mode);
        } else {
                rc = -ENOSYS;
        }
        if (__unlikely(rc < 0)) {
                goto err_drv_set_mode;
        }
        canc->mode = mode;

        xwds_canc_release(canc);
        xwds_canc_put(canc);

        xwds_canc_lib_mode_indication(canc, mode);
        return XWOK;

err_drv_set_mode:
err_same_mode:
        xwds_canc_release(canc);
err_canc_request:
        xwds_canc_put(canc);
err_canc_grab:
        return rc;
}

__xwds_api
xwer_t xwds_canc_set_bd(struct xwds_canc * canc, xwid_t bdcfgid)
{
        xwer_t rc;
        const struct xwds_canc_driver * drv;
        const struct xwds_canc_bd_cfg * bdcfg;

        XWDS_VALIDATE(canc, "nullptr", -EFAULT);

        rc = xwds_canc_grab(canc);
        if (__unlikely(rc < 0)) {
                goto err_canc_grab;
        }
        rc = xwds_canc_request(canc);
        if (__unlikely(rc < 0)) {
                goto err_canc_request;
        }

        if (bdcfgid >= canc->cfg->bdcfgs_num) {
                rc = -ERANGE;
                goto err_nobd;
        }

        bdcfg = &canc->cfg->bdcfgs[bdcfgid];
        drv = xwds_static_cast(const struct xwds_canc_driver *, canc->bc.dev.drv);
        if ((drv) && (drv->set_bd)) {
                rc = drv->set_bd(canc, bdcfg);
        } else {
                rc = -ENOSYS;
        }
        if (__unlikely(rc < 0)) {
                goto err_drv_set_bd;
        }

        xwds_canc_release(canc);
        xwds_canc_put(canc);
        return XWOK;

err_drv_set_bd:
err_nobd:
        xwds_canc_release(canc);
err_canc_request:
        xwds_canc_put(canc);
err_canc_grab:
        return rc;
}

__xwds_api
xwer_t xwds_canc_enable_irqs(struct xwds_canc * canc)
{
        const struct xwds_canc_driver * drv;
        xwer_t rc;

        XWDS_VALIDATE(canc, "nullptr", -EFAULT);

        rc = xwds_canc_grab(canc);
        if (__unlikely(rc < 0)) {
                goto err_canc_grab;
        }
        rc = xwds_canc_request(canc);
        if (__unlikely(rc < 0)) {
                goto err_canc_request;
        }

        drv = xwds_static_cast(const struct xwds_canc_driver *, canc->bc.dev.drv);
        if ((drv) && (drv->enable_irqs)) {
                rc = drv->enable_irqs(canc);
        } else {
                rc = -ENOSYS;
        }
        if (__unlikely(rc < 0)) {
                goto err_canc_drv_enable_irqs;
        }

        return XWOK;

err_canc_drv_enable_irqs:
        xwds_canc_release(canc);
err_canc_request:
        xwds_canc_put(canc);
err_canc_grab:
        return rc;
}

__xwds_api
xwer_t xwds_canc_disable_irqs(struct xwds_canc * canc)
{
        const struct xwds_canc_driver * drv;
        xwer_t rc;

        XWDS_VALIDATE(canc, "nullptr", -EFAULT);

        drv = xwds_static_cast(const struct xwds_canc_driver *, canc->bc.dev.drv);
        if ((drv) && (drv->disable_irqs)) {
                rc = drv->disable_irqs(canc);
        } else {
                rc = -ENOSYS;
        }
        if (__unlikely(rc < 0)) {
                goto err_canc_drv_disable_irqs;
        }

        xwds_canc_release(canc);
        xwds_canc_put(canc);
        return XWOK;

err_canc_drv_disable_irqs:
        return rc;
}

/******** ******** CAN controller Libraries for BSP driver ******** ********/
/**
 * @brief XWDS LIB：设置“指示发送结果”的回调函数
 * @param canc: (I) CAN总线消息结构体的指针
 * @param cb: (I) 回调函数
 */
__xwds_lib_code
void xwds_canc_lib_setcb_tx_indication(struct xwds_canc * canc,
                                       void (*cb)(struct xwds_canc *, xwid_t, xwer_t))
{
#if !defined(XWMDCFG_ds_CAN_CONTROLLER_ROCBT) || (1 != XWMDCFG_ds_CAN_CONTROLLER_ROCBT)
        if (canc->cbtbl) {
                canc->cbtbl->tx_indication = cb;
        }/* else {} */
#else
        XWOS_UNUSED(canc);
        XWOS_UNUSED(cb);
#endif
}

/**
 * @brief XWDS LIB：设置“指示接收结果”的回调函数
 * @param canc: (I) CAN总线消息结构体的指针
 * @param cb: (I) 回调函数
 */
__xwds_lib_code
void xwds_canc_lib_setcb_rx_indication(struct xwds_canc * canc,
                                       void (*cb)(struct xwds_canc *,
                                                  xwid_t,
                                                  struct xwds_can_msg *))
{
#if !defined(XWMDCFG_ds_CAN_CONTROLLER_ROCBT) || (1 != XWMDCFG_ds_CAN_CONTROLLER_ROCBT)
        if (canc->cbtbl) {
                canc->cbtbl->rx_indication = cb;
        }/* else {} */
#else
        XWOS_UNUSED(canc);
        XWOS_UNUSED(cb);
#endif
}

/**
 * @brief XWDS LIB：设置“唤醒通知”回调函数
 * @param canc: (I) CAN总线消息结构体的指针
 * @param cb: (I) 回调函数
 */
__xwds_lib_code
void xwds_canc_lib_setcb_wakeup_notification(struct xwds_canc * canc,
                                             void (*cb)(struct xwds_canc *))
{
#if !defined(XWMDCFG_ds_CAN_CONTROLLER_ROCBT) || (1 != XWMDCFG_ds_CAN_CONTROLLER_ROCBT)
        if (canc->cbtbl) {
                canc->cbtbl->wakeup_notification = cb;
        }/* else {} */
#else
        XWOS_UNUSED(canc);
        XWOS_UNUSED(cb);
#endif
}

/**
 * @brief XWDS LIB：设置“模式切换通知”回调函数
 * @param canc: (I) CAN总线消息结构体的指针
 * @param cb: (I) 回调函数
 */
__xwds_lib_code
void xwds_canc_lib_setcb_mode_indication(struct xwds_canc * canc,
                                         void (*cb)(struct xwds_canc *, xwsq_t))
{
#if !defined(XWMDCFG_ds_CAN_CONTROLLER_ROCBT) || (1 != XWMDCFG_ds_CAN_CONTROLLER_ROCBT)
        if (canc->cbtbl) {
                canc->cbtbl->mode_indication = cb;
        }/* else {} */
#else
        XWOS_UNUSED(canc);
        XWOS_UNUSED(cb);
#endif
}

/**
 * @brief XWDS LIB：设置“错误通知”回调函数
 * @param canc: (I) CAN总线消息结构体的指针
 * @param cb: (I) 回调函数
 */
__xwds_lib_code
void xwds_canc_lib_setcb_err_indication(struct xwds_canc * canc,
                                        void (*cb)(struct xwds_canc *,
                                                   xwsq_t, xwsq_t, xwsq_t))
{
#if !defined(XWMDCFG_ds_CAN_CONTROLLER_ROCBT) || (1 != XWMDCFG_ds_CAN_CONTROLLER_ROCBT)
        if (canc->cbtbl) {
                canc->cbtbl->err_indication = cb;
        }/* else {} */
#else
        XWOS_UNUSED(canc);
        XWOS_UNUSED(cb);
#endif
}

/**
 * @brief XWDS LIB：设置“BUSOFF通知”回调函数
 * @param canc: (I) CAN总线消息结构体的指针
 * @param cb: (I) 回调函数
 */
__xwds_lib_code
void xwds_canc_lib_setcb_busoff_indication(struct xwds_canc * canc,
                                           void (*cb)(struct xwds_canc *))
{
#if !defined(XWMDCFG_ds_CAN_CONTROLLER_ROCBT) || (1 != XWMDCFG_ds_CAN_CONTROLLER_ROCBT)
        if (canc->cbtbl) {
                canc->cbtbl->busoff_indication = cb;
        }/* else {} */
#else
        XWOS_UNUSED(canc);
        XWOS_UNUSED(cb);
#endif
}

/**
 * @brief XWDS LIB：初始化CAN总线消息结构体
 * @param msg: (I) CAN总线消息结构体的指针
 * @param sdu: (I) 数据缓冲区的指针
 */
__xwds_lib_code
void xwds_canc_lib_init_msg(struct xwds_can_msg * msg, xwu32_t canid,
                            xwsq_t flag, xwsz_t dlc, xwu8_t sdu[])
{
        msg->id = canid;
        msg->flag = flag;
        msg->dlc = dlc;
        if (sdu) {
                memcpy(msg->sdu, sdu, dlc);
        } else {
                memset(msg->sdu, 0, XWDS_CANC_SDU_MAXSIZE);
        }
}

/**
 * @brief XWDS LIB：指示发送结果
 * @param canc: (I) CAN控制器对象指针
 * @param txobjid: (I) 发送邮箱的ID
 * @param rc: (I) 发送结果
 * @note
 * - 这个回调函数在中断上下文中被调用，用于通知接收结果
 */
__xwds_lib_code
void xwds_canc_lib_tx_indication(struct xwds_canc * canc, xwid_t txobjid,
                                 xwer_t rc)
{
        __xwds_canc_cbtbl_qualifier struct xwds_canc_cbtbl * cbtbl;

        cbtbl = canc->cbtbl;
        if ((cbtbl) && (cbtbl->tx_indication)) {
                cbtbl->tx_indication(canc, txobjid, rc);
        }/* else {} */
}

/**
 * @brief XWDS LIB：指示接收到CAN消息
 * @param canc: (I) CAN控制器对象指针
 * @param rxobjid: (I) 接收邮箱的ID
 * @param rxmsg: (O) 指向缓冲区的指针，通过此缓冲区返回接收到的CAN消息
 * @note
 * - 此回调函数在中断上下文中调用，用于通知接收结果；
 * - rxmsg的生命周期只在这个函数内有效，若要保存CAN数据，应在函数内部进行拷贝。
 */
__xwds_lib_code
void xwds_canc_lib_rx_indication(struct xwds_canc * canc, xwid_t rxobjid,
                                 struct xwds_can_msg * rxmsg)
{
        __xwds_canc_cbtbl_qualifier struct xwds_canc_cbtbl * cbtbl;

        cbtbl = canc->cbtbl;
        if ((cbtbl) && (cbtbl->rx_indication)) {
                cbtbl->rx_indication(canc, rxobjid, rxmsg);
        }/* else {} */
}

/**
 * @brief XWDS LIB：CAN控制器的唤醒通知
 * @param canc: (I) CAN控制器对象指针
 * @note
 * - 这个回调函数在中断上下文中被调用，用于通知唤醒
 */
__xwds_lib_code
void xwds_canc_lib_wakeup_notification(struct xwds_canc * canc)
{
        __xwds_canc_cbtbl_qualifier struct xwds_canc_cbtbl * cbtbl;

        cbtbl = canc->cbtbl;
        if ((cbtbl) && (cbtbl->wakeup_notification)) {
                cbtbl->wakeup_notification(canc);
        }/* else {} */
}

/**
 * @brief XWDS LIB：指示CAN控制器的模式已经切换
 * @param canc: (I) CAN控制器对象指针
 * @param mode: (I) 当前模式
 * @note
 * - 这个回调函数在中断上下文中被调用，用于通知模式切换
 */
__xwds_lib_code
void xwds_canc_lib_mode_indication(struct xwds_canc * canc, xwsq_t mode)
{
        __xwds_canc_cbtbl_qualifier struct xwds_canc_cbtbl * cbtbl;

        cbtbl = canc->cbtbl;
        if ((cbtbl) && (cbtbl->mode_indication)) {
                cbtbl->mode_indication(canc, mode);
        }/* else {} */
}

/**
 * @brief XWDS LIB：指示CAN控制器发生错误
 * @param canc: (I) CAN控制器对象指针
 * @param errcode: (I) 错误码
 * @param tec: (I) 发送错误计数
 * @param rec: (I) 接收错误计数
 * @note
 * - 这个回调函数在中断上下文中被调用，用于通知错误
 */
__xwds_lib_code
void xwds_canc_lib_err_indication(struct xwds_canc * canc, xwsq_t errcode,
                                  xwsq_t tec, xwsq_t rec)
{
        __xwds_canc_cbtbl_qualifier struct xwds_canc_cbtbl * cbtbl;

        cbtbl = canc->cbtbl;
        if ((cbtbl) && (cbtbl->err_indication)) {
                cbtbl->err_indication(canc, errcode, tec, rec);
        }/* else {} */
}

/**
 * @brief XWDS LIB：指示CAN控制器发生busoff
 * @param canc: (I) CAN控制器对象指针
 * @note
 * - 这个回调函数在中断上下文中被调用，用于通知BUSOFF
 */
__xwds_lib_code
void xwds_canc_lib_busoff_indication(struct xwds_canc * canc)
{
        __xwds_canc_cbtbl_qualifier struct xwds_canc_cbtbl * cbtbl;

        cbtbl = canc->cbtbl;
        if ((cbtbl) && (cbtbl->busoff_indication)) {
                cbtbl->busoff_indication(canc);
        }/* else {} */
}

/**
 * @brief XWDS LIB：初始化接收缓冲队列
 * @param rxq: (I) 接收缓冲队列对象指针
 */
__xwds_lib_code
void xwds_canc_lib_rxq_init(struct xwds_canc_rxqueue * rxq)
{
        xwosal_splk_init(&rxq->lock);
        memset(rxq->q, 0, sizeof(rxq->q));
        rxq->pos = 0;
        rxq->num = 0;
        xwosal_smr_init(&rxq->smr, 0, XWDS_CANC_RXQNUM);
}

/**
 * @brief XWDS LIB：发布一条消息到接收缓冲队列中
 * @param rxq: (I) 接收缓冲队列对象指针
 * @param msg: (I) 待发布的CAN消息结构体指针
 */
__xwds_lib_code
void xwds_canc_lib_rxq_publish(struct xwds_canc_rxqueue * rxq,
                               struct xwds_can_msg * msg)
{
        xwreg_t cpuirq;

        xwosal_splk_lock_cpuirqsv(&rxq->lock, &cpuirq);
        memcpy(&rxq->q[rxq->num], msg, sizeof(struct xwds_can_msg));
        rxq->num++;
        if (rxq->num >= (xwssz_t)XWDS_CANC_RXQNUM) {
                rxq->num = 0;
        } /* else {} */
        if (rxq->num == rxq->pos) {
                /* Queue is overflow. Discard the oldest data */
                rxq->pos++;
                if (rxq->pos >= (xwssq_t)XWDS_CANC_RXQNUM) {
                        rxq->pos = 0;
                }/* else {} */
                xwosal_splk_unlock_cpuirqrs(&rxq->lock, cpuirq);
        } else {
                xwosal_splk_unlock_cpuirqrs(&rxq->lock, cpuirq);
                xwosal_smr_post(xwosal_smr_get_id(&rxq->smr));
        }
}

/**
 * @brief XWDS LIB：从接收缓冲队列中获取一条消息
 * @param rxq: (I) 接收缓冲队列对象指针
 * @param buf: (I) 获取CAN消息结构体的缓存指针
 * @param xwtm: 指向缓冲区的指针，此缓冲区：
 *              (I) 作为输入时，表示期望的阻塞等待时间
 *              (O) 作为输出时，返回剩余的期望时间
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效指针
 * @note
 * - 同步/异步：同步
 * - 中断上下文：不可以使用
 * - 中断底半部：不可以使用
 * - 线程上下文：可以使用
 * - 重入性：可重入
 */
__xwds_lib_code
xwer_t xwds_canc_lib_rxq_acquire(struct xwds_canc_rxqueue * rxq,
                                 struct xwds_can_msg * buf,
                                 xwtm_t * xwtm)
{
        xwer_t rc;
        xwreg_t cpuirq;

        XWDS_VALIDATE(rxq, "nullptr", -EFAULT);
        XWDS_VALIDATE(buf, "nullptr", -EFAULT);
        XWDS_VALIDATE(xwtm, "nullptr", -EFAULT);

        rc = xwosal_smr_timedwait(xwosal_smr_get_id(&rxq->smr), xwtm);
        if (__unlikely(rc < 0)) {
                goto err_smr_timedwait;
        }
        xwosal_splk_lock_cpuirqsv(&rxq->lock, &cpuirq);
        *buf = rxq->q[rxq->pos];
        rxq->pos++;
        if (rxq->pos >= (xwssq_t)XWDS_CANC_RXQNUM) {
                rxq->pos = 0;
        } /* else {} */
        xwosal_splk_unlock_cpuirqrs(&rxq->lock, cpuirq);
        return XWOK;

err_smr_timedwait:
        return rc;
}
