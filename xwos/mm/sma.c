/**
 * @file
 * @brief 玄武OS内存管理：简单内存分配器
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <xwos/standard.h>
#include <xwos/lib/xwbop.h>
#include <xwos/lib/xwaop.h>
#include <xwos/mm/common.h>
#include <xwos/mm/sma.h>

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
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
__xwos_api
xwer_t xwmm_sma_init(struct xwmm_sma * sa,
                     xwptr_t origin, xwsz_t size, xwsq_t pos,
                     const char * name)
{
        xwer_t rc;

        origin = XWBOP_ALIGN(origin, XWMM_ALIGNMENT);
        size = XWBOP_ROUND(size, XWMM_ALIGNMENT);
        pos = XWBOP_ALIGN(pos, XWMM_ALIGNMENT);
        if (size < pos) {
                rc = -EINVAL;
        } else {
                rc = XWOK;
                sa->zone.origin = origin;
                sa->zone.size = size;
                sa->pos = pos;
                sa->name = name;
        }
        return rc;
}

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
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
__xwos_api
xwer_t xwmm_sma_alloc(struct xwmm_sma * sa, xwsz_t size, xwsz_t aligned,
                      void ** membuf)
{
        xwsq_t cpos, npos;
        xwsq_t tmp;
        xwer_t rc;

        XWOS_VALIDATE((sa), "nullptr", -EFAULT);
        XWOS_VALIDATE((membuf), "nullptr", -EFAULT);

        if (__xwcc_unlikely(0 == size)) {
                rc = -EINVAL;
                *membuf = NULL;
                goto err_inval;
        }

        if (aligned & XWMM_ALIGNMENT_MASK) {
                rc = -EINVAL;
                *membuf = NULL;
                goto err_inval;
        }

        if (0 == aligned) {
                aligned = XWMM_ALIGNMENT;
        }/* else {} */

        size = XWBOP_ALIGN(size, XWMM_ALIGNMENT);
        do {
                cpos = xwaop_load(xwsq, &sa->pos, xwmb_modr_acquire);
                tmp = XWBOP_ALIGN((sa->zone.origin + cpos), aligned);
                npos = size + tmp - sa->zone.origin;
                if (npos > sa->zone.size) {
                        rc = -ENOMEM;
                        *membuf = NULL;
                        goto err_nomem;
                }
                *membuf = (void *)tmp;
        } while (xwaop_teq_then_write(xwsq, &sa->pos, cpos, npos, NULL));
        return XWOK;

err_nomem:
err_inval:
        return rc;
}

/**
 * @brief XWMM API：释放内存
 * @param[in] sa: 简单内存分配器对象
 * @param[in] mem: 内存的首地址
 * @return 错误码
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 * @note
 * - dummy函数，简单内存分配器不支持释放内存。
 */
__xwos_api
xwer_t xwmm_sma_free(struct xwmm_sma * sa, void * mem)
{
        XWOS_UNUSED(sa);
        XWOS_UNUSED(mem);
        return XWOK;
}
