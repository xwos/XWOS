#! /bin/make -f
# @file
# @brief Top makefile of board/project
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

XWOS ?= ../..
WKSPC ?= WKSPC
MKCFG := $(XWOS)/xwbs/util/el/mkcfg.el

all: $(WKSPC) cfg script
	$(MAKE) -C $(WKSPC)

$(WKSPC):
	@mkdir -p $@
	@cp $(XWOS)/xwbs/wkspc.mk $@/makefile

cfg: $(WKSPC) $(WKSPC)/XWOS.cfg

$(WKSPC)/XWOS.cfg: $(filter-out cfg/autogen.h,$(wildcard cfg/*.h))
	@[ -x ${MKCFG} ] || chmod +x ${MKCFG}
	@${MKCFG} --XWOS=$(XWOS) --cfgdir=cfg --wkspc=$(WKSPC)

script: $(WKSPC) $(WKSPC)/core0.gdb $(WKSPC)/core1.gdb $(WKSPC)/core2.gdb $(WKSPC)/core3.gdb

$(WKSPC)/%.gdb: debug/gdb/%.gdb.in
	@sed "s/\$${DIR}/$(subst /,\\/,$(shell pwd)/$(WKSPC))/g" $< > $@
	@chmod +x $@

jlink:
	@openocd -f debug/openocd/jlink.cfg

daplink:
	@openocd -f debug/openocd/daplink.cfg

core0.dbg: $(WKSPC)/core0.gdb
	@$(shell xwtcwd)/arm/bin/aarch64-none-elf-gdb -x $(WKSPC)/core0.gdb

core1.dbg: $(WKSPC)/core1.gdb
	@$(shell xwtcwd)/arm/bin/aarch64-none-elf-gdb -x $(WKSPC)/core1.gdb

core2.dbg: $(WKSPC)/core2.gdb
	@$(shell xwtcwd)/arm/bin/aarch64-none-elf-gdb -x $(WKSPC)/core2.gdb

core3.dbg: $(WKSPC)/core3.gdb
	@$(shell xwtcwd)/arm/bin/aarch64-none-elf-gdb -x $(WKSPC)/core3.gdb

c clean: cfg
	$(MAKE) -C $(WKSPC) NODEP=y clean

d distclean: cfg
	$(MAKE) -C $(WKSPC) NODEP=y distclean
	@rm -rf $(WKSPC)
	@rm -rf $(WKSPC)/*.gdb

FORCE:

.DEFAULT:
	@$(MAKE) cfg
	$(MAKE) -C $(WKSPC) $@

.PHONY : all cfg c clean d distclean script jlink daplink core0.dbg core1.dbg core2.dbg core3.dbg
