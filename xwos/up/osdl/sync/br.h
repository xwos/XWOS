/**
 * @file
 * @brief 玄武OS UP内核操作系统接口描述层：线程栅栏
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __xwos_up_osdl_sync_br_h__
#define __xwos_up_osdl_sync_br_h__

#include <xwos/up/osdl/sync/sel.h>
#include <xwos/up/sync/evt.h>

#define xwosdl_br xwup_evt
#define XWOSDL_BR_MAXNUM                XWUP_EVT_MAXNUM
#define xwosdl_br_declare_bitmap(name) xwup_evt_declare_bitmap(name)

static __xwcc_inline
xwer_t xwosdl_br_init(struct xwosdl_br * br)
{
        return xwup_evt_init(br, NULL, XWUP_EVT_TYPE_BR);
}

static __xwcc_inline
xwer_t xwosdl_br_destroy(struct xwosdl_br * br)
{
        return xwup_evt_destroy(br);
}

static __xwcc_inline
xwer_t xwosdl_br_create(struct xwosdl_br ** brbuf)
{
        xwer_t rc;

        if (NULL != brbuf) {
                *brbuf = NULL;
                rc = xwup_evt_create(brbuf, NULL, XWUP_EVT_TYPE_BR);
        } else {
                rc = -EFAULT;
        }
        return rc;
}

static __xwcc_inline
xwer_t xwosdl_br_delete(struct xwosdl_br * br)
{
        return xwup_evt_delete(br);
}

static __xwcc_inline
xwsq_t xwosdl_br_gettik(struct xwosdl_br * br)
{
        XWOS_UNUSED(br);
        return 0;
}

static __xwcc_inline
xwer_t xwosdl_br_acquire(struct xwosdl_br * br, xwsq_t tik)
{
        XWOS_UNUSED(br);
        XWOS_UNUSED(tik);
        return XWOK;
}

static __xwcc_inline
xwer_t xwosdl_br_release(struct xwosdl_br * br, xwsq_t tik)
{
        XWOS_UNUSED(br);
        XWOS_UNUSED(tik);
        return XWOK;
}

static __xwcc_inline
xwer_t xwosdl_br_grab(struct xwosdl_br * br)
{
        XWOS_UNUSED(br);
        return XWOK;
}

static __xwcc_inline
xwer_t xwosdl_br_put(struct xwosdl_br * br)
{
        XWOS_UNUSED(br);
        return XWOK;
}

static __xwcc_inline
xwer_t xwosdl_br_bind(struct xwosdl_br * br, struct xwosdl_sel * sel, xwsq_t pos)
{
        return xwup_evt_bind(br, sel, pos);
}

static __xwcc_inline
xwer_t xwosdl_br_unbind(struct xwosdl_br * br, struct xwosdl_sel * sel)
{
        return xwup_evt_unbind(br, sel);
}

static __xwcc_inline
xwer_t xwosdl_br_intr_all(struct xwosdl_br * br)
{
        return xwup_evt_intr_all(br);
}

static __xwcc_inline
xwer_t xwosdl_br_sync(struct xwosdl_br * br, xwsq_t pos,
                      xwbmp_t msk[], xwbmp_t sync[])
{
        return xwup_evt_sync(br, pos, msk, sync);
}

static __xwcc_inline
xwer_t xwosdl_br_timedsync(struct xwosdl_br * br, xwsq_t pos,
                           xwbmp_t msk[], xwbmp_t sync[],
                           xwtm_t * xwtm)
{
        return xwup_evt_timedsync(br, pos, msk, sync, xwtm);
}

#endif /* xwos/up/osdl/sync/br.h */
