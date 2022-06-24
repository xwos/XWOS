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

include $(XuanWuOS_ARCH_DIR)/arch.mk
include $(XuanWuOS_CPU_DIR)/cpu.mk
include $(XuanWuOS_SOC_DIR)/soc.mk
include $(XuanWuOS_BRD_DIR)/brd.mk
include $(XuanWuOS_XWOS_DIR)/xwos.mk
include $(XuanWuOS_BRD_DIR)/lib.mk
include $(XWBS_UTIL_MK_XWMO)
include xwbs/$(XuanWuOS_CFG_ARCH).$(XuanWuOS_CFG_COMPILER).rule

XWMO_NAME := $(call getXwmoName)
XWMO_DIR := $(call getXwmoDir)
XWMO_OBJ_DIR ?= $(XWMO_DIR)
XWMO_RUSTLIB_NAME := lib$(notdir $(XWMO_DIR)).a

ifeq ($(XuanWuOS_CFG_HOSTOS),windows-nt)
  RUST_TOOLCHAIN := +nightly-x86_64-pc-windows-gnu
else
  RUST_TOOLCHAIN := +nightly
endif

ifeq ($(~D),0)
  CARGO_BUILD_FLAGS += --release
  XWMO_RUSTLIB := $(XWMO_DIR)/target/$(RUST_TARGET)/release/$(XWMO_RUSTLIB_NAME)
else
  XWMO_RUSTLIB := $(XWMO_DIR)/target/$(RUST_TARGET)/debug/$(XWMO_RUSTLIB_NAME)
endif

export XuanWuOS_PATH
export XuanWuOS_WKSPC_DIR

$(XuanWuOS_OBJ_DIR)/$(XWMO_OBJ_DIR)/$(XWMO_NAME): $(XWMO_RUSTLIB) $(XuanWuOS_OBJ_DIR)/$(XWMO_OBJ_DIR)
	$(SHOW_CP) $(CP) $< $@

$(XuanWuOS_OBJ_DIR)/$(XWMO_OBJ_DIR):
	@[ ! -d $@ ] && mkdir -p $@ || true

$(XWMO_RUSTLIB):
	cd $(XWMO_DIR); cargo $(RUST_TOOLCHAIN) build $(CARGO_BUILD_FLAGS) --target=$(RUST_TARGET)

doc:
	cd $(XWMO_DIR); cargo $(RUST_TOOLCHAIN) doc $(CARGO_BUILD_FLAGS) --target=$(RUST_TARGET)

rustdoc:
	cd $(XWMO_DIR); cargo $(RUST_TOOLCHAIN) rustdoc $(CARGO_BUILD_FLAGS) --target=$(RUST_TARGET)

clean:
	@$(RM) -f $(XuanWuOS_OBJ_DIR)/$(XWMO_OBJ_DIR)/$(XWMO_NAME)
	@cd $(XWMO_DIR); cargo $(RUST_TOOLCHAIN) clean

distclean:
	$(RM) -rf $(XuanWuOS_OBJ_DIR)/$(XWMO_OBJ_DIR)
	@cd $(XWMO_DIR); cargo $(RUST_TOOLCHAIN) clean; rm -rf $(XWMO_DIR)/target

.PHONY: dsm clean distclean $(XWMO_RUSTLIB)
