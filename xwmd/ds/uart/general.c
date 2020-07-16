/**
 * @file
 * @brief xwmd设备栈：UART
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
#include <xwos/osal/scheduler.h>
#include <xwos/osal/lock/seqlock.h>
#include <xwos/osal/sync/semaphore.h>
#include <xwmd/ds/uart/general.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********     static function prototypes      ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
static __xwds_vop
xwer_t xwds_uartc_cvop_probe(struct xwds_uartc * uartc);

static __xwds_vop
xwer_t xwds_uartc_cvop_remove(struct xwds_uartc * uartc);

static __xwds_vop
xwer_t xwds_uartc_cvop_start(struct xwds_uartc * uartc);

static __xwds_vop
xwer_t xwds_uartc_cvop_stop(struct xwds_uartc * uartc);

#if defined(XWMDCFG_ds_PM) && (1 == XWMDCFG_ds_PM)
static __xwds_vop
xwer_t xwds_uartc_cvop_suspend(struct xwds_uartc * uartc);

static __xwds_vop
xwer_t xwds_uartc_cvop_resume(struct xwds_uartc * uartc);
#endif /* XWMDCFG_ds_PM */

static __xwds_code
xwer_t xwds_uartc_tx_1byte(struct xwds_uartc * uartc, const xwu8_t byte);

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
__xwds_rodata const struct xwds_base_virtual_operations xwds_uartc_cvops = {
        .probe = (void *)xwds_uartc_cvop_probe,
        .remove = (void *)xwds_uartc_cvop_remove,
        .start = (void *)xwds_uartc_cvop_start,
        .stop = (void *)xwds_uartc_cvop_stop,
#if defined(XWMDCFG_ds_PM) && (1 == XWMDCFG_ds_PM)
        .suspend = (void *)xwds_uartc_cvop_suspend,
        .resume = (void *)xwds_uartc_cvop_resume,
#endif /* XWMDCFG_ds_PM */
};

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/******** ******** ******** constructor & destructor ******** ******** ********/
/**
 * @brief XWDS API：UART控制器的构造函数
 * @param uartc: (I) UART控制器对象指针
 */
__xwds_api
void xwds_uartc_construct(struct xwds_uartc * uartc)
{
        xwds_device_construct(&uartc->dev);
        uartc->dev.cvops = &xwds_uartc_cvops;
}

/**
 * @brief XWDS API：UART控制器对象的析构函数
 * @param uartc: (I) UART控制器对象指针
 */
__xwds_api
void xwds_uartc_destruct(struct xwds_uartc * uartc)
{
        xwds_device_destruct(&uartc->dev);
}

/******** ******** base virtual operations ******** ********/
/**
 * @brief XWDS VOP：探测UART控制器
 * @param uartc: (I) UART控制器对象指针
 * @return 错误码
 */
static __xwds_vop
xwer_t xwds_uartc_cvop_probe(struct xwds_uartc * uartc)
{
        xwer_t rc;

        /* init rx */
        xwosal_sqlk_init(&uartc->rxseqlock);
        memset(uartc->rxq, 0, XWMDCFG_ds_UART_GNR_RXQ_SIZE);
        uartc->rxpos = 0;
        uartc->rxnum = 0;

        /* init tx */
        uartc->txport_available = 0;
        xwosal_sqlk_init(&uartc->txseqlock);
        memset(uartc->txq, 0, XWMDCFG_ds_UART_GNR_TXQ_SIZE);
        uartc->txpos = 0;
        uartc->txnum = 0;

        /* create RX semaphore */
        rc = xwosal_smr_init(&uartc->rxsmr, 0, XWMDCFG_ds_UART_GNR_RXQ_SIZE);
        if (__unlikely(rc < 0)) {
                goto err_rxsmr_init;
        }
        rc = xwosal_smr_freeze(xwosal_smr_get_id(&uartc->rxsmr));
        if (__unlikely(rc < 0)) {
                goto err_rxsmr_freeze;
        }
        rc = xwosal_mtx_init(&uartc->txmtx, XWOSAL_SD_PRIORITY_RT_MIN);
        if (__unlikely(rc < 0)) {
                goto err_txmtx_init;
        }

        rc = xwds_device_cvop_probe(&uartc->dev);
        if (__unlikely(rc < 0)) {
                goto err_dev_probe;
        }
        return XWOK;

err_dev_probe:
        xwosal_mtx_destroy(&uartc->txmtx);
err_txmtx_init:
err_rxsmr_freeze:
        xwosal_smr_destroy(&uartc->rxsmr);
err_rxsmr_init:
        return rc;
}

/**
 * @brief XWDS VOP：移除UART控制器
 * @param uartc: (I) UART控制器对象指针
 * @return 错误码
 */
static __xwds_vop
xwer_t xwds_uartc_cvop_remove(struct xwds_uartc * uartc)
{
        xwer_t rc;

        rc = xwds_device_cvop_remove(&uartc->dev);
        if (__unlikely(rc < 0)) {
                goto err_dev_cvop_remove;
        }
        xwosal_mtx_destroy(&uartc->txmtx);
        xwosal_smr_destroy(&uartc->rxsmr);
        return XWOK;

err_dev_cvop_remove:
        return rc;
}

/**
 * @brief XWDS VOP：启动UART控制器
 * @param uartc: (I) UART控制器对象指针
 * @return 错误码
 */
static __xwds_vop
xwer_t xwds_uartc_cvop_start(struct xwds_uartc * uartc)
{
        const struct xwds_uartc_driver * drv;
        xwreg_t cpuirq;
        xwer_t rc;

        rc = xwds_device_cvop_start(&uartc->dev);
        if (__unlikely(rc < 0)) {
                goto err_dev_start;
        }
        rc = xwosal_smr_thaw(xwosal_smr_get_id(&uartc->rxsmr),
                             0, XWMDCFG_ds_UART_GNR_RXQ_SIZE);
        if (__unlikely(rc < 0)) {
                goto err_rxsmr_thaw;
        }

        xwosal_sqlk_wr_lock_cpuirqsv(&uartc->txseqlock, &cpuirq);
        if (uartc->txnum != uartc->txpos) {
                drv = xwds_static_cast(const struct xwds_uartc_driver *,
                                       uartc->dev.drv);
                rc = drv->tx(uartc, uartc->txq[uartc->txpos]);
                if (XWOK == rc) {
                        uartc->txpos++;
                        if (uartc->txpos >= (xwssq_t)XWMDCFG_ds_UART_GNR_TXQ_SIZE) {
                                uartc->txpos = 0;
                        }/* else {} */
                }/* else {} */
        } else {
                uartc->txport_available = 1;
        }
        xwosal_sqlk_wr_unlock_cpuirqrs(&uartc->txseqlock, cpuirq);

        return XWOK;

err_rxsmr_thaw:
        xwds_device_cvop_stop(&uartc->dev);
err_dev_start:
        return rc;
}

/**
 * @brief XWDS VOP：停止UART控制器
 * @param uartc: (I) UART控制器对象指针
 * @return 错误码
 */
static __xwds_vop
xwer_t xwds_uartc_cvop_stop(struct xwds_uartc * uartc)
{
        xwreg_t cpuirq;
        xwer_t rc;

        rc = xwosal_smr_freeze(xwosal_smr_get_id(&uartc->rxsmr));
        if (__unlikely(rc < 0)) {
                goto err_rxsmr_freeze;
        }
        rc = xwds_device_cvop_stop(&uartc->dev);
        if (__unlikely(rc < 0)) {
                goto err_dev_stop;
        }
        xwosal_sqlk_wr_lock_cpuirqsv(&uartc->txseqlock, &cpuirq);
        uartc->txport_available = 0;
        xwosal_sqlk_wr_unlock_cpuirqrs(&uartc->txseqlock, cpuirq);

        return XWOK;

err_dev_stop:
        xwosal_smr_thaw(xwosal_smr_get_id(&uartc->rxsmr), 0,
                        XWMDCFG_ds_UART_GNR_RXQ_SIZE);
err_rxsmr_freeze:
        return rc;
}

#if defined(XWMDCFG_ds_PM) && (1 == XWMDCFG_ds_PM)
/******** ******** pm ******** ********/
/**
 * @brief XWDS VOP：暂停UART控制器
 * @param uartc: (I) UART控制器对象指针
 * @return 错误码
 */
static __xwds_vop
xwer_t xwds_uartc_cvop_suspend(struct xwds_uartc * uartc)
{
        xwreg_t cpuirq;
        xwer_t rc;

        rc = xwds_device_cvop_suspend(&uartc->dev);
        if (__unlikely(rc < 0)) {
                goto err_dev_suspend;
        }
        xwosal_sqlk_wr_lock_cpuirqsv(&uartc->txseqlock, &cpuirq);
        uartc->txport_available = 0;
        xwosal_sqlk_wr_unlock_cpuirqrs(&uartc->txseqlock, cpuirq);
        return XWOK;

err_dev_suspend:
        return rc;
}

/**
 * @brief XWDS VOP：继续UART控制器
 * @param uartc: (I) UART控制器对象指针
 * @return 错误码
 */
static __xwds_vop
xwer_t xwds_uartc_cvop_resume(struct xwds_uartc * uartc)
{
        const struct xwds_uartc_driver * drv;
        xwreg_t cpuirq;
        xwer_t rc;

        rc = xwds_device_cvop_resume(&uartc->dev);
        if (__unlikely(rc < 0)) {
                goto err_dev_resume;
        }
        xwosal_sqlk_wr_lock_cpuirqsv(&uartc->txseqlock, &cpuirq);
        if (uartc->txnum != uartc->txpos) {
                drv = xwds_static_cast(const struct xwds_uartc_driver *, uartc->dev.drv);
                rc = drv->tx(uartc, uartc->txq[uartc->txpos]);
                if (XWOK == rc) {
                        uartc->txpos++;
                        if (uartc->txpos >= (xwssq_t)XWMDCFG_ds_UART_GNR_TXQ_SIZE) {
                                uartc->txpos = 0;
                        }/* else {} */
                }
        } else {
                uartc->txport_available = 1;
        }
        xwosal_sqlk_wr_unlock_cpuirqrs(&uartc->txseqlock, cpuirq);
        return XWOK;

err_dev_resume:
        return rc;
}
#endif /* XWMDCFG_ds_PM */

/******** ******** UART APIs ******** ********/
__xwds_api
xwer_t xwds_uartc_clear_rxq(struct xwds_uartc * uartc)
{
        xwreg_t cpuirq;
        xwer_t rc;

        XWDS_VALIDATE(uartc, "nullptr", -EFAULT);

        rc = xwds_uartc_grab(uartc);
        if (__unlikely(rc < 0)) {
                goto err_uartc_grab;
        }

        rc = xwosal_smr_freeze(xwosal_smr_get_id(&uartc->rxsmr));
        if (__unlikely(rc < 0)) {
                goto err_smr_freeze;
        }
        xwosal_sqlk_wr_lock_cpuirqsv(&uartc->rxseqlock, &cpuirq);
        uartc->rxnum = 0;
        uartc->rxpos = 0;
        xwosal_sqlk_wr_unlock_cpuirqrs(&uartc->rxseqlock, cpuirq);
        rc = xwosal_smr_thaw(xwosal_smr_get_id(&uartc->rxsmr), 0,
                             XWMDCFG_ds_UART_GNR_RXQ_SIZE);
        if (__unlikely(rc < 0)) {
                goto err_smr_thaw;
        }

        xwds_uartc_put(uartc);
        return XWOK;

err_smr_thaw:
err_smr_freeze:
        xwds_uartc_put(uartc);
err_uartc_grab:
        return rc;
}

__xwds_api
xwer_t xwds_uartc_get_rxq_datasize(struct xwds_uartc * uartc, xwsz_t *ret)
{
        xwssz_t size;
        xwsq_t seq;
        xwer_t rc;

        XWDS_VALIDATE(uartc, "nullptr", -EFAULT);
        XWDS_VALIDATE(ret, "nullptr", -EFAULT);

        rc = xwds_uartc_grab(uartc);
        if (__unlikely(rc < 0)) {
                goto err_uartc_grab;
        }

        do {
                seq = xwosal_sqlk_rd_begin(&uartc->rxseqlock);
                size = uartc->rxnum - uartc->rxpos;
                if (size < 0) {
                        size = (xwssz_t)XWMDCFG_ds_UART_GNR_RXQ_SIZE -
                               (xwssz_t)uartc->rxpos;
                        size += uartc->rxnum;
                }
        } while (xwosal_sqlk_rd_retry(&uartc->rxseqlock, seq));

        xwds_uartc_put(uartc);
        *ret = (xwsz_t)size;
        return XWOK;

err_uartc_grab:
        return rc;
}

__xwds_api
xwer_t xwds_uartc_getc(struct xwds_uartc * uartc, xwu8_t * buf, xwtm_t * xwtm)
{
        xwer_t rc;
        xwreg_t cpuirq;

        XWDS_VALIDATE(uartc, "nullptr", -EFAULT);
        XWDS_VALIDATE(buf, "nullptr", -EFAULT);
        XWDS_VALIDATE(xwtm, "nullptr", -EFAULT);

        rc = xwds_uartc_grab(uartc);
        if (__unlikely(rc < 0)) {
                goto err_uartc_grab;
        }

        rc = xwosal_smr_timedwait(xwosal_smr_get_id(&uartc->rxsmr), xwtm);
        if (__unlikely(rc < 0)) {
                goto err_smr_timedwait;
        }
        xwosal_sqlk_wr_lock_cpuirqsv(&uartc->rxseqlock, &cpuirq);
        *buf = uartc->rxq[uartc->rxpos];
        uartc->rxpos++;
        if (uartc->rxpos >= (xwssq_t)XWMDCFG_ds_UART_GNR_RXQ_SIZE) {
                uartc->rxpos = 0;
        }/* else {} */
        xwosal_sqlk_wr_unlock_cpuirqrs(&uartc->rxseqlock, cpuirq);

        xwds_uartc_put(uartc);
        return XWOK;

err_smr_timedwait:
        xwds_uartc_put(uartc);
err_uartc_grab:
        return rc;
}

__xwds_api
xwer_t xwds_uartc_rx(struct xwds_uartc * uartc,
                     void * buf, xwsz_t * size,
                     xwtm_t * xwtm)
{
        xwsz_t i;
        xwer_t rc;
        xwreg_t cpuirq;

        XWDS_VALIDATE(uartc, "nullptr", -EFAULT);
        XWDS_VALIDATE(buf, "nullptr", -EFAULT);
        XWDS_VALIDATE(size, "nullptr", -EFAULT);
        XWDS_VALIDATE(xwtm, "nullptr", -EFAULT);

        rc = xwds_uartc_grab(uartc);
        if (__unlikely(rc < 0)) {
                *size = 0;
                goto err_uartc_grab;
        }

        for (i = 0; i < *size; i++) {
                rc = xwosal_smr_timedwait(xwosal_smr_get_id(&uartc->rxsmr), xwtm);
                if (__unlikely(rc < 0)) {
                        *size = i;
                        goto err_smr_timedwait;
                }
                xwosal_sqlk_wr_lock_cpuirqsv(&uartc->rxseqlock, &cpuirq);
                ((xwu8_t *)buf)[i] = uartc->rxq[uartc->rxpos];
                uartc->rxpos++;
                if (uartc->rxpos >= (xwssq_t)XWMDCFG_ds_UART_GNR_RXQ_SIZE) {
                        uartc->rxpos = 0;
                }/* else {} */
                xwosal_sqlk_wr_unlock_cpuirqrs(&uartc->rxseqlock, cpuirq);
        }

        xwds_uartc_put(uartc);
        *size = i;
        return XWOK;

err_smr_timedwait:
        xwds_uartc_put(uartc);
err_uartc_grab:
        return rc;
}

__xwds_api
xwer_t xwds_uartc_try_getc(struct xwds_uartc * uartc, xwu8_t * buf)
{
        xwer_t rc;
        xwreg_t cpuirq;

        XWDS_VALIDATE(uartc, "nullptr", -EFAULT);
        XWDS_VALIDATE(buf, "nullptr", -EFAULT);

        rc = xwds_uartc_grab(uartc);
        if (__unlikely(rc < 0)) {
                goto err_uartc_grab;
        }

        rc = xwosal_smr_trywait(xwosal_smr_get_id(&uartc->rxsmr));
        if (__unlikely(rc < 0)) {
                goto err_smr_trywait;
        }
        xwosal_sqlk_wr_lock_cpuirqsv(&uartc->rxseqlock, &cpuirq);
        *buf = uartc->rxq[uartc->rxpos];
        uartc->rxpos++;
        if (uartc->rxpos >= (xwssq_t)XWMDCFG_ds_UART_GNR_RXQ_SIZE) {
                uartc->rxpos = 0;
        }/* else {} */
        xwosal_sqlk_wr_unlock_cpuirqrs(&uartc->rxseqlock, cpuirq);

        xwds_uartc_put(uartc);
        return XWOK;

err_smr_trywait:
        xwds_uartc_put(uartc);
err_uartc_grab:
        return rc;
}

__xwds_api
xwer_t xwds_uartc_try_rx(struct xwds_uartc * uartc, void * buf, xwsz_t * size)
{
        xwsz_t i;
        xwer_t rc;
        xwreg_t cpuirq;

        XWDS_VALIDATE(uartc, "nullptr", -EFAULT);
        XWDS_VALIDATE(buf, "nullptr", -EFAULT);
        XWDS_VALIDATE(size, "nullptr", -EFAULT);

        rc = xwds_uartc_grab(uartc);
        if (__unlikely(rc < 0)) {
                *size = 0;
                goto err_uartc_grab;
        }

        for (i = 0; i < *size; i++) {
                rc = xwosal_smr_trywait(xwosal_smr_get_id(&uartc->rxsmr));
                if (__unlikely(rc < 0)) {
                        *size = i;
                        goto err_smr_trywait;
                }
                xwosal_sqlk_wr_lock_cpuirqsv(&uartc->rxseqlock, &cpuirq);
                ((xwu8_t *)buf)[i] = uartc->rxq[uartc->rxpos];
                uartc->rxpos++;
                if (uartc->rxpos >= (xwssq_t)XWMDCFG_ds_UART_GNR_RXQ_SIZE) {
                        uartc->rxpos = 0;
                }/* else {} */
                xwosal_sqlk_wr_unlock_cpuirqrs(&uartc->rxseqlock, cpuirq);
        }

        xwds_uartc_put(uartc);
        *size = i;
        return XWOK;

err_smr_trywait:
        xwds_uartc_put(uartc);
err_uartc_grab:
        return rc;
}

__xwds_api
xwer_t xwds_uartc_clear_txq(struct xwds_uartc * uartc)
{
        xwer_t rc = XWOK;
        xwreg_t cpuirq;

        XWDS_VALIDATE(uartc, "nullptr", -EFAULT);

        rc = xwds_uartc_grab(uartc);
        if (__unlikely(rc < 0)) {
                goto err_uartc_grab;
        }

        xwosal_sqlk_wr_lock_cpuirqsv(&uartc->txseqlock, &cpuirq);
        uartc->txnum = 0;
        uartc->txpos = 0;
        xwosal_sqlk_wr_unlock_cpuirqrs(&uartc->txseqlock, cpuirq);

        xwds_uartc_put(uartc);
        return rc;

err_uartc_grab:
        return rc;
}

static __xwds_code
xwer_t xwds_uartc_tx_1byte(struct xwds_uartc * uartc, const xwu8_t byte)
{
        const struct xwds_uartc_driver * drv;
        xwreg_t cpuirq;
        xwer_t rc;

        xwosal_sqlk_wr_lock_cpuirqsv(&uartc->txseqlock, &cpuirq);
        if (1 == uartc->txport_available) {
                drv = xwds_static_cast(const struct xwds_uartc_driver *, uartc->dev.drv);
                rc = drv->tx(uartc, byte);
                if (XWOK == rc) {
                        uartc->txport_available = 0;
                }/* else {} */
        } else {
                rc = -EBUSY;
        }
        if (rc < 0) {
                uartc->txq[uartc->txnum] = byte;
                uartc->txnum++;
                if (uartc->txnum >= (xwssz_t)XWMDCFG_ds_UART_GNR_TXQ_SIZE) {
                        uartc->txnum = 0;
                }/* else {} */
                if (uartc->txnum == uartc->txpos) {
                        /* overflow: discard the newest one and stop tx */
                        uartc->txnum--;
                        if (uartc->txnum < 0) {
                                uartc->txnum = (xwssz_t)XWMDCFG_ds_UART_GNR_TXQ_SIZE - 1;
                        }/* else {} */
                        rc = -EOVERFLOW;
                } else {
                        rc = XWOK;
                }
        }
        xwosal_sqlk_wr_unlock_cpuirqrs(&uartc->txseqlock, cpuirq);
        return rc;
}

__xwds_api
xwer_t xwds_uartc_putc(struct xwds_uartc * uartc,
                       const xwu8_t byte,
                       xwtm_t * xwtm)
{
        xwer_t rc;

        XWDS_VALIDATE(uartc, "nullptr", -EFAULT);
        XWDS_VALIDATE(xwtm, "nullptr", -EFAULT);

        rc = xwds_uartc_grab(uartc);
        if (__unlikely(rc < 0)) {
                goto err_uartc_grab;
        }

        rc = xwosal_mtx_timedlock(xwosal_mtx_get_id(&uartc->txmtx), xwtm);
        if (__unlikely(rc < 0)) {
                goto err_uartc_lock;
        }
        rc = xwds_uartc_tx_1byte(uartc, byte);
        if (__unlikely(rc < 0)) {
                goto err_tx_1byte;
        }
        xwosal_mtx_unlock(xwosal_mtx_get_id(&uartc->txmtx));

        xwds_uartc_put(uartc);
        return XWOK;

err_tx_1byte:
        xwosal_mtx_unlock(xwosal_mtx_get_id(&uartc->txmtx));
err_uartc_lock:
        xwds_uartc_put(uartc);
err_uartc_grab:
        return rc;
}

__xwds_api
xwer_t xwds_uartc_tx(struct xwds_uartc * uartc,
                     const xwu8_t * data, xwsz_t * size,
                     xwtm_t * xwtm)
{
        xwsz_t i;
        xwer_t rc;

        XWDS_VALIDATE(uartc, "nullptr", -EFAULT);
        XWDS_VALIDATE(data, "nullptr", -EFAULT);
        XWDS_VALIDATE(size, "nullptr", -EFAULT);
        XWDS_VALIDATE(xwtm, "nullptr", -EFAULT);

        rc = xwds_uartc_grab(uartc);
        if (__unlikely(rc < 0)) {
                *size = 0;
                goto err_uartc_grab;
        }

        rc = xwosal_mtx_timedlock(xwosal_mtx_get_id(&uartc->txmtx), xwtm);
        if (__unlikely(rc < 0)) {
                *size = 0;
                goto err_uartc_lock;
        }
        rc = -EINVAL;
        for (i = 0; i < *size; i++) {
                rc = xwds_uartc_tx_1byte(uartc, data[i]);
                if (__unlikely(rc < 0)) {
                        *size = i;
                        goto err_tx_1byte;
                }
        }
        xwosal_mtx_unlock(xwosal_mtx_get_id(&uartc->txmtx));

        xwds_uartc_put(uartc);
        *size = i;
        return XWOK;

err_tx_1byte:
        xwosal_mtx_unlock(xwosal_mtx_get_id(&uartc->txmtx));
err_uartc_lock:
        xwds_uartc_put(uartc);
err_uartc_grab:
        return rc;
}

__xwds_api
xwer_t xwds_uartc_cfg(struct xwds_uartc * uartc,
                      const struct xwds_uart_cfg * cfg)
{
        const struct xwds_uartc_driver * drv;
        xwer_t rc;

        XWDS_VALIDATE(uartc, "nullptr", -EFAULT);
        XWDS_VALIDATE(cfg, "nullptr", -EFAULT);

        rc = xwds_uartc_grab(uartc);
        if (__unlikely(rc < 0)) {
                goto err_uartc_grab;
        }

        drv = xwds_static_cast(const struct xwds_uartc_driver *, uartc->dev.drv);
        if ((drv) && (drv->cfg)) {
                rc = drv->cfg(uartc, cfg);
        } else {
                rc = -ENOSYS;
        }
        if (__unlikely(rc < 0)) {
                goto err_drv_cfg;
        }

        xwds_uartc_put(uartc);
        return XWOK;

err_drv_cfg:
        xwds_uartc_put(uartc);
err_uartc_grab:
        return rc;
}

/******** ******** Libraries for BSP driver ******** ********/
/**
 * @brief XWDS LIB：UART接收中断响应函数
 * @param uartc: (I) UART控制器对象指针
 */
__xwds_lib_code
void xwds_uartc_lib_rx_isr(struct xwds_uartc * uartc)
{
        const struct xwds_uartc_driver * drv;
        xwreg_t cpuirq;
        xwu8_t data;
        xwer_t rc;

        drv = xwds_static_cast(const struct xwds_uartc_driver *, uartc->dev.drv);
        rc = drv->rx(uartc, &data);
        if (__unlikely(rc < 0)) {
                goto err_drv_rx;
        }
        xwosal_sqlk_wr_lock_cpuirqsv(&uartc->rxseqlock, &cpuirq);
        uartc->rxq[uartc->rxnum] = data;
        uartc->rxnum++;
        if (uartc->rxnum >= (xwssz_t)XWMDCFG_ds_UART_GNR_RXQ_SIZE) {
                uartc->rxnum = 0;
        }/* else {} */
        if (uartc->rxnum == uartc->rxpos) {
                /* overflow: discard the oldest data */
                uartc->rxpos++;
                if (uartc->rxpos >= (xwssq_t)XWMDCFG_ds_UART_GNR_RXQ_SIZE) {
                        uartc->rxpos = 0;
                }/* else {} */
                xwosal_sqlk_wr_unlock_cpuirqrs(&uartc->rxseqlock, cpuirq);
        } else {
                xwosal_sqlk_wr_unlock_cpuirqrs(&uartc->rxseqlock, cpuirq);
                xwosal_smr_post(xwosal_smr_get_id(&uartc->rxsmr));
        }

err_drv_rx:
        return;
}

/**
 * @brief XWDS LIB：UART发送中断响应函数
 * @param uartc: (I) UART控制器对象指针
 */
__xwds_lib_code
void xwds_uartc_lib_tx_isr(struct xwds_uartc * uartc)
{
        const struct xwds_uartc_driver * drv;
        xwreg_t cpuirq;
        xwer_t rc;

        /* send a byte */
        xwosal_sqlk_wr_lock_cpuirqsv(&uartc->txseqlock, &cpuirq);
        if (uartc->txnum != uartc->txpos) {
                drv = xwds_static_cast(const struct xwds_uartc_driver *,
                                       uartc->dev.drv);
                rc = drv->tx(uartc, uartc->txq[uartc->txpos]);
                if (XWOK == rc) {
                        uartc->txpos++;
                        if (uartc->txpos >= (xwssq_t)XWMDCFG_ds_UART_GNR_TXQ_SIZE) {
                                uartc->txpos = 0;
                        }/* else {} */
                } else {
                        uartc->txport_available = 1;
                }
        } else {
                uartc->txport_available = 1;
        }
        xwosal_sqlk_wr_unlock_cpuirqrs(&uartc->txseqlock, cpuirq);
}
