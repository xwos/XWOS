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
include $(XWOS_WKSPC_DIR)/XuanWuOS.cfg
include $(XWBS_UTIL_MK_XWMO)

XWMO_CSRCS :=
XWMO_CSRCS += $(call getAllFileUnderXwmoDir,*.c,pe/Generated_Code)
XWMO_CSRCS += $(call getAllFileUnderXwmoDir,*.c,sdk/platform/drivers/src/clock/S32K1xx)
XWMO_CSRCS += $(call getAllFileUnderXwmoDir,*.c,sdk/platform/drivers/src/interrupt)
XWMO_CSRCS += $(call getAllFileUnderXwmoDir,*.c,sdk/platform/drivers/src/pins)
XWMO_CSRCS += $(call getAllFileUnderXwmoDir,*.c,sdk/platform/drivers/src/power)
XWMO_CSRCS += $(call getAllFileUnderXwmoDir,*.c,sdk/platform/drivers/src/power/S32K1xx)
XWMO_CSRCS += $(call getAllFileUnderXwmoDir,*.c,sdk/platform/drivers/src/wdog)
XWMO_CSRCS += $(call getAllFileUnderXwmoDir,*.c,xwac)
XWMO_CSRCS += xwmo.c

# 定义编译选项
XWMO_CFLAGS :=
XWMO_CFLAGS += -DCPU_S32K144 -DCPU_S32K144HFT0VLLT -DTURN_ON_CPU0
XWMO_CFLAGS += -Wno-unused-parameter

# 定义头文件路径
XWMO_INCDIRS :=
XWMO_INCDIRS += $(call getXwmoDir)/pe/Generated_Code
XWMO_INCDIRS += $(call getXwmoDir)/pe/Sources
XWMO_INCDIRS += $(call getXwmoDir)/sdk/platform/drivers/inc
XWMO_INCDIRS += $(call getXwmoDir)/sdk/platform/drivers/src/clock/S32K1xx
XWMO_INCDIRS += $(call getXwmoDir)/sdk/platform/drivers/src/power/S32K1xx
XWMO_INCDIRS += $(call getXwmoDir)/sdk/platform/devices
XWMO_INCDIRS += $(call getXwmoDir)/sdk/platform/devices/common
XWMO_INCDIRS += $(call getXwmoDir)/sdk/platform/devices/S32K144/include
XWMO_INCDIRS += $(call getXwmoDir)/sdk/platform/devices/S32K144/startup

# 引入编译规则
include xwbs/$(XuanWuOS_CFG_XWMO_MK)
