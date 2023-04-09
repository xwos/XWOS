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
 */

#ifndef __xwcd_ds_uart_controller_h__
#define __xwcd_ds_uart_controller_h__

#include <xwcd/ds/standard.h>
#include <xwos/lib/xwbop.h>
#include <xwos/osal/lock/spinlock.h>
#include <xwos/osal/sync/sem.h>
#include <xwos/osal/lock/mtx.h>
#include <xwcd/ds/device.h>
#include <xwcd/ds/soc/dma.h>
#include <xwcd/ds/uart/common.h>

#define XWDS_UART_RXQ_SIZE XWCDCFG_ds_UART_RXQ_SIZE

struct xwds_uartc;

/**
 * @brief BSP中需要提供的UART控制器驱动函数表
 */
struct xwds_uartc_driver {
        struct xwds_driver base; /**< C语言面向对象：继承struct xwds_driver */
        xwer_t (* cfg)(struct xwds_uartc * /*uartc*/,
                       const struct xwds_uart_cfg * /*cfg*/); /**< 配置UART控制器 */
        xwer_t (* tx)(struct xwds_uartc * /*uartc*/,
                      const xwu8_t * /*data*/, xwsz_t * /*size*/,
                      xwtm_t /*to*/); /**< 配置DMA通道并发送 */
        xwer_t (* putc)(struct xwds_uartc * /*uartc*/,
                        const xwu8_t /*byte*/); /**< 发送一个字节 */
};

/**
 * @brief UART控制器
 */
struct xwds_uartc {
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
                xwu8_t mem[2 * XWDS_UART_RXQ_SIZE] __xwcc_alignl1cache; /**< 双缓冲区 */
        } rxq; /**< 循环接收队列 */
};

void xwds_uartc_construct(struct xwds_uartc * uartc);
void xwds_uartc_destruct(struct xwds_uartc * uartc);
xwer_t xwds_uartc_grab(struct xwds_uartc * uartc);
xwer_t xwds_uartc_put(struct xwds_uartc * uartc);

xwer_t xwds_uartc_rx(struct xwds_uartc * uartc,
                     xwu8_t * buf, xwsz_t * size,
                     xwtm_t to);
xwer_t xwds_uartc_try_rx(struct xwds_uartc * uartc,
                         xwu8_t * buf, xwsz_t * size);
xwer_t xwds_uartc_tx(struct xwds_uartc * uartc,
                     const xwu8_t * data, xwsz_t * size,
                     xwtm_t to);
xwer_t xwds_uartc_putc(struct xwds_uartc * uartc,
                       const xwu8_t byte,
                       xwtm_t to);
xwer_t xwds_uartc_cfg(struct xwds_uartc * uartc,
                      const struct xwds_uart_cfg * cfg);

/******** ******** Callbacks for driver ******** ********/
void xwds_uartc_drvcb_rxq_flush(struct xwds_uartc * uartc);
void xwds_uartc_drvcb_rxq_pub(struct xwds_uartc * uartc, xwsq_t tail);

#endif /* xwcd/ds/uart/controller.h */
