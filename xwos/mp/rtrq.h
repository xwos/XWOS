/**
 * @file
 * @brief 玄武OS MP内核：实时就绪队列
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __xwos_mp_rtrq_h__
#define __xwos_mp_rtrq_h__

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <xwos/lib/bclst.h>
#include <xwos/lib/xwbop.h>
#include <xwos/mp/lock/spinlock.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define XWMP_RTRQ_QNUM          XWMPCFG_SKD_PRIORITY_RT_NUM

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
struct xwmp_skd;
struct xwmp_tcb;

/**
 * @brief 实时就绪队列
 */
struct xwmp_rtrq {
        struct xwlib_bclst_head q[XWMP_RTRQ_QNUM];
        xwbmpop_declare(bmp, XWMP_RTRQ_QNUM);
        xwpr_t top;
        struct xwmp_splk lock;
};

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         function prototypes         ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
xwer_t xwmp_rtrq_init(struct xwmp_rtrq * xwrtrq);
xwer_t xwmp_rtrq_add_head_locked(struct xwmp_rtrq * xwrtrq, struct xwmp_tcb * tcb);
xwer_t xwmp_rtrq_add_tail_locked(struct xwmp_rtrq * xwrtrq, struct xwmp_tcb * tcb);
xwer_t xwmp_rtrq_remove_locked(struct xwmp_rtrq * xwrtrq, struct xwmp_tcb * tcb);
struct xwmp_tcb * xwmp_rtrq_choose_locked(struct xwmp_rtrq * xwrtrq);

#endif /* xwos/mp/rtrq.h */
