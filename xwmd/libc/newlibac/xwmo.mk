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

$(eval $(call XwmoReqCfg,XWEMCFG_fs_fatfs,y))
$(eval $(call XwmoReqCfg,XWMMCFG_MEMPOOL,y))

XWMO_CSRCS := mif.c errno.c lock.c

ifeq ($(XWMDCFG_libc_newlibac_string),y)
  XWMO_CSRCS += string.c
endif
ifeq ($(XWMDCFG_libc_newlibac_mem),y)
  XWMO_CSRCS += mem.c
endif
ifeq ($(XWMDCFG_libc_newlibac_fops),y)
  XWMO_CSRCS += fops.c
endif

XWMO_CFLAGS := -Wno-unused-value -Wno-unused-parameter
XWMO_INCDIRS :=
include xwbs/$(XWOS_CFG_XWMO_MK)
