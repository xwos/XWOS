#! /bin/make -f
# @file
# @brief 查找第三方模块/软件包并生成Makefile目标
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

include xwbs/util/mk/xwmo.mk

XWEM_LIST := $(shell test -d $(XuanWuOS_XWEM_DIR) && \
                     find $(XuanWuOS_XWEM_DIR) -type f -name "xwmo.mk" -exec dirname {} \;)

define findXwemTarget
  ifeq ($$(XWEMCFG$(call xwmoPathToName,$(1),$(XuanWuOS_XWEM_DIR))),y)
    XWEM += $(1)/xwem$(call xwmoPathToName,$(1),$(XuanWuOS_XWEM_DIR)).a
    XWEM_DSM += $(1)/xwem$(call xwmoPathToName,$(1),$(XuanWuOS_XWEM_DIR)).a.dsm
    XWEM_CLEAN += $(1)/xwem$(call xwmoPathToName,$(1),$(XuanWuOS_XWEM_DIR)).a.clean
    XWEM_DISTCLEAN += $(1)/xwem$(call xwmoPathToName,$(1),$(XuanWuOS_XWEM_DIR)).a.distclean
  endif
endef

$(foreach xwem,$(XWEM_LIST),$(eval $(call findXwemTarget,$(xwem))))
XWEM := $(addprefix $(XuanWuOS_OBJ_DIR)/,$(XWEM))
XWEM_DSM := $(addprefix $(XuanWuOS_OBJ_DIR)/,$(XWEM_DSM))
XWEM_CLEAN := $(addprefix $(XuanWuOS_OBJ_DIR)/,$(XWEM_CLEAN))
XWEM_DISTCLEAN := $(addprefix $(XuanWuOS_OBJ_DIR)/,$(XWEM_DISTCLEAN))

$(XWEM):
	@echo "building $@ ..."
	$(MAKE) -C $(XuanWuOS_PATH) -f $(subst $(XuanWuOS_OBJ_DIR)/,,$(@D))/xwmo.mk XuanWuOS_BRD_DIR=$(XuanWuOS_BRD_DIR)

$(XWEM_DSM):
	$(MAKE) -C $(XuanWuOS_PATH) -f $(subst $(XuanWuOS_OBJ_DIR)/,,$(@D))/xwmo.mk XuanWuOS_BRD_DIR=$(XuanWuOS_BRD_DIR) NODEP=y dsm

$(XWEM_CLEAN):
	$(MAKE) -C $(XuanWuOS_PATH) -f $(subst $(XuanWuOS_OBJ_DIR)/,,$(@D))/xwmo.mk XuanWuOS_BRD_DIR=$(XuanWuOS_BRD_DIR) NODEP=y clean

$(XWEM_DISTCLEAN):
	$(MAKE) -C $(XuanWuOS_PATH) -f $(subst $(XuanWuOS_OBJ_DIR)/,,$(@D))/xwmo.mk XuanWuOS_BRD_DIR=$(XuanWuOS_BRD_DIR) NODEP=y distclean

xwem.mk.dbg:
	@echo XWEM_LIST:$(XWEM_LIST)
	@echo XWEM:$(XWEM)

.PHONY: xwem.mk.dbg
