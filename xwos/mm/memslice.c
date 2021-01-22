/**
 * @file
 * @brief 玄武OS内存管理：内存切片分配器
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
#include <xwos/lib/lfq.h>
#include <xwos/mm/common.h>
#include <xwos/mm/kma.h>
#include <xwos/mm/memslice.h>

/**
 * @brief XWMM API：静态方式初始化内存切片分配器
 * @param msa: (I) 内存切片分配器对象的指针
 * @param origin: (I) 建立内存切片分配算法的内存区域首地址
 * @param total_size: (I) 建立内存切片分配算法的内存区域大小
 * @param card_size: (I) 切片大小
 * @param name: (I) 名字
 * @param ctor: (I) 切片的构造函数
 * @param dtor: (I) 切片的析构函数
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -E2SMALL: 内存区域太小
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
__xwos_api
xwer_t xwmm_memslice_init(struct xwmm_memslice * msa, xwptr_t origin,
                          xwsz_t total_size, xwsz_t card_size,
                          const char * name, ctor_f ctor, dtor_f dtor)
{
        xwsz_t num_max, n, nm;
        xwptr_t curr, next;
        xwer_t rc;

        XWOS_VALIDATE((msa), "nullptr", -EFAULT);

        card_size = ALIGN(card_size, XWMM_ALIGNMENT);
        num_max = total_size / card_size;
        if (0 == num_max) {
                rc = -E2SMALL;
                goto err_mem2small;
        }

        msa->zone.origin = origin;
        msa->zone.size = total_size;
        msa->name = name;
        msa->card_size = card_size;
        msa->num_max = num_max;
        msa->num_free = num_max;
        msa->ctor = ctor;
        msa->dtor = dtor;

        /* 构造所有对象 */
        if (ctor) {
                curr = origin;
                next = curr;
                for (n = 0; n < num_max; n++) {
                        next += card_size;
                        ctor((void *)curr);
                        curr = next;
                }
                msa->backup = *((xwptr_t *)origin);
        } else {
                msa->backup = 0;
        }

        xwlib_lfq_init(&msa->free_list);
        curr = origin;
        next = origin;
        nm = num_max;
        for (n = 0; n < nm; n++) {
                next += card_size;
                xwlib_lfq_init((xwlfq_a *)curr);
                xwlib_lfq_push(&msa->free_list, (xwlfq_a *)curr);
                curr = next;
        }

        return XWOK;

err_mem2small:
        return rc;
}

/**
 * @brief XWMM API：申请一片内存切片
 * @param msa: (I) 内存切片分配器对象的指针
 * @param membuf: (O) 指向地址缓存的指针，通过此指针缓存返回申请到的内存的首地址
 * @return 错误码
 * @retval -EFAULT: 空指针
 * @retval -ENOMEM: 内存不足
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
__xwos_api
xwer_t xwmm_memslice_alloc(struct xwmm_memslice * msa, void ** membuf)
{
        xwlfq_t * card;
        xwer_t rc;

        XWOS_VALIDATE((msa), "nullptr", -EFAULT);
        XWOS_VALIDATE((membuf), "nullptr", -EFAULT);

        card = xwlib_lfq_pop(&msa->free_list);
        if (NULL == card) {
                rc = -ENOMEM;
                *membuf = NULL;
                goto err_lfq_pop;
        }
        xwaop_sub(xwsz, &msa->num_free, 1, NULL, NULL);
        *(xwptr_t *)card = msa->backup; /* restore original data */
        *membuf = card;
        return XWOK;

err_lfq_pop:
        return rc;
}

/**
 * @brief XWMM API：释放一片内存切片
 * @param msa: (I) 内存切片分配器对象的指针
 * @param mem: (I) 内存切片的首地址
 * @return 错误码
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
__xwos_api
xwer_t xwmm_memslice_free(struct xwmm_memslice * msa, void * mem)
{
        xwer_t rc;
        xwlfq_a * card;

        XWOS_VALIDATE((msa), "nullptr", -EFAULT);
        XWOS_VALIDATE((mem), "nullptr", -EFAULT);

        if (__xwcc_unlikely((xwptr_t)mem < msa->zone.origin ||
                            ((xwptr_t)mem - msa->zone.origin) > msa->zone.size)) {
                rc = -EOWNER;
        } else {
                if (msa->dtor) {
                        msa->dtor(mem);
                }/* else {} */
                card = (xwlfq_a *)mem;
                xwlib_lfq_push(&msa->free_list, card);
                xwaop_add(xwsz, &msa->num_free, 1, NULL, NULL);
                rc = XWOK;
        }

        return rc;
}
