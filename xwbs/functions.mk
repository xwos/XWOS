#! /bin/make -f
# @file
# @brief 编译xwmo的makefile辅助函数
# @author
# + 隐星曜 (Roy Sun) <xwos@xwos.tech>
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

# Don't use the function `$(dir ...)', because the result has the tailing '/'
getXwmoDir = $(shell dirname $(firstword $(MAKEFILE_LIST)))

xwmoPathToName = $(subst -,_,$(subst .,_,$(subst /,_,$(subst _,__,$(subst $(2),,$(1))))))

getXwmdName = xwmd$(call xwmoPathToName,$(shell dirname $(firstword $(MAKEFILE_LIST))),$(XWOS_XWMD_DIR)).a

getXwcdName = xwcd$(call xwmoPathToName,$(shell dirname $(firstword $(MAKEFILE_LIST))),$(XWOS_XWCD_DIR)).a

getXwemName = xwem$(call xwmoPathToName,$(shell dirname $(firstword $(MAKEFILE_LIST))),$(XWOS_XWEM_DIR)).a

getXwamName = xwam$(call xwmoPathToName,$(shell dirname $(firstword $(MAKEFILE_LIST))),$(XWOS_XWAM_DIR)).a

getBmName = bm$(call xwmoPathToName,$(shell dirname $(firstword $(MAKEFILE_LIST))),$(XWOS_BM_DIR)).a

getOemName = oem$(call xwmoPathToName,$(shell dirname $(firstword $(MAKEFILE_LIST))),$(XWOS_OEM_DIR)).a

define getXwmoName
$(strip
  $(if $(findstring $(XWOS_XWMD_DIR),$(call getXwmoDir)),\
      $(call getXwmdName),\
      $(if $(findstring $(XWOS_XWCD_DIR),$(call getXwmoDir)),\
          $(call getXwcdName),\
          $(if $(findstring $(XWOS_BM_DIR),$(call getXwmoDir)),\
	      $(call getBmName),\
              $(if $(findstring $(XWOS_XWEM_DIR),$(call getXwmoDir)),\
                  $(call getXwemName),\
                  $(if $(findstring $(XWOS_XWAM_DIR),$(call getXwmoDir)),\
                      $(call getXwamName),\
		      $(if $(findstring $(XWOS_OEM_DIR),$(call getXwmoDir)),\
			  $(call getOemName),\
			  "")))))))
endef

XwmoWildcard = $(subst $(call getXwmoDir)/,,$(wildcard $(call getXwmoDir)/$(2)/$(1)))

define XwmoWildcardRecursively
$(subst $(call getXwmoDir)/,,$(shell find $(call getXwmoDir)/$(2) -name $(1)))
endef

XwosWildcard = $(subst $(XWOS_OS_DIR)/,,$(wildcard $(XWOS_OS_DIR)/$(2)/$(1)))

define XwosWildcardRecursively
$(subst $(XWOS_OS_DIR)/,,$(shell find $(XWOS_OS_DIR)/$(2) -name $(1)))
endef

ArchWildcard = $(subst $(XWOS_ARCH_DIR)/,,$(wildcard $(XWOS_ARCH_DIR)/$(2)/$(1)))

define ArchWildcardRecursively
$(subst $(XWOS_ARCH_DIR)/,,$(shell find $(XWOS_ARCH_DIR)/$(2) -name $(1)))
endef

CpuWildcard = $(subst $(XWOS_CPU_DIR)/,,$(wildcard $(XWOS_CPU_DIR)/$(2)/$(1)))

define CpuWildcardRecursively
$(subst $(XWOS_CPU_DIR)/,,$(shell find $(XWOS_CPU_DIR)/$(2) -name $(1)))
endef

SocWildcard = $(subst $(XWOS_SOC_DIR)/,,$(wildcard $(XWOS_SOC_DIR)/$(2)/$(1)))

define SocWildcardRecursively
$(subst $(XWOS_SOC_DIR)/,,$(shell find $(XWOS_SOC_DIR)/$(2) -name $(1)))
endef

BrdWildcard = $(subst $(XWOS_BRD_DIR)/,,$(wildcard $(XWOS_BRD_DIR)/$(2)/$(1)))

define BrdWildcardRecursively
$(subst $(XWOS_BRD_DIR)/,,$(shell find $(XWOS_BRD_DIR)/$(2) -name $(1)))
endef

define XwmoReqCfg
ifneq ($$($(1)),$(2))
    $$(error "$(1) is not configured to $(2)!")
endif
endef

define XwmoReqNotCfg
ifeq ($$($(1)),$(2))
    $$(error "$(1) is configured to $(2)!")
endif
endef
