/**
 * @file
 * @brief XWRUST FFI：条件量
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#include <xwos/standard.h>
#include <xwos/osal/sync/cond.h>

xwer_t xwrustffi_cond_init(struct xwos_cond * cond)
{
        return xwos_cond_init(cond);
}

xwer_t xwrustffi_cond_fini(struct xwos_cond * cond)
{
        return xwos_cond_fini(cond);
}

xwer_t xwrustffi_cond_grab(struct xwos_cond * cond)
{
        return xwos_cond_grab(cond);
}

xwer_t xwrustffi_cond_put(struct xwos_cond * cond)
{
        return xwos_cond_put(cond);
}

xwer_t xwrustffi_cond_create(struct xwos_cond ** condbuf, xwsq_t * tik)
{
        xwos_cond_d condd;
        xwer_t rc;

        rc = xwos_cond_create(&condd);
        *condbuf = condd.cond;
        *tik = condd.tik;
        return rc;
}

xwer_t xwrustffi_cond_delete(struct xwos_cond * cond, xwsq_t tik)
{
        xwos_cond_d condd = {
                .cond = cond,
                .tik = tik,
        };
        return xwos_cond_delete(condd);
}

xwsq_t xwrustffi_cond_gettik(struct xwos_cond * cond)
{
        return xwos_cond_gettik(cond);
}

xwer_t xwrustffi_cond_acquire(struct xwos_cond * cond, xwsq_t tik)
{
        xwos_cond_d condd = {
                .cond = cond,
                .tik = tik,
        };
        return xwos_cond_acquire(condd);
}

xwer_t xwrustffi_cond_release(struct xwos_cond * cond, xwsq_t tik)
{
        xwos_cond_d condd = {
                .cond = cond,
                .tik = tik,
        };
        return xwos_cond_release(condd);
}

xwer_t xwrustffi_cond_freeze(struct xwos_cond * cond)
{
        return xwos_cond_freeze(cond);
}

xwer_t xwrustffi_cond_thaw(struct xwos_cond * cond)
{
        return xwos_cond_thaw(cond);
}

xwer_t xwrustffi_cond_intr_all(struct xwos_cond * cond)
{
        return xwos_cond_intr_all(cond);
}

xwer_t xwrustffi_cond_broadcast(struct xwos_cond * cond)
{
        return xwos_cond_broadcast(cond);
}

xwer_t xwrustffi_cond_unicast(struct xwos_cond * cond)
{
        return xwos_cond_unicast(cond);
}

xwer_t xwrustffi_cond_wait(struct xwos_cond * cond,
                           void * lock, xwsq_t lktype, void * lkdata,
                           xwsq_t * lkst)
{
        return xwos_cond_wait(cond, *((union xwos_ulock *)&lock), lktype, lkdata, lkst);
}

xwer_t xwrustffi_cond_wait_to(struct xwos_cond * cond,
                              void * lock, xwsq_t lktype, void * lkdata,
                              xwtm_t to, xwsq_t * lkst)
{
        return xwos_cond_wait_to(cond,
                                 *((union xwos_ulock *)&lock), lktype, lkdata,
                                 to, lkst);
}
