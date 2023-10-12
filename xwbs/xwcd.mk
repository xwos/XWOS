#! /bin/make -f
# @file
# @brief 查找驱动模块并生成Makefile目标
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

include xwbs/functions.mk

XWCD_LIST := $(shell test -d $(XWOS_XWCD_DIR) && \
                     find $(XWOS_XWCD_DIR) -type f -name "xwmo.mk" -exec dirname {} \;)

define findXwcdTarget
  ifeq ($$(XWCDCFG$(call xwmoPathToName,$(1),$(XWOS_XWCD_DIR))),y)
    XWCD += $(1)/xwcd$(call xwmoPathToName,$(1),$(XWOS_XWCD_DIR)).a
    XWCD_DSM += $(1)/xwcd$(call xwmoPathToName,$(1),$(XWOS_XWCD_DIR)).a.dsm
    XWCD_CLEAN += $(1)/xwcd$(call xwmoPathToName,$(1),$(XWOS_XWCD_DIR)).a.clean
    XWCD_DISTCLEAN += $(1)/xwcd$(call xwmoPathToName,$(1),$(XWOS_XWCD_DIR)).a.distclean
  endif
endef

$(foreach xwcd,$(XWCD_LIST),$(eval $(call findXwcdTarget,$(xwcd))))
XWCD := $(addprefix $(XWOS_OBJ_DIR)/,$(XWCD))
XWCD_DSM := $(addprefix $(XWOS_OBJ_DIR)/,$(XWCD_DSM))
XWCD_CLEAN := $(addprefix $(XWOS_OBJ_DIR)/,$(XWCD_CLEAN))
XWCD_DISTCLEAN := $(addprefix $(XWOS_OBJ_DIR)/,$(XWCD_DISTCLEAN))

$(XWCD):
	@echo "building $@ ..."
	$(MAKE) -C $(XWOS_PATH) -f $(subst $(XWOS_OBJ_DIR)/,,$(@D))/xwmo.mk XWOS_BRD_DIR=$(XWOS_BRD_DIR)

$(XWCD_DSM):
	$(MAKE) -C $(XWOS_PATH) -f $(subst $(XWOS_OBJ_DIR)/,,$(@D))/xwmo.mk XWOS_BRD_DIR=$(XWOS_BRD_DIR) NODEP=y dsm

$(XWCD_CLEAN):
	$(MAKE) -C $(XWOS_PATH) -f $(subst $(XWOS_OBJ_DIR)/,,$(@D))/xwmo.mk XWOS_BRD_DIR=$(XWOS_BRD_DIR) NODEP=y clean

$(XWCD_DISTCLEAN):
	$(MAKE) -C $(XWOS_PATH) -f $(subst $(XWOS_OBJ_DIR)/,,$(@D))/xwmo.mk XWOS_BRD_DIR=$(XWOS_BRD_DIR) NODEP=y distclean

xwcd.mk.dbg:
	@echo XWCD_LIST:$(XWCD_LIST)
	@echo XWCD:$(XWCD)

.PHONY: xwcd.mk.dbg
