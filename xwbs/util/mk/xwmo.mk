#! /bin/make -f
# @file
# @brief 编译xwmo的makefile辅助函数
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

# Don't use the function `$(dir ...)' in make, because the result has the tailing '/'
getXwmoDir = $(shell dirname $(firstword $(MAKEFILE_LIST)))

xwmoPathToName = $(subst -,_,$(subst .,_,$(subst /,_,$(subst _,__,$(subst $(2),,$(1))))))

getXwmdName = xwmd$(call xwmoPathToName,$(shell dirname $(firstword $(MAKEFILE_LIST))),$(XWOS_MD_DIR)).a

getXwemName = xwem$(call xwmoPathToName,$(shell dirname $(firstword $(MAKEFILE_LIST))),$(XWOS_EM_DIR)).a

getXwppName = xwpp$(call xwmoPathToName,$(shell dirname $(firstword $(MAKEFILE_LIST))),$(XWOS_PP_DIR)).a

getXwbmName = xwbm$(call xwmoPathToName,$(shell dirname $(firstword $(MAKEFILE_LIST))),$(XWOS_BM_DIR)).a

getXwoemName = xwoem$(call xwmoPathToName,$(shell dirname $(firstword $(MAKEFILE_LIST))),$(XWOS_OEM_DIR)).a

define getXwmoName
$(strip
  $(if $(findstring $(XWOS_MD_DIR),$(call getXwmoDir)),\
      $(call getXwmdName),\
      $(if $(findstring $(XWOS_EM_DIR),$(call getXwmoDir)),\
          $(call getXwemName),\
          $(if $(findstring $(XWOS_PP_DIR),$(call getXwmoDir)),\
              $(call getXwppName),\
              $(if $(findstring $(XWOS_BM_DIR),$(call getXwmoDir)),\
	          $(call getXwbmName),\
		      $(if $(findstring $(XWOS_OEM_DIR),$(call getXwmoDir)),\
			  $(call getXwoemName),\
			  ""))))))
endef
