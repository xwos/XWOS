/**
 * @file
 * @brief 玄武设备栈：SOC：DMA
 * @author
 * + 隐星曜 (Roy Sun) <xwos@xwos.tech>
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

#ifndef __xwcd_ds_soc_dma_h__
#define __xwcd_ds_soc_dma_h__

#include <xwcd/ds/standard.h>
#include <xwos/lib/xwbop.h>
#include <xwcd/ds/soc/chip.h>

/**
 * @ingroup xwcd_ds_soc
 * @{
 */

/**
 * @brief XWDS DMA 通道
 */
#define XWDS_DMA_CH(x)          ((xwid_t)(x))

/**
 * @brief XWDS API：申请DMA的通道
 * @param[in] soc: SOC对象指针
 * @param[in] ch: DMA通道
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效指针
 * @retval -ERANGE: 无效的DMA通道
 * @retval -EBUSY: DMA通道繁忙
 * @note
 * + 上下文：中断、中断底半部、线程
 */
xwer_t xwds_dma_req(struct xwds_soc * soc, xwid_t ch);

/**
 * @brief XWDS API：释放DMA的通道
 * @param[in] soc: SOC对象指针
 * @param[in] ch: DMA通道
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效指针
 * @retval -ERANGE: 无效的DMA通道
 * @retval -EPERM: DMA通道未被使用
 * @note
 * + 上下文：中断、中断底半部、线程
 */
xwer_t xwds_dma_rls(struct xwds_soc * soc, xwid_t ch);

/**
 * @brief XWDS API：配置DMA的通道
 * @param[in] soc: SOC对象指针
 * @param[in] ch: DMA通道
 * @param[in] cfg: SOC DMA通道配置
 * @param[in] cb: 传输结束回调函数
 * @param[in] cfg: 回调函数参数
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效指针
 * @retval -ERANGE: 无效的DMA通道
 * @retval -EPERM: DMA通道未被使用
 * @note
 * + 上下文：中断、中断底半部、线程
 */
xwer_t xwds_dma_cfg(struct xwds_soc * soc, xwid_t ch, void * cfg,
                    xwds_dma_f cb, xwds_dma_cbarg_t arg);

/**
 * @brief XWDS API：打开DMA的通道
 * @param[in] soc: SOC对象指针
 * @param[in] ch: DMA通道
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效指针
 * @retval -ERANGE: 无效的DMA通道
 * @retval -EPERM: DMA通道未被使用
 * @note
 * + 上下文：中断、中断底半部、线程
 */
xwer_t xwds_dma_enable(struct xwds_soc * soc, xwid_t ch);

/**
 * @brief XWDS API：关闭DMA的通道
 * @param[in] soc: SOC对象指针
 * @param[in] ch: DMA通道
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效指针
 * @retval -ERANGE: 无效的DMA通道
 * @retval -EPERM: DMA通道未被使用
 * @note
 * + 上下文：中断、中断底半部、线程
 */
xwer_t xwds_dma_disable(struct xwds_soc * soc, xwid_t ch);

/**
 * @brief XWDS API：启动DMA的通道
 * @param[in] soc: SOC对象指针
 * @param[in] ch: DMA通道
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效指针
 * @retval -ERANGE: 无效的DMA通道
 * @retval -EPERM: DMA通道未被使用
 * @note
 * + 上下文：中断、中断底半部、线程
 */
xwer_t xwds_dma_start(struct xwds_soc * soc, xwid_t ch);

/**
 * @brief XWDS API：停止DMA的通道
 * @param[in] soc: SOC对象指针
 * @param[in] ch: DMA通道
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效指针
 * @retval -ERANGE: 无效的DMA通道
 * @retval -EPERM: DMA通道未被使用
 * @note
 * + 上下文：中断、中断底半部、线程
 */
xwer_t xwds_dma_stop(struct xwds_soc * soc, xwid_t ch);

/**
 * @} xwcd_ds_soc
 */

#endif /* xwcd/ds/soc/dma.h */
