/**
 * @file
 * @brief XWOS MP内核操作系统接口描述层：互斥锁
 * @author
 * + 隐星曜 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#include <xwos/standard.h>
#include <xwos/mp/osdl/lock/mtx.h>

#if (1 == XWOSRULE_LOCK_MTX_CREATE_DELETE)
__xwmp_code
xwer_t xwosdl_mtx_create(xwosdl_mtx_d * mtxd, xwpr_t sprio)
{
        xwer_t rc;
        struct xwmp_mtx * mtx;

        XWOS_VALIDATE((mtxd), "nullptr", -EFAULT);

        rc = xwmp_mtx_create(&mtx, sprio);
        if (XWOK == rc) {
                mtxd->mtx = mtx;
                mtxd->tik = xwmp_mtx_get_tik(mtx);
        } else {
                *mtxd = XWOSDL_MTX_NILD;
        }
        return rc;
}
#endif
