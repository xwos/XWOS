#! /bin/make -f
# @file
# @brief XWOS UP内核：编译规则
# @author
# + 隐星魂 (Roy Sun) <xwos@xwos.tech>
# @copyright
# + Copyright © 2015 xwos.tech, All Rights Reserved.
# > This Source Code Form is subject to the terms of the Mozilla Public
# > License, v. 2.0. If a copy of the MPL was not distributed with this
# > file, You can obtain one at <<http://mozilla.org/MPL/2.0/>>.

XWOS_EOBJS :=

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

######## ######## ######## ######## init ######## ######## ######## ########
XWOS_CSRCS += up/init.c

######## ######## ######## ######## irq ######## ######## ######## ########
XWOS_CSRCS += up/irq.c

######## ######## ######## ######## scheduler ######## ######## ######## ########
XWOS_CSRCS += up/skd.c
XWOS_CSRCS += up/rtrq.c
ifeq ($(XWOSCFG_SKD_BH),y)
  XWOS_CSRCS += up/bh.c
endif
XWOS_CSRCS += up/tt.c
XWOS_CSRCS += up/thd.c
XWOS_CSRCS += up/wqn.c

ifeq ($(XWOSRULE_SKD_WQ_PL),y)
  XWOS_CSRCS += up/plwq.c
endif

ifeq ($(XWOSRULE_SKD_WQ_RT),y)
  XWOS_CSRCS += up/rtwq.c
endif

ifeq ($(XWOSCFG_LOCK_MTX),y)
  XWOS_CSRCS += up/mtxtree.c
endif

######## ######## ######## ######## swt ######## ######## ######## ########
ifeq ($(XWOSCFG_SKD_SWT),y)
  XWOS_CSRCS += up/swt.c
endif

######## ######## ######## ######## locks ######## ######## ######## ########
XWOS_CSRCS += up/lock/seqlock.c

ifeq ($(XWOSCFG_LOCK_MTX),y)
  XWOS_CSRCS += up/lock/mtx.c
endif

######## ######## ######## ######## sync ######## ######## ######## ########
XWOS_CSRCS += up/sync/obj.c
XWOS_CSRCS += up/sync/vsem.c
ifeq ($(XWOSCFG_SYNC_PLSEM),y)
  XWOS_CSRCS += up/sync/plsem.c
endif

ifeq ($(XWOSCFG_SYNC_RTSEM),y)
  XWOS_CSRCS += up/sync/rtsem.c
endif

ifeq ($(XWOSCFG_SYNC_COND),y)
  XWOS_CSRCS += up/sync/cond.c
endif

ifeq ($(XWOSCFG_SYNC_EVT),y)
  XWOS_CSRCS += up/sync/evt.c
endif

######## ######## ######## ######## osdl ######## ######## ######## ########
XWOS_CSRCS += up/osdl/skd.c
XWOS_CSRCS += up/osdl/thd.c
XWOS_CSRCS += up/osdl/swt.c
XWOS_CSRCS += up/osdl/lock/mtx.c
XWOS_CSRCS += up/osdl/sync/sem.c
ifeq ($(XWOSCFG_SYNC_COND),y)
  XWOS_CSRCS += up/osdl/sync/cond.c
endif
ifeq ($(XWOSCFG_SYNC_EVT),y)
  XWOS_CSRCS += up/osdl/sync/flg.c
  XWOS_CSRCS += up/osdl/sync/sel.c
  XWOS_CSRCS += up/osdl/sync/br.c
endif
