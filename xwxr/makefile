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

XWOS ?= ..
WKSPC ?= wkspc
TARGET ?= XWOS

MKCFG := ${XWOS}/xwbs/util/el/mkcfg.el

all: $(WKSPC) cfg html rustdoc luadoc

html: cfg
	cd .. && doxygen -d Preprocessor xwxr/doxyfile

rustdoc: cfg
	cd fakeboard/rustapp; cargo +nightly clean --target=thumbv7em-none-eabihf
	cd fakeboard/rustapp; cargo +nightly doc --target=thumbv7em-none-eabihf
	rm -rf wkspc/rustdoc
	cp -ap fakeboard/rustapp/target/thumbv7em-none-eabihf/doc wkspc/rustdoc

luadoc: cfg
	cd fakeboard/xwlua; ldoc .

$(WKSPC):
	@mkdir -p $@

cfg: $(WKSPC) $(WKSPC)/XuanWuOS.cfg

$(WKSPC)/XuanWuOS.cfg: fakeboard/cfg/*.h
	@[ -x ${MKCFG} ] || chmod +x ${MKCFG}
	@${MKCFG} --XWOS=$(XWOS) --cfgdir=$(<D) --wkspc=$(WKSPC) -- $(@F)

c clean : cfg
	$(MAKE) -C $(WKSPC) TARGET=$(TARGET) NODEP=y clean

d distclean: cfg
	$(MAKE) -C $(WKSPC) TARGET=$(TARGET) NODEP=y distclean
	@rm -rf $(WKSPC)

.PHONY : all cfg c clean d distclean
