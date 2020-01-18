#! /bin/make -f
# @file
# @brief XWOS内核的的编译规则
# @author
# + 隐星魂 (Roy.Sun) <www.starsoul.tech>
# @copyright
# + (c) 2015 隐星魂 (Roy.Sun) <www.starsoul.tech>
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
ifeq ($(XWUPCFG_SYNC_CDT),y)
        SOUPRULE_SD_THRD_DO_LOCK := y
        SOUPRULE_SD_THRD_DO_UNLOCK := y
else
        SOUPRULE_SD_THRD_DO_LOCK := n
        SOUPRULE_SD_THRD_DO_UNLOCK := n
endif

ifeq ($(XWUPCFG_SYNC_PLSMR),y)
    SOUPRULE_SD_WQ_PL := y
else
    ifeq ($(XWUPCFG_SYNC_CDT),y)
        SOUPRULE_SD_WQ_PL := y
    else
        SOUPRULE_SD_WQ_PL := n
    endif
endif

ifeq ($(XWUPCFG_LOCK_MTX),y)
    SOUPRULE_SD_WQ_RT := y
else
    ifeq ($(XWUPCFG_SYNC_RTSMR),y)
        SOUPRULE_SD_WQ_RT := y
    else
        SOUPRULE_SD_WQ_RT := n
    endif
endif

######## ######## ######## ######## init ######## ######## ######## ########
XWOS_CSRCS += up/init.c

######## ######## ######## ######## irq ######## ######## ######## ########
XWOS_CSRCS += up/irq.c

######## ######## ######## ######## scheduler ######## ######## ######## ########
XWOS_CSRCS += up/scheduler.c
XWOS_CSRCS += up/rtrq.c
ifeq ($(XWUPCFG_SD_BH),y)
    XWOS_CSRCS += up/bh.c
endif
XWOS_CSRCS += up/tt.c
XWOS_CSRCS += up/thread.c

ifeq ($(SOUPRULE_SD_WQ_PL),y)
    XWOS_CSRCS += up/plwq.c
endif

ifeq ($(SOUPRULE_SD_WQ_RT),y)
    XWOS_CSRCS += up/rtwq.c
endif

ifeq ($(XWUPCFG_LOCK_MTX),y)
    XWOS_CSRCS += up/mutextree.c
endif

######## ######## ######## ######## swt ######## ######## ######## ########
ifeq ($(XWUPCFG_SD_SWT),y)
    XWOS_CSRCS += up/swt.c
endif

######## ######## ######## ######## locks ######## ######## ######## ########
XWOS_CSRCS += up/lock/seqlock.c

ifeq ($(XWUPCFG_LOCK_MTX),y)
    XWOS_CSRCS += up/lock/mutex.c
endif

######## ######## ######## ######## sync ######## ######## ######## ########
XWOS_CSRCS += up/sync/vsmr.c
ifeq ($(XWUPCFG_SYNC_PLSMR),y)
    XWOS_CSRCS += up/sync/plsmr.c
endif

ifeq ($(XWUPCFG_SYNC_RTSMR),y)
    XWOS_CSRCS += up/sync/rtsmr.c
endif

ifeq ($(XWUPCFG_SYNC_CDT),y)
    XWOS_CSRCS += up/sync/condition.c
endif

ifeq ($(XWUPCFG_SYNC_EVT),y)
    XWOS_CSRCS += up/sync/event.c
endif
