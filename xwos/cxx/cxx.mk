#! /bin/make -f
# @file
# @brief XWOS C++编译规则
# @author
# + 隐星魂 (Roy Sun) <xwos@xwos.tech>
# @copyright
# + Copyright © 2015 xwos.tech, All Rights Reserved.
# > This Source Code Form is subject to the terms of the Mozilla Public
# > License, v. 2.0. If a copy of the MPL was not distributed with this
# > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.

######## ######## ######## ######## rule ######## ######## ######## ########
ifeq ($(XWOSCFG_SKD_THD_MEMPOOL),y)
  XWOSCXXRULE_DTHD := y
else ifeq ($(XWOSCFG_SKD_THD_MEMSLICE),y)
  XWOSCXXRULE_DTHD := y
else ifeq ($(XWOSCFG_SKD_THD_SMA),y)
  XWOSCXXRULE_DTHD := y
else ifeq ($(XWOSCFG_SKD_THD_STDC_MM),y)
  XWOSCXXRULE_DTHD := y
else
  XWOSCXXRULE_DTHD := n
endif

ifeq ($(XWOSCFG_SKD_SWT_MEMPOOL),y)
  XWOSCXXRULE_DSWT := y
else ifeq ($(XWOSCFG_SKD_SWT_MEMSLICE),y)
  XWOSCXXRULE_DSWT := y
else ifeq ($(XWOSCFG_SKD_SWT_SMA),y)
  XWOSCXXRULE_DSWT := y
else ifeq ($(XWOSCFG_SKD_SWT_STDC_MM),y)
  XWOSCXXRULE_DSWT := y
else
  XWOSCXXRULE_DSWT := n
endif

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
ifeq ($(XWOSCFG_SKD_THD_CXX), y)
  XWOS_CXXSRCS += cxx/SThd.cxx
  ifeq ($(XWOSCXXRULE_DTHD),y)
    XWOS_CXXSRCS += cxx/DThd.cxx
  endif
endif
######## ######## ######## ######## SoftwareTimer ######## ######## ######## ########
ifeq ($(XWOSCFG_SKD_SWT_CXX), y)
  XWOS_CXXSRCS += cxx/SSwt.cxx
  ifeq ($(XWOSCXXRULE_DSWT),y)
    XWOS_CXXSRCS += cxx/DSwt.cxx
  endif
endif

######## ######## ######## ######## Lock ######## ######## ######## ########
ifeq ($(XWOSCFG_LOCK_MTX_CXX), y)
  XWOS_CXXSRCS += cxx/lock/SMtx.cxx
  ifeq ($(XWOSCXXRULE_DMTX),y)
    XWOS_CXXSRCS += cxx/lock/DMtx.cxx
  endif
endif

ifeq ($(XWOSCFG_LOCK_SPLK_CXX), y)
  XWOS_CXXSRCS += cxx/lock/Spinlock.cxx
endif

ifeq ($(XWOSCFG_LOCK_SQLK_CXX), y)
  XWOS_CXXSRCS += cxx/lock/Seqlock.cxx
endif

######## ######## ######## ######## Pm ######## ######## ######## ########
ifeq ($(XWOSCFG_PM_CXX), y)
  XWOS_CXXSRCS += cxx/Pm.cxx
endif
