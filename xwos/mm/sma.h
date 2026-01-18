/**
 * @file
 * @brief XWOS内存管理：简单内存分配器
 * @author
 * + 隐星曜 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef __xwos_mm_sma_h__
#define __xwos_mm_sma_h__

#include <xwos/standard.h>
#include <xwos/mm/common.h>

/**
 * @defgroup xwmm_sma 简单内存分配器
 * @ingroup xwmm
 * 简单内存分配器只分配大小最适应内存块，不对内存进行回收。
 *
 * + 优点：
 *   + 简单，代码量非常小；
 *   + 运行时间稳定。
 * + 缺点：内存一旦分配出去不再回收，释放函数只是一个 **Dummy** 函数；
 * + 适用性：简单的RTOS应用场合或不需要回收内存的场合
 * + 上下文的安全性：在任何上下文（中断、中断底半部、线程）都是安全的。
 * @{
 */

/**
 * @brief 简单内存分配器
 */
struct xwmm_sma {
        struct xwmm_zone zone; /**< 内存区域 */
        const char * name; /**< 名字 */
        atomic_xwsq_t pos; /**< 当前位置指针 */
};

/**
 * @brief XWMM API：初始化简单内存分配器对象
 * @param[in] sa: 简单内存分配器对象
 * @param[in] origin: 内存区域的首地址
 * @param[in] size: 内存区域的大小
 * @param[in] pos: 内存区域的位置
 * @param[in] name: 分配器的名字
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EINVAL: 内存的大小无效
 * @note
 * + 同步/异步：同步
 * + 上下文：中断、中断底半部、线程
 * + 重入性：不可重入
 */
xwer_t xwmm_sma_init(struct xwmm_sma * sa,
                     xwptr_t origin, xwsz_t size, xwsq_t pos,
                     const char * name);

/**
 * @brief XWMM API：从简单内存分配器中申请内存
 * @param[in] sa: 简单内存分配器对象
 * @param[in] size: 大小
 * @param[in] aligned: 申请到的内存的首地址需要对齐到的边界
 * @param[out] membuf: 指向地址缓存的指针，通过此指针缓存返回申请到的内存的首地址
 * @return 错误码
 * @retval -EFAULT: 空指针
 * @retval -EINVAL: 参数无效
 * @retval -ENOMEM: 内存不足
 * @note
 * + 同步/异步：同步
 * + 上下文：中断、中断底半部、线程
 * + 重入性：可重入
 */
xwer_t xwmm_sma_alloc(struct xwmm_sma * sa, xwsz_t size, xwsz_t aligned,
                      void ** membuf);

/**
 * @brief XWMM API：释放内存
 * @param[in] sa: 简单内存分配器对象
 * @param[in] mem: 内存的首地址
 * @return 错误码
 * @note
 * + 同步/异步：同步
 * + 上下文：中断、中断底半部、线程
 * + 重入性：可重入
 * @details
 * + Dummy函数，简单内存分配器不支持释放内存。
 */
xwer_t xwmm_sma_free(struct xwmm_sma * sa, void * mem);

/**
 * @} xwmm_sma
 */

#endif /* xwos/mm/sma.h */
