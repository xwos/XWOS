/**
 * @file
 * @brief 玄武OS UP内核操作系统接口描述层：事件标志
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __xwos_up_osdl_sync_flg_h__
#define __xwos_up_osdl_sync_flg_h__

#include <xwos/up/osdl/sync/sel.h>
#include <xwos/up/sync/evt.h>

#define xwosdl_flg xwup_evt

#define XWOSDL_FLG_TRIGGER_SET_ALL      XWUP_EVT_TRIGGER_SET_ALL
#define XWOSDL_FLG_TRIGGER_SET_ANY      XWUP_EVT_TRIGGER_SET_ANY
#define XWOSDL_FLG_TRIGGER_CLR_ALL      XWUP_EVT_TRIGGER_CLR_ALL
#define XWOSDL_FLG_TRIGGER_CLR_ANY      XWUP_EVT_TRIGGER_CLR_ANY
#define XWOSDL_FLG_TRIGGER_TGL_ALL      XWUP_EVT_TRIGGER_TGL_ALL
#define XWOSDL_FLG_TRIGGER_TGL_ANY      XWUP_EVT_TRIGGER_TGL_ANY

#define XWOSDL_FLG_ACTION_NONE          XWUP_EVT_ACTION_NONE
#define XWOSDL_FLG_ACTION_CONSUMPTION   XWUP_EVT_ACTION_CONSUMPTION

#define XWOSDL_FLG_MAXNUM               XWUP_EVT_MAXNUM

#define xwosdl_flg_declare_bitmap(name) xwup_evt_declare_bitmap(name)

static __xwcc_inline
xwer_t xwosdl_flg_init(struct xwosdl_flg * flg, xwbmp_t initval[])
{
        return xwup_evt_init(flg, initval, XWUP_EVT_TYPE_FLG);
}

static __xwcc_inline
xwer_t xwosdl_flg_destroy(struct xwosdl_flg * flg)
{
        return xwup_evt_destroy(flg);
}

static __xwcc_inline
xwer_t xwosdl_flg_create(struct xwosdl_flg ** flgbuf, xwbmp_t initval[])
{
        xwer_t rc;

        if (NULL != flgbuf) {
                *flgbuf = NULL;
                rc = xwup_evt_create(flgbuf, initval, XWUP_EVT_TYPE_FLG);
        } else {
                rc = -EFAULT;
        }
        return rc;
}

static __xwcc_inline
xwer_t xwosdl_flg_delete(struct xwosdl_flg * flg)
{
        return xwup_evt_delete(flg);
}

static __xwcc_inline
xwsq_t xwosdl_flg_gettik(struct xwosdl_flg * flg)
{
        XWOS_UNUSED(flg);
        return 0;
}

static __xwcc_inline
xwer_t xwosdl_flg_acquire(struct xwosdl_flg * flg, xwsq_t tik)
{
        XWOS_UNUSED(flg);
        XWOS_UNUSED(tik);
        return XWOK;
}

static __xwcc_inline
xwer_t xwosdl_flg_release(struct xwosdl_flg * flg, xwsq_t tik)
{
        XWOS_UNUSED(flg);
        XWOS_UNUSED(tik);
        return XWOK;
}

static __xwcc_inline
xwer_t xwosdl_flg_grab(struct xwosdl_flg * flg)
{
        XWOS_UNUSED(flg);
        return XWOK;
}

static __xwcc_inline
xwer_t xwosdl_flg_put(struct xwosdl_flg * flg)
{
        XWOS_UNUSED(flg);
        return XWOK;
}

static __xwcc_inline
xwer_t xwosdl_flg_bind(struct xwosdl_flg * flg, struct xwosdl_sel * sel,
                       xwsq_t pos)
{
        return xwup_evt_bind(flg, sel, pos);
}

static __xwcc_inline
xwer_t xwosdl_flg_unbind(struct xwosdl_flg * flg, struct xwosdl_sel * sel)
{
        return xwup_evt_unbind(flg, sel);
}

static __xwcc_inline
xwer_t xwosdl_flg_intr_all(struct xwosdl_flg * flg)
{
        return xwup_evt_intr_all(flg);
}

static __xwcc_inline
xwer_t xwosdl_flg_s1m(struct xwosdl_flg * flg, xwbmp_t msk[])
{
        return xwup_evt_s1m(flg, msk);
}

static __xwcc_inline
xwer_t xwosdl_flg_s1i(struct xwosdl_flg * flg, xwsq_t pos)
{
        return xwup_evt_s1i(flg, pos);
}

static __xwcc_inline
xwer_t xwosdl_flg_c0m(struct xwosdl_flg * flg, xwbmp_t msk[])
{
        return xwup_evt_c0m(flg, msk);
}

static __xwcc_inline
xwer_t xwosdl_flg_c0i(struct xwosdl_flg * flg, xwsq_t pos)
{
        return xwup_evt_c0i(flg, pos);
}

static __xwcc_inline
xwer_t xwosdl_flg_x1m(struct xwosdl_flg * flg, xwbmp_t msk[])
{
        return xwup_evt_x1m(flg, msk);
}

static __xwcc_inline
xwer_t xwosdl_flg_x1i(struct xwosdl_flg * flg, xwsq_t pos)
{
        return xwup_evt_x1i(flg, pos);
}

static __xwcc_inline
xwer_t xwosdl_flg_read(struct xwosdl_flg * flg, xwbmp_t out[])
{
        return xwup_evt_read(flg, out);
}

static __xwcc_inline
xwer_t xwosdl_flg_trywait(struct xwosdl_flg * flg,
                          xwsq_t trigger, xwsq_t action,
                          xwbmp_t origin[], xwbmp_t msk[])
{
        return xwup_evt_trywait(flg, trigger, action, origin, msk);
}

static __xwcc_inline
xwer_t xwosdl_flg_wait(struct xwosdl_flg * flg,
                       xwsq_t trigger, xwsq_t action,
                       xwbmp_t origin[], xwbmp_t msk[])
{
        return xwup_evt_wait(flg, trigger, action, origin, msk);
}

static __xwcc_inline
xwer_t xwosdl_flg_timedwait(struct xwosdl_flg * flg,
                            xwsq_t trigger, xwsq_t action,
                            xwbmp_t origin[], xwbmp_t msk[],
                            xwtm_t * xwtm)
{
        return xwup_evt_timedwait(flg, trigger, action, origin, msk, xwtm);
}

#endif /* xwos/up/osdl/sync/flg.h */
