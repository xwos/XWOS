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

#ifndef __xwcd_ds_uart_general_h__
#define __xwcd_ds_uart_general_h__

#include <xwcd/ds/standard.h>
#include <xwos/lib/xwbop.h>
#include <xwos/osal/lock/seqlock.h>
#include <xwos/osal/lock/mtx.h>
#include <xwos/osal/sync/sem.h>
#include <xwcd/ds/device.h>
#include <xwcd/ds/uart/common.h>

struct xwds_uartc;

/**
 * @brief UART控制器驱动层操作接口
 */
struct xwds_uartc_driver {
        struct xwds_driver base; /**< C语言面向对象：继承struct xwds_driver */
        xwer_t (* cfg)(struct xwds_uartc *,
                       const struct xwds_uart_cfg *); /**< 配置 */
        xwer_t (* tx)(struct xwds_uartc *, xwu8_t); /**< 发送一个字节 */
        xwer_t (* rx)(struct xwds_uartc *, xwu8_t *); /**< 接收一个字节 */
};

/**
 * @brief UART控制器
 */
struct xwds_uartc {
        struct xwds_device dev; /**< 继承struct xwds_device */

        /* attributes */
        const struct xwds_uart_cfg * cfg; /**< uart配置 */

        /* private */
        /* rx */
        struct xwos_sem rxsem; /**< 接收队列的数据信号量 */
        struct xwos_sqlk rxseqlock; /**< 保护接收队列的锁 */
        xwu8_t rxq[XWCDCFG_ds_UART_GNR_RXQ_SIZE]; /**< 接收队列（循环数组） */
        xwssq_t rxpos; /**< 当前可以接收数据的位置 */
        xwssz_t rxnum; /**< 缓冲区中有效数据的数量 */

        /* tx */
        struct xwos_mtx txmtx; /**< 多线程安全的互斥锁 */
        xwu32_t txport_available; /**< 硬件发送端口可用标志 */
        struct xwos_sqlk txseqlock; /**< 保护发送队列的锁 */
        xwu8_t txq[XWCDCFG_ds_UART_GNR_TXQ_SIZE]; /**< 发送队列（循环数组）*/
        xwssq_t txpos; /**< 当前可以加入数据的位置 */
        xwssz_t txnum; /**< 缓冲区中待发送的数据的数量 */
};

void xwds_uartc_construct(struct xwds_uartc * uartc);
void xwds_uartc_destruct(struct xwds_uartc * uartc);

xwer_t xwds_uartc_clear_rxq(struct xwds_uartc * uartc);
xwer_t xwds_uartc_get_rxq_datasize(struct xwds_uartc * uartc, xwsz_t *ret);
xwer_t xwds_uartc_getc(struct xwds_uartc * uartc,
                       xwu8_t * buf, xwtm_t * xwtm);
xwer_t xwds_uartc_try_getc(struct xwds_uartc * uartc, xwu8_t * buf);
xwer_t xwds_uartc_rx(struct xwds_uartc * uartc,
                     void * buf, xwsz_t * size,
                     xwtm_t * xwtm);
xwer_t xwds_uartc_try_rx(struct xwds_uartc * uartc, void * buf, xwsz_t * size);
xwer_t xwds_uartc_clear_txq(struct xwds_uartc * uartc);
xwer_t xwds_uartc_putc(struct xwds_uartc * uartc,
                       const xwu8_t byte,
                       xwtm_t * xwtm);
xwer_t xwds_uartc_tx(struct xwds_uartc * uartc,
                     const xwu8_t * data, xwsz_t * size,
                     xwtm_t * xwtm);
xwer_t xwds_uartc_cfg(struct xwds_uartc * uartc,
                      const struct xwds_uart_cfg * cfg);

/**
 * @brief XWDS API：增加对象的引用计数
 * @param[in] uartc: UART控制器对象指针
 * @return 错误码
 * @retval @ref xwds_device_grab()
 */
static __xwds_inline_api
xwer_t xwds_uartc_grab(struct xwds_uartc * uartc)
{
        return xwds_device_grab(&uartc->dev);
}

/**
 * @brief XWDS API：减少对象的引用计数
 * @param[in] uartc: UART控制器对象指针
 * @return 错误码
 * @retval @ref xwds_device_put()
 */
static __xwds_inline_api
xwer_t xwds_uartc_put(struct xwds_uartc * uartc)
{
        return xwds_device_put(&uartc->dev);
}

/******** ******** Callbacks for driver ******** ********/
void xwds_uartc_drvcb_rx_isr(struct xwds_uartc * uartc);
void xwds_uartc_drvcb_tx_isr(struct xwds_uartc * uartc);

#endif /* xwcd/ds/uart/general.h */
