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

#include <xwos/standard.h>
#include <xwos/up/osdl/skd.h>

__xwup_code
xwer_t xwosdl_skd_continue_lc(void)
{
        xwer_t rc;

        rc = xwup_skd_start_syshwt_lc();
        if (XWOK == rc) {
#if defined(XWOSCFG_SKD_BH) && (1 == XWOSCFG_SKD_BH)
                xwup_skd_enbh_lc(); // cppcheck-suppress [misra-c2012-17.7]
#endif
                xwup_skd_enpmpt_lc(); // cppcheck-suppress [misra-c2012-17.7]
        }
        return rc;
}

__xwup_code
xwer_t xwosdl_skd_pause_lc(void)
{
        xwer_t rc;

        xwup_skd_dspmpt_lc(); // cppcheck-suppress [misra-c2012-17.7]
#if defined(XWOSCFG_SKD_BH) && (1 == XWOSCFG_SKD_BH)
        xwup_skd_dsbh_lc(); // cppcheck-suppress [misra-c2012-17.7]
#endif
        rc = xwup_skd_stop_syshwt_lc();
        if (rc < 0) {
#if defined(XWOSCFG_SKD_BH) && (1 == XWOSCFG_SKD_BH)
                xwup_skd_enbh_lc(); // cppcheck-suppress [misra-c2012-17.7]
#endif
                xwup_skd_enpmpt_lc(); // cppcheck-suppress [misra-c2012-17.7]
        }
        return rc;
}
