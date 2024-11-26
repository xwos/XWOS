/**
 * @file
 * @brief XWOS UP内核操作系统接口描述层：软件定时器
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#include <xwos/standard.h>
#include <xwos/up/osdl/swt.h>

#if (1 == XWOSRULE_SKD_SWT_CREATE_DELETE)
__xwup_code
xwer_t xwosdl_swt_create(xwosdl_swt_d * swtd, xwsq_t flag)
{
        xwer_t rc;
        struct xwup_swt * swt;

        XWOS_VALIDATE((NULL != swtd), "nullptr", -EFAULT);
        XWOS_VALIDATE(((XWOSDL_SWT_FLAG_NULL == flag) ||
                       (XWOSDL_SWT_FLAG_RESTART == flag)),
                      "invalid-value", -EINVAL);

        rc = xwup_swt_create(&swt, flag);
        if (XWOK == rc) {
                swtd->swt = swt;
                swtd->tik = swt->xwobj.tik;
        } else {
                *swtd = XWOSDL_SWT_NILD;
        }
        return rc;
}
#endif
