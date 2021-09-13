/**
 * @file
 * @brief 玄武OS UP内核：编译规则
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __xwos_up_rule_h__
#define __xwos_up_rule_h__

#include <cfg/XuanWuOS.h>

#if defined(XWUPCFG_SKD_THD_EXIT) && (1 == XWUPCFG_SKD_THD_EXIT)
#  if !defined(XWUPCFG_SYNC_COND) || (0 == XWUPCFG_SYNC_COND)
#    error "XWUPCFG_SYNC_COND must be '1' when (XWUPCFG_SKD_THD_EXIT == 1) !"
#  endif
#endif

#if defined(XWUPCFG_SYNC_EVT) && (1 == XWUPCFG_SYNC_EVT)
#  if !defined(XWUPCFG_SYNC_COND) || (0 == XWUPCFG_SYNC_COND)
#    error "XWUPCFG_SYNC_EVT must be '1' when (XWUPCFG_SKD_THD_EXIT == 1) !"
#  endif
#endif

#if defined(XWUPCFG_SYNC_COND) && (1 == XWUPCFG_SYNC_COND)
#  define XWUPRULE_SKD_THD_DO_LOCK              1
#  define XWUPRULE_SKD_THD_DO_UNLOCK            1
#else
#  define XWUPRULE_SKD_THD_DO_LOCK              0
#  define XWUPRULE_SKD_THD_DO_UNLOCK            0
#endif

#if (defined(XWUPCFG_SYNC_PLSEM) && (1 == XWUPCFG_SYNC_PLSEM))
#  define XWUPRULE_SKD_WQ_PL                    1
#elif (defined(XWUPCFG_SYNC_COND) && (1 == XWUPCFG_SYNC_COND))
#  define XWUPRULE_SKD_WQ_PL                    1
#else
#  define XWUPRULE_SKD_WQ_PL                    0
#endif

#if (defined(XWUPCFG_LOCK_MTX) && (1 == XWUPCFG_LOCK_MTX))
#  define XWUPRULE_SKD_WQ_RT                    1
#elif (defined(XWUPCFG_SYNC_RTSEM) && (1 == XWUPCFG_SYNC_RTSEM))
#  define XWUPRULE_SKD_WQ_RT                    1
#else
#  define XWUPRULE_SKD_WQ_RT                    0
#endif

#if (defined(XWUPCFG_SKD_PM) && (1 == XWUPCFG_SKD_PM))
#  define XWUPRULE_SKD_THD_FREEZE               1
#else
#  define XWUPRULE_SKD_THD_FREEZE               0
#endif

#endif /* xwos/up/rule.h */
