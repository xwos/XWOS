/**
 * @file
 * @brief 玄武OS UP内核操作系统接口描述层：软件定时器
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

__xwup_code
xwer_t xwosdl_swt_grab(struct xwosdl_swt * swt)
{
        XWOS_VALIDATE((swt), "nullptr", -EFAULT);
        return xwup_swt_grab(swt);
}

__xwup_code
xwer_t xwosdl_swt_put(struct xwosdl_swt * swt)
{
        XWOS_VALIDATE((swt), "nullptr", -EFAULT);
        return xwup_swt_put(swt);
}

__xwup_code
xwer_t xwosdl_swt_create(xwosdl_swt_d * swtd, xwsq_t flag)
{
        xwer_t rc;
        struct xwup_swt * swt;

        XWOS_VALIDATE((swtd), "nullptr", -EFAULT);

        rc = xwup_swt_create(&swt, flag);
        if (XWOK == rc) {
                swtd->swt = swt;
                swtd->tik = 1;
        } else {
                *swtd = XWOSDL_SWT_NILD;
        }
        return rc;
}

__xwup_code
xwer_t xwosdl_swt_acquire(struct xwosdl_swt * swt, xwsq_t tik)
{
        xwer_t rc;

        if ((NULL == swt) || (0 == tik)) {
                rc = -ENILOBJD;
        } else {
                rc = xwup_swt_grab(swt);
        }
        return rc;
}

__xwup_code
xwer_t xwosdl_swt_release(struct xwosdl_swt * swt, xwsq_t tik)
{
        xwer_t rc;

        if ((NULL == swt) || (0 == tik)) {
                rc = -ENILOBJD;
        } else {
                rc = xwup_swt_put(swt);
        }
        return rc;
}
