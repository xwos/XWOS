/**
 * @file
 * @brief XWOS UP内核操作系统接口描述层：信号选择器
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef __xwos_up_osdl_sync_sel_h__
#define __xwos_up_osdl_sync_sel_h__

#include <xwos/up/irq.h>
#include <xwos/up/sync/evt.h>

#define xwosdl_sel xwup_evt

typedef struct {
        struct xwosdl_sel * sel;
        xwsq_t tik;
} xwosdl_sel_d;

#define XWOSDL_SEL_NILD ((xwosdl_sel_d){NULL, 0,})

static __xwcc_inline
xwer_t xwosdl_sel_init(struct xwosdl_sel * sel, xwsz_t num,
                       xwbmp_t * bmp, xwbmp_t * msk)
{
        XWOS_VALIDATE((sel), "nullptr", -EFAULT);
        XWOS_VALIDATE((bmp), "nullptr", -EFAULT);
        XWOS_VALIDATE((msk), "nullptr", -EFAULT);

        return xwup_evt_init(sel, XWUP_EVT_TYPE_SEL, num, bmp, msk);
}

static __xwcc_inline
xwer_t xwosdl_sel_fini(struct xwosdl_sel * sel)
{
        XWOS_VALIDATE((sel), "nullptr", -EFAULT);

        return xwup_evt_fini(sel);
}

static __xwcc_inline
xwer_t xwosdl_sel_grab(struct xwosdl_sel * sel)
{
        XWOS_VALIDATE((sel), "nullptr", -EFAULT);

        return xwup_evt_grab(sel);
}

static __xwcc_inline
xwer_t xwosdl_sel_put(struct xwosdl_sel * sel)
{
        XWOS_VALIDATE((sel), "nullptr", -EFAULT);

        return xwup_evt_put(sel);
}

xwer_t xwosdl_sel_create(xwosdl_sel_d * seld, xwsz_t num);

static __xwcc_inline
xwer_t xwosdl_sel_delete(struct xwosdl_sel * sel, xwsq_t tik)
{
        return xwup_evt_delete(sel, tik);
}

static __xwcc_inline
xwer_t xwosdl_sel_acquire(struct xwosdl_sel * sel, xwsq_t tik)
{
        return xwup_evt_acquire(sel, tik);
}

static __xwcc_inline
xwer_t xwosdl_sel_release(struct xwosdl_sel * sel, xwsq_t tik)
{
        return xwup_evt_release(sel, tik);
}

static __xwcc_inline
xwsq_t xwosdl_sel_get_tik(struct xwosdl_sel * sel)
{
        return (NULL != sel) ? sel->cond.synobj.xwobj.tik : (xwsq_t)0;
}

static __xwcc_inline
xwer_t xwosdl_sel_bind(struct xwosdl_sel * src, struct xwosdl_sel * dst,
                       xwsq_t pos)
{
        XWOS_VALIDATE((src), "nullptr", -EFAULT);
        XWOS_VALIDATE((dst), "nullptr", -EFAULT);
        XWOS_VALIDATE((dst->type == XWUP_EVT_TYPE_SEL), "type-error", -ETYPE);

        return xwup_evt_bind(src, dst, pos);
}

static __xwcc_inline
xwer_t xwosdl_sel_unbind(struct xwosdl_sel * src, struct xwosdl_sel * dst)
{
        XWOS_VALIDATE((src), "nullptr", -EFAULT);
        XWOS_VALIDATE((dst), "nullptr", -EFAULT);
        XWOS_VALIDATE((dst->type == XWUP_EVT_TYPE_SEL), "type-error", -ETYPE);

        return xwup_evt_unbind(src, dst);
}

static __xwcc_inline
xwer_t xwosdl_sel_get_num(struct xwosdl_sel * sel, xwsz_t * numbuf)
{
        XWOS_VALIDATE((sel), "nullptr", -EFAULT);

        return xwup_evt_get_num(sel, numbuf);
}

static __xwcc_inline
xwer_t xwosdl_sel_select(struct xwosdl_sel * sel, xwbmp_t msk[], xwbmp_t trg[])
{
        XWOS_VALIDATE((sel), "nullptr", -EFAULT);
        XWOS_VALIDATE((msk), "nullptr", -EFAULT);
        XWOS_VALIDATE((sel->type == XWUP_EVT_TYPE_SEL), "type-error", -ETYPE);
        XWOS_VALIDATE((XWOK != xwup_irq_get_id(NULL)), "not-thd-ctx", -EISRCTX);

        return xwup_sel_select(sel, msk, trg);
}

static __xwcc_inline
xwer_t xwosdl_sel_select_to(struct xwosdl_sel * sel,
                            xwbmp_t msk[], xwbmp_t trg[],
                            xwtm_t to)
{
        XWOS_VALIDATE((sel), "nullptr", -EFAULT);
        XWOS_VALIDATE((msk), "nullptr", -EFAULT);
        XWOS_VALIDATE((sel->type == XWUP_EVT_TYPE_SEL), "type-error", -ETYPE);
        XWOS_VALIDATE((XWOK != xwup_irq_get_id(NULL)), "not-thd-ctx", -EISRCTX);

        return xwup_sel_select_to(sel, msk, trg, to);
}

static __xwcc_inline
xwer_t xwosdl_sel_tryselect(struct xwosdl_sel * sel, xwbmp_t msk[], xwbmp_t trg[])
{
        XWOS_VALIDATE((sel), "nullptr", -EFAULT);
        XWOS_VALIDATE((msk), "nullptr", -EFAULT);
        XWOS_VALIDATE((sel->type == XWUP_EVT_TYPE_SEL), "type-error", -ETYPE);

        return xwup_sel_tryselect(sel, msk, trg);
}

static __xwcc_inline
xwer_t xwosdl_sel_select_unintr(struct xwosdl_sel * sel, xwbmp_t msk[], xwbmp_t trg[])
{
        XWOS_VALIDATE((sel), "nullptr", -EFAULT);
        XWOS_VALIDATE((msk), "nullptr", -EFAULT);
        XWOS_VALIDATE((sel->type == XWUP_EVT_TYPE_SEL), "type-error", -ETYPE);
        XWOS_VALIDATE((XWOK != xwup_irq_get_id(NULL)), "not-thd-ctx", -EISRCTX);

        return xwup_sel_select_unintr(sel, msk, trg);
}

#endif /* xwos/up/osdl/sync/sel.h */
