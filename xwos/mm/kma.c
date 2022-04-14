/**
 * @file
 * @brief 玄武OS内存管理：内核内存分配器
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#include <xwos/standard.h>
#include <xwos/mm/common.h>
#include <xwos/mm/sma.h>
#include <xwos/mm/kma.h>

static __xwos_code
xwer_t xwmm_dkma_alloc(xwsz_t size, xwsz_t aligned, void ** membuf);

static __xwos_code
xwer_t xwmm_dkma_free(void * mem);

extern xwu8_t xwmm_dkma_mmz_origin[];
extern xwu8_t xwmm_dkma_mmz_size[];
extern xwu8_t xwmm_dkma_mmz_pos[];

/**
 * @brief 使用简单内存分配器作为默认的内核内存分配器
 */
__xwos_data struct xwmm_sma xwmm_dkma;

/**
 * @brief 默认的内核内存分配器的名字
 */
__xwos_rodata const char xwmm_dkma_name[] = "xwmm_dkma";

/**
 * @brief 申请内核内存虚函数
 */
__xwos_data xwmm_kma_alloc_f xwmm_kma_alloc_vf = xwmm_dkma_alloc;

/**
 * @brief 释放内核内存虚函数
 */
__xwos_data xwmm_kma_free_f xwmm_kma_free_vf = xwmm_dkma_free;

/**
 * @brief 初始化默认的内核内存分配器
 */
static __xwos_init_code
xwer_t xwmm_dkma_init(void)
{
        xwer_t rc;

        rc = xwmm_sma_init(&xwmm_dkma,
                           (xwptr_t)xwmm_dkma_mmz_origin,
                           (xwsz_t)xwmm_dkma_mmz_size,
                           (xwsq_t)xwmm_dkma_mmz_pos,
                           xwmm_dkma_name);
        return rc;
}

/**
 * @brief 从默认的内核内存分配器中申请内存
 * @param[in] size: 大小
 * @param[in] aligned: 申请到的内存的首地址需要对齐到的边界
 * @param[out] membuf: 指向指针缓存的指针，此指针缓存用于返回申请到的内存的首地址
 * @return 错误码
 * @retval -EFAULT: 空指针
 * @retval -EINVAL: 参数无效
 * @retval -ENOMEM: 内存不足
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
static __xwos_code
xwer_t xwmm_dkma_alloc(xwsz_t size, xwsz_t aligned, void ** membuf)
{
        return xwmm_sma_alloc(&xwmm_dkma, size, aligned, membuf);
}

/**
 * @brief 释放内存到默认的内核内存分配器
 * @param[in] mem: 内存首地址指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
static __xwos_code
xwer_t xwmm_dkma_free(void * mem)
{
        return xwmm_sma_free(&xwmm_dkma, mem);
}

/**
 * @brief 初始化内核内存分配器
 */
__xwos_init_code
xwer_t xwmm_kma_init(void)
{
        return xwmm_dkma_init();
}

/**
 * @brief 退出内核内存分配器
 */
__xwos_exit_code
xwer_t xwmm_kma_exit(void)
{
        return XWOK;
}

/**
 * @brief XWMM API：申请内核内存
 * @param[in] size: 大小
 * @param[in] aligned: 申请到的内存的首地址需要对齐到的边界
 * @param[out] membuf: 指向指针缓存的指针，此指针缓存用于返回申请到的内存的首地址
 * @return 错误码
 * @retval -EFAULT: 空指针
 * @retval -EINVAL: 参数无效
 * @retval -ENOMEM: 内存不足
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
__xwos_api
xwer_t xwmm_kma_alloc(xwsz_t size, xwsz_t aligned, void ** membuf)
{
        return xwmm_kma_alloc_vf(size, aligned, membuf);
}

/**
 * @brief XWMM API：释放内核内存
 * @param[in] mem: 内存首地址指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
__xwos_api
xwer_t xwmm_kma_free(void * mem)
{
        return xwmm_kma_free_vf(mem);
}
