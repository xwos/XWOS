#! /bin/make -f
# @file
# @brief 玄武OS MP内核：编译规则
# @author
# + 隐星魂 (Roy Sun) <xwos@xwos.tech>
# @copyright
# + Copyright © 2015 xwos.tech, All Rights Reserved.
# > This Source Code Form is subject to the terms of the Mozilla Public
# > License, v. 2.0. If a copy of the MPL was not distributed with this
# > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.

######## ######## ######## ######## rule ######## ######## ######## ########

######## ######## ######## ######## init ######## ######## ######## ########
XWOS_CSRCS += mp/init.c

######## ######## ######## ######## irq ######## ######## ######## ########
XWOS_CSRCS += mp/irq.c

######## ######## ######## ######## scheduler ######## ######## ######## ########
XWOS_CSRCS += mp/rtrq.c
XWOS_CSRCS += mp/bh.c
XWOS_CSRCS += mp/tt.c
XWOS_CSRCS += mp/wqn.c
XWOS_CSRCS += mp/rtwq.c
XWOS_CSRCS += mp/plwq.c
XWOS_CSRCS += mp/skd.c
XWOS_CSRCS += mp/thd.c
XWOS_CSRCS += mp/pm.c
XWOS_CSRCS += mp/mtxtree.c

######## ######## ######## ######## swt ######## ######## ######## ########
ifeq ($(XWMPCFG_SKD_SWT),y)
    XWOS_CSRCS += mp/swt.c
endif

######## ######## ######## ######## lock ######## ######## ######## ########
XWOS_CSRCS += mp/lock/spinlock.c
XWOS_CSRCS += mp/lock/mtx.c

######## ######## ######## ######## sync ######## ######## ######## ########
XWOS_CSRCS += mp/sync/obj.c
XWOS_CSRCS += mp/sync/sem.c
XWOS_CSRCS += mp/sync/cond.c
ifeq ($(XWMPCFG_SYNC_EVT),y)
    XWOS_CSRCS += mp/sync/evt.c
endif
