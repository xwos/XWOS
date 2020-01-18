/**
 * @file
 * @brief xwmd设备栈：UART with DMA
 * @author
 * + 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 * @note
 * + 中断优先级：定时器中断的优先级 < DMA半完成与完成中断的优先级
 * + 在DMA半完成与完成中断中，进入xwds_dmauartc_lib_rxq_pub前，需要关闭定时器
 */

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <xwos/lib/string.h>
#include <xwos/osal/scheduler.h>
#include <xwos/osal/lock/spinlock.h>
#include <xwos/osal/sync/semaphore.h>
#include <xwmd/ds/uart/dma.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********     static function prototypes      ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
static __xwds_vop
xwer_t xwds_dmauartc_cvop_probe(struct xwds_dmauartc * dmauartc);

static __xwds_vop
xwer_t xwds_dmauartc_cvop_remove(struct xwds_dmauartc * dmauartc);

static __xwds_vop
xwer_t xwds_dmauartc_cvop_start(struct xwds_dmauartc * dmauartc);

static __xwds_vop
xwer_t xwds_dmauartc_cvop_stop(struct xwds_dmauartc * dmauartc);

#if defined(XWMDCFG_ds_LPM) && (1 == XWMDCFG_ds_LPM)
static __xwds_vop
xwer_t xwds_dmauartc_cvop_suspend(struct xwds_dmauartc * dmauartc);

static __xwds_vop
xwer_t xwds_dmauartc_cvop_resume(struct xwds_dmauartc * dmauartc);
#endif /* XWMDCFG_ds_LPM */

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#if !defined(XWMDCFG_ds_NANO) || (1 != XWMDCFG_ds_NANO)
__xwds_rodata const struct xwds_base_virtual_operations xwds_dmauartc_cvops = {
        .probe = (void *)xwds_dmauartc_cvop_probe,
        .remove = (void *)xwds_dmauartc_cvop_remove,
        .start = (void *)xwds_dmauartc_cvop_start,
        .stop = (void *)xwds_dmauartc_cvop_stop,
#if defined(XWMDCFG_ds_LPM) && (1 == XWMDCFG_ds_LPM)
        .suspend = (void *)xwds_dmauartc_cvop_suspend,
        .resume = (void *)xwds_dmauartc_cvop_resume,
#endif /* XWMDCFG_ds_LPM */
};
#endif /* !XWMDCFG_ds_NANO */

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#if !defined(XWMDCFG_ds_NANO) || (1 != XWMDCFG_ds_NANO)
/******** ******** ******** constructor & destructor ******** ******** ********/
/**
 * @brief DMA UART控制器的构造函数
 * @param dmauartc: (I) DMA UART控制器对象指针
 */
__xwds_code
void xwds_dmauartc_construct(struct xwds_dmauartc * dmauartc)
{
        xwds_device_construct(&dmauartc->dev);
        dmauartc->dev.cvops = &xwds_dmauartc_cvops;
}

/**
 * @brief DMA UART控制器对象的析构函数
 * @param dmauartc: (I) DMA UART控制器对象指针
 */
__xwds_code
void xwds_dmauartc_destruct(struct xwds_dmauartc * dmauartc)
{
        xwds_device_destruct(&dmauartc->dev);
}
#endif /* !XWMDCFG_ds_NANO */

/******** ******** base virtual operations ******** ********/
/**
 * @brief SODS VOP：探测DMA UART控制器
 * @param dmauartc: (I) DMA UART控制器对象指针
 * @return 错误码
 */
static __xwds_vop
xwer_t xwds_dmauartc_cvop_probe(struct xwds_dmauartc * dmauartc)
{
        xwer_t rc;

        xwosal_splk_init(&dmauartc->rxq.lock);
        rc = xwosal_smr_init(&dmauartc->rxq.smr, 0, 1);
        if (__unlikely(rc < 0)) {
                goto err_smr_init;
        }
        rc = xwds_device_cvop_probe(&dmauartc->dev);
        if (__unlikely(rc < 0)) {
                goto err_dev_probe;
        }
        return OK;

err_dev_probe:
        xwosal_smr_destroy(&dmauartc->rxq.smr);
err_smr_init:
        return rc;
}

/**
 * @brief SODS VOP：移除DMA UART控制器
 * @param dmauartc: (I) DMA UART控制器对象指针
 * @return 错误码
 */
static __xwds_vop
xwer_t xwds_dmauartc_cvop_remove(struct xwds_dmauartc * dmauartc)
{
        xwer_t rc;

        rc = xwds_device_cvop_remove(&dmauartc->dev);
        if (__unlikely(rc < 0)) {
                goto err_dev_cvop_remove;
        }

        xwosal_smr_destroy(&dmauartc->rxq.smr);
        return OK;

err_dev_cvop_remove:
        return rc;
}

/**
 * @brief SODS VOP：启动DMA UART控制器
 * @param dmauartc: (I) DMA UART控制器对象指针
 * @return 错误码
 */
static __xwds_vop
xwer_t xwds_dmauartc_cvop_start(struct xwds_dmauartc * dmauartc)
{
        xwer_t rc;

        memset(dmauartc->rxq.mem, 0, sizeof(dmauartc->rxq.mem));
        dmauartc->rxq.pos = 0;
        dmauartc->rxq.tail = 0;

        rc = xwds_device_cvop_start(&dmauartc->dev);
        return rc;
}

/**
 * @brief SODS VOP：停止DMA UART控制器
 * @param dmauartc: (I) DMA UART控制器对象指针
 * @return 错误码
 */
static __xwds_vop
xwer_t xwds_dmauartc_cvop_stop(struct xwds_dmauartc * dmauartc)
{
        xwer_t rc;

        rc = xwds_device_cvop_stop(&dmauartc->dev);
        return rc;
}

#if (defined(XWMDCFG_ds_LPM)) && (1 == XWMDCFG_ds_LPM)
/******** ******** pm ******** ********/
/**
 * @brief SODS VOP：暂停DMA UART控制器
 * @param dmauartc: (I) DMA UART控制器对象指针
 * @return 错误码
 */
static __xwds_vop
xwer_t xwds_dmauartc_cvop_suspend(struct xwds_dmauartc * dmauartc)
{
        xwer_t rc;

        rc = xwds_device_cvop_suspend(&dmauartc->dev);
        return rc;
}

/**
 * @brief SODS VOP：继续DMA UART控制器
 * @param dmauartc: (I) DMA UART控制器对象指针
 * @return 错误码
 */
static __xwds_vop
xwer_t xwds_dmauartc_cvop_resume(struct xwds_dmauartc * dmauartc)
{
        xwer_t rc;

        memset(dmauartc->rxq.mem, 0, sizeof(dmauartc->rxq.mem));
        dmauartc->rxq.pos = 0;
        dmauartc->rxq.tail = 0;

        rc = xwds_device_cvop_resume(&dmauartc->dev);
        return rc;
}
#endif /* XWMDCFG_ds_LPM */

/******** ******** ******** DMA UART APIs ******** ******** ********/
#if defined(XWMDCFG_ds_NANO) && (1 == XWMDCFG_ds_NANO)
/**
 * @brief SODS API：探测DMA UART控制器
 * @param dmauartc: (I) DMA UART控制器对象指针
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
xwer_t xwds_dmauartc_probe(struct xwds_dmauartc * dmauartc)
{
        xwer_t rc;

        SODS_VALIDATE(dmauartc, "nullptr", -EFAULT);

        rc = xwds_dmauartc_cvop_probe(dmauartc);
        return rc;
}

/**
 * @brief SODS API：移除DMA UART控制器
 * @param dmauartc: (I) DMA UART控制器对象指针
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
xwer_t xwds_dmauartc_remove(struct xwds_dmauartc * dmauartc)
{
        xwer_t rc;

        SODS_VALIDATE(dmauartc, "nullptr", -EFAULT);

        rc = xwds_dmauartc_cvop_remove(dmauartc);
        return rc;
}

/**
 * @brief SODS API：启动DMA UART控制器
 * @param dmauartc: (I) DMA UART控制器对象指针
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
xwer_t xwds_dmauartc_start(struct xwds_dmauartc * dmauartc)
{
        xwer_t rc;

        SODS_VALIDATE(dmauartc, "nullptr", -EFAULT);

        rc = xwds_dmauartc_cvop_start(dmauartc);
        return rc;
}

/**
 * @brief SODS API：停止DMA UART控制器
 * @param dmauartc: (I) DMA UART控制器对象指针
 * @return 错误码
 * @note
 * - 同步/异步：同步
 * - 中断上下文：可以使用
 * - 中断底半部：可以使用
 * - 线程上下文：可以使用
 * - 重入性：对于同一个设备不可重入；对于不同设备可重入
 */
__xwds_api
xwer_t xwds_dmauartc_stop(struct xwds_dmauartc * dmauartc)
{
        xwer_t rc;

        SODS_VALIDATE(dmauartc, "nullptr", -EFAULT);

        rc = xwds_dmauartc_cvop_stop(dmauartc);
        return rc;
}

#if (defined(XWMDCFG_ds_LPM)) && (1 == XWMDCFG_ds_LPM)
/******** ******** pm ******** ********/
/**
 * @brief SODS API：暂停DMA UART控制器
 * @param dmauartc: (I) DMA UART控制器对象指针
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
xwer_t xwds_dmauartc_suspend(struct xwds_dmauartc * dmauartc)
{
        xwer_t rc;

        SODS_VALIDATE(dmauartc, "nullptr", -EFAULT);

        rc = xwds_dmauartc_cvop_suspend(dmauartc);
        return rc;
}

/**
 * @brief SODS API：继续DMA UART控制器
 * @param dmauartc: (I) DMA UART控制器对象指针
 * @return 错误码
 * @note
 * - 同步/异步：同步
 * - 中断上下文：可以使用
 * - 中断底半部：可以使用
 * - 线程上下文：可以使用
 * - 重入性：对于同一个设备不可重入；对于不同设备可重入
 */
__xwds_api
xwer_t xwds_dmauartc_resume(struct xwds_dmauartc * dmauartc)
{
        xwer_t rc;

        SODS_VALIDATE(dmauartc, "nullptr", -EFAULT);

        rc = xwds_dmauartc_cvop_resume(dmauartc);
        return rc;
}
#endif /* XWMDCFG_ds_LPM */
#endif /* XWMDCFG_ds_NANO */

/**
 * @brief SODS API：从接收队列中获取数据
 * @param dmauartc: (I) DMA UART控制器对象指针
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
xwer_t xwds_dmauartc_rx(struct xwds_dmauartc * dmauartc,
                        xwu8_t * buf, xwsz_t * size,
                        xwtm_t * xwtm)
{
        xwsz_t available, real, cp, rest_buffer_size;
        xwsq_t pos;
        xwreg_t cpuirq;
        xwer_t rc;

        SODS_VALIDATE(dmauartc, "nullptr", -EFAULT);
        SODS_VALIDATE(buf, "nullptr", -EFAULT);
        SODS_VALIDATE(size, "nullptr", -EFAULT);
        SODS_VALIDATE(xwtm, "nullptr", -EFAULT);

        pos = 0;

#if !defined(XWMDCFG_ds_NANO) || (1 != XWMDCFG_ds_NANO)
        rc = xwds_dmauartc_grab(dmauartc);
        if (__unlikely(rc < 0)) {
                goto err_dmauartc_grab;
        }
#endif /* !XWMDCFG_ds_NANO */

        rest_buffer_size = *size;
        while (rest_buffer_size) {
                rc = xwosal_smr_timedwait(xwosal_smr_get_id(&dmauartc->rxq.smr), xwtm);
                if (__unlikely(rc < 0)) {
                        goto err_smr_timedwait;
                }
                xwosal_splk_lock_cpuirqsv(&dmauartc->rxq.lock, &cpuirq);
                if (dmauartc->rxq.tail >= dmauartc->rxq.pos) {
                        available = dmauartc->rxq.tail - dmauartc->rxq.pos;
                } else {
                        available = (2 * SODS_DMAUART_RXQ_SIZE) - dmauartc->rxq.pos;
                        available += dmauartc->rxq.tail;
                }
                real = available > rest_buffer_size ? rest_buffer_size : available;
                if ((real + dmauartc->rxq.pos) >= (2 * SODS_DMAUART_RXQ_SIZE)) {
                        cp = (2 * SODS_DMAUART_RXQ_SIZE) - dmauartc->rxq.pos;
                        memcpy(&buf[pos], &dmauartc->rxq.mem[dmauartc->rxq.pos], cp);
                        memcpy(&buf[cp + pos], &dmauartc->rxq.mem[0], real - cp);
                        dmauartc->rxq.pos = real - cp;
                } else {
                        memcpy(&buf[pos], &dmauartc->rxq.mem[dmauartc->rxq.pos], real);
                        dmauartc->rxq.pos += real;
                }
                xwosal_splk_unlock_cpuirqrs(&dmauartc->rxq.lock, cpuirq);
                pos += real;
                rest_buffer_size -= real;
                if (available > real) {
                        xwosal_smr_post(xwosal_smr_get_id(&dmauartc->rxq.smr));
                }
        }

#if !defined(XWMDCFG_ds_NANO) || (1 != XWMDCFG_ds_NANO)
        xwds_dmauartc_put(dmauartc);
#endif /* !XWMDCFG_ds_NANO */

        *size = pos;
        return OK;

err_smr_timedwait:
#if !defined(XWMDCFG_ds_NANO) || (1 != XWMDCFG_ds_NANO)
        xwds_dmauartc_put(dmauartc);
err_dmauartc_grab:
#endif /* !XWMDCFG_ds_NANO */
        *size = pos;
        return rc;
}

/**
 * @brief SODS API：尝试从接收队列中获取数据
 * @param dmauartc: (I) DMA UART控制器对象指针
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
xwer_t xwds_dmauartc_try_rx(struct xwds_dmauartc * dmauartc,
                            xwu8_t * buf, xwsz_t * size)
{
        xwsz_t available, real, cp, rest_buffer_size;
        xwsq_t pos;
        xwreg_t cpuirq;
        xwer_t rc;

        SODS_VALIDATE(dmauartc, "nullptr", -EFAULT);
        SODS_VALIDATE(buf, "nullptr", -EFAULT);
        SODS_VALIDATE(size, "nullptr", -EFAULT);

        pos = 0;

#if !defined(XWMDCFG_ds_NANO) || (1 != XWMDCFG_ds_NANO)
        rc = xwds_dmauartc_grab(dmauartc);
        if (__unlikely(rc < 0)) {
                goto err_dmauartc_grab;
        }
#endif /* !XWMDCFG_ds_NANO */

        rest_buffer_size = *size;
        while (rest_buffer_size) {
                rc = xwosal_smr_trywait(xwosal_smr_get_id(&dmauartc->rxq.smr));
                if (__unlikely(rc < 0)) {
                        goto err_smr_trywait;
                }
                xwosal_splk_lock_cpuirqsv(&dmauartc->rxq.lock, &cpuirq);
                if (dmauartc->rxq.tail >= dmauartc->rxq.pos) {
                        available = dmauartc->rxq.tail - dmauartc->rxq.pos;
                } else {
                        available = (2 * SODS_DMAUART_RXQ_SIZE) - dmauartc->rxq.pos;
                        available += dmauartc->rxq.tail;
                }
                real = available > rest_buffer_size ? rest_buffer_size : available;
                if ((real + dmauartc->rxq.pos) >= (2 * SODS_DMAUART_RXQ_SIZE)) {
                        cp = (2 * SODS_DMAUART_RXQ_SIZE) - dmauartc->rxq.pos;
                        memcpy(&buf[pos], &dmauartc->rxq.mem[dmauartc->rxq.pos], cp);
                        memcpy(&buf[cp + pos], &dmauartc->rxq.mem[0], real - cp);
                        dmauartc->rxq.pos = real - cp;
                } else {
                        memcpy(&buf[pos], &dmauartc->rxq.mem[dmauartc->rxq.pos], real);
                        dmauartc->rxq.pos += real;
                }
                xwosal_splk_unlock_cpuirqrs(&dmauartc->rxq.lock, cpuirq);
                pos += real;
                rest_buffer_size -= real;
                if (available > real) {
                        xwosal_smr_post(xwosal_smr_get_id(&dmauartc->rxq.smr));
                }/* else {} */
        }

#if !defined(XWMDCFG_ds_NANO) || (1 != XWMDCFG_ds_NANO)
        xwds_dmauartc_put(dmauartc);
#endif /* !XWMDCFG_ds_NANO */

        *size = pos;
        return OK;

err_smr_trywait:
#if !defined(XWMDCFG_ds_NANO) || (1 != XWMDCFG_ds_NANO)
        xwds_dmauartc_put(dmauartc);
err_dmauartc_grab:
#endif /* !XWMDCFG_ds_NANO */
        *size = pos;
        return rc;
}

/**
 * @brief SODS API：配置DMA通道发送数据
 * @param dmauartc: (I) DMA UART控制器对象指针
 * @param data: (I) 待发送的数据的缓冲区
 * @param size: (I) 待发送的数据的大小
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
xwer_t xwds_dmauartc_tx(struct xwds_dmauartc * dmauartc,
                        const xwu8_t * data, xwsz_t size,
                        xwtm_t * xwtm)
{
        xwer_t rc;
        const struct xwds_dmauartc_driver * drv;

        SODS_VALIDATE(dmauartc, "nullptr", -EFAULT);
        SODS_VALIDATE(data, "nullptr", -EFAULT);

#if !defined(XWMDCFG_ds_NANO) || (1 != XWMDCFG_ds_NANO)
        rc = xwds_dmauartc_grab(dmauartc);
        if (__unlikely(rc < 0)) {
                goto err_dmauartc_grab;
        }
#endif /* !XWMDCFG_ds_NANO */

        drv = xwds_static_cast(const struct xwds_dmauartc_driver *,
                               dmauartc->dev.drv);
        if ((drv) && (drv->tx)) {
                rc = drv->tx(dmauartc, data, size, xwtm);
        } else {
                rc = -ENOSYS;
        }
        if (__unlikely(rc < 0)) {
                goto err_txdma_cfg;
        }

#if !defined(XWMDCFG_ds_NANO) || (1 != XWMDCFG_ds_NANO)
        xwds_dmauartc_put(dmauartc);
#endif /* !XWMDCFG_ds_NANO */

        return OK;

err_txdma_cfg:
#if !defined(XWMDCFG_ds_NANO) || (1 != XWMDCFG_ds_NANO)
        xwds_dmauartc_put(dmauartc);
err_dmauartc_grab:
#endif /* !XWMDCFG_ds_NANO */
        return rc;
}

/**
 * @brief SODS API：配置UART
 * @param dmauartc: (I) DMA UART控制器对象指针
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
xwer_t xwds_dmauartc_cfg(struct xwds_dmauartc * dmauartc,
                         const struct xwds_uart_cfg * cfg)
{
        const struct xwds_dmauartc_driver * drv;
        xwer_t rc;

        SODS_VALIDATE(dmauartc, "nullptr", -EFAULT);
        SODS_VALIDATE(cfg, "nullptr", -EFAULT);

#if !defined(XWMDCFG_ds_NANO) || (1 != XWMDCFG_ds_NANO)
        rc = xwds_dmauartc_grab(dmauartc);
        if (__unlikely(rc < 0)) {
                goto err_dmauartc_grab;
        }
#endif /* !XWMDCFG_ds_NANO */

        drv = xwds_static_cast(const struct xwds_dmauartc_driver *, dmauartc->dev.drv);
        if ((drv) && (drv->cfg)) {
                rc = drv->cfg(dmauartc, cfg);
        } else {
                rc = -ENOSYS;
        }
        if (__unlikely(rc < 0)) {
                goto err_drv_cfg;
        }

#if !defined(XWMDCFG_ds_NANO) || (1 != XWMDCFG_ds_NANO)
        xwds_dmauartc_put(dmauartc);
#endif /* !XWMDCFG_ds_NANO */

        return OK;

err_drv_cfg:
#if !defined(XWMDCFG_ds_NANO) || (1 != XWMDCFG_ds_NANO)
        xwds_dmauartc_put(dmauartc);
err_dmauartc_grab:
#endif /* !XWMDCFG_ds_NANO */
        return rc;
}

/******** ******** Libraries for BSP driver ******** ********/
/**
 * @brief SODS LIB：发布数据到接收队列
 * @param dmauartc: (I) DMA UART控制器对象指针
 * @param tail: (I) 新的数据接收位置（有效数据结尾 + 1）
 */
__xwds_lib_code
void xwds_dmauartc_lib_rxq_pub(struct xwds_dmauartc * dmauartc, xwsq_t tail)
{
        xwsz_t pubsz;
        xwreg_t cpuirq;

        pubsz = 0;
        xwosal_splk_lock_cpuirqsv(&dmauartc->rxq.lock, &cpuirq);
        if (tail == dmauartc->rxq.tail) {
                pubsz = 0;
        } else if (0 == dmauartc->rxq.tail) {
                if (tail > SODS_DMAUART_RXQ_SIZE) {
                        /* Low buffer is overflow. Discard the oldest data. */
                        /* SODS_BUG(); */
                        pubsz = tail - SODS_DMAUART_RXQ_SIZE;
                        dmauartc->rxq.tail = tail;
                        dmauartc->rxq.pos = SODS_DMAUART_RXQ_SIZE;
                } else {
                        if (0 == dmauartc->rxq.pos) {
                                pubsz = tail;
                                dmauartc->rxq.tail = tail;
                        } else {
                                SODS_BUG_ON(dmauartc->rxq.pos < SODS_DMAUART_RXQ_SIZE);
                                if (SODS_DMAUART_RXQ_SIZE == tail) {
                                        /* Low buffer is overflow.
                                           Discard the oldest data */
                                        pubsz = dmauartc->rxq.pos - tail;
                                        dmauartc->rxq.tail = tail;
                                        dmauartc->rxq.pos = 0;
                                }/* else {} */
                        }
                }
        } else if (dmauartc->rxq.tail < SODS_DMAUART_RXQ_SIZE) {
                SODS_BUG_ON((tail < dmauartc->rxq.tail) ||
                            (tail > SODS_DMAUART_RXQ_SIZE));
                SODS_BUG_ON(dmauartc->rxq.pos > dmauartc->rxq.tail);
                if (tail > SODS_DMAUART_RXQ_SIZE) {
                        /* Low buffer is overflow. Discard the oldest data. */
                        /* SODS_BUG(); */
                        dmauartc->rxq.tail = tail;
                        dmauartc->rxq.pos = SODS_DMAUART_RXQ_SIZE;
                        pubsz = tail - SODS_DMAUART_RXQ_SIZE;
                } else if (tail < dmauartc->rxq.tail) {
                        SODS_BUG();
                } else {
                        pubsz = tail - dmauartc->rxq.tail;
                        dmauartc->rxq.tail = tail;
                }
        } else if (SODS_DMAUART_RXQ_SIZE == dmauartc->rxq.tail) {
                if ((tail < SODS_DMAUART_RXQ_SIZE) && (0 != tail)) {
                        /* High buffer is overflow. Discard the oldest data. */
                        /* SODS_BUG(); */
                        pubsz = tail;
                        dmauartc->rxq.tail = tail;
                        dmauartc->rxq.pos = 0;
                } else {
                        if (SODS_DMAUART_RXQ_SIZE == dmauartc->rxq.pos) {
                                if (0 == tail) {
                                        pubsz = SODS_DMAUART_RXQ_SIZE;
                                } else {
                                        pubsz = tail - SODS_DMAUART_RXQ_SIZE;
                                }
                                dmauartc->rxq.tail = tail;
                        } else {
                                SODS_BUG_ON(dmauartc->rxq.pos > SODS_DMAUART_RXQ_SIZE);
                                if (0 == tail) {
                                        /* High buffer is overflow.
                                           Discard the oldest data */
                                        pubsz = dmauartc->rxq.pos;
                                        dmauartc->rxq.tail = 0;
                                        dmauartc->rxq.pos = SODS_DMAUART_RXQ_SIZE;
                                }/* else {} */
                        }
                }
        } else {
                SODS_BUG_ON(dmauartc->rxq.pos < SODS_DMAUART_RXQ_SIZE);
                SODS_BUG_ON(dmauartc->rxq.pos > dmauartc->rxq.tail);
                if ((tail < dmauartc->rxq.tail) && (0 != tail)) {
                        /* High buffer is overflow. Discard the oldest data. */
                        /* SODS_BUG(); */
                        if (tail <= SODS_DMAUART_RXQ_SIZE) {
                                pubsz = tail;
                                dmauartc->rxq.tail = tail;
                                dmauartc->rxq.pos = 0;
                        } else {
                                SODS_BUG();
                        }
                } else {
                        if (0 == tail) {
                                pubsz = (2 * SODS_DMAUART_RXQ_SIZE) -
                                        dmauartc->rxq.tail;
                        } else {
                                pubsz = tail - dmauartc->rxq.tail;
                        }
                        dmauartc->rxq.tail = tail;
                }
        }
        xwosal_splk_unlock_cpuirqrs(&dmauartc->rxq.lock, cpuirq);
        if (pubsz > 0) {
                xwosal_smr_post(xwosal_smr_get_id(&dmauartc->rxq.smr));
        }/* else {} */
}
