/**
 * @file
 * @brief XWRUST FFI：软件定时器
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#include <xwos/standard.h>
#include <xwos/osal/swt.h>

xwer_t xwrustffi_swt_once(struct xwos_swt * swt,
                          xwtm_t origin, xwtm_t period,
                          xwos_swt_f callback, void * arg)
{
        xwer_t rc;

        xwos_swt_init(swt, XWOS_SWT_FLAG_NULL);
        rc = xwos_swt_start(swt, origin, period, callback, arg);
        return rc;
}

xwer_t xwrustffi_swt_repeat(struct xwos_swt * swt,
                            xwtm_t origin, xwtm_t period,
                            xwos_swt_f callback, void * arg)
{
        xwer_t rc;

        xwos_swt_init(swt, XWOS_SWT_FLAG_RESTART);
        rc = xwos_swt_start(swt, origin, period, callback, arg);
        return rc;
}

xwer_t xwrustffi_swt_fini(struct xwos_swt * swt)
{
        return xwos_swt_fini(swt);
}

xwer_t xwrustffi_swt_grab(struct xwos_swt * swt)
{
        return xwos_swt_grab(swt);
}

xwer_t xwrustffi_swt_put(struct xwos_swt * swt)
{
        return xwos_swt_put(swt);
}

xwsq_t xwrustffi_swt_get_tik(struct xwos_swt * swt)
{
        return xwos_swt_get_tik(swt);
}

xwer_t xwrustffi_swt_acquire(struct xwos_swt * swt, xwsq_t tik)
{
        xwos_swt_d swtd = {
                .swt = swt,
                .tik = tik,
        };
        return xwos_swt_acquire(swtd);
}

xwer_t xwrustffi_swt_release(struct xwos_swt * swt, xwsq_t tik)
{
        xwos_swt_d swtd = {
                .swt = swt,
                .tik = tik,
        };
        return xwos_swt_release(swtd);
}

xwer_t xwrustffi_swt_stop(struct xwos_swt * swt)
{
        return xwos_swt_stop(swt);
}
