/**
 * @file
 * @brief xwmd设备栈：UART
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
#include <xwos/standard.h>
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

#if defined(XWMDCFG_ds_LPM) && (1 == XWMDCFG_ds_LPM)
static __xwds_vop
xwer_t xwds_uartc_cvop_suspend(struct xwds_uartc * uartc);

static __xwds_vop
xwer_t xwds_uartc_cvop_resume(struct xwds_uartc * uartc);
#endif /* XWMDCFG_ds_LPM */

static __xwds_code
xwer_t xwds_uartc_tx_1byte(struct xwds_uartc * uartc, const xwu8_t byte);

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#if !defined(XWMDCFG_ds_NANO) || (1 != XWMDCFG_ds_NANO)
__xwds_rodata const struct xwds_base_virtual_operations xwds_uartc_cvops = {
        .probe = (void *)xwds_uartc_cvop_probe,
        .remove = (void *)xwds_uartc_cvop_remove,
        .start = (void *)xwds_uartc_cvop_start,
        .stop = (void *)xwds_uartc_cvop_stop,
#if defined(XWMDCFG_ds_LPM) && (1 == XWMDCFG_ds_LPM)
        .suspend = (void *)xwds_uartc_cvop_suspend,
        .resume = (void *)xwds_uartc_cvop_resume,
#endif /* XWMDCFG_ds_LPM */
};
#endif /* !XWMDCFG_ds_NANO */

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#if !defined(XWMDCFG_ds_NANO) || (1 != XWMDCFG_ds_NANO)
/******** ******** ******** constructor & destructor ******** ******** ********/
/**
 * @brief UART控制器的构造函数
 * @param uartc: (I) UART控制器对象指针
 */
__xwds_code
void xwds_uartc_construct(struct xwds_uartc * uartc)
{
        xwds_device_construct(&uartc->dev);
        uartc->dev.cvops = &xwds_uartc_cvops;
}

/**
 * @brief UART控制器对象的析构函数
 * @param uartc: (I) UART控制器对象指针
 */
__xwds_code
void xwds_uartc_destruct(struct xwds_uartc * uartc)
{
        xwds_device_destruct(&uartc->dev);
}
#endif /* !XWMDCFG_ds_NANO */

/******** ******** base virtual operations ******** ********/
/**
 * @brief SODS VOP：探测UART控制器
 * @param uartc: (I) UART控制器对象指针
 * @return 错误码
 * @retval OK: OK
 * @retval -EFAULT: 无效指针
 * @note
 * - 同步/异步：同步
 * - 中断上下文：可以使用
 * - 中断底半部：可以使用
 * - 线程上下文：可以使用
 * - 重入性：对于同一设备不可重入；对于不同设备可重入
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
        return OK;

err_dev_probe:
        xwosal_mtx_destroy(&uartc->txmtx);
err_txmtx_init:
err_rxsmr_freeze:
        xwosal_smr_destroy(&uartc->rxsmr);
err_rxsmr_init:
        return rc;
}

/**
 * @brief SODS VOP：移除UART控制器
 * @param uartc: (I) UART控制器对象指针
 * @return 错误码
 * @retval OK: OK
 * @retval -EFAULT: 无效指针
 * @note
 * - 同步/异步：同步
 * - 中断上下文：可以使用
 * - 中断底半部：可以使用
 * - 线程上下文：可以使用
 * - 重入性：对于同一个设备不可重入；对于不同设备可重入
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
        return OK;

err_dev_cvop_remove:
        return rc;
}

/**
 * @brief SODS VOP：启动UART控制器
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
                if (OK == rc) {
                        uartc->txpos++;
                        if (uartc->txpos >= (xwssq_t)XWMDCFG_ds_UART_GNR_TXQ_SIZE) {
                                uartc->txpos = 0;
                        }/* else {} */
                }/* else {} */
        } else {
                uartc->txport_available = 1;
        }
        xwosal_sqlk_wr_unlock_cpuirqrs(&uartc->txseqlock, cpuirq);

        return OK;

err_rxsmr_thaw:
        xwds_device_cvop_stop(&uartc->dev);
err_dev_start:
        return rc;
}

/**
 * @brief SODS VOP：停止UART控制器
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

        return OK;

err_dev_stop:
        xwosal_smr_thaw(xwosal_smr_get_id(&uartc->rxsmr), 0,
                        XWMDCFG_ds_UART_GNR_RXQ_SIZE);
err_rxsmr_freeze:
        return rc;
}

#if defined(XWMDCFG_ds_LPM) && (1 == XWMDCFG_ds_LPM)
/******** ******** pm ******** ********/
/**
 * @brief SODS VOP：暂停UART控制器
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
        return OK;

err_dev_suspend:
        return rc;
}

/**
 * @brief SODS VOP：继续UART控制器
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
                if (OK == rc) {
                        uartc->txpos++;
                        if (uartc->txpos >= (xwssq_t)XWMDCFG_ds_UART_GNR_TXQ_SIZE) {
                                uartc->txpos = 0;
                        }/* else {} */
                }
        } else {
                uartc->txport_available = 1;
        }
        xwosal_sqlk_wr_unlock_cpuirqrs(&uartc->txseqlock, cpuirq);
        return OK;

err_dev_resume:
        return rc;
}
#endif /* XWMDCFG_ds_LPM */

/******** ******** UART APIs ******** ********/
#if defined(XWMDCFG_ds_NANO) && (1 == XWMDCFG_ds_NANO)
/**
 * @brief SODS API：探测UART控制器
 * @param uartc: (I) UART控制器对象指针
 * @return 错误码
 * @retval OK: OK
 * @retval -EFAULT: 无效指针
 * @note
 * - 同步/异步：同步
 * - 中断上下文：可以使用
 * - 中断底半部：可以使用
 * - 线程上下文：可以使用
 * - 重入性：对于同一设备不可重入；对于不同设备可重入
 */
__xwds_api
xwer_t xwds_uartc_probe(struct xwds_uartc * uartc)
{
        xwer_t rc;

        SODS_VALIDATE(uartc, "nullptr", -EFAULT);

        rc = xwds_uartc_cvop_probe(uartc);
        return rc;
}

/**
 * @brief SODS API：移除UART控制器
 * @param uartc: (I) UART控制器对象指针
 * @return 错误码
 * @retval OK: OK
 * @retval -EFAULT: 无效指针
 * @note
 * - 同步/异步：同步
 * - 中断上下文：可以使用
 * - 中断底半部：可以使用
 * - 线程上下文：可以使用
 * - 重入性：对于同一个设备不可重入；对于不同设备可重入
 */
__xwds_api
xwer_t xwds_uartc_remove(struct xwds_uartc * uartc)
{
        xwer_t rc;

        SODS_VALIDATE(uartc, "nullptr", -EFAULT);

        rc = xwds_uartc_cvop_remove(uartc);
        return rc;
}

/**
 * @brief SODS API：启动UART控制器
 * @param uartc: (I) UART控制器对象指针
 * @return 错误码
 * @note
 * - 同步/异步：同步
 * - 中断上下文：可以使用
 * - 中断底半部：可以使用
 * - 线程上下文：可以使用
 * - 重入性：对于同一个设备不可重入；对于不同设备可重入
 */
__xwds_api
xwer_t xwds_uartc_start(struct xwds_uartc * uartc)
{
        xwer_t rc;

        SODS_VALIDATE(uartc, "nullptr", -EFAULT);

        rc = xwds_uartc_cvop_start(uartc);
        return rc;
}

/**
 * @brief SODS API：停止UART控制器
 * @param uartc: (I) UART控制器对象指针
 * @return 错误码
 * @note
 * - 同步/异步：同步
 * - 中断上下文：可以使用
 * - 中断底半部：可以使用
 * - 线程上下文：可以使用
 * - 重入性：对于同一个设备不可重入；对于不同设备可重入
 */
__xwds_api
xwer_t xwds_uartc_stop(struct xwds_uartc * uartc)
{
        xwer_t rc;

        SODS_VALIDATE(uartc, "nullptr", -EFAULT);

        rc = xwds_uartc_cvop_stop(uartc);
        return rc;
}

#if defined(XWMDCFG_ds_LPM) && (1 == XWMDCFG_ds_LPM)
/******** ******** pm ******** ********/
/**
 * @brief SODS API：暂停UART控制器
 * @param uartc: (I) UART控制器对象指针
 * @return 错误码
 * @retval OK: OK
 * @retval -EFAULT: 无效指针
 * @note
 * - 同步/异步：同步
 * - 中断上下文：可以使用
 * - 中断底半部：可以使用
 * - 线程上下文：可以使用
 * - 重入性：对于同一个设备不可重入；对于不同设备可重入
 */
__xwds_api
xwer_t xwds_uartc_suspend(struct xwds_uartc * uartc)
{
        xwer_t rc;

        SODS_VALIDATE(uartc, "nullptr", -EFAULT);

        rc = xwds_uartc_cvop_suspend(uartc);
        return rc;
}

/**
 * @brief SODS API：继续UART控制器
 * @param uartc: (I) UART控制器对象指针
 * @return 错误码
 * @retval OK: OK
 * @retval -EFAULT: 无效指针
 * @note
 * - 同步/异步：同步
 * - 中断上下文：可以使用
 * - 中断底半部：可以使用
 * - 线程上下文：可以使用
 * - 重入性：对于同一个设备不可重入；对于不同设备可重入
 */
__xwds_api
xwer_t xwds_uartc_resume(struct xwds_uartc * uartc)
{
        xwer_t rc;

        SODS_VALIDATE(uartc, "nullptr", -EFAULT);

        rc = xwds_uartc_cvop_resume(uartc);
        return rc;
}
#endif /* XWMDCFG_ds_LPM */
#endif /* XWMDCFG_ds_NANO */

/**
 * @brief SODS API：清空接收队列
 * @param uartc: (I) UART控制器对象指针
 * @return 错误码
 * @retval OK: OK
 * @retval -EFAULT: 无效指针
 * @note
 * - 同步/异步：同步
 * - 中断上下文：可以使用
 * - 中断底半部：可以使用
 * - 线程上下文：可以使用
 * - 重入性：对于同一个设备不可重入；对于不同设备可重入
 */
__xwds_api
xwer_t xwds_uartc_clear_rxq(struct xwds_uartc * uartc)
{
        xwreg_t cpuirq;
        xwer_t rc;

        SODS_VALIDATE(uartc, "nullptr", -EFAULT);

#if !defined(XWMDCFG_ds_NANO) || (1 != XWMDCFG_ds_NANO)
        rc = xwds_uartc_grab(uartc);
        if (__unlikely(rc < 0)) {
                goto err_uartc_grab;
        }
#endif /* !XWMDCFG_ds_NANO */

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

#if !defined(XWMDCFG_ds_NANO) || (1 != XWMDCFG_ds_NANO)
        xwds_uartc_put(uartc);
#endif /* !XWMDCFG_ds_NANO */

        return OK;

err_smr_thaw:
err_smr_freeze:
#if !defined(XWMDCFG_ds_NANO) || (1 != XWMDCFG_ds_NANO)
        xwds_uartc_put(uartc);
err_uartc_grab:
#endif /* !XWMDCFG_ds_NANO */
        return rc;
}

/**
 * @brief SODS API：得到接收队列有效数据的大小
 * @param uartc: (I) UART控制器对象指针
 * @param ret: (O) 指向缓冲区的指针，通过此缓冲区返回接收队列中有效数据的大小
 * @return 错误码
 * @retval OK: OK
 * @retval -EFAULT: 无效指针
 * @note
 * - 同步/异步：同步
 * - 中断上下文：可以使用
 * - 中断底半部：可以使用
 * - 线程上下文：可以使用
 * - 重入性：可重入
 */
__xwds_api
xwer_t xwds_uartc_get_rxq_datasize(struct xwds_uartc * uartc, xwsz_t *ret)
{
        xwssz_t size;
        xwsq_t seq;
        xwer_t rc = OK;

        SODS_VALIDATE(uartc, "nullptr", -EFAULT);
        SODS_VALIDATE(ret, "nullptr", -EFAULT);

#if !defined(XWMDCFG_ds_NANO) || (1 != XWMDCFG_ds_NANO)
        rc = xwds_uartc_grab(uartc);
        if (__unlikely(rc < 0)) {
                goto err_uartc_grab;
        }
#endif /* !XWMDCFG_ds_NANO */

        do {
                seq = xwosal_sqlk_rd_begin(&uartc->rxseqlock);
                size = uartc->rxnum - uartc->rxpos;
                if (size < 0) {
                        size = (xwssz_t)XWMDCFG_ds_UART_GNR_RXQ_SIZE -
                               (xwssz_t)uartc->rxpos;
                        size += uartc->rxnum;
                }
        } while (xwosal_sqlk_rd_retry(&uartc->rxseqlock, seq));

#if !defined(XWMDCFG_ds_NANO) || (1 != XWMDCFG_ds_NANO)
        xwds_uartc_put(uartc);
#endif /* !XWMDCFG_ds_NANO */

        *ret = (xwsz_t)size;
        return rc;

#if !defined(XWMDCFG_ds_NANO) || (1 != XWMDCFG_ds_NANO)
err_uartc_grab:
        return rc;
#endif /* !XWMDCFG_ds_NANO */
}

/**
 * @brief SODS API：从接收队列中获取一个字节的数据
 * @param uartc: (I) UART控制器对象指针
 * @param buf: (O) 指向缓冲区的指针，此缓冲区被用于返回数据
 * @param xwtm: 指向缓冲区的指针，此缓冲区：
 *              (I) 作为输入时，表示期望的阻塞等待时间
 *              (O) 作为输出时，返回剩余的期望时间
 * @return 错误码
 * @retval OK: OK
 * @retval -EFAULT: 无效指针
 * @note
 * - 同步/异步：同步
 * - 中断上下文：不可以使用
 * - 中断底半部：不可以使用
 * - 线程上下文：可以使用
 * - 重入性：可重入
 */
__xwds_api
xwer_t xwds_uartc_getc(struct xwds_uartc * uartc, xwu8_t * buf, xwtm_t * xwtm)
{
        xwer_t rc;
        xwreg_t cpuirq;

        SODS_VALIDATE(uartc, "nullptr", -EFAULT);
        SODS_VALIDATE(buf, "nullptr", -EFAULT);
        SODS_VALIDATE(xwtm, "nullptr", -EFAULT);

#if !defined(XWMDCFG_ds_NANO) || (1 != XWMDCFG_ds_NANO)
        rc = xwds_uartc_grab(uartc);
        if (__unlikely(rc < 0)) {
                goto err_uartc_grab;
        }
#endif /* !XWMDCFG_ds_NANO */

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

#if !defined(XWMDCFG_ds_NANO) || (1 != XWMDCFG_ds_NANO)
        xwds_uartc_put(uartc);
#endif /* !XWMDCFG_ds_NANO */

        return OK;

err_smr_timedwait:
#if !defined(XWMDCFG_ds_NANO) || (1 != XWMDCFG_ds_NANO)
        xwds_uartc_put(uartc);
err_uartc_grab:
#endif /* !XWMDCFG_ds_NANO */
        return rc;
}

/**
 * @brief SODS API：从接收队列中获取多个数据
 * @param uartc: (I) UART控制器对象指针
 * @param buf: (O) 指向缓冲区的指针，此缓冲区被用于返回数据
 * @param size: 指向缓冲区的指针，此缓冲区：
 *              (I) 作为输入时，表示缓冲区大小（单位：字节）
 *              (O) 作为输出时，返回实际读取的数据大小
 * @param xwtm: 指向缓冲区的指针，此缓冲区：
 *              (I) 作为输入时，表示期望的阻塞等待时间
 *              (O) 作为输出时，返回剩余的期望时间
 * @return 错误码
 * @retval OK: OK
 * @retval -EFAULT: 无效指针
 * @note
 * - 同步/异步：同步
 * - 中断上下文：不可以使用
 * - 中断底半部：不可以使用
 * - 线程上下文：可以使用
 * - 重入性：可重入
 */
__xwds_api
xwer_t xwds_uartc_rx(struct xwds_uartc * uartc,
                     void * buf, xwsz_t * size,
                     xwtm_t * xwtm)
{
        xwsz_t i;
        xwer_t rc;
        xwreg_t cpuirq;

        SODS_VALIDATE(uartc, "nullptr", -EFAULT);
        SODS_VALIDATE(buf, "nullptr", -EFAULT);
        SODS_VALIDATE(size, "nullptr", -EFAULT);
        SODS_VALIDATE(xwtm, "nullptr", -EFAULT);

#if !defined(XWMDCFG_ds_NANO) || (1 != XWMDCFG_ds_NANO)
        rc = xwds_uartc_grab(uartc);
        if (__unlikely(rc < 0)) {
                *size = 0;
                goto err_uartc_grab;
        }
#endif /* !XWMDCFG_ds_NANO */

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

#if !defined(XWMDCFG_ds_NANO) || (1 != XWMDCFG_ds_NANO)
        xwds_uartc_put(uartc);
#endif /* !XWMDCFG_ds_NANO */

        *size = i;
        return OK;

err_smr_timedwait:
#if !defined(XWMDCFG_ds_NANO) || (1 != XWMDCFG_ds_NANO)
        xwds_uartc_put(uartc);
err_uartc_grab:
#endif /* !XWMDCFG_ds_NANO */
        return rc;
}

/**
 * @brief SODS API：尝试从接收队列中获取一个字节的数据
 * @param uartc: (I) UART控制器对象指针
 * @param buf: (O) 指向缓冲区的指针，此缓冲区被用于返回数据
 * @return 错误码
 * @retval OK: OK
 * @retval -EFAULT: 无效指针
 * @note
 * - 同步/异步：同步
 * - 中断上下文：可以使用
 * - 中断底半部：可以使用
 * - 线程上下文：可以使用
 * - 重入性：可重入
 */
__xwds_api
xwer_t xwds_uartc_try_getc(struct xwds_uartc * uartc, xwu8_t * buf)
{
        xwer_t rc;
        xwreg_t cpuirq;

        SODS_VALIDATE(uartc, "nullptr", -EFAULT);
        SODS_VALIDATE(buf, "nullptr", -EFAULT);

#if !defined(XWMDCFG_ds_NANO) || (1 != XWMDCFG_ds_NANO)
        rc = xwds_uartc_grab(uartc);
        if (__unlikely(rc < 0)) {
                goto err_uartc_grab;
        }
#endif /* !XWMDCFG_ds_NANO */

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

#if !defined(XWMDCFG_ds_NANO) || (1 != XWMDCFG_ds_NANO)
        xwds_uartc_put(uartc);
#endif /* !XWMDCFG_ds_NANO */

        return OK;

err_smr_trywait:
#if !defined(XWMDCFG_ds_NANO) || (1 != XWMDCFG_ds_NANO)
        xwds_uartc_put(uartc);
err_uartc_grab:
#endif /* !XWMDCFG_ds_NANO */
        return rc;
}

/**
 * @brief SODS API：尝试从接收队列中获取多个字节的数据
 * @param uartc: (I) UART控制器对象指针
 * @param buf: (O) 指向缓冲区的指针，此缓冲区被用于返回数据
 * @param size: 指向缓冲区的指针，此缓冲区：
 *              (I) 作为输入时，表示缓冲区大小（单位：字节）
 *              (O) 作为输出时，返回实际读取的数据大小
 * @return 错误码
 * @retval OK: OK
 * @retval -EFAULT: 无效指针
 * @note
 * - 同步/异步：同步
 * - 中断上下文：可以使用
 * - 中断底半部：可以使用
 * - 线程上下文：可以使用
 * - 重入性：可重入
 */
__xwds_api
xwer_t xwds_uartc_try_rx(struct xwds_uartc * uartc, void * buf, xwsz_t * size)
{
        xwsz_t i;
        xwer_t rc;
        xwreg_t cpuirq;

        SODS_VALIDATE(uartc, "nullptr", -EFAULT);
        SODS_VALIDATE(buf, "nullptr", -EFAULT);
        SODS_VALIDATE(size, "nullptr", -EFAULT);

#if !defined(XWMDCFG_ds_NANO) || (1 != XWMDCFG_ds_NANO)
        rc = xwds_uartc_grab(uartc);
        if (__unlikely(rc < 0)) {
                *size = 0;
                goto err_uartc_grab;
        }
#endif /* !XWMDCFG_ds_NANO */

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

#if !defined(XWMDCFG_ds_NANO) || (1 != XWMDCFG_ds_NANO)
        xwds_uartc_put(uartc);
#endif /* !XWMDCFG_ds_NANO */

        *size = i;
        return OK;

err_smr_trywait:
#if !defined(XWMDCFG_ds_NANO) || (1 != XWMDCFG_ds_NANO)
        xwds_uartc_put(uartc);
err_uartc_grab:
#endif /* !XWMDCFG_ds_NANO */
        return rc;
}

/**
 * @brief SODS API：清空发送队列
 * @param uartc: (I) UART控制器对象指针
 * @return 错误码
 * @retval OK: OK
 * @retval -EFAULT: 无效指针
 * @note
 * - 同步/异步：同步
 * - 中断上下文：可以使用
 * - 中断底半部：可以使用
 * - 线程上下文：可以使用
 * - 重入性：对于同一个设备不可重入；对于不同设备可重入
 */
__xwds_api
xwer_t xwds_uartc_clear_txq(struct xwds_uartc * uartc)
{
        xwer_t rc = OK;
        xwreg_t cpuirq;

        SODS_VALIDATE(uartc, "nullptr", -EFAULT);

#if !defined(XWMDCFG_ds_NANO) || (1 != XWMDCFG_ds_NANO)
        rc = xwds_uartc_grab(uartc);
        if (__unlikely(rc < 0)) {
                goto err_uartc_grab;
        }
#endif /* !XWMDCFG_ds_NANO */

        xwosal_sqlk_wr_lock_cpuirqsv(&uartc->txseqlock, &cpuirq);
        uartc->txnum = 0;
        uartc->txpos = 0;
        xwosal_sqlk_wr_unlock_cpuirqrs(&uartc->txseqlock, cpuirq);

#if !defined(XWMDCFG_ds_NANO) || (1 != XWMDCFG_ds_NANO)
        xwds_uartc_put(uartc);
#endif /* !XWMDCFG_ds_NANO */

        return rc;

#if !defined(XWMDCFG_ds_NANO) || (1 != XWMDCFG_ds_NANO)
err_uartc_grab:
        return rc;
#endif /* !XWMDCFG_ds_NANO */
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
                if (OK == rc) {
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
                        rc = OK;
                }
        }
        xwosal_sqlk_wr_unlock_cpuirqrs(&uartc->txseqlock, cpuirq);
        return rc;
}

/**
 * @brief SODS API：发送一个字节
 * @param uartc: (I) UART控制器对象指针
 * @param byte: (I) 待发送的数据
 * @param xwtm: 指向缓冲区的指针，此缓冲区：
 *              (I) 作为输入时，表示期望的阻塞等待时间
 *              (O) 作为输出时，返回剩余的期望时间
 * @return 错误码
 * @retval OK: OK
 * @retval -EFAULT: 无效指针
 * @retval -EOVERFLOW: 发送队列已满
 * @note
 * - 同步/异步：同步
 * - 中断上下文：不可以使用
 * - 中断底半部：不可以使用
 * - 线程上下文：可以使用
 * - 重入性：对于同一个设备不可重入；对于不同设备可重入
 */
__xwds_api
xwer_t xwds_uartc_putc(struct xwds_uartc * uartc,
                       const xwu8_t byte,
                       xwtm_t * xwtm)
{
        xwer_t rc;

        SODS_VALIDATE(uartc, "nullptr", -EFAULT);
        SODS_VALIDATE(xwtm, "nullptr", -EFAULT);

#if !defined(XWMDCFG_ds_NANO) || (1 != XWMDCFG_ds_NANO)
        rc = xwds_uartc_grab(uartc);
        if (__unlikely(rc < 0)) {
                goto err_uartc_grab;
        }
#endif /* !XWMDCFG_ds_NANO */

        rc = xwosal_mtx_timedlock(xwosal_mtx_get_id(&uartc->txmtx), xwtm);
        if (__unlikely(rc < 0)) {
                goto err_uartc_lock;
        }
        rc = xwds_uartc_tx_1byte(uartc, byte);
        if (__unlikely(rc < 0)) {
                goto err_tx_1byte;
        }
        xwosal_mtx_unlock(xwosal_mtx_get_id(&uartc->txmtx));

#if !defined(XWMDCFG_ds_NANO) || (1 != XWMDCFG_ds_NANO)
        xwds_uartc_put(uartc);
#endif /* !XWMDCFG_ds_NANO */

        return OK;

err_tx_1byte:
        xwosal_mtx_unlock(xwosal_mtx_get_id(&uartc->txmtx));
err_uartc_lock:
#if !defined(XWMDCFG_ds_NANO) || (1 != XWMDCFG_ds_NANO)
        xwds_uartc_put(uartc);
err_uartc_grab:
#endif /* !XWMDCFG_ds_NANO */
        return rc;
}

/**
 * @brief SODS API：发送一些数据
 * @param uartc: (I) UART控制器对象指针
 * @param data: (I) 待发送的数据的缓冲区
 * @param size: 指向缓冲区的指针，此缓冲区：
 *              (I) 作为输入时，表示待发送的数据的大小
 *              (O) 作为输出时，返回实际放入发送队列的数据的大小
 * @param xwtm: 指向缓冲区的指针，此缓冲区：
 *              (I) 作为输入时，表示期望的阻塞等待时间
 *              (O) 作为输出时，返回剩余的期望时间
 * @return 错误码
 * @retval OK: OK
 * @retval -EFAULT: 无效指针
 * @note
 * - 同步/异步：同步
 * - 中断上下文：不可以使用
 * - 中断底半部：不可以使用
 * - 线程上下文：可以使用
 * - 重入性：对于同一个设备不可重入；对于不同设备可重入
 */
__xwds_api
xwer_t xwds_uartc_tx(struct xwds_uartc * uartc,
                     const xwu8_t * data, xwsz_t * size,
                     xwtm_t * xwtm)
{
        xwsz_t i;
        xwer_t rc;

        SODS_VALIDATE(uartc, "nullptr", -EFAULT);
        SODS_VALIDATE(data, "nullptr", -EFAULT);
        SODS_VALIDATE(size, "nullptr", -EFAULT);
        SODS_VALIDATE(xwtm, "nullptr", -EFAULT);

#if !defined(XWMDCFG_ds_NANO) || (1 != XWMDCFG_ds_NANO)
        rc = xwds_uartc_grab(uartc);
        if (__unlikely(rc < 0)) {
                *size = 0;
                goto err_uartc_grab;
        }
#endif /* !XWMDCFG_ds_NANO */

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

#if !defined(XWMDCFG_ds_NANO) || (1 != XWMDCFG_ds_NANO)
        xwds_uartc_put(uartc);
#endif /* !XWMDCFG_ds_NANO */

        *size = i;
        return OK;

err_tx_1byte:
        xwosal_mtx_unlock(xwosal_mtx_get_id(&uartc->txmtx));
err_uartc_lock:
#if !defined(XWMDCFG_ds_NANO) || (1 != XWMDCFG_ds_NANO)
        xwds_uartc_put(uartc);
err_uartc_grab:
#endif /* !XWMDCFG_ds_NANO */
        return rc;
}

/**
 * @brief SODS API：配置UART
 * @param uartc: (I) UART控制器对象指针
 * @param cfg: (I) 新的配置
 * @return 错误码
 * @retval OK: OK
 * @retval -EFAULT: 无效指针
 * @note
 * - 同步/异步：同步
 * - 中断上下文：可以使用
 * - 中断底半部：可以使用
 * - 线程上下文：可以使用
 * - 重入性：对于同一个设备不可重入；对于不同设备可重入
 */
__xwds_api
xwer_t xwds_uartc_cfg(struct xwds_uartc * uartc,
                      const struct xwds_uart_cfg * cfg)
{
        const struct xwds_uartc_driver * drv;
        xwer_t rc;

        SODS_VALIDATE(uartc, "nullptr", -EFAULT);
        SODS_VALIDATE(cfg, "nullptr", -EFAULT);

#if !defined(XWMDCFG_ds_NANO) || (1 != XWMDCFG_ds_NANO)
        rc = xwds_uartc_grab(uartc);
        if (__unlikely(rc < 0)) {
                goto err_uartc_grab;
        }
#endif /* !XWMDCFG_ds_NANO */

        drv = xwds_static_cast(const struct xwds_uartc_driver *, uartc->dev.drv);
        if ((drv) && (drv->cfg)) {
                rc = drv->cfg(uartc, cfg);
        } else {
                rc = -ENOSYS;
        }
        if (__unlikely(rc < 0)) {
                goto err_drv_cfg;
        }

#if !defined(XWMDCFG_ds_NANO) || (1 != XWMDCFG_ds_NANO)
        xwds_uartc_put(uartc);
#endif /* !XWMDCFG_ds_NANO */

        return OK;

err_drv_cfg:
#if !defined(XWMDCFG_ds_NANO) || (1 != XWMDCFG_ds_NANO)
        xwds_uartc_put(uartc);
err_uartc_grab:
#endif /* !XWMDCFG_ds_NANO */
        return rc;
}

/******** ******** Libraries for BSP driver ******** ********/
/**
 * @brief SODS LIB：UART接收中断响应函数
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
 * @brief SODS LIB：UART发送中断响应函数
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
                if (OK == rc) {
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
