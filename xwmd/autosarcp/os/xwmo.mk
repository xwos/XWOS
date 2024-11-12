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

# 引入配置
include $(XWOS_WKSPC_DIR)/XWOS.cfg
include xwbs/functions.mk

# 增加.c源文件
# 函数$(call XwmoWildcard,*.c,xxx)表示查找xxx目录下的所有.c文件
XWMO_CSRCS :=
XWMO_CSRCS += $(call XwmoWildcard,*.c,.)

# 定义编译选项
XWMO_CFLAGS :=

# 定义头文件路径
# 函数$(call getXwmoDir)表示获取当前玄武模块的路径
XWMO_INCDIRS := $(call getXwmoDir)

# 引入编译规则
include xwbs/xwmo.mk
