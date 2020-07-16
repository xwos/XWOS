/**
 * @file
 * @brief XuanWuOS的内存管理机制：内存池 —— 页分配器
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <xwos/lib/string.h>
#include <xwos/lib/xwbop.h>
#include <xwos/mm/common.h>
#include <xwos/mm/kma.h>
#include <xwos/mm/mempool/page.h>
#include <xwos/mm/mempool/allocator.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      macros       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      static function prototypes     ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
static __xwos_code
xwer_t xwmm_mempool_construct(struct xwmm_mempool * mp, const char * name,
                              xwptr_t origin, xwsz_t size,
                              struct xwmm_mempool_page_odrbtree * odrbtree,
                              struct xwmm_mempool_page * pgarray);

static __xwos_code
xwer_t xwmm_mempool_destruct(struct xwmm_mempool * mp);

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief XWMM API：内存池的构造函数
 * @param mp: (I) 内存池的指针
 * @param name: (I) 名字
 * @param origin: (I) 内存区域的起始地址
 * @param size: (I) 内存区域的总大小
 * @param odrbtree: (I) 阶红黑树数组的指针
 * @param pgarray: (I) 页控制块数组的指针
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
        xwmm_mempool_objcache_destroy(&mp->oc_1024);
err_oc_1024_init:
        xwmm_mempool_objcache_destroy(&mp->oc_768);
err_oc_768_init:
        xwmm_mempool_objcache_destroy(&mp->oc_512);
err_oc_512_init:
        xwmm_mempool_objcache_destroy(&mp->oc_384);
err_oc_384_init:
        xwmm_mempool_objcache_destroy(&mp->oc_256);
err_oc_256_init:
        xwmm_mempool_objcache_destroy(&mp->oc_192);
err_oc_192_init:
        xwmm_mempool_objcache_destroy(&mp->oc_128);
err_oc_128_init:
        xwmm_mempool_objcache_destroy(&mp->oc_96);
err_oc_96_init:
        xwmm_mempool_objcache_destroy(&mp->oc_64);
err_oc_64_init:
        xwmm_mempool_objcache_destroy(&mp->oc_32);
err_oc_32_init:
        xwmm_mempool_objcache_destroy(&mp->oc_16);
err_oc_16_init:
        xwmm_mempool_objcache_destroy(&mp->oc_8);
err_oc_8_init:
        xwmm_mempool_page_allocator_destroy(&mp->pa);
err_pa_init:
        return rc;
}

/**
 * @brief XWMM API：内存池的析构函数
 * @param mp: (I) 内存池的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 */
static __xwos_code
xwer_t xwmm_mempool_destruct(struct xwmm_mempool * mp)
{
        xwmm_mempool_objcache_destroy(&mp->oc_2048);
        xwmm_mempool_objcache_destroy(&mp->oc_1024);
        xwmm_mempool_objcache_destroy(&mp->oc_768);
        xwmm_mempool_objcache_destroy(&mp->oc_512);
        xwmm_mempool_objcache_destroy(&mp->oc_384);
        xwmm_mempool_objcache_destroy(&mp->oc_256);
        xwmm_mempool_objcache_destroy(&mp->oc_192);
        xwmm_mempool_objcache_destroy(&mp->oc_128);
        xwmm_mempool_objcache_destroy(&mp->oc_96);
        xwmm_mempool_objcache_destroy(&mp->oc_64);
        xwmm_mempool_objcache_destroy(&mp->oc_32);
        xwmm_mempool_objcache_destroy(&mp->oc_16);
        xwmm_mempool_objcache_destroy(&mp->oc_8);
        xwmm_mempool_page_allocator_destroy(&mp->pa);

        return XWOK;
}

__xwos_api
xwer_t xwmm_mempool_init(struct xwmm_mempool * mp, const char * name,
                         xwptr_t origin, xwsz_t size)
{
        struct xwmm_mempool_page_odrbtree * odrbtree;
        struct xwmm_mempool_page * pgarray;
        xwsz_t nr;
        xwssq_t order;
        xwsz_t odrbtree_size;
        xwsz_t array_size;
        xwer_t rc;

        XWOS_VALIDATE((mp), "nullptr", -EFAULT);

        nr = size / XWMM_MEMPOOL_PAGE_SIZE;
        order = xwbop_fls(xwsz_t, nr);
        if ((order < 0) || (nr & ((1U << order) - 1U))) {
                rc = -EALIGN;
                goto err_aligned;
        }
        odrbtree_size = sizeof(struct xwmm_mempool_page_odrbtree) * (xwsz_t)(order + 1);
        array_size = (sizeof(struct xwmm_mempool_page) << order);
        odrbtree = (void *)(((xwptr_t)mp) + sizeof(struct xwmm_mempool));
        pgarray = (struct xwmm_mempool_page *)(((xwptr_t)odrbtree) + odrbtree_size);

        rc = xwmm_mempool_construct(mp, name, origin, size, odrbtree, pgarray);
        if (rc < 0) {
                goto err_mempool_construct;
        }

        if ((xwptr_t)mp == origin) {
                xwsz_t mpsz;
                void * mem;

                mpsz = sizeof(struct xwmm_mempool) + odrbtree_size + array_size;
                if (mpsz < XWMM_MEMPOOL_PAGE_SIZE) {
                        mpsz = XWMM_MEMPOOL_PAGE_SIZE;
                }
                rc = xwmm_mempool_malloc(mp, mpsz, &mem);
                if (rc < 0) {
                        goto err_mempool_alloc;
                }
        }

        return XWOK;

err_mempool_alloc:
        xwmm_mempool_destroy(mp);
err_mempool_construct:
err_aligned:
        return rc;
}

__xwos_api
xwer_t xwmm_mempool_destroy(struct xwmm_mempool * mp)
{
        XWOS_VALIDATE((mp), "nullptr", -EFAULT);

        return xwmm_mempool_destruct(mp);
}

__xwos_api
xwer_t xwmm_mempool_create(struct xwmm_mempool ** ptrbuf, const char * name,
                           xwptr_t origin, xwsz_t size)
{
        void * mem;
        xwsz_t nr;
        xwssq_t order;
        xwsz_t odrbtree_size;
        xwsz_t array_size;
        struct xwmm_mempool * mp;
        struct xwmm_mempool_page_odrbtree * odrbtree;
        struct xwmm_mempool_page * pgarray;
        xwer_t rc;

        XWOS_VALIDATE((ptrbuf), "nullptr", -EFAULT);

        if (size < (XWMM_MEMPOOL_PAGE_SIZE * 16)) {
                rc = -E2SMALL;
                goto err_mem2small;
        }

        nr = size / XWMM_MEMPOOL_PAGE_SIZE;
        order = xwbop_fls(xwsz_t, nr);
        if ((order < 0) || (nr & ((1U << order) - 1U))) {
                rc = -EALIGN;
                goto err_aligned;
        }

        odrbtree_size = sizeof(struct xwmm_mempool_page_odrbtree) * (xwsz_t)(order + 1);
        array_size = (sizeof(struct xwmm_mempool_page) << order);
        rc = xwmm_kma_alloc(sizeof(struct xwmm_mempool) +
                            odrbtree_size + array_size,
                            XWMM_ALIGNMENT, &mem);
        if (__unlikely(rc < 0)) {
                goto err_alloc;
        }
        mp = mem;
        odrbtree = (void *)(((xwptr_t)mem) + sizeof(struct xwmm_mempool));
        pgarray = (struct xwmm_mempool_page *)(((xwptr_t)odrbtree) + odrbtree_size);

        rc = xwmm_mempool_construct(mp, name, origin, size, odrbtree, pgarray);
        if (__unlikely(rc < 0)) {
                goto err_mempool_construct;
        }
        *ptrbuf = mp;
        return XWOK;

err_mempool_construct:
        xwmm_kma_free(mp);
err_alloc:
err_aligned:
err_mem2small:
        return rc;
}

__xwos_api
xwer_t xwmm_mempool_delete(struct xwmm_mempool * mp)
{

        XWOS_VALIDATE((mp), "nullptr", -EFAULT);

        xwmm_mempool_destroy(mp);
        xwmm_kma_free(mp);
        return XWOK;
}

__xwos_api
xwer_t xwmm_mempool_malloc(struct xwmm_mempool * mp, xwsz_t size, void ** membuf)
{
        xwsz_t real;
        interface xwmm_mempool_i_allocator * ia;
        xwssq_t odr;
        xwer_t rc;

        XWOS_VALIDATE((mp), "nullptr", -EFAULT);
        XWOS_VALIDATE((membuf), "nullptr", -EFAULT);

        if (0 == size) {
                rc = XWOK;
                *membuf = NULL;
        } else {
                real = ALIGN(size, 8);
                odr = xwbop_fls(xwsz_t, real);
                if ((odr < 0) || ((1U << odr) < real)) {
                        odr++;
                }
                real = 1U << odr;

                switch (real) {
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
                case 192:
                        ia = (void *)&mp->oc_192;
                        break;
                case 256:
                        ia = (void *)&mp->oc_256;
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
                rc = ia->malloc(ia, real, membuf);
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
        case 192:
                ia = (void *)&mp->oc_192;
                break;
        case 256:
                ia = (void *)&mp->oc_256;
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

                oldmem = *membuf;
                rc = xwmm_mempool_page_find(&mp->pa, oldmem, &pg);
                if (XWOK == rc) {
                        if (size <= pg->data.value) {
                                rc = XWOK;
                        } else {
                                void * newmem;

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
