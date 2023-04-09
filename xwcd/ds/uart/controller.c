/**
 * @file
 * @brief 玄武设备栈：UART控制器
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
 * @note
 * + 方案1：
 *   - 中断优先级：定时器中断的优先级 == DMA半完成与完成中断的优先级
 *   - 定时器中断函数与DMA半完成/完成中断函数运行在同一个CPU上
 * + 方案2：
 *   - 中断优先级：定时器中断的优先级 < DMA半完成与完成中断的优先级
 *   - 定时器中断中，获取DMA剩余计数前关闭DMA中断或总中断开关
 *   - 定时器中断函数与DMA半完成/完成中断函数运行在同一个CPU上
 */

#include <xwcd/ds/standard.h>
#include <string.h>
#include <xwos/osal/lock/spinlock.h>
#include <xwos/osal/lock/mtx.h>
#include <xwos/osal/sync/sem.h>
#include <xwcd/ds/uart/controller.h>

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

/**
 * @brief 增加对象的引用计数
 * @param[in] uartc: UART控制器对象指针
 */
__xwds_api
xwer_t xwds_uartc_grab(struct xwds_uartc * uartc)
{
        return xwds_device_grab(&uartc->dev);
}

/**
 * @brief 减少对象的引用计数
 * @param[in] uartc: UART控制器对象指针
 */
__xwds_api
xwer_t xwds_uartc_put(struct xwds_uartc * uartc)
{
        return xwds_device_put(&uartc->dev);
}

/******** ******** base virtual operations ******** ********/
/**
 * @brief XWDS VOP：探测UART控制器
 * @param[in] uartc: UART控制器对象指针
 */
static __xwds_vop
xwer_t xwds_uartc_vop_probe(struct xwds_uartc * uartc)
{
        xwer_t rc;

        xwos_splk_init(&uartc->rxq.lock);
        rc = xwos_sem_init(&uartc->rxq.sem, 0, 1);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_sem_init;
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
        xwos_sem_fini(&uartc->rxq.sem);
err_sem_init:
        return rc;
}

/**
 * @brief XWDS VOP：移除UART控制器
 * @param[in] uartc: UART控制器对象指针
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
        xwos_sem_fini(&uartc->rxq.sem);
        return XWOK;

err_dev_vop_remove:
        return rc;
}

/**
 * @brief XWDS VOP：启动UART控制器
 * @param[in] uartc: UART控制器对象指针
 */
static __xwds_vop
xwer_t xwds_uartc_vop_start(struct xwds_uartc * uartc)
{
        xwer_t rc;

        memset(uartc->rxq.mem, 0, sizeof(uartc->rxq.mem));
        uartc->rxq.pos = 0;
        uartc->rxq.tail = 0;

        rc = xwds_device_vop_start(&uartc->dev);
        return rc;
}

/**
 * @brief XWDS VOP：停止UART控制器
 * @param[in] uartc: UART控制器对象指针
 */
static __xwds_vop
xwer_t xwds_uartc_vop_stop(struct xwds_uartc * uartc)
{
        xwer_t rc;

        rc = xwds_device_vop_stop(&uartc->dev);
        return rc;
}

#if (defined(XWCDCFG_ds_PM)) && (1 == XWCDCFG_ds_PM)
/******** ******** pm ******** ********/
/**
 * @brief XWDS VOP：暂停UART控制器
 * @param[in] uartc: UART控制器对象指针
 */
static __xwds_vop
xwer_t xwds_uartc_vop_suspend(struct xwds_uartc * uartc)
{
        xwer_t rc;

        rc = xwds_device_vop_suspend(&uartc->dev);
        return rc;
}

/**
 * @brief XWDS VOP：继续UART控制器
 * @param[in] uartc: UART控制器对象指针
 */
static __xwds_vop
xwer_t xwds_uartc_vop_resume(struct xwds_uartc * uartc)
{
        xwer_t rc;

        memset(uartc->rxq.mem, 0, sizeof(uartc->rxq.mem));
        uartc->rxq.pos = 0;
        uartc->rxq.tail = 0;
        rc = xwds_device_vop_resume(&uartc->dev);
        return rc;
}
#endif

/******** ******** ******** UART APIs ******** ******** ********/
/**
 * @brief XWDS API：从接收队列中获取数据
 * @param[in] uartc: UART控制器对象指针
 * @param[out] buf: 指向缓冲区的指针，通过此缓冲区返回数据
 * @param[in,out] size: 指向缓冲区的指针，此缓冲区：
 * + (I) 作为输入时，表示缓冲区大小（单位：字节）
 * + (O) 作为输出时，返回实际读取的数据大小
 * @param[in] to: 期望唤醒的时间点
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效指针
 * @retval -ETIMEDOUT: 超时
 * @note
 * + 上下文：线程
 * @details
 * 如果 `to` 是过去的时间点，将直接返回 `-ETIMEDOUT` 。
 */
__xwds_api
xwer_t xwds_uartc_rx(struct xwds_uartc * uartc,
                     xwu8_t * buf, xwsz_t * size,
                     xwtm_t to)
{
        xwsz_t available, real, cp, rest_buffer_size;
        xwsq_t pos;
        xwreg_t cpuirq;
        xwer_t rc;

        XWDS_VALIDATE(uartc, "nullptr", -EFAULT);
        XWDS_VALIDATE(buf, "nullptr", -EFAULT);
        XWDS_VALIDATE(size, "nullptr", -EFAULT);

        pos = 0;
        rc = xwds_uartc_grab(uartc);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_uartc_grab;
        }
        rest_buffer_size = *size;
        while (rest_buffer_size) {
                rc = xwos_sem_wait_to(&uartc->rxq.sem, to);
                if (__xwcc_unlikely(rc < 0)) {
                        goto err_sem_wait_to;
                }
                xwos_splk_lock_cpuirqsv(&uartc->rxq.lock, &cpuirq);
                if (uartc->rxq.tail >= uartc->rxq.pos) {
                        available = uartc->rxq.tail - uartc->rxq.pos;
                } else {
                        available = (2 * XWDS_UART_RXQ_SIZE) - uartc->rxq.pos;
                        available += uartc->rxq.tail;
                }
                real = available > rest_buffer_size ? rest_buffer_size : available;
                if ((real + uartc->rxq.pos) >= (2 * XWDS_UART_RXQ_SIZE)) {
                        cp = (2 * XWDS_UART_RXQ_SIZE) - uartc->rxq.pos;
                        memcpy(&buf[pos], &uartc->rxq.mem[uartc->rxq.pos], cp);
                        memcpy(&buf[cp + pos], &uartc->rxq.mem[0], real - cp);
                        uartc->rxq.pos = real - cp;
                } else {
                        memcpy(&buf[pos], &uartc->rxq.mem[uartc->rxq.pos], real);
                        uartc->rxq.pos += real;
                }
                xwos_splk_unlock_cpuirqrs(&uartc->rxq.lock, cpuirq);
                pos += real;
                rest_buffer_size -= real;
                if (available > real) {
                        xwos_sem_post(&uartc->rxq.sem);
                }
        }
        xwds_uartc_put(uartc);
        *size = pos;
        return XWOK;

err_sem_wait_to:
        xwds_uartc_put(uartc);
err_uartc_grab:
        *size = pos;
        return rc;
}

/**
 * @brief XWDS API：尝试从接收队列中获取数据
 * @param[in] uartc: UART控制器对象指针
 * @param[out] buf: 指向缓冲区的指针，通过此缓冲区返回数据
 * @param[in,out] size: 指向缓冲区的指针，此缓冲区：
 * + (I) 作为输入时，表示缓冲区大小（单位：字节）
 * + (O) 作为输出时，返回实际读取的数据大小
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效指针
 * @retval -ENODATA: 没有数据
 * @note
 * + 上下文：中断、中断底半部、线程
 */
__xwds_api
xwer_t xwds_uartc_try_rx(struct xwds_uartc * uartc,
                         xwu8_t * buf, xwsz_t * size)
{
        xwsz_t available, real, cp, rest_buffer_size;
        xwsq_t pos;
        xwreg_t cpuirq;
        xwer_t rc;

        XWDS_VALIDATE(uartc, "nullptr", -EFAULT);
        XWDS_VALIDATE(buf, "nullptr", -EFAULT);
        XWDS_VALIDATE(size, "nullptr", -EFAULT);

        pos = 0;
        rc = xwds_uartc_grab(uartc);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_uartc_grab;
        }
        rest_buffer_size = *size;
        while (rest_buffer_size) {
                rc = xwos_sem_trywait(&uartc->rxq.sem);
                if (__xwcc_unlikely(rc < 0)) {
                        goto err_sem_trywait;
                }
                xwos_splk_lock_cpuirqsv(&uartc->rxq.lock, &cpuirq);
                if (uartc->rxq.tail >= uartc->rxq.pos) {
                        available = uartc->rxq.tail - uartc->rxq.pos;
                } else {
                        available = (2 * XWDS_UART_RXQ_SIZE) - uartc->rxq.pos;
                        available += uartc->rxq.tail;
                }
                real = available > rest_buffer_size ? rest_buffer_size : available;
                if ((real + uartc->rxq.pos) >= (2 * XWDS_UART_RXQ_SIZE)) {
                        cp = (2 * XWDS_UART_RXQ_SIZE) - uartc->rxq.pos;
                        memcpy(&buf[pos], &uartc->rxq.mem[uartc->rxq.pos], cp);
                        memcpy(&buf[cp + pos], &uartc->rxq.mem[0], real - cp);
                        uartc->rxq.pos = real - cp;
                } else {
                        memcpy(&buf[pos], &uartc->rxq.mem[uartc->rxq.pos], real);
                        uartc->rxq.pos += real;
                }
                xwos_splk_unlock_cpuirqrs(&uartc->rxq.lock, cpuirq);
                pos += real;
                rest_buffer_size -= real;
                if (available > real) {
                        xwos_sem_post(&uartc->rxq.sem);
                }/* else {} */
        }
        xwds_uartc_put(uartc);
        *size = pos;
        return XWOK;

err_sem_trywait:
        xwds_uartc_put(uartc);
err_uartc_grab:
        *size = pos;
        return rc;
}

/**
 * @brief XWDS API：配置UART的DMA通道发送数据
 * @param[in] uartc: UART控制器对象指针
 * @param[in] data: 待发送的数据的缓冲区
 * @param[in,out] size: 指向缓冲区的指针，此缓冲区：
 * + (I) 作为输入时，表示期望发送的数据的大小（单位：字节）
 * + (O) 作为输出时，返回实际发送的数据大小
 * @param[in] to: 期望唤醒的时间点
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效指针
 * @retval -ENOSYS: 不支持此操作
 * @retval -ECANCELED: 发送被取消
 * @retval -ETIMEDOUT: 超时
 * @note
 * + 上下文：线程
 * @details
 * 如果 `to` 是过去的时间点，将直接返回 `-ETIMEDOUT` 。
 */
__xwds_api
xwer_t xwds_uartc_tx(struct xwds_uartc * uartc,
                     const xwu8_t * data, xwsz_t * size,
                     xwtm_t to)
{
        const struct xwds_uartc_driver * drv;
        xwer_t rc;

        XWDS_VALIDATE(uartc, "nullptr", -EFAULT);
        XWDS_VALIDATE(data, "nullptr", -EFAULT);
        XWDS_VALIDATE(size, "nullptr", -EFAULT);

        rc = xwds_uartc_grab(uartc);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_uartc_grab;
        }
        rc = xwos_mtx_lock_to(&uartc->txmtx, to);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_uartc_lock;
        }
        drv = xwds_cast(const struct xwds_uartc_driver *, uartc->dev.drv);
        if ((drv) && (drv->tx)) {
                rc = drv->tx(uartc, data, size, to);
        } else {
                rc = -ENOSYS;
        }
        if (__xwcc_unlikely(rc < 0)) {
                goto err_tx;
        }
        xwos_mtx_unlock(&uartc->txmtx);
        xwds_uartc_put(uartc);
        return XWOK;

err_tx:
        xwos_mtx_unlock(&uartc->txmtx);
err_uartc_lock:
        xwds_uartc_put(uartc);
err_uartc_grab:
        return rc;
}

/**
 * @brief XWDS API：直接发送一个字节（非DMA模式）
 * @param[in] uartc: UART控制器对象指针
 * @param[in] byte: 待发送的字节
 * @param[in] to: 期望唤醒的时间点
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效指针
 * @retval -ENOSYS: 不支持此操作
 * @note
 * + 上下文：线程
 * @details
 * 如果 `to` 是过去的时间点，将直接返回 `-ETIMEDOUT` 。
 */
__xwds_api
xwer_t xwds_uartc_putc(struct xwds_uartc * uartc,
                       const xwu8_t byte,
                       xwtm_t to)
{
        xwer_t rc;
        const struct xwds_uartc_driver * drv;

        XWDS_VALIDATE(uartc, "nullptr", -EFAULT);

        rc = xwds_uartc_grab(uartc);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_uartc_grab;
        }
        rc = xwos_mtx_lock_to(&uartc->txmtx, to);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_uartc_lock;
        }
        drv = xwds_cast(const struct xwds_uartc_driver *, uartc->dev.drv);
        if ((drv) && (drv->putc)) {
                rc = drv->putc(uartc, byte);
        } else {
                rc = -ENOSYS;
        }
        if (__xwcc_unlikely(rc < 0)) {
                goto err_putc;
        }
        xwos_mtx_unlock(&uartc->txmtx);
        xwds_uartc_put(uartc);
        return XWOK;

err_putc:
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
 * @retval -ENOSYS: 不支持此操作
 * @note
 * + 上下文：中断、中断底半部、线程
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

/******** ******** Callbacks for driver ******** ********/
/**
 * @brief XWDS Driver Callback：清空接收队列
 * @param[in] uartc: UART控制器对象指针
 */
__xwds_api
void xwds_uartc_drvcb_rxq_flush(struct xwds_uartc * uartc)
{
        xwreg_t cpuirq;

        xwos_sem_trywait(&uartc->rxq.sem);
        xwos_splk_lock_cpuirqsv(&uartc->rxq.lock, &cpuirq);
        uartc->rxq.pos = 0;
        uartc->rxq.tail = 0;
        xwos_splk_unlock_cpuirqrs(&uartc->rxq.lock, cpuirq);
}

/**
 * @brief XWDS Driver Callback：发布数据到接收队列
 * @param[in] uartc: UART控制器对象指针
 * @param[in] tail: 新的数据接收位置（有效数据结尾 + 1）
 */
__xwds_code
void xwds_uartc_drvcb_rxq_pub(struct xwds_uartc * uartc, xwsq_t tail)
{
        xwsz_t pubsz;
        xwreg_t cpuirq;

        pubsz = 0;
        xwos_splk_lock_cpuirqsv(&uartc->rxq.lock, &cpuirq);
        if (tail == uartc->rxq.tail) {
                pubsz = 0;
        } else if (0 == uartc->rxq.tail) {
                if (tail > XWDS_UART_RXQ_SIZE) {
                        /* Low buffer is overflow. Discard the oldest data. */
                        pubsz = tail - XWDS_UART_RXQ_SIZE;
                        uartc->rxq.tail = tail;
                        uartc->rxq.pos = XWDS_UART_RXQ_SIZE;
                } else {
                        if (0 == uartc->rxq.pos) {
                                pubsz = tail;
                                uartc->rxq.tail = tail;
                        } else {
                                XWDS_BUG_ON(uartc->rxq.pos < XWDS_UART_RXQ_SIZE);
                                if (XWDS_UART_RXQ_SIZE == tail) {
                                        /* Low buffer is overflow.
                                           Discard the oldest data */
                                        pubsz = uartc->rxq.pos - tail;
                                        uartc->rxq.tail = tail;
                                        uartc->rxq.pos = 0;
                                }/* else {} */
                        }
                }
        } else if (uartc->rxq.tail < XWDS_UART_RXQ_SIZE) {
                XWDS_BUG_ON(uartc->rxq.pos > uartc->rxq.tail);
                if (tail > XWDS_UART_RXQ_SIZE) {
                        /* Low buffer is overflow. Discard the oldest data. */
                        uartc->rxq.tail = tail;
                        uartc->rxq.pos = XWDS_UART_RXQ_SIZE;
                        pubsz = tail - XWDS_UART_RXQ_SIZE;
                } else if (tail < uartc->rxq.tail) {
                        XWDS_BUG();
                } else {
                        pubsz = tail - uartc->rxq.tail;
                        uartc->rxq.tail = tail;
                }
        } else if (XWDS_UART_RXQ_SIZE == uartc->rxq.tail) {
                if ((tail < XWDS_UART_RXQ_SIZE) && (0 != tail)) {
                        /* High buffer is overflow. Discard the oldest data. */
                        pubsz = tail;
                        uartc->rxq.tail = tail;
                        uartc->rxq.pos = 0;
                } else {
                        if (XWDS_UART_RXQ_SIZE == uartc->rxq.pos) {
                                if (0 == tail) {
                                        pubsz = XWDS_UART_RXQ_SIZE;
                                } else {
                                        pubsz = tail - XWDS_UART_RXQ_SIZE;
                                }
                                uartc->rxq.tail = tail;
                        } else {
                                XWDS_BUG_ON(uartc->rxq.pos > XWDS_UART_RXQ_SIZE);
                                if (0 == tail) {
                                        /* High buffer is overflow.
                                           Discard the oldest data */
                                        pubsz = uartc->rxq.pos;
                                        uartc->rxq.tail = 0;
                                        uartc->rxq.pos = XWDS_UART_RXQ_SIZE;
                                }/* else {} */
                        }
                }
        } else {
                XWDS_BUG_ON(uartc->rxq.pos < XWDS_UART_RXQ_SIZE);
                XWDS_BUG_ON(uartc->rxq.pos > uartc->rxq.tail);
                if ((tail < uartc->rxq.tail) && (0 != tail)) {
                        /* High buffer is overflow. Discard the oldest data. */
                        if (tail <= XWDS_UART_RXQ_SIZE) {
                                pubsz = tail;
                                uartc->rxq.tail = tail;
                                uartc->rxq.pos = 0;
                        } else {
                                XWDS_BUG();
                        }
                } else {
                        if (0 == tail) {
                                pubsz = (2 * XWDS_UART_RXQ_SIZE) -
                                                uartc->rxq.tail;
                        } else {
                                pubsz = tail - uartc->rxq.tail;
                        }
                        uartc->rxq.tail = tail;
                }
        }
        xwos_splk_unlock_cpuirqrs(&uartc->rxq.lock, cpuirq);
        if (pubsz > 0) {
                xwos_sem_post(&uartc->rxq.sem);
        }/* else {} */
}
