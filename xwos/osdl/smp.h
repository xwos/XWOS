/**
 * @file
 * @brief 操作系统接口描述
 * @author
 * + 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __xwos_osdl_smp_h__
#define __xwos_osdl_smp_h__

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********        irq        ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/smp/irq.h>

static __xw_inline
void xwosdl_cpuirq_enable_lc(void)
{
        xwos_cpuirq_enable_lc();
}

static __xw_inline
void xwosdl_cpuirq_disable_lc(void)
{
        xwos_cpuirq_disable_lc();
}

static __xw_inline
void xwosdl_cpuirq_restore_lc(xwreg_t cpuirq)
{
        xwos_cpuirq_restore_lc(cpuirq);
}

static __xw_inline
void xwosdl_cpuirq_save_lc(xwreg_t * cpuirq)
{
        xwos_cpuirq_save_lc(cpuirq);
}

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********     scheduler     ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/smp/scheduler.h>

#define XWOSDL_SD_PRIORITY_RT_MIN           XWOS_SD_PRIORITY_RT_MIN
#define XWOSDL_SD_PRIORITY_RT_MAX           XWOS_SD_PRIORITY_RT_MAX
#define XWOSDL_SD_PRIORITY_INVALID          XWOS_SD_PRIORITY_INVALID
#define XWOSDL_SD_PRIORITY_RAISE(base, inc) XWOS_SD_PRIORITY_RAISE(base, inc)
#define XWOSDL_SD_PRIORITY_DROP(base, dec)  XWOS_SD_PRIORITY_DROP(base, dec)

static __xw_inline
bool xwosdl_scheduler_prio_tst_valid(xwpr_t prio)
{
        return xwos_scheduler_prio_tst_valid(prio);
}

static __xw_inline
xwer_t xwosdl_scheduler_start_lc(void)
{
        return xwos_scheduler_start_lc();
}

static __xw_inline
xwtm_t xwosdl_scheduler_get_timetick_lc(void)
{
        struct xwos_scheduler * xwsd;

        xwsd = xwos_scheduler_get_lc();
        return xwos_syshwt_get_timetick(&xwsd->tt.hwt);
}

static __xw_inline
xwu64_t xwosdl_scheduler_get_tickcount_lc(void)
{
        struct xwos_scheduler * xwsd;

        xwsd = xwos_scheduler_get_lc();
        return xwos_syshwt_get_tickcount(&xwsd->tt.hwt);
}

static __xw_inline
xwtm_t xwosdl_scheduler_get_timestamp_lc(void)
{
        struct xwos_scheduler * xwsd;

        xwsd = xwos_scheduler_get_lc();
        return xwos_syshwt_get_timestamp(&xwsd->tt.hwt);
}

static __xw_inline
void xwosdl_scheduler_dspmpt_lc(void)
{
        xwos_scheduler_dspmpt_lc();
}

static __xw_inline
void xwosdl_scheduler_enpmpt_lc(void)
{
        xwos_scheduler_enpmpt_lc();
}

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      thread       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/smp/thread.h>

#define xwosdl_tcb xwos_tcb
typedef xwos_thrd_f xwosdl_thrd_f;

static __xw_inline
xwer_t xwosdl_thrd_init(struct xwosdl_tcb * tcb,
                        const char * name,
                        xwosdl_thrd_f mainfunc, void * arg,
                        xwstk_t * stack, xwsz_t stack_size,
                        xwpr_t priority, xwsq_t attr)
{
        return xwos_thrd_init(tcb,
                              name,
                              (xwos_thrd_f)mainfunc, arg,
                              stack, stack_size,
                              priority, attr);
}

static __xw_inline
xwer_t xwosdl_thrd_destroy(struct xwosdl_tcb * tcb)
{
        return xwos_thrd_destroy(tcb);
}

static __xw_inline
xwer_t xwosdl_thrd_create(xwid_t * tidbuf, const char * name,
                          xwosdl_thrd_f mainfunc, void * arg,
                          xwsz_t stack_size,
                          xwpr_t priority, xwsq_t attr)
{
        struct xwos_tcb * tcb;
        xwer_t rc;

        rc = xwos_thrd_create(&tcb,
                              name,
                              (xwos_thrd_f)mainfunc, arg,
                              stack_size,
                              priority, attr);
        if (OK == rc) {
                /* FIXME: Add to ID table */
                *tidbuf = (xwid_t)tcb;
        } else {
                *tidbuf = (xwid_t)0;
        }
        return rc;
}

static __xw_inline
xwer_t xwosdl_thrd_delete(xwid_t tid)
{
        struct xwos_tcb * tcb;

        /* FIXME: Del from ID table */
        tcb = (struct xwos_tcb *)tid;

        return xwos_thrd_delete(tcb);
}

static __xw_inline
xwid_t xwosdl_thrd_get_id(struct xwosdl_tcb * tcb)
{
        /* FIXME: find ID from obj */
        return (xwid_t)tcb;
}

static __xw_inline
xwid_t xwosdl_cthrd_get_id(void)
{
        struct xwos_tcb * ctcb;

        ctcb = xwos_scheduler_get_ctcb_lc();
        /* FIXME: find ID from obj */
        return (xwid_t)ctcb;
}

static __xw_inline
void xwosdl_cthrd_yield(void)
{
        return xwos_cthrd_yield();
}

static __xw_inline
void xwosdl_cthrd_exit(xwer_t rc)
{
        xwos_cthrd_exit(rc);
}

static __xw_inline
xwer_t xwosdl_thrd_terminate(xwid_t tid, xwer_t * rc)
{
        struct xwos_tcb * tcb;

        /* FIXME: Add ID table */
        tcb = (struct xwos_tcb *)tid;
        return xwos_thrd_terminate(tcb, rc);
}

static __xw_inline
void xwosdl_cthrd_wait_exit(void)
{
        xwos_cthrd_wait_exit();
}

static __xw_inline
bool xwosdl_cthrd_shld_frz(void)
{
        return xwos_cthrd_shld_frz();
}

static __xw_inline
bool xwosdl_cthrd_shld_stop(void)
{
        return xwos_cthrd_shld_stop();
}

static __xw_inline
bool xwosdl_cthrd_frz_shld_stop(bool * frozen)
{
        return xwos_cthrd_frz_shld_stop(frozen);
}

static __xw_inline
xwer_t xwosdl_thrd_continue(xwid_t tid)
{
        struct xwos_tcb * tcb;

        /* FIXME: Add ID table */
        tcb = (struct xwos_tcb *)tid;
        return xwos_thrd_continue(tcb);
}

static __xw_inline
xwer_t xwosdl_cthrd_pause(union xwlk_ulock lock, xwsq_t lktype,
                          void * lkdata, xwsz_t datanum,
                          xwsq_t * lkst)
{
        union xwlk_ulock oslk;

        /* FIXME: find lock from ID table */
        oslk.anon = lock.anon;
        return xwos_cthrd_pause(oslk.anon, lktype, lkdata, datanum, lkst);
}

static __xw_inline
xwer_t xwosdl_cthrd_timedpause(union xwlk_ulock lock, xwsq_t lktype,
                               void * lkdata, xwsz_t datanum,
                               xwtm_t * xwtm, xwsq_t * lkst)
{
        union xwlk_ulock oslk;

        /* FIXME: find lock from ID table */
        oslk.anon = lock.anon;
        return xwos_cthrd_timedpause(oslk.anon, lktype,
                                     lkdata, datanum,
                                     xwtm, lkst);
}

static __xw_inline
xwer_t xwosdl_cthrd_sleep(xwtm_t * xwtm)
{
        return xwos_cthrd_sleep(xwtm);
}

static __xw_inline
xwer_t xwosdl_cthrd_sleep_from(xwtm_t * origin, xwtm_t inc)
{
        return xwos_cthrd_sleep_from(origin, inc);
}

static __xw_inline
xwer_t xwosdl_thrd_intr(xwid_t tid)
{
        struct xwos_tcb * tcb;

        /* FIXME: Add ID table */
        tcb = (struct xwos_tcb *)tid;

        return xwos_thrd_intr(tcb);
}

static __xw_inline
xwer_t xwosdl_cthrd_freeze(void)
{
        return xwos_cthrd_freeze();
}

static __xw_inline
xwer_t xwosdl_thrd_migrate(xwid_t tid, xwid_t dstcpu)
{
        struct xwos_tcb * tcb;

        /* FIXME: Add ID table */
        tcb = (struct xwos_tcb *)tid;

        return xwos_thrd_migrate(tcb, dstcpu);
}

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********        swt        ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/smp/swt.h>

#define XWOSDL_SWT_FLAG_NULL        XWOS_SWT_FLAG_NULL
#define XWOSDL_SWT_FLAG_RESTART     XWOS_SWT_FLAG_RESTART
#define XWOSDL_SWT_FLAG_AUTORM      XWOS_SWT_FLAG_AUTORM

#define xwosdl_swt xwos_swt
typedef xwos_swt_f xwosdl_swt_f;

static __xw_inline
xwid_t xwosdl_swt_get_id(struct xwosdl_swt * swt)
{
        /* FIXME: find ID from obj */
        return (xwid_t)swt;
}

static __xw_inline
struct xwosdl_swt * xwosdl_swt_get_obj(xwid_t swtid)
{
        /* FIXME: find obj from ID table */
        return (struct xwosdl_swt *)swtid;
}

static __xw_inline
xwer_t xwosdl_swt_init(struct xwosdl_swt * swt, const char * name, xwsq_t flag)
{
        return xwos_swt_init(swt, name, flag);
}

static __xw_inline
xwer_t xwosdl_swt_destroy(struct xwosdl_swt * swt)
{
        return xwos_swt_destroy(swt);
}

static __xw_inline
xwer_t xwosdl_swt_create(xwid_t * swtidbuf, const char * name, xwsq_t flag)
{
        struct xwos_swt * swt;
        xwer_t rc;

        swt = NULL;
        rc = xwos_swt_create(&swt, name, flag);
        if (OK == rc) {
                /* FIXME: Add to ID table */
                *swtidbuf = (xwid_t)swt;
        } else {
                *swtidbuf = (xwid_t)0;
        }
        return rc;
}

static __xw_inline
xwer_t xwosdl_swt_delete(xwid_t swtid)
{
        struct xwos_swt * swt;
        xwer_t rc;

        swt = xwosdl_swt_get_obj(swtid);
        rc = xwos_swt_delete(swt);
        if (OK == rc) {
                /* FIXME: Del from ID table */
        }
        return rc;
}

static __xw_inline
xwer_t xwosdl_swt_start(xwid_t swtid, xwtm_t base, xwtm_t period,
                        xwosdl_swt_f callback, void * arg)
{
        struct xwos_swt * swt;
        xwer_t rc;

        swt = xwosdl_swt_get_obj(swtid);
        rc = xwos_swt_start(swt, base, period, (xwos_swt_f)callback, arg);
        return rc;
}

static __xw_inline
xwer_t xwosdl_swt_stop(xwid_t swtid)
{
        struct xwos_swt * swt;
        xwer_t rc;

        swt = xwosdl_swt_get_obj(swtid);
        rc = xwos_swt_stop(swt);
        return rc;
}

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       sync        ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/******** definitions ********/
#define xwosdl_selector xwsync_evt

static __xw_inline
struct xwosdl_selector * xwosdl_selector_get_obj(xwid_t sltid);

/******** semaphore ********/
#include <xwos/smp/sync/semaphore.h>

#define xwosdl_smr xwsync_smr

static __xw_inline
xwid_t xwosdl_smr_get_id(struct xwosdl_smr * smr)
{
        /* FIXME: find ID from obj */
        return (xwid_t)smr;
}

static __xw_inline
struct xwosdl_smr * xwosdl_smr_get_obj(xwid_t smrid)
{
        /* FIXME: find obj from ID table */
        return (struct xwosdl_smr *)smrid;
}

static __xw_inline
xwer_t xwosdl_smr_getvalue(xwid_t smrid, xwssq_t * sval)
{
        struct xwsync_smr * smr;
        xwer_t rc;

        smr = xwosdl_smr_get_obj(smrid);
        rc = xwsync_smr_getvalue(smr, sval);
        return rc;
}

static __xw_inline
xwer_t xwosdl_smr_bind(xwid_t smrid, xwid_t sltid, xwsq_t pos)
{
        struct xwosdl_selector * slt;
        struct xwosdl_smr * smr;
        xwer_t rc;

        slt = xwosdl_selector_get_obj(sltid);
        smr = xwosdl_smr_get_obj(smrid);
        rc = xwsync_smr_bind(smr, slt, pos);
        return rc;
}

static __xw_inline
xwer_t xwosdl_smr_unbind(xwid_t smrid, xwid_t sltid)
{
        struct xwosdl_selector * slt;
        struct xwosdl_smr * smr;
        xwer_t rc;

        slt = xwosdl_selector_get_obj(sltid);
        smr = xwosdl_smr_get_obj(smrid);
        rc = xwsync_smr_unbind(smr, slt);
        return rc;
}

#if defined(XWSMPCFG_SYNC_RTSMR) && (1 == XWSMPCFG_SYNC_RTSMR)
static __xw_inline
xwer_t xwosdl_smr_init(struct xwosdl_smr * smr, xwssq_t val, xwssq_t max)
{
        return xwsync_rtsmr_init(smr, val, max);
}

static __xw_inline
xwer_t xwosdl_smr_destroy(struct xwosdl_smr * smr)
{
        return xwsync_smr_destroy(smr);
}

static __xw_inline
xwer_t xwosdl_smr_create(xwid_t * smridbuf, xwssq_t val, xwssq_t max)
{
        struct xwosdl_smr * smr;
        xwer_t rc;

        smr = NULL;
        rc = xwsync_smr_create(&smr, XWSYNC_SMR_TYPE_RT, val, max);
        if (OK == rc) {
                /* FIXME: Add to ID table */
                *smridbuf = (xwid_t)smr;
        } else {
                *smridbuf = (xwid_t)0;
        }
        return rc;
}

static __xw_inline
xwer_t xwosdl_smr_delete(xwid_t smrid)
{
        struct xwsync_smr * smr;
        xwer_t rc;

        smr = xwosdl_smr_get_obj(smrid);
        rc = xwsync_smr_delete(smr);
        if (OK == rc) {
                /* FIXME: Del from ID table */
        }
        return rc;
}

static __xw_inline
xwer_t xwosdl_smr_freeze(xwid_t smrid)
{
        struct xwsync_smr * smr;
        xwer_t rc;

        smr = xwosdl_smr_get_obj(smrid);
        rc = xwsync_rtsmr_freeze(smr);
        return rc;
}

static __xw_inline
xwer_t xwosdl_smr_thaw(xwid_t smrid, xwssq_t val, xwssq_t max)
{
        struct xwsync_smr * smr;
        xwer_t rc;

        smr = xwosdl_smr_get_obj(smrid);
        rc = xwsync_rtsmr_thaw(smr, val, max);
        return rc;
}

static __xw_inline
xwer_t xwosdl_smr_post(xwid_t smrid)
{
        struct xwsync_smr * smr;
        xwer_t rc;

        smr = xwosdl_smr_get_obj(smrid);
        rc = xwsync_rtsmr_post(smr);
        return rc;
}

static __xw_inline
xwer_t xwosdl_smr_wait(xwid_t smrid)
{
        struct xwsync_smr * smr;
        xwer_t rc;

        smr = xwosdl_smr_get_obj(smrid);
        rc = xwsync_rtsmr_wait(smr);
        return rc;
}

static __xw_inline
xwer_t xwosdl_smr_trywait(xwid_t smrid)
{
        struct xwsync_smr * smr;
        xwer_t rc;

        smr = xwosdl_smr_get_obj(smrid);
        rc = xwsync_rtsmr_trywait(smr);
        return rc;
}

static __xw_inline
xwer_t xwosdl_smr_timedwait(xwid_t smrid, xwtm_t * xwtm)
{
        struct xwsync_smr * smr;
        xwer_t rc;

        smr = xwosdl_smr_get_obj(smrid);
        rc = xwsync_rtsmr_timedwait(smr, xwtm);
        return rc;
}

static __xw_inline
xwer_t xwosdl_smr_wait_unintr(xwid_t smrid)
{
        struct xwsync_smr * smr;
        xwer_t rc;

        smr = xwosdl_smr_get_obj(smrid);
        rc = xwsync_rtsmr_wait_unintr(smr);
        return rc;
}

#elif defined(XWSMPCFG_SYNC_PLSMR) && (1 == XWSMPCFG_SYNC_PLSMR)

static __xw_inline
xwer_t xwosdl_smr_init(struct xwosdl_smr * smr, xwssq_t val, xwssq_t max)
{
        return xwsync_plsmr_init(smr, val, max);
}

static __xw_inline
xwer_t xwosdl_smr_destroy(struct xwosdl_smr * smr)
{
        return xwsync_smr_destroy(smr);
}

static __xw_inline
xwer_t xwosdl_smr_create(xwid_t * smridbuf, xwssq_t val, xwssq_t max)
{
        struct xwosdl_smr * smr;
        xwer_t rc;

        smr = NULL;
        rc = xwsync_smr_create(&smr, XWSYNC_SMR_TYPE_PIPELINE, val, max);
        if (OK == rc) {
                /* FIXME: Add to ID table */
                *smridbuf = (xwid_t)smr;
        } else {
                *smridbuf = (xwid_t)0;
        }
        return rc;
}

static __xw_inline
xwer_t xwosdl_smr_delete(xwid_t smrid)
{
        struct xwsync_smr * smr;
        xwer_t rc;

        smr = xwosdl_smr_get_obj(smrid);
        rc = xwsync_smr_delete(smr);
        if (OK == rc) {
                /* FIXME: Del from ID table */
        }
        return rc;
}

static __xw_inline
xwer_t xwosdl_smr_freeze(xwid_t smrid)
{
        struct xwsync_smr * smr;
        xwer_t rc;

        smr = xwosdl_smr_get_obj(smrid);
        rc = xwsync_plsmr_freeze(smr);
        return rc;
}

static __xw_inline
xwer_t xwosdl_smr_thaw(xwid_t smrid, xwssq_t val, xwssq_t max)
{
        struct xwsync_smr * smr;
        xwer_t rc;

        smr = xwosdl_smr_get_obj(smrid);
        rc = xwsync_plsmr_thaw(smr, val, max);
        return rc;
}

static __xw_inline
xwer_t xwosdl_smr_post(xwid_t smrid)
{
        struct xwsync_smr * smr;
        xwer_t rc;

        smr = xwosdl_smr_get_obj(smrid);
        rc = xwsync_plsmr_post(smr);
        return rc;
}

static __xw_inline
xwer_t xwosdl_smr_wait(xwid_t smrid)
{
        struct xwsync_smr * smr;
        xwer_t rc;

        smr = xwosdl_smr_get_obj(smrid);
        rc = xwsync_plsmr_wait(smr);
        return rc;
}

static __xw_inline
xwer_t xwosdl_smr_trywait(xwid_t smrid)
{
        struct xwsync_smr * smr;
        xwer_t rc;

        smr = xwosdl_smr_get_obj(smrid);
        rc = xwsync_plsmr_trywait(smr);
        return rc;
}

static __xw_inline
xwer_t xwosdl_smr_timedwait(xwid_t smrid, xwtm_t * xwtm)
{
        struct xwsync_smr * smr;
        xwer_t rc;

        smr = xwosdl_smr_get_obj(smrid);
        rc = xwsync_plsmr_timedwait(smr, xwtm);
        return rc;
}

static __xw_inline
xwer_t xwosdl_smr_wait_unintr(xwid_t smrid)
{
        struct xwsync_smr * smr;
        xwer_t rc;

        smr = xwosdl_smr_get_obj(smrid);
        rc = xwsync_plsmr_wait_unintr(smr);
        return rc;
}

#else /* XWSMPCFG_SYNC_PLSMR */
  #error "Can't find the semaphore configuration!"
#endif /* !XWSMPCFG_SYNC_PLSMR */
/******** condition ********/
#include <xwos/smp/sync/condition.h>

#define xwosdl_cdt xwsync_cdt

static __xw_inline
xwid_t xwosdl_cdt_get_id(struct xwosdl_cdt * cdt)
{
        /* FIXME: find ID from obj */
        return (xwid_t)cdt;
}

static __xw_inline
struct xwosdl_cdt * xwosdl_cdt_get_obj(xwid_t cdtid)
{
        /* FIXME: find obj from ID table */
        return (struct xwosdl_cdt *)cdtid;
}

static __xw_inline
xwer_t xwosdl_cdt_init(struct xwosdl_cdt * cdt)
{
        return xwsync_cdt_init(cdt);
}

static __xw_inline
xwer_t xwosdl_cdt_destroy(struct xwosdl_cdt * cdt)
{
        return xwsync_cdt_destroy(cdt);
}

static __xw_inline
xwer_t xwosdl_cdt_create(xwid_t * cdtidbuf)
{
        struct xwosdl_cdt * cdt;
        xwer_t rc;

        cdt = NULL;
        rc = xwsync_cdt_create(&cdt);
        if (OK == rc) {
                /* FIXME: Add to ID table */
                *cdtidbuf = (xwid_t)cdt;
        } else {
                *cdtidbuf = (xwid_t)0;
        }
        return rc;
}

static __xw_inline
xwer_t xwosdl_cdt_delete(xwid_t cdtid)
{
        struct xwosdl_cdt * cdt;
        xwer_t rc;

        cdt = xwosdl_cdt_get_obj(cdtid);
        rc = xwsync_cdt_delete(cdt);
        if (OK == rc) {
                /* FIXME: Del from ID table */
        }
        return rc;
}

static __xw_inline
xwer_t xwosdl_cdt_bind(xwid_t cdtid, xwid_t sltid, xwsq_t pos)
{
        struct xwosdl_selector * slt;
        struct xwosdl_cdt * cdt;
        xwer_t rc;

        slt = xwosdl_selector_get_obj(sltid);
        cdt = xwosdl_cdt_get_obj(cdtid);
        rc = xwsync_cdt_bind(cdt, slt, pos);
        return rc;
}

static __xw_inline
xwer_t xwosdl_cdt_unbind(xwid_t cdtid, xwid_t sltid)
{
        struct xwosdl_selector * slt;
        struct xwosdl_cdt * cdt;
        xwer_t rc;

        slt = xwosdl_selector_get_obj(sltid);
        cdt = xwosdl_cdt_get_obj(cdtid);
        rc = xwsync_cdt_unbind(cdt, slt);
        return rc;
}

static __xw_inline
xwer_t xwosdl_cdt_thaw(xwid_t cdtid)
{
        struct xwosdl_cdt * cdt;
        xwer_t rc;

        cdt = xwosdl_cdt_get_obj(cdtid);
        rc = xwsync_cdt_thaw(cdt);
        return rc;
}

static __xw_inline
xwer_t xwosdl_cdt_freeze(xwid_t cdtid)
{
        struct xwosdl_cdt * cdt;
        xwer_t rc;

        cdt = xwosdl_cdt_get_obj(cdtid);
        rc = xwsync_cdt_freeze(cdt);
        return rc;
}

static __xw_inline
xwer_t xwosdl_cdt_intr_all(xwid_t cdtid)
{
        struct xwosdl_cdt * cdt;
        xwer_t rc;

        cdt = xwosdl_cdt_get_obj(cdtid);
        rc = xwsync_cdt_intr_all(cdt);
        return rc;
}

static __xw_inline
xwer_t xwosdl_cdt_broadcast(xwid_t cdtid)
{
        struct xwosdl_cdt * cdt;
        xwer_t rc;

        cdt = xwosdl_cdt_get_obj(cdtid);
        rc = xwsync_cdt_broadcast(cdt);
        return rc;
}

static __xw_inline
xwer_t xwosdl_cdt_unicast(xwid_t cdtid)
{
        struct xwosdl_cdt * cdt;
        xwer_t rc;

        cdt = xwosdl_cdt_get_obj(cdtid);
        rc = xwsync_cdt_unicast(cdt);
        return rc;
}

static __xw_inline
xwer_t xwosdl_cdt_wait(xwid_t cdtid,
                       union xwlk_ulock lock, xwsq_t lktype,
                       void * lkdata, xwsz_t datanum,
                       xwsq_t * lkst)
{
        struct xwosdl_cdt * cdt;
        union xwlk_ulock oslk;
        xwer_t rc;

        cdt = xwosdl_cdt_get_obj(cdtid);
        /* FIXME: find lock from ID table */
        oslk.anon = lock.anon;
        rc = xwsync_cdt_wait(cdt,
                             oslk.anon, lktype,
                             lkdata, datanum,
                             lkst);
        return rc;
}

static __xw_inline
xwer_t xwosdl_cdt_timedwait(xwid_t cdtid,
                            union xwlk_ulock lock, xwsq_t lktype,
                            void * lkdata, xwsz_t datanum,
                            xwtm_t * xwtm, xwsq_t * lkst)
{
        struct xwosdl_cdt * cdt;
        union xwlk_ulock oslk;
        xwer_t rc;

        cdt = xwosdl_cdt_get_obj(cdtid);
        /* FIXME: find lock from ID table */
        oslk.anon = lock.anon;
        rc = xwsync_cdt_timedwait(cdt,
                                  oslk.anon, lktype,
                                  lkdata, datanum,
                                  xwtm,
                                  lkst);
        return rc;
}

/******** flag ********/
#include <xwos/smp/sync/event.h>

#define xwosdl_flg xwsync_evt

#define XWOSDL_FLG_TRIGGER_SET_ALL      XWSYNC_EVT_TRIGGER_SET_ALL
#define XWOSDL_FLG_TRIGGER_SET_ANY      XWSYNC_EVT_TRIGGER_SET_ANY
#define XWOSDL_FLG_TRIGGER_CLR_ALL      XWSYNC_EVT_TRIGGER_CLR_ALL
#define XWOSDL_FLG_TRIGGER_CLR_ANY      XWSYNC_EVT_TRIGGER_CLR_ANY
#define XWOSDL_FLG_TRIGGER_TGL_ALL      XWSYNC_EVT_TRIGGER_TGL_ALL
#define XWOSDL_FLG_TRIGGER_TGL_ANY      XWSYNC_EVT_TRIGGER_TGL_ANY

#define XWOSDL_FLG_ACTION_NONE          XWSYNC_EVT_ACTION_NONE
#define XWOSDL_FLG_ACTION_CONSUMPTION   XWSYNC_EVT_ACTION_CONSUMPTION

#define XWOSDL_FLG_MAXNUM               XWSYNC_EVT_MAXNUM

#define XWOSDL_FLG_DECLARE_BITMAP(name) XWSYNC_EVT_DECLARE_BITMAP(name)

static __xw_inline
xwid_t xwosdl_flg_get_id(struct xwosdl_flg * flg)
{
        /* FIXME: find ID from obj */
        return (xwid_t)flg;
}

static __xw_inline
struct xwosdl_flg * xwosdl_flg_get_obj(xwid_t flgid)
{
        /* FIXME: find obj from ID table */
        return (struct xwosdl_flg *)flgid;
}

static __xw_inline
xwer_t xwosdl_flg_init(struct xwosdl_flg * flg, xwbmp_t initval[])
{
        return xwsync_evt_init(flg, initval, XWSYNC_EVT_TYPE_FLAG);
}

static __xw_inline
xwer_t xwosdl_flg_destroy(struct xwosdl_flg * flg)
{
        return xwsync_evt_destroy(flg);
}

static __xw_inline
xwer_t xwosdl_flg_create(xwid_t * flgidbuf, xwbmp_t initval[])
{
        struct xwosdl_flg * flg;
        xwer_t rc;

        flg = NULL;
        rc = xwsync_evt_create(&flg, initval, XWSYNC_EVT_TYPE_FLAG);
        if (OK == rc) {
                /* FIXME: Add to ID table */
                *flgidbuf = (xwid_t)flg;
        } else {
                *flgidbuf = (xwid_t)0;
        }
        return rc;
}

static __xw_inline
xwer_t xwosdl_flg_delete(xwid_t flgid)
{
        struct xwosdl_flg * flg;
        xwer_t rc;

        flg = xwosdl_flg_get_obj(flgid);
        rc = xwsync_evt_delete(flg);
        if (OK == rc) {
                /* FIXME: Del from ID table */
        }
        return rc;
}

static __xw_inline
xwer_t xwosdl_flg_bind(xwid_t flgid, xwid_t sltid, xwsq_t pos)
{
        struct xwosdl_selector * slt;
        struct xwosdl_flg * flg;
        xwer_t rc;

        slt = xwosdl_selector_get_obj(sltid);
        flg = xwosdl_flg_get_obj(flgid);
        rc = xwsync_evt_bind(flg, slt, pos);
        return rc;
}

static __xw_inline
xwer_t xwosdl_flg_unbind(xwid_t flgid, xwid_t sltid)
{
        struct xwosdl_selector * slt;
        struct xwosdl_flg * flg;
        xwer_t rc;

        slt = xwosdl_selector_get_obj(sltid);
        flg = xwosdl_flg_get_obj(flgid);
        rc = xwsync_evt_unbind(flg, slt);
        return rc;
}

static __xw_inline
xwer_t xwosdl_flg_intr_all(xwid_t flgid)
{
        struct xwosdl_flg * flg;
        xwer_t rc;

        flg = xwosdl_flg_get_obj(flgid);
        rc = xwsync_evt_intr_all(flg);
        return rc;
}

static __xw_inline
xwer_t xwosdl_flg_s1m(xwid_t flgid, xwbmp_t msk[])
{
        struct xwosdl_flg * flg;
        xwer_t rc;

        flg = xwosdl_flg_get_obj(flgid);
        rc = xwsync_evt_s1m(flg, msk);
        return rc;
}

static __xw_inline
xwer_t xwosdl_flg_s1i(xwid_t flgid, xwsq_t pos)
{
        struct xwosdl_flg * flg;
        xwer_t rc;

        flg = xwosdl_flg_get_obj(flgid);
        rc = xwsync_evt_s1i(flg, pos);
        return rc;
}

static __xw_inline
xwer_t xwosdl_flg_c0m(xwid_t flgid, xwbmp_t msk[])
{
        struct xwosdl_flg * flg;
        xwer_t rc;

        flg = xwosdl_flg_get_obj(flgid);
        rc = xwsync_evt_c0m(flg, msk);
        return rc;
}

static __xw_inline
xwer_t xwosdl_flg_c0i(xwid_t flgid, xwsq_t pos)
{
        struct xwosdl_flg * flg;
        xwer_t rc;

        flg = xwosdl_flg_get_obj(flgid);
        rc = xwsync_evt_c0i(flg, pos);
        return rc;
}

static __xw_inline
xwer_t xwosdl_flg_x1m(xwid_t flgid, xwbmp_t msk[])
{
        struct xwosdl_flg * flg;
        xwer_t rc;

        flg = xwosdl_flg_get_obj(flgid);
        rc = xwsync_evt_x1m(flg, msk);
        return rc;
}

static __xw_inline
xwer_t xwosdl_flg_x1i(xwid_t flgid, xwsq_t pos)
{
        struct xwosdl_flg * flg;
        xwer_t rc;

        flg = xwosdl_flg_get_obj(flgid);
        rc = xwsync_evt_x1i(flg, pos);
        return rc;
}

static __xw_inline
xwer_t xwosdl_flg_read(xwid_t flgid, xwbmp_t out[])
{
        struct xwosdl_flg * flg;
        xwer_t rc;

        flg = xwosdl_flg_get_obj(flgid);
        rc = xwsync_evt_read(flg, out);
        return rc;
}

static __xw_inline
xwer_t xwosdl_flg_trywait(xwid_t flgid, xwsq_t trigger, xwsq_t action,
                          xwbmp_t origin[], xwbmp_t msk[])
{
        struct xwosdl_flg * flg;
        xwer_t rc;

        flg = xwosdl_flg_get_obj(flgid);
        rc = xwsync_evt_trywait(flg, trigger, action, origin, msk);
        return rc;
}

static __xw_inline
xwer_t xwosdl_flg_wait(xwid_t flgid, xwsq_t trigger, xwsq_t action,
                       xwbmp_t origin[], xwbmp_t msk[])
{
        struct xwosdl_flg * flg;
        xwer_t rc;

        flg = xwosdl_flg_get_obj(flgid);
        rc = xwsync_evt_wait(flg, trigger, action, origin, msk);
        return rc;
}

static __xw_inline
xwer_t xwosdl_flg_timedwait(xwid_t flgid, xwsq_t trigger, xwsq_t action,
                            xwbmp_t origin[], xwbmp_t msk[],
                            xwtm_t * xwtm)
{
        struct xwosdl_flg * flg;
        xwer_t rc;

        flg = xwosdl_flg_get_obj(flgid);
        rc = xwsync_evt_timedwait(flg, trigger, action, origin, msk, xwtm);
        return rc;
}

/******** selector ********/
#include <xwos/smp/sync/event.h>

#define XWOSDL_SELECTOR_MAXNUM          XWSYNC_EVT_MAXNUM

static __xw_inline
xwid_t xwosdl_selector_get_id(struct xwosdl_selector * slt)
{
        /* FIXME: find ID from obj */
        return (xwid_t)slt;
}

static __xw_inline
struct xwosdl_selector * xwosdl_selector_get_obj(xwid_t sltid)
{
        /* FIXME: find obj from ID table */
        return (struct xwosdl_selector *)sltid;
}

static __xw_inline
xwer_t xwosdl_selector_init(struct xwosdl_selector * slt)
{
        return xwsync_evt_init(slt, NULL, XWSYNC_EVT_TYPE_SELECTOR);
}

static __xw_inline
xwer_t xwosdl_selector_destroy(struct xwosdl_selector * slt)
{
        return xwsync_evt_destroy(slt);
}

static __xw_inline
xwer_t xwosdl_selector_create(xwid_t * sltidbuf)
{
        struct xwosdl_selector * slt;
        xwer_t rc;

        slt = NULL;
        rc = xwsync_evt_create(&slt, NULL, XWSYNC_EVT_TYPE_SELECTOR);
        if (OK == rc) {
                /* FIXME: Add to ID table */
                *sltidbuf = (xwid_t)slt;
        } else {
                *sltidbuf = (xwid_t)0;
        }
        return rc;
}

static __xw_inline
xwer_t xwosdl_selector_delete(xwid_t sltid)
{
        struct xwosdl_selector * slt;
        xwer_t rc;

        slt = xwosdl_selector_get_obj(sltid);
        rc = xwsync_evt_delete(slt);
        if (OK == rc) {
                /* FIXME: Del from ID table */
        }
        return rc;
}

static __xw_inline
xwer_t xwosdl_selector_bind(xwid_t srcid, xwid_t dstid, xwsq_t pos)
{
        struct xwosdl_selector * src;
        struct xwosdl_selector * dst;
        xwer_t rc;

        src = xwosdl_selector_get_obj(srcid);
        dst = xwosdl_selector_get_obj(dstid);
        rc = xwsync_evt_bind(src, dst, pos);
        return rc;
}

static __xw_inline
xwer_t xwosdl_selector_unbind(xwid_t srcid, xwid_t dstid)
{
        struct xwosdl_selector * src;
        struct xwosdl_selector * dst;
        xwer_t rc;

        src = xwosdl_selector_get_obj(srcid);
        dst = xwosdl_selector_get_obj(dstid);
        rc = xwsync_evt_unbind(src, dst);
        return rc;
}

static __xw_inline
xwer_t xwosdl_selector_intr_all(xwid_t sltid)
{
        struct xwosdl_selector * slt;
        xwer_t rc;

        slt = xwosdl_selector_get_obj(sltid);
        rc = xwsync_evt_intr_all(slt);
        return rc;
}

static __xw_inline
xwer_t xwosdl_selector_tryselect(xwid_t sltid, xwbmp_t msk[], xwbmp_t trg[])
{
        struct xwosdl_selector * slt;
        xwer_t rc;

        slt = xwosdl_selector_get_obj(sltid);
        rc = xwsync_evt_tryselect(slt, msk, trg);
        return rc;
}

static __xw_inline
xwer_t xwosdl_selector_select(xwid_t sltid, xwbmp_t msk[], xwbmp_t trg[])
{
        struct xwosdl_selector * slt;
        xwer_t rc;

        slt = xwosdl_selector_get_obj(sltid);
        rc = xwsync_evt_select(slt, msk, trg);
        return rc;
}

static __xw_inline
xwer_t xwosdl_selector_timedselect(xwid_t sltid, xwbmp_t msk[], xwbmp_t trg[],
                                   xwtm_t * xwtm)
{
        struct xwosdl_selector * slt;
        xwer_t rc;

        slt = xwosdl_selector_get_obj(sltid);
        rc = xwsync_evt_timedselect(slt, msk, trg, xwtm);
        return rc;
}

/******** barrier ********/
#include <xwos/smp/sync/event.h>

#define xwosdl_barrier xwsync_evt
#define XWOSDL_BARRIER_MAXNUM           XWSYNC_EVT_MAXNUM

static __xw_inline
xwid_t xwosdl_barrier_get_id(struct xwosdl_barrier * bar)
{
        /* FIXME: find ID from obj */
        return (xwid_t)bar;
}

static __xw_inline
struct xwosdl_barrier * xwosdl_barrier_get_obj(xwid_t barid)
{
        /* FIXME: find obj from ID table */
        return (struct xwosdl_barrier *)barid;
}

static __xw_inline
xwer_t xwosdl_barrier_init(struct xwosdl_barrier * bar)
{
        return xwsync_evt_init(bar, NULL, XWSYNC_EVT_TYPE_BARRIER);
}

static __xw_inline
xwer_t xwosdl_barrier_destroy(struct xwosdl_barrier * bar)
{
        return xwsync_evt_destroy(bar);
}

static __xw_inline
xwer_t xwosdl_barrier_create(xwid_t * baridbuf)
{
        struct xwosdl_barrier * bar;
        xwer_t rc;

        bar = NULL;
        rc = xwsync_evt_create(&bar, NULL, XWSYNC_EVT_TYPE_BARRIER);
        if (OK == rc) {
                /* FIXME: Add to ID table */
                *baridbuf = (xwid_t)bar;
        } else {
                *baridbuf = (xwid_t)0;
        }
        return rc;
}

static __xw_inline
xwer_t xwosdl_barrier_delete(xwid_t barid)
{
        struct xwosdl_barrier * bar;
        xwer_t rc;

        bar = xwosdl_barrier_get_obj(barid);
        rc = xwsync_evt_delete(bar);
        if (OK == rc) {
                /* FIXME: Del from ID table */
        }
        return rc;
}

static __xw_inline
xwer_t xwosdl_barrier_bind(xwid_t barid, xwid_t sltid, xwsq_t pos)
{
        struct xwosdl_barrier * bar;
        struct xwosdl_selector * slt;
        xwer_t rc;

        bar = xwosdl_barrier_get_obj(barid);
        slt = xwosdl_selector_get_obj(sltid);
        rc = xwsync_evt_bind(bar, slt, pos);
        return rc;
}

static __xw_inline
xwer_t xwosdl_barrier_unbind(xwid_t barid, xwid_t sltid)
{
        struct xwosdl_barrier * bar;
        struct xwosdl_selector * slt;
        xwer_t rc;

        bar = xwosdl_barrier_get_obj(barid);
        slt = xwosdl_selector_get_obj(sltid);
        rc = xwsync_evt_unbind(bar, slt);
        return rc;
}

static __xw_inline
xwer_t xwosdl_barrier_intr_all(xwid_t barid)
{
        struct xwosdl_barrier * bar;
        xwer_t rc;

        bar = xwosdl_barrier_get_obj(barid);
        rc = xwsync_evt_intr_all(bar);
        return rc;
}

static __xw_inline
xwer_t xwosdl_barrier_sync(xwid_t barid, xwsq_t pos, xwbmp_t sync[])
{
        struct xwosdl_barrier * bar;
        xwer_t rc;

        bar = xwosdl_barrier_get_obj(barid);
        rc = xwsync_evt_sync(bar, pos, sync);
        return rc;
}

static __xw_inline
xwer_t xwosdl_barrier_timedsync(xwid_t barid, xwsq_t pos, xwbmp_t sync[],
                                xwtm_t * xwtm)
{
        struct xwosdl_barrier * bar;
        xwer_t rc;

        bar = xwosdl_barrier_get_obj(barid);
        rc = xwsync_evt_timedsync(bar, pos, sync, xwtm);
        return rc;
}

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       lock        ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/******** spinlock ********/
#include <xwos/smp/lock/spinlock.h>

typedef struct xwlk_splk xwosdl_splk_t;

static __xw_inline
void xwosdl_splk_init(xwosdl_splk_t * ossl)
{
        xwlk_splk_init(ossl);
}

static __xw_inline
void xwosdl_splk_lock(xwosdl_splk_t * ossl)
{
        xwlk_splk_lock(ossl);
}

static __xw_inline
xwer_t xwosdl_splk_trylock(xwosdl_splk_t * ossl)
{
        return xwlk_splk_trylock(ossl);
}

static __xw_inline
void xwosdl_splk_unlock(xwosdl_splk_t * ossl)
{
        xwlk_splk_unlock(ossl);
}

static __xw_inline
void xwosdl_splk_lock_cpuirq(xwosdl_splk_t * ossl)
{
        xwlk_splk_lock_cpuirq(ossl);
}

static __xw_inline
xwer_t xwosdl_splk_trylock_cpuirq(xwosdl_splk_t * ossl)
{
        return xwlk_splk_trylock_cpuirq(ossl);
}

static __xw_inline
void xwosdl_splk_unlock_cpuirq(xwosdl_splk_t * ossl)
{
        xwlk_splk_unlock_cpuirq(ossl);
}

static __xw_inline
void xwosdl_splk_lock_cpuirqsv(xwosdl_splk_t * ossl, xwreg_t * cpuirq)
{
        xwlk_splk_lock_cpuirqsv(ossl, cpuirq);
}

static __xw_inline
xwer_t xwosdl_splk_trylock_cpuirqsv(xwosdl_splk_t * ossl, xwreg_t * cpuirq)
{
        return xwlk_splk_trylock_cpuirqsv(ossl, cpuirq);
}

static __xw_inline
void xwosdl_splk_unlock_cpuirqrs(xwosdl_splk_t * ossl, xwreg_t cpuirq)
{
        xwlk_splk_unlock_cpuirqrs(ossl, cpuirq);
}

static __xw_inline
void xwosdl_splk_lock_irqs(xwosdl_splk_t * ossl,
                           const struct xwos_irq_resource * irqs, xwsz_t num)
{
        xwlk_splk_lock_irqs(ossl, irqs, num);
}

static __xw_inline
xwer_t xwosdl_splk_trylock_irqs(xwosdl_splk_t * ossl,
                                const struct xwos_irq_resource * irqs, xwsz_t num)
{
        return xwlk_splk_trylock_irqs(ossl, irqs, num);
}

static __xw_inline
void xwosdl_splk_unlock_irqs(xwosdl_splk_t * ossl,
                             const struct xwos_irq_resource * irqs, xwsz_t num)
{
        xwlk_splk_unlock_irqs(ossl, irqs, num);
}

static __xw_inline
void xwosdl_splk_lock_irqssv(xwosdl_splk_t * ossl,
                             const struct xwos_irq_resource * irqs,
                             xwreg_t flags[], xwsz_t num)
{
        xwlk_splk_lock_irqssv(ossl, irqs, flags, num);
}

static __xw_inline
xwer_t xwosdl_splk_trylock_irqssv(xwosdl_splk_t * ossl,
                                  const struct xwos_irq_resource * irqs,
                                  xwreg_t flags[], xwsz_t num)
{
        return xwlk_splk_trylock_irqssv(ossl, irqs, flags, num);
}

static __xw_inline
void xwosdl_splk_unlock_irqsrs(xwosdl_splk_t * ossl,
                               const struct xwos_irq_resource * irqs,
                               xwreg_t flags[], xwsz_t num)
{
        xwlk_splk_unlock_irqsrs(ossl, irqs, flags, num);
}

static __xw_inline
void xwosdl_splk_lock_bh(xwosdl_splk_t * ossl)
{
        xwlk_splk_lock_bh(ossl);
}

static __xw_inline
xwer_t xwosdl_splk_trylock_bh(xwosdl_splk_t * ossl)
{
        return xwlk_splk_trylock_bh(ossl);
}

static __xw_inline
void xwosdl_splk_unlock_bh(xwosdl_splk_t * ossl)
{
        xwlk_splk_unlock_bh(ossl);
}

/******** seqlock ********/
#include <xwos/smp/lock/seqlock.h>

#define XWOSDL_SQLK_GRANULARITY          XWLK_SQLK_GRANULARITY

typedef struct xwlk_sqlk xwosdl_sqlk_t;

static __xw_inline
void xwosdl_sqlk_init(xwosdl_sqlk_t * ossql)
{
        xwlk_sqlk_init(ossql);
}

static __xw_inline
xwsq_t xwosdl_sqlk_rd_begin(xwosdl_sqlk_t * ossql)
{
        return xwlk_sqlk_rd_begin(ossql);
}

static __xw_inline
bool xwosdl_sqlk_rd_retry(xwosdl_sqlk_t * ossql, xwsq_t start)
{
        return xwlk_sqlk_rd_retry(ossql, start);
}

static __xw_inline
xwsq_t xwosdl_sqlk_get_seq(xwosdl_sqlk_t * ossql)
{
        return xwlk_sqlk_get_seq(ossql);
}

static __xw_inline
void xwosdl_sqlk_rdex_lock(xwosdl_sqlk_t * ossql)
{
        xwlk_sqlk_rdex_lock(ossql);
}

static __xw_inline
xwer_t xwosdl_sqlk_rdex_trylock(xwosdl_sqlk_t * ossql)
{
        return xwlk_sqlk_rdex_trylock(ossql);
}

static __xw_inline
void xwosdl_sqlk_rdex_unlock(xwosdl_sqlk_t * ossql)
{
        xwlk_sqlk_rdex_unlock(ossql);
}

static __xw_inline
void xwosdl_sqlk_rdex_lock_cpuirq(xwosdl_sqlk_t * ossql)
{
        xwlk_sqlk_rdex_lock_cpuirq(ossql);
}

static __xw_inline
xwer_t xwosdl_sqlk_rdex_trylock_cpuirq(xwosdl_sqlk_t * ossql)
{
        return xwlk_sqlk_rdex_trylock_cpuirq(ossql);
}

static __xw_inline
void xwosdl_sqlk_rdex_unlock_cpuirq(xwosdl_sqlk_t * ossql)
{
        xwlk_sqlk_rdex_unlock_cpuirq(ossql);
}

static __xw_inline
void xwosdl_sqlk_rdex_lock_cpuirqsv(xwosdl_sqlk_t * ossql, xwreg_t * cpuirq)
{
        xwlk_sqlk_rdex_lock_cpuirqsv(ossql, cpuirq);
}

static __xw_inline
xwer_t xwosdl_sqlk_rdex_trylock_cpuirqsv(xwosdl_sqlk_t * ossql, xwreg_t * cpuirq)
{
        return xwlk_sqlk_rdex_trylock_cpuirqsv(ossql, cpuirq);
}

static __xw_inline
void xwosdl_sqlk_rdex_unlock_cpuirqrs(xwosdl_sqlk_t * ossql, xwreg_t cpuirq)
{
        xwlk_sqlk_rdex_unlock_cpuirqrs(ossql, cpuirq);
}

static __xw_inline
void xwosdl_sqlk_rdex_lock_irqs(xwosdl_sqlk_t * ossql,
                                const struct xwos_irq_resource * irqs,
                                xwsz_t num)
{
        xwlk_sqlk_rdex_lock_irqs(ossql, irqs, num);
}

static __xw_inline
xwer_t xwosdl_sqlk_rdex_trylock_irqs(xwosdl_sqlk_t * ossql,
                                     const struct xwos_irq_resource * irqs,
                                     xwsz_t num)
{
        return xwlk_sqlk_rdex_trylock_irqs(ossql, irqs, num);
}

static __xw_inline
void xwosdl_sqlk_rdex_unlock_irqs(xwosdl_sqlk_t * ossql,
                                  const struct xwos_irq_resource * irqs,
                                  xwsz_t num)
{
        xwlk_sqlk_rdex_unlock_irqs(ossql, irqs, num);
}

static __xw_inline
void xwosdl_sqlk_rdex_lock_irqssv(xwosdl_sqlk_t * ossql,
                                  const struct xwos_irq_resource * irqs,
                                  xwreg_t flags[], xwsz_t num)
{
        xwlk_sqlk_rdex_lock_irqssv(ossql, irqs, flags, num);
}

static __xw_inline
xwer_t xwosdl_sqlk_rdex_trylock_irqssv(xwosdl_sqlk_t * ossql,
                                       const struct xwos_irq_resource * irqs,
                                       xwreg_t flags[], xwsz_t num)
{
        return xwlk_sqlk_rdex_trylock_irqssv(ossql, irqs, flags, num);
}

static __xw_inline
void xwosdl_sqlk_rdex_unlock_irqsrs(xwosdl_sqlk_t * ossql,
                                    const struct xwos_irq_resource * irqs,
                                    xwreg_t flags[], xwsz_t num)
{
        xwlk_sqlk_rdex_unlock_irqsrs(ossql, irqs, flags, num);
}

static __xw_inline
void xwosdl_sqlk_rdex_lock_bh(xwosdl_sqlk_t * ossql)
{
        xwlk_sqlk_rdex_lock_bh(ossql);
}

static __xw_inline
xwer_t xwosdl_sqlk_rdex_trylock_bh(xwosdl_sqlk_t * ossql)
{
        return xwlk_sqlk_rdex_trylock_bh(ossql);
}

static __xw_inline
void xwosdl_sqlk_rdex_unlock_bh(xwosdl_sqlk_t * ossql)
{
        xwlk_sqlk_rdex_unlock_bh(ossql);
}

static __xw_inline
void xwosdl_sqlk_wr_lock(xwosdl_sqlk_t * ossql)
{
        xwlk_sqlk_wr_lock(ossql);
}

static __xw_inline
xwer_t xwosdl_sqlk_wr_trylock(xwosdl_sqlk_t * ossql)
{
        return xwlk_sqlk_wr_trylock(ossql);
}

static __xw_inline
void xwosdl_sqlk_wr_unlock(xwosdl_sqlk_t * ossql)
{
        xwlk_sqlk_wr_unlock(ossql);
}

static __xw_inline
void xwosdl_sqlk_wr_lock_cpuirq(xwosdl_sqlk_t * ossql)
{
        xwlk_sqlk_wr_lock_cpuirq(ossql);
}

static __xw_inline
xwer_t xwosdl_sqlk_wr_trylock_cpuirq(xwosdl_sqlk_t * ossql)
{
        return xwlk_sqlk_wr_trylock_cpuirq(ossql);
}

static __xw_inline
void xwosdl_sqlk_wr_unlock_cpuirq(xwosdl_sqlk_t * ossql)
{
        xwlk_sqlk_wr_unlock_cpuirq(ossql);
}

static __xw_inline
void xwosdl_sqlk_wr_lock_cpuirqsv(xwosdl_sqlk_t * ossql, xwreg_t * cpuirq)
{
        xwlk_sqlk_wr_lock_cpuirqsv(ossql, cpuirq);
}


static __xw_inline
xwer_t xwosdl_sqlk_wr_trylock_cpuirqsv(xwosdl_sqlk_t * ossql, xwreg_t * cpuirq)
{
        return xwlk_sqlk_wr_trylock_cpuirqsv(ossql, cpuirq);
}

static __xw_inline
void xwosdl_sqlk_wr_unlock_cpuirqrs(xwosdl_sqlk_t * ossql, xwreg_t cpuirq)
{
        xwlk_sqlk_wr_unlock_cpuirqrs(ossql, cpuirq);
}

static __xw_inline
void xwosdl_sqlk_wr_lock_irqs(xwosdl_sqlk_t * ossql,
                              const struct xwos_irq_resource * irqs,
                              xwsz_t num)
{
        xwlk_sqlk_wr_lock_irqs(ossql, irqs, num);
}

static __xw_inline
xwer_t xwosdl_sqlk_wr_trylock_irqs(xwosdl_sqlk_t * ossql,
                                   const struct xwos_irq_resource * irqs,
                                   xwsz_t num)
{
        return xwlk_sqlk_wr_trylock_irqs(ossql, irqs, num);
}

static __xw_inline
void xwosdl_sqlk_wr_unlock_irqs(xwosdl_sqlk_t * ossql,
                                const struct xwos_irq_resource * irqs,
                                xwsz_t num)
{
        xwlk_sqlk_wr_unlock_irqs(ossql, irqs, num);
}

static __xw_inline
void xwosdl_sqlk_wr_lock_irqssv(xwosdl_sqlk_t * ossql,
                                const struct xwos_irq_resource * irqs,
                                xwreg_t flags[], xwsz_t num)
{
        xwlk_sqlk_wr_lock_irqssv(ossql, irqs, flags, num);
}

static __xw_inline
xwer_t xwosdl_sqlk_wr_trylock_irqssv(xwosdl_sqlk_t * ossql,
                                     const struct xwos_irq_resource * irqs,
                                     xwreg_t flags[], xwsz_t num)
{
        return xwlk_sqlk_wr_trylock_irqssv(ossql, irqs, flags, num);
}

static __xw_inline
void xwosdl_sqlk_wr_unlock_irqsrs(xwosdl_sqlk_t * ossql,
                                  const struct xwos_irq_resource * irqs,
                                  xwreg_t flags[], xwsz_t num)
{
        xwlk_sqlk_wr_unlock_irqsrs(ossql, irqs, flags, num);
}

static __xw_inline
void xwosdl_sqlk_wr_lock_bh(xwosdl_sqlk_t * ossql)
{
        xwlk_sqlk_wr_lock_bh(ossql);
}

static __xw_inline
xwer_t xwosdl_sqlk_wr_trylock_bh(xwosdl_sqlk_t * ossql)
{
        return xwlk_sqlk_wr_trylock_bh(ossql);
}

static __xw_inline
void xwosdl_sqlk_wr_unlock_bh(xwosdl_sqlk_t * ossql)
{
        xwlk_sqlk_wr_unlock_bh(ossql);
}

/******** mutex ********/
#include <xwos/smp/lock/mutex.h>

#define xwosdl_mtx xwlk_mtx

static __xw_inline
xwid_t xwosdl_mtx_get_id(struct xwosdl_mtx * mtx)
{
        /* FIXME: find ID from obj */
        return (xwid_t)mtx;
}

static __xw_inline
struct xwosdl_mtx * xwosdl_mtx_get_obj(xwid_t mtxid)
{
        /* FIXME: find obj from ID table */
        return (struct xwosdl_mtx *)mtxid;
}

static __xw_inline
xwer_t xwosdl_mtx_init(struct xwosdl_mtx * mtx, xwpr_t sprio)
{
        return xwlk_mtx_init(mtx, sprio);
}

static __xw_inline
xwer_t xwosdl_mtx_destroy(struct xwosdl_mtx * mtx)
{
        return xwlk_mtx_destroy(mtx);
}

static __xw_inline
xwer_t xwosdl_mtx_create(xwid_t * mtxidbuf, xwpr_t sprio)
{
        struct xwosdl_mtx * mtx;
        xwer_t rc;

        mtx = NULL;
        rc = xwlk_mtx_create(&mtx, sprio);
        if (OK == rc) {
                /* FIXME: Add to ID table */
                *mtxidbuf = (xwid_t)mtx;
        } else {
                *mtxidbuf = (xwid_t)0;
        }
        return rc;
}

static __xw_inline
xwer_t xwosdl_mtx_delete(xwid_t mtxid)
{
        struct xwlk_mtx * mtx;
        xwer_t rc;

        mtx = xwosdl_mtx_get_obj(mtxid);
        rc = xwlk_mtx_delete(mtx);
        if (OK == rc) {
                /* FIXME: Del from ID table */
        }
        return rc;
}

static __xw_inline
xwer_t xwosdl_mtx_unlock(xwid_t mtxid)
{
        struct xwlk_mtx * mtx;
        xwer_t rc;

        mtx = xwosdl_mtx_get_obj(mtxid);
        rc = xwlk_mtx_unlock(mtx);
        return rc;
}

static __xw_inline
xwer_t xwosdl_mtx_lock(xwid_t mtxid)
{
        struct xwlk_mtx * mtx;
        xwer_t rc;

        mtx = xwosdl_mtx_get_obj(mtxid);
        rc = xwlk_mtx_lock(mtx);
        return rc;
}

static __xw_inline
xwer_t xwosdl_mtx_trylock(xwid_t mtxid)
{
        struct xwlk_mtx * mtx;
        xwer_t rc;

        mtx = xwosdl_mtx_get_obj(mtxid);
        rc = xwlk_mtx_trylock(mtx);
        return rc;
}

static __xw_inline
xwer_t xwosdl_mtx_timedlock(xwid_t mtxid, xwtm_t * xwtm)
{
        struct xwlk_mtx * mtx;
        xwer_t rc;

        mtx = xwosdl_mtx_get_obj(mtxid);
        rc = xwlk_mtx_timedlock(mtx, xwtm);
        return rc;
}

static __xw_inline
xwer_t xwosdl_mtx_lock_unintr(xwid_t mtxid)
{
        struct xwlk_mtx * mtx;
        xwer_t rc;

        mtx = xwosdl_mtx_get_obj(mtxid);
        rc = xwlk_mtx_lock_unintr(mtx);
        return rc;
}

#endif /* xwos/osdl/smp.h */
