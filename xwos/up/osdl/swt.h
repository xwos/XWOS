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

#ifndef __xwos_up_osdl_swt_h__
#define __xwos_up_osdl_swt_h__

#include <xwos/up/swt.h>

#define XWOSDL_SWT_FLAG_NULL            XWUP_SWT_FLAG_NULL
#define XWOSDL_SWT_FLAG_RESTART         XWUP_SWT_FLAG_RESTART

#define xwosdl_swt xwup_swt
typedef xwup_swt_f xwosdl_swt_f;

typedef struct {
        struct xwosdl_swt * swt;
        xwsq_t tik;
} xwosdl_swt_d;

#define XWOSDL_SWT_NILD ((xwosdl_swt_d){NULL, 0,})

static __xwcc_inline
xwer_t xwosdl_swt_init(struct xwosdl_swt * swt, xwsq_t flag)
{
        return xwup_swt_init(swt, flag);
}

static __xwcc_inline
xwer_t xwosdl_swt_fini(struct xwosdl_swt * swt)
{
        return xwup_swt_fini(swt);
}

xwer_t xwosdl_swt_create(xwosdl_swt_d * swtd, xwsq_t flag);

static __xwcc_inline
xwer_t xwosdl_swt_delete(struct xwosdl_swt * swt, xwsq_t tik)
{
        XWOS_UNUSED(tik);
        return xwup_swt_delete(swt);
}

xwer_t xwosdl_swt_acquire(struct xwosdl_swt * swt, xwsq_t tik);

xwer_t xwosdl_swt_release(struct xwosdl_swt * swt, xwsq_t tik);

static __xwcc_inline
xwsq_t xwosdl_swt_gettik(struct xwosdl_swt * swt)
{
        XWOS_UNUSED(swt);
        return 0;
}

xwer_t xwosdl_swt_grab(struct xwosdl_swt * swt);

xwer_t xwosdl_swt_put(struct xwosdl_swt * swt);

static __xwcc_inline
xwer_t xwosdl_swt_start(struct xwosdl_swt * swt, xwtm_t origin, xwtm_t period,
                        xwosdl_swt_f callback, void * arg)
{
        return xwup_swt_start(swt, origin, period, (xwup_swt_f)callback, arg);
}

static __xwcc_inline
xwer_t xwosdl_swt_stop(struct xwosdl_swt * swt)
{
        return xwup_swt_stop(swt);
}

#endif /* xwos/up/osdl/swt.h */
