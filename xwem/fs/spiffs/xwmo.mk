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

XWMO_CSRCS := spiffs_nucleus.c spiffs_gc.c spiffs_hydrogen.c spiffs_cache.c spiffs_check.c
XWMO_CFLAGS := -DCONFIG_BUILD_SPIFFS -Wno-sign-conversion -Wno-undef
XWMO_CFLAGS_gcc := -Wno-stringop-truncation -Wno-error=maybe-uninitialized
XWMO_INCDIRS := $(call getXwmoDir)
include xwbs/$(XuanWuOS_CFG_XWMO_MK)
