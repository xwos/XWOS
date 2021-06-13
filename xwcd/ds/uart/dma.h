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
 */

#ifndef __xwcd_ds_uart_dma_h__
#define __xwcd_ds_uart_dma_h__

#include <xwcd/ds/standard.h>
#include <xwos/lib/xwbop.h>
#include <xwos/osal/lock/spinlock.h>
#include <xwos/osal/sync/sem.h>
#include <xwos/osal/lock/mtx.h>
#include <xwcd/ds/device.h>
#include <xwcd/ds/soc/dma.h>
#include <xwcd/ds/uart/common.h>

#define XWDS_DMAUART_RXQ_SIZE   XWCDCFG_ds_UART_DMA_RXQ_SIZE

struct xwds_dmauartc;

/**
 * @brief BSP中需要提供的DMA UART控制器驱动函数表
 */
struct xwds_dmauartc_driver {
        struct xwds_driver base; /**< C语言面向对象：继承struct xwds_driver */
        xwer_t (* cfg)(struct xwds_dmauartc * /*dmauartc*/,
                       const struct xwds_uart_cfg * /*cfg*/); /**< 配置UART控制器 */
        xwer_t (* tx)(struct xwds_dmauartc * /*dmauartc*/,
                      const xwu8_t * /*data*/, xwsz_t * /*size*/,
                      xwtm_t * /*xwtm*/); /**< 配置DMA通道并发送 */
        xwer_t (* putc)(struct xwds_dmauartc * /*dmauartc*/,
                        const xwu8_t /*byte*/); /**< 发送一个字节 */
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

        /* TX */
        struct xwos_mtx txmtx; /**< 发送互斥锁 */

        /* RX */
        struct {
                struct xwos_sem sem; /**< 接收队列中数据可用的信号量 */
                struct xwos_splk lock; /**< 保护接收队列的锁 */
                xwsq_t pos; /**< 当前有效数据的起始位置 */
                xwsz_t tail; /**< 当前有效数据的结束位置 + 1 */
                xwu8_t mem[2 * XWDS_DMAUART_RXQ_SIZE] __xwcc_alignl1cache;
                                /**< 双缓冲区 */
        } rxq; /**< 循环接收队列 */
};

void xwds_dmauartc_construct(struct xwds_dmauartc * dmauartc);
void xwds_dmauartc_destruct(struct xwds_dmauartc * dmauartc);

xwer_t xwds_dmauartc_rx(struct xwds_dmauartc * dmauartc,
                        xwu8_t * buf, xwsz_t * size,
                        xwtm_t * xwtm);
xwer_t xwds_dmauartc_try_rx(struct xwds_dmauartc * dmauartc,
                            xwu8_t * buf, xwsz_t * size);
xwer_t xwds_dmauartc_tx(struct xwds_dmauartc * dmauartc,
                        const xwu8_t * data, xwsz_t * size,
                        xwtm_t * xwtm);
xwer_t xwds_dmauartc_putc(struct xwds_dmauartc * dmauartc,
                          const xwu8_t byte,
                          xwtm_t * xwtm);
xwer_t xwds_dmauartc_cfg(struct xwds_dmauartc * dmauartc,
                         const struct xwds_uart_cfg * cfg);

/**
 * @brief 增加对象的引用计数
 * @param[in] dmauartc: DMA UART控制器对象指针
 * @return 错误码
 * @retval @ref xwds_device_grab()
 */
static __xwds_inline_api
xwer_t xwds_dmauartc_grab(struct xwds_dmauartc * dmauartc)
{
        return xwds_device_grab(&dmauartc->dev);
}

/**
 * @brief 减少对象的引用计数
 * @param[in] dmauartc: DMA UART控制器对象指针
 * @return 错误码
 * @retval @ref xwds_device_put()
 */
static __xwds_inline_api
xwer_t xwds_dmauartc_put(struct xwds_dmauartc * dmauartc)
{
        return xwds_device_put(&dmauartc->dev);
}

/******** ******** Callbacks for driver ******** ********/
void xwds_dmauartc_drvcb_rxq_flush(struct xwds_dmauartc * dmauartc);
void xwds_dmauartc_drvcb_rxq_pub(struct xwds_dmauartc * dmauartc, xwsq_t tail);

#endif /* xwcd/ds/uart/dma.h */
