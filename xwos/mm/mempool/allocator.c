/**
 * @file
 * @brief XWOS内存管理：内存池：页分配器
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

__xwos_api
xwer_t xwmm_mempool_init(struct xwmm_mempool * mp, const char * name,
                         xwptr_t origin, xwsz_t size, xwsz_t pgodr,
                         xwsz_t pre, void ** membuf)
{
        xwer_t rc;
        struct xwmm_mempool_page_odrbtree * odrbtrees;
        xwsz_t odrbtrees_size;
        struct xwmm_mempool_page * pages;
        xwsz_t pages_nr;
        xwsz_t pages_size;
        void * mem;

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

        if (pre > 0) {
                rc = xwmm_mempool_malloc(mp, pre, &mem);
                if (rc < 0) {
                        goto err_mempool_prealloc;
                }
                if (membuf) {
                        *membuf = mem;
                }
        } else if ((xwptr_t)mp == origin) {
                xwsz_t mpsz = sizeof(struct xwmm_mempool) + odrbtrees_size + pages_size;
                if (mpsz < XWMM_MEMPOOL_PAGE_SIZE) {
                        mpsz = XWMM_MEMPOOL_PAGE_SIZE;
                }
                rc = xwmm_mempool_malloc(mp, mpsz, &mem);
                if (rc < 0) {
                        goto err_mempool_prealloc;
                }
                if (membuf) {
                        *membuf = mem;
                }
        }
        return XWOK;

err_mempool_prealloc:
err_mempool_construct:
err_size:
        return rc;
}

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
