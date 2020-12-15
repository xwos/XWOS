#! /bin/make -f
# @file
# @brief 玄武OS MP内核：编译规则
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
