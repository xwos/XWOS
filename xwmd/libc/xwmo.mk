#! /bin/make -f
# @file
# @brief XWOS模块的编译规则
# @author
# + 隐星魂 (Roy Sun) <xwos@xwos.tech>
# @copyright
# + Copyright © 2015 xwos.tech, All Rights Reserved.
# > This Source Code Form is subject to the terms of the Mozilla Public
# > License, v. 2.0. If a copy of the MPL was not distributed with this
# > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.

include $(XWOS_WKSPC_DIR)/XWOS.cfg
include $(XWBS_UTIL_MK_XWMO)

ifeq ($(XWOS_CFG_LIBC),newlib)
  include $(call getXwmoDir)/newlibac/newlib.mk
  XWMO_CSRCS := $(addprefix newlibac/,$(XWMO_CSRCS))
endif
ifeq ($(XWOS_CFG_LIBC),picolibc)
  include $(call getXwmoDir)/picolibcac/picolibc.mk
  XWMO_CSRCS := $(addprefix picolibcac/,$(XWMO_CSRCS))
endif

include xwbs/$(XWOS_CFG_XWMO_MK)
