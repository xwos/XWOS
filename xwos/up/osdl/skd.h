/**
 * @file
 * @brief 玄武OS UP内核操作系统接口描述层：调度
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __xwos_up_osdl_skd_h__
#define __xwos_up_osdl_skd_h__

#include <xwos/up/skd.h>

#define XWOSDL_SKDATTR_PRIVILEGED               XWUP_SKDATTR_PRIVILEGED
#define XWOSDL_SKDATTR_DETACHED                 XWUP_SKDATTR_DETACHED
#define XWOSDL_SKDATTR_JOINABLE                 XWUP_SKDATTR_JOINABLE
#define XWOSDL_SKD_PRIORITY_RT_MIN              XWUP_SKD_PRIORITY_RT_MIN
#define XWOSDL_SKD_PRIORITY_RT_MAX              XWUP_SKD_PRIORITY_RT_MAX
#define XWOSDL_SKD_PRIORITY_INVALID             XWUP_SKD_PRIORITY_INVALID
#define XWOSDL_SKD_PRIORITY_RAISE(base, inc)    XWUP_SKD_PRIORITY_RAISE(base, inc)
#define XWOSDL_SKD_PRIORITY_DROP(base, dec)     XWUP_SKD_PRIORITY_DROP(base, dec)

#define XWOSDL_SKD_CONTEXT_INIT_EXIT            XWUP_SKD_CONTEXT_INIT_EXIT
#define XWOSDL_SKD_CONTEXT_THD                  XWUP_SKD_CONTEXT_THD
#define XWOSDL_SKD_CONTEXT_ISR                  XWUP_SKD_CONTEXT_ISR
#define XWOSDL_SKD_CONTEXT_BH                   XWUP_SKD_CONTEXT_BH
#define XWOSDL_SKD_CONTEXT_IDLE                 XWUP_SKD_CONTEXT_IDLE

#if defined(XWUPCFG_SKD_THD_LOCAL_DATA_NUM)
#  define XWOSDL_THD_LOCAL_DATA_NUM             XWUPCFG_SKD_THD_LOCAL_DATA_NUM
#else
#  define XWOSDL_THD_LOCAL_DATA_NUM             (0U)
#endif

static __xwcc_inline
bool xwosdl_skd_prio_tst_valid(xwpr_t prio)
{
        return xwup_skd_prio_tst_valid(prio);
}

static __xwcc_inline
xwer_t xwosdl_skd_start_lc(void)
{
        return xwup_skd_start_lc();
}

static __xwcc_inline
xwid_t xwosdl_skd_id_lc(void)
{
        return 0;
}

static __xwcc_inline
void xwosdl_skd_get_context_lc(xwsq_t * ctxbuf, xwirq_t * irqnbuf)
{
        xwup_skd_get_context_lc(ctxbuf, irqnbuf);
}

static __xwcc_inline
xwtm_t xwosdl_skd_get_timetick_lc(void)
{
        struct xwup_skd * xwskd;

        xwskd = xwup_skd_get_lc();
        return xwup_syshwt_get_timetick(&xwskd->tt.hwt);
}

static __xwcc_inline
xwu64_t xwosdl_skd_get_tickcount_lc(void)
{
        struct xwup_skd * xwskd;

        xwskd = xwup_skd_get_lc();
        return xwup_syshwt_get_tickcount(&xwskd->tt.hwt);
}

static __xwcc_inline
xwtm_t xwosdl_skd_get_timestamp_lc(void)
{
        struct xwup_skd * xwskd;

        xwskd = xwup_skd_get_lc();
        return xwup_syshwt_get_timestamp(&xwskd->tt.hwt);
}

static __xwcc_inline
void xwosdl_skd_dspmpt_lc(void)
{
        xwup_skd_dspmpt_lc();
}

static __xwcc_inline
void xwosdl_skd_enpmpt_lc(void)
{
        xwup_skd_enpmpt_lc();
}

#include <xwos/up/thd.h>

#define xwosdl_thd xwup_thd
typedef xwup_thd_f xwosdl_thd_f;

static __xwcc_inline
xwer_t xwosdl_thd_init(struct xwosdl_thd * thd, const char * name,
                       xwosdl_thd_f mainfunc, void * arg,
                       xwstk_t * stack, xwsz_t stack_size,
                       xwpr_t priority, xwsq_t attr)
{
        xwer_t rc;

        rc = xwup_thd_init(thd, name,
                           (xwup_thd_f)mainfunc, arg,
                           stack, stack_size,
                           priority, attr);
        return rc;
}

static __xwcc_inline
xwer_t xwosdl_thd_create(struct xwosdl_thd ** thdbuf, const char * name,
                         xwosdl_thd_f mainfunc, void * arg, xwsz_t stack_size,
                         xwpr_t priority, xwsq_t attr)
{
        return xwup_thd_create(thdbuf, name, mainfunc, arg,
                               stack_size, priority, attr);
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
xwer_t xwosdl_thd_stop(struct xwosdl_thd * thd, xwer_t * rc)
{
        return xwup_thd_stop(thd, rc);
}

static __xwcc_inline
xwer_t xwosdl_thd_cancel(struct xwosdl_thd * thd)
{
        return xwup_thd_cancel(thd);
}

static __xwos_inline_api
xwer_t xwosdl_thd_join(struct xwosdl_thd * thd, xwer_t * trc)
{
        return xwup_thd_join(thd, trc);
}

static __xwcc_inline
xwer_t xwosdl_thd_detach(struct xwosdl_thd * thd)
{
        return xwup_thd_detach(thd);
}

static __xwcc_inline
xwer_t xwosdl_thd_intr(struct xwosdl_thd * thd)
{
        return xwup_thd_intr(thd);
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
xwer_t xwosdl_cthd_sleep(xwtm_t * xwtm)
{
        return xwup_cthd_sleep(xwtm);
}

static __xwcc_inline
xwer_t xwosdl_cthd_sleep_from(xwtm_t * origin, xwtm_t inc)
{
        return xwup_cthd_sleep_from(origin, inc);
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

#endif /* xwos/up/osdl/skd.h */
