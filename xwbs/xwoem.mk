#! /bin/make -f
# @file
# @brief 查找OEM模块/软件包并生成Makefile目标
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

XWOEM_LIST := $(shell test -d $(XWOS_OEM_DIR) && \
                     find $(XWOS_OEM_DIR) -type f -name "xwmo.mk" -exec dirname {} \;)

define findXwoemTarget
ifeq ($$(OEMCFG$(call xwmoPathToName,$(1),$(XWOS_OEM_DIR))),y)
    XWOEM += $(subst $(XWOS_OEM_DIR)/,,$(1))/xwoem$(call xwmoPathToName,$(1),$(XWOS_OEM_DIR)).a
    XWOEM_DSM += $(subst $(XWOS_OEM_DIR)/,,$(1))/xwoem$(call xwmoPathToName,$(1),$(XWOS_OEM_DIR)).a.dsm
    XWOEM_CLEAN += $(subst $(XWOS_OEM_DIR)/,,$(1))/xwoem$(call xwmoPathToName,$(1),$(XWOS_OEM_DIR)).a.clean
    XWOEM_DISTCLEAN += $(subst $(XWOS_OEM_DIR)/,,$(1))/xwoem$(call xwmoPathToName,$(1),$(XWOS_OEM_DIR)).a.distclean
endif
endef

$(foreach xwoem,$(XWOEM_LIST),$(eval $(call findXwoemTarget,$(xwoem))))
XWOEM := $(addprefix $(XWOS_OEMOBJ_DIR)/,$(XWOEM))
XWOEM_DSM := $(addprefix $(XWOS_OEMOBJ_DIR)/,$(XWOEM_DSM))
XWOEM_CLEAN := $(addprefix $(XWOS_OEMOBJ_DIR)/,$(XWOEM_CLEAN))
XWOEM_DISTCLEAN := $(addprefix $(XWOS_OEMOBJ_DIR)/,$(XWOEM_DISTCLEAN))

$(XWOEM):
	@echo "building $@ ..."
	$(MAKE) $(strip -C $(XWOS_PATH) \
                        -f $(subst $(XWOS_OEMOBJ_DIR),$(XWOS_OEM_DIR),$(@D))/xwmo.mk \
                        XWOS_BRD_DIR=$(XWOS_BRD_DIR) \
                        XWMO_OBJ_DIR=$(XWOS_BWDPATH)/$(subst $(XWOS_OEMOBJ_DIR),oem,$(@D)))

$(XWOEM_DSM):
	$(MAKE) $(strip -C $(XWOS_PATH) \
                        -f $(subst $(XWOS_OEMOBJ_DIR),$(XWOS_OEM_DIR),$(@D))/xwmo.mk \
                        XWOS_BRD_DIR=$(XWOS_BRD_DIR) \
                        XWMO_OBJ_DIR=$(XWOS_BWDPATH)/$(subst $(XWOS_OEMOBJ_DIR),oem,$(@D)) \
                        NODEP=y \
                        dsm)

$(XWOEM_CLEAN):
	$(MAKE) $(strip -C $(XWOS_PATH) \
                        -f $(subst $(XWOS_OEMOBJ_DIR),$(XWOS_OEM_DIR),$(@D))/xwmo.mk \
                        XWOS_BRD_DIR=$(XWOS_BRD_DIR) \
                        XWMO_OBJ_DIR=$(XWOS_BWDPATH)/$(subst $(XWOS_OEMOBJ_DIR),oem,$(@D)) \
                        NODEP=y \
                        clean)

$(XWOEM_DISTCLEAN):
	$(MAKE) $(strip -C $(XWOS_PATH) \
                        -f $(subst $(XWOS_OEMOBJ_DIR),$(XWOS_OEM_DIR),$(@D))/xwmo.mk \
		        XWOS_BRD_DIR=$(XWOS_BRD_DIR) \
                        XWMO_OBJ_DIR=$(XWOS_BWDPATH)/$(subst $(XWOS_OEMOBJ_DIR),oem,$(@D)) \
                        NODEP=y \
                        distclean)

xwoem.mk.dbg:
	@echo XWOEM_LIST:$(XWOEM_LIST)
	@echo XWOEM:$(XWOEM)

.PHONY: xwoem.mk.dbg
