/**
 * @file
 * @brief XuanWuOS的内存管理机制：内存切片分配器
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
#include <xwos/lib/xwbop.h>
#include <xwos/lib/xwaop.h>
#include <xwos/lib/lfq.h>
#include <xwos/mm/common.h>
#include <xwos/mm/kma.h>
#include <xwos/mm/memslice.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      macros       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      static function prototypes     ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
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
                xwlib_lfq_init((__xwcc_atomic xwlfq_t *)curr);
                xwlib_lfq_push(&msa->free_list, (__xwcc_atomic xwlfq_t *)curr);
                curr = next;
        }

        return XWOK;

err_mem2small:
        return rc;
}

__xwos_api
xwer_t xwmm_memslice_destroy(struct xwmm_memslice * msa)
{
        XWOS_VALIDATE((msa), "nullptr", -EFAULT);

        XWOS_UNUSED(msa);
        return XWOK;
}

__xwos_api
xwer_t xwmm_memslice_create(struct xwmm_memslice ** ptrbuf,
                            xwptr_t origin, xwsz_t total_size, xwsz_t card_size,
                            const char * name, ctor_f ctor, dtor_f dtor)
{
        void * mem;
        struct xwmm_memslice * msa;
        xwsz_t num_max;
        xwer_t rc;

        XWOS_VALIDATE((ptrbuf), "nullptr", -EFAULT);

        card_size = ALIGN(card_size, XWMM_ALIGNMENT);
        num_max = total_size / card_size;
        if (__xwcc_unlikely(0 == num_max)) {
                rc = -E2SMALL;
                goto err_mem2small;
        }

        rc = xwmm_kma_alloc(sizeof(struct xwmm_memslice), XWMM_ALIGNMENT, &mem);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_msa_alloc;
        }
        msa = mem;

        rc = xwmm_memslice_init(msa, origin, total_size, card_size, name, ctor, dtor);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_msa_init;
        }

        *ptrbuf = msa;
        return XWOK;

err_msa_init:
        xwmm_kma_free(msa);
err_msa_alloc:
err_mem2small:
        return rc;
}

__xwos_api
xwer_t xwmm_memslice_delete(struct xwmm_memslice * msa)
{
        XWOS_VALIDATE((msa), "nullptr", -EFAULT);
        xwmm_kma_free(msa);
        return XWOK;
}

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
        xwaop_sub(xwsz_t, &msa->num_free, 1, NULL, NULL);
        *(xwptr_t *)card = msa->backup; /* restore original data */
        *membuf = card;
        return XWOK;

err_lfq_pop:
        return rc;
}

__xwos_api
xwer_t xwmm_memslice_free(struct xwmm_memslice * msa, void * mem)
{
        xwer_t rc;
        __xwcc_atomic xwlfq_t * card;

        XWOS_VALIDATE((msa), "nullptr", -EFAULT);
        XWOS_VALIDATE((mem), "nullptr", -EFAULT);

        if (__xwcc_unlikely((xwptr_t)mem < msa->zone.origin ||
                            ((xwptr_t)mem - msa->zone.origin) > msa->zone.size)) {
                rc = -EOWNER;
        } else {
                if (msa->dtor) {
                        msa->dtor(mem);
                }/* else {} */
                card = (__xwcc_atomic xwlfq_t *)mem;
                xwlib_lfq_push(&msa->free_list, card);
                xwaop_add(xwsz_t, &msa->num_free, 1, NULL, NULL);
                rc = XWOK;
        }

        return rc;
}
