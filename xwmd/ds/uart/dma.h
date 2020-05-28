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
 */

#ifndef __xwmd_ds_uart_dma_h__
#define __xwmd_ds_uart_dma_h__

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwmd/ds/standard.h>
#include <xwos/lib/xwbop.h>
#include <xwos/osal/lock/spinlock.h>
#include <xwos/osal/sync/semaphore.h>
#include <xwmd/ds/device.h>
#include <xwmd/ds/soc/dma.h>
#include <xwmd/ds/uart/common.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define XWDS_DMAUART_RXQ_SIZE   XWMDCFG_ds_UART_DMA_RXQ_SIZE

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
struct xwds_dmauartc;

/**
 * @brief BSP中需要提供的DMA UART控制器驱动函数表
 */
struct xwds_dmauartc_driver {
        struct xwds_driver base; /**< C语言面向对象：继承struct xwds_driver */
        xwer_t (* cfg)(struct xwds_dmauartc *,
                       const struct xwds_uart_cfg *); /**< 配置UART控制器 */
        xwer_t (* tx)(struct xwds_dmauartc *, const xwu8_t *, xwsz_t,
                      xwtm_t *); /**< 配置DMA通道并发送 */
};

/**
 * @brief DMA UART控制器
 */
struct xwds_dmauartc {
        struct xwds_device dev; /**< 继承struct xwds_device */

        /* attributes */
        const struct xwds_uart_cfg * cfg; /**< 配置 */
        const struct xwds_resource_dma * rxdmarsc; /**< 用于接收UART数据的DMA资源 */
        const struct xwds_resource_dma * txdmarsc; /**< 用于发送UART数据的DMA资源 */

        /* RX FSM */
        struct {
                struct xwosal_smr smr; /**< 接收队列中数据可用的信号量 */
                struct xwosal_splk lock; /**< 保护接收队列的锁 */
                xwu8_t mem[2 * XWDS_DMAUART_RXQ_SIZE]; /**< double-queue */
                xwsq_t pos; /**< 当前有效数据的起始位置 */
                xwsz_t tail; /**< 当前有效数据的结束位置 + 1 */
        } rxq; /**< 循环接收队列 */
};

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         function prototypes         ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/******** ******** ******** constructor & destructor ******** ******** ********/
__xwds_api
void xwds_dmauartc_construct(struct xwds_dmauartc * dmauartc);

__xwds_api
void xwds_dmauartc_destruct(struct xwds_dmauartc * dmauartc);

/******** ******** ******** APIs ******** ******** ********/
/**
 * @brief XWDS API：从接收队列中获取数据
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
 * - 上下文：线程
 * - 重入性：可重入
 */
__xwds_api
xwer_t xwds_dmauartc_rx(struct xwds_dmauartc * dmauartc,
                        xwu8_t * buf, xwsz_t * size,
                        xwtm_t * xwtm);

/**
 * @brief XWDS API：尝试从接收队列中获取数据
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
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
__xwds_api
xwer_t xwds_dmauartc_try_rx(struct xwds_dmauartc * dmauartc,
                            xwu8_t * buf, xwsz_t * size);

/**
 * @brief XWDS API：配置DMA通道发送数据
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
 * - 上下文：线程
 * - 重入性：可重入
 */
__xwds_api
xwer_t xwds_dmauartc_tx(struct xwds_dmauartc * dmauartc,
                        const xwu8_t * data, xwsz_t size,
                        xwtm_t * xwtm);

/**
 * @brief XWDS API：配置UART
 * @param dmauartc: (I) DMA UART控制器对象指针
 * @param cfg: (I) 新的配置
 * @return 错误码
 * @retval OK: OK
 * @retval -EFAULT: 无效指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
__xwds_api
xwer_t xwds_dmauartc_cfg(struct xwds_dmauartc * dmauartc,
                         const struct xwds_uart_cfg * cfg);

/******** ******** Libraries for BSP driver ******** ********/
__xwds_lib_code
void xwds_dmauartc_lib_rxq_pub(struct xwds_dmauartc * dmauartc, xwsq_t tail);

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********   inline function implementations   ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief 增加对象的引用计数
 * @param dmauartc: (I) DMA UART控制器对象指针
 * @return 错误码
 * @retval @ref xwds_device_grab()
 */
static __xwds_inline
xwer_t xwds_dmauartc_grab(struct xwds_dmauartc * dmauartc)
{
        return xwds_device_grab(&dmauartc->dev);
}

/**
 * @brief 减少对象的引用计数
 * @param dmauartc: (I) DMA UART控制器对象指针
 * @return 错误码
 * @retval @ref xwds_device_put()
 */
static __xwds_inline
xwer_t xwds_dmauartc_put(struct xwds_dmauartc * dmauartc)
{
        return xwds_device_put(&dmauartc->dev);
}

/**
 * @brief 增加设备运行状态计数器
 * @param dmauartc: (I) DMA UART控制器对象指针
 * @return 错误码
 * @retval @ref xwds_device_request()
 */
static __xwds_inline
xwer_t xwds_dmauartc_request(struct xwds_dmauartc * dmauartc)
{
        return xwds_device_request(&dmauartc->dev);
}

/**
 * @brief 减少设备运行状态计数器
 * @param dmauartc: (I) DMA UART控制器对象指针
 * @return 错误码
 * @retval @ref xwds_device_release()
 */
static __xwds_inline
xwer_t xwds_dmauartc_release(struct xwds_dmauartc * dmauartc)
{
        return xwds_device_release(&dmauartc->dev);
}

#endif /* xwmd/ds/uart/dma.h */
