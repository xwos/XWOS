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

XuanWuOS ?= ../..
WKSPC ?= wkspc
TARGET ?= XuanWuOS

MKCFG := ${XuanWuOS}/xwbs/util/el/mkcfg.el

all: $(WKSPC) cfg
	$(MAKE) -C $(WKSPC) TARGET=$(TARGET)

$(WKSPC):
	@mkdir -p $@

cfg: $(WKSPC) $(WKSPC)/XuanWuOS.cfg

$(WKSPC)/XuanWuOS.cfg: cfg/*.h
	@[ -x ${MKCFG} ] || chmod +x ${MKCFG}
	@${MKCFG} --XuanWuOS=$(XuanWuOS) --cfgdir=cfg --wkspc=$(WKSPC) -- $(@F)

fmw: all
	@dd if=$(WKSPC)/$(TARGET).bin of=$(WKSPC)/$(TARGET).fmw bs=1 skip=65536

pegdbserver:
	pegdbserver_power_console \
		-device=MPC5607B -startserver -singlesession \
		-serverport=7224 -gdbmiport=6224 -interface=OPENSDA -speed=5000 -port=USB1 \
		-configfile=cfg/dbg/config.ini

c clean: cfg
	$(MAKE) -C $(WKSPC) TARGET=$(TARGET) NODEP=y clean

d distclean: cfg
	@rm -rf $(WKSPC)
	@rm -rf cfg/dbg/*.gdb

.DEFAULT:
	@$(MAKE) cfg
	$(MAKE) -C $(WKSPC) TARGET=$(TARGET) $@

.PHONY : all cfg fmw c clean d distclean
