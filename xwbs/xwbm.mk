#! /bin/make -f
# @file
# @brief 查找工程/电路板模块并生成Makefile目标
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

XWBM_LIST := $(shell test -d $(XWOS_BM_DIR) && \
                     find $(XWOS_BM_DIR) -type f -name "xwmo.mk" -exec dirname {} \;)

define findXwbmTarget
ifeq ($$(BMCFG$(call xwmoPathToName,$(1),$(XWOS_BM_DIR))),y)
    XWBM += $(1)/xwbm$(call xwmoPathToName,$(1),$(XWOS_BM_DIR)).a
    XWBM_DSM += $(1)/xwbm$(call xwmoPathToName,$(1),$(XWOS_BM_DIR)).a.dsm
    XWBM_CLEAN += $(1)/xwbm$(call xwmoPathToName,$(1),$(XWOS_BM_DIR)).a.clean
    XWBM_DISTCLEAN += $(1)/xwbm$(call xwmoPathToName,$(1),$(XWOS_BM_DIR)).a.distclean
endif
endef

$(foreach xwbm,$(XWBM_LIST),$(eval $(call findXwbmTarget,$(xwbm))))

XWBM := $(addprefix $(OBJ_DIR),$(XWBM))
XWBM_DSM := $(addprefix $(OBJ_DIR),$(XWBM_DSM))
XWBM_CLEAN := $(addprefix $(OBJ_DIR),$(XWBM_CLEAN))
XWBM_DISTCLEAN := $(addprefix $(OBJ_DIR),$(XWBM_DISTCLEAN))

$(XWBM):
	@echo "building $@ ..."
	@$(MAKE) -C $(XWOS_PATH) -f $(subst $(OBJ_DIR),,$(@D))/xwmo.mk XWOS_BRD_DIR=$(XWOS_BRD_DIR)

$(XWBM_DSM):
	@$(MAKE) -C $(XWOS_PATH) -f $(subst $(OBJ_DIR),,$(@D))/xwmo.mk XWOS_BRD_DIR=$(XWOS_BRD_DIR) NODEP=y dsm

$(XWBM_CLEAN):
	@$(MAKE) -C $(XWOS_PATH) -f $(subst $(OBJ_DIR),,$(@D))/xwmo.mk XWOS_BRD_DIR=$(XWOS_BRD_DIR) NODEP=y clean

$(XWBM_DISTCLEAN):
	@$(MAKE) -C $(XWOS_PATH) -f $(subst $(OBJ_DIR),,$(@D))/xwmo.mk XWOS_BRD_DIR=$(XWOS_BRD_DIR) NODEP=y distclean

xwbm.mk.dbg:
	@echo XWBM_LIST:$(XWBM_LIST)
	@echo XWBM:$(XWBM)

.PHONY: xwbm.mk.dbg
