#! /bin/make -f
# @file
# @brief 编译模块并生成.a的Makefile模板 (ARM-cortex-m-gcc)
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

include xwbs/$(XuanWuOS_CFG_ARCH).$(XuanWuOS_CFG_COMPILER).rule

XWMO_NAME := $(call getXwmoName)
XWMO_DIR := $(call getXwmoDir)
XWMO_OBJ_DIR ?= $(XWMO_DIR)
XWMO_PREBUILT := $(firstword $(XWMO_PREBUILT))

$(XuanWuOS_OBJ_DIR)/$(XWMO_OBJ_DIR)/$(XWMO_NAME): $(XWMO_DIR)/$(XWMO_PREBUILT)
	@[ ! -d $(@D) ] && mkdir -p $(@D) || true
	$(SHOW_CP) $(CP) $< $@

clean:
	@$(RM) -f $(XuanWuOS_OBJ_DIR)/$(XWMO_OBJ_DIR)/$(XWMO_NAME)

distclean:
	$(RM) -rf $(XuanWuOS_OBJ_DIR)/$(XWMO_OBJ_DIR)

.PHONY: dsm clean distclean
