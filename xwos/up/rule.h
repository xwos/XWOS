/**
 * @file
 * @brief XuanWuOS内核：编译规则
 * @author
 * + 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __xwos_up_rule_h__
#define __xwos_up_rule_h__

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <cfg/XuanWuOS.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       rules       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#if defined(XWUPCFG_SD_THRD_EXIT) && (1 == XWUPCFG_SD_THRD_EXIT)
  #if !defined(XWUPCFG_SYNC_CDT) || (0 == XWUPCFG_SYNC_CDT)
    #error "XWUPCFG_SYNC_CDT must be '1' when (XWUPCFG_SD_THRD_EXIT == 1) !"
  #endif /* !XWUPCFG_SYNC_CDT */
#endif /* XWUPCFG_SD_THRD_EXIT */

#if defined(XWUPCFG_SYNC_EVT) && (1 == XWUPCFG_SYNC_EVT)
  #if !defined(XWUPCFG_SYNC_CDT) || (0 == XWUPCFG_SYNC_CDT)
    #error "XWUPCFG_SYNC_EVT must be '1' when (XWUPCFG_SD_THRD_EXIT == 1) !"
  #endif /* !XWUPCFG_SYNC_CDT */
#endif /* XWUPCFG_SYNC_EVT */

#if defined(XWUPCFG_SYNC_CDT) && (1 == XWUPCFG_SYNC_CDT)
  #define XWUPRULE_SD_THRD_DO_LOCK              1
  #define XWUPRULE_SD_THRD_DO_UNLOCK            1
#else
  #define XWUPRULE_SD_THRD_DO_LOCK              0
  #define XWUPRULE_SD_THRD_DO_UNLOCK            0
#endif

#if (defined(XWUPCFG_SYNC_PLSMR) && (1 == XWUPCFG_SYNC_PLSMR))
  #define XWUPRULE_SD_WQ_PL                     1
#elif (defined(XWUPCFG_SYNC_CDT) && (1 == XWUPCFG_SYNC_CDT))
  #define XWUPRULE_SD_WQ_PL                     1
#else
  #define XWUPRULE_SD_WQ_PL                     0
#endif

#if (defined(XWUPCFG_LOCK_MTX) && (1 == XWUPCFG_LOCK_MTX))
  #define XWUPRULE_SD_WQ_RT                     1
#elif (defined(XWUPCFG_SYNC_RTSMR) && (1 == XWUPCFG_SYNC_RTSMR))
  #define XWUPRULE_SD_WQ_RT                     1
#else
  #define XWUPRULE_SD_WQ_RT                     0
#endif

#if (defined(XWUPCFG_SD_PM) && (1 == XWUPCFG_SD_PM))
  #define XWUPRULE_SD_THRD_FREEZE               1
#else
  #define XWUPRULE_SD_THRD_FREEZE               0
#endif

#endif /* xwos/up/rule.h */
