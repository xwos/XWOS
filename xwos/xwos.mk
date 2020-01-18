#! /bin/make -f
# @file
# @brief XWOS内核的编译规则
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
XWOS_CSRCS :=
XWOS_ASRCS :=

######## ######## ######## ######## clib ######## ######## ######## ########
include xwos/lib/lib.mk

######## ######## ######## ######## kernel ######## ######## ######## ########
######## init ########
XWOS_CSRCS += init.c

######## mm ########
XWOS_CSRCS += mm/sma.c
XWOS_CSRCS += mm/kma.c

ifeq ($(XWMMCFG_BMA),y)
    XWOS_CSRCS += mm/bma.c
endif

ifeq ($(XWMMCFG_MEMSLICE),y)
    XWOS_CSRCS += mm/memslice.c
endif

ifeq ($(XWMMCFG_MEMPOOL),y)
    XWOS_CSRCS += mm/mempool/page.c
    XWOS_CSRCS += mm/mempool/objcache.c
    XWOS_CSRCS += mm/mempool/allocator.c
endif

######## kernel ########
ifeq ($(XuanWuOS_CFG_CORE),smp)
    include xwos/smp/smp.mk
else
    include xwos/up/up.mk
endif
