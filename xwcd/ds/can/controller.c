/**
 * @file
 * @brief 玄武设备栈：CAN：总线控制器
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
#include <xwos/osal/lock/spinlock.h>
#include <xwos/osal/sync/sem.h>
#include <xwcd/ds/can/controller.h>

static __xwds_vop
xwer_t xwds_canc_vop_probe(struct xwds_canc * canc);

static __xwds_vop
xwer_t xwds_canc_vop_remove(struct xwds_canc * canc);

static __xwds_vop
xwer_t xwds_canc_vop_start(struct xwds_canc * canc);

static __xwds_vop
xwer_t xwds_canc_vop_stop(struct xwds_canc * canc);

#if defined(XWCDCFG_ds_PM) && (1 == XWCDCFG_ds_PM)
static __xwds_vop
xwer_t xwds_canc_vop_suspend(struct xwds_canc * canc);

static __xwds_vop
xwer_t xwds_canc_vop_resume(struct xwds_canc * canc);
#endif

__xwds_rodata const struct xwds_virtual_operation xwds_canc_vop = {
        .probe = (void *)xwds_canc_vop_probe,
        .remove = (void *)xwds_canc_vop_remove,
        .start = (void *)xwds_canc_vop_start,
        .stop = (void *)xwds_canc_vop_stop,
#if defined(XWCDCFG_ds_PM) && (1 == XWCDCFG_ds_PM)
        .suspend = (void *)xwds_canc_vop_suspend,
        .resume = (void *)xwds_canc_vop_resume,
#endif
};

/******** ******** ******** constructor & destructor ******** ******** ********/
__xwds_api
void xwds_canc_construct(struct xwds_canc * canc)
{
        xwds_device_construct(&canc->bc.dev);
        canc->bc.dev.vop = &xwds_canc_vop;
}

__xwds_api
void xwds_canc_destruct(struct xwds_canc * canc)
{
        xwds_device_destruct(&canc->bc.dev);
}

__xwds_api
xwer_t xwds_canc_grab(struct xwds_canc * canc)
{
        return xwds_device_grab(&canc->bc.dev);
}

__xwds_api
xwer_t xwds_canc_put(struct xwds_canc * canc)
{
        return xwds_device_put(&canc->bc.dev);
}

/******** ******** base virtual operations ******** ********/
/**
 * @brief XWDS VOP：探测CAN控制器
 * @param[in] canc: CAN控制器对象指针
 */
static __xwds_vop
xwer_t xwds_canc_vop_probe(struct xwds_canc * canc)
{
        xwer_t rc;

        XWDS_VALIDATE(canc->cfg, "nullptr", -EFAULT);

        canc->mode = XWDS_CANC_MODE_UNINIT;
        rc = xwds_device_vop_probe(&canc->bc.dev);
        return rc;
}

/**
 * @brief XWDS VOP：移除CAN控制器
 * @param[in] canc: CAN控制器对象指针
 */
static __xwds_vop
xwer_t xwds_canc_vop_remove(struct xwds_canc * canc)
{
        xwer_t rc;

        rc = xwds_device_vop_remove(&canc->bc.dev);
        return rc;
}

/**
 * @brief XWDS VOP：启动CAN控制器
 * @param[in] canc: CAN控制器对象指针
 */
static __xwds_vop
xwer_t xwds_canc_vop_start(struct xwds_canc * canc)
{
        xwer_t rc;

        rc = xwds_device_vop_start(&canc->bc.dev);
        return rc;
}

/**
 * @brief XWDS VOP：停止CAN控制器
 * @param[in] canc: CAN控制器对象指针
 */
static __xwds_vop
xwer_t xwds_canc_vop_stop(struct xwds_canc * canc)
{
        xwer_t rc;

        rc = xwds_device_vop_stop(&canc->bc.dev);
        return rc;
}

#if defined(XWCDCFG_ds_PM) && (1 == XWCDCFG_ds_PM)
/******** ******** pm ******** ********/
/**
 * @brief XWDS VOP：暂停CAN控制器
 * @param[in] canc: CAN控制器对象指针
 */
static __xwds_vop
xwer_t xwds_canc_vop_suspend(struct xwds_canc * canc)
{
        xwer_t rc;

        rc = xwds_device_vop_suspend(&canc->bc.dev);
        return rc;
}

/**
 * @brief XWDS VOP：继续CAN控制器
 * @param[in] canc: CAN控制器对象指针
 */
static __xwds_vop
xwer_t xwds_canc_vop_resume(struct xwds_canc * canc)
{
        xwer_t rc;

        rc = xwds_device_vop_resume(&canc->bc.dev);
        return rc;
}
#endif

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
        if (rc < 0) {
                goto err_canc_grab;
        }

        if (txobjid >= canc->cfg->txobjs_num) {
                rc = -ERANGE;
                goto err_txobjid;
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
        drv = xwds_cast(const struct xwds_canc_driver *, canc->bc.dev.drv);
        if ((drv) && (drv->write)) {
                rc = drv->write(canc, txobjcfg, msg);
        } else {
                rc = -ENOSYS;
        }
        if (rc < 0) {
                goto err_drv_write;
        }
        xwds_canc_put(canc);

        return XWOK;

err_drv_write:
err_badtxobj:
err_txobjid:
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
        if (rc < 0) {
                goto err_canc_grab;
        }
        drv = xwds_cast(const struct xwds_canc_driver *, canc->bc.dev.drv);
        if ((drv) && (drv->enable_irqs)) {
                rc = drv->enable_irqs(canc);
        } else {
                rc = -ENOSYS;
        }
        if (rc < 0) {
                goto err_canc_drv_enable_irqs;
        }
        xwds_canc_put(canc);
        return XWOK;

err_canc_drv_enable_irqs:
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

        rc = xwds_canc_grab(canc);
        if (rc < 0) {
                goto err_canc_grab;
        }
        drv = xwds_cast(const struct xwds_canc_driver *, canc->bc.dev.drv);
        if ((drv) && (drv->disable_irqs)) {
                rc = drv->disable_irqs(canc);
        } else {
                rc = -ENOSYS;
        }
        if (rc < 0) {
                goto err_canc_drv_disable_irqs;
        }
        xwds_canc_put(canc);
        return XWOK;

err_canc_drv_disable_irqs:
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
        if (rc < 0) {
                goto err_canc_grab;
        }

        if (canc->mode == mode) {
                rc = -EALREADY;
                goto err_same_mode;
        }
        drv = xwds_cast(const struct xwds_canc_driver *, canc->bc.dev.drv);
        if ((drv) && (drv->set_mode)) {
                rc = drv->set_mode(canc, mode);
        } else {
                rc = -ENOSYS;
        }
        if (rc < 0) {
                goto err_drv_set_mode;
        }
        canc->mode = mode;

        xwds_canc_put(canc);
        xwds_canc_drvcb_mode_indication(canc, mode);
        return XWOK;

err_drv_set_mode:
err_same_mode:
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
        if (rc < 0) {
                goto err_canc_grab;
        }
        if (bdcfgid >= canc->cfg->bdcfgs_num) {
                rc = -ERANGE;
                goto err_nobd;
        }
        bdcfg = &canc->cfg->bdcfgs[bdcfgid];
        drv = xwds_cast(const struct xwds_canc_driver *, canc->bc.dev.drv);
        if ((drv) && (drv->set_bd)) {
                rc = drv->set_bd(canc, bdcfg);
        } else {
                rc = -ENOSYS;
        }
        if (rc < 0) {
                goto err_drv_set_bd;
        }
        xwds_canc_put(canc);
        return XWOK;

err_drv_set_bd:
err_nobd:
        xwds_canc_put(canc);
err_canc_grab:
        return rc;
}

/******** ******** RX Queue ******** ********/
__xwds_api
void xwds_canc_rxq_init(struct xwds_canc_rxqueue * rxq)
{
        xwos_splk_init(&rxq->lock);
        memset(rxq->q, 0, sizeof(rxq->q));
        rxq->pos = 0;
        rxq->num = 0;
        xwos_sem_init(&rxq->sem, 0, XWDS_CANC_RXQNUM);
}

__xwds_code
void xwds_canc_rxq_publish(struct xwds_canc_rxqueue * rxq,
                           struct xwds_can_msg * msg)
{
        xwreg_t cpuirq;

        xwos_splk_lock_cpuirqsv(&rxq->lock, &cpuirq);
        memcpy(&rxq->q[rxq->num], msg, sizeof(struct xwds_can_msg));
        rxq->num++;
        if (rxq->num >= (xwssz_t)XWDS_CANC_RXQNUM) {
                rxq->num = 0;
        }
        if (rxq->num == rxq->pos) {
                /* Queue is overflow. Discard the oldest data */
                rxq->pos++;
                if (rxq->pos >= (xwssq_t)XWDS_CANC_RXQNUM) {
                        rxq->pos = 0;
                }
                xwos_splk_unlock_cpuirqrs(&rxq->lock, cpuirq);
        } else {
                xwos_splk_unlock_cpuirqrs(&rxq->lock, cpuirq);
                xwos_sem_post(&rxq->sem);
        }
}

__xwds_api
xwer_t xwds_canc_rxq_acquire(struct xwds_canc_rxqueue * rxq,
                             struct xwds_can_msg * buf,
                             xwtm_t to)
{
        xwer_t rc;
        xwreg_t cpuirq;

        XWDS_VALIDATE(rxq, "nullptr", -EFAULT);
        XWDS_VALIDATE(buf, "nullptr", -EFAULT);

        rc = xwos_sem_wait_to(&rxq->sem, to);
        if (rc < 0) {
                goto err_sem_wait_to;
        }
        xwos_splk_lock_cpuirqsv(&rxq->lock, &cpuirq);
        *buf = rxq->q[rxq->pos];
        rxq->pos++;
        if (rxq->pos >= (xwssq_t)XWDS_CANC_RXQNUM) {
                rxq->pos = 0;
        }
        xwos_splk_unlock_cpuirqrs(&rxq->lock, cpuirq);
        return XWOK;

err_sem_wait_to:
        return rc;
}

/******** ******** Callbacks for driver ******** ********/
__xwds_api
void xwds_canc_setcb_tx_indication(struct xwds_canc * canc,
                                   void (*cb)(struct xwds_canc *,
                                              xwid_t, xwer_t))
{
#if !defined(XWCDCFG_ds_CAN_CONTROLLER_ROCBT) || (1 != XWCDCFG_ds_CAN_CONTROLLER_ROCBT)
        if (canc->cbtbl) {
                canc->cbtbl->tx_indication = cb;
        }
#else
        XWOS_UNUSED(canc);
        XWOS_UNUSED(cb);
#endif
}

__xwds_api
void xwds_canc_setcb_rx_indication(struct xwds_canc * canc,
                                   void (*cb)(struct xwds_canc *,
                                              xwid_t,
                                              struct xwds_can_msg *))
{
#if !defined(XWCDCFG_ds_CAN_CONTROLLER_ROCBT) || (1 != XWCDCFG_ds_CAN_CONTROLLER_ROCBT)
        if (canc->cbtbl) {
                canc->cbtbl->rx_indication = cb;
        }
#else
        XWOS_UNUSED(canc);
        XWOS_UNUSED(cb);
#endif
}

__xwds_api
void xwds_canc_setcb_wakeup_notification(struct xwds_canc * canc,
                                         void (*cb)(struct xwds_canc *))
{
#if !defined(XWCDCFG_ds_CAN_CONTROLLER_ROCBT) || (1 != XWCDCFG_ds_CAN_CONTROLLER_ROCBT)
        if (canc->cbtbl) {
                canc->cbtbl->wakeup_notification = cb;
        }
#else
        XWOS_UNUSED(canc);
        XWOS_UNUSED(cb);
#endif
}

__xwds_api
void xwds_canc_setcb_mode_indication(struct xwds_canc * canc,
                                     void (*cb)(struct xwds_canc *, xwsq_t))
{
#if !defined(XWCDCFG_ds_CAN_CONTROLLER_ROCBT) || (1 != XWCDCFG_ds_CAN_CONTROLLER_ROCBT)
        if (canc->cbtbl) {
                canc->cbtbl->mode_indication = cb;
        }
#else
        XWOS_UNUSED(canc);
        XWOS_UNUSED(cb);
#endif
}

__xwds_api
void xwds_canc_setcb_err_indication(struct xwds_canc * canc,
                                    void (*cb)(struct xwds_canc *,
                                               xwsq_t, xwsq_t, xwsq_t))
{
#if !defined(XWCDCFG_ds_CAN_CONTROLLER_ROCBT) || (1 != XWCDCFG_ds_CAN_CONTROLLER_ROCBT)
        if (canc->cbtbl) {
                canc->cbtbl->err_indication = cb;
        }
#else
        XWOS_UNUSED(canc);
        XWOS_UNUSED(cb);
#endif
}

__xwds_api
void xwds_canc_setcb_busoff_indication(struct xwds_canc * canc,
                                       void (*cb)(struct xwds_canc *))
{
#if !defined(XWCDCFG_ds_CAN_CONTROLLER_ROCBT) || (1 != XWCDCFG_ds_CAN_CONTROLLER_ROCBT)
        if (canc->cbtbl) {
                canc->cbtbl->busoff_indication = cb;
        }
#else
        XWOS_UNUSED(canc);
        XWOS_UNUSED(cb);
#endif
}

__xwds_code
void xwds_canc_drvcb_init_msg(struct xwds_can_msg * msg, xwu32_t canid,
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

__xwds_code
void xwds_canc_drvcb_tx_indication(struct xwds_canc * canc, xwid_t txobjid,
                                   xwer_t rc)
{
        __xwds_canc_cbtbl_qualifier struct xwds_canc_cbtbl * cbtbl;

        cbtbl = canc->cbtbl;
        if ((cbtbl) && (cbtbl->tx_indication)) {
                cbtbl->tx_indication(canc, txobjid, rc);
        }
}

__xwds_code
void xwds_canc_drvcb_rx_indication(struct xwds_canc * canc, xwid_t rxobjid,
                                   struct xwds_can_msg * rxmsg)
{
        __xwds_canc_cbtbl_qualifier struct xwds_canc_cbtbl * cbtbl;

        cbtbl = canc->cbtbl;
        if ((cbtbl) && (cbtbl->rx_indication)) {
                cbtbl->rx_indication(canc, rxobjid, rxmsg);
        }
}

__xwds_code
void xwds_canc_drvcb_wakeup_notification(struct xwds_canc * canc)
{
        __xwds_canc_cbtbl_qualifier struct xwds_canc_cbtbl * cbtbl;

        cbtbl = canc->cbtbl;
        if ((cbtbl) && (cbtbl->wakeup_notification)) {
                cbtbl->wakeup_notification(canc);
        }
}

__xwds_code
void xwds_canc_drvcb_mode_indication(struct xwds_canc * canc, xwsq_t mode)
{
        __xwds_canc_cbtbl_qualifier struct xwds_canc_cbtbl * cbtbl;

        cbtbl = canc->cbtbl;
        if ((cbtbl) && (cbtbl->mode_indication)) {
                cbtbl->mode_indication(canc, mode);
        }
}

__xwds_code
void xwds_canc_drvcb_err_indication(struct xwds_canc * canc, xwsq_t errcode,
                                    xwsq_t tec, xwsq_t rec)
{
        __xwds_canc_cbtbl_qualifier struct xwds_canc_cbtbl * cbtbl;

        cbtbl = canc->cbtbl;
        if ((cbtbl) && (cbtbl->err_indication)) {
                cbtbl->err_indication(canc, errcode, tec, rec);
        }
}

__xwds_code
void xwds_canc_drvcb_busoff_indication(struct xwds_canc * canc)
{
        __xwds_canc_cbtbl_qualifier struct xwds_canc_cbtbl * cbtbl;

        cbtbl = canc->cbtbl;
        if ((cbtbl) && (cbtbl->busoff_indication)) {
                cbtbl->busoff_indication(canc);
        }
}
