/**
 * @file
 * @brief 玄武OS UP内核操作系统接口描述层：信号选择器
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __xwos_up_osdl_sync_sel_h__
#define __xwos_up_osdl_sync_sel_h__

#include <xwos/up/sync/evt.h>

#define xwosdl_sel xwup_evt
#define XWOSDL_SEL_MAXNUM       XWUP_EVT_MAXNUM

#define xwosdl_sel_declare_bitmap(name) xwup_evt_declare_bitmap(name)

static __xwcc_inline
xwer_t xwosdl_sel_init(struct xwosdl_sel * sel)
{
        return xwup_evt_init(sel, NULL, XWUP_EVT_TYPE_SEL);
}

static __xwcc_inline
xwer_t xwosdl_sel_destroy(struct xwosdl_sel * sel)
{
        return xwup_evt_destroy(sel);
}

static __xwcc_inline
xwer_t xwosdl_sel_create(struct xwosdl_sel ** selbuf)
{
        xwer_t rc;

        if (NULL != selbuf) {
                *selbuf = NULL;
                rc = xwup_evt_create(selbuf, NULL, XWUP_EVT_TYPE_SEL);
        } else {
                rc = -EFAULT;
        }
        return rc;
}

static __xwcc_inline
xwer_t xwosdl_sel_delete(struct xwosdl_sel * sel)
{
        return xwup_evt_delete(sel);
}

static __xwcc_inline
xwsq_t xwosdl_sel_gettik(struct xwosdl_sel * sel)
{
        XWOS_UNUSED(sel);
        return 0;
}

static __xwcc_inline
xwer_t xwosdl_sel_acquire(struct xwosdl_sel * sel, xwsq_t tik)
{
        XWOS_UNUSED(sel);
        XWOS_UNUSED(tik);
        return XWOK;
}

static __xwcc_inline
xwer_t xwosdl_sel_release(struct xwosdl_sel * sel, xwsq_t tik)
{
        XWOS_UNUSED(sel);
        XWOS_UNUSED(tik);
        return XWOK;
}

static __xwcc_inline
xwer_t xwosdl_sel_grab(struct xwosdl_sel * sel)
{
        XWOS_UNUSED(sel);
        return XWOK;
}

static __xwcc_inline
xwer_t xwosdl_sel_put(struct xwosdl_sel * sel)
{
        XWOS_UNUSED(sel);
        return XWOK;
}

static __xwcc_inline
xwer_t xwosdl_sel_bind(struct xwosdl_sel * src, struct xwosdl_sel * dst,
                       xwsq_t pos)
{
        return xwup_evt_bind(src, dst, pos);
}

static __xwcc_inline
xwer_t xwosdl_sel_unbind(struct xwosdl_sel * src, struct xwosdl_sel * dst)
{
        return xwup_evt_unbind(src, dst);
}

static __xwcc_inline
xwer_t xwosdl_sel_intr_all(struct xwosdl_sel * sel)
{
        return xwup_evt_intr_all(sel);
}

static __xwcc_inline
xwer_t xwosdl_sel_tryselect(struct xwosdl_sel * sel, xwbmp_t msk[], xwbmp_t trg[])
{
        return xwup_evt_tryselect(sel, msk, trg);
}

static __xwcc_inline
xwer_t xwosdl_sel_select(struct xwosdl_sel * sel, xwbmp_t msk[], xwbmp_t trg[])
{
        return xwup_evt_select(sel, msk, trg);
}

static __xwcc_inline
xwer_t xwosdl_sel_timedselect(struct xwosdl_sel * sel,
                              xwbmp_t msk[], xwbmp_t trg[],
                              xwtm_t * xwtm)
{
        return xwup_evt_timedselect(sel, msk, trg, xwtm);
}

#endif /* xwos/up/osdl/sync/sel.h */