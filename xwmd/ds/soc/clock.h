/**
 * @file
 * @brief xwmd设备栈：SOC时钟
 * @author
 * + 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __xwmd_ds_soc_clock_h__
#define __xwmd_ds_soc_clock_h__

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwmd/ds/standard.h>
#include <xwmd/ds/soc/chip.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      macors       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         function prototypes         ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
__xwds_api
xwer_t xwds_clk_req(struct xwds_soc * soc, xwid_t id);

__xwds_api
xwer_t xwds_clk_rls(struct xwds_soc * soc, xwid_t id);

__xwds_api
xwer_t xwds_clk_getfqcy(struct xwds_soc * soc, xwid_t id,
                        xwu32_t * buf, xwsz_t * num);

__xwds_lib_code
xwer_t xwds_get_clkrsc(const struct xwds_resource_clk base[], xwsz_t num,
                       const char * descay[], xwsz_t descnum,
                       const struct xwds_resource_clk ** ret);

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********  inline functions implementations   ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

#endif /* xwmd/ds/soc/clock.h */
