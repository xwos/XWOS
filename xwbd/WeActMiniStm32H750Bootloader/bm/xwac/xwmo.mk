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
include xwbs/util/mk/xwmo.mk

# 增加源代码文件
XWMO_CSRCS :=

XWMO_CSRCS += xwos/xwskd_hook.c

XWMO_CSRCS += xwds/core.c
XWMO_CSRCS += xwds/device.c
XWMO_CSRCS += xwds/soc.c
XWMO_CSRCS += xwds/uart.c

XWMO_CSRCS += xwlib/log.c
XWMO_CSRCS += xwlib/crc.c

# 定义编译选项
XWMO_CFLAGS :=
XWMO_CFLAGS += -Wno-unused-parameter -Wno-sign-conversion

# 定义头文件路径
XWMO_INCDIRS :=

# 引入编译规则
include xwbs/xwmo.mk
