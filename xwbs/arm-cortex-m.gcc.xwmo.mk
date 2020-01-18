#! /bin/make -f
# @file
# @brief 编译模块并生成.a的Makefile模板 (ARM-cortex-m-gcc)
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

include xwbs/$(XuanWuOS_CFG_MK_RULE)

XWMO_NAME := $(call getXwmoName)
XWMO_DIR := $(call getXwmoDir)
XWMO_CSRCS := $(addprefix $(XWMO_DIR)/,$(XWMO_CSRCS))
XWMO_CXXSRCS := $(addprefix $(XWMO_DIR)/,$(XWMO_CXXSRCS))
XWMO_COBJS := $(addprefix $(OBJ_DIR),$(addsuffix .o,$(basename $(XWMO_CSRCS))))
XWMO_CXXOBJS := $(addprefix $(OBJ_DIR),$(addsuffix .o,$(basename $(XWMO_CXXSRCS))))
XWMO_DSMS := $(addprefix $(OBJ_DIR),$(addsuffix .dsm,$(basename $(XWMO_CSRCS))))
XWMO_DSMS += $(addprefix $(OBJ_DIR),$(addsuffix .dsm,$(basename $(XWMO_CXXSRCS))))
XWMO_INCDIRS := $(if $(strip $(XWMO_INCDIRS)),$(addprefix -I,$(strip $(XWMO_INCDIRS))))

XWMO_LUASRCS := $(addprefix $(XWMO_DIR)/,$(XWMO_LUASRCS))
XWMO_LUA2HEXCSRCS := $(addprefix $(OBJ_DIR),$(addsuffix .c,$(XWMO_LUASRCS)))
XWMO_LUA2HEXCOBJS := $(addprefix $(OBJ_DIR),$(addsuffix .o,$(XWMO_LUASRCS)))

MM_ARGS = $(strip $(MMFLAGS))

CC_ARGS = $(strip -c $(CFLAGS) $(ARCH_CFLAGS) $(CPU_CFLAGS) $(XWMO_CFLAGS) \
                  $(INCDIRS) $(XWMO_INCDIRS))

CXX_ARGS = $(strip -c $(CXXFLAGS) $(ARCH_CXXFLAGS) $(CPU_CXXFLAGS) $(XWMO_CXXFLAGS) \
                  $(INCDIRS) $(XWMO_INCDIRS))

#$(info "building $(XWMO_DIR) ---> $(OBJ_DIR)$(XWMO_DIR)/$(XWMO_NAME)")

$(OBJ_DIR)$(XWMO_DIR)/$(XWMO_NAME): $(XWMO_COBJS) $(XWMO_LUA2HEXCOBJS) $(XWMO_CXXOBJS) $(OBJ_DIR)$(XWMO_DIR)
	@[ ! -d $(@D) ] && mkdir -p $(@D) || true
	$(SHOW_AR) $(AR) rcs $@ $(XWMO_COBJS) $(XWMO_LUA2HEXCOBJS) $(XWMO_CXXOBJS)

$(OBJ_DIR)$(XWMO_DIR):
	@[ ! -d $@ ] && mkdir -p $@ || true

$(XWMO_LUA2HEXCOBJS): $(XWMO_LUA2HEXCSRCS)

ifneq ($(XWMO_COBJS),)
    ifneq ($(NODEP),y)
        -include $(XWMO_COBJS:.o=.o.d)
    endif
endif

ifneq ($(XWMO_CXXOBJS),)
    ifneq ($(NODEP),y)
        -include $(XWMO_CXXOBJS:.o=.o.d)
    endif
endif

$(OBJ_DIR)%.o.d: %.c
	@[ ! -d $(@D) ] && mkdir -p $(@D) || true
	$(SHOW_MM) $(CC) $(MM_ARGS) $(CC_ARGS) $< > $@;
	@sed -i 's,\(^.*\)\.o[ :]*,$*.o $@ : ,g' $@

$(OBJ_DIR)%.lua.c: %.lua
	@[ ! -d $(@D) ] && mkdir -p $(@D) || true
	$(SHOW_LUA2HEX) $(LUA2HEX) -o $@ $<

$(OBJ_DIR)%.lua.o: $(OBJ_DIR)%.lua.c
	@[ ! -d $(@D) ] && mkdir -p $(@D) || true
	$(SHOW_CC) $(CC) $(CC_ARGS) $< -o $@

$(OBJ_DIR)%.o: %.c
	@[ ! -d $(@D) ] && mkdir -p $(@D) || true
	$(SHOW_CC) $(CC) $(CC_ARGS) $< -o $@

$(OBJ_DIR)%.o.d: %.cpp
	@[ ! -d $(@D) ] && mkdir -p $(@D) || true
	$(SHOW_MM) $(CXX) $(MM_ARGS) $(CXX_ARGS) $< > $@;
	@sed -i 's,\(^.*\)\.o[ :]*,$*.o $@ : ,g' $@

$(OBJ_DIR)%.o: %.cpp
	@[ ! -d $(@D) ] && mkdir -p $(@D) || true
	$(SHOW_CXX) $(CXX) $(CXX_ARGS) $< -o $@

$(OBJ_DIR)%.o.d: %.cxx
	@[ ! -d $(@D) ] && mkdir -p $(@D) || true
	$(SHOW_MM) $(CXX) $(MM_ARGS) $(CXX_ARGS) $< > $@;
	@sed -i 's,\(^.*\)\.o[ :]*,$*.o $@ : ,g' $@

$(OBJ_DIR)%.o: %.cxx
	@[ ! -d $(@D) ] && mkdir -p $(@D) || true
	$(SHOW_CXX) $(CXX) $(CXX_ARGS) $< -o $@

%.dsm: %.o
	$(SHOW_OD) $(OD) -D $< > $@

dsm: $(XWMO_DSMS)

clean:
ifeq ($(NEAR),y)
	$(SHOW_RM)-rm -f $(XWMO_COBJS:.o=.o.lst)
	$(SHOW_RM)-rm -f $(XWMO_COBJS:.o=.dsm)
	$(SHOW_RM)-rm -f $(XWMO_COBJS:.o=.i)
	$(SHOW_RM)-rm -f $(XWMO_COBJS)
	$(SHOW_RM)-rm -f $(XWMO_LUA2HEXCOBJS:.o=.o.lst)
	$(SHOW_RM)-rm -f $(XWMO_LUA2HEXCOBJS:.o=.dsm)
	$(SHOW_RM)-rm -f $(XWMO_LUA2HEXCOBJS:.o=.i)
	$(SHOW_RM)-rm -f $(XWMO_LUA2HEXCOBJS)
	$(SHOW_RM)-rm -f $(XWMO_CXXOBJS:.o=.o.lst)
	$(SHOW_RM)-rm -f $(XWMO_CXXOBJS:.o=.dsm)
	$(SHOW_RM)-rm -f $(XWMO_CXXOBJS:.o=.i)
	$(SHOW_RM)-rm -f $(XWMO_CXXOBJS)
else
	$(SHOW_RM)-[ -d "$(OBJ_DIR)/$(XWMO_DIR)" ] && \
                   find $(OBJ_DIR)/$(XWMO_DIR) -type f -name "*.o" -exec rm -f {} \; || true
	$(SHOW_RM)-[ -d "$(OBJ_DIR)/$(XWMO_DIR)" ] && \
                   find $(OBJ_DIR)/$(XWMO_DIR) -type f -name "*.o.lst" -exec rm -f {} \; || true
	$(SHOW_RM)-[ -d "$(OBJ_DIR)/$(XWMO_DIR)" ] && \
                   find $(OBJ_DIR)/$(XWMO_DIR) -type f -name "*.dsm" -exec rm -f {} \; || true
	$(SHOW_RM)-[ -d "$(OBJ_DIR)/$(XWMO_DIR)" ] && \
                   find $(OBJ_DIR)/$(XWMO_DIR) -type f -name "*.i" -exec rm -f {} \; || true
endif
	$(SHOW_RM)-rm -f $(OBJ_DIR)$(XWMO_DIR)/$(XWMO_NAME)

distclean:
ifeq ($(NEAR),y)
	$(SHOW_RM)-rm -f $(XWMO_COBJS:.o=.o.lst)
	$(SHOW_RM)-rm -f $(XWMO_COBJS:.o=.o.d)
	$(SHOW_RM)-rm -f $(XWMO_COBJS:.o=.dsm)
	$(SHOW_RM)-rm -f $(XWMO_COBJS:.o=.i)
	$(SHOW_RM)-rm -f $(XWMO_COBJS)
	$(SHOW_RM)-rm -f $(XWMO_CXXOBJS:.o=.o.lst)
	$(SHOW_RM)-rm -f $(XWMO_CXXOBJS:.o=.o.d)
	$(SHOW_RM)-rm -f $(XWMO_CXXOBJS:.o=.dsm)
	$(SHOW_RM)-rm -f $(XWMO_CXXOBJS:.o=.i)
	$(SHOW_RM)-rm -f $(XWMO_CXXOBJS)
else
	$(SHOW_RM)-[ -d "$(OBJ_DIR)/$(XWMO_DIR)" ] && \
                   find $(OBJ_DIR)/$(XWMO_DIR) -type f -name "*.o" -exec rm -f {} \; || true
	$(SHOW_RM)-[ -d "$(OBJ_DIR)/$(XWMO_DIR)" ] && \
                   find $(OBJ_DIR)/$(XWMO_DIR) -type f -name "*.o.d" -exec rm -f {} \; || true
	$(SHOW_RM)-[ -d "$(OBJ_DIR)/$(XWMO_DIR)" ] && \
                   find $(OBJ_DIR)/$(XWMO_DIR) -type f -name "*.o.lst" -exec rm -f {} \; || true
	$(SHOW_RM)-[ -d "$(OBJ_DIR)/$(XWMO_DIR)" ] && \
                   find $(OBJ_DIR)/$(XWMO_DIR) -type f -name "*.dsm" -exec rm -f {} \; || true
	$(SHOW_RM)-[ -d "$(OBJ_DIR)/$(XWMO_DIR)" ] && \
                   find $(OBJ_DIR)/$(XWMO_DIR) -type f -name "*.i" -exec rm -f {} \; || true
endif
	$(SHOW_RM)-rm -f $(OBJ_DIR)$(XWMO_DIR)/$(XWMO_NAME)

.PHONY: dsm clean distclean
