#! /bin/make -f
# @file
# @brief XWOS模块的编译规则
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
#

include $(XWOS_WKSPC_DIR)/XuanWuOS.cfg
include $(XWBS_UTIL_MK_XWMO)

CUBEMX_CSRCS := $(shell xwbs/util/el/makefile-grep-variable.el --variable=C_SOURCES $(call getXwmoDir)/cubemx/Makefile)
CUBEMX_CSRCS += Core/Src/sys.c
CUBEMX_CFLAGS := $(shell xwbs/util/el/makefile-grep-variable.el --variable=C_DEFS $(call getXwmoDir)/cubemx/Makefile)
CUBEMX_INCDIRS := $(shell xwbs/util/el/makefile-grep-variable.el --variable=C_INCLUDES $(call getXwmoDir)/cubemx/Makefile)

XWMO_CSRCS :=
XWMO_CSRCS += $(addprefix cubemx/,$(CUBEMX_CSRCS))

XWMO_CSRCS += xwac/init.c
XWMO_CSRCS += xwac/override.c
XWMO_CSRCS += xwac/crc32.c
XWMO_CSRCS += xwac/vector.c

XWMO_CSRCS += xwds/init.c
XWMO_CSRCS += xwds/pm.c
XWMO_CSRCS += xwds/stm32cube.c

XWMO_CSRCS += xwmo.c

XWMO_CFLAGS := -include standard.h
XWMO_CFLAGS += -Wno-undef -Wno-unused-parameter
XWMO_CFLAGS += $(CUBEMX_CFLAGS)

XWMO_INCDIRS := $(call getXwmoDir)
XWMO_INCDIRS += $(addprefix $(call getXwmoDir)/cubemx/,$(patsubst -I%,%,$(CUBEMX_INCDIRS)))

include xwbs/$(XuanWuOS_CFG_XWMO_MK)
