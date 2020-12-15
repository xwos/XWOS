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
#define XWOSDL_SKDATTR_DETACHED                 XWMP_SKDATTR_DETACHED
#define XWOSDL_SKDATTR_JOINABLE                 XWMP_SKDATTR_JOINABLE
#define XWOSDL_SKD_PRIORITY_RT_MIN              XWMP_SKD_PRIORITY_RT_MIN
#define XWOSDL_SKD_PRIORITY_RT_MAX              XWMP_SKD_PRIORITY_RT_MAX
#define XWOSDL_SKD_PRIORITY_INVALID             XWMP_SKD_PRIORITY_INVALID
#define XWOSDL_SKD_PRIORITY_RAISE(base, inc)    XWMP_SKD_PRIORITY_RAISE(base, inc)
#define XWOSDL_SKD_PRIORITY_DROP(base, dec)     XWMP_SKD_PRIORITY_DROP(base, dec)

#define XWOSDL_SKD_CONTEXT_INIT_EXIT            XWMP_SKD_CONTEXT_INIT_EXIT
#define XWOSDL_SKD_CONTEXT_THD                  XWMP_SKD_CONTEXT_THD
#define XWOSDL_SKD_CONTEXT_ISR                  XWMP_SKD_CONTEXT_ISR
#define XWOSDL_SKD_CONTEXT_BH                   XWMP_SKD_CONTEXT_BH
#define XWOSDL_SKD_CONTEXT_IDLE                 XWMP_SKD_CONTEXT_IDLE

#if defined(XWMPCFG_SKD_THD_LOCAL_DATA_NUM)
  #define XWOSDL_THD_LOCAL_DATA_NUM             XWMPCFG_SKD_THD_LOCAL_DATA_NUM
#else
  #define XWOSDL_THD_LOCAL_DATA_NUM            (0U)
#endif

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
xwid_t xwosdl_skd_id_lc(void)
{
        return xwmp_skd_id_lc();
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

#include <xwos/mp/thd.h>

#define xwosdl_thd xwmp_thd
typedef xwmp_thd_f xwosdl_thd_f;
typedef xwmp_thd_d xwosdl_thd_d;
#define XWOSDL_THD_NILD XWMP_THD_NILD

static __xwcc_inline
xwer_t xwosdl_thd_init(struct xwosdl_thd * thd,
                       xwosdl_thd_d * thdd,
                       const char * name,
                       xwosdl_thd_f mainfunc, void * arg,
                       xwstk_t * stack, xwsz_t stack_size,
                       xwpr_t priority, xwsq_t attr)
{
        xwer_t rc;

        rc = xwmp_thd_init(thd,
                           name,
                           (xwmp_thd_f)mainfunc, arg,
                           stack, stack_size,
                           priority, attr);
        if (XWOK == rc) {
                if (thdd) {
                        thdd->thd = thd;
                        thdd->ticket = thd->xwobj.ticket;
                }
        } else {
                if (thdd) {
                        *thdd = XWOSDL_THD_NILD;
                }
        }
        return rc;
}

static __xwcc_inline
xwer_t xwosdl_thd_destroy(xwosdl_thd_d thdd)
{
        xwer_t rc;

        rc = xwmp_thd_grab_safely(thdd);
        if (XWOK == rc) {
                xwmp_thd_put(thdd.thd);
                rc = xwmp_thd_destroy(thdd.thd);
        }
        return rc;
}

static __xwcc_inline
xwer_t xwosdl_thd_create(xwosdl_thd_d * thdd, const char * name,
                         xwosdl_thd_f mainfunc, void * arg,
                         xwsz_t stack_size,
                         xwpr_t priority, xwsq_t attr)
{
        struct xwmp_thd * thd;
        xwer_t rc;

        if (NULL != thdd) {
                rc = xwmp_thd_create(&thd,
                                     name,
                                     (xwmp_thd_f)mainfunc, arg,
                                     stack_size,
                                     priority, attr);
                if (XWOK == rc) {
                        thdd->thd = thd;
                        thdd->ticket = thd->xwobj.ticket;
                } else {
                        *thdd = XWOSDL_THD_NILD;
                }
        } else {
                rc = EFAULT;
        }
        return rc;
}

static __xwcc_inline
xwer_t xwosdl_thd_delete(xwosdl_thd_d thdd)
{
        xwer_t rc;

        rc = xwmp_thd_grab_safely(thdd);
        if (XWOK == rc) {
                xwmp_thd_put(thdd.thd);
                rc = xwmp_thd_delete(thdd.thd);
        }
        return rc;
}

static __xwcc_inline
xwosdl_thd_d xwosdl_cthd_getd(void)
{
        struct xwmp_thd * cthd;
        xwosdl_thd_d thdd;

        cthd = xwmp_skd_get_cthd_lc();
        thdd.thd = cthd;
        thdd.ticket = cthd->xwobj.ticket;
        return thdd;
}

static __xwcc_inline
xwosdl_thd_d xwosdl_thd_getd(struct xwosdl_thd * thd)
{
        xwosdl_thd_d thdd;

        if (thd) {
                thdd.ticket = thd->xwobj.ticket;
        } else {
                thdd.ticket = 0;
        }
        thdd.thd = thd;
        return thdd;
}

static __xwcc_inline
void xwosdl_cthd_yield(void)
{
        return xwmp_cthd_yield();
}

static __xwcc_inline
void xwosdl_cthd_exit(xwer_t rc)
{
        xwmp_cthd_exit(rc);
}

static __xwcc_inline
xwer_t xwosdl_thd_stop(xwosdl_thd_d thdd, xwer_t * trc)
{
        xwer_t rc;

        rc = xwmp_thd_grab_safely(thdd);
        if (XWOK == rc) {
                rc = xwmp_thd_stop(thdd.thd, trc);
                xwmp_thd_put(thdd.thd);
        }
        return rc;
}

static __xwcc_inline
xwer_t xwosdl_thd_cancel(xwosdl_thd_d thdd)
{
        xwer_t rc;

        rc = xwmp_thd_grab_safely(thdd);
        if (XWOK == rc) {
                rc = xwmp_thd_cancel(thdd.thd);
                xwmp_thd_put(thdd.thd);
        }
        return rc;
}

static __xwos_inline_api
xwer_t xwosdl_thd_join(xwosdl_thd_d thdd, xwer_t * trc)
{
        xwer_t rc;

        rc = xwmp_thd_grab_safely(thdd);
        if (XWOK == rc) {
                rc = xwmp_thd_join(thdd.thd, trc);
                xwmp_thd_put(thdd.thd);
        }
        return rc;
}

static __xwcc_inline
xwer_t xwosdl_thd_detach(xwosdl_thd_d thdd)
{
        xwer_t rc;

        rc = xwmp_thd_grab_safely(thdd);
        if (XWOK == rc) {
                rc = xwmp_thd_detach(thdd.thd);
                xwmp_thd_put(thdd.thd);
        }
        return rc;
}

static __xwcc_inline
bool xwosdl_cthd_shld_frz(void)
{
        return xwmp_cthd_shld_frz();
}

static __xwcc_inline
bool xwosdl_cthd_shld_stop(void)
{
        return xwmp_cthd_shld_stop();
}

static __xwcc_inline
bool xwosdl_cthd_frz_shld_stop(bool * frozen)
{
        return xwmp_cthd_frz_shld_stop(frozen);
}

static __xwcc_inline
xwer_t xwosdl_cthd_sleep(xwtm_t * xwtm)
{
        return xwmp_cthd_sleep(xwtm);
}

static __xwcc_inline
xwer_t xwosdl_cthd_sleep_from(xwtm_t * origin, xwtm_t inc)
{
        return xwmp_cthd_sleep_from(origin, inc);
}

static __xwcc_inline
xwer_t xwosdl_thd_intr(xwosdl_thd_d thdd)
{
        xwer_t rc;

        rc = xwmp_thd_grab_safely(thdd);
        if (XWOK == rc) {
                rc = xwmp_thd_intr(thdd.thd);
                xwmp_thd_put(thdd.thd);
        }
        return rc;
}

static __xwcc_inline
xwer_t xwosdl_cthd_freeze(void)
{
        return xwmp_cthd_freeze();
}

static __xwcc_inline
xwer_t xwosdl_thd_migrate(xwosdl_thd_d thdd, xwid_t dstcpu)
{
        xwer_t rc;

        rc = xwmp_thd_grab_safely(thdd);
        if (XWOK == rc) {
                rc = xwmp_thd_migrate(thdd.thd, dstcpu);
                xwmp_thd_put(thdd.thd);
        }
        return rc;
}

#if defined(XWMPCFG_SKD_THD_LOCAL_DATA_NUM) && (XWMPCFG_SKD_THD_LOCAL_DATA_NUM > 0U)
static __xwos_inline_api
xwer_t xwosdl_thd_set_data(xwosdl_thd_d thdd, xwsq_t pos, void * data)
{
        xwer_t rc;

        rc = xwmp_thd_grab_safely(thdd);
        if (XWOK == rc) {
                rc = xwmp_thd_set_data(thdd.thd, pos, data);
                xwmp_thd_put(thdd.thd);
        }
        return rc;
}

static __xwos_inline_api
xwer_t xwosdl_thd_get_data(xwosdl_thd_d thdd, xwsq_t pos, void ** databuf)
{
        xwer_t rc;

        rc = xwmp_thd_grab_safely(thdd);
        if (XWOK == rc) {
                rc = xwmp_thd_get_data(thdd.thd, pos, databuf);
                xwmp_thd_put(thdd.thd);
        }
        return rc;
}

static __xwos_inline_api
xwer_t xwosdl_cthd_set_data(xwsq_t pos, void * data)
{
        return xwmp_cthd_set_data(pos, data);
}

static __xwos_inline_api
xwer_t xwosdl_cthd_get_data(xwsq_t pos, void ** databuf)
{
        return xwmp_cthd_get_data(pos, databuf);

}
#endif /* XWMPCFG_SKD_THD_LOCAL_DATA_NUM */

#endif /* xwos/mp/osdl/skd.h */
