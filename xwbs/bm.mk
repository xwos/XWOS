#! /bin/make -f
# @file
# @brief 查找工程/电路板模块并生成Makefile目标
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

BM_LIST := $(shell test -d $(XWOS_BM_DIR) && \
                   find $(XWOS_BM_DIR) -type f -name "xwmo.mk" -exec dirname {} \;)

define findBmTarget
  ifeq ($$(BMCFG$(call xwmoPathToName,$(1),$(XWOS_BM_DIR))),y)
    BM += $(1)/bm$(call xwmoPathToName,$(1),$(XWOS_BM_DIR)).a
    BM_DSM += $(1)/bm$(call xwmoPathToName,$(1),$(XWOS_BM_DIR)).a.dsm
    BM_CLEAN += $(1)/bm$(call xwmoPathToName,$(1),$(XWOS_BM_DIR)).a.clean
    BM_DISTCLEAN += $(1)/bm$(call xwmoPathToName,$(1),$(XWOS_BM_DIR)).a.distclean
  endif
endef

$(foreach bm,$(BM_LIST),$(eval $(call findBmTarget,$(bm))))
BM := $(addprefix $(XWOS_OBJ_DIR)/,$(BM))
BM_DSM := $(addprefix $(XWOS_OBJ_DIR)/,$(BM_DSM))
BM_CLEAN := $(addprefix $(XWOS_OBJ_DIR)/,$(BM_CLEAN))
BM_DISTCLEAN := $(addprefix $(XWOS_OBJ_DIR)/,$(BM_DISTCLEAN))

$(BM):
	@echo "building $@ ..."
	$(MAKE) -C $(XWOS_PATH) -f $(subst $(XWOS_OBJ_DIR)/,,$(@D))/xwmo.mk XWOS_BRD_DIR=$(XWOS_BRD_DIR)

$(BM_DSM):
	$(MAKE) -C $(XWOS_PATH) -f $(subst $(XWOS_OBJ_DIR)/,,$(@D))/xwmo.mk XWOS_BRD_DIR=$(XWOS_BRD_DIR) NODEP=y dsm

$(BM_CLEAN):
	$(MAKE) -C $(XWOS_PATH) -f $(subst $(XWOS_OBJ_DIR)/,,$(@D))/xwmo.mk XWOS_BRD_DIR=$(XWOS_BRD_DIR) NODEP=y clean

$(BM_DISTCLEAN):
	$(MAKE) -C $(XWOS_PATH) -f $(subst $(XWOS_OBJ_DIR)/,,$(@D))/xwmo.mk XWOS_BRD_DIR=$(XWOS_BRD_DIR) NODEP=y distclean

bm.mk.dbg:
	@echo BM_LIST:$(BM_LIST)
	@echo BM:$(BM)

.PHONY: bm.mk.dbg
