/**
 * @file
 * @brief XWOS UP内核操作系统接口描述层：事件标志
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#include <xwos/standard.h>
#include <xwos/up/osdl/sync/flg.h>

__xwup_code
xwer_t xwosdl_flg_create(xwosdl_flg_d * flgd, xwsz_t num)
{
        xwer_t rc;
        struct xwup_evt * flg;

        XWOS_VALIDATE((flgd), "nullptr", -EFAULT);

        rc = xwup_evt_create(&flg, XWUP_EVT_TYPE_FLG, num);
        if (XWOK == rc) {
                flgd->flg = flg;
                flgd->tik = flg->cond.synobj.xwobj.tik;
        } else {
                *flgd = XWOSDL_FLG_NILD;
        }
        return rc;
}
