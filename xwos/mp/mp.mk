#! /bin/make -f
# @file
# @brief XWOS MP内核：编译规则
# @author
# + 隐星魂 (Roy Sun) <xwos@xwos.tech>
# @copyright
# + Copyright © 2015 xwos.tech, All Rights Reserved.
# > This Source Code Form is subject to the terms of the Mozilla Public
# > License, v. 2.0. If a copy of the MPL was not distributed with this
# > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.

######## ######## ######## ######## rule ######## ######## ######## ########
ifeq ($(XWOSCFG_SYNC_PLSEM),y)
  XWOSRULE_SKD_WQ_PL := y
else ifeq ($(XWOSCFG_SYNC_COND),y)
  XWOSRULE_SKD_WQ_PL := y
else
  XWOSRULE_SKD_WQ_PL := n
endif

ifeq ($(XWOSCFG_LOCK_MTX),y)
  XWOSRULE_SKD_WQ_RT := y
else ifeq ($(XWOSCFG_SYNC_RTSEM),y)
  XWOSRULE_SKD_WQ_RT := y
else
  XWOSRULE_SKD_WQ_RT := n
endif

ifeq ($(XWOSCFG_SYNC_PLSEM),y)
  XWOSRULE_SYNC_SEM := y
else ifeq ($(XWOSCFG_SYNC_RTSEM),y)
  XWOSRULE_SYNC_SEM := y
else
  XWOSRULE_SYNC_SEM := n
endif

######## ######## ######## ######## init ######## ######## ######## ########
XWOS_CSRCS += mp/init.c

######## ######## ######## ######## irq ######## ######## ######## ########
XWOS_CSRCS += mp/irq.c

######## ######## ######## ######## scheduler ######## ######## ######## ########
XWOS_CSRCS += mp/rtrq.c
ifeq ($(XWOSCFG_SKD_BH),y)
  XWOS_CSRCS += mp/bh.c
endif
XWOS_CSRCS += mp/tt.c
XWOS_CSRCS += mp/wqn.c
ifeq ($(XWOSRULE_SKD_WQ_PL),y)
  XWOS_CSRCS += mp/plwq.c
endif
ifeq ($(XWOSRULE_SKD_WQ_RT),y)
  XWOS_CSRCS += mp/rtwq.c
endif
XWOS_CSRCS += mp/skd.c
XWOS_CSRCS += mp/thd.c
XWOS_CSRCS += mp/pm.c
ifeq ($(XWOSCFG_LOCK_MTX),y)
  XWOS_CSRCS += mp/mtxtree.c
endif

######## ######## ######## ######## swt ######## ######## ######## ########
ifeq ($(XWOSCFG_SKD_SWT),y)
  XWOS_CSRCS += mp/swt.c
endif

######## ######## ######## ######## lock ######## ######## ######## ########
XWOS_CSRCS += mp/lock/spinlock.c
ifeq ($(XWOSCFG_LOCK_MTX),y)
  XWOS_CSRCS += mp/lock/mtx.c
endif

######## ######## ######## ######## sync ######## ######## ######## ########
XWOS_CSRCS += mp/sync/obj.c
ifeq ($(XWOSRULE_SYNC_SEM),y)
  XWOS_CSRCS += mp/sync/sem.c
endif
ifeq ($(XWOSCFG_SYNC_COND),y)
  XWOS_CSRCS += mp/sync/cond.c
endif
ifeq ($(XWOSCFG_SYNC_EVT),y)
  XWOS_CSRCS += mp/sync/evt.c
endif

######## ######## ######## ######## osdl ######## ######## ######## ########
XWOS_CSRCS += mp/osdl/skd.c
XWOS_CSRCS += mp/osdl/thd.c
XWOS_CSRCS += mp/osdl/swt.c
XWOS_CSRCS += mp/osdl/lock/mtx.c
ifeq ($(XWOSRULE_SYNC_SEM),y)
  XWOS_CSRCS += mp/osdl/sync/sem.c
endif
ifeq ($(XWOSCFG_SYNC_COND),y)
  XWOS_CSRCS += mp/osdl/sync/cond.c
endif
ifeq ($(XWOSCFG_SYNC_EVT),y)
  XWOS_CSRCS += mp/osdl/sync/flg.c
  XWOS_CSRCS += mp/osdl/sync/sel.c
  XWOS_CSRCS += mp/osdl/sync/br.c
endif
