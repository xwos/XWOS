#! /bin/make -f
# @file
# @brief XuanWuOS内核：编译规则
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

######## ######## ######## ######## rule ######## ######## ######## ########

######## ######## ######## ######## init ######## ######## ######## ########
XWOS_CSRCS += smp/init.c

######## ######## ######## ######## irq ######## ######## ######## ########
XWOS_CSRCS += smp/irq.c

######## ######## ######## ######## scheduler ######## ######## ######## ########
XWOS_CSRCS += smp/scheduler.c
XWOS_CSRCS += smp/rtrq.c
XWOS_CSRCS += smp/bh.c
XWOS_CSRCS += smp/tt.c
XWOS_CSRCS += smp/pm.c
XWOS_CSRCS += smp/thread.c
XWOS_CSRCS += smp/rtwq.c
XWOS_CSRCS += smp/plwq.c
XWOS_CSRCS += smp/mutextree.c

######## ######## ######## ######## swt ######## ######## ######## ########
ifeq ($(XWSMPCFG_SD_SWT),y)
    XWOS_CSRCS += smp/swt.c
endif

######## ######## ######## ######## system call ######## ######## ######## ########
XWOS_CSRCS += smp/sc.c

######## ######## ######## ######## lock ######## ######## ######## ########
XWOS_CSRCS += smp/lock/spinlock.c
XWOS_CSRCS += smp/lock/mutex.c

######## ######## ######## ######## sync ######## ######## ######## ########
XWOS_CSRCS += smp/sync/semaphore.c
XWOS_CSRCS += smp/sync/condition.c
ifeq ($(XWSMPCFG_SYNC_EVT),y)
    XWOS_CSRCS += smp/sync/event.c
endif
