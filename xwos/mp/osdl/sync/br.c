/**
 * @file
 * @brief XWOS MP内核操作系统接口描述层：线程栅栏
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#include <xwos/standard.h>
#include <xwos/mp/osdl/sync/br.h>

#if (1 == XWOSRULE_SYNC_EVT_CREATE_DELETE)
__xwmp_code
xwer_t xwosdl_br_create(xwosdl_br_d * brd, xwsz_t num)
{
        xwer_t rc;
        struct xwmp_evt * br;

        XWOS_VALIDATE((brd), "nullptr", -EFAULT);

        rc = xwmp_evt_create(&br, XWMP_EVT_TYPE_BR, num);
        if (XWOK == rc) {
                brd->br = br;
                brd->tik = br->cond.synobj.xwobj.tik;
        } else {
                *brd = XWOSDL_BR_NILD;
        }
        return rc;
}
#endif
