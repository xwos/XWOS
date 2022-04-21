/**
 * @file
 * @brief 玄武OS UP内核操作系统接口描述层：调度
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef __xwos_up_osdl_thd_h__
#define __xwos_up_osdl_thd_h__

#include <xwos/up/thd.h>

#if defined(XWUPCFG_SKD_THD_LOCAL_DATA_NUM)
#  define XWOSDL_THD_LOCAL_DATA_NUM             XWUPCFG_SKD_THD_LOCAL_DATA_NUM
#else
#  define XWOSDL_THD_LOCAL_DATA_NUM             (0U)
#endif

#define xwosdl_thd_attr xwup_thd_attr
#define xwosdl_thd xwup_thd
typedef xwup_thd_f xwosdl_thd_f;

static __xwcc_inline
void xwosdl_thd_attr_init(struct xwosdl_thd_attr * attr)
{
        xwup_thd_attr_init(attr);
}

static __xwcc_inline
xwer_t xwosdl_thd_init(struct xwosdl_thd * thd,
                       const struct xwosdl_thd_attr * attr,
                       xwosdl_thd_f mainfunc, void * arg)
{
        return xwup_thd_init(thd, attr, mainfunc, arg);
}

static __xwcc_inline
xwer_t xwosdl_thd_create(struct xwosdl_thd ** thdbuf,
                         const struct xwosdl_thd_attr * attr,
                         xwosdl_thd_f mainfunc, void * arg)
{
        return xwup_thd_create(thdbuf, attr, mainfunc, arg);
}

static __xwcc_inline
xwsq_t xwosdl_thd_gettik(struct xwosdl_thd * thd)
{
        XWOS_UNUSED(thd);
        return 0;
}

static __xwcc_inline
xwer_t xwosdl_thd_acquire(struct xwosdl_thd * thd, xwsq_t tik)
{
        XWOS_UNUSED(thd);
        XWOS_UNUSED(tik);
        return XWOK;
}

static __xwcc_inline
xwer_t xwosdl_thd_release(struct xwosdl_thd * thd, xwsq_t tik)
{
        XWOS_UNUSED(thd);
        XWOS_UNUSED(tik);
        return XWOK;
}

static __xwcc_inline
xwer_t xwosdl_thd_grab(struct xwosdl_thd * thd)
{
        XWOS_UNUSED(thd);
        return XWOK;
}

static __xwcc_inline
xwer_t xwosdl_thd_put(struct xwosdl_thd * thd)
{
        XWOS_UNUSED(thd);
        return XWOK;
}

static __xwcc_inline
xwer_t xwosdl_thd_quit(struct xwosdl_thd * thd)
{
        return xwup_thd_quit(thd);
}

static __xwos_inline_api
xwer_t xwosdl_thd_join(struct xwosdl_thd * thd, xwer_t * trc)
{
        return xwup_thd_join(thd, trc);
}

xwer_t xwosdl_thd_stop(struct xwosdl_thd * thd, xwer_t * rc);

static __xwcc_inline
xwer_t xwosdl_thd_detach(struct xwosdl_thd * thd)
{
        return xwup_thd_detach(thd);
}

static __xwcc_inline
xwer_t xwosdl_thd_migrate(struct xwosdl_thd * thd, xwid_t dstcpu)
{
        XWOS_UNUSED(thd);
        XWOS_UNUSED(dstcpu);
        return XWOK;
}

static __xwcc_inline
struct xwosdl_thd * xwosdl_cthd_self(void)
{
        return xwup_skd_get_cthd_lc();
}

static __xwcc_inline
void xwosdl_cthd_yield(void)
{
        return xwup_cthd_yield();
}

static __xwcc_inline
void xwosdl_cthd_exit(xwer_t rc)
{
        xwup_cthd_exit(rc);
}

static __xwcc_inline
bool xwosdl_cthd_shld_frz(void)
{
        return xwup_cthd_shld_frz();
}

static __xwcc_inline
bool xwosdl_cthd_shld_stop(void)
{
        return xwup_cthd_shld_stop();
}

static __xwcc_inline
bool xwosdl_cthd_frz_shld_stop(bool * frozen)
{
        return xwup_cthd_frz_shld_stop(frozen);
}

static __xwcc_inline
xwer_t xwosdl_cthd_sleep_to(xwtm_t to)
{
        return xwup_cthd_sleep_to(to);
}

static __xwcc_inline
xwer_t xwosdl_cthd_sleep_from(xwtm_t * from, xwtm_t dur)
{
        return xwup_cthd_sleep_from(from, dur);
}

static __xwcc_inline
xwer_t xwosdl_cthd_freeze(void)
{
        return xwup_cthd_freeze();
}

#if defined(XWUPCFG_SKD_THD_LOCAL_DATA_NUM) && (XWUPCFG_SKD_THD_LOCAL_DATA_NUM > 0U)
static __xwos_inline_api
xwer_t xwosdl_thd_set_data(struct xwosdl_thd * thd, xwsq_t pos, void * data)
{
        return xwup_thd_set_data(thd, pos, data);
}

static __xwos_inline_api
xwer_t xwosdl_thd_get_data(struct xwosdl_thd * thd, xwsq_t pos, void ** databuf)
{
        return xwup_thd_get_data(thd, pos, databuf);
}

static __xwos_inline_api
xwer_t xwosdl_cthd_set_data(xwsq_t pos, void * data)
{
        return xwup_cthd_set_data(pos, data);
}

static __xwos_inline_api
xwer_t xwosdl_cthd_get_data(xwsq_t pos, void ** databuf)
{
        return xwup_cthd_get_data(pos, databuf);

}
#endif

#endif /* xwos/up/osdl/thd.h */
