/**
 * @file
 * @brief XWRUST FFI：调度
 * @author
 * + 隐星曜 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#include <xwos/standard.h>
#include <xwos/osal/skd.h>

void xwrustffi_skd_get_context_lc(xwsq_t * ctxbuf, xwirq_t * irqnbuf)
{
        xwos_skd_get_context_lc(ctxbuf, irqnbuf);
}

void xwrustffi_skd_dspmpt_lc(void)
{
        xwos_skd_dspmpt_lc();
}

void xwrustffi_skd_enpmpt_lc(void)
{
        xwos_skd_enpmpt_lc();
}
