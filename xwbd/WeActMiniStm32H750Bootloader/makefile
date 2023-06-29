#! /bin/make -f
# @file
# @brief Top makefile of board/project
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

XWOS ?= ../..
WKSPC ?= wkspc
MKCFG := $(XWOS)/xwbs/util/el/mkcfg.el

all: $(WKSPC) cfg
	$(MAKE) -C $(WKSPC)

$(WKSPC):
	@mkdir -p $@
	@cp $(XWOS)/xwbs/wkspc.mk $@/makefile

cfg: $(WKSPC) $(WKSPC)/XWOS.cfg

$(WKSPC)/XWOS.cfg: $(filter-out cfg/autogen.h,$(wildcard cfg/*.h))
	@[ -x ${MKCFG} ] || chmod +x ${MKCFG}
	@${MKCFG} --XWOS=$(XWOS) --cfgdir=cfg --wkspc=$(WKSPC)

c clean: cfg
	$(MAKE) -C $(WKSPC) NODEP=y clean

d distclean: cfg
	$(MAKE) -C $(WKSPC) NODEP=y distclean
	@rm -rf $(WKSPC)

FORCE:

.DEFAULT:
	@$(MAKE) cfg
	$(MAKE) -C $(WKSPC) $@

.PHONY : all cfg c clean d distclean
