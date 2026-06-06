#! /bin/make -f
# @file
# @brief XWOS C++编译规则
# @author
# + 隐星曜 (Roy Sun) <xwos@xwos.tech>
# @copyright
# + Copyright © 2015 xwos.tech, All Rights Reserved.
# > This Source Code Form is subject to the terms of the Mozilla Public
# > License, v. 2.0. If a copy of the MPL was not distributed with this
# > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.

######## ######## ######## ######## rule ######## ######## ######## ########
ifeq ($(XWOSCFG_LOCK_MTX_MEMPOOL),y)
  XWOSCXXRULE_DMTX := y
else ifeq ($(XWOSCFG_LOCK_MTX_MEMSLICE),y)
  XWOSCXXRULE_DMTX := y
else ifeq ($(XWOSCFG_LOCK_MTX_SMA),y)
  XWOSCXXRULE_DMTX := y
else ifeq ($(XWOSCFG_LOCK_MTX_STDC_MM),y)
  XWOSCXXRULE_DMTX := y
else
  XWOSCXXRULE_DMTX := n
endif

######## ######## ######## ######## Thread ######## ######## ######## ########

######## ######## ######## ######## SoftwareTimer ######## ######## ######## ########

######## ######## ######## ######## Lock ######## ######## ######## ########
ifeq ($(XWOSCFG_LOCK_MTX_CXX), y)
  XWOS_CXXSRCS += cxx/lock/Mtx.cxx
endif

ifeq ($(XWOSCFG_LOCK_SPLK_CXX), y)
  XWOS_CXXSRCS += cxx/lock/Spinlock.cxx
endif

ifeq ($(XWOSCFG_LOCK_SQLK_CXX), y)
  XWOS_CXXSRCS += cxx/lock/Seqlock.cxx
endif

######## ######## ######## ######## sync ######## ######## ######## ########

######## ######## ######## ######## Pm ######## ######## ######## ########
ifeq ($(XWOSCFG_PM_CXX), y)
  XWOS_CXXSRCS += cxx/Pm.cxx
endif
