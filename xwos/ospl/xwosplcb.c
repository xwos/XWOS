/**
 * @file
 * @brief 玄武OS移植层：回调
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <xwos/standard.h>
#include <xwos/ospl/skd.h>
#include <xwos/ospl/syshwt.h>

#if defined(XuanWuOS_CFG_CORE__mp)
/******** skd ********/
__xwos_code
xwer_t xwosplcb_skd_init_lc(void)
{
        return xwmp_skd_init_lc();
}

__xwos_code
struct xwospl_skd * xwosplcb_skd_get_lc(void)
{
        return xwmp_skd_get_lc();
}

__xwos_code
struct xwospl_skd * xwosplcb_skd_pre_swcx_lic(struct xwospl_skd * xwskd)
{
        return xwmp_skd_pre_swcx_lic(xwskd);
}

__xwos_code
struct xwospl_skd * xwosplcb_skd_post_swcx_lic(struct xwospl_skd * xwskd)
{
        return xwmp_skd_post_swcx_lic(xwskd);
}

__xwos_code
xwer_t xwosplcb_skd_suspend_lic(struct xwospl_skd * xwskd)
{
        return xwmp_skd_suspend_lic(xwskd);
}

__xwos_code
xwer_t xwosplcb_skd_resume_lic(struct xwospl_skd * xwskd)
{
        return xwmp_skd_resume_lic(xwskd);
}

__xwos_code
xwer_t xwosplcb_thd_exit_lic(struct xwospl_thd * thd, xwer_t rc)
{
        return xwmp_thd_exit_lic(thd, rc);
}

__xwos_code
xwer_t xwosplcb_thd_freeze_lic(struct xwospl_thd * thd)
{
        return xwmp_thd_freeze_lic(thd);
}

__xwos_code
void xwosplcb_thd_immigrate_lic(struct xwospl_thd * thd)
{
        return xwmp_thd_immigrate_lic(thd);
}

__xwos_code
xwer_t xwosplcb_thd_outmigrate_lic(struct xwospl_thd * thd, xwid_t dstcpu)
{
        return xwmp_thd_outmigrate_lic(thd, dstcpu);
}

/******** syshwt ********/
__xwos_code
void xwosplcb_syshwt_task(struct xwospl_syshwt * hwt)
{
        xwmp_syshwt_task(hwt);
}

#elif defined(XuanWuOS_CFG_CORE__up)
/******** skd ********/
__xwos_code
xwer_t xwosplcb_skd_init_lc(void)
{
        return xwup_skd_init_lc();
}

__xwos_code
struct xwospl_skd * xwosplcb_skd_get_lc(void)
{
        return xwup_skd_get_lc();
}

__xwos_code
struct xwospl_skd * xwosplcb_skd_pre_swcx_lic(struct xwospl_skd * xwskd)
{
        return xwup_skd_pre_swcx_lic(xwskd);
}

__xwos_code
struct xwospl_skd * xwosplcb_skd_post_swcx_lic(struct xwospl_skd * xwskd)
{
        return xwup_skd_post_swcx_lic(xwskd);
}

__xwos_code
xwer_t xwosplcb_skd_suspend_lic(struct xwospl_skd * xwskd)
{
        return xwup_skd_suspend_lic(xwskd);
}

__xwos_code
xwer_t xwosplcb_skd_resume_lic(struct xwospl_skd * xwskd)
{
        return xwup_skd_resume_lic(xwskd);
}

__xwos_code
xwer_t xwosplcb_thd_exit_lic(struct xwospl_thd * thd, xwer_t rc)
{
        return xwup_thd_exit_lic(thd, rc);
}

__xwos_code
xwer_t xwosplcb_thd_freeze_lic(struct xwospl_thd * thd)
{
        return xwup_thd_freeze_lic(thd);
}

/******** syshwt ********/
__xwos_code
void xwosplcb_syshwt_task(struct xwospl_syshwt * hwt)
{
        xwup_syshwt_task(hwt);
}

#endif
