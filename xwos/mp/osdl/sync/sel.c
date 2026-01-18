/**
 * @file
 * @brief XWOS MP内核操作系统接口描述层：信号选择器
 * @author
 * + 隐星曜 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#include <xwos/standard.h>
#include <xwos/mp/osdl/sync/sel.h>

#if (1 == XWOSRULE_SYNC_EVT_CREATE_DELETE)
__xwmp_code
xwer_t xwosdl_sel_create(xwosdl_sel_d * seld, xwsz_t num)
{
        xwer_t rc;
        struct xwmp_evt * sel;

        XWOS_VALIDATE((seld), "nullptr", -EFAULT);

        rc = xwmp_evt_create(&sel, XWMP_EVT_TYPE_SEL, num);
        if (XWOK == rc) {
                seld->sel = sel;
                seld->tik = sel->cond.synobj.xwobj.tik;
        } else {
                *seld = XWOSDL_SEL_NILD;
        }
        return rc;
}
#endif
