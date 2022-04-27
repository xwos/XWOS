/**
 * @file
 * @brief 玄武OS UP内核操作系统接口描述层：线程
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#include <xwos/standard.h>
#include <xwos/up/osdl/thd.h>

__xwup_code
xwer_t xwosdl_thd_init(struct xwosdl_thd * thd, xwosdl_thd_d * thdd,
                       const struct xwosdl_thd_attr * inattr,
                       xwosdl_thd_f mainfunc, void * arg)
{
        xwer_t rc;

        XWOS_VALIDATE((NULL != thd), "nullptr", -EFAULT);
        XWOS_VALIDATE((NULL != thdd), "nullptr", -EFAULT);
        XWOS_VALIDATE((NULL != inattr), "nullptr", -EFAULT);
        XWOS_VALIDATE((NULL != inattr->stack), "nullptr", -EFAULT);

        rc = xwup_thd_init(thd, inattr, mainfunc, arg);
        if (XWOK == rc) {
                thdd->thd = thd;
                thdd->tik = 0;
        } else {
                *thdd = XWOSDL_THD_NILD;
        }
        return rc;
}

__xwup_code
xwer_t xwosdl_thd_create(xwosdl_thd_d * thdd,
                         const struct xwosdl_thd_attr * attr,
                         xwosdl_thd_f mainfunc, void * arg)
{
        xwer_t rc;
        struct xwosdl_thd * thd;

        XWOS_VALIDATE((thdd), "nullptr", -EFAULT);

        rc = xwup_thd_create(&thd, attr, mainfunc, arg);
        if (XWOK == rc) {
                thdd->thd = thd;
                thdd->tik = 0;
        } else {
                *thdd = XWOSDL_THD_NILD;
        }
        return rc;
}

__xwup_code
xwer_t xwosdl_thd_acquire(struct xwosdl_thd * thd, xwsq_t tik)
{
        xwer_t rc;

        XWOS_UNUSED(tik);

        if (NULL == thd) {
                rc = -ENILOBJD;
        } else {
                rc = XWOK;
        }
        return rc;
}

__xwup_code
xwer_t xwosdl_thd_release(struct xwosdl_thd * thd, xwsq_t tik)
{
        xwer_t rc;

        XWOS_UNUSED(tik);

        if (NULL == thd) {
                rc = -ENILOBJD;
        } else {
                rc = XWOK;
        }
        return rc;
}

__xwup_code
xwer_t xwosdl_thd_stop(struct xwosdl_thd * thd, xwsq_t tik, xwer_t * trc)
{
        xwer_t rc;

        XWOS_VALIDATE((NULL != thd), "nild", -ENILOBJD);
        XWOS_UNUSED(tik);

        xwup_thd_quit(thd);
        rc = xwup_thd_join(thd, trc);
        return rc;
}

xwosdl_thd_d xwosdl_cthd_self(void)
{
        xwosdl_thd_d thdd;

        thdd.thd = xwup_skd_get_cthd_lc();
        thdd.tik = 0;
        return thdd;
}
