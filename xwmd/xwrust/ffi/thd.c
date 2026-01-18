/**
 * @file
 * @brief XWRUST FFI：线程
 * @author
 * + 隐星曜 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#include <xwos/standard.h>
#include <xwos/osal/thd.h>


xwsz_t xwrustffi_thd_stack_size_default(void)
{
        return XWOS_STACK_SIZE_DEFAULT;
}

void xwrustffi_thd_attr_init(struct xwos_thd_attr * attr)
{
        xwos_thd_attr_init(attr);
}

const char xwrustffi_thd_name[] = "xwrust.xwos.thd";

xwer_t xwrustffi_thd_create(struct xwos_thd ** thd, xwsq_t * tik,
                            struct xwos_thd_attr * attr,
                            xwos_thd_f mainfunc, void * arg)
{
        xwos_thd_d thdd;
        xwer_t rc;

        attr->name = xwrustffi_thd_name;
        rc = xwos_thd_create(&thdd, attr, mainfunc, arg);
        *thd = thdd.thd;
        *tik = thdd.tik;
        return rc;
}

xwer_t xwrustffi_thd_init(struct xwos_thd * thd, xwsq_t * tik,
                          struct xwos_thd_attr * attr,
                          xwos_thd_f mainfunc, void * arg)
{
        xwos_thd_d thdd;
        xwer_t rc;

        attr->name = xwrustffi_thd_name;
        rc = xwos_thd_init(thd, &thdd, attr, mainfunc, arg);
        *tik = thdd.tik;
        return rc;
}

xwer_t xwrustffi_thd_acquire(struct xwos_thd * thd, xwsq_t tik)
{
        return xwos_thd_acquire((xwos_thd_d){thd, tik});
}

xwer_t xwrustffi_thd_release(struct xwos_thd * thd, xwsq_t tik)
{
        return xwos_thd_release((xwos_thd_d){thd, tik});
}

xwer_t xwrustffi_thd_intr(struct xwos_thd * thd, xwsq_t tik)
{
        return xwos_thd_intr((xwos_thd_d){thd, tik});
}

xwer_t xwrustffi_thd_quit(struct xwos_thd * thd, xwsq_t tik)
{
        return xwos_thd_quit((xwos_thd_d){thd, tik});
}

xwer_t xwrustffi_thd_stop(struct xwos_thd * thd, xwsq_t tik, xwer_t * trc)
{
        return xwos_thd_stop((xwos_thd_d){thd, tik}, trc);
}

xwer_t xwrustffi_thd_join(struct xwos_thd * thd, xwsq_t tik, xwer_t * trc)
{
        return xwos_thd_join((xwos_thd_d){thd, tik}, trc);
}

xwer_t xwrustffi_thd_detach(struct xwos_thd * thd, xwsq_t tik)
{
        return xwos_thd_detach((xwos_thd_d){thd, tik});
}

xwer_t xwrustffi_thd_migrate(struct xwos_thd * thd, xwsq_t tik, xwid_t dstcpu)
{
        return xwos_thd_migrate((xwos_thd_d){thd, tik}, dstcpu);
}

void xwrustffi_cthd_self(struct xwos_thd ** thd, xwsq_t * tik)
{
        xwos_thd_d thdd;

        thdd = xwos_cthd_self();
        *thd = thdd.thd;
        *tik = thdd.tik;
}

void xwrustffi_cthd_yield(void)
{
        xwos_cthd_yield();
}

void xwrustffi_cthd_exit(xwer_t rc)
{
        xwos_cthd_exit(rc);
}

bool xwrustffi_cthd_shld_frz(void)
{
        return xwos_cthd_shld_frz();
}

bool xwrustffi_cthd_shld_stop(void)
{
        return xwos_cthd_shld_stop();
}

bool xwrustffi_cthd_frz_shld_stop(bool * frozen)
{
        return xwos_cthd_frz_shld_stop(frozen);
}

xwer_t xwrustffi_cthd_sleep(xwtm_t xwtm)
{
        return xwos_cthd_sleep(xwtm);
}

xwer_t xwrustffi_cthd_sleep_to(xwtm_t to)
{
        return xwos_cthd_sleep_to(to);
}

xwer_t xwrustffi_cthd_sleep_from(xwtm_t * origin, xwtm_t inc)
{
        return xwos_cthd_sleep_from(origin, inc);
}

xwer_t xwrustffi_cthd_freeze(void)
{
        return xwos_cthd_freeze();
}
