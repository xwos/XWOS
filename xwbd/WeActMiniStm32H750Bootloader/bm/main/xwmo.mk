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

# 增加源代码文件
XWMO_CSRCS :=
XWMO_CSRCS += main.c xwscp.c

# 定义编译选项
XWMO_CFLAGS :=
XWMO_CFLAGS += -Wno-undef -Wno-unused-parameter

# 定义头文件路径
XWMO_INCDIRS := $(call getXwmoDir) # getXwmoDir函数可获取当前玄武模块的路径

# 引入编译规则
include xwbs/$(XuanWuOS_CFG_XWMO_MK)
