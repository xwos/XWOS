/**
 * @file
 * @brief XWOS内存管理：内存池 —— 页分配器
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#include <xwos/standard.h>
#include <string.h>
#include <xwos/lib/xwbop.h>
#include <xwos/mm/common.h>
#include <xwos/mm/kma.h>
#include <xwos/mm/mempool/page.h>
#include <xwos/mm/mempool/allocator.h>

/**
 * @brief 构建内存池
 * @param[in] mp: 内存池的指针
 * @param[in] name: 名字
 * @param[in] origin: 内存区域的起始地址
 * @param[in] size: 内存区域的总大小
 * @param[in] odrbtree: 阶红黑树数组的指针
 * @param[in] pgarray: 页控制块数组的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -E2SMALL: 内存区域太小
 * @retval -EALIGN: 内存区域没有对齐
 */
static __xwos_code
xwer_t xwmm_mempool_construct(struct xwmm_mempool * mp, const char * name,
                              xwptr_t origin, xwsz_t size,
                              struct xwmm_mempool_page_odrbtree * odrbtree,
                              struct xwmm_mempool_page * pgarray)
{
        xwer_t rc;

        rc = xwmm_mempool_page_allocator_init(&mp->pa, NULL,
                                              origin, size, XWMM_MEMPOOL_PAGE_SIZE,
                                              odrbtree, pgarray);
        if (rc < 0) {
                goto err_pa_init;
        }

        rc = xwmm_mempool_objcache_init(&mp->oc_8, &mp->pa, NULL,
                                        8, XWMM_ALIGNMENT,
                                        XWMM_MEMPOOL_OC_8_PAGE_ODR,
                                        NULL, NULL);
        if (rc < 0) {
                goto err_oc_8_init;
        }

        rc = xwmm_mempool_objcache_init(&mp->oc_16, &mp->pa, NULL,
                                        16, XWMM_ALIGNMENT,
                                        XWMM_MEMPOOL_OC_16_PAGE_ODR,
                                        NULL, NULL);
        if (rc < 0) {
                goto err_oc_16_init;
        }

        rc = xwmm_mempool_objcache_init(&mp->oc_32, &mp->pa, NULL,
                                        32, XWMM_ALIGNMENT,
                                        XWMM_MEMPOOL_OC_32_PAGE_ODR,
                                        NULL, NULL);
        if (rc < 0) {
                goto err_oc_32_init;
        }

        rc = xwmm_mempool_objcache_init(&mp->oc_64, &mp->pa, NULL,
                                        64, XWMM_ALIGNMENT,
                                        XWMM_MEMPOOL_OC_64_PAGE_ODR,
                                        NULL, NULL);
        if (rc < 0) {
                goto err_oc_64_init;
        }

        rc = xwmm_mempool_objcache_init(&mp->oc_96, &mp->pa, NULL,
                                        96, XWMM_ALIGNMENT,
                                        XWMM_MEMPOOL_OC_96_PAGE_ODR,
                                        NULL, NULL);
        if (rc < 0) {
                goto err_oc_96_init;
        }

        rc = xwmm_mempool_objcache_init(&mp->oc_128, &mp->pa, NULL,
                                        128, XWMM_ALIGNMENT,
                                        XWMM_MEMPOOL_OC_128_PAGE_ODR,
                                        NULL, NULL);
        if (rc < 0) {
                goto err_oc_128_init;
        }

        rc = xwmm_mempool_objcache_init(&mp->oc_160, &mp->pa, NULL,
                                        160, XWMM_ALIGNMENT,
                                        XWMM_MEMPOOL_OC_160_PAGE_ODR,
                                        NULL, NULL);
        if (rc < 0) {
                goto err_oc_160_init;
        }

        rc = xwmm_mempool_objcache_init(&mp->oc_192, &mp->pa, NULL,
                                        192, XWMM_ALIGNMENT,
                                        XWMM_MEMPOOL_OC_192_PAGE_ODR,
                                        NULL, NULL);
        if (rc < 0) {
                goto err_oc_192_init;
        }

        rc = xwmm_mempool_objcache_init(&mp->oc_256, &mp->pa, NULL,
                                        256, XWMM_ALIGNMENT,
                                        XWMM_MEMPOOL_OC_256_PAGE_ODR,
                                        NULL, NULL);
        if (rc < 0) {
                goto err_oc_256_init;
        }

        rc = xwmm_mempool_objcache_init(&mp->oc_320, &mp->pa, NULL,
                                        320, XWMM_ALIGNMENT,
                                        XWMM_MEMPOOL_OC_320_PAGE_ODR,
                                        NULL, NULL);
        if (rc < 0) {
                goto err_oc_320_init;
        }

        rc = xwmm_mempool_objcache_init(&mp->oc_384, &mp->pa, NULL,
                                        384, XWMM_ALIGNMENT,
                                        XWMM_MEMPOOL_OC_384_PAGE_ODR,
                                        NULL, NULL);
        if (rc < 0) {
                goto err_oc_384_init;
        }

        rc = xwmm_mempool_objcache_init(&mp->oc_512, &mp->pa, NULL,
                                        512, XWMM_ALIGNMENT,
                                        XWMM_MEMPOOL_OC_512_PAGE_ODR,
                                        NULL, NULL);
        if (rc < 0) {
                goto err_oc_512_init;
        }

        rc = xwmm_mempool_objcache_init(&mp->oc_768, &mp->pa, NULL,
                                        768, XWMM_ALIGNMENT,
                                        XWMM_MEMPOOL_OC_768_PAGE_ODR,
                                        NULL, NULL);
        if (rc < 0) {
                goto err_oc_768_init;
        }

        rc = xwmm_mempool_objcache_init(&mp->oc_1024, &mp->pa, NULL,
                                        1024, XWMM_ALIGNMENT,
                                        XWMM_MEMPOOL_OC_1024_PAGE_ODR,
                                        NULL, NULL);
        if (rc < 0) {
                goto err_oc_1024_init;
        }

        rc = xwmm_mempool_objcache_init(&mp->oc_2048, &mp->pa, NULL,
                                        2048, XWMM_ALIGNMENT,
                                        XWMM_MEMPOOL_OC_2048_PAGE_ODR,
                                        NULL, NULL);
        if (rc < 0) {
                goto err_oc_2048_init;
        }

        mp->name = name;

        return XWOK;

err_oc_2048_init:
err_oc_1024_init:
err_oc_768_init:
err_oc_512_init:
err_oc_384_init:
err_oc_320_init:
err_oc_256_init:
err_oc_192_init:
err_oc_160_init:
err_oc_128_init:
err_oc_96_init:
err_oc_64_init:
err_oc_32_init:
err_oc_16_init:
err_oc_8_init:
err_pa_init:
        return rc;
}

/**
 * @brief XWMM API：初始化内存池
 * @param[in] mp: 内存池的指针
 * @param[in] name: 名字
 * @param[in] origin: 内存区域的起始地址
 * @param[in] size: 内存区域的总大小
 * @param[in] pgodr: 页的数量，以2的pgodr次方形式表示
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -E2SMALL: 内存区域太小
 * @retval -ESIZE: 内存区域大小size与pgodr指明的页数量不匹配
 * @note
 * + 同步/异步：同步
 * + 上下文：中断、中断底半部、线程
 * + 重入性：不可重入
 * @details
 * + 将首地址为 `origin` ，大小为 `size` 的内存初始化为内存池：
 *   + 页的数量用 `pgodr` 表示，只能是2的n次方，即 `2, 4, 8, 16, 32, 64, 128, ...` ，
 *     对应的pgodr分别为 `1, 2, 3, 4, 5, 6, 7, ...` ；
 *   + 内存区域大小必须满足关系： `size == (XWMM_MEMPOOL_PAGE_SIZE * (1 << pgodr))` 。
 */
__xwos_api
xwer_t xwmm_mempool_init(struct xwmm_mempool * mp, const char * name,
                         xwptr_t origin, xwsz_t size, xwsz_t pgodr)
{
        xwer_t rc;
        struct xwmm_mempool_page_odrbtree * odrbtrees;
        xwsz_t odrbtrees_size;
        struct xwmm_mempool_page * pages;
        xwsz_t pages_nr;
        xwsz_t pages_size;

        XWOS_VALIDATE((mp), "nullptr", -EFAULT);

        pages_nr = size / XWMM_MEMPOOL_PAGE_SIZE;
        if (pages_nr != (1U << pgodr)) {
                rc = - ESIZE;
                goto err_size;
        }

        odrbtrees = (struct xwmm_mempool_page_odrbtree *)&mp[1];
        pages = (struct xwmm_mempool_page *)&odrbtrees[pgodr + 1];
        odrbtrees_size = sizeof(struct xwmm_mempool_page_odrbtree) * (pgodr + 1);
        pages_size = sizeof(struct xwmm_mempool_page) << pgodr;
        rc = xwmm_mempool_construct(mp, name, origin, size, odrbtrees, pages);
        if (rc < 0) {
                goto err_mempool_construct;
        }

        if ((xwptr_t)mp == origin) {
                xwsz_t mpsz;
                void * mem;

                mpsz = sizeof(struct xwmm_mempool) + odrbtrees_size + pages_size;
                if (mpsz < XWMM_MEMPOOL_PAGE_SIZE) {
                        mpsz = XWMM_MEMPOOL_PAGE_SIZE;
                }
                rc = xwmm_mempool_malloc(mp, mpsz, &mem);
                if (rc < 0) {
                        goto err_mempool_selfalloc;
                }
        }

        return XWOK;

err_mempool_selfalloc:
err_mempool_construct:
err_size:
        return rc;
}

/**
 * @brief XWMM API：从内存池中申请内存
 * @param[in] mp: 内存池的指针
 * @param[in] size: 申请的大小
 * @param[out] membuf: 指向缓冲区的指针，通过此缓冲区返回申请到的内存的首地址
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -ENOMEM: 内存不足
 * @note
 * + 同步/异步：同步
 * + 上下文：中断、中断底半部、线程
 * + 重入性：可重入
 * @details
 * + 此函数向内存池申请大小为 `size` 的内存：
 *   + 若申请成功，通过 `*membuf` 返回申请到的内存地址 ，返回错误码为 `XWOK` ；
 *   + 若申请失败，通过 `*membuf` 返回 `NULL` ，返回值为负的错误码。
 * + 内存不会被初始化；
 * + 当 `size` 为 `0` ， 通过 `*membuf` 返回 `NULL` ，返回错误码为 `XWOK` ；
 */
__xwos_api
xwer_t xwmm_mempool_malloc(struct xwmm_mempool * mp, xwsz_t size, void ** membuf)
{
        interface xwmm_mempool_i_allocator * ia;
        xwssq_t odr;
        xwer_t rc;

        XWOS_VALIDATE((mp), "nullptr", -EFAULT);
        XWOS_VALIDATE((membuf), "nullptr", -EFAULT);

        if (0 == size) {
                rc = XWOK;
                *membuf = NULL;
        } else {
                size = XWBOP_ALIGN(size, XWMM_ALIGNMENT);
                odr = xwbop_fls(xwsz_t, size);
                while ((1U << odr) < size) {
                        odr++;
                }
                size = 1U << odr;

                switch (size) {
                case 8:
                        ia = (void *)&mp->oc_8;
                        break;
                case 16:
                        ia = (void *)&mp->oc_16;
                        break;
                case 32:
                        ia = (void *)&mp->oc_32;
                        break;
                case 64:
                        ia = (void *)&mp->oc_64;
                        break;
                case 128:
                        if (size <= 96) {
                                ia = (void *)&mp->oc_96;
                        } else {
                                ia = (void *)&mp->oc_128;
                        }
                        break;
                case 256:
                        if (size <= 160) {
                                ia = (void *)&mp->oc_160;
                        } else if (size <= 192) {
                                ia = (void *)&mp->oc_192;
                        } else {
                                ia = (void *)&mp->oc_256;
                        }
                        break;
                case 512:
                        if (size <= 320) {
                                ia = (void *)&mp->oc_320;
                        } else if (size <= 384) {
                                ia = (void *)&mp->oc_384;
                        } else {
                                ia = (void *)&mp->oc_512;
                        }
                        break;
                case 1024:
                        if (size <= 768) {
                                ia = (void *)&mp->oc_768;
                        } else {
                                ia = (void *)&mp->oc_1024;
                        }
                        break;
                case 2048:
                        ia = (void *)&mp->oc_2048;
                        break;
                default:
                        ia = (void *)&mp->pa;
                        break;
                }
                rc = ia->malloc(ia, size, membuf);
                if (rc < 0) {
                        *membuf = NULL;
                }
        }
        return rc;
}

/**
 * @brief XWMM API：释放内存
 * @param[in] mp: 内存池的指针
 * @param[in] mem: 内存的首地址
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -EALREADY: 页内存已释放
 * @retval -ERANGE: 内存地址不在内存池的范围内
 * @note
 * + 同步/异步：同步
 * + 上下文：中断、中断底半部、线程
 * + 重入性：可重入
 * @details
 * 此函数向内存池释放内存。内存必须是此前申请的，如果 `mem` 是错误的地址或之前被释放过，
 * 将产生未定义的错误。 `mem` 可以为 `NULL` ，此函数什么也不做，并且返回错误码 `XWOK` 。
 */
__xwos_api
xwer_t xwmm_mempool_free(struct xwmm_mempool * mp, void * mem)
{
        interface xwmm_mempool_i_allocator * ia;
        struct xwmm_mempool_page * pg;
        xwer_t rc;

        XWOS_VALIDATE((mp), "nullptr", -EFAULT);

        if (NULL == mem) {
                rc = XWOK;
                goto do_nothing;
        }

        rc = xwmm_mempool_page_find(&mp->pa, mem, &pg);
        if (rc < 0) {
                goto err_pg_find;
        }

        switch (pg->data.value) {
        case 8:
                ia = (void *)&mp->oc_8;
                break;
        case 16:
                ia = (void *)&mp->oc_16;
                break;
        case 32:
                ia = (void *)&mp->oc_32;
                break;
        case 64:
                ia = (void *)&mp->oc_64;
                break;
        case 96:
                ia = (void *)&mp->oc_96;
                break;
        case 128:
                ia = (void *)&mp->oc_128;
                break;
        case 160:
                ia = (void *)&mp->oc_160;
                break;
        case 192:
                ia = (void *)&mp->oc_192;
                break;
        case 256:
                ia = (void *)&mp->oc_256;
                break;
        case 320:
                ia = (void *)&mp->oc_320;
                break;
        case 384:
                ia = (void *)&mp->oc_384;
                break;
        case 512:
                ia = (void *)&mp->oc_512;
                break;
        case 768:
                ia = (void *)&mp->oc_768;
                break;
        case 1024:
                ia = (void *)&mp->oc_1024;
                break;
        case 2048:
                ia = (void *)&mp->oc_2048;
                break;
        default:
                ia = (void *)&mp->pa;
                break;
        }
        rc = ia->free(ia, mem);

err_pg_find:
do_nothing:
        return rc;
}

/**
 * @brief XWMM API：调整内存大小
 * @param[in] mp: 内存池的指针
 * @param[in] size: 申请的大小，当size == 0，realloc等价于free
 * @param[in,out] membuf: 指向缓冲区的指针，此缓冲区
 * + (I) 作为输入时，当*membuf == NULL，realloc等价于malloc
 * + (O) 作为输出时，通过此缓冲区返回申请到的内存的首地址
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -ENOMEM: 内存不足
 * @retval -ERANGE: 内存地址不在内存池的范围内
 * @note
 * + 同步/异步：同步
 * + 上下文：中断、中断底半部、线程
 * + 重入性：可重入
 * @details
 * + 此API类似于C11标准中的 `realloc()` 函数：
 *   + 当 `*membuf` 为 `NULL` ，此函数等价于 `xwmm_mempool_mealloc(mp, size, membuf)` ；
 *   + 当 `*membuf` 不为 `NULL` 且 `size` 为 `0` ，
 *     此函数等价于 `xwmm_mempool_free(mp, *membuf)` ，并且通过 `*membuf` 返回 `NULL` ；
 *   + 当 `*membuf` 不为 `NULL` 且 `size` 比之前的小 ，此函数不重新申请内存，直接返回原来的 `*membuf` 以及 `XWOK` ；
 *   + 当 `*membuf` 不为 `NULL` 且 `size` 比之前的大 ，此函数会尝试重新申请内存：
 *     + 如果申请失败，原来的内存不会受影响，且通过 `*membuf` 返回 `NULL` 以及 错误码；
 *     + 如果申请成功，会将旧内存空间的内容移动到新内存空间内，然后返回新的 `*membuf` 以及 `XWOK` 。
 */
__xwos_api
xwer_t xwmm_mempool_realloc(struct xwmm_mempool * mp, xwsz_t size, void ** membuf)
{
        xwer_t rc;

        XWOS_VALIDATE((mp), "nullptr", -EFAULT);
        XWOS_VALIDATE((membuf), "nullptr", -EFAULT);

        if (NULL == *membuf) {
                rc = xwmm_mempool_malloc(mp, size, membuf);
        } else if (0 == size) {
                rc = xwmm_mempool_free(mp, *membuf);
                if (XWOK == rc) {
                        *membuf = NULL;
                }
        } else {
                struct xwmm_mempool_page * pg;
                void * oldmem;
                void * newmem;

                oldmem = *membuf;
                rc = xwmm_mempool_page_find(&mp->pa, oldmem, &pg);
                if (XWOK == rc) {
                        if (size <= pg->data.value) {
                                rc = XWOK;
                        } else {
                                rc = xwmm_mempool_malloc(mp, size, &newmem);
                                if (XWOK == rc) {
                                        memcpy(newmem, oldmem, pg->data.value);
                                        rc = xwmm_mempool_free(mp, oldmem);
                                        if (XWOK == rc) {
                                                *membuf = newmem;
                                        } else {
                                                xwmm_mempool_free(mp, newmem);
                                                *membuf = NULL;
                                        }
                                } else {
                                        *membuf = NULL;
                                }
                        }
                } else {
                        *membuf = NULL;
                }
        }
        return rc;
}

/**
 * @brief XWMM API：从内存池中申请对齐的内存
 * @param[in] mp: 内存池的指针
 * @param[in] alignment: 内存的起始地址对齐的字节数，只能是2的n次方
 * @param[in] size: 申请的大小
 * @param[out] membuf: 指向缓冲区的指针，通过此缓冲区返回申请到的内存的首地址
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -ENOMEM: 内存不足
 * @note
 * + 同步/异步：同步
 * + 上下文：中断、中断底半部、线程
 * + 重入性：可重入
 * @details
 * + 此API类似于C标准中的 `memalign()` 函数：
 *   + `alignment` 如果比 `XWMM_ALIGNMENT` 小，会被扩大为 `XWMM_ALIGNMENT` ：
 *   + `alignment` 只能是2的n次方：
 *   + 若size小于 `alignment` ， `size` 会被扩大为 `alignment` ，
 *   + 若size大于 `alignment` ， `size` 会向上对齐到2的n次方，此时也一定为 `alignment` 的整数倍。
 * + 申请内存失败时，此函数不会修改 `*membuf` 的值。
 */
__xwos_api
xwer_t xwmm_mempool_memalign(struct xwmm_mempool * mp,
                             xwsz_t alignment, xwsz_t size,
                             void ** membuf)
{
        interface xwmm_mempool_i_allocator * ia;
        xwssq_t p2;
        xwer_t rc;

        XWOS_VALIDATE((mp), "nullptr", -EFAULT);
        XWOS_VALIDATE((membuf), "nullptr", -EFAULT);

        if (alignment < XWMM_ALIGNMENT) {
                alignment = XWMM_ALIGNMENT;
        }
        p2 = xwbop_fls(xwsz_t, alignment);
        if ((1U << p2) != alignment) {
                rc = -EINVAL;
                goto err_notp2;
        }
        if (0 == size) {
                rc = XWOK;
                *membuf = NULL;
                goto nothing;
        }
        if (size <= alignment) {
                size = alignment;
        } else {
                p2 = xwbop_fls(xwsz_t, size);
                while ((1U << p2) < size) {
                        p2++;
                }
                size = 1U << p2;
        }

        switch (size) {
        case 8:
                ia = (void *)&mp->oc_8;
                break;
        case 16:
                ia = (void *)&mp->oc_16;
                break;
        case 32:
                ia = (void *)&mp->oc_32;
                break;
        case 64:
                ia = (void *)&mp->oc_64;
                break;
        case 128:
                ia = (void *)&mp->oc_128;
                break;
        case 256:
                ia = (void *)&mp->oc_256;
                break;
        case 512:
                ia = (void *)&mp->oc_512;
                break;
        case 1024:
                ia = (void *)&mp->oc_1024;
                break;
        case 2048:
                ia = (void *)&mp->oc_2048;
                break;
        default:
                ia = (void *)&mp->pa;
                break;
        }
        rc = ia->malloc(ia, size, membuf);

nothing:
err_notp2:
        return rc;
}
