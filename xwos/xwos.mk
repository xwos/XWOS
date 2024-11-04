#! /bin/make -f
# @file
# @brief XWOS内核的编译规则
# @author
# + 隐星魂 (Roy Sun) <xwos@xwos.tech>
# @copyright
# + Copyright © 2015 xwos.tech, All Rights Reserved.
# > This Source Code Form is subject to the terms of the Mozilla Public
# > License, v. 2.0. If a copy of the MPL was not distributed with this
# > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.

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
XWOS_CSRCS += ospl/xwosplcb.c
ifeq ($(XWCFG_CORE),mp)
  include xwos/mp/mp.mk
else
  include xwos/up/up.mk
endif

######## ######## ######## ######## arcos ######## ######## ######## ########
include xwos/arcos/arcos.mk
