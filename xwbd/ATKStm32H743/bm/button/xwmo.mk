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

XWMO_CSRCS := xwmo.c
XWMO_CFLAGS := -Wno-error=unused-value
XWMO_INCDIRS :=
include xwbs/$(XuanWuOS_CFG_XWMO_MK)
