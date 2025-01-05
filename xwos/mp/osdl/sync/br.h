/**
 * @file
 * @brief XWOS MP内核操作系统接口描述层：线程栅栏
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef __xwos_mp_osdl_sync_br_h__
#define __xwos_mp_osdl_sync_br_h__

#include <xwos/mp/irq.h>
#include <xwos/mp/osdl/sync/sel.h>
#include <xwos/mp/sync/evt.h>

#define xwosdl_br xwmp_evt

typedef struct {
        struct xwosdl_br * br;
        xwsq_t tik;
} xwosdl_br_d;

#define XWOSDL_BR_NILD ((xwosdl_br_d){NULL, 0,})

static __xwcc_inline
xwer_t xwosdl_br_init(struct xwosdl_br * br, xwsz_t num,
                      xwbmp_t * bmp, xwbmp_t * msk)
{
        XWOS_VALIDATE((br), "nullptr", -EFAULT);
        XWOS_VALIDATE((bmp), "nullptr", -EFAULT);
        XWOS_VALIDATE((msk), "nullptr", -EFAULT);

        return xwmp_evt_init(br, XWMP_EVT_TYPE_BR, num, bmp, msk);
}

static __xwcc_inline
xwer_t xwosdl_br_fini(struct xwosdl_br * br)
{
        XWOS_VALIDATE((br), "nullptr", -EFAULT);

        return xwmp_evt_fini(br);
}

static __xwcc_inline
xwer_t xwosdl_br_grab(struct xwosdl_br * br)
{
        XWOS_VALIDATE((br), "nullptr", -EFAULT);

        return xwmp_evt_grab(br);
}

static __xwcc_inline
xwer_t xwosdl_br_put(struct xwosdl_br * br)
{
        XWOS_VALIDATE((br), "nullptr", -EFAULT);

        return xwmp_evt_put(br);
}

xwer_t xwosdl_br_create(xwosdl_br_d * brd, xwsz_t num);

static __xwcc_inline
xwer_t xwosdl_br_delete(struct xwosdl_br * br, xwsq_t tik)
{
        return xwmp_evt_delete(br, tik);
}

static __xwcc_inline
xwer_t xwosdl_br_acquire(struct xwosdl_br * br, xwsq_t tik)
{
        return xwmp_evt_acquire(br, tik);
}

static __xwcc_inline
xwer_t xwosdl_br_release(struct xwosdl_br * br, xwsq_t tik)
{
        return xwmp_evt_release(br, tik);
}

static __xwcc_inline
xwsq_t xwosdl_br_get_tik(struct xwosdl_br * br)
{
        return (NULL != br) ? br->cond.synobj.xwobj.tik : (xwsq_t)0;
}

static __xwcc_inline
xwer_t xwosdl_br_bind(struct xwosdl_br * br, struct xwosdl_sel * sel, xwsq_t pos)
{
        XWOS_VALIDATE((br), "nullptr", -EFAULT);
        XWOS_VALIDATE((sel), "nullptr", -EFAULT);
        XWOS_VALIDATE((sel->type == XWMP_EVT_TYPE_SEL), "type-error", -ETYPE);

        return xwmp_evt_bind(br, sel, pos);
}

static __xwcc_inline
xwer_t xwosdl_br_unbind(struct xwosdl_br * br, struct xwosdl_sel * sel)
{
        XWOS_VALIDATE((br), "nullptr", -EFAULT);
        XWOS_VALIDATE((sel), "nullptr", -EFAULT);
        XWOS_VALIDATE((sel->type == XWMP_EVT_TYPE_SEL), "type-error", -ETYPE);

        return xwmp_evt_unbind(br, sel);
}

static __xwcc_inline
xwer_t xwosdl_br_get_num(struct xwosdl_br * br, xwsz_t * numbuf)
{
        XWOS_VALIDATE((br), "nullptr", -EFAULT);
        XWOS_VALIDATE((numbuf), "nullptr", -EFAULT);

        return xwmp_evt_get_num(br, numbuf);
}

static __xwcc_inline
xwer_t xwosdl_br_wait(struct xwosdl_br * br)
{
        XWOS_VALIDATE((br), "nullptr", -EFAULT);
        XWOS_VALIDATE((br->type == XWMP_EVT_TYPE_BR), "type-error", -ETYPE);
        XWOS_VALIDATE((XWOK != xwmp_irq_get_id(NULL)), "not-thd-ctx", -EISRCTX);

        return xwmp_br_wait(br);
}

static __xwcc_inline
xwer_t xwosdl_br_wait_to(struct xwosdl_br * br, xwtm_t to)
{
        XWOS_VALIDATE((br), "nullptr", -EFAULT);
        XWOS_VALIDATE((br->type == XWMP_EVT_TYPE_BR), "type-error", -ETYPE);
        XWOS_VALIDATE((XWOK != xwmp_irq_get_id(NULL)), "not-thd-ctx", -EISRCTX);

        return xwmp_br_wait_to(br, to);
}

static __xwcc_inline
xwer_t xwosdl_br_wait_unintr(struct xwosdl_br * br)
{
        XWOS_VALIDATE((br), "nullptr", -EFAULT);
        XWOS_VALIDATE((br->type == XWMP_EVT_TYPE_BR), "type-error", -ETYPE);
        XWOS_VALIDATE((XWOK != xwmp_irq_get_id(NULL)), "not-thd-ctx", -EISRCTX);

        return xwmp_br_wait_unintr(br);
}

#endif /* xwos/mp/osdl/sync/br.h */
