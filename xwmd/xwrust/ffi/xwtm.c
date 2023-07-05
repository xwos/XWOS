/**
 * @file
 * @brief XWRUST FFI：调度
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#include <xwos/standard.h>
#include <xwos/osal/time.h>

xwtm_t xwrustffi_xwtm_now(void)
{
        return xwtm_now();
}

xwtm_t xwrustffi_xwtm_ft(xwtm_t xwtm)
{
        return xwtm_ft(xwtm);
}

xwtm_t xwrustffi_xwtm_nowts(void)
{
        return xwtm_nowts();
}

xwtm_t xwrustffi_xwtm_fts(xwtm_t xwtm)
{
        return xwtm_fts(xwtm);;
}

xwu64_t xwrustffi_xwtm_nowtc(void)
{
        return xwtm_nowtc();
}
