/**
 * @file
 * @brief 玄武OS MP内核操作系统接口描述层：软件定时器
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __xwos_mp_osdl_swt_h__
#define __xwos_mp_osdl_swt_h__

#include <xwos/mp/swt.h>

#define XWOSDL_SWT_FLAG_NULL        XWMP_SWT_FLAG_NULL
#define XWOSDL_SWT_FLAG_RESTART     XWMP_SWT_FLAG_RESTART
#define XWOSDL_SWT_FLAG_AUTORM      XWMP_SWT_FLAG_AUTORM

#define xwosdl_swt xwmp_swt
typedef xwmp_swt_f xwosdl_swt_f;

static __xwcc_inline
xwer_t xwosdl_swt_init(struct xwosdl_swt * swt, const char * name, xwsq_t flag)
{
        return xwmp_swt_init(swt, name, flag);
}

static __xwcc_inline
xwer_t xwosdl_swt_destroy(struct xwosdl_swt * swt)
{
        return xwmp_swt_destroy(swt);
}

static __xwcc_inline
xwer_t xwosdl_swt_create(struct xwosdl_swt ** swtbuf, const char * name, xwsq_t flag)
{
        xwer_t rc;

        XWOS_VALIDATE((swtbuf), "nullptr", -EFAULT);
        *swtbuf = NULL;
        rc = xwmp_swt_create(swtbuf, name, flag);
        return rc;
}

static __xwcc_inline
xwer_t xwosdl_swt_delete(struct xwosdl_swt * swt)
{
        return xwmp_swt_delete(swt);
}

static __xwcc_inline
xwsq_t xwosdl_swt_gettik(struct xwosdl_swt * swt)
{
        xwsq_t tik;

        if (swt) {
                tik = swt->xwobj.tik;
        } else {
                tik = 0;
        }
        return tik;
}

static __xwcc_inline
xwer_t xwosdl_swt_acquire(struct xwosdl_swt * swt, xwsq_t tik)
{
        return xwmp_swt_acquire(swt, tik);
}

static __xwcc_inline
xwer_t xwosdl_swt_release(struct xwosdl_swt * swt, xwsq_t tik)
{
        return xwmp_swt_release(swt, tik);
}

static __xwcc_inline
xwer_t xwosdl_swt_grab(struct xwosdl_swt * swt)
{
        return xwmp_swt_grab(swt);
}

static __xwcc_inline
xwer_t xwosdl_swt_put(struct xwosdl_swt * swt)
{
        return xwmp_swt_put(swt);
}

static __xwcc_inline
xwer_t xwosdl_swt_start(struct xwosdl_swt * swt, xwtm_t base, xwtm_t period,
                        xwosdl_swt_f callback, void * arg)
{
        return xwmp_swt_start(swt, base, period, (xwmp_swt_f)callback, arg);
}

static __xwcc_inline
xwer_t xwosdl_swt_stop(struct xwosdl_swt * swt)
{
        return xwmp_swt_stop(swt);
}

#endif /* xwos/mp/osdl/swt.h */
