/**
 * @file
 * @brief XWRUST FFI：线程
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
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

xwer_t xwrustffi_thd_create(struct xwos_thd ** thdbuf,
                            const struct xwos_thd_attr * attr,
                            xwos_thd_f mainfunc, void * arg)
{
        return xwos_thd_create(thdbuf, attr, mainfunc, arg);
}

xwsq_t xwrustffi_thd_gettik(struct xwos_thd * thd)
{
        return xwos_thd_gettik(thd);
}

xwer_t xwrustffi_thd_acquire(struct xwos_thd * thd, xwsq_t tik)
{
        xwos_thd_d thdd = {
                .thd = thd,
                .tik = tik,
        };
        return xwos_thd_acquire(thdd);
}

xwer_t xwrustffi_thd_release(struct xwos_thd * thd, xwsq_t tik)
{
        xwos_thd_d thdd = {
                .thd = thd,
                .tik = tik,
        };
        return xwos_thd_release(thdd);
}

xwer_t xwrustffi_thd_quit(struct xwos_thd * thd)
{
        return xwos_thd_quit(thd);
}

xwer_t xwrustffi_thd_stop(struct xwos_thd * thd, xwer_t * trc)
{
        return xwos_thd_stop(thd, trc);
}

xwer_t xwrustffi_thd_join(struct xwos_thd * thd, xwer_t * trc)
{
        return xwos_thd_join(thd, trc);
}

xwer_t xwrustffi_thd_detach(struct xwos_thd * thd)
{
        return xwos_thd_detach(thd);
}

xwer_t xwrustffi_thd_migrate(struct xwos_thd * thd, xwid_t dstcpu)
{
        return xwos_thd_migrate(thd, dstcpu);
}

struct xwos_thd * xwrustffi_cthd_self(void)
{
        return xwos_cthd_self();
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
