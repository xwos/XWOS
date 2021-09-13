/**
 * @file
 * @brief 玄武设备栈：UART with DMA
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
 * @note
 * + 方案1：
 *   - 中断优先级：定时器中断的优先级 == DMA半完成与完成中断的优先级
 *   - 定时器中断函数与DMA半完成与完成中断函数运行在同一个CPU上
 * + 方案2：
 *   - 中断优先级：定时器中断的优先级 < DMA半完成与完成中断的优先级
 *   - 定时器中断中，获取DMA剩余计数前关闭DMA中断或总中断开关
 *   - 定时器中断函数与DMA半完成与完成中断函数运行在同一个CPU上
 */

#include <xwcd/ds/standard.h>
#include <string.h>
#include <xwos/osal/skd.h>
#include <xwos/osal/lock/spinlock.h>
#include <xwos/osal/lock/mtx.h>
#include <xwos/osal/sync/sem.h>
#include <xwcd/ds/uart/dma.h>

static __xwds_vop
xwer_t xwds_dmauartc_vop_probe(struct xwds_dmauartc * dmauartc);

static __xwds_vop
xwer_t xwds_dmauartc_vop_remove(struct xwds_dmauartc * dmauartc);

static __xwds_vop
xwer_t xwds_dmauartc_vop_start(struct xwds_dmauartc * dmauartc);

static __xwds_vop
xwer_t xwds_dmauartc_vop_stop(struct xwds_dmauartc * dmauartc);

#if defined(XWCDCFG_ds_PM) && (1 == XWCDCFG_ds_PM)
static __xwds_vop
xwer_t xwds_dmauartc_vop_suspend(struct xwds_dmauartc * dmauartc);

static __xwds_vop
xwer_t xwds_dmauartc_vop_resume(struct xwds_dmauartc * dmauartc);
#endif

__xwds_rodata const struct xwds_virtual_operation xwds_dmauartc_vop = {
        .probe = (void *)xwds_dmauartc_vop_probe,
        .remove = (void *)xwds_dmauartc_vop_remove,
        .start = (void *)xwds_dmauartc_vop_start,
        .stop = (void *)xwds_dmauartc_vop_stop,
#if defined(XWCDCFG_ds_PM) && (1 == XWCDCFG_ds_PM)
        .suspend = (void *)xwds_dmauartc_vop_suspend,
        .resume = (void *)xwds_dmauartc_vop_resume,
#endif
};

/******** ******** ******** constructor & destructor ******** ******** ********/
/**
 * @brief XWDS API：DMA UART控制器的构造函数
 * @param[in] dmauartc: DMA UART控制器对象指针
 */
__xwds_api
void xwds_dmauartc_construct(struct xwds_dmauartc * dmauartc)
{
        xwds_device_construct(&dmauartc->dev);
        dmauartc->dev.vop = &xwds_dmauartc_vop;
}

/**
 * @brief XWDS API：DMA UART控制器对象的析构函数
 * @param[in] dmauartc: DMA UART控制器对象指针
 */
__xwds_api
void xwds_dmauartc_destruct(struct xwds_dmauartc * dmauartc)
{
        xwds_device_destruct(&dmauartc->dev);
}

/******** ******** base virtual operations ******** ********/
/**
 * @brief XWDS VOP：探测DMA UART控制器
 * @param[in] dmauartc: DMA UART控制器对象指针
 * @return 错误码
 */
static __xwds_vop
xwer_t xwds_dmauartc_vop_probe(struct xwds_dmauartc * dmauartc)
{
        xwer_t rc;

        xwos_splk_init(&dmauartc->rxq.lock);
        rc = xwos_sem_init(&dmauartc->rxq.sem, 0, 1);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_sem_init;
        }
        rc = xwos_mtx_init(&dmauartc->txmtx, XWOS_SKD_PRIORITY_RT_MIN);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_txmtx_init;
        }
        rc = xwds_device_vop_probe(&dmauartc->dev);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_dev_probe;
        }
        return XWOK;

err_dev_probe:
        xwos_mtx_fini(&dmauartc->txmtx);
err_txmtx_init:
        xwos_sem_fini(&dmauartc->rxq.sem);
err_sem_init:
        return rc;
}

/**
 * @brief XWDS VOP：移除DMA UART控制器
 * @param[in] dmauartc: DMA UART控制器对象指针
 * @return 错误码
 */
static __xwds_vop
xwer_t xwds_dmauartc_vop_remove(struct xwds_dmauartc * dmauartc)
{
        xwer_t rc;

        rc = xwds_device_vop_remove(&dmauartc->dev);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_dev_vop_remove;
        }

        xwos_mtx_fini(&dmauartc->txmtx);
        xwos_sem_fini(&dmauartc->rxq.sem);
        return XWOK;

err_dev_vop_remove:
        return rc;
}

/**
 * @brief XWDS VOP：启动DMA UART控制器
 * @param[in] dmauartc: DMA UART控制器对象指针
 * @return 错误码
 */
static __xwds_vop
xwer_t xwds_dmauartc_vop_start(struct xwds_dmauartc * dmauartc)
{
        xwer_t rc;

        memset(dmauartc->rxq.mem, 0, sizeof(dmauartc->rxq.mem));
        dmauartc->rxq.pos = 0;
        dmauartc->rxq.tail = 0;

        rc = xwds_device_vop_start(&dmauartc->dev);
        return rc;
}

/**
 * @brief XWDS VOP：停止DMA UART控制器
 * @param[in] dmauartc: DMA UART控制器对象指针
 * @return 错误码
 */
static __xwds_vop
xwer_t xwds_dmauartc_vop_stop(struct xwds_dmauartc * dmauartc)
{
        xwer_t rc;

        rc = xwds_device_vop_stop(&dmauartc->dev);
        return rc;
}

#if (defined(XWCDCFG_ds_PM)) && (1 == XWCDCFG_ds_PM)
/******** ******** pm ******** ********/
/**
 * @brief XWDS VOP：暂停DMA UART控制器
 * @param[in] dmauartc: DMA UART控制器对象指针
 * @return 错误码
 */
static __xwds_vop
xwer_t xwds_dmauartc_vop_suspend(struct xwds_dmauartc * dmauartc)
{
        xwer_t rc;

        rc = xwds_device_vop_suspend(&dmauartc->dev);
        return rc;
}

/**
 * @brief XWDS VOP：继续DMA UART控制器
 * @param[in] dmauartc: DMA UART控制器对象指针
 * @return 错误码
 */
static __xwds_vop
xwer_t xwds_dmauartc_vop_resume(struct xwds_dmauartc * dmauartc)
{
        xwer_t rc;

        memset(dmauartc->rxq.mem, 0, sizeof(dmauartc->rxq.mem));
        dmauartc->rxq.pos = 0;
        dmauartc->rxq.tail = 0;
        rc = xwds_device_vop_resume(&dmauartc->dev);
        return rc;
}
#endif

/******** ******** ******** DMA UART APIs ******** ******** ********/
/**
 * @brief XWDS API：从接收队列中获取数据
 * @param[in] dmauartc: DMA UART控制器对象指针
 * @param[out] buf: 指向缓冲区的指针，通过此缓冲区返回数据
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
xwer_t xwds_dmauartc_rx(struct xwds_dmauartc * dmauartc,
                        xwu8_t * buf, xwsz_t * size,
                        xwtm_t * xwtm)
{
        xwsz_t available, real, cp, rest_buffer_size;
        xwsq_t pos;
        xwreg_t cpuirq;
        xwer_t rc;

        XWDS_VALIDATE(dmauartc, "nullptr", -EFAULT);
        XWDS_VALIDATE(buf, "nullptr", -EFAULT);
        XWDS_VALIDATE(size, "nullptr", -EFAULT);
        XWDS_VALIDATE(xwtm, "nullptr", -EFAULT);

        pos = 0;
        rc = xwds_dmauartc_grab(dmauartc);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_dmauartc_grab;
        }
        rest_buffer_size = *size;
        while (rest_buffer_size) {
                rc = xwos_sem_timedwait(&dmauartc->rxq.sem, xwtm);
                if (__xwcc_unlikely(rc < 0)) {
                        goto err_sem_timedwait;
                }
                xwos_splk_lock_cpuirqsv(&dmauartc->rxq.lock, &cpuirq);
                if (dmauartc->rxq.tail >= dmauartc->rxq.pos) {
                        available = dmauartc->rxq.tail - dmauartc->rxq.pos;
                } else {
                        available = (2 * XWDS_DMAUART_RXQ_SIZE) - dmauartc->rxq.pos;
                        available += dmauartc->rxq.tail;
                }
                real = available > rest_buffer_size ? rest_buffer_size : available;
                if ((real + dmauartc->rxq.pos) >= (2 * XWDS_DMAUART_RXQ_SIZE)) {
                        cp = (2 * XWDS_DMAUART_RXQ_SIZE) - dmauartc->rxq.pos;
                        memcpy(&buf[pos], &dmauartc->rxq.mem[dmauartc->rxq.pos], cp);
                        memcpy(&buf[cp + pos], &dmauartc->rxq.mem[0], real - cp);
                        dmauartc->rxq.pos = real - cp;
                } else {
                        memcpy(&buf[pos], &dmauartc->rxq.mem[dmauartc->rxq.pos], real);
                        dmauartc->rxq.pos += real;
                }
                xwos_splk_unlock_cpuirqrs(&dmauartc->rxq.lock, cpuirq);
                pos += real;
                rest_buffer_size -= real;
                if (available > real) {
                        xwos_sem_post(&dmauartc->rxq.sem);
                }
        }
        xwds_dmauartc_put(dmauartc);
        *size = pos;
        return XWOK;

err_sem_timedwait:
        xwds_dmauartc_put(dmauartc);
err_dmauartc_grab:
        *size = pos;
        return rc;
}

/**
 * @brief XWDS API：尝试从接收队列中获取数据
 * @param[in] dmauartc: DMA UART控制器对象指针
 * @param[out] buf: 指向缓冲区的指针，通过此缓冲区返回数据
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
xwer_t xwds_dmauartc_try_rx(struct xwds_dmauartc * dmauartc,
                            xwu8_t * buf, xwsz_t * size)
{
        xwsz_t available, real, cp, rest_buffer_size;
        xwsq_t pos;
        xwreg_t cpuirq;
        xwer_t rc;

        XWDS_VALIDATE(dmauartc, "nullptr", -EFAULT);
        XWDS_VALIDATE(buf, "nullptr", -EFAULT);
        XWDS_VALIDATE(size, "nullptr", -EFAULT);

        pos = 0;
        rc = xwds_dmauartc_grab(dmauartc);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_dmauartc_grab;
        }
        rest_buffer_size = *size;
        while (rest_buffer_size) {
                rc = xwos_sem_trywait(&dmauartc->rxq.sem);
                if (__xwcc_unlikely(rc < 0)) {
                        goto err_sem_trywait;
                }
                xwos_splk_lock_cpuirqsv(&dmauartc->rxq.lock, &cpuirq);
                if (dmauartc->rxq.tail >= dmauartc->rxq.pos) {
                        available = dmauartc->rxq.tail - dmauartc->rxq.pos;
                } else {
                        available = (2 * XWDS_DMAUART_RXQ_SIZE) - dmauartc->rxq.pos;
                        available += dmauartc->rxq.tail;
                }
                real = available > rest_buffer_size ? rest_buffer_size : available;
                if ((real + dmauartc->rxq.pos) >= (2 * XWDS_DMAUART_RXQ_SIZE)) {
                        cp = (2 * XWDS_DMAUART_RXQ_SIZE) - dmauartc->rxq.pos;
                        memcpy(&buf[pos], &dmauartc->rxq.mem[dmauartc->rxq.pos], cp);
                        memcpy(&buf[cp + pos], &dmauartc->rxq.mem[0], real - cp);
                        dmauartc->rxq.pos = real - cp;
                } else {
                        memcpy(&buf[pos], &dmauartc->rxq.mem[dmauartc->rxq.pos], real);
                        dmauartc->rxq.pos += real;
                }
                xwos_splk_unlock_cpuirqrs(&dmauartc->rxq.lock, cpuirq);
                pos += real;
                rest_buffer_size -= real;
                if (available > real) {
                        xwos_sem_post(&dmauartc->rxq.sem);
                }/* else {} */
        }
        xwds_dmauartc_put(dmauartc);
        *size = pos;
        return XWOK;

err_sem_trywait:
        xwds_dmauartc_put(dmauartc);
err_dmauartc_grab:
        *size = pos;
        return rc;
}

/**
 * @brief XWDS API：配置UART的DMA通道发送数据
 * @param[in] dmauartc: DMA UART控制器对象指针
 * @param[in] data: 待发送的数据的缓冲区
 * @param[in,out] size: 指向缓冲区的指针，此缓冲区：
 * + (I) 作为输入时，表示期望发送的数据的大小（单位：字节）
 * + (O) 作为输出时，返回实际发送的数据大小
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
xwer_t xwds_dmauartc_tx(struct xwds_dmauartc * dmauartc,
                        const xwu8_t * data, xwsz_t * size,
                        xwtm_t * xwtm)
{
        xwer_t rc;
        const struct xwds_dmauartc_driver * drv;

        XWDS_VALIDATE(dmauartc, "nullptr", -EFAULT);
        XWDS_VALIDATE(data, "nullptr", -EFAULT);
        XWDS_VALIDATE(size, "nullptr", -EFAULT);

        rc = xwds_dmauartc_grab(dmauartc);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_dmauartc_grab;
        }
        rc = xwos_mtx_timedlock(&dmauartc->txmtx, xwtm);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_dmauartc_lock;
        }
        drv = xwds_cast(const struct xwds_dmauartc_driver *, dmauartc->dev.drv);
        if ((drv) && (drv->tx)) {
                rc = drv->tx(dmauartc, data, size, xwtm);
        } else {
                rc = -ENOSYS;
        }
        if (__xwcc_unlikely(rc < 0)) {
                goto err_tx;
        }
        xwos_mtx_unlock(&dmauartc->txmtx);
        xwds_dmauartc_put(dmauartc);
        return XWOK;

err_tx:
        xwos_mtx_unlock(&dmauartc->txmtx);
err_dmauartc_lock:
        xwds_dmauartc_put(dmauartc);
err_dmauartc_grab:
        return rc;
}

/**
 * @brief XWDS API：直接发送一个字节（非DMA模式）
 * @param[in] dmauartc: DMA UART控制器对象指针
 * @param[in] byte: 待发送的字节
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
xwer_t xwds_dmauartc_putc(struct xwds_dmauartc * dmauartc,
                          const xwu8_t byte,
                          xwtm_t * xwtm)
{
        xwer_t rc;
        const struct xwds_dmauartc_driver * drv;

        XWDS_VALIDATE(dmauartc, "nullptr", -EFAULT);
        XWDS_VALIDATE(xwtm, "nullptr", -EFAULT);

        rc = xwds_dmauartc_grab(dmauartc);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_dmauartc_grab;
        }
        rc = xwos_mtx_timedlock(&dmauartc->txmtx, xwtm);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_dmauartc_lock;
        }
        drv = xwds_cast(const struct xwds_dmauartc_driver *, dmauartc->dev.drv);
        if ((drv) && (drv->putc)) {
                rc = drv->putc(dmauartc, byte);
        } else {
                rc = -ENOSYS;
        }
        if (__xwcc_unlikely(rc < 0)) {
                goto err_putc;
        }
        xwos_mtx_unlock(&dmauartc->txmtx);
        xwds_dmauartc_put(dmauartc);
        return XWOK;

err_putc:
        xwos_mtx_unlock(&dmauartc->txmtx);
err_dmauartc_lock:
        xwds_dmauartc_put(dmauartc);
err_dmauartc_grab:
        return rc;
}

/**
 * @brief XWDS API：配置UART
 * @param[in] dmauartc: DMA UART控制器对象指针
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
xwer_t xwds_dmauartc_cfg(struct xwds_dmauartc * dmauartc,
                         const struct xwds_uart_cfg * cfg)
{
        const struct xwds_dmauartc_driver * drv;
        xwer_t rc;

        XWDS_VALIDATE(dmauartc, "nullptr", -EFAULT);
        XWDS_VALIDATE(cfg, "nullptr", -EFAULT);

        rc = xwds_dmauartc_grab(dmauartc);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_dmauartc_grab;
        }
        drv = xwds_cast(const struct xwds_dmauartc_driver *, dmauartc->dev.drv);
        if ((drv) && (drv->cfg)) {
                rc = drv->cfg(dmauartc, cfg);
        } else {
                rc = -ENOSYS;
        }
        if (__xwcc_unlikely(rc < 0)) {
                goto err_drv_cfg;
        }
        xwds_dmauartc_put(dmauartc);
        return XWOK;

err_drv_cfg:
        xwds_dmauartc_put(dmauartc);
err_dmauartc_grab:
        return rc;
}

/******** ******** Callbacks for driver ******** ********/
/**
 * @brief XWDS Driver Callback：清空接收队列
 * @param[in] dmauartc: DMA UART控制器对象指针
 */
__xwds_api
void xwds_dmauartc_drvcb_rxq_flush(struct xwds_dmauartc * dmauartc)
{
        xwreg_t cpuirq;

        xwos_sem_trywait(&dmauartc->rxq.sem);
        xwos_splk_lock_cpuirqsv(&dmauartc->rxq.lock, &cpuirq);
        dmauartc->rxq.pos = 0;
        dmauartc->rxq.tail = 0;
        xwos_splk_unlock_cpuirqrs(&dmauartc->rxq.lock, cpuirq);
}

/**
 * @brief XWDS Driver Callback：发布数据到接收队列
 * @param[in] dmauartc: DMA UART控制器对象指针
 * @param[in] tail: 新的数据接收位置（有效数据结尾 + 1）
 */
__xwds_code
void xwds_dmauartc_drvcb_rxq_pub(struct xwds_dmauartc * dmauartc, xwsq_t tail)
{
        xwsz_t pubsz;
        xwreg_t cpuirq;

        pubsz = 0;
        xwos_splk_lock_cpuirqsv(&dmauartc->rxq.lock, &cpuirq);
        if (tail == dmauartc->rxq.tail) {
                pubsz = 0;
        } else if (0 == dmauartc->rxq.tail) {
                if (tail > XWDS_DMAUART_RXQ_SIZE) {
                        /* Low buffer is overflow. Discard the oldest data. */
                        pubsz = tail - XWDS_DMAUART_RXQ_SIZE;
                        dmauartc->rxq.tail = tail;
                        dmauartc->rxq.pos = XWDS_DMAUART_RXQ_SIZE;
                } else {
                        if (0 == dmauartc->rxq.pos) {
                                pubsz = tail;
                                dmauartc->rxq.tail = tail;
                        } else {
                                XWDS_BUG_ON(dmauartc->rxq.pos < XWDS_DMAUART_RXQ_SIZE);
                                if (XWDS_DMAUART_RXQ_SIZE == tail) {
                                        /* Low buffer is overflow.
                                           Discard the oldest data */
                                        pubsz = dmauartc->rxq.pos - tail;
                                        dmauartc->rxq.tail = tail;
                                        dmauartc->rxq.pos = 0;
                                }/* else {} */
                        }
                }
        } else if (dmauartc->rxq.tail < XWDS_DMAUART_RXQ_SIZE) {
                XWDS_BUG_ON(dmauartc->rxq.pos > dmauartc->rxq.tail);
                if (tail > XWDS_DMAUART_RXQ_SIZE) {
                        /* Low buffer is overflow. Discard the oldest data. */
                        dmauartc->rxq.tail = tail;
                        dmauartc->rxq.pos = XWDS_DMAUART_RXQ_SIZE;
                        pubsz = tail - XWDS_DMAUART_RXQ_SIZE;
                } else if (tail < dmauartc->rxq.tail) {
                        XWDS_BUG();
                } else {
                        pubsz = tail - dmauartc->rxq.tail;
                        dmauartc->rxq.tail = tail;
                }
        } else if (XWDS_DMAUART_RXQ_SIZE == dmauartc->rxq.tail) {
                if ((tail < XWDS_DMAUART_RXQ_SIZE) && (0 != tail)) {
                        /* High buffer is overflow. Discard the oldest data. */
                        pubsz = tail;
                        dmauartc->rxq.tail = tail;
                        dmauartc->rxq.pos = 0;
                } else {
                        if (XWDS_DMAUART_RXQ_SIZE == dmauartc->rxq.pos) {
                                if (0 == tail) {
                                        pubsz = XWDS_DMAUART_RXQ_SIZE;
                                } else {
                                        pubsz = tail - XWDS_DMAUART_RXQ_SIZE;
                                }
                                dmauartc->rxq.tail = tail;
                        } else {
                                XWDS_BUG_ON(dmauartc->rxq.pos > XWDS_DMAUART_RXQ_SIZE);
                                if (0 == tail) {
                                        /* High buffer is overflow.
                                           Discard the oldest data */
                                        pubsz = dmauartc->rxq.pos;
                                        dmauartc->rxq.tail = 0;
                                        dmauartc->rxq.pos = XWDS_DMAUART_RXQ_SIZE;
                                }/* else {} */
                        }
                }
        } else {
                XWDS_BUG_ON(dmauartc->rxq.pos < XWDS_DMAUART_RXQ_SIZE);
                XWDS_BUG_ON(dmauartc->rxq.pos > dmauartc->rxq.tail);
                if ((tail < dmauartc->rxq.tail) && (0 != tail)) {
                        /* High buffer is overflow. Discard the oldest data. */
                        if (tail <= XWDS_DMAUART_RXQ_SIZE) {
                                pubsz = tail;
                                dmauartc->rxq.tail = tail;
                                dmauartc->rxq.pos = 0;
                        } else {
                                XWDS_BUG();
                        }
                } else {
                        if (0 == tail) {
                                pubsz = (2 * XWDS_DMAUART_RXQ_SIZE) -
                                        dmauartc->rxq.tail;
                        } else {
                                pubsz = tail - dmauartc->rxq.tail;
                        }
                        dmauartc->rxq.tail = tail;
                }
        }
        xwos_splk_unlock_cpuirqrs(&dmauartc->rxq.lock, cpuirq);
        if (pubsz > 0) {
                xwos_sem_post(&dmauartc->rxq.sem);
        }/* else {} */
}
