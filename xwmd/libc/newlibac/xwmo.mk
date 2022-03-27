#! /bin/make -f
# @file
# @brief XWOS模块的编译规则
# @author
# + 隐星魂 (Roy Sun) <xwos@xwos.tech>
# @copyright
# + Copyright © 2015 xwos.tech, All Rights Reserved.
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
#

include $(XuanWuOS_WKSPC_DIR)/XuanWuOS.cfg
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

XWMO_CSRCS += $(call getAllFileUnderXwmoDir,*.c,posix)

XWMO_CFLAGS := -Wno-unused-value -Wno-unused-parameter
XWMO_INCDIRS :=
include xwbs/$(XuanWuOS_CFG_XWMO_MK)
