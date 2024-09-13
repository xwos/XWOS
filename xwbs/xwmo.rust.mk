#! /bin/make -f
# @file
# @brief 编译RUST玄武模块的Makefile
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

include $(XWOS_ARCH_DIR)/arch.mk
include $(XWOS_CPU_DIR)/cpu.mk
include $(XWOS_SOC_DIR)/soc.mk
include $(XWOS_BRD_DIR)/brd.mk
include $(XWOS_OS_DIR)/xwos.mk
include $(XWOS_BRD_DIR)/lib.mk
include xwbs/functions.mk
include xwbs/$(XWCFG_ARCH).$(XWCFG_COMPILER).rule

XWMO_NAME := $(call getXwmoName)
XWMO_DIR := $(call getXwmoDir)
XWMO_OBJ_DIR ?= $(XWMO_DIR)
XWMO_RUSTLIB_NAME := lib$(notdir $(XWMO_DIR)).a

ifeq ($(XWCFG_HOSTOS),windows-nt)
  RUST_TOOLCHAIN := +nightly-x86_64-pc-windows-gnu
else
  RUST_TOOLCHAIN := +nightly
endif

ifeq ($(D),0)
  CARGO_BUILD_FLAGS += --release
  XWMO_RUSTLIB := $(XWMO_DIR)/target/$(RUST_TARGET)/release/$(XWMO_RUSTLIB_NAME)
else
  XWMO_RUSTLIB := $(XWMO_DIR)/target/$(RUST_TARGET)/debug/$(XWMO_RUSTLIB_NAME)
endif

export XWOS_PATH
export XWOS_WKSPC_DIR

$(XWOS_OBJ_DIR)/$(XWMO_OBJ_DIR)/$(XWMO_NAME): $(XWMO_RUSTLIB) $(XWOS_OBJ_DIR)/$(XWMO_OBJ_DIR)
	$(SHOW_CP) $(CP) $< $@

$(XWOS_OBJ_DIR)/$(XWMO_OBJ_DIR):
	@[ ! -d $@ ] && mkdir -p $@ || true

$(XWMO_RUSTLIB):
	cd $(XWMO_DIR); cargo $(RUST_TOOLCHAIN) build --target=$(XWOS_PATH)/$(XWOS_CPU_DIR)/$(RUST_TARGET).json $(CARGO_BUILD_FLAGS)

doc:
	cd $(XWMO_DIR); cargo $(RUST_TOOLCHAIN) doc --target=$(XWOS_PATH)/$(XWOS_CPU_DIR)/$(RUST_TARGET).json $(CARGO_BUILD_FLAGS)

rustdoc:
	cd $(XWMO_DIR); cargo $(RUST_TOOLCHAIN) rustdoc --target=$(XWOS_PATH)/$(XWOS_CPU_DIR)/$(RUST_TARGET).json $(CARGO_BUILD_FLAGS)

clean:
	@$(RM) -f $(XWOS_OBJ_DIR)/$(XWMO_OBJ_DIR)/$(XWMO_NAME)
	@cd $(XWMO_DIR); cargo $(RUST_TOOLCHAIN) clean

distclean:
	$(RM) -rf $(XWOS_OBJ_DIR)/$(XWMO_OBJ_DIR)
	@cd $(XWMO_DIR); cargo $(RUST_TOOLCHAIN) clean; rm -rf $(XWMO_DIR)/target

.PHONY: dsm clean distclean $(XWMO_RUSTLIB)
