#! /bin/make -f
# @file
# @brief Top makefile of board/project
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

WKSPC ?= wkspc
TARGET ?= XuanWuOS

all: $(WKSPC) cfg
	$(MAKE) -C $(WKSPC) TARGET=$(TARGET)
	@cp $(WKSPC)/$(TARGET).elf $(WKSPC)/$(TARGET).axf

$(WKSPC):
	@mkdir -p $@

cfg: $(WKSPC) $(WKSPC)/XuanWuOS.cfg

$(WKSPC)/XuanWuOS.cfg: cfg/XuanWuOS.h cfg/arch.h cfg/cpu.h cfg/soc.h \
 cfg/perpheral.h cfg/board.h cfg/xwos.h cfg/xwmd.h cfg/xwem.h
	@../../xwbs/util/el/mkcfg.el --cfgdir=cfg --topdir=../.. --wkspc=$(WKSPC) -- $(@F)

%.gdb: %.gdb.template
	@sed "s/##DIR##/$(subst /,\\/,$(shell pwd)/$(WKSPC))/g" $< > $@
	@chmod +x $@

c clean: cfg
	$(MAKE) -C $(WKSPC) TARGET=$(TARGET) NODEP=y clean

d distclean: cfg
	@rm -rf $(WKSPC)

.DEFAULT:
	@$(MAKE) cfg
	$(MAKE) -C $(WKSPC) TARGET=$(TARGET) $@

.PHONY : all cfg c clean d distclean
