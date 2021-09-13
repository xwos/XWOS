/**
 * @file
 * @brief 玄武OS UP内核操作系统接口描述层：条件量
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __xwos_up_osdl_sync_cond_h__
#define __xwos_up_osdl_sync_cond_h__

#include <xwos/up/osdl/sync/sel.h>
#include <xwos/up/sync/cond.h>

#define xwosdl_cond xwup_cond

static __xwcc_inline
xwer_t xwosdl_cond_init(struct xwosdl_cond * cond)
{
        return xwup_cond_init(cond);
}

static __xwcc_inline
xwer_t xwosdl_cond_fini(struct xwosdl_cond * cond)
{
        return xwup_cond_fini(cond);
}

static __xwcc_inline
xwer_t xwosdl_cond_create(struct xwosdl_cond ** condbuf)
{
        return xwup_cond_create(condbuf);
}

static __xwcc_inline
xwer_t xwosdl_cond_delete(struct xwosdl_cond * cond)
{
        return xwup_cond_delete(cond);
}

static __xwcc_inline
xwsq_t xwosdl_cond_gettik(struct xwosdl_cond * cond)
{
        XWOS_UNUSED(cond);
        return 0;
}

static __xwcc_inline
xwer_t xwosdl_cond_acquire(struct xwosdl_cond * cond, xwsq_t tik)
{
        XWOS_UNUSED(cond);
        XWOS_UNUSED(tik);
        return XWOK;
}

static __xwcc_inline
xwer_t xwosdl_cond_release(struct xwosdl_cond * cond, xwsq_t tik)
{
        XWOS_UNUSED(cond);
        XWOS_UNUSED(tik);
        return XWOK;
}

static __xwcc_inline
xwer_t xwosdl_cond_grab(struct xwosdl_cond * cond)
{
        XWOS_UNUSED(cond);
        return XWOK;
}

static __xwcc_inline
xwer_t xwosdl_cond_put(struct xwosdl_cond * cond)
{
        XWOS_UNUSED(cond);
        return XWOK;
}

static __xwcc_inline
xwer_t xwosdl_cond_bind(struct xwosdl_cond * cond, struct xwosdl_sel * sel,
                        xwsq_t pos)
{
#if !defined(XWUPCFG_SYNC_EVT) || (1 != XWUPCFG_SYNC_EVT)
        XWOS_UNUSED(cond);
        XWOS_UNUSED(sel);
        XWOS_UNUSED(pos);
#endif
        return xwup_cond_bind(cond, sel, pos);
}

static __xwcc_inline
xwer_t xwosdl_cond_unbind(struct xwosdl_cond * cond, struct xwosdl_sel * sel)
{
#if !defined(XWUPCFG_SYNC_EVT) || (1 != XWUPCFG_SYNC_EVT)
        XWOS_UNUSED(cond);
        XWOS_UNUSED(sel);
#endif
        return xwup_cond_unbind(cond, sel);
}

static __xwcc_inline
xwer_t xwosdl_cond_freeze(struct xwosdl_cond * cond)
{
        return xwup_cond_freeze(cond);
}

static __xwcc_inline
xwer_t xwosdl_cond_thaw(struct xwosdl_cond * cond)
{
        return xwup_cond_thaw(cond);
}

static __xwcc_inline
xwer_t xwosdl_cond_intr_all(struct xwosdl_cond * cond)
{
        return xwup_cond_intr_all(cond);
}

static __xwcc_inline
xwer_t xwosdl_cond_broadcast(struct xwosdl_cond * cond)
{
        return xwup_cond_broadcast(cond);
}

static __xwcc_inline
xwer_t xwosdl_cond_unicast(struct xwosdl_cond * cond)
{
        return xwup_cond_unicast(cond);
}

static __xwcc_inline
xwer_t xwosdl_cond_wait(struct xwosdl_cond * cond,
                        union xwos_ulock lock, xwsq_t lktype, void * lkdata,
                        xwsq_t * lkst)
{
        return xwup_cond_wait(cond,
                              lock.anon, lktype, lkdata,
                              lkst);
}

static __xwcc_inline
xwer_t xwosdl_cond_timedwait(struct xwosdl_cond * cond,
                             union xwos_ulock lock, xwsq_t lktype, void * lkdata,
                             xwtm_t * xwtm, xwsq_t * lkst)
{
        return xwup_cond_timedwait(cond,
                                   lock.anon, lktype, lkdata,
                                   xwtm, lkst);
}

#endif /* xwos/up/osdl/sync/cond.h */
