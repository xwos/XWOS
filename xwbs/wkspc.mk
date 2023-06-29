#! /bin/make -f
# @file
# @brief L2 building makefile in workspace
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

include XWOS.cfg

MAKE_ELF_ARGS = $(strip -C $(XWOS_PATH) -f xwbs/$(XWOS_CFG_ELF_MK) XWOS_WKSPC_DIR=$(XWOS_WKSPC_DIR))
MAKE_MISRA_C_ARGS = $(strip -C $(XWOS_PATH) -f xwbs/misra.c.mk XWOS_WKSPC_DIR=$(XWOS_WKSPC_DIR))

all:
	$(MAKE) $(MAKE_ELF_ARGS) all

clean:
	@$(MAKE) $(MAKE_ELF_ARGS) clean

distclean:
	@$(MAKE) $(MAKE_ELF_ARGS) distclean

%.mc: FORCE
	$(MAKE) $(MAKE_MISRA_C_ARGS) $@

mc: FORCE
	$(MAKE) $(MAKE_MISRA_C_ARGS) \
		$(XWOS_ARCH_DIR) \
		$(XWOS_CPU_DIR) \
		$(XWOS_SOC_DIR) \
		$(XWOS_BRD_DIR) \
		$(XWOS_OS_DIR) \
		$(XWOS_XWMD_DIR)

.DEFAULT:
	$(MAKE) $(MAKE_ELF_ARGS) $@

FORCE:

.PHONY : all clean distclean misra-c/xwos
