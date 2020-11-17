/**
 * @file
 * @brief 玄武OS MP内核操作系统接口描述层：调度
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __xwos_mp_osdl_skd_h__
#define __xwos_mp_osdl_skd_h__

#include <xwos/mp/skd.h>

#define XWOSDL_SKDATTR_PRIVILEGED               XWMP_SKDATTR_PRIVILEGED
#define XWOSDL_SKD_PRIORITY_RT_MIN              XWMP_SKD_PRIORITY_RT_MIN
#define XWOSDL_SKD_PRIORITY_RT_MAX              XWMP_SKD_PRIORITY_RT_MAX
#define XWOSDL_SKD_PRIORITY_INVALID             XWMP_SKD_PRIORITY_INVALID
#define XWOSDL_SKD_PRIORITY_RAISE(base, inc)    XWMP_SKD_PRIORITY_RAISE(base, inc)
#define XWOSDL_SKD_PRIORITY_DROP(base, dec)     XWMP_SKD_PRIORITY_DROP(base, dec)

#define XWOSDL_SKD_CONTEXT_INIT_EXIT            XWMP_SKD_CONTEXT_INIT_EXIT
#define XWOSDL_SKD_CONTEXT_THRD                 XWMP_SKD_CONTEXT_THRD
#define XWOSDL_SKD_CONTEXT_ISR                  XWMP_SKD_CONTEXT_ISR
#define XWOSDL_SKD_CONTEXT_BH                   XWMP_SKD_CONTEXT_BH
#define XWOSDL_SKD_CONTEXT_IDLE                 XWMP_SKD_CONTEXT_IDLE

static __xwcc_inline
bool xwosdl_skd_prio_tst_valid(xwpr_t prio)
{
        return xwmp_skd_prio_tst_valid(prio);
}

static __xwcc_inline
xwer_t xwosdl_skd_start_lc(void)
{
        return xwmp_skd_start_lc();
}

static __xwcc_inline
void xwosdl_skd_get_context_lc(xwsq_t * ctxbuf, xwirq_t * irqnbuf)
{
        xwmp_skd_get_context_lc(ctxbuf, irqnbuf);
}

static __xwcc_inline
xwtm_t xwosdl_skd_get_timetick_lc(void)
{
        struct xwmp_skd * xwskd;

        xwskd = xwmp_skd_get_lc();
        return xwmp_syshwt_get_timetick(&xwskd->tt.hwt);
}

static __xwcc_inline
xwu64_t xwosdl_skd_get_tickcount_lc(void)
{
        struct xwmp_skd * xwskd;

        xwskd = xwmp_skd_get_lc();
        return xwmp_syshwt_get_tickcount(&xwskd->tt.hwt);
}

static __xwcc_inline
xwtm_t xwosdl_skd_get_timestamp_lc(void)
{
        struct xwmp_skd * xwskd;

        xwskd = xwmp_skd_get_lc();
        return xwmp_syshwt_get_timestamp(&xwskd->tt.hwt);
}

static __xwcc_inline
void xwosdl_skd_dspmpt_lc(void)
{
        xwmp_skd_dspmpt_lc();
}

static __xwcc_inline
void xwosdl_skd_enpmpt_lc(void)
{
        xwmp_skd_enpmpt_lc();
}

#include <xwos/mp/thrd.h>

#define xwosdl_tcb xwmp_tcb
typedef xwmp_thrd_f xwosdl_thrd_f;

static __xwcc_inline
xwer_t xwosdl_thrd_init(struct xwosdl_tcb * tcb,
                        const char * name,
                        xwosdl_thrd_f mainfunc, void * arg,
                        xwstk_t * stack, xwsz_t stack_size,
                        xwpr_t priority, xwsq_t attr)
{
        return xwmp_thrd_init(tcb,
                              name,
                              (xwmp_thrd_f)mainfunc, arg,
                              stack, stack_size,
                              priority, attr);
}

static __xwcc_inline
xwer_t xwosdl_thrd_destroy(struct xwosdl_tcb * tcb)
{
        return xwmp_thrd_destroy(tcb);
}

static __xwcc_inline
xwer_t xwosdl_thrd_create(xwid_t * tidbuf, const char * name,
                          xwosdl_thrd_f mainfunc, void * arg,
                          xwsz_t stack_size,
                          xwpr_t priority, xwsq_t attr)
{
        struct xwmp_tcb * tcb;
        xwer_t rc;

        rc = xwmp_thrd_create(&tcb,
                              name,
                              (xwmp_thrd_f)mainfunc, arg,
                              stack_size,
                              priority, attr);
        if (XWOK == rc) {
                *tidbuf = (xwid_t)tcb;
        } else {
                *tidbuf = (xwid_t)0;
        }
        return rc;
}

static __xwcc_inline
xwer_t xwosdl_thrd_delete(xwid_t tid)
{
        struct xwmp_tcb * tcb;

        tcb = (struct xwmp_tcb *)tid;
        return xwmp_thrd_delete(tcb);
}

static __xwcc_inline
xwid_t xwosdl_cthrd_id(void)
{
        struct xwmp_tcb * ctcb;

        ctcb = xwmp_skd_get_ctcb_lc();
        return (xwid_t)ctcb;
}

static __xwcc_inline
struct xwosdl_thrd * xwosdl_cthrd_obj(void)
{
        struct xwmp_tcb * ctcb;

        ctcb = xwmp_skd_get_ctcb_lc();
        return (struct xwosdl_thrd *)ctcb;
}

static __xwcc_inline
xwid_t xwosdl_thrd_id(struct xwosdl_tcb * tcb)
{
        return (xwid_t)tcb;
}

static __xwcc_inline
struct xwosdl_thrd * xwosdl_thrd_obj(xwid_t tid)
{
        return (struct xwosdl_thrd *)tid;
}

static __xwcc_inline
void xwosdl_cthrd_yield(void)
{
        return xwmp_cthrd_yield();
}

static __xwcc_inline
void xwosdl_cthrd_exit(xwer_t rc)
{
        xwmp_cthrd_exit(rc);
}

static __xwcc_inline
xwer_t xwosdl_thrd_terminate(xwid_t tid, xwer_t * rc)
{
        struct xwmp_tcb * tcb;

        tcb = (struct xwmp_tcb *)tid;
        return xwmp_thrd_terminate(tcb, rc);
}

static __xwcc_inline
void xwosdl_cthrd_wait_exit(void)
{
        xwmp_cthrd_wait_exit();
}

static __xwcc_inline
bool xwosdl_cthrd_shld_frz(void)
{
        return xwmp_cthrd_shld_frz();
}

static __xwcc_inline
bool xwosdl_cthrd_shld_stop(void)
{
        return xwmp_cthrd_shld_stop();
}

static __xwcc_inline
bool xwosdl_cthrd_frz_shld_stop(bool * frozen)
{
        return xwmp_cthrd_frz_shld_stop(frozen);
}

static __xwcc_inline
xwer_t xwosdl_cthrd_sleep(xwtm_t * xwtm)
{
        return xwmp_cthrd_sleep(xwtm);
}

static __xwcc_inline
xwer_t xwosdl_cthrd_sleep_from(xwtm_t * origin, xwtm_t inc)
{
        return xwmp_cthrd_sleep_from(origin, inc);
}

static __xwcc_inline
xwer_t xwosdl_thrd_intr(xwid_t tid)
{
        struct xwmp_tcb * tcb;

        tcb = (struct xwmp_tcb *)tid;
        return xwmp_thrd_intr(tcb);
}

static __xwcc_inline
xwer_t xwosdl_cthrd_freeze(void)
{
        return xwmp_cthrd_freeze();
}

static __xwcc_inline
xwer_t xwosdl_thrd_migrate(xwid_t tid, xwid_t dstcpu)
{
        struct xwmp_tcb * tcb;

        tcb = (struct xwmp_tcb *)tid;
        return xwmp_thrd_migrate(tcb, dstcpu);
}

#endif /* xwos/mp/osdl/skd.h */
