/**
 * @file
 * @brief 玄武OS UP内核操作系统接口描述层：信号选择器
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#include <xwos/standard.h>
#include <xwos/up/osdl/sync/sel.h>

__xwup_code
xwer_t xwosdl_sel_grab(struct xwosdl_sel * sel)
{
        XWOS_VALIDATE((sel), "nullptr", -EFAULT);
        return xwup_evt_grab(sel);
}

__xwup_code
xwer_t xwosdl_sel_put(struct xwosdl_sel * sel)
{
        XWOS_VALIDATE((sel), "nullptr", -EFAULT);
        return xwup_evt_put(sel);
}

__xwup_code
xwer_t xwosdl_sel_create(xwosdl_sel_d * seld, xwsz_t num)
{
        xwer_t rc;
        struct xwup_evt * sel;

        XWOS_VALIDATE((seld), "nullptr", -EFAULT);

        rc = xwup_evt_create(&sel, XWUP_EVT_TYPE_SEL, num);
        if (XWOK == rc) {
                seld->sel = sel;
                seld->tik = sel->cond.synobj.xwobj.tik;
        } else {
                *seld = XWOSDL_SEL_NILD;
        }
        return rc;
}
