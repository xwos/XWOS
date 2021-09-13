#! /bin/make -f
# @file
# @brief 编译xwmo的makefile辅助函数
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

# Don't use the function `$(dir ...)' in make, because the result has the tailing '/'
getXwmoDir = $(shell dirname $(firstword $(MAKEFILE_LIST)))

xwmoPathToName = $(subst -,_,$(subst .,_,$(subst /,_,$(subst _,__,$(subst $(2),,$(1))))))

getXwmdName = xwmd$(call xwmoPathToName,$(shell dirname $(firstword $(MAKEFILE_LIST))),$(XuanWuOS_XWMD_DIR)).a

getXwcdName = xwcd$(call xwmoPathToName,$(shell dirname $(firstword $(MAKEFILE_LIST))),$(XuanWuOS_XWCD_DIR)).a

getXwemName = xwem$(call xwmoPathToName,$(shell dirname $(firstword $(MAKEFILE_LIST))),$(XuanWuOS_XWEM_DIR)).a

getXwamName = xwam$(call xwmoPathToName,$(shell dirname $(firstword $(MAKEFILE_LIST))),$(XuanWuOS_XWAM_DIR)).a

getBmName = bm$(call xwmoPathToName,$(shell dirname $(firstword $(MAKEFILE_LIST))),$(XuanWuOS_BM_DIR)).a

getOemName = oem$(call xwmoPathToName,$(shell dirname $(firstword $(MAKEFILE_LIST))),$(XuanWuOS_OEM_DIR)).a

define getXwmoName
$(strip
  $(if $(findstring $(XuanWuOS_XWMD_DIR),$(call getXwmoDir)),\
      $(call getXwmdName),\
      $(if $(findstring $(XuanWuOS_XWCD_DIR),$(call getXwmoDir)),\
          $(call getXwcdName),\
          $(if $(findstring $(XuanWuOS_BM_DIR),$(call getXwmoDir)),\
	      $(call getBmName),\
              $(if $(findstring $(XuanWuOS_XWEM_DIR),$(call getXwmoDir)),\
                  $(call getXwemName),\
                  $(if $(findstring $(XuanWuOS_XWAM_DIR),$(call getXwmoDir)),\
                      $(call getXwamName),\
		      $(if $(findstring $(XuanWuOS_OEM_DIR),$(call getXwmoDir)),\
			  $(call getOemName),\
			  "")))))))
endef

getAllFileUnderXwmoDir = $(subst $(call getXwmoDir)/,,$(wildcard $(call getXwmoDir)/$(2)/$(1)))

define getAllFileUnderXwmoDirRecursively
$(subst $(call getXwmoDir)/,,$(shell find $(call getXwmoDir)/$(2) -name $(1)))
endef

define XwmoReqCfg
ifneq ($$($(1)),$(2))
    $$(error "$(1) is not configured to $(2)!")
endif
endef
