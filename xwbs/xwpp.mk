#! /bin/make -f
# @file
# @brief 查找外设驱动模块并生成Makefile目标
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

XWPP_LIST := $(shell test -d $(XWOS_PP_DIR) && \
                     find $(XWOS_PP_DIR) -type f -name "xwmo.mk" -exec dirname {} \;)

define findXwppTarget
ifeq ($$(PPCFG$(call xwmoPathToName,$(1),$(XWOS_PP_DIR))),y)
    XWPP += $(1)/xwpp$(call xwmoPathToName,$(1),$(XWOS_PP_DIR)).a
    XWPP_DSM += $(1)/xwpp$(call xwmoPathToName,$(1),$(XWOS_PP_DIR)).a.dsm
    XWPP_CLEAN += $(1)/xwpp$(call xwmoPathToName,$(1),$(XWOS_PP_DIR)).a.clean
    XWPP_DISTCLEAN += $(1)/xwpp$(call xwmoPathToName,$(1),$(XWOS_PP_DIR)).a.distclean
endif
endef

$(foreach xwpp,$(XWPP_LIST),$(eval $(call findXwppTarget,$(xwpp))))

XWPP := $(addprefix $(OBJ_DIR),$(XWPP))
XWPP_DSM := $(addprefix $(OBJ_DIR),$(XWPP_DSM))
XWPP_CLEAN := $(addprefix $(OBJ_DIR),$(XWPP_CLEAN))
XWPP_DISTCLEAN := $(addprefix $(OBJ_DIR),$(XWPP_DISTCLEAN))

$(XWPP):
	@echo "building $@ ..."
	@$(MAKE) -C $(XWOS_PATH) -f $(subst $(OBJ_DIR),,$(@D))/xwmo.mk XWOS_BRD_DIR=$(XWOS_BRD_DIR)

$(XWPP_DSM):
	@$(MAKE) -C $(XWOS_PATH) -f $(subst $(OBJ_DIR),,$(@D))/xwmo.mk XWOS_BRD_DIR=$(XWOS_BRD_DIR) NODEP=y dsm

$(XWPP_CLEAN):
	@$(MAKE) -C $(XWOS_PATH) -f $(subst $(OBJ_DIR),,$(@D))/xwmo.mk XWOS_BRD_DIR=$(XWOS_BRD_DIR) NODEP=y clean

$(XWPP_DISTCLEAN):
	@$(MAKE) -C $(XWOS_PATH) -f $(subst $(OBJ_DIR),,$(@D))/xwmo.mk XWOS_BRD_DIR=$(XWOS_BRD_DIR) NODEP=y distclean

xwpp.mk.dbg:
	@echo XWPP_LIST:$(XWPP_LIST)
	@echo XWPP:$(XWPP)

.PHONY: xwpp.mk.dbg
