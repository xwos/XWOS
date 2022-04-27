/**
 * @file
 * @brief 玄武OS MP内核操作系统接口描述层：信号选择器
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef __xwos_mp_osdl_sync_sel_h__
#define __xwos_mp_osdl_sync_sel_h__

#include <xwos/mp/sync/evt.h>

#define xwosdl_sel xwmp_evt

static __xwcc_inline
xwer_t xwosdl_sel_init(struct xwosdl_sel * sel, xwsz_t num,
                       xwbmp_t * bmp, xwbmp_t * msk)
{
        return xwmp_evt_init(sel, XWMP_EVT_TYPE_SEL, num, bmp, msk);
}

static __xwcc_inline
xwer_t xwosdl_sel_fini(struct xwosdl_sel * sel)
{
        return xwmp_evt_fini(sel);
}

static __xwcc_inline
xwer_t xwosdl_sel_create(struct xwosdl_sel ** selbuf, xwsz_t num)
{
        return xwmp_evt_create(selbuf, XWMP_EVT_TYPE_SEL, num);
}

static __xwcc_inline
xwer_t xwosdl_sel_delete(struct xwosdl_sel * sel)
{
        return xwmp_evt_delete(sel);
}

static __xwcc_inline
xwsq_t xwosdl_sel_gettik(struct xwosdl_sel * sel)
{
        xwsq_t tik;

        if (sel) {
                tik = sel->cond.synobj.xwobj.tik;
        } else {
                tik = 0;
        }
        return tik;
}

static __xwcc_inline
xwer_t xwosdl_sel_acquire(struct xwosdl_sel * sel, xwsq_t tik)
{
        return xwmp_evt_acquire(sel, tik);
}

static __xwcc_inline
xwer_t xwosdl_sel_release(struct xwosdl_sel * sel, xwsq_t tik)
{
        return xwmp_evt_release(sel, tik);
}

static __xwcc_inline
xwer_t xwosdl_sel_grab(struct xwosdl_sel * sel)
{
        XWOS_VALIDATE((sel), "nullptr", -EFAULT);
        return xwmp_evt_grab(sel);
}

static __xwcc_inline
xwer_t xwosdl_sel_put(struct xwosdl_sel * sel)
{
        XWOS_VALIDATE((sel), "nullptr", -EFAULT);
        return xwmp_evt_put(sel);
}

static __xwcc_inline
xwer_t xwosdl_sel_bind(struct xwosdl_sel * src, struct xwosdl_sel * dst, xwsq_t pos)
{
        return xwmp_evt_bind(src, dst, pos);
}

static __xwcc_inline
xwer_t xwosdl_sel_unbind(struct xwosdl_sel * src, struct xwosdl_sel * dst)
{
        return xwmp_evt_unbind(src, dst);
}

static __xwcc_inline
xwer_t xwosdl_sel_intr_all(struct xwosdl_sel * sel)
{
        return xwmp_evt_intr_all(sel);
}

static __xwcc_inline
xwer_t xwosdl_sel_get_num(struct xwosdl_sel * sel, xwsz_t * numbuf)
{
        return xwmp_evt_get_num(sel, numbuf);
}

static __xwcc_inline
xwer_t xwosdl_sel_select(struct xwosdl_sel * sel, xwbmp_t msk[], xwbmp_t trg[])
{
        return xwmp_sel_select(sel, msk, trg);
}

static __xwcc_inline
xwer_t xwosdl_sel_tryselect(struct xwosdl_sel * sel, xwbmp_t msk[], xwbmp_t trg[])
{
        return xwmp_sel_tryselect(sel, msk, trg);
}

static __xwcc_inline
xwer_t xwosdl_sel_select_to(struct xwosdl_sel * sel,
                            xwbmp_t msk[], xwbmp_t trg[],
                            xwtm_t to)
{
        return xwmp_sel_select_to(sel, msk, trg, to);
}

#endif /* xwos/mp/osdl/sync/sel.h */
