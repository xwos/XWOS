/**
 * @file
 * @brief XWOS UP内核操作系统接口描述层：调度器
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef __xwos_up_osdl_skd_h__
#define __xwos_up_osdl_skd_h__

#include <xwos/up/skd.h>

#define XWOSDL_SKD_PRIORITY_RT_MIN              XWUP_SKD_PRIORITY_RT_MIN
#define XWOSDL_SKD_PRIORITY_RT_MAX              XWUP_SKD_PRIORITY_RT_MAX
#define XWOSDL_SKD_PRIORITY_INVALID             XWUP_SKD_PRIORITY_INVALID
#define XWOSDL_SKD_PRIORITY_RAISE(base, inc)    XWUP_SKD_PRIORITY_RAISE(base, inc)
#define XWOSDL_SKD_PRIORITY_DROP(base, dec)     XWUP_SKD_PRIORITY_DROP(base, dec)

#define XWOSDL_SKD_CONTEXT_BOOT                 XWUP_SKD_CONTEXT_BOOT
#define XWOSDL_SKD_CONTEXT_THD                  XWUP_SKD_CONTEXT_THD
#define XWOSDL_SKD_CONTEXT_ISR                  XWUP_SKD_CONTEXT_ISR
#define XWOSDL_SKD_CONTEXT_BH                   XWUP_SKD_CONTEXT_BH
#define XWOSDL_SKD_CONTEXT_IDLE                 XWUP_SKD_CONTEXT_IDLE

#define XWOSDL_SKD_TC_PER_S                     (XWTM_MS(1000) / XWOSCFG_SYSHWT_PERIOD)

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
xwid_t xwosdl_skd_get_cpuid_lc(void)
{
        return 0;
}

static __xwcc_inline
void xwosdl_skd_get_context_lc(xwsq_t * ctxbuf, xwirq_t * irqnbuf)
{
        xwup_skd_get_context_lc(ctxbuf, irqnbuf);
}

static __xwcc_inline
xwtm_t xwosdl_skd_get_time_lc(void)
{
        struct xwup_skd * xwskd;

        xwskd = xwup_skd_get_lc();
        return xwup_syshwt_get_time(&xwskd->tt.hwt);
}

static __xwcc_inline
xwtk_t xwosdl_skd_get_tick_lc(void)
{
        struct xwup_skd * xwskd;

        xwskd = xwup_skd_get_lc();
        return xwup_syshwt_get_tick(&xwskd->tt.hwt);
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
        // cppcheck-suppress [misra-c2012-17.7]
        xwup_skd_dspmpt_lc();
}

static __xwcc_inline
void xwosdl_skd_enpmpt_lc(void)
{
        // cppcheck-suppress [misra-c2012-17.7]
        xwup_skd_enpmpt_lc();
}

static __xwcc_inline
void xwosdl_skd_dsbh_lc(void)
{
        // cppcheck-suppress [misra-c2012-17.7]
        xwup_skd_dsbh_lc();
}

static __xwcc_inline
void xwosdl_skd_enbh_lc(void)
{
        // cppcheck-suppress [misra-c2012-17.7]
        xwup_skd_enbh_lc();
}

xwer_t xwosdl_skd_continue_lc(void);
xwer_t xwosdl_skd_pause_lc(void);


#endif /* xwos/up/osdl/skd.h */
