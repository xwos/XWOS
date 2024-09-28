#! /bin/make -f
# @file
# @brief 查找OEM模块/软件包并生成Makefile目标
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

OEM_LIST := $(shell test -d $(XWOS_OEM_DIR) && \
                    find $(XWOS_OEM_DIR) -type f -name "xwmo.mk" -exec dirname {} \;)

define findOemTarget
  ifeq ($$(OEMCFG$(call xwmoPathToName,$(1),$(XWOS_OEM_DIR))),y)
    OEM += $(subst $(XWOS_OEM_DIR)/,,$(1))/oem$(call xwmoPathToName,$(1),$(XWOS_OEM_DIR)).a
    OEM_DSM += $(subst $(XWOS_OEM_DIR)/,,$(1))/oem$(call xwmoPathToName,$(1),$(XWOS_OEM_DIR)).a.dsm
    OEM_CLEAN += $(subst $(XWOS_OEM_DIR)/,,$(1))/oem$(call xwmoPathToName,$(1),$(XWOS_OEM_DIR)).a.clean
    OEM_DISTCLEAN += $(subst $(XWOS_OEM_DIR)/,,$(1))/oem$(call xwmoPathToName,$(1),$(XWOS_OEM_DIR)).a.distclean
  endif
endef

$(foreach oem,$(OEM_LIST),$(eval $(call findOemTarget,$(oem))))
OEM := $(addprefix $(XWOS_OEMOBJ_DIR)/,$(OEM))
OEM_DSM := $(addprefix $(XWOS_OEMOBJ_DIR)/,$(OEM_DSM))
OEM_CLEAN := $(addprefix $(XWOS_OEMOBJ_DIR)/,$(OEM_CLEAN))
OEM_DISTCLEAN := $(addprefix $(XWOS_OEMOBJ_DIR)/,$(OEM_DISTCLEAN))

$(OEM):
	@echo "building $@ ..."
	$(MAKE) $(strip -C $(XWOS_PATH) \
                        -f $(subst $(XWOS_OEMOBJ_DIR),$(XWOS_OEM_DIR),$(@D))/xwmo.mk \
                        XWOS_BRD_DIR=$(XWOS_BRD_DIR) \
                        XWMO_OBJ_DIR=$(XWOS_OBJ_DIR_BACK_WKSPCOBJ_DIR)/$(subst $(XWOS_OEMOBJ_DIR),oem,$(@D)))

$(OEM_DSM):
	$(MAKE) $(strip -C $(XWOS_PATH) \
                        -f $(subst $(XWOS_OEMOBJ_DIR),$(XWOS_OEM_DIR),$(@D))/xwmo.mk \
                        XWOS_BRD_DIR=$(XWOS_BRD_DIR) \
                        XWMO_OBJ_DIR=$(XWOS_OBJ_DIR_BACK_WKSPCOBJ_DIR)/$(subst $(XWOS_OEMOBJ_DIR),oem,$(@D)) \
                        NODEP=y \
                        dsm)

$(OEM_CLEAN):
	$(MAKE) $(strip -C $(XWOS_PATH) \
                        -f $(subst $(XWOS_OEMOBJ_DIR),$(XWOS_OEM_DIR),$(@D))/xwmo.mk \
                        XWOS_BRD_DIR=$(XWOS_BRD_DIR) \
                        XWMO_OBJ_DIR=$(XWOS_OBJ_DIR_BACK_WKSPCOBJ_DIR)/$(subst $(XWOS_OEMOBJ_DIR),oem,$(@D)) \
                        NODEP=y \
                        clean)

$(OEM_DISTCLEAN):
	$(MAKE) $(strip -C $(XWOS_PATH) \
                        -f $(subst $(XWOS_OEMOBJ_DIR),$(XWOS_OEM_DIR),$(@D))/xwmo.mk \
		        XWOS_BRD_DIR=$(XWOS_BRD_DIR) \
                        XWMO_OBJ_DIR=$(XWOS_OBJ_DIR_BACK_WKSPCOBJ_DIR)/$(subst $(XWOS_OEMOBJ_DIR),oem,$(@D)) \
                        NODEP=y \
                        distclean)

oem.mk.dbg:
	@echo OEM_LIST:$(OEM_LIST)
	@echo OEM:$(OEM)

.PHONY: oem.mk.dbg
