/**
 * @file
 * @brief 玄武OS MP内核操作系统接口描述层：线程栅栏
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __xwos_mp_osdl_sync_br_h__
#define __xwos_mp_osdl_sync_br_h__

#include <xwos/mp/osdl/sync/sel.h>
#include <xwos/mp/sync/evt.h>

#define xwosdl_br xwmp_evt
#define XWOSDL_BR_MAXNUM        XWMP_EVT_MAXNUM
#define xwosdl_br_declare_bitmap(name) xwmp_evt_declare_bitmap(name)

static __xwcc_inline
xwer_t xwosdl_br_init(struct xwosdl_br * br)
{
        return xwmp_evt_init(br, NULL, XWMP_EVT_TYPE_BR);
}

static __xwcc_inline
xwer_t xwosdl_br_destroy(struct xwosdl_br * br)
{
        return xwmp_evt_destroy(br);
}

static __xwcc_inline
xwer_t xwosdl_br_create(struct xwosdl_br ** brbuf)
{
        xwer_t rc;

        if (NULL != brbuf) {
                *brbuf = NULL;
                rc = xwmp_evt_create(brbuf, NULL, XWMP_EVT_TYPE_BR);
        } else {
                rc = -EFAULT;
        }
        return rc;
}

static __xwcc_inline
xwer_t xwosdl_br_delete(struct xwosdl_br * br)
{
        return xwmp_evt_delete(br);
}

static __xwcc_inline
xwsq_t xwosdl_br_gettik(struct xwosdl_br * br)
{
        xwsq_t tik;

        if (br) {
                tik = br->cond.synobj.xwobj.tik;
        } else {
                tik = 0;
        }
        return tik;
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
xwer_t xwosdl_br_grab(struct xwosdl_br * br)
{
        return xwmp_evt_grab(br);
}

static __xwcc_inline
xwer_t xwosdl_br_put(struct xwosdl_br * br)
{
        return xwmp_evt_put(br);
}

static __xwcc_inline
xwer_t xwosdl_br_bind(struct xwosdl_br * br, struct xwosdl_sel * sel, xwsq_t pos)
{
        return xwmp_evt_bind(br, sel, pos);
}

static __xwcc_inline
xwer_t xwosdl_br_unbind(struct xwosdl_br * br, struct xwosdl_sel * sel)
{
        return xwmp_evt_unbind(br, sel);
}

static __xwcc_inline
xwer_t xwosdl_br_intr_all(struct xwosdl_br * br)
{
        return xwmp_evt_intr_all(br);
}

static __xwcc_inline
xwer_t xwosdl_br_sync(struct xwosdl_br * br, xwsq_t pos,
                      xwbmp_t msk[], xwbmp_t sync[])
{
        return xwmp_evt_sync(br, pos, msk, sync);
}

static __xwcc_inline
xwer_t xwosdl_br_timedsync(struct xwosdl_br * br, xwsq_t pos,
                           xwbmp_t msk[], xwbmp_t sync[],
                           xwtm_t * xwtm)
{
        return xwmp_evt_timedsync(br, pos, msk, sync, xwtm);
}

#endif /* xwos/mp/osdl/sync/br.h */
