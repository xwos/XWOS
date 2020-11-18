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
#define XWOSDL_SKD_PRIORITY_RT_MIN              XWUP_SKD_PRIORITY_RT_MIN
#define XWOSDL_SKD_PRIORITY_RT_MAX              XWUP_SKD_PRIORITY_RT_MAX
#define XWOSDL_SKD_PRIORITY_INVALID             XWUP_SKD_PRIORITY_INVALID
#define XWOSDL_SKD_PRIORITY_RAISE(base, inc)    XWUP_SKD_PRIORITY_RAISE(base, inc)
#define XWOSDL_SKD_PRIORITY_DROP(base, dec)     XWUP_SKD_PRIORITY_DROP(base, dec)

#define XWOSDL_SKD_CONTEXT_INIT_EXIT            XWUP_SKD_CONTEXT_INIT_EXIT
#define XWOSDL_SKD_CONTEXT_THRD                 XWUP_SKD_CONTEXT_THRD
#define XWOSDL_SKD_CONTEXT_ISR                  XWUP_SKD_CONTEXT_ISR
#define XWOSDL_SKD_CONTEXT_BH                   XWUP_SKD_CONTEXT_BH
#define XWOSDL_SKD_CONTEXT_IDLE                 XWUP_SKD_CONTEXT_IDLE

#if defined(XWUPCFG_SKD_TCB_LOCAL_DATA_NUM)
  #define XWOSDL_THRD_LOCAL_DATA_NUM            XWUPCFG_SKD_TCB_LOCAL_DATA_NUM
#else
  #define XWOSDL_THRD_LOCAL_DATA_NUM            (0U)
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

#include <xwos/up/thrd.h>

#define xwosdl_tcb xwup_tcb
typedef xwup_thrd_f xwosdl_thrd_f;

static __xwcc_inline
xwer_t xwosdl_thrd_init(struct xwosdl_tcb * tcb,
                        const char * name,
                        xwosdl_thrd_f mainfunc, void * arg,
                        xwstk_t * stack, xwsz_t stack_size,
                        xwpr_t priority, xwsq_t attr)
{
        return xwup_thrd_init(tcb,
                              name,
                              (xwup_thrd_f)mainfunc, arg,
                              stack, stack_size,
                              priority, attr);
}

static __xwcc_inline
xwer_t xwosdl_thrd_destroy(struct xwosdl_tcb * tcb)
{
        return xwup_thrd_destroy(tcb);
}

static __xwcc_inline
xwer_t xwosdl_thrd_create(xwid_t * tidbuf, const char * name,
                          xwosdl_thrd_f mainfunc, void * arg, xwsz_t stack_size,
                          xwpr_t priority, xwsq_t attr)
{
        struct xwup_tcb * tcb;
        xwer_t rc;

        rc = xwup_thrd_create(&tcb, name,
                              (xwup_thrd_f)mainfunc, arg,
                              stack_size, priority, attr);
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
        struct xwup_tcb * tcb;

        tcb = (struct xwup_tcb *)tid;

        return xwup_thrd_delete(tcb);
}

static __xwcc_inline
xwid_t xwosdl_cthrd_id(void)
{
        struct xwup_tcb * ctcb;

        ctcb = xwup_skd_get_ctcb_lc();
        return (xwid_t)ctcb;
}

static __xwcc_inline
struct xwosdl_thrd * xwosdl_cthrd_obj(void)
{
        struct xwup_tcb * ctcb;

        ctcb = xwup_skd_get_ctcb_lc();
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
        return xwup_cthrd_yield();
}

static __xwcc_inline
void xwosdl_cthrd_exit(xwer_t rc)
{
        xwup_cthrd_exit(rc);
}

static __xwcc_inline
xwer_t xwosdl_thrd_terminate(xwid_t tid, xwer_t * rc)
{
        struct xwup_tcb * tcb;

        tcb = (struct xwup_tcb *)tid;
        return xwup_thrd_terminate(tcb, rc);
}

static __xwcc_inline
void xwosdl_cthrd_wait_exit(void)
{
        xwup_cthrd_wait_exit();
}

static __xwcc_inline
bool xwosdl_cthrd_shld_frz(void)
{
        return xwup_cthrd_shld_frz();
}

static __xwcc_inline
bool xwosdl_cthrd_shld_stop(void)
{
        return xwup_cthrd_shld_stop();
}

static __xwcc_inline
bool xwosdl_cthrd_frz_shld_stop(bool * frozen)
{
        return xwup_cthrd_frz_shld_stop(frozen);
}

static __xwcc_inline
xwer_t xwosdl_cthrd_sleep(xwtm_t * xwtm)
{
        return xwup_cthrd_sleep(xwtm);
}

static __xwcc_inline
xwer_t xwosdl_cthrd_sleep_from(xwtm_t * origin, xwtm_t inc)
{
        return xwup_cthrd_sleep_from(origin, inc);
}

static __xwcc_inline
xwer_t xwosdl_thrd_intr(xwid_t tid)
{
        struct xwup_tcb * tcb;

        tcb = (struct xwup_tcb *)tid;
        return xwup_thrd_intr(tcb);
}

static __xwcc_inline
xwer_t xwosdl_cthrd_freeze(void)
{
        return xwup_cthrd_freeze();
}

static __xwcc_inline
xwer_t xwosdl_thrd_migrate(xwid_t tid, xwid_t dstcpu)
{
        XWOS_UNUSED(tid);
        XWOS_UNUSED(dstcpu);
        return XWOK;
}

#if defined(XWUPCFG_SKD_TCB_LOCAL_DATA_NUM) && (XWUPCFG_SKD_TCB_LOCAL_DATA_NUM > 0U)
static __xwos_inline_api
xwer_t xwosdl_thrd_set_data(struct xwosdl_tcb * tcb, xwsq_t pos, void * data)
{
        return xwup_thrd_set_data(tcb, pos, data);
}

static __xwos_inline_api
xwer_t xwosdl_thrd_get_data(struct xwosdl_tcb * tcb, xwsq_t pos, void ** databuf)
{
        return xwup_thrd_get_data(tcb, pos, databuf);
}

static __xwos_inline_api
xwer_t xwosdl_cthrd_set_data(xwsq_t pos, void * data)
{
        return xwup_cthrd_set_data(pos, data);
}

static __xwos_inline_api
xwer_t xwosdl_cthrd_get_data(xwsq_t pos, void ** databuf)
{
        return xwup_cthrd_get_data(pos, databuf);

}
#endif /* XWUPCFG_SKD_TCB_LOCAL_DATA_NUM */

#endif /* xwos/up/osdl/skd.h */
