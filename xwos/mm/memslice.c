/**
 * @file
 * @brief XWOS内存管理：内存切片分配器
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#include <xwos/standard.h>
#include <xwos/lib/xwbop.h>
#include <xwos/lib/xwaop.h>
#include <xwos/lib/lfq.h>
#include <xwos/mm/common.h>
#include <xwos/mm/memslice.h>

__xwos_api
xwer_t xwmm_memslice_init(struct xwmm_memslice * msa, xwptr_t origin,
                          xwsz_t total_size, xwsz_t card_size,
                          const char * name, ctor_f ctor, dtor_f dtor)
{
        xwsz_t num_max;
        xwsz_t n;
        xwsz_t nm;
        xwptr_t curr;
        xwptr_t next;
        xwer_t rc;

        XWOS_VALIDATE((msa), "nullptr", -EFAULT);

        // cppcheck-suppress [misra-c2012-17.8]
        card_size = XWBOP_ALIGN(card_size, XWMM_ALIGNMENT);
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
        if (NULL != ctor) {
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
                xwlib_lfq_init((atomic_xwlfq_t *)curr);
                xwlib_lfq_push(&msa->free_list, (atomic_xwlfq_t *)curr);
                curr = next;
        }

        return XWOK;

err_mem2small:
        return rc;
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
        atomic_xwlfq_t * card;

        XWOS_VALIDATE((msa), "nullptr", -EFAULT);
        XWOS_VALIDATE((mem), "nullptr", -EFAULT);

        if (((xwptr_t)mem < msa->zone.origin) ||
            (((xwptr_t)mem - msa->zone.origin) > msa->zone.size)) {
                rc = -EOWNER;
        } else {
                if (NULL != msa->dtor) {
                        msa->dtor(mem);
                }
                card = (atomic_xwlfq_t *)mem;
                xwlib_lfq_push(&msa->free_list, card);
                xwaop_add(xwsz_t, &msa->num_free, 1, NULL, NULL);
                rc = XWOK;
        }

        return rc;
}
