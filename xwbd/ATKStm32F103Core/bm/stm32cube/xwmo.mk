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

# 引入配置
include $(XuanWuOS_WKSPC_DIR)/XuanWuOS.cfg
include $(XWBS_UTIL_MK_XWMO)

# 使用脚本makefile-grep-variable.el获取STM32CubeMX生成的Makefile中的配置
CUBEMX_CSRCS := $(shell xwbs/util/el/makefile-grep-variable.el --variable=C_SOURCES $(call getXwmoDir)/cubemx/Makefile)
CUBEMX_CFLAGS := $(shell xwbs/util/el/makefile-grep-variable.el --variable=C_DEFS $(call getXwmoDir)/cubemx/Makefile)
CUBEMX_INCDIRS := $(shell xwbs/util/el/makefile-grep-variable.el --variable=C_INCLUDES $(call getXwmoDir)/cubemx/Makefile)

CUBEMX_CSRCS += IVT/isr.c
CUBEMX_CSRCS += IVT/ivt.c

# cubemx中的源文件路径统一增加前缀"cubemx/"
XWMO_CSRCS :=
XWMO_CSRCS += $(addprefix cubemx/,$(CUBEMX_CSRCS))

XWMO_CSRCS += mif.c

# 增加xwac中的源文件
XWMO_CSRCS += xwac/xwlib/crc32.c
XWMO_CSRCS += xwac/xwlib/log.c
XWMO_CSRCS += xwac/xwos/hook.c

XWMO_CSRCS += $(call getAllFileUnderXwmoDir,*.c,xwac/xwds)

# 定义编译选项
XWMO_CFLAGS := -Wno-undef -Wno-unused-parameter -Wno-sign-conversion
XWMO_CFLAGS += $(CUBEMX_CFLAGS)

# 定义头文件路径
XWMO_INCDIRS := $(call getXwmoDir) # getXwmoDir函数可获取当前玄武模块的路径
XWMO_INCDIRS += $(addprefix $(call getXwmoDir)/cubemx/,$(patsubst -I%,%,$(CUBEMX_INCDIRS)))

# 引入编译规则
include xwbs/$(XuanWuOS_CFG_XWMO_MK)
