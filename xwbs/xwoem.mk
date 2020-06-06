#! /bin/make -f
# @file
# @brief 查找OEM模块/软件包并生成Makefile目标
# @author
# + 隐星魂 (Roy.Sun) <www.starsoul.tech>
# @copyright
# + (c) 2015 隐星魂 (Roy.Sun) <www.starsoul.tech>
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

XWOEM_LIST := $(shell test -d $(XWOS_OEM_DIR) && \
                     find $(XWOS_OEM_DIR) -type f -name "xwmo.mk" -exec dirname {} \;)
XWOEM_LIST := $(addprefix oem,$(subst $(XWOS_OEM_DIR),,$(XWOEM_LIST)))
#$(info XWOEM_LIST:$(XWOEM_LIST))

define findXwoemTarget
ifeq ($$(OEMCFG$(call xwmoPathToName,$(1),oem)),y)
    XWOEM += $(1)/xwoem$(call xwmoPathToName,$(1),oem).a
    XWOEM_DSM += $(1)/xwoem$(call xwmoPathToName,$(1),oem).a.dsm
    XWOEM_CLEAN += $(1)/xwoem$(call xwmoPathToName,$(1),oem).a.clean
    XWOEM_DISTCLEAN += $(1)/xwoem$(call xwmoPathToName,$(1),oem).a.distclean
endif
endef

$(foreach xwoem,$(XWOEM_LIST),$(eval $(call findXwoemTarget,$(xwoem))))

XWOEM := $(addprefix $(OBJ_DIR),$(XWOEM))
XWOEM_DSM := $(addprefix $(OBJ_DIR),$(XWOEM_DSM))
XWOEM_CLEAN := $(addprefix $(OBJ_DIR),$(XWOEM_CLEAN))
XWOEM_DISTCLEAN := $(addprefix $(OBJ_DIR),$(XWOEM_DISTCLEAN))

$(XWOEM):
	@echo "building $@ ..."
	@$(MAKE) -C $(XWOS_PATH) -f $(subst $(OBJ_DIR)oem,$(XWOS_OEM_DIR),$(@D))/xwmo.mk \
		XWOS_BRD_DIR=$(XWOS_BRD_DIR) XWMO_OBJ_DIR=$(subst $(OBJ_DIR),,$(@D))

$(XWOEM_DSM):
	@$(MAKE) -C $(XWOS_PATH) -f $(subst $(OBJ_DIR)oem,$(XWOS_OEM_DIR),$(@D))/xwmo.mk \
		XWOS_BRD_DIR=$(XWOS_BRD_DIR) XWMO_OBJ_DIR=$(subst $(OBJ_DIR),,$(@D)) NODEP=y \
		dsm

$(XWOEM_CLEAN):
	@$(MAKE) -C $(XWOS_PATH) -f $(subst $(OBJ_DIR)oem,$(XWOS_OEM_DIR),$(@D))/xwmo.mk \
		XWOS_BRD_DIR=$(XWOS_BRD_DIR) XWMO_OBJ_DIR=$(subst $(OBJ_DIR),,$(@D)) NODEP=y \
		clean

$(XWOEM_DISTCLEAN):
	@$(MAKE) -C $(XWOS_PATH) -f $(subst $(OBJ_DIR)oem,$(XWOS_OEM_DIR),$(@D))/xwmo.mk \
		XWOS_BRD_DIR=$(XWOS_BRD_DIR) XWMO_OBJ_DIR=$(subst $(OBJ_DIR),,$(@D)) NODEP=y \
		distclean

xwoem.mk.dbg:
	@echo XWOEM_LIST:$(XWOEM_LIST)
	@echo XWOEM:$(XWOEM)

.PHONY: xwoem.mk.dbg
