#! /bin/make -f
# @file
# @brief XWOS模块的编译规则
# @author
# + 隐星曜 (Roy Sun) <xwos@xwos.tech>
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

include $(XWOS_WKSPC_DIR)/XWOS.cfg
include xwbs/functions.mk

XWMO_CSRCS := error.c journal.c map.c
XWMO_CFLAGS := -Wno-sign-conversion -Wno-undef -Wno-type-limits
XWMO_CFLAGS += -Wno-unused-parameter -Wno-sign-compare
XWMO_CFLAGS_llvm := -Wno-unused-function
XWMO_INCDIRS := $(call getXwmoDir)
include xwbs/xwmo.mk
