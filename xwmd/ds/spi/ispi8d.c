/**
 * @file
 * @brief xwmd设备栈：inter-SPI-8-bit从机通讯协议设备
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
#include <xwos/osal/scheduler.h>
#include <xwos/osal/thread.h>
#include <xwos/osal/lock/spinlock.h>
#include <xwos/osal/sync/semaphore.h>
#include <xwmd/ds/spi/ispi8d.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********     static function prototypes      ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
static __xwds_vop
xwer_t xwds_ispi8d_cvop_probe(struct xwds_ispi8d * ispi8d);

static __xwds_vop
xwer_t xwds_ispi8d_cvop_remove(struct xwds_ispi8d * ispi8d);

static __xwds_vop
xwer_t xwds_ispi8d_cvop_start(struct xwds_ispi8d * ispi8d);

static __xwds_vop
xwer_t xwds_ispi8d_cvop_stop(struct xwds_ispi8d * ispi8d);

#if defined(XWMDCFG_ds_PM) && (1 == XWMDCFG_ds_PM)
static __xwds_vop
xwer_t xwds_ispi8d_cvop_suspend(struct xwds_ispi8d * ispi8d);

static __xwds_vop
xwer_t xwds_ispi8d_cvop_resume(struct xwds_ispi8d * ispi8d);
#endif /* XWMDCFG_ds_PM */

static __xwds_code
xwer_t xwds_ispi8d_get_rxslot(struct xwds_ispi8d * ispi8d,
                              struct xwds_ispi8d_rxslot ** rxslotbuf);

static __xwds_code
xwer_t xwds_ispi8d_publish_rxslot(struct xwds_ispi8d * ispi8d);

static __xwds_code
void xwds_ispi8d_getdata(struct xwds_ispi8d * ispi8d, xwu8_t * buf, xwsz_t * size);

static __xwds_code
void xwds_ispi8d_tx_complete(struct xwds_ispi8d_txslot * txslot);

static __xwds_code
xwer_t xwds_ispi8d_cb_lock(void * arg);

static __xwds_code
xwer_t xwds_ispi8d_cb_unlock(void * arg);

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
__xwds_rodata const struct xwds_base_virtual_operations xwds_ispi8d_cvops = {
        .probe = (void *)xwds_ispi8d_cvop_probe,
        .remove = (void *)xwds_ispi8d_cvop_remove,
        .start = (void *)xwds_ispi8d_cvop_start,
        .stop = (void *)xwds_ispi8d_cvop_stop,
#if defined(XWMDCFG_ds_PM) && (1 == XWMDCFG_ds_PM)
        .suspend = (void *)xwds_ispi8d_cvop_suspend,
        .resume = (void *)xwds_ispi8d_cvop_resume,
#endif /* XWMDCFG_ds_PM */
};

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/******** ******** ******** constructor & destructor ******** ******** ********/
/**
 * @brief SODS API：iSPI8设备的构造函数
 * @param ispi8d: (I) iSPI8设备对象指针
 */
__xwds_api
void xwds_ispi8d_construct(struct xwds_ispi8d * ispi8d)
{
        xwds_device_construct(&ispi8d->dev);
        ispi8d->dev.cvops = &xwds_ispi8d_cvops;
}

/**
 * @brief SODS API：iSPI8设备对象的析构函数
 * @param ispi8d: (I) iSPI8设备对象指针
 */
__xwds_api
void xwds_ispi8d_destruct(struct xwds_ispi8d * ispi8d)
{
        struct xwds_device * dev;

        dev = xwds_static_cast(struct xwds_device *, ispi8d);
        xwds_device_destruct(dev);
}

/******** ******** base virtual operations ******** ********/
/**
 * @brief SODS VOP：探测iSPI8设备
 * @param ispi8d: (I) iSPI8设备对象指针
 * @return 错误码
 */
static __xwds_vop
xwer_t xwds_ispi8d_cvop_probe(struct xwds_ispi8d * ispi8d)
{
        xwer_t rc;

        xwosal_splk_init(&ispi8d->comi.lock);
        ispi8d->comi.state = SODS_ISPI8D_STATE_IDLE;
        ispi8d->comi.ctxslot = NULL;
        ispi8d->comi.ntxslot = NULL;
        ispi8d->comi.crxslot = NULL;

        xwosal_splk_init(&ispi8d->rxq.lock);
        ispi8d->rxq.num = 0;
        ispi8d->rxq.pos = 0;
        memset(ispi8d->rxq.slot, 0, sizeof(ispi8d->rxq.slot));

        rc = xwosal_smr_init(&ispi8d->rxq.smr, 0, XWMDCFG_ds_SPI_ISPI8C_RXQ_SIZE);
        if (__unlikely(rc < 0)) {
                goto err_rxqsmr_init;
        }
        rc = xwosal_smr_freeze(xwosal_smr_get_id(&ispi8d->rxq.smr));
        if (__unlikely(rc < 0)) {
                goto err_rxqsmr_freeze;
        }
        rc = xwosal_mtx_init(&ispi8d->txmtx, XWOSAL_SD_PRIORITY_RT_MIN);
        if (__unlikely(rc < 0)) {
                goto err_txmtx_init;
        }
        rc = xwds_device_cvop_probe(&ispi8d->dev);
        if (__unlikely(rc < 0)) {
                goto err_dev_probe;
        }
        return OK;

err_dev_probe:
        xwosal_mtx_destroy(&ispi8d->txmtx);
err_txmtx_init:
err_rxqsmr_freeze:
        xwosal_smr_destroy(&ispi8d->rxq.smr);
err_rxqsmr_init:
        return rc;
}

/**
 * @brief SODS VOP：移除iSPI8设备
 * @param ds: (I) 设备栈控制块指针
 * @param ispi8d: (I) iSPI8设备对象指针
 * @return 错误码
 */
static __xwds_vop
xwer_t xwds_ispi8d_cvop_remove(struct xwds_ispi8d * ispi8d)
{
        xwer_t rc;

        rc = xwds_device_cvop_remove(&ispi8d->dev);
        if (__unlikely(rc < 0)) {
                goto err_dev_cvop_remove;
        }

        xwosal_mtx_destroy(&ispi8d->txmtx);
        xwosal_smr_destroy(&ispi8d->rxq.smr);
        return OK;

err_dev_cvop_remove:
        return rc;
}

/**
 * @brief SODS VOP：启动iSPI8设备
 * @param ispi8d: (I) iSPI8设备对象指针
 * @return 错误码
 */
static __xwds_vop
xwer_t xwds_ispi8d_cvop_start(struct xwds_ispi8d * ispi8d)
{
        xwreg_t cpuirq;
        xwer_t rc;

        rc = xwds_device_cvop_start(&ispi8d->dev);
        if (__unlikely(rc < 0)) {
                goto err_dev_cvop_start;
        }

        xwosal_splk_lock_cpuirqsv(&ispi8d->rxq.lock, &cpuirq);
        ispi8d->rxq.pos = 0;
        ispi8d->rxq.num = 0;
        xwosal_splk_unlock_cpuirqrs(&ispi8d->rxq.lock, cpuirq);
        rc = xwosal_smr_thaw(xwosal_smr_get_id(&ispi8d->rxq.smr), 0,
                             XWMDCFG_ds_SPI_ISPI8C_RXQ_SIZE);
        return rc;

err_dev_cvop_start:
        return rc;
}

/**
 * @brief SODS VOP：停止iSPI8设备
 * @param ispi8d: (I) iSPI8设备对象指针
 * @return 错误码
 */
static __xwds_vop
xwer_t xwds_ispi8d_cvop_stop(struct xwds_ispi8d * ispi8d)
{
        xwreg_t cpuirq;
        xwer_t rc;

        rc = xwosal_smr_freeze(xwosal_smr_get_id(&ispi8d->rxq.smr));
        if (__unlikely(rc < 0)) {
                goto err_rxsmr_freeze;
        }
        rc = xwds_device_cvop_stop(&ispi8d->dev);
        if (__unlikely(rc < 0)) {
                goto err_dev_stop;
        }

        xwosal_splk_lock_cpuirqsv(&ispi8d->rxq.lock, &cpuirq);
        ispi8d->rxq.pos = 0;
        ispi8d->rxq.num = 0;
        xwosal_splk_unlock_cpuirqrs(&ispi8d->rxq.lock, cpuirq);
        return OK;

err_dev_stop:
        xwosal_smr_thaw(xwosal_smr_get_id(&ispi8d->rxq.smr), 0,
                        XWMDCFG_ds_SPI_ISPI8C_RXQ_SIZE);
err_rxsmr_freeze:
        return rc;
}

#if defined(XWMDCFG_ds_PM) && (1 == XWMDCFG_ds_PM)
/******** ******** pm ******** ********/
/**
 * @brief SODS VOP：暂停iSPI8设备
 * @param ispi8d: (I) iSPI8设备对象指针
 * @return 错误码
 */
static __xwds_vop
xwer_t xwds_ispi8d_cvop_suspend(struct xwds_ispi8d * ispi8d)
{
        xwer_t rc;

        rc = xwds_device_cvop_suspend(&ispi8d->dev);
        return rc;
}

/**
 * @brief SODS VOP：继续iSPI8设备
 * @param ispi8d: (I) iSPI8设备对象指针
 * @return 错误码
 */
static __xwds_vop
xwer_t xwds_ispi8d_cvop_resume(struct xwds_ispi8d * ispi8d)
{
        xwer_t rc;

        rc = xwds_device_cvop_resume(&ispi8d->dev);
        return rc;
}
#endif /* XWMDCFG_ds_PM */

/******** ******** ******** APIs ******** ******** ********/
static __xwds_code
xwer_t xwds_ispi8d_get_rxslot(struct xwds_ispi8d * ispi8d,
                              struct xwds_ispi8d_rxslot ** rxslotbuf)
{
        struct xwds_ispi8d_rxslot * rxslot;
        xwreg_t cpuirq;
        xwer_t rc;

        xwosal_splk_lock_cpuirqsv(&ispi8d->rxq.lock, &cpuirq);
        rxslot = &ispi8d->rxq.slot[ispi8d->rxq.num];
        ispi8d->rxq.num++;
        if (ispi8d->rxq.num >= (xwssz_t)XWMDCFG_ds_SPI_ISPI8C_RXQ_SIZE) {
                ispi8d->rxq.num = 0;
        }
        if (ispi8d->rxq.num == ispi8d->rxq.pos) {
                /* overflow */
                xwlogf(ERR, "<%s:%d> RX queue is overflow!\n",
                       ispi8d->dev.name, ispi8d->dev.id);
                if (0 == ispi8d->rxq.num) {
                        ispi8d->rxq.num = XWMDCFG_ds_SPI_ISPI8C_RXQ_SIZE - 1;
                } else {
                        ispi8d->rxq.num--;
                }
                rxslot = NULL;
                rc = -EOVERFLOW;
        } else {
                rc = OK;
        }
        xwosal_splk_unlock_cpuirqrs(&ispi8d->rxq.lock, cpuirq);
        *rxslotbuf = rxslot;
        return rc;
}

static __xwds_code
xwer_t xwds_ispi8d_publish_rxslot(struct xwds_ispi8d * ispi8d)
{
        return xwosal_smr_post(xwosal_smr_get_id(&ispi8d->rxq.smr));
}

/**
 * @brief SODS API：清空接收队列
 * @param ispi8d: (I) iSPI8设备对象指针
 * @note
 * - 同步/异步：同步
 * - 中断上下文：可以使用
 * - 中断底半部：可以使用
 * - 线程上下文：可以使用
 * - 重入性：对于同一个设备不可重入；对于不同设备可重入
 */
__xwds_api
xwer_t xwds_ispi8d_clear_rxq(struct xwds_ispi8d * ispi8d)
{
        xwreg_t cpuirq;
        xwer_t rc;

        SODS_VALIDATE(ispi8d, "nullptr", -EFAULT);

        rc = xwds_ispi8d_grab(ispi8d);
        if (__unlikely(rc < 0)) {
                goto err_ispi8d_grab;
        }

        rc = xwosal_smr_freeze(xwosal_smr_get_id(&ispi8d->rxq.smr));
        if (__unlikely(rc < 0)) {
                goto err_smr_freeze;
        }
        xwosal_splk_lock_cpuirqsv(&ispi8d->rxq.lock, &cpuirq);
        ispi8d->rxq.num = 0;
        ispi8d->rxq.pos = 0;
        xwosal_splk_unlock_cpuirqrs(&ispi8d->rxq.lock, cpuirq);
        rc = xwosal_smr_thaw(xwosal_smr_get_id(&ispi8d->rxq.smr), 0,
                             XWMDCFG_ds_SPI_ISPI8C_RXQ_SIZE);
        if (__unlikely(rc < 0)) {
                goto err_smr_thaw;
        }

        xwds_ispi8d_put(ispi8d);
        return OK;

err_smr_thaw:
err_smr_freeze:
        xwds_ispi8d_put(ispi8d);
err_ispi8d_grab:
        return rc;
}

static __xwds_code
void xwds_ispi8d_getdata(struct xwds_ispi8d * ispi8d, xwu8_t * buf, xwsz_t * size)
{
        struct xwds_ispi8d_rxslot * rxslot;
        xwreg_t cpuirq;
        xwsz_t realsize;

        xwosal_splk_lock_cpuirqsv(&ispi8d->rxq.lock, &cpuirq);
        rxslot = &ispi8d->rxq.slot[ispi8d->rxq.pos];
        ispi8d->rxq.pos++;
        if (ispi8d->rxq.pos >= (xwssq_t)XWMDCFG_ds_SPI_ISPI8C_RXQ_SIZE) {
                ispi8d->rxq.pos = 0;
        }
        xwosal_splk_unlock_cpuirqrs(&ispi8d->rxq.lock, cpuirq);
        realsize = *size > rxslot->size ? *size : rxslot->size;
        memcpy(buf, rxslot->buf, realsize);
        *size = realsize;
}

/**
 * @brief SODS API：从接收队列中读取数据
 * @param ispi8d: (I) iSPI8设备对象指针
 * @param buf: (O) 指向缓冲区的指针，通过此缓冲区返回数据
 * @param size: 指向缓冲区的指针，此缓冲区：
 *              (I) 作为输入时，表示缓冲区的大小
 *              (O) 作为输出时，返回实际接收到的数据的大小
 * @param xwtm: 指向缓冲区的指针，此缓冲区：
 *              (I) 作为输入时，表示期望的阻塞等待时间
 *              (O) 作为输出时，返回剩余的期望时间
 * @return 错误码
 * @note
 * - 同步/异步：同步
 * - 中断上下文：可以使用
 * - 中断底半部：可以使用
 * - 线程上下文：可以使用
 * - 重入性：可重入
 */
__xwds_api
xwer_t xwds_ispi8d_rx(struct xwds_ispi8d * ispi8d, xwu8_t * buf, xwsz_t * size,
                      xwtm_t * xwtm)
{
        xwer_t rc;

        SODS_VALIDATE(ispi8d, "nullptr", -EFAULT);
        SODS_VALIDATE(buf, "nullptr", -EFAULT);
        SODS_VALIDATE(size, "nullptr", -EFAULT);
        SODS_VALIDATE(xwtm, "nullptr", -EFAULT);

        rc = xwds_ispi8d_grab(ispi8d);
        if (__unlikely(rc < 0)) {
                goto err_ispi8d_grab;
        }

        rc = xwosal_smr_timedwait(xwosal_smr_get_id(&ispi8d->rxq.smr), xwtm);
        if (__unlikely(rc < 0)) {
                goto err_smr_timedwait;
        }
        xwds_ispi8d_getdata(ispi8d, buf, size);

        xwds_ispi8d_put(ispi8d);
        return OK;

err_smr_timedwait:
        xwds_ispi8d_put(ispi8d);
err_ispi8d_grab:
        return rc;
}

/**
 * @brief SODS API：尝试从接收队列中读取数据
 * @param ispi8d: (I) iSPI8设备对象指针
 * @param buf: (O) 指向缓冲区的指针，通过此缓冲区返回数据
 * @param size: 指向缓冲区的指针，此缓冲区：
 *              (I) 作为输入时，表示缓冲区的大小
 *              (O) 作为输出时，返回实际接收到的数据的大小
 * @return 错误码
 * @note
 * - 同步/异步：同步
 * - 中断上下文：可以使用
 * - 中断底半部：可以使用
 * - 线程上下文：可以使用
 * - 重入性：可重入
 */
__xwds_api
xwer_t xwds_ispi8d_try_rx(struct xwds_ispi8d * ispi8d, xwu8_t * buf, xwsz_t * size)
{
        xwer_t rc;

        SODS_VALIDATE(ispi8d, "nullptr", -EFAULT);
        SODS_VALIDATE(buf, "nullptr", -EFAULT);
        SODS_VALIDATE(size, "nullptr", -EFAULT);

        rc = xwds_ispi8d_grab(ispi8d);
        if (__unlikely(rc < 0)) {
                goto err_ispi8d_grab;
        }

        rc = xwosal_smr_trywait(xwosal_smr_get_id(&ispi8d->rxq.smr));
        if (__unlikely(rc < 0)) {
                goto err_smr_trywait;
        }
        xwds_ispi8d_getdata(ispi8d, buf, size);

        xwds_ispi8d_put(ispi8d);
        return OK;

err_smr_trywait:
        xwds_ispi8d_put(ispi8d);
err_ispi8d_grab:
        return rc;
}

static __xwds_code
void xwds_ispi8d_tx_complete(struct xwds_ispi8d_txslot * txslot)
{
        xwosal_thrd_continue((xwid_t)txslot->context);
}

static __xwds_code
xwer_t xwds_ispi8d_cb_lock(void * arg)
{
        struct xwds_ispi8d * ispi8d;
        const struct xwds_ispi8d_driver * drv;
        xwer_t rc;

        ispi8d = arg;
        drv = xwds_static_cast(const struct xwds_ispi8d_driver *, ispi8d->dev.drv);
        rc = OK;
        if ((drv) && (drv->notify)) {
                rc = drv->notify(ispi8d, SODS_ISPI8D_DATA_NON_AVAILABLE);
        }
        xwosal_splk_lock_cpuirq(&ispi8d->comi.lock);
        return rc;
}

static __xwds_code
xwer_t xwds_ispi8d_cb_unlock(void * arg)
{
        struct xwds_ispi8d * ispi8d;
        const struct xwds_ispi8d_driver * drv;
        xwer_t rc;

        ispi8d = arg;
        drv = xwds_static_cast(const struct xwds_ispi8d_driver *, ispi8d->dev.drv);
        xwosal_splk_unlock_cpuirq(&ispi8d->comi.lock);
        rc = OK;
        if ((drv) && (drv->notify)) {
                rc = drv->notify(ispi8d, SODS_ISPI8D_DATA_AVAILABLE);
        }
        return rc;
}

/**
 * @brief SODS API：通知主机，并准备发送
 * @param ispi8d: (I) iSPI8设备对象指针
 * @param data: (I) 待发送的数据的缓冲区指针
 * @param size: (I) 待发送的数据的大小
 * @param xwtm: 指向缓冲区的指针，此缓冲区：
 *              (I) 作为输入时，表示期望的阻塞等待时间
 *              (O) 作为输出时，返回剩余的期望时间
 * @return 错误码
 * @note
 * - 同步/异步：同步
 * - 中断上下文：可以使用
 * - 中断底半部：可以使用
 * - 线程上下文：可以使用
 * - 重入性：可重入
 */
__xwds_api
xwer_t xwds_ispi8d_tx(struct xwds_ispi8d * ispi8d,
                      const xwu8_t * data, xwsz_t size,
                      xwtm_t * xwtm)
{
        struct xwds_ispi8d_txslot txslot;
        const struct xwds_ispi8d_driver * drv;
        struct xwos_lockcb lockcb;
        xwsq_t lkst;
        xwreg_t cpuirq;
        xwer_t rc;

        SODS_VALIDATE(ispi8d, "nullptr", -EFAULT);
        SODS_VALIDATE(data, "nullptr", -EFAULT);
        SODS_VALIDATE(xwtm, "nullptr", -EFAULT);

        if (size > (xwsz_t)XWMDCFG_ds_SPI_ISPI8C_MAX_COMM_BYTES) {
                rc = -EMSGSIZE;
                goto err_msgsize;
        }

        rc = xwds_ispi8d_grab(ispi8d);
        if (__unlikely(rc < 0)) {
                goto err_ispi8d_grab;
        }
        rc = xwds_ispi8d_request(ispi8d);
        if (__unlikely(rc < 0)) {
                goto err_ispi8d_request;
        }

        rc = xwosal_mtx_timedlock(xwosal_mtx_get_id(&ispi8d->txmtx), xwtm);
        if (__unlikely(rc < 0)) {
                goto err_ispi8d_lock;
        }
        txslot.size = (xwu8_t)size;
        txslot.data = (void *)data;
        txslot.pos = 0;
        txslot.rc = -EINPROGRESS;
        txslot.complete = xwds_ispi8d_tx_complete;
        txslot.context = (void *)xwosal_cthrd_get_tid();

        lockcb.lock = xwds_ispi8d_cb_lock;
        lockcb.unlock = xwds_ispi8d_cb_unlock;

        xwosal_splk_lock_cpuirqsv(&ispi8d->comi.lock, &cpuirq);
        ispi8d->comi.ntxslot = &txslot;
        rc = xwosal_cthrd_timedpause(&lockcb,
                                     XWLK_TYPE_CALLBACK,
                                     ispi8d, 1,
                                     xwtm,
                                     &lkst);
        if (OK == rc) {
                xwosal_splk_unlock_cpuirqrs(&ispi8d->comi.lock, cpuirq);
                rc = txslot.rc;
        } else {
                if (XWLK_STATE_LOCKED == lkst) {
                        xwosal_splk_unlock_cpuirqrs(&ispi8d->comi.lock, cpuirq);
                        drv = xwds_static_cast(const struct xwds_ispi8d_driver *,
                                               ispi8d->dev.drv);
                        if ((drv) && (drv->notify)) {
                                rc = drv->notify(ispi8d,
                                                 SODS_ISPI8D_DATA_NON_AVAILABLE);
                        }
                }
        }
        xwosal_mtx_unlock(xwosal_mtx_get_id(&ispi8d->txmtx));

err_ispi8d_lock:
        xwds_ispi8d_release(ispi8d);
err_ispi8d_request:
        xwds_ispi8d_put(ispi8d);
err_ispi8d_grab:
err_msgsize:
        return rc;
}

/**
 * @brief SODS API：改变片选状态
 * @param ispi8d: (I) iSPI8设备对象指针
 * @param cs: (I) SODS_SPI_CS_ACTIVE 或 SODS_SPI_CS_INACTIVE
 * @param xwtm: 指向缓冲区的指针，此缓冲区：
 *              (I) 作为输入时，表示期望的阻塞等待时间
 *              (O) 作为输出时，返回剩余的期望时间
 * @return 错误码
 * @note
 * - 同步/异步：同步
 * - 中断上下文：可以使用
 * - 中断底半部：可以使用
 * - 线程上下文：可以使用
 * - 重入性：对于同一个设备不可重入；对于不同设备可重入
 */
__xwds_api
xwer_t xwds_ispi8d_swcs(struct xwds_ispi8d * ispi8d, xwu8_t cs, xwtm_t * xwtm)
{
        const struct xwds_ispi8d_driver * drv;
        xwer_t rc;

        SODS_VALIDATE(ispi8d, "nullptr", -EFAULT);
        SODS_VALIDATE(xwtm, "nullptr", -EFAULT);

        rc = xwds_ispi8d_grab(ispi8d);
        if (__unlikely(rc < 0)) {
                goto err_ispi8d_grab;
        }
        rc = xwds_ispi8d_request(ispi8d);
        if (__unlikely(rc < 0)) {
                goto err_ispi8d_request;
        }

        rc = xwosal_mtx_timedlock(xwosal_mtx_get_id(&ispi8d->txmtx), xwtm);
        if (__unlikely(rc < 0)) {
                goto err_ispi8d_lock;
        }
        drv = xwds_static_cast(const struct xwds_ispi8d_driver *, ispi8d->dev.drv);
        if (__likely(drv && drv->swcs)) {
                rc = drv->swcs(ispi8d, cs, xwtm);
        } else {
                rc = -ENOSYS;
        }
        if (__unlikely(rc < 0)) {
                goto err_drv_swcs;
        }
        xwosal_mtx_unlock(xwosal_mtx_get_id(&ispi8d->txmtx));

        xwds_ispi8d_release(ispi8d);
        xwds_ispi8d_put(ispi8d);
        return OK;

err_drv_swcs:
        xwosal_mtx_unlock(xwosal_mtx_get_id(&ispi8d->txmtx));
err_ispi8d_lock:
        xwds_ispi8d_release(ispi8d);
err_ispi8d_request:
        xwds_ispi8d_put(ispi8d);
err_ispi8d_grab:
        return rc;
}

/******** ******** Libraries for BSP driver ******** ********/
/**
 * @brief SODS LIB：交换同步字节
 * @param ispi8d: (I) iSPI8设备对象指针
 * @param input: (I) 主机端的同步字节
 * @param txslotbuf: (O) 指向缓冲区的指针，通过此缓存区返回本次通讯的发送槽的指针
 * @return 错误码
 * @retval OK: 通讯完成
 * @retval -EPROTO: 没有收到主机的同步信号
 * @note
 * - 这个函数只能在取得锁ispi8d->comi.lock时调用
 */
__xwds_lib_code
xwer_t xwds_ispi8d_lib_sync(struct xwds_ispi8d * ispi8d, xwu8_t input,
                            struct xwds_ispi8d_txslot ** txslotbuf)
{
        xwer_t rc;

        if (XWMDCFG_ds_SPI_ISPI8C_SYNC_CHAR != input) {
                rc = -EPROTO;
        } else {
                rc = OK;
                ispi8d->comi.state = SODS_ISPI8D_STATE_SYNC;
                ispi8d->comi.ctxslot = ispi8d->comi.ntxslot;
                ispi8d->comi.ntxslot = NULL;
                *txslotbuf = ispi8d->comi.ctxslot;
        }
        return rc;
}

/**
 * @brief SODS LIB：交换数据大小
 * @param ispi8d: (I) iSPI8设备对象指针
 * @param input: (I) 主机端的数据大小
 * @param txslotbuf: (O) 指向缓冲区的指针，通过此缓冲区返回本次通讯的发送槽的指针
 * @return 错误码
 * @retval OK: 通讯完成
 * @retval -EBADMSG: 主机端的数据大小
 * @note
 * - 这个函数只能在取得锁ispi8d->comi.lock时调用
 */
__xwds_lib_code
xwer_t xwds_ispi8d_lib_swapsize(struct xwds_ispi8d * ispi8d, xwu8_t input,
                                struct xwds_ispi8d_txslot ** txslotbuf)
{
        struct xwds_ispi8d_rxslot * rxslot;
        xwer_t rc;

        if (input > XWMDCFG_ds_SPI_ISPI8C_MAX_COMM_BYTES) {
                rc = -EBADMSG;
        } else {
                rc = OK;
                ispi8d->comi.state = SODS_ISPI8D_STATE_XDATA;
                if (input > 0) {
                        rc = xwds_ispi8d_get_rxslot(ispi8d, &rxslot);
                        if (OK == rc) {
                                rxslot->pos = 0;
                                rxslot->size = input;
                                ispi8d->comi.crxslot = rxslot;
                        } else {
                                ispi8d->comi.crxslot = NULL;
                        }
                }
                *txslotbuf = ispi8d->comi.ctxslot;
        }
        return rc;
}

/**
 * @brief SODS LIB：交换数据
 * @param ispi8d: (I) iSPI8设备对象指针
 * @param rxdata: (I) 接收到的数据的缓冲区的指针
 * @param rxdata: (I) 接收到的数据的大小
 * @param txslotbuf: (O) 指向缓冲区的指针，通过此缓冲区返回本次通讯的发送槽的指针
 * @return 错误码
 * @retval OK: 通讯完成
 * @retval -EINPROGRESS: 通讯正在进行
 * @note
 * - 这个函数只能在取得锁ispi8d->comi.lock时调用
 */
__xwds_lib_code
xwer_t xwds_ispi8d_lib_swapdata(struct xwds_ispi8d * ispi8d,
                                xwu8_t * rxdata, xwsz_t rxsize,
                                struct xwds_ispi8d_txslot ** txslotbuf)
{
        struct xwds_ispi8d_txslot * txslot;
        struct xwds_ispi8d_rxslot * rxslot;
        xwer_t rc;

        rc = -EINPROGRESS;
        if ((ispi8d->comi.crxslot) && (rxsize > 0)) {
                rxslot = ispi8d->comi.crxslot;
                if (rxslot->pos + rxsize <= rxslot->size) {
                        memcpy(&rxslot->buf[rxslot->pos], rxdata, rxsize);
                        rxslot->pos += rxsize;
                } else {
                        rxsize = rxslot->size - rxslot->pos;
                        memcpy(&rxslot->buf[rxslot->pos], rxdata, rxsize);
                        rxslot->pos = rxslot->size;
                        ispi8d->comi.crxslot = NULL;
                        xwds_ispi8d_publish_rxslot(ispi8d);
                }
        }
        if (ispi8d->comi.ctxslot) {
                txslot = ispi8d->comi.ctxslot;
                if (txslot->pos == txslot->size) {
                        ispi8d->comi.ctxslot = NULL;
                        txslot->rc = OK;
                        txslot->complete(txslot);
                }
        }

        *txslotbuf = ispi8d->comi.ctxslot;
        if ((NULL == ispi8d->comi.crxslot) && (NULL == ispi8d->comi.ctxslot)) {
                ispi8d->comi.state = SODS_ISPI8D_STATE_IDLE;
                rc = OK;
        }
        return rc;
}
