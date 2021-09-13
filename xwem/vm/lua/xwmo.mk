#! /bin/make -f
# @file
# @brief XWOS模块的编译规则
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
#

include $(XuanWuOS_WKSPC_DIR)/XuanWuOS.cfg
include $(XWBS_UTIL_MK_XWMO)

$(eval $(call XwmoReqCfg,XuanWuOS_CFG_CORE,mp))
$(eval $(call XwmoReqCfg,XWEMCFG_fs_fatfs,y))
$(eval $(call XwmoReqCfg,XWMDCFG_libc_newlibac,y))

CORE_O := $(shell xwbs/util/el/makefile-grep-variable.el -a CORE_O $(call getXwmoDir)/src/Makefile)
LUA_CORE := $(addprefix src/,$(addsuffix .c,$(basename $(CORE_O))))
LIB_O := $(shell xwbs/util/el/makefile-grep-variable.el -a LIB_O $(call getXwmoDir)/src/Makefile)
LUA_LIB := $(addprefix src/,$(addsuffix .c,$(basename $(LIB_O))))
LUA_PORT := xwlua/port.c xwlua/readline.c xwlua/lua.c mif.c
LUA_PORT += $(call getAllFileUnderXwmoDir,*.c,xwlua/xwxt)
LUA_PORT += $(call getAllFileUnderXwmoDir,*.c,xwlua/xwlib)
LUA_PORT += $(call getAllFileUnderXwmoDir,*.c,xwlua/xwos)
LUA_PORT += $(call getAllFileUnderXwmoDir,*.c,xwlua/xwds)

XWMO_CSRCS := $(LUA_CORE) $(LUA_LIB) $(LUA_PORT)
XWMO_CFLAGS := -Wno-sign-conversion
XWMO_INCDIRS := $(call getXwmoDir) $(call getXwmoDir)/src

XWMO_CFLAGS_llvm := -Wno-format

include xwbs/$(XuanWuOS_CFG_XWMO_MK)
