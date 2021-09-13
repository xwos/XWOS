#! /bin/make -f
# @file
# @brief 查找应用模块/软件包并生成Makefile目标
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

XWAM_LIST := $(shell test -d $(XuanWuOS_XWAM_DIR) && \
                     find $(XuanWuOS_XWAM_DIR) -type f -name "xwmo.mk" -exec dirname {} \;)

define findXwamTarget
  ifeq ($$(XWAMCFG$(call xwmoPathToName,$(1),$(XuanWuOS_XWAM_DIR))),y)
    XWAM += $(1)/xwam$(call xwmoPathToName,$(1),$(XuanWuOS_XWAM_DIR)).a
    XWAM_DSM += $(1)/xwam$(call xwmoPathToName,$(1),$(XuanWuOS_XWAM_DIR)).a.dsm
    XWAM_CLEAN += $(1)/xwam$(call xwmoPathToName,$(1),$(XuanWuOS_XWAM_DIR)).a.clean
    XWAM_DISTCLEAN += $(1)/xwam$(call xwmoPathToName,$(1),$(XuanWuOS_XWAM_DIR)).a.distclean
  endif
endef

$(foreach xwam,$(XWAM_LIST),$(eval $(call findXwamTarget,$(xwam))))
XWAM := $(addprefix $(XuanWuOS_OBJ_DIR)/,$(XWAM))
XWAM_DSM := $(addprefix $(XuanWuOS_OBJ_DIR)/,$(XWAM_DSM))
XWAM_CLEAN := $(addprefix $(XuanWuOS_OBJ_DIR)/,$(XWAM_CLEAN))
XWAM_DISTCLEAN := $(addprefix $(XuanWuOS_OBJ_DIR)/,$(XWAM_DISTCLEAN))

$(XWAM):
	@echo "building $@ ..."
	$(MAKE) -C $(XuanWuOS_PATH) -f $(subst $(XuanWuOS_OBJ_DIR)/,,$(@D))/xwmo.mk XuanWuOS_BRD_DIR=$(XuanWuOS_BRD_DIR)

$(XWAM_DSM):
	$(MAKE) -C $(XuanWuOS_PATH) -f $(subst $(XuanWuOS_OBJ_DIR)/,,$(@D))/xwmo.mk XuanWuOS_BRD_DIR=$(XuanWuOS_BRD_DIR) NODEP=y dsm

$(XWAM_CLEAN):
	$(MAKE) -C $(XuanWuOS_PATH) -f $(subst $(XuanWuOS_OBJ_DIR)/,,$(@D))/xwmo.mk XuanWuOS_BRD_DIR=$(XuanWuOS_BRD_DIR) NODEP=y clean

$(XWAM_DISTCLEAN):
	$(MAKE) -C $(XuanWuOS_PATH) -f $(subst $(XuanWuOS_OBJ_DIR)/,,$(@D))/xwmo.mk XuanWuOS_BRD_DIR=$(XuanWuOS_BRD_DIR) NODEP=y distclean

xwam.mk.dbg:
	@echo XWAM_LIST:$(XWAM_LIST)
	@echo XWAM:$(XWAM)

.PHONY: xwam.mk.dbg
