/**
 * @file
 * @brief XWOS UP内核操作系统接口描述层：条件量
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef __xwos_up_osdl_sync_cond_h__
#define __xwos_up_osdl_sync_cond_h__

#include <xwos/up/irq.h>
#include <xwos/up/osdl/sync/sel.h>
#include <xwos/up/sync/cond.h>

#define xwosdl_cond xwup_cond

typedef struct {
        struct xwosdl_cond * cond;
        xwsq_t tik;
} xwosdl_cond_d;

#define XWOSDL_COND_NILD ((xwosdl_cond_d){NULL, 0,})

static __xwcc_inline
xwer_t xwosdl_cond_init(struct xwosdl_cond * cond)
{
        XWOS_VALIDATE((cond), "nullptr", -EFAULT);

        return xwup_cond_init(cond);
}

static __xwcc_inline
xwer_t xwosdl_cond_fini(struct xwosdl_cond * cond)
{
        XWOS_VALIDATE((cond), "nullptr", -EFAULT);

        return xwup_cond_fini(cond);
}

static __xwcc_inline
xwer_t xwosdl_cond_grab(struct xwosdl_cond * cond)
{
        XWOS_VALIDATE((cond), "nullptr", -EFAULT);

        return xwup_cond_grab(cond);
}

static __xwcc_inline
xwer_t xwosdl_cond_put(struct xwosdl_cond * cond)
{
        XWOS_VALIDATE((cond), "nullptr", -EFAULT);

        return xwup_cond_put(cond);
}

xwer_t xwosdl_cond_create(xwosdl_cond_d * condd);

static __xwcc_inline
xwer_t xwosdl_cond_delete(struct xwosdl_cond * cond, xwsq_t tik)
{
        return xwup_cond_delete(cond, tik);
}

static __xwcc_inline
xwer_t xwosdl_cond_acquire(struct xwosdl_cond * cond, xwsq_t tik)
{
        return xwup_cond_acquire(cond, tik);
}

static __xwcc_inline
xwer_t xwosdl_cond_release(struct xwosdl_cond * cond, xwsq_t tik)
{
        return xwup_cond_release(cond, tik);
}

static __xwcc_inline
xwsq_t xwosdl_cond_gettik(struct xwosdl_cond * cond)
{
        return (NULL != cond) ? cond->synobj.xwobj.tik : (xwsq_t)0;
}

static __xwcc_inline
xwer_t xwosdl_cond_bind(struct xwosdl_cond * cond, struct xwosdl_sel * sel, xwsq_t pos)
{
#if defined(XWOSCFG_SYNC_EVT) && (1 == XWOSCFG_SYNC_EVT)
        XWOS_VALIDATE((cond), "nullptr", -EFAULT);
        XWOS_VALIDATE((sel), "nullptr", -EFAULT);
        XWOS_VALIDATE((sel->type == XWUP_EVT_TYPE_SEL), "type-error", -ETYPE);

        return xwup_cond_bind(cond, sel, pos);
#else
        XWOS_UNUSED(cond);
        XWOS_UNUSED(sel);
        XWOS_UNUSED(pos);
        return -ENOSYS;
#endif
}

static __xwcc_inline
xwer_t xwosdl_cond_unbind(struct xwosdl_cond * cond, struct xwosdl_sel * sel)
{
#if defined(XWOSCFG_SYNC_EVT) && (1 == XWOSCFG_SYNC_EVT)
        XWOS_VALIDATE((cond), "nullptr", -EFAULT);
        XWOS_VALIDATE((sel), "nullptr", -EFAULT);
        XWOS_VALIDATE((sel->type == XWUP_EVT_TYPE_SEL), "type-error", -ETYPE);

        return xwup_cond_unbind(cond, sel);
#else
        XWOS_UNUSED(cond);
        XWOS_UNUSED(sel);
        return -ENOSYS;
#endif
}

static __xwcc_inline
xwer_t xwosdl_cond_freeze(struct xwosdl_cond * cond)
{
        XWOS_VALIDATE((cond), "nullptr", -EFAULT);

        return xwup_cond_freeze(cond);
}

static __xwcc_inline
xwer_t xwosdl_cond_thaw(struct xwosdl_cond * cond)
{
        XWOS_VALIDATE((cond), "nullptr", -EFAULT);

        return xwup_cond_thaw(cond);
}

static __xwcc_inline
xwer_t xwosdl_cond_broadcast(struct xwosdl_cond * cond)
{
        XWOS_VALIDATE((cond), "nullptr", -EFAULT);

        return xwup_cond_broadcast(cond);
}

static __xwcc_inline
xwer_t xwosdl_cond_unicast(struct xwosdl_cond * cond)
{
        XWOS_VALIDATE((cond), "nullptr", -EFAULT);

        return xwup_cond_unicast(cond);
}

static __xwcc_inline
xwer_t xwosdl_cond_wait(struct xwosdl_cond * cond,
                        union xwos_ulock lock, xwsq_t lktype, void * lkdata,
                        xwsq_t * lkst)
{
        XWOS_VALIDATE((cond), "nullptr", -EFAULT);
        XWOS_VALIDATE((lkst), "nullptr", -EFAULT);
        XWOS_VALIDATE((lktype < XWOS_LK_NUM), "invalid-type", -EINVAL);
        XWOS_VALIDATE((((NULL == lock.anon) && (XWOS_LK_NONE == lktype)) ||
                       ((lock.anon) && (lktype > XWOS_LK_NONE))),
                      "invalid-lock", -EINVAL);
        XWOS_VALIDATE((-ETHDCTX == xwup_irq_get_id(NULL)), "not-thd-ctx", -ENOTTHDCTX);

        return xwup_cond_wait(cond,
                              lock.anon, lktype, lkdata,
                              lkst);
}

static __xwcc_inline
xwer_t xwosdl_cond_wait_to(struct xwosdl_cond * cond,
                           union xwos_ulock lock, xwsq_t lktype, void * lkdata,
                           xwtm_t to, xwsq_t * lkst)
{
        XWOS_VALIDATE((cond), "nullptr", -EFAULT);
        XWOS_VALIDATE((lkst), "nullptr", -EFAULT);
        XWOS_VALIDATE((lktype < XWOS_LK_NUM), "invalid-type", -EINVAL);
        XWOS_VALIDATE((((NULL == lock.anon) && (XWOS_LK_NONE == lktype)) ||
                       ((lock.anon) && (lktype > XWOS_LK_NONE))),
                      "invalid-lock", -EINVAL);
        XWOS_VALIDATE((-ETHDCTX == xwup_irq_get_id(NULL)), "not-thd-ctx", -ENOTTHDCTX);

        return xwup_cond_wait_to(cond,
                                 lock.anon, lktype, lkdata,
                                 to, lkst);
}

static __xwcc_inline
xwer_t xwosdl_cond_wait_unintr(struct xwosdl_cond * cond,
                               union xwos_ulock lock, xwsq_t lktype, void * lkdata,
                               xwsq_t * lkst)
{
        XWOS_VALIDATE((cond), "nullptr", -EFAULT);
        XWOS_VALIDATE((lkst), "nullptr", -EFAULT);
        XWOS_VALIDATE((lktype < XWOS_LK_NUM), "invalid-type", -EINVAL);
        XWOS_VALIDATE((((NULL == lock.anon) && (XWOS_LK_NONE == lktype)) ||
                       ((lock.anon) && (lktype > XWOS_LK_NONE))),
                      "invalid-lock", -EINVAL);
        XWOS_VALIDATE((-ETHDCTX == xwup_irq_get_id(NULL)), "not-thd-ctx", -ENOTTHDCTX);

        return xwup_cond_wait_unintr(cond,
                                     lock.anon, lktype, lkdata,
                                     lkst);
}

#endif /* xwos/up/osdl/sync/cond.h */
