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
        XWOS_VALIDATE((inattr->stack_size >= XWMMCFG_STACK_SIZE_MIN),
                      "nullptr", -ESIZE);
        XWOS_VALIDATE(((inattr->stack_size & (XWMMCFG_STACK_ALIGNMENT - 1)) == 0),
                      "nullptr", -EALIGN);

        rc = xwup_thd_init(thd, inattr, mainfunc, arg);
        if (XWOK == rc) {
                thdd->thd = thd;
                thdd->tik = thd->xwobj.tik;
        } else {
                *thdd = XWOSDL_THD_NILD;
        }
        return rc;
}

__xwup_code
xwer_t xwosdl_thd_grab(struct xwosdl_thd * thd)
{
        XWOS_VALIDATE((thd), "nullptr", -EFAULT);
        return xwup_thd_grab(thd);
}

__xwup_code
xwer_t xwosdl_thd_put(struct xwosdl_thd * thd)
{
        XWOS_VALIDATE((thd), "nullptr", -EFAULT);
        return xwup_thd_put(thd);
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
                thdd->tik = thd->xwobj.tik;
        } else {
                *thdd = XWOSDL_THD_NILD;
        }
        return rc;
}

__xwup_code
xwer_t xwosdl_thd_quit(struct xwosdl_thd * thd, xwsq_t tik)
{
        xwer_t rc;

        rc = xwup_thd_acquire(thd, tik);
        if (XWOK == rc) {
                xwup_thd_quit(thd);
                xwup_thd_put(thd);
        }
        return rc;
}

__xwup_code
xwer_t xwosdl_thd_join(struct xwosdl_thd * thd, xwsq_t tik, xwer_t * trc)
{
        xwer_t rc;

        rc = xwup_thd_acquire(thd, tik);
        if (XWOK == rc) {
                rc = xwup_thd_join(thd, trc);
                xwup_thd_put(thd);
        }
        return rc;
}

__xwup_code
xwer_t xwosdl_thd_stop(struct xwosdl_thd * thd, xwsq_t tik, xwer_t * trc)
{
        xwer_t rc;

        rc = xwup_thd_acquire(thd, tik);
        if (XWOK == rc) {
                xwup_thd_quit(thd);
                rc = xwup_thd_join(thd, trc);
                xwup_thd_put(thd);
        }
        return rc;
}

__xwup_code
xwer_t xwosdl_thd_detach(struct xwosdl_thd * thd, xwsq_t tik)
{
        xwer_t rc;

        rc = xwup_thd_acquire(thd, tik);
        if (XWOK == rc) {
                rc = xwup_thd_detach(thd);
                xwup_thd_put(thd);
        }
        return rc;
}

xwosdl_thd_d xwosdl_cthd_self(void)
{
        xwosdl_thd_d thdd;

        thdd.thd = xwup_skd_get_cthd_lc();
        thdd.tik = thdd.thd->xwobj.tik;;
        return thdd;
}
