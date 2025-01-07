/**
 * @file
 * @brief XWRUST FFI：事件标志
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#include <xwos/standard.h>
#include <xwos/osal/sync/flg.h>

xwer_t xwrustffi_flg_init(struct xwos_flg * flg, xwsz_t num,
                          xwbmp_t * bmp, xwbmp_t * msk)
{
        return xwos_flg_init(flg, num, bmp, msk);
}

xwer_t xwrustffi_flg_fini(struct xwos_flg * flg)
{
        return xwos_flg_fini(flg);
}

xwer_t xwrustffi_flg_grab(struct xwos_flg * flg)
{
        return xwos_flg_grab(flg);
}

xwer_t xwrustffi_flg_put(struct xwos_flg * flg)
{
        return xwos_flg_put(flg);
}

xwsq_t xwrustffi_flg_get_tik(struct xwos_flg * flg)
{
        return xwos_flg_get_tik(flg);
}

xwer_t xwrustffi_flg_acquire(struct xwos_flg * flg, xwsq_t tik)
{
        xwos_flg_d flgd = {
                .flg = flg,
                .tik = tik,
        };
        return xwos_flg_acquire(flgd);
}

xwer_t xwrustffi_flg_release(struct xwos_flg * flg, xwsq_t tik)
{
        xwos_flg_d flgd = {
                .flg = flg,
                .tik = tik,
        };
        return xwos_flg_release(flgd);
}

xwer_t xwrustffi_flg_bind(struct xwos_flg * flg, struct xwos_sel * sel, xwsq_t pos)
{
        return xwos_flg_bind(flg, sel, pos);
}

xwer_t xwrustffi_flg_unbind(struct xwos_flg * flg, struct xwos_sel * sel)
{
        return xwos_flg_unbind(flg, sel);
}

xwer_t xwrustffi_flg_s1m(struct xwos_flg * flg, xwbmp_t msk[])
{
        return xwos_flg_s1m(flg, msk);
}

xwer_t xwrustffi_flg_s1i(struct xwos_flg * flg, xwsq_t pos)
{
        return xwos_flg_s1i(flg, pos);
}

xwer_t xwrustffi_flg_c0m(struct xwos_flg * flg, xwbmp_t msk[])
{
        return xwos_flg_c0m(flg, msk);
}

xwer_t xwrustffi_flg_c0i(struct xwos_flg * flg, xwsq_t pos)
{
        return xwos_flg_c0i(flg, pos);
}

xwer_t xwrustffi_flg_x1m(struct xwos_flg * flg, xwbmp_t msk[])
{
        return xwos_flg_x1m(flg, msk);
}

xwer_t xwrustffi_flg_x1i(struct xwos_flg * flg, xwsq_t pos)
{
        return xwos_flg_x1i(flg, pos);
}

xwer_t xwrustffi_flg_read(struct xwos_flg * flg, xwbmp_t out[])
{
        return xwos_flg_read(flg, out);
}

xwer_t xwrustffi_flg_wait(struct xwos_flg * flg, xwsq_t trigger, bool consumption,
                          xwbmp_t origin[], xwbmp_t msk[])
{
        return xwos_flg_wait(flg, trigger, consumption, origin, msk);
}

xwer_t xwrustffi_flg_wait_to(struct xwos_flg * flg, xwsq_t trigger, bool consumption,
                             xwbmp_t origin[], xwbmp_t msk[],
                             xwtm_t to)
{
        return xwos_flg_wait_to(flg, trigger, consumption, origin, msk, to);
}

xwer_t xwrustffi_flg_trywait(struct xwos_flg * flg, xwsq_t trigger, bool consumption,
                             xwbmp_t origin[], xwbmp_t msk[])
{
        return xwos_flg_trywait(flg, trigger, consumption, origin, msk);
}

xwer_t xwrustffi_flg_wait_unintr(struct xwos_flg * flg, xwsq_t trigger, bool consumption,
                                 xwbmp_t origin[], xwbmp_t msk[])
{
        return xwos_flg_wait(flg, trigger, consumption, origin, msk);
}
