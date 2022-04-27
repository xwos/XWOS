/**
 * @file
 * @brief 玄武OS UP内核操作系统接口描述层：线程栅栏
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#include <xwos/standard.h>
#include <xwos/up/osdl/sync/br.h>

__xwup_code
xwer_t xwosdl_br_create(xwosdl_br_d * brd, xwsz_t num)
{
        xwer_t rc;
        struct xwup_evt * br;

        XWOS_VALIDATE((brd), "nullptr", -EFAULT);

        rc = xwup_evt_create(&br, XWUP_EVT_TYPE_BR, num);
        if (XWOK == rc) {
                brd->br = br;
                brd->tik = 0;
        } else {
                *brd = XWOSDL_BR_NILD;
        }
        return rc;
}

__xwup_code
xwer_t xwosdl_br_acquire(struct xwosdl_br * br, xwsq_t tik)
{
        xwer_t rc;

        XWOS_UNUSED(tik);

        if (NULL == br) {
                rc = -ENILOBJD;
        } else {
                rc = XWOK;
        }
        return rc;
}

__xwup_code
xwer_t xwosdl_br_release(struct xwosdl_br * br, xwsq_t tik)
{
        xwer_t rc;

        XWOS_UNUSED(tik);

        if (NULL == br) {
                rc = -ENILOBJD;
        } else {
                rc = XWOK;
        }
        return rc;
}
