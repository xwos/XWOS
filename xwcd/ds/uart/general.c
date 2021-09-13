/**
 * @file
 * @brief 玄武设备栈：UART
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
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
#include <xwos/osal/skd.h>
#include <xwos/osal/lock/seqlock.h>
#include <xwos/osal/sync/sem.h>
#include <xwcd/ds/uart/general.h>

static __xwds_vop
xwer_t xwds_uartc_vop_probe(struct xwds_uartc * uartc);

static __xwds_vop
xwer_t xwds_uartc_vop_remove(struct xwds_uartc * uartc);

static __xwds_vop
xwer_t xwds_uartc_vop_start(struct xwds_uartc * uartc);

static __xwds_vop
xwer_t xwds_uartc_vop_stop(struct xwds_uartc * uartc);

#if defined(XWCDCFG_ds_PM) && (1 == XWCDCFG_ds_PM)
static __xwds_vop
xwer_t xwds_uartc_vop_suspend(struct xwds_uartc * uartc);

static __xwds_vop
xwer_t xwds_uartc_vop_resume(struct xwds_uartc * uartc);
#endif

static __xwds_code
xwer_t xwds_uartc_tx_1byte(struct xwds_uartc * uartc, const xwu8_t byte);

__xwds_rodata const struct xwds_virtual_operation xwds_uartc_vop = {
        .probe = (void *)xwds_uartc_vop_probe,
        .remove = (void *)xwds_uartc_vop_remove,
        .start = (void *)xwds_uartc_vop_start,
        .stop = (void *)xwds_uartc_vop_stop,
#if defined(XWCDCFG_ds_PM) && (1 == XWCDCFG_ds_PM)
        .suspend = (void *)xwds_uartc_vop_suspend,
        .resume = (void *)xwds_uartc_vop_resume,
#endif
};

/******** ******** ******** constructor & destructor ******** ******** ********/
/**
 * @brief XWDS API：UART控制器的构造函数
 * @param[in] uartc: UART控制器对象指针
 */
__xwds_api
void xwds_uartc_construct(struct xwds_uartc * uartc)
{
        xwds_device_construct(&uartc->dev);
        uartc->dev.vop = &xwds_uartc_vop;
}

/**
 * @brief XWDS API：UART控制器对象的析构函数
 * @param[in] uartc: UART控制器对象指针
 */
__xwds_api
void xwds_uartc_destruct(struct xwds_uartc * uartc)
{
        xwds_device_destruct(&uartc->dev);
}

/******** ******** base virtual operations ******** ********/
/**
 * @brief XWDS VOP：探测UART控制器
 * @param[in] uartc: UART控制器对象指针
 * @return 错误码
 */
static __xwds_vop
xwer_t xwds_uartc_vop_probe(struct xwds_uartc * uartc)
{
        xwer_t rc;

        /* init rx */
        xwos_sqlk_init(&uartc->rxseqlock);
        memset(uartc->rxq, 0, XWCDCFG_ds_UART_GNR_RXQ_SIZE);
        uartc->rxpos = 0;
        uartc->rxnum = 0;

        /* init tx */
        uartc->txport_available = 0;
        xwos_sqlk_init(&uartc->txseqlock);
        memset(uartc->txq, 0, XWCDCFG_ds_UART_GNR_TXQ_SIZE);
        uartc->txpos = 0;
        uartc->txnum = 0;

        /* create RX semaphore */
        rc = xwos_sem_init(&uartc->rxsem, 0, XWCDCFG_ds_UART_GNR_RXQ_SIZE);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_rxsem_init;
        }
        rc = xwos_sem_freeze(&uartc->rxsem);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_rxsem_freeze;
        }
        rc = xwos_mtx_init(&uartc->txmtx, XWOS_SKD_PRIORITY_RT_MIN);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_txmtx_init;
        }

        rc = xwds_device_vop_probe(&uartc->dev);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_dev_probe;
        }
        return XWOK;

err_dev_probe:
        xwos_mtx_fini(&uartc->txmtx);
err_txmtx_init:
err_rxsem_freeze:
        xwos_sem_fini(&uartc->rxsem);
err_rxsem_init:
        return rc;
}

/**
 * @brief XWDS VOP：移除UART控制器
 * @param[in] uartc: UART控制器对象指针
 * @return 错误码
 */
static __xwds_vop
xwer_t xwds_uartc_vop_remove(struct xwds_uartc * uartc)
{
        xwer_t rc;

        rc = xwds_device_vop_remove(&uartc->dev);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_dev_vop_remove;
        }
        xwos_mtx_fini(&uartc->txmtx);
        xwos_sem_fini(&uartc->rxsem);
        return XWOK;

err_dev_vop_remove:
        return rc;
}

/**
 * @brief XWDS VOP：启动UART控制器
 * @param[in] uartc: UART控制器对象指针
 * @return 错误码
 */
static __xwds_vop
xwer_t xwds_uartc_vop_start(struct xwds_uartc * uartc)
{
        const struct xwds_uartc_driver * drv;
        xwreg_t cpuirq;
        xwer_t rc;

        rc = xwds_device_vop_start(&uartc->dev);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_dev_start;
        }
        rc = xwos_sem_thaw(&uartc->rxsem);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_rxsem_thaw;
        }

        xwos_sqlk_wr_lock_cpuirqsv(&uartc->txseqlock, &cpuirq);
        if (uartc->txnum != uartc->txpos) {
                drv = xwds_cast(const struct xwds_uartc_driver *, uartc->dev.drv);
                rc = drv->tx(uartc, uartc->txq[uartc->txpos]);
                if (XWOK == rc) {
                        uartc->txpos++;
                        if (uartc->txpos >= (xwssq_t)XWCDCFG_ds_UART_GNR_TXQ_SIZE) {
                                uartc->txpos = 0;
                        }/* else {} */
                }/* else {} */
        } else {
                uartc->txport_available = 1;
        }
        xwos_sqlk_wr_unlock_cpuirqrs(&uartc->txseqlock, cpuirq);

        return XWOK;

err_rxsem_thaw:
        xwds_device_vop_stop(&uartc->dev);
err_dev_start:
        return rc;
}

/**
 * @brief XWDS VOP：停止UART控制器
 * @param[in] uartc: UART控制器对象指针
 * @return 错误码
 */
static __xwds_vop
xwer_t xwds_uartc_vop_stop(struct xwds_uartc * uartc)
{
        xwreg_t cpuirq;
        xwer_t rc;

        rc = xwos_sem_freeze(&uartc->rxsem);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_rxsem_freeze;
        }
        rc = xwds_device_vop_stop(&uartc->dev);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_dev_stop;
        }
        xwos_sqlk_wr_lock_cpuirqsv(&uartc->txseqlock, &cpuirq);
        uartc->txport_available = 0;
        xwos_sqlk_wr_unlock_cpuirqrs(&uartc->txseqlock, cpuirq);

        return XWOK;

err_dev_stop:
        xwos_sem_thaw(&uartc->rxsem);
err_rxsem_freeze:
        return rc;
}

#if defined(XWCDCFG_ds_PM) && (1 == XWCDCFG_ds_PM)
/******** ******** pm ******** ********/
/**
 * @brief XWDS VOP：暂停UART控制器
 * @param[in] uartc: UART控制器对象指针
 * @return 错误码
 */
static __xwds_vop
xwer_t xwds_uartc_vop_suspend(struct xwds_uartc * uartc)
{
        xwreg_t cpuirq;
        xwer_t rc;

        rc = xwds_device_vop_suspend(&uartc->dev);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_dev_suspend;
        }
        xwos_sqlk_wr_lock_cpuirqsv(&uartc->txseqlock, &cpuirq);
        uartc->txport_available = 0;
        xwos_sqlk_wr_unlock_cpuirqrs(&uartc->txseqlock, cpuirq);
        return XWOK;

err_dev_suspend:
        return rc;
}

/**
 * @brief XWDS VOP：继续UART控制器
 * @param[in] uartc: UART控制器对象指针
 * @return 错误码
 */
static __xwds_vop
xwer_t xwds_uartc_vop_resume(struct xwds_uartc * uartc)
{
        const struct xwds_uartc_driver * drv;
        xwreg_t cpuirq;
        xwer_t rc;

        rc = xwds_device_vop_resume(&uartc->dev);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_dev_resume;
        }
        xwos_sqlk_wr_lock_cpuirqsv(&uartc->txseqlock, &cpuirq);
        if (uartc->txnum != uartc->txpos) {
                drv = xwds_cast(const struct xwds_uartc_driver *, uartc->dev.drv);
                rc = drv->tx(uartc, uartc->txq[uartc->txpos]);
                if (XWOK == rc) {
                        uartc->txpos++;
                        if (uartc->txpos >= (xwssq_t)XWCDCFG_ds_UART_GNR_TXQ_SIZE) {
                                uartc->txpos = 0;
                        }/* else {} */
                }
        } else {
                uartc->txport_available = 1;
        }
        xwos_sqlk_wr_unlock_cpuirqrs(&uartc->txseqlock, cpuirq);
        return XWOK;

err_dev_resume:
        return rc;
}
#endif

/******** ******** UART APIs ******** ********/
/**
 * @brief XWDS API：清空接收队列
 * @param[in] uartc: UART控制器对象指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
__xwds_api
xwer_t xwds_uartc_clear_rxq(struct xwds_uartc * uartc)
{
        xwreg_t cpuirq;
        xwer_t rc;

        XWDS_VALIDATE(uartc, "nullptr", -EFAULT);

        rc = xwds_uartc_grab(uartc);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_uartc_grab;
        }
        rc = xwos_sem_freeze(&uartc->rxsem);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_sem_freeze;
        }
        xwos_sqlk_wr_lock_cpuirqsv(&uartc->rxseqlock, &cpuirq);
        uartc->rxnum = 0;
        uartc->rxpos = 0;
        xwos_sqlk_wr_unlock_cpuirqrs(&uartc->rxseqlock, cpuirq);
        rc = xwos_sem_thaw(&uartc->rxsem);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_sem_thaw;
        }
        xwds_uartc_put(uartc);
        return XWOK;

err_sem_thaw:
err_sem_freeze:
        xwds_uartc_put(uartc);
err_uartc_grab:
        return rc;
}

/**
 * @brief XWDS API：获取接收队列有效数据的大小
 * @param[in] uartc: UART控制器对象指针
 * @param[out] ret: 指向缓冲区的指针，通过此缓冲区返回接收队列中有效数据的大小
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
__xwds_api
xwer_t xwds_uartc_get_rxq_datasize(struct xwds_uartc * uartc, xwsz_t *ret)
{
        xwssz_t size;
        xwsq_t seq;
        xwer_t rc;

        XWDS_VALIDATE(uartc, "nullptr", -EFAULT);
        XWDS_VALIDATE(ret, "nullptr", -EFAULT);

        rc = xwds_uartc_grab(uartc);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_uartc_grab;
        }
        do {
                seq = xwos_sqlk_rd_begin(&uartc->rxseqlock);
                size = uartc->rxnum - uartc->rxpos;
                if (size < 0) {
                        size = (xwssz_t)XWCDCFG_ds_UART_GNR_RXQ_SIZE -
                               (xwssz_t)uartc->rxpos;
                        size += uartc->rxnum;
                }
        } while (xwos_sqlk_rd_retry(&uartc->rxseqlock, seq));
        xwds_uartc_put(uartc);
        *ret = (xwsz_t)size;
        return XWOK;

err_uartc_grab:
        return rc;
}

/**
 * @brief XWDS API：从接收队列中获取一个字节的数据
 * @param[in] uartc: UART控制器对象指针
 * @param[out] buf: 指向缓冲区的指针，此缓冲区被用于返回数据
 * @param[in,out] xwtm: 指向缓冲区的指针，此缓冲区：
 * + (I) 作为输入时，表示期望的阻塞等待时间
 * + (O) 作为输出时，返回剩余的期望时间
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效指针
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：可重入
 */
__xwds_api
xwer_t xwds_uartc_getc(struct xwds_uartc * uartc, xwu8_t * buf, xwtm_t * xwtm)
{
        xwer_t rc;
        xwreg_t cpuirq;

        XWDS_VALIDATE(uartc, "nullptr", -EFAULT);
        XWDS_VALIDATE(buf, "nullptr", -EFAULT);
        XWDS_VALIDATE(xwtm, "nullptr", -EFAULT);

        rc = xwds_uartc_grab(uartc);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_uartc_grab;
        }
        rc = xwos_sem_timedwait(&uartc->rxsem, xwtm);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_sem_timedwait;
        }
        xwos_sqlk_wr_lock_cpuirqsv(&uartc->rxseqlock, &cpuirq);
        *buf = uartc->rxq[uartc->rxpos];
        uartc->rxpos++;
        if (uartc->rxpos >= (xwssq_t)XWCDCFG_ds_UART_GNR_RXQ_SIZE) {
                uartc->rxpos = 0;
        }/* else {} */
        xwos_sqlk_wr_unlock_cpuirqrs(&uartc->rxseqlock, cpuirq);
        xwds_uartc_put(uartc);
        return XWOK;

err_sem_timedwait:
        xwds_uartc_put(uartc);
err_uartc_grab:
        return rc;
}

/**
 * @brief XWDS API：尝试从接收队列中获取一个字节的数据
 * @param[in] uartc: UART控制器对象指针
 * @param[out] buf: 指向缓冲区的指针，此缓冲区被用于返回数据
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
__xwds_api
xwer_t xwds_uartc_try_getc(struct xwds_uartc * uartc, xwu8_t * buf)
{
        xwer_t rc;
        xwreg_t cpuirq;

        XWDS_VALIDATE(uartc, "nullptr", -EFAULT);
        XWDS_VALIDATE(buf, "nullptr", -EFAULT);

        rc = xwds_uartc_grab(uartc);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_uartc_grab;
        }
        rc = xwos_sem_trywait(&uartc->rxsem);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_sem_trywait;
        }
        xwos_sqlk_wr_lock_cpuirqsv(&uartc->rxseqlock, &cpuirq);
        *buf = uartc->rxq[uartc->rxpos];
        uartc->rxpos++;
        if (uartc->rxpos >= (xwssq_t)XWCDCFG_ds_UART_GNR_RXQ_SIZE) {
                uartc->rxpos = 0;
        }/* else {} */
        xwos_sqlk_wr_unlock_cpuirqrs(&uartc->rxseqlock, cpuirq);
        xwds_uartc_put(uartc);
        return XWOK;

err_sem_trywait:
        xwds_uartc_put(uartc);
err_uartc_grab:
        return rc;
}

/**
 * @brief XWDS API：从接收队列中获取多个数据
 * @param[in] uartc: UART控制器对象指针
 * @param[out] buf: 指向缓冲区的指针，此缓冲区被用于返回数据
 * @param[in,out] size: 指向缓冲区的指针，此缓冲区：
 * + (I) 作为输入时，表示缓冲区大小（单位：字节）
 * + (O) 作为输出时，返回实际读取的数据大小
 * @param[in,out] xwtm: 指向缓冲区的指针，此缓冲区：
 * + (I) 作为输入时，表示期望的阻塞等待时间
 * + (O) 作为输出时，返回剩余的期望时间
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效指针
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
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

        XWDS_VALIDATE(uartc, "nullptr", -EFAULT);
        XWDS_VALIDATE(buf, "nullptr", -EFAULT);
        XWDS_VALIDATE(size, "nullptr", -EFAULT);
        XWDS_VALIDATE(xwtm, "nullptr", -EFAULT);

        rc = xwds_uartc_grab(uartc);
        if (__xwcc_unlikely(rc < 0)) {
                *size = 0;
                goto err_uartc_grab;
        }
        for (i = 0; i < *size; i++) {
                rc = xwos_sem_timedwait(&uartc->rxsem, xwtm);
                if (__xwcc_unlikely(rc < 0)) {
                        *size = i;
                        goto err_sem_timedwait;
                }
                xwos_sqlk_wr_lock_cpuirqsv(&uartc->rxseqlock, &cpuirq);
                ((xwu8_t *)buf)[i] = uartc->rxq[uartc->rxpos];
                uartc->rxpos++;
                if (uartc->rxpos >= (xwssq_t)XWCDCFG_ds_UART_GNR_RXQ_SIZE) {
                        uartc->rxpos = 0;
                }/* else {} */
                xwos_sqlk_wr_unlock_cpuirqrs(&uartc->rxseqlock, cpuirq);
        }
        xwds_uartc_put(uartc);
        *size = i;
        return XWOK;

err_sem_timedwait:
        xwds_uartc_put(uartc);
err_uartc_grab:
        return rc;
}

/**
 * @brief XWDS API：尝试从接收队列中获取多个字节的数据
 * @param[in] uartc: UART控制器对象指针
 * @param[out] buf: 指向缓冲区的指针，此缓冲区被用于返回数据
 * @param[in,out] size: 指向缓冲区的指针，此缓冲区：
 * + (I) 作为输入时，表示缓冲区大小（单位：字节）
 * + (O) 作为输出时，返回实际读取的数据大小
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
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
        if (__xwcc_unlikely(rc < 0)) {
                *size = 0;
                goto err_uartc_grab;
        }
        for (i = 0; i < *size; i++) {
                rc = xwos_sem_trywait(&uartc->rxsem);
                if (__xwcc_unlikely(rc < 0)) {
                        *size = i;
                        goto err_sem_trywait;
                }
                xwos_sqlk_wr_lock_cpuirqsv(&uartc->rxseqlock, &cpuirq);
                ((xwu8_t *)buf)[i] = uartc->rxq[uartc->rxpos];
                uartc->rxpos++;
                if (uartc->rxpos >= (xwssq_t)XWCDCFG_ds_UART_GNR_RXQ_SIZE) {
                        uartc->rxpos = 0;
                }/* else {} */
                xwos_sqlk_wr_unlock_cpuirqrs(&uartc->rxseqlock, cpuirq);
        }
        xwds_uartc_put(uartc);
        *size = i;
        return XWOK;

err_sem_trywait:
        xwds_uartc_put(uartc);
err_uartc_grab:
        return rc;
}

/**
 * @brief XWDS API：清空发送队列
 * @param[in] uartc: UART控制器对象指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
__xwds_api
xwer_t xwds_uartc_clear_txq(struct xwds_uartc * uartc)
{
        xwer_t rc = XWOK;
        xwreg_t cpuirq;

        XWDS_VALIDATE(uartc, "nullptr", -EFAULT);

        rc = xwds_uartc_grab(uartc);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_uartc_grab;
        }
        xwos_sqlk_wr_lock_cpuirqsv(&uartc->txseqlock, &cpuirq);
        uartc->txnum = 0;
        uartc->txpos = 0;
        xwos_sqlk_wr_unlock_cpuirqrs(&uartc->txseqlock, cpuirq);
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

        xwos_sqlk_wr_lock_cpuirqsv(&uartc->txseqlock, &cpuirq);
        if (1 == uartc->txport_available) {
                drv = xwds_cast(const struct xwds_uartc_driver *, uartc->dev.drv);
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
                if (uartc->txnum >= (xwssz_t)XWCDCFG_ds_UART_GNR_TXQ_SIZE) {
                        uartc->txnum = 0;
                }/* else {} */
                if (uartc->txnum == uartc->txpos) {
                        /* overflow: discard the newest one and stop tx */
                        uartc->txnum--;
                        if (uartc->txnum < 0) {
                                uartc->txnum = (xwssz_t)XWCDCFG_ds_UART_GNR_TXQ_SIZE -
                                               1;
                        }/* else {} */
                        rc = -EOVERFLOW;
                } else {
                        rc = XWOK;
                }
        }
        xwos_sqlk_wr_unlock_cpuirqrs(&uartc->txseqlock, cpuirq);
        return rc;
}

/**
 * @brief XWDS API：发送一个字节
 * @param[in] uartc: UART控制器对象指针
 * @param[in] byte: 待发送的数据
 * @param[in,out] xwtm: 指向缓冲区的指针，此缓冲区：
 * + (I) 作为输入时，表示期望的阻塞等待时间
 * + (O) 作为输出时，返回剩余的期望时间
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效指针
 * @retval -EOVERFLOW: 发送队列已满
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：可重入
 */
__xwds_api
xwer_t xwds_uartc_putc(struct xwds_uartc * uartc,
                       const xwu8_t byte,
                       xwtm_t * xwtm)
{
        xwer_t rc;

        XWDS_VALIDATE(uartc, "nullptr", -EFAULT);
        XWDS_VALIDATE(xwtm, "nullptr", -EFAULT);

        rc = xwds_uartc_grab(uartc);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_uartc_grab;
        }
        rc = xwos_mtx_timedlock(&uartc->txmtx, xwtm);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_uartc_lock;
        }
        rc = xwds_uartc_tx_1byte(uartc, byte);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_tx_1byte;
        }
        xwos_mtx_unlock(&uartc->txmtx);
        xwds_uartc_put(uartc);
        return XWOK;

err_tx_1byte:
        xwos_mtx_unlock(&uartc->txmtx);
err_uartc_lock:
        xwds_uartc_put(uartc);
err_uartc_grab:
        return rc;
}

/**
 * @brief XWDS API：发送一些数据
 * @param[in] uartc: UART控制器对象指针
 * @param[in] data: 待发送的数据的缓冲区
 * @param[in,out] size: 指向缓冲区的指针，此缓冲区：
 * + (I) 作为输入时，表示待发送的数据的大小
 * + (O) 作为输出时，返回实际放入发送队列的数据的大小
 * @param[in,out] xwtm: 指向缓冲区的指针，此缓冲区：
 * + (I) 作为输入时，表示期望的阻塞等待时间
 * + (O) 作为输出时，返回剩余的期望时间
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效指针
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：可重入
 */
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
        if (__xwcc_unlikely(rc < 0)) {
                *size = 0;
                goto err_uartc_grab;
        }
        rc = xwos_mtx_timedlock(&uartc->txmtx, xwtm);
        if (__xwcc_unlikely(rc < 0)) {
                *size = 0;
                goto err_uartc_lock;
        }
        rc = -EINVAL;
        for (i = 0; i < *size; i++) {
                rc = xwds_uartc_tx_1byte(uartc, data[i]);
                if (__xwcc_unlikely(rc < 0)) {
                        *size = i;
                        goto err_tx_1byte;
                }
        }
        xwos_mtx_unlock(&uartc->txmtx);
        xwds_uartc_put(uartc);
        *size = i;
        return XWOK;

err_tx_1byte:
        xwos_mtx_unlock(&uartc->txmtx);
err_uartc_lock:
        xwds_uartc_put(uartc);
err_uartc_grab:
        return rc;
}

/**
 * @brief XWDS API：配置UART
 * @param[in] uartc: UART控制器对象指针
 * @param[in] cfg: 新的配置
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
__xwds_api
xwer_t xwds_uartc_cfg(struct xwds_uartc * uartc,
                      const struct xwds_uart_cfg * cfg)
{
        const struct xwds_uartc_driver * drv;
        xwer_t rc;

        XWDS_VALIDATE(uartc, "nullptr", -EFAULT);
        XWDS_VALIDATE(cfg, "nullptr", -EFAULT);

        rc = xwds_uartc_grab(uartc);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_uartc_grab;
        }
        drv = xwds_cast(const struct xwds_uartc_driver *, uartc->dev.drv);
        if ((drv) && (drv->cfg)) {
                rc = drv->cfg(uartc, cfg);
        } else {
                rc = -ENOSYS;
        }
        if (__xwcc_unlikely(rc < 0)) {
                goto err_drv_cfg;
        }
        xwds_uartc_put(uartc);
        return XWOK;

err_drv_cfg:
        xwds_uartc_put(uartc);
err_uartc_grab:
        return rc;
}

/******** ******** Callbacks for BSP driver ******** ********/
/**
 * @brief XWDS API：UART接收中断响应函数
 * @param[in] uartc: UART控制器对象指针
 */
__xwds_api
void xwds_uartc_drvcb_rx_isr(struct xwds_uartc * uartc)
{
        const struct xwds_uartc_driver * drv;
        xwreg_t cpuirq;
        xwu8_t data;
        xwer_t rc;

        drv = xwds_cast(const struct xwds_uartc_driver *, uartc->dev.drv);
        rc = drv->rx(uartc, &data);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_drv_rx;
        }
        xwos_sqlk_wr_lock_cpuirqsv(&uartc->rxseqlock, &cpuirq);
        uartc->rxq[uartc->rxnum] = data;
        uartc->rxnum++;
        if (uartc->rxnum >= (xwssz_t)XWCDCFG_ds_UART_GNR_RXQ_SIZE) {
                uartc->rxnum = 0;
        }/* else {} */
        if (uartc->rxnum == uartc->rxpos) {
                /* overflow: discard the oldest data */
                uartc->rxpos++;
                if (uartc->rxpos >= (xwssq_t)XWCDCFG_ds_UART_GNR_RXQ_SIZE) {
                        uartc->rxpos = 0;
                }/* else {} */
                xwos_sqlk_wr_unlock_cpuirqrs(&uartc->rxseqlock, cpuirq);
        } else {
                xwos_sqlk_wr_unlock_cpuirqrs(&uartc->rxseqlock, cpuirq);
                xwos_sem_post(&uartc->rxsem);
        }

err_drv_rx:
        return;
}

/**
 * @brief XWDS API：UART发送中断响应函数
 * @param[in] uartc: UART控制器对象指针
 */
__xwds_api
void xwds_uartc_drvcb_tx_isr(struct xwds_uartc * uartc)
{
        const struct xwds_uartc_driver * drv;
        xwreg_t cpuirq;
        xwer_t rc;

        /* send a byte */
        xwos_sqlk_wr_lock_cpuirqsv(&uartc->txseqlock, &cpuirq);
        if (uartc->txnum != uartc->txpos) {
                drv = xwds_cast(const struct xwds_uartc_driver *, uartc->dev.drv);
                rc = drv->tx(uartc, uartc->txq[uartc->txpos]);
                if (XWOK == rc) {
                        uartc->txpos++;
                        if (uartc->txpos >= (xwssq_t)XWCDCFG_ds_UART_GNR_TXQ_SIZE) {
                                uartc->txpos = 0;
                        }/* else {} */
                } else {
                        uartc->txport_available = 1;
                }
        } else {
                uartc->txport_available = 1;
        }
        xwos_sqlk_wr_unlock_cpuirqrs(&uartc->txseqlock, cpuirq);
}
