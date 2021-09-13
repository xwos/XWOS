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

OEM_LIST := $(shell test -d $(XuanWuOS_OEM_DIR) && \
                    find $(XuanWuOS_OEM_DIR) -type f -name "xwmo.mk" -exec dirname {} \;)

define findOemTarget
  ifeq ($$(OEMCFG$(call xwmoPathToName,$(1),$(XuanWuOS_OEM_DIR))),y)
    OEM += $(subst $(XuanWuOS_OEM_DIR)/,,$(1))/oem$(call xwmoPathToName,$(1),$(XuanWuOS_OEM_DIR)).a
    OEM_DSM += $(subst $(XuanWuOS_OEM_DIR)/,,$(1))/oem$(call xwmoPathToName,$(1),$(XuanWuOS_OEM_DIR)).a.dsm
    OEM_CLEAN += $(subst $(XuanWuOS_OEM_DIR)/,,$(1))/oem$(call xwmoPathToName,$(1),$(XuanWuOS_OEM_DIR)).a.clean
    OEM_DISTCLEAN += $(subst $(XuanWuOS_OEM_DIR)/,,$(1))/oem$(call xwmoPathToName,$(1),$(XuanWuOS_OEM_DIR)).a.distclean
  endif
endef

$(foreach oem,$(OEM_LIST),$(eval $(call findOemTarget,$(oem))))
OEM := $(addprefix $(XuanWuOS_OEMOBJ_DIR)/,$(OEM))
OEM_DSM := $(addprefix $(XuanWuOS_OEMOBJ_DIR)/,$(OEM_DSM))
OEM_CLEAN := $(addprefix $(XuanWuOS_OEMOBJ_DIR)/,$(OEM_CLEAN))
OEM_DISTCLEAN := $(addprefix $(XuanWuOS_OEMOBJ_DIR)/,$(OEM_DISTCLEAN))

$(OEM):
	@echo "building $@ ..."
	$(MAKE) $(strip -C $(XuanWuOS_PATH) \
                        -f $(subst $(XuanWuOS_OEMOBJ_DIR),$(XuanWuOS_OEM_DIR),$(@D))/xwmo.mk \
                        XuanWuOS_BRD_DIR=$(XuanWuOS_BRD_DIR) \
                        XWMO_OBJ_DIR=$(XuanWuOS_BWDPATH)/$(subst $(XuanWuOS_OEMOBJ_DIR),oem,$(@D)))

$(OEM_DSM):
	$(MAKE) $(strip -C $(XuanWuOS_PATH) \
                        -f $(subst $(XuanWuOS_OEMOBJ_DIR),$(XuanWuOS_OEM_DIR),$(@D))/xwmo.mk \
                        XuanWuOS_BRD_DIR=$(XuanWuOS_BRD_DIR) \
                        XWMO_OBJ_DIR=$(XuanWuOS_BWDPATH)/$(subst $(XuanWuOS_OEMOBJ_DIR),oem,$(@D)) \
                        NODEP=y \
                        dsm)

$(OEM_CLEAN):
	$(MAKE) $(strip -C $(XuanWuOS_PATH) \
                        -f $(subst $(XuanWuOS_OEMOBJ_DIR),$(XuanWuOS_OEM_DIR),$(@D))/xwmo.mk \
                        XuanWuOS_BRD_DIR=$(XuanWuOS_BRD_DIR) \
                        XWMO_OBJ_DIR=$(XuanWuOS_BWDPATH)/$(subst $(XuanWuOS_OEMOBJ_DIR),oem,$(@D)) \
                        NODEP=y \
                        clean)

$(OEM_DISTCLEAN):
	$(MAKE) $(strip -C $(XuanWuOS_PATH) \
                        -f $(subst $(XuanWuOS_OEMOBJ_DIR),$(XuanWuOS_OEM_DIR),$(@D))/xwmo.mk \
		        XuanWuOS_BRD_DIR=$(XuanWuOS_BRD_DIR) \
                        XWMO_OBJ_DIR=$(XuanWuOS_BWDPATH)/$(subst $(XuanWuOS_OEMOBJ_DIR),oem,$(@D)) \
                        NODEP=y \
                        distclean)

oem.mk.dbg:
	@echo OEM_LIST:$(OEM_LIST)
	@echo OEM:$(OEM)

.PHONY: oem.mk.dbg
