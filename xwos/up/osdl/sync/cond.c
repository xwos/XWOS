/**
 * @file
 * @brief 玄武OS UP内核操作系统接口描述层：条件量
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#include <xwos/standard.h>
#include <xwos/up/osdl/sync/cond.h>

__xwup_code
xwer_t xwosdl_cond_create(xwosdl_cond_d * condd)
{
        xwer_t rc;
        struct xwup_cond * cond;

        XWOS_VALIDATE((condd), "nullptr", -EFAULT);

        rc = xwup_cond_create(&cond);
        if (XWOK == rc) {
                condd->cond = cond;
                condd->tik = 0;
        } else {
                *condd = XWOSDL_COND_NILD;
        }
        return rc;
}

__xwup_code
xwer_t xwosdl_cond_acquire(struct xwosdl_cond * cond, xwsq_t tik)
{
        xwer_t rc;

        XWOS_UNUSED(tik);

        if (NULL == cond) {
                rc = -ENILOBJD;
        } else {
                rc = XWOK;
        }
        return rc;
}

__xwup_code
xwer_t xwosdl_cond_release(struct xwosdl_cond * cond, xwsq_t tik)
{
        xwer_t rc;

        XWOS_UNUSED(tik);

        if (NULL == cond) {
                rc = -ENILOBJD;
        } else {
                rc = XWOK;
        }
        return rc;
}
