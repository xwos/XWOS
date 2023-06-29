#! /bin/make -f
# @file
# @brief 进行MISRA-C检查的Makefile
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

include $(XWOS_WKSPC_DIR)/XWOS.cfg
include $(XWOS_ARCH_DIR)/arch.mk
include $(XWOS_CPU_DIR)/cpu.mk
include $(XWOS_SOC_DIR)/soc.mk
include $(XWOS_BRD_DIR)/brd.mk
include xwbs/misra.c.rule

CPPCHECK_C_ARGS = $(strip $(CPPCHECK_INCDIRS) $(CPPCHECK_CFLAGS))

%.mc: $(CPPCHECK_OUT) FORCE
	$(SHOW_CPPCHECK) $(CPPCHECK) $(CPPCHECK_C_ARGS) $*

$(CPPCHECK_OUT):
	@mkdir -p $@

FORCE:
