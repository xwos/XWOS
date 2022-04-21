/**
 * @file
 * @brief 玄武OS MP内核操作系统接口描述层：线程
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#include <xwos/standard.h>
#include <xwos/mp/osdl/thd.h>

__xwmp_code
xwer_t xwosdl_thd_init(struct xwosdl_thd * thd, xwosdl_thd_d * thdd,
                       const struct xwosdl_thd_attr * inattr,
                       xwosdl_thd_f mainfunc, void * arg)
{
        xwer_t rc;

        XWOS_VALIDATE((NULL != thd), "nullptr", -EFAULT);
        XWOS_VALIDATE((NULL != thdd), "nullptr", -EFAULT);
        XWOS_VALIDATE((NULL != inattr), "nullptr", -EFAULT);
        XWOS_VALIDATE((NULL != inattr->stack), "nullptr", -EFAULT);
        XWOS_VALIDATE((mainfunc), "nullptr", -EFAULT);

        rc = xwmp_thd_init(thd, inattr, mainfunc, arg);
        if (XWOK == rc) {
                thdd->thd = thd;
                thdd->tik = thd->xwobj.tik;
        } else {
                *thdd = XWOSDL_THD_NILD;
        }
        return rc;
}

__xwmp_code
xwer_t xwosdl_thd_create(xwosdl_thd_d * thdd,
                         const struct xwosdl_thd_attr * inattr,
                         xwosdl_thd_f mainfunc, void * arg)
{
        xwer_t rc;
        struct xwosdl_thd * thd;

        XWOS_VALIDATE((thdd), "nullptr", -EFAULT);
        XWOS_VALIDATE((mainfunc), "nullptr", -EFAULT);

        rc = xwmp_thd_create(&thd, inattr, mainfunc, arg);
        if (XWOK == rc) {
                thdd->thd = thd;
                thdd->tik = thd->xwobj.tik;
        } else {
                *thdd = XWOSDL_THD_NILD;
        }
        return rc;
}

__xwmp_code
xwer_t xwosdl_thd_quit(struct xwosdl_thd * thd, xwsq_t tik)
{
        xwer_t rc;

        XWOS_VALIDATE((NULL != thd), "nild", -EBADOBJD);
        XWOS_VALIDATE((0 != tik), "nild", -EBADOBJD);

        rc = xwmp_thd_acquire(thd, tik);
        if (XWOK == rc) {
                xwmp_thd_quit(thd);
                xwmp_thd_put(thd);
        }
        return rc;
}

__xwmp_code
xwer_t xwosdl_thd_join(struct xwosdl_thd * thd, xwsq_t tik, xwer_t * trc)
{
        xwer_t rc;

        XWOS_VALIDATE((NULL != thd), "nild", -EBADOBJD);
        XWOS_VALIDATE((0 != tik), "nild", -EBADOBJD);

        rc = xwmp_thd_acquire(thd, tik);
        if (XWOK == rc) {
                rc = xwmp_thd_join(thd, trc);
                xwmp_thd_put(thd);
        }
        return rc;
}

__xwmp_code
xwer_t xwosdl_thd_stop(struct xwosdl_thd * thd, xwsq_t tik, xwer_t * trc)
{
        xwer_t rc;

        XWOS_VALIDATE((NULL != thd), "nild", -EBADOBJD);
        XWOS_VALIDATE((0 != tik), "nild", -EBADOBJD);

        rc = xwmp_thd_acquire(thd, tik);
        if (XWOK == rc) {
                xwmp_thd_quit(thd);
                rc = xwmp_thd_join(thd, trc);
                xwmp_thd_put(thd);
        }
        return rc;
}

__xwmp_code
xwer_t xwosdl_thd_detach(struct xwosdl_thd * thd, xwsq_t tik)
{
        xwer_t rc;

        XWOS_VALIDATE((NULL != thd), "nild", -EBADOBJD);
        XWOS_VALIDATE((0 != tik), "nild", -EBADOBJD);

        rc = xwmp_thd_acquire(thd, tik);
        if (XWOK == rc) {
                rc = xwmp_thd_detach(thd);
                xwmp_thd_put(thd);
        }
        return rc;
}

__xwmp_code
xwer_t xwosdl_thd_migrate(struct xwosdl_thd * thd, xwsq_t tik, xwid_t dstcpu)
{

        xwer_t rc;

        XWOS_VALIDATE((NULL != thd), "nild", -EBADOBJD);
        XWOS_VALIDATE((0 != tik), "nild", -EBADOBJD);

        rc = xwmp_thd_acquire(thd, tik);
        if (XWOK == rc) {
                rc = xwmp_thd_migrate(thd, dstcpu);
                xwmp_thd_put(thd);
        }
        return rc;
}

__xwmp_code
xwosdl_thd_d xwosdl_cthd_self(void)
{
        xwosdl_thd_d thdd;

        thdd.thd = xwmp_skd_get_cthd_lc();
        thdd.tik = thdd.thd->xwobj.tik;
        return thdd;
}

#if defined(XWMPCFG_SKD_THD_LOCAL_DATA_NUM) && (XWMPCFG_SKD_THD_LOCAL_DATA_NUM > 0U)
__xwmp_code
xwer_t xwosdl_thd_set_data(struct xwosdl_thd * thd, xwsq_t tik,
                           xwsq_t pos, void * data)
{
        xwer_t rc;

        XWOS_VALIDATE((NULL != thd), "nild", -EBADOBJD);
        XWOS_VALIDATE((0 != tik), "nild", -EBADOBJD);

        rc = xwmp_thd_acquire(thd, tik);
        if (XWOK == rc) {
                rc = xwmp_thd_set_data(thd, pos, data);
                xwmp_thd_put(thd);
        }
        return rc;
}

__xwmp_code
xwer_t xwosdl_thd_get_data(struct xwosdl_thd * thd, xwsq_t tik,
                           xwsq_t pos, void ** databuf)
{
        xwer_t rc;

        XWOS_VALIDATE((NULL != thd), "nild", -EBADOBJD);
        XWOS_VALIDATE((0 != tik), "nild", -EBADOBJD);

        rc = xwmp_thd_acquire(thd, tik);
        if (XWOK == rc) {
                rc = xwmp_thd_get_data(thd, pos, databuf);
                xwmp_thd_put(thd);
        }
        return rc;
}
#endif
