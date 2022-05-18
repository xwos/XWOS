/**
 * @file
 * @brief XWRUST FFI：信号选择器
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#include <xwos/standard.h>
#include <xwos/osal/sync/sel.h>

xwer_t xwrustffi_sel_init(struct xwos_sel * sel, xwsz_t num,
                          xwbmp_t * bmp, xwbmp_t * msk)
{
        return xwos_sel_init(sel, num, bmp, msk);
}

xwer_t xwrustffi_sel_fini(struct xwos_sel * sel)
{
        return xwos_sel_fini(sel);
}

xwer_t xwrustffi_sel_grab(struct xwos_sel * sel)
{
        return xwos_sel_grab(sel);
}

xwer_t xwrustffi_sel_put(struct xwos_sel * sel)
{
        return xwos_sel_put(sel);
}

xwsq_t xwrustffi_sel_gettik(struct xwos_sel * sel)
{
        return xwos_sel_gettik(sel);
}

xwer_t xwrustffi_sel_acquire(struct xwos_sel * sel, xwsq_t tik)
{
        xwos_sel_d seld = {
                .sel = sel,
                .tik = tik,
        };
        return xwos_sel_acquire(seld);
}

xwer_t xwrustffi_sel_release(struct xwos_sel * sel, xwsq_t tik)
{
        xwos_sel_d seld = {
                .sel = sel,
                .tik = tik,
        };
        return xwos_sel_release(seld);
}

xwer_t xwrustffi_sel_bind(struct xwos_sel * src, struct xwos_sel * dst, xwsq_t pos)
{
        return xwos_sel_bind(src, dst, pos);
}

xwer_t xwrustffi_sel_unbind(struct xwos_sel * src, struct xwos_sel * dst)
{
        return xwos_sel_unbind(src, dst);
}

xwer_t xwrustffi_sel_select(struct xwos_sel * sel, xwbmp_t msk[], xwbmp_t trg[])
{
        return xwos_sel_select(sel, msk, trg);
}

xwer_t xwrustffi_sel_select_to(struct xwos_sel * sel,
                               xwbmp_t msk[], xwbmp_t trg[],
                               xwtm_t to)
{
        return xwos_sel_select_to(sel, msk, trg, to);
}

xwer_t xwrustffi_sel_tryselect(struct xwos_sel * sel, xwbmp_t msk[], xwbmp_t trg[])
{
        return xwos_sel_select(sel, msk, trg);
}
