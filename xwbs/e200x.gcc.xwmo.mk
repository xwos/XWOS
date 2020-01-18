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
XWMO_COBJS := $(addprefix $(OBJ_DIR),$(addsuffix .o,$(basename $(XWMO_CSRCS))))
XWMO_DSMS := $(addprefix $(OBJ_DIR),$(addsuffix .dsm,$(basename $(XWMO_CSRCS))))
XWMO_INCDIRS := $(if $(strip $(XWMO_INCDIRS)),$(addprefix -I,$(strip $(XWMO_INCDIRS))))

CC_ARGS = $(strip -c $(CFLAGS) $(ARCH_CFLAGS) $(CPU_CFLAGS) $(XWMO_CFLAGS) \
                  $(INCDIRS) $(XWMO_INCDIRS))

$(OBJ_DIR)$(XWMO_DIR)/$(XWMO_NAME): $(XWMO_COBJS)
	$(SHOW_AR) $(AR) rcs $@ $(XWMO_COBJS)

ifneq ($(XWMO_COBJS),)
    ifneq ($(NODEP),y)
        -include $(XWMO_COBJS:.o=.o.d)
    endif
endif

$(OBJ_DIR)%.o.d: %.c
	@[ ! -d $(@D) ] && mkdir -p $(@D) || true
	$(SHOW_MM) $(MM) $(CC_ARGS) $< > $@;
	@sed -i 's,\(^.*\)\.o[ :]*,$*.o $@ : ,g' $@

$(OBJ_DIR)%.o: %.c
	@[ ! -d $(@D) ] && mkdir -p $(@D) || true
	$(SHOW_CC) $(CC) $(CC_ARGS) $< -o $@

%.dsm: %.o
	$(SHOW_OD) $(OD) -D $< > $@

dsm: $(XWMO_DSMS)

clean:
	$(SHOW_RM)-rm -f $(XWMO_COBJS:.o=.o.lst)
	$(SHOW_RM)-rm -f $(XWMO_COBJS:.o=.dsm)
	$(SHOW_RM)-rm -f $(XWMO_COBJS)
	$(SHOW_RM)-rm -f $(OBJ_DIR)$(XWMO_DIR)/$(XWMO_NAME)

distclean:
	$(SHOW_RM)-rm -f $(XWMO_COBJS:.o=.o.lst)
	$(SHOW_RM)-rm -f $(XWMO_COBJS:.o=.o.d)
	$(SHOW_RM)-rm -f $(XWMO_COBJS:.o=.dsm)
	$(SHOW_RM)-rm -f $(XWMO_COBJS)
	$(SHOW_RM)-rm -f $(OBJ_DIR)$(XWMO_DIR)/$(XWMO_NAME)

.PHONY: dsm clean distclean
