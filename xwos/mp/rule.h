/**
 * @file
 * @brief XWOS MP内核：编译规则
 * @author
 * + 隐星曜 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef __xwos_mp_rule_h__
#define __xwos_mp_rule_h__

#include <cfg/project.h>

#if defined(XWOSCFG_SKD_THD_EXIT) && (1 == XWOSCFG_SKD_THD_EXIT)
#  if !defined(XWOSCFG_SYNC_COND) || (0 == XWOSCFG_SYNC_COND)
#    error "XWOSCFG_SYNC_COND must be set to '1' when (XWOSCFG_SKD_THD_EXIT == 1) !"
#  endif
#endif

#if defined(XWOSCFG_SYNC_EVT) && (1 == XWOSCFG_SYNC_EVT)
#  if !defined(XWOSCFG_SYNC_COND) || (0 == XWOSCFG_SYNC_COND)
#    error "XWOSCFG_SYNC_EVT must be set to '1' when (XWOSCFG_SKD_THD_EXIT == 1) !"
#  endif
#endif

#if defined(XWOSCFG_SKD_THD_MEMPOOL) && (1 == XWOSCFG_SKD_THD_MEMPOOL)
#  define XWOSRULE_SKD_THD_CREATE               1
#elif defined(XWOSCFG_SKD_THD_MEMSLICE) && (1 == XWOSCFG_SKD_THD_MEMSLICE)
#  define XWOSRULE_SKD_THD_CREATE               1
#elif defined(XWOSCFG_SKD_THD_SMA) && (1 == XWOSCFG_SKD_THD_SMA)
#  define XWOSRULE_SKD_THD_CREATE               1
#elif defined(XWOSCFG_SKD_THD_STDC_MM) && (1 == XWOSCFG_SKD_THD_STDC_MM)
#  define XWOSRULE_SKD_THD_CREATE               1
#else
#  define XWOSRULE_SKD_THD_CREATE               0
#endif

#if defined(XWOSCFG_SKD_SWT_MEMPOOL) && (1 == XWOSCFG_SKD_SWT_MEMPOOL)
#  define XWOSRULE_SKD_SWT_CREATE_DELETE        1
#elif defined(XWOSCFG_SKD_SWT_MEMSLICE) && (1 == XWOSCFG_SKD_SWT_MEMSLICE)
#  define XWOSRULE_SKD_SWT_CREATE_DELETE        1
#elif defined(XWOSCFG_SKD_SWT_SMA) && (1 == XWOSCFG_SKD_SWT_SMA)
#  define XWOSRULE_SKD_SWT_CREATE_DELETE        1
#elif defined(XWOSCFG_SKD_SWT_STDC_MM) && (1 == XWOSCFG_SKD_SWT_STDC_MM)
#  define XWOSRULE_SKD_SWT_CREATE_DELETE        1
#else
#  define XWOSRULE_SKD_SWT_CREATE_DELETE        0
#endif

#if defined(XWOSCFG_SYNC_SEM_MEMPOOL) && (1 == XWOSCFG_SYNC_SEM_MEMPOOL)
#  define XWOSRULE_SYNC_SEM_CREATE_DELETE       1
#elif defined(XWOSCFG_SYNC_SEM_MEMSLICE) && (1 == XWOSCFG_SYNC_SEM_MEMSLICE)
#  define XWOSRULE_SYNC_SEM_CREATE_DELETE       1
#elif defined(XWOSCFG_SYNC_SEM_SMA) && (1 == XWOSCFG_SYNC_SEM_SMA)
#  define XWOSRULE_SYNC_SEM_CREATE_DELETE       1
#elif defined(XWOSCFG_SYNC_SEM_STDC_MM) && (1 == XWOSCFG_SYNC_SEM_STDC_MM)
#  define XWOSRULE_SYNC_SEM_CREATE_DELETE       1
#else
#  define XWOSRULE_SYNC_SEM_CREATE_DELETE       0
#endif

#if defined(XWOSCFG_SYNC_COND_MEMPOOL) && (1 == XWOSCFG_SYNC_COND_MEMPOOL)
#  define XWOSRULE_SYNC_COND_CREATE_DELETE      1
#elif defined(XWOSCFG_SYNC_COND_MEMSLICE) && (1 == XWOSCFG_SYNC_COND_MEMSLICE)
#  define XWOSRULE_SYNC_COND_CREATE_DELETE      1
#elif defined(XWOSCFG_SYNC_COND_SMA) && (1 == XWOSCFG_SYNC_COND_SMA)
#  define XWOSRULE_SYNC_COND_CREATE_DELETE      1
#elif defined(XWOSCFG_SYNC_COND_STDC_MM) && (1 == XWOSCFG_SYNC_COND_STDC_MM)
#  define XWOSRULE_SYNC_COND_CREATE_DELETE      1
#else
#  define XWOSRULE_SYNC_COND_CREATE_DELETE      0
#endif

#if defined(XWOSCFG_SYNC_EVT_MEMPOOL) && (1 == XWOSCFG_SYNC_EVT_MEMPOOL)
#  define XWOSRULE_SYNC_EVT_CREATE_DELETE       1
#elif defined(XWOSCFG_SYNC_EVT_MEMSLICE) && (1 == XWOSCFG_SYNC_EVT_MEMSLICE)
#  define XWOSRULE_SYNC_EVT_CREATE_DELETE       1
#elif defined(XWOSCFG_SYNC_EVT_SMA) && (1 == XWOSCFG_SYNC_EVT_SMA)
#  define XWOSRULE_SYNC_EVT_CREATE_DELETE       1
#elif defined(XWOSCFG_SYNC_EVT_STDC_MM) && (1 == XWOSCFG_SYNC_EVT_STDC_MM)
#  define XWOSRULE_SYNC_EVT_CREATE_DELETE       1
#else
#  define XWOSRULE_SYNC_EVT_CREATE_DELETE       0
#endif

#if defined(XWOSCFG_LOCK_MTX_MEMPOOL) && (1 == XWOSCFG_LOCK_MTX_MEMPOOL)
#  define XWOSRULE_LOCK_MTX_CREATE_DELETE       1
#elif defined(XWOSCFG_LOCK_MTX_MEMSLICE) && (1 == XWOSCFG_LOCK_MTX_MEMSLICE)
#  define XWOSRULE_LOCK_MTX_CREATE_DELETE       1
#elif defined(XWOSCFG_LOCK_MTX_SMA) && (1 == XWOSCFG_LOCK_MTX_SMA)
#  define XWOSRULE_LOCK_MTX_CREATE_DELETE       1
#elif defined(XWOSCFG_LOCK_MTX_STDC_MM) && (1 == XWOSCFG_LOCK_MTX_STDC_MM)
#  define XWOSRULE_LOCK_MTX_CREATE_DELETE       1
#else
#  define XWOSRULE_LOCK_MTX_CREATE_DELETE       0
#endif

#if (defined(XWOSCFG_SYNC_PLSEM) && (1 == XWOSCFG_SYNC_PLSEM))
#  define XWOSRULE_SKD_WQ_PL                    1
#elif (defined(XWOSCFG_SYNC_COND) && (1 == XWOSCFG_SYNC_COND))
#  define XWOSRULE_SKD_WQ_PL                    1
#else
#  define XWOSRULE_SKD_WQ_PL                    0
#endif

#if (defined(XWOSCFG_LOCK_MTX) && (1 == XWOSCFG_LOCK_MTX))
#  define XWOSRULE_SKD_WQ_RT                    1
#elif (defined(XWOSCFG_SYNC_RTSEM) && (1 == XWOSCFG_SYNC_RTSEM))
#  define XWOSRULE_SKD_WQ_RT                    1
#else
#  define XWOSRULE_SKD_WQ_RT                    0
#endif

#define XWOSRULE_THD_MIGRATION                  1

#endif /* xwos/mp/rule.h */
