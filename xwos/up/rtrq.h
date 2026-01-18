/**
 * @file
 * @brief XWOS UP内核：实时就绪队列
 * @author
 * + 隐星曜 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef __xwos_up_rtrq_h__
#define __xwos_up_rtrq_h__

#include <xwos/standard.h>
#include <xwos/lib/bclst.h>
#include <xwos/lib/xwbop.h>

#define XWUP_RTRQ_QNUM XWOSCFG_SKD_PRIORITY_RT_NUM

struct xwup_skd;
struct xwup_thd;

/**
 * @brief 实时就绪队列
 */
struct xwup_rtrq {
        struct xwlib_bclst_head q[XWUP_RTRQ_QNUM];
        xwbmpop_define(bmp, XWUP_RTRQ_QNUM);
        xwpr_t top;
};

void xwup_rtrq_init(struct xwup_rtrq * xwrtrq);
void xwup_rtrq_add_head(struct xwup_rtrq * xwrtrq, struct xwup_thd * thd);
void xwup_rtrq_add_tail(struct xwup_rtrq * xwrtrq, struct xwup_thd * thd);
void xwup_rtrq_remove(struct xwup_rtrq * xwrtrq, struct xwup_thd * thd);
struct xwup_thd * xwup_rtrq_choose(struct xwup_rtrq * xwrtrq);

#endif /* xwos/up/rtrq.h */
