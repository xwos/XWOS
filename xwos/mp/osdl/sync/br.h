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

static __xwcc_inline
xwer_t xwosdl_br_init(struct xwosdl_br * br, xwsz_t num,
                      xwbmp_t * bmp, xwbmp_t * msk)
{
        return xwmp_evt_init(br, XWMP_EVT_TYPE_BR, num, bmp, msk);
}

static __xwcc_inline
xwer_t xwosdl_br_destroy(struct xwosdl_br * br)
{
        return xwmp_evt_destroy(br);
}

static __xwcc_inline
xwer_t xwosdl_br_create(struct xwosdl_br ** brbuf, xwsz_t num)
{
        return xwmp_evt_create(brbuf, XWMP_EVT_TYPE_BR, num);
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
xwer_t xwosdl_br_get_num(struct xwosdl_br * br, xwsz_t * numbuf)
{
        return xwmp_evt_get_num(br, numbuf);
}

static __xwcc_inline
xwer_t xwosdl_br_sync(struct xwosdl_br * br, xwsq_t pos, xwbmp_t msk[])
{
        return xwmp_br_sync(br, pos, msk);
}

static __xwcc_inline
xwer_t xwosdl_br_timedsync(struct xwosdl_br * br, xwsq_t pos, xwbmp_t msk[],
                           xwtm_t * xwtm)
{
        return xwmp_br_timedsync(br, pos, msk, xwtm);
}

#endif /* xwos/mp/osdl/sync/br.h */
