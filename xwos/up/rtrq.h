/**
 * @file
 * @brief XuanWuOS内核：实时就绪队列
 * @author
 * + 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __xwos_up_rtrq_h__
#define __xwos_up_rtrq_h__

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <xwos/lib/bclst.h>
#include <xwos/lib/xwbop.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define XWOS_RTRQ_QNUM          XWUPCFG_SD_PRIORITY_RT_NUM

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
struct xwos_scheduler;
struct xwos_tcb;

/**
 * @brief 实时就绪队列
 */
struct xwos_rtrq {
        struct xwlib_bclst_head q[XWOS_RTRQ_QNUM];
        DECLARE_BITMAP(bmp, XWOS_RTRQ_QNUM);
        xwpr_t top;
};

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         function prototypes         ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
__xwos_code
void xwos_rtrq_init(struct xwos_rtrq * xwrtrq);

__xwos_code
void xwos_rtrq_add_head(struct xwos_rtrq * xwrtrq, struct xwos_tcb * tcb);

__xwos_code
void xwos_rtrq_add_tail(struct xwos_rtrq * xwrtrq, struct xwos_tcb * tcb);

__xwos_code
void xwos_rtrq_remove(struct xwos_rtrq * xwrtrq, struct xwos_tcb * tcb);

__xwos_code
struct xwos_tcb *xwos_rtrq_choose(struct xwos_rtrq * xwrtrq);

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********  inline functions implementations   ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

#endif /* xwos/up/rtrq.h */
