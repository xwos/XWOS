/**
 * @file
 * @brief XWOS MP内核操作系统接口描述层：事件标志
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef __xwos_mp_osdl_sync_flg_h__
#define __xwos_mp_osdl_sync_flg_h__

#include <xwos/mp/irq.h>
#include <xwos/mp/osdl/sync/sel.h>
#include <xwos/mp/sync/evt.h>

#define xwosdl_flg xwmp_evt

typedef struct {
        struct xwosdl_flg * flg;
        xwsq_t tik;
} xwosdl_flg_d;

#define XWOSDL_FLG_NILD ((xwosdl_flg_d){NULL, 0,})

#define XWOSDL_FLG_TRIGGER_SET_ALL      XWMP_FLG_TRIGGER_SET_ALL
#define XWOSDL_FLG_TRIGGER_SET_ANY      XWMP_FLG_TRIGGER_SET_ANY
#define XWOSDL_FLG_TRIGGER_CLR_ALL      XWMP_FLG_TRIGGER_CLR_ALL
#define XWOSDL_FLG_TRIGGER_CLR_ANY      XWMP_FLG_TRIGGER_CLR_ANY
#define XWOSDL_FLG_TRIGGER_TGL_ALL      XWMP_FLG_TRIGGER_TGL_ALL
#define XWOSDL_FLG_TRIGGER_TGL_ANY      XWMP_FLG_TRIGGER_TGL_ANY

#define XWOSDL_FLG_ACTION_NONE          XWMP_FLG_ACTION_NONE
#define XWOSDL_FLG_ACTION_CONSUMPTION   XWMP_FLG_ACTION_CONSUMPTION

static __xwcc_inline
xwer_t xwosdl_flg_init(struct xwosdl_flg * flg, xwsz_t num,
                       xwbmp_t * bmp, xwbmp_t * msk)
{
        XWOS_VALIDATE((flg), "nullptr", -EFAULT);
        XWOS_VALIDATE((bmp), "nullptr", -EFAULT);
        XWOS_VALIDATE((msk), "nullptr", -EFAULT);

        return xwmp_evt_init(flg, XWMP_EVT_TYPE_FLG, num, bmp, msk);
}

static __xwcc_inline
xwer_t xwosdl_flg_fini(struct xwosdl_flg * flg)
{
        XWOS_VALIDATE((flg), "nullptr", -EFAULT);

        return xwmp_evt_fini(flg);
}

static __xwcc_inline
xwer_t xwosdl_flg_grab(struct xwosdl_flg * flg)
{
        XWOS_VALIDATE((flg), "nullptr", -EFAULT);

        return xwmp_evt_grab(flg);
}

static __xwcc_inline
xwer_t xwosdl_flg_put(struct xwosdl_flg * flg)
{
        XWOS_VALIDATE((flg), "nullptr", -EFAULT);

        return xwmp_evt_put(flg);
}

xwer_t xwosdl_flg_create(xwosdl_flg_d * flgd, xwsz_t num);

static __xwcc_inline
xwer_t xwosdl_flg_delete(struct xwosdl_flg * flg, xwsq_t tik)
{
        return xwmp_evt_delete(flg, tik);
}

static __xwcc_inline
xwer_t xwosdl_flg_acquire(struct xwosdl_flg * flg, xwsq_t tik)
{
        return xwmp_evt_acquire(flg, tik);
}

static __xwcc_inline
xwer_t xwosdl_flg_release(struct xwosdl_flg * flg, xwsq_t tik)
{
        return xwmp_evt_release(flg, tik);
}

static __xwcc_inline
xwsq_t xwosdl_flg_get_tik(struct xwosdl_flg * flg)
{
        return (NULL != flg) ? flg->cond.synobj.xwobj.tik : (xwsq_t)0;
}

static __xwcc_inline
xwer_t xwosdl_flg_bind(struct xwosdl_flg * flg, struct xwosdl_sel * sel, xwsq_t pos)
{
        XWOS_VALIDATE((flg), "nullptr", -EFAULT);
        XWOS_VALIDATE((sel), "nullptr", -EFAULT);
        XWOS_VALIDATE((sel->type == XWMP_EVT_TYPE_SEL), "type-error", -ETYPE);

        return xwmp_evt_bind(flg, sel, pos);
}

static __xwcc_inline
xwer_t xwosdl_flg_unbind(struct xwosdl_flg * flg, struct xwosdl_sel * sel)
{
        XWOS_VALIDATE((flg), "nullptr", -EFAULT);
        XWOS_VALIDATE((sel), "nullptr", -EFAULT);
        XWOS_VALIDATE((sel->type == XWMP_EVT_TYPE_SEL), "type-error", -ETYPE);

        return xwmp_evt_unbind(flg, sel);
}

static __xwcc_inline
xwer_t xwosdl_flg_get_num(struct xwosdl_flg * flg, xwsz_t * numbuf)
{
        XWOS_VALIDATE((flg), "nullptr", -EFAULT);
        XWOS_VALIDATE((numbuf), "nullptr", -EFAULT);

        return xwmp_evt_get_num(flg, numbuf);
}

static __xwcc_inline
xwer_t xwosdl_flg_s1m(struct xwosdl_flg * flg, xwbmp_t msk[])
{
        XWOS_VALIDATE((flg), "nullptr", -EFAULT);
        XWOS_VALIDATE((msk), "nullptr", -EFAULT);
        XWOS_VALIDATE((flg->type == XWMP_EVT_TYPE_FLG), "type-error", -ETYPE);

        return xwmp_flg_s1m(flg, msk);
}

static __xwcc_inline
xwer_t xwosdl_flg_s1i(struct xwosdl_flg * flg, xwsq_t pos)
{
        XWOS_VALIDATE((flg), "nullptr", -EFAULT);
        XWOS_VALIDATE((flg->type == XWMP_EVT_TYPE_FLG), "type-error", -ETYPE);

        return xwmp_flg_s1i(flg, pos);
}

static __xwcc_inline
xwer_t xwosdl_flg_c0m(struct xwosdl_flg * flg, xwbmp_t msk[])
{
        XWOS_VALIDATE((flg), "nullptr", -EFAULT);
        XWOS_VALIDATE((msk), "nullptr", -EFAULT);
        XWOS_VALIDATE((flg->type == XWMP_EVT_TYPE_FLG), "type-error", -ETYPE);

        return xwmp_flg_c0m(flg, msk);
}

static __xwcc_inline
xwer_t xwosdl_flg_c0i(struct xwosdl_flg * flg, xwsq_t pos)
{
        XWOS_VALIDATE((flg), "nullptr", -EFAULT);
        XWOS_VALIDATE((flg->type == XWMP_EVT_TYPE_FLG), "type-error", -ETYPE);

        return xwmp_flg_c0i(flg, pos);
}

static __xwcc_inline
xwer_t xwosdl_flg_x1m(struct xwosdl_flg * flg, xwbmp_t msk[])
{
        XWOS_VALIDATE((flg), "nullptr", -EFAULT);
        XWOS_VALIDATE((msk), "nullptr", -EFAULT);
        XWOS_VALIDATE((flg->type == XWMP_EVT_TYPE_FLG), "type-error", -ETYPE);

        return xwmp_flg_x1m(flg, msk);
}

static __xwcc_inline
xwer_t xwosdl_flg_x1i(struct xwosdl_flg * flg, xwsq_t pos)
{
        XWOS_VALIDATE((flg), "nullptr", -EFAULT);
        XWOS_VALIDATE((flg->type == XWMP_EVT_TYPE_FLG), "type-error", -ETYPE);

        return xwmp_flg_x1i(flg, pos);
}

static __xwcc_inline
xwer_t xwosdl_flg_read(struct xwosdl_flg * flg, xwbmp_t out[])
{
        XWOS_VALIDATE((flg), "nullptr", -EFAULT);
        XWOS_VALIDATE((out), "nullptr", -EFAULT);

        return xwmp_flg_read(flg, out);
}

static __xwcc_inline
xwer_t xwosdl_flg_wait(struct xwosdl_flg * flg, xwsq_t trigger, xwsq_t action,
                       xwbmp_t origin[], xwbmp_t msk[])
{
        XWOS_VALIDATE((flg), "nullptr", -EFAULT);
        XWOS_VALIDATE((flg->type == XWMP_EVT_TYPE_FLG), "type-error", -ETYPE);
        XWOS_VALIDATE((trigger < XWMP_FLG_TRIGGER_NUM), "invalid-trigger", -EINVAL);
        XWOS_VALIDATE((action < XWMP_FLG_ACTION_NUM), "invalid-action", -EINVAL);
        XWOS_VALIDATE((origin), "nullptr", -EFAULT);
        XWOS_VALIDATE((msk), "nullptr", -EFAULT);
        XWOS_VALIDATE((XWOK != xwmp_irq_get_id(NULL)), "not-thd-ctx", -EISRCTX);

        return xwmp_flg_wait(flg, trigger, action, origin, msk);
}

static __xwcc_inline
xwer_t xwosdl_flg_wait_to(struct xwosdl_flg * flg, xwsq_t trigger, xwsq_t action,
                          xwbmp_t origin[], xwbmp_t msk[],
                          xwtm_t to)
{
        XWOS_VALIDATE((flg), "nullptr", -EFAULT);
        XWOS_VALIDATE((flg->type == XWMP_EVT_TYPE_FLG), "type-error", -ETYPE);
        XWOS_VALIDATE((trigger < XWMP_FLG_TRIGGER_NUM), "invalid-trigger", -EINVAL);
        XWOS_VALIDATE((action < XWMP_FLG_ACTION_NUM), "invalid-action", -EINVAL);
        XWOS_VALIDATE((origin), "nullptr", -EFAULT);
        XWOS_VALIDATE((msk), "nullptr", -EFAULT);
        XWOS_VALIDATE((XWOK != xwmp_irq_get_id(NULL)), "not-thd-ctx", -EISRCTX);

        return xwmp_flg_wait_to(flg, trigger, action, origin, msk, to);
}

static __xwcc_inline
xwer_t xwosdl_flg_trywait(struct xwosdl_flg * flg, xwsq_t trigger, xwsq_t action,
                          xwbmp_t origin[], xwbmp_t msk[])
{
        XWOS_VALIDATE((flg), "nullptr", -EFAULT);
        XWOS_VALIDATE((flg->type == XWMP_EVT_TYPE_FLG), "type-error", -ETYPE);
        XWOS_VALIDATE((trigger < XWMP_FLG_TRIGGER_NUM), "invalid-trigger", -EINVAL);
        XWOS_VALIDATE((action < XWMP_FLG_ACTION_NUM), "invalid-action", -EINVAL);
        XWOS_VALIDATE((origin), "nullptr", -EFAULT);
        XWOS_VALIDATE((msk), "nullptr", -EFAULT);

        return xwmp_flg_trywait(flg, trigger, action, origin, msk);
}

static __xwcc_inline
xwer_t xwosdl_flg_wait_unintr(struct xwosdl_flg * flg, xwsq_t trigger, xwsq_t action,
                              xwbmp_t origin[], xwbmp_t msk[])
{
        XWOS_VALIDATE((flg), "nullptr", -EFAULT);
        XWOS_VALIDATE((flg->type == XWMP_EVT_TYPE_FLG), "type-error", -ETYPE);
        XWOS_VALIDATE((trigger < XWMP_FLG_TRIGGER_NUM), "invalid-trigger", -EINVAL);
        XWOS_VALIDATE((action < XWMP_FLG_ACTION_NUM), "invalid-action", -EINVAL);
        XWOS_VALIDATE((origin), "nullptr", -EFAULT);
        XWOS_VALIDATE((msk), "nullptr", -EFAULT);
        XWOS_VALIDATE((XWOK != xwmp_irq_get_id(NULL)), "not-thd-ctx", -EISRCTX);

        return xwmp_flg_wait_unintr(flg, trigger, action, origin, msk);
}

#endif /* xwos/mp/osdl/sync/flg.h */
