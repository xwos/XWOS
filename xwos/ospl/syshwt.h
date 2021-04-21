/**
 * @file
 * @brief 玄武OS移植层：硬件定时器
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __xwos_ospl_syshwt_h__
#define __xwos_ospl_syshwt_h__

#include <xwos/standard.h>

#if defined(XuanWuOS_CFG_CORE__mp)
  #include <xwos/mp/tt.h>

  #define xwospl_syshwt xwmp_syshwt
#elif defined(XuanWuOS_CFG_CORE__up)
  #include <xwos/up/tt.h>

  #define xwospl_syshwt xwup_syshwt
#else
  #error "Can't find the configuration XuanWuOS_CFG_CORE!"
#endif

struct xwospl_syshwt;

xwer_t xwospl_syshwt_init(struct xwospl_syshwt * hwt);
xwer_t xwospl_syshwt_start(struct xwospl_syshwt * hwt);
xwer_t xwospl_syshwt_stop(struct xwospl_syshwt * hwt);
xwtm_t xwospl_syshwt_get_timeconfetti(struct xwospl_syshwt * hwt);

#include <xwosimpl_syshwt.h>

#endif /* xwos/ospl/syshwt.h */
