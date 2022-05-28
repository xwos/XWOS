/**
 * @file
 * @brief 玄武OS UP内核操作系统接口描述层：互斥锁
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#include <xwos/standard.h>
#include <xwos/up/osdl/lock/mtx.h>

__xwup_code
xwer_t xwosdl_mtx_create(xwosdl_mtx_d * mtxd, xwpr_t sprio)
{
        xwer_t rc;
        struct xwup_mtx * mtx;

        XWOS_VALIDATE((mtxd), "nullptr", -EFAULT);

        rc = xwup_mtx_create(&mtx, sprio);
        if (XWOK == rc) {
                mtxd->mtx = mtx;
                mtxd->tik = 1;
        } else {
                *mtxd = XWOSDL_MTX_NILD;
        }
        return rc;
}

__xwup_code
xwer_t xwosdl_mtx_acquire(struct xwosdl_mtx * mtx, xwsq_t tik)
{
        xwer_t rc;

        if ((NULL == mtx) || (0 == tik)) {
                rc = -ENILOBJD;
        } else {
                rc = XWOK;
        }
        return rc;
}

__xwup_code
xwer_t xwosdl_mtx_release(struct xwosdl_mtx * mtx, xwsq_t tik)
{
        xwer_t rc;

        if ((NULL == mtx) || (0 == tik)) {
                rc = -ENILOBJD;
        } else {
                rc = XWOK;
        }
        return rc;
}
