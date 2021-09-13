#! /bin/make -f
# @file
# @brief 玄武OS UP内核：编译规则
# @author
# + 隐星魂 (Roy.Sun) <https://xwos.tech>
# @copyright
# + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
# > Licensed under the Apache License, Version 2.0 (the "License");
# > you may not use this file except in compliance with the License.
# > You may obtain a copy of the License at
# >
# >         http://www.apache.org/licenses/LICENSE-2.0
# >
# > Unless required by applicable law or agreed to in writing, software
# > distributed under the License is distributed on an "AS IS" BASIS,
# > WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# > See the License for the specific language governing permissions and
# > limitations under the License.

XWOS_EOBJS :=

######## ######## ######## ######## ******** ######## ######## ######## ########
######## ######## ######## ########   rules  ######## ######## ######## ########
######## ######## ######## ######## ******** ######## ######## ######## ########
ifeq ($(XWUPCFG_SYNC_COND),y)
    XWUPRULE_SKD_THD_DO_LOCK := y
    XWUPRULE_SKD_THD_DO_UNLOCK := y
else
    XWUPRULE_SKD_THD_DO_LOCK := n
    XWUPRULE_SKD_THD_DO_UNLOCK := n
endif

ifeq ($(XWUPCFG_SYNC_PLSEM),y)
    XWUPRULE_SKD_WQ_PL := y
else
    ifeq ($(XWUPCFG_SYNC_COND),y)
        XWUPRULE_SKD_WQ_PL := y
    else
        XWUPRULE_SKD_WQ_PL := n
    endif
endif

ifeq ($(XWUPCFG_LOCK_MTX),y)
    XWUPRULE_SKD_WQ_RT := y
else
    ifeq ($(XWUPCFG_SYNC_RTSEM),y)
        XWUPRULE_SKD_WQ_RT := y
    else
        XWUPRULE_SKD_WQ_RT := n
    endif
endif

######## ######## ######## ######## init ######## ######## ######## ########
XWOS_CSRCS += up/init.c

######## ######## ######## ######## irq ######## ######## ######## ########
XWOS_CSRCS += up/irq.c

######## ######## ######## ######## scheduler ######## ######## ######## ########
XWOS_CSRCS += up/skd.c
XWOS_CSRCS += up/rtrq.c
ifeq ($(XWUPCFG_SKD_BH),y)
    XWOS_CSRCS += up/bh.c
endif
XWOS_CSRCS += up/tt.c
XWOS_CSRCS += up/thd.c
XWOS_CSRCS += up/wqn.c

ifeq ($(XWUPRULE_SKD_WQ_PL),y)
    XWOS_CSRCS += up/plwq.c
endif

ifeq ($(XWUPRULE_SKD_WQ_RT),y)
    XWOS_CSRCS += up/rtwq.c
endif

ifeq ($(XWUPCFG_LOCK_MTX),y)
    XWOS_CSRCS += up/mtxtree.c
endif

######## ######## ######## ######## swt ######## ######## ######## ########
ifeq ($(XWUPCFG_SKD_SWT),y)
    XWOS_CSRCS += up/swt.c
endif

######## ######## ######## ######## locks ######## ######## ######## ########
XWOS_CSRCS += up/lock/seqlock.c

ifeq ($(XWUPCFG_LOCK_MTX),y)
    XWOS_CSRCS += up/lock/mtx.c
endif

######## ######## ######## ######## sync ######## ######## ######## ########
XWOS_CSRCS += up/sync/vsem.c
ifeq ($(XWUPCFG_SYNC_PLSEM),y)
    XWOS_CSRCS += up/sync/plsem.c
endif

ifeq ($(XWUPCFG_SYNC_RTSEM),y)
    XWOS_CSRCS += up/sync/rtsem.c
endif

ifeq ($(XWUPCFG_SYNC_COND),y)
    XWOS_CSRCS += up/sync/cond.c
endif

ifeq ($(XWUPCFG_SYNC_EVT),y)
    XWOS_CSRCS += up/sync/evt.c
endif
