/**
 * @file
 * @brief XWRUST FFI：线程栅栏
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#include <xwos/standard.h>
#include <xwos/osal/sync/br.h>

xwer_t xwrustffi_br_init(struct xwos_br * br, xwsz_t num,
                         xwbmp_t * bmp, xwbmp_t * msk)
{
        return xwos_br_init(br, num, bmp, msk);
}

xwer_t xwrustffi_br_fini(struct xwos_br * br)
{
        return xwos_br_fini(br);
}

xwer_t xwrustffi_br_grab(struct xwos_br * br)
{
        return xwos_br_grab(br);
}

xwer_t xwrustffi_br_put(struct xwos_br * br)
{
        return xwos_br_put(br);
}

xwsq_t xwrustffi_br_get_tik(struct xwos_br * br)
{
        return xwos_br_get_tik(br);
}

xwer_t xwrustffi_br_acquire(struct xwos_br * br, xwsq_t tik)
{
        xwos_br_d brd = {
                .br = br,
                .tik = tik,
        };
        return xwos_br_acquire(brd);
}

xwer_t xwrustffi_br_release(struct xwos_br * br, xwsq_t tik)
{
        xwos_br_d brd = {
                .br = br,
                .tik = tik,
        };
        return xwos_br_release(brd);
}

xwer_t xwrustffi_br_bind(struct xwos_br * br, struct xwos_sel * sel, xwsq_t pos)
{
        return xwos_br_bind(br, sel, pos);
}

xwer_t xwrustffi_br_unbind(struct xwos_br * br, struct xwos_sel * sel)
{
        return xwos_br_unbind(br, sel);
}

xwer_t xwrustffi_br_wait(struct xwos_br * br)
{
        return xwos_br_wait(br);
}

xwer_t xwrustffi_br_wait_to(struct xwos_br * br, xwtm_t to)
{
        return xwos_br_wait_to(br, to);
}
