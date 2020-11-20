/**
 * @file
 * @brief 玄武OS MP内核操作系统接口描述层：事件标志
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __xwos_mp_osdl_sync_flg_h__
#define __xwos_mp_osdl_sync_flg_h__

#include <xwos/mp/osdl/sync/sel.h>
#include <xwos/mp/sync/evt.h>

#define xwosdl_flg xwmp_evt

#define XWOSDL_FLG_TRIGGER_SET_ALL      XWMP_EVT_TRIGGER_SET_ALL
#define XWOSDL_FLG_TRIGGER_SET_ANY      XWMP_EVT_TRIGGER_SET_ANY
#define XWOSDL_FLG_TRIGGER_CLR_ALL      XWMP_EVT_TRIGGER_CLR_ALL
#define XWOSDL_FLG_TRIGGER_CLR_ANY      XWMP_EVT_TRIGGER_CLR_ANY
#define XWOSDL_FLG_TRIGGER_TGL_ALL      XWMP_EVT_TRIGGER_TGL_ALL
#define XWOSDL_FLG_TRIGGER_TGL_ANY      XWMP_EVT_TRIGGER_TGL_ANY

#define XWOSDL_FLG_ACTION_NONE          XWMP_EVT_ACTION_NONE
#define XWOSDL_FLG_ACTION_CONSUMPTION   XWMP_EVT_ACTION_CONSUMPTION

#define XWOSDL_FLG_MAXNUM               XWMP_EVT_MAXNUM

#define xwosdl_flg_declare_bitmap(name) xwmp_evt_declare_bitmap(name)

static __xwcc_inline
xwer_t xwosdl_flg_init(struct xwosdl_flg * flg, xwbmp_t initval[])
{
        return xwmp_evt_init(flg, initval, XWMP_EVT_TYPE_FLG);
}

static __xwcc_inline
xwer_t xwosdl_flg_destroy(struct xwosdl_flg * flg)
{
        return xwmp_evt_destroy(flg);
}

static __xwcc_inline
xwer_t xwosdl_flg_create(struct xwosdl_flg ** flgp, xwbmp_t initval[])
{
        xwer_t rc;

        if (NULL != flgp) {
                *flgp = NULL;
                rc = xwmp_evt_create(flgp, initval, XWMP_EVT_TYPE_FLG);
        } else {
                rc = -EFAULT;
        }
        return rc;
}

static __xwcc_inline
xwer_t xwosdl_flg_delete(struct xwosdl_flg * flg)
{
        return xwmp_evt_delete(flg);
}

static __xwcc_inline
xwer_t xwosdl_flg_bind(struct xwosdl_flg * flg, struct xwosdl_sel * sel, xwsq_t pos)
{
        return xwmp_evt_bind(flg, sel, pos);
}

static __xwcc_inline
xwer_t xwosdl_flg_unbind(struct xwosdl_flg * flg, struct xwosdl_sel * sel)
{
        return xwmp_evt_unbind(flg, sel);
}

static __xwcc_inline
xwer_t xwosdl_flg_intr_all(struct xwosdl_flg * flg)
{
        return xwmp_evt_intr_all(flg);
}

static __xwcc_inline
xwer_t xwosdl_flg_s1m(struct xwosdl_flg * flg, xwbmp_t msk[])
{
        return xwmp_evt_s1m(flg, msk);
}

static __xwcc_inline
xwer_t xwosdl_flg_s1i(struct xwosdl_flg * flg, xwsq_t pos)
{
        return xwmp_evt_s1i(flg, pos);
}

static __xwcc_inline
xwer_t xwosdl_flg_c0m(struct xwosdl_flg * flg, xwbmp_t msk[])
{
        return xwmp_evt_c0m(flg, msk);
}

static __xwcc_inline
xwer_t xwosdl_flg_c0i(struct xwosdl_flg * flg, xwsq_t pos)
{
        return xwmp_evt_c0i(flg, pos);
}

static __xwcc_inline
xwer_t xwosdl_flg_x1m(struct xwosdl_flg * flg, xwbmp_t msk[])
{
        return xwmp_evt_x1m(flg, msk);
}

static __xwcc_inline
xwer_t xwosdl_flg_x1i(struct xwosdl_flg * flg, xwsq_t pos)
{
        return xwmp_evt_x1i(flg, pos);
}

static __xwcc_inline
xwer_t xwosdl_flg_read(struct xwosdl_flg * flg, xwbmp_t out[])
{
        return xwmp_evt_read(flg, out);
}

static __xwcc_inline
xwer_t xwosdl_flg_trywait(struct xwosdl_flg * flg, xwsq_t trigger, xwsq_t action,
                          xwbmp_t origin[], xwbmp_t msk[])
{
        return xwmp_evt_trywait(flg, trigger, action, origin, msk);
}

static __xwcc_inline
xwer_t xwosdl_flg_wait(struct xwosdl_flg * flg, xwsq_t trigger, xwsq_t action,
                       xwbmp_t origin[], xwbmp_t msk[])
{
        return xwmp_evt_wait(flg, trigger, action, origin, msk);
}

static __xwcc_inline
xwer_t xwosdl_flg_timedwait(struct xwosdl_flg * flg, xwsq_t trigger, xwsq_t action,
                            xwbmp_t origin[], xwbmp_t msk[],
                            xwtm_t * xwtm)
{
        return xwmp_evt_timedwait(flg, trigger, action, origin, msk, xwtm);
}

#endif /* xwos/mp/osdl/sync/flg.h */
