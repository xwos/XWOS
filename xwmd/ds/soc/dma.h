/**
 * @file
 * @brief xwmd设备栈：SOC DMA
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __xwmd_ds_soc_dma_h__
#define __xwmd_ds_soc_dma_h__

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwmd/ds/standard.h>
#include <xwos/lib/xwbop.h>
#include <xwmd/ds/soc/chip.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief XWDS DMA 通道
 */
#define XWDS_DMA_CH(x)          ((xwid_t)x)

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         function prototypes         ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief XWDS API：申请DMA的通道
 * @param soc: (I) SOC对象指针
 * @param ch: (I) DMA通道
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效指针
 * @retval -ERANGE: 无效的DMA通道
 * @retval -EBUSY: DMA通道繁忙
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
__xwds_api
xwer_t xwds_dma_req(struct xwds_soc * soc, xwid_t ch);

/**
 * @brief XWDS API：释放DMA的通道
 * @param soc: (I) SOC对象指针
 * @param ch: (I) DMA通道
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效指针
 * @retval -ERANGE: 无效的DMA通道
 * @retval -EPERM: DMA通道未被使用
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
__xwds_api
xwer_t xwds_dma_rls(struct xwds_soc * soc, xwid_t ch);

/**
 * @brief XWDS API：配置DMA的通道
 * @param soc: (I) SOC对象指针
 * @param ch: (I) DMA通道
 * @param cfg: (I) SOC DMA通道配置
 * @param cb: (I) 传输结束回调函数
 * @param cfg: (I) 回调函数参数
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效指针
 * @retval -ERANGE: 无效的DMA通道
 * @retval -EPERM: DMA通道未被使用
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
__xwds_api
xwer_t xwds_dma_cfg(struct xwds_soc * soc, xwid_t ch, void * cfg,
                    xwds_dma_f cb, xwds_dma_cbarg_t arg);

/**
 * @brief XWDS API：打开DMA的通道
 * @param soc: (I) SOC对象指针
 * @param ch: (I) DMA通道
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效指针
 * @retval -ERANGE: 无效的DMA通道
 * @retval -EPERM: DMA通道未被使用
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
__xwds_api
xwer_t xwds_dma_enable(struct xwds_soc * soc, xwid_t ch);

/**
 * @brief XWDS API：关闭DMA的通道
 * @param soc: (I) SOC对象指针
 * @param ch: (I) DMA通道
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效指针
 * @retval -ERANGE: 无效的DMA通道
 * @retval -EPERM: DMA通道未被使用
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
__xwds_api
xwer_t xwds_dma_disable(struct xwds_soc * soc, xwid_t ch);

/**
 * @brief XWDS API：启动DMA的通道
 * @param soc: (I) SOC对象指针
 * @param ch: (I) DMA通道
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效指针
 * @retval -ERANGE: 无效的DMA通道
 * @retval -EPERM: DMA通道未被使用
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
__xwds_api
xwer_t xwds_dma_start(struct xwds_soc * soc, xwid_t ch);

/**
 * @brief XWDS API：停止DMA的通道
 * @param soc: (I) SOC对象指针
 * @param ch: (I) DMA通道
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效指针
 * @retval -ERANGE: 无效的DMA通道
 * @retval -EPERM: DMA通道未被使用
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
__xwds_api
xwer_t xwds_dma_stop(struct xwds_soc * soc, xwid_t ch);

/**
 * @brief XWDS API：通过描述得到DMA资源
 * @param base: (I) DMA资源数组的基地址
 * @param num: (I) DMA资源数量
 * @param descay: (I) 寄存器描述数组
 * @param descnum: (I) 寄存器描述数组数量
 * @param ret: (O) 返回DMA资源地址的缓存
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效指针
 * @retval -ENOSR: 找不到描述的资源
 */
__xwds_api
xwer_t xwds_get_dmarsc(const struct xwds_resource_dma base[], xwsz_t num,
                       const char * descay[], xwsz_t descnum,
                       const struct xwds_resource_dma ** ret);

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********  inline functions implementations   ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

#endif /* xwmd/ds/soc/dma.h */
