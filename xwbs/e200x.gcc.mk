#! /bin/make -f
# @file
# @brief Makefile模板 (powerpc-eabivle-gcc)
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

include $(XWOS_WKSPC_DIR)/XuanWuOS.cfg
include xwbs/$(XuanWuOS_CFG_MK_RULE)
include $(XWOS_BRD_DIR)/cfg/elibs.list

ARCH_CSRCS := $(addprefix $(XWOS_ARCH_DIR)/,$(ARCH_CSRCS))
ARCH_ASRCS := $(addprefix $(XWOS_ARCH_DIR)/,$(ARCH_ASRCS))
ARCH_COBJS := $(addprefix $(OBJ_DIR),$(addsuffix .o,$(basename $(ARCH_CSRCS))))
ARCH_AOBJS := $(addprefix $(OBJ_DIR),$(addsuffix .o,$(basename $(ARCH_ASRCS))))
ARCH_OBJS := $(ARCH_COBJS) $(ARCH_AOBJS)
ARCH_LIB := $(OBJ_DIR)$(XWOS_ARCH_DIR)/arch.a

CPU_CSRCS := $(addprefix $(XWOS_CPU_DIR)/,$(CPU_CSRCS))
CPU_ASRCS := $(addprefix $(XWOS_CPU_DIR)/,$(CPU_ASRCS))
CPU_COBJS := $(addprefix $(OBJ_DIR),$(addsuffix .o,$(basename $(CPU_CSRCS))))
CPU_AOBJS := $(addprefix $(OBJ_DIR),$(addsuffix .o,$(basename $(CPU_ASRCS))))
CPU_OBJS := $(CPU_COBJS) $(CPU_AOBJS)
CPU_LIB := $(OBJ_DIR)$(XWOS_CPU_DIR)/cpu.a

SOC_CSRCS := $(addprefix $(XWOS_SOC_DIR)/,$(SOC_CSRCS))
SOC_ASRCS := $(addprefix $(XWOS_SOC_DIR)/,$(SOC_ASRCS))
SOC_COBJS := $(addprefix $(OBJ_DIR),$(addsuffix .o,$(basename $(SOC_CSRCS))))
SOC_AOBJS := $(addprefix $(OBJ_DIR),$(addsuffix .o,$(basename $(SOC_ASRCS))))
SOC_OBJS := $(SOC_COBJS) $(SOC_AOBJS)
SOC_LIB := $(OBJ_DIR)$(XWOS_SOC_DIR)/soc.a

BDL_CSRCS := $(addprefix $(XWOS_BDL_DIR)/,$(BDL_CSRCS))
BDL_ASRCS := $(addprefix $(XWOS_BDL_DIR)/,$(BDL_ASRCS))
BDL_COBJS := $(addprefix $(OBJ_DIR),$(addsuffix .o,$(basename $(BDL_CSRCS))))
BDL_AOBJS := $(addprefix $(OBJ_DIR),$(addsuffix .o,$(basename $(BDL_ASRCS))))
BDL_OBJS := $(BDL_COBJS) $(BDL_AOBJS)
BDL_LIB := $(OBJ_DIR)$(XWOS_BDL_DIR)/bdl.a

XWOS_CSRCS := $(addprefix $(XWOS_KN_DIR)/,$(XWOS_CSRCS))
XWOS_ASRCS := $(addprefix $(XWOS_KN_DIR)/,$(XWOS_ASRCS))
XWOS_COBJS := $(addprefix $(OBJ_DIR),$(addsuffix .o,$(basename $(XWOS_CSRCS))))
XWOS_AOBJS := $(addprefix $(OBJ_DIR),$(addsuffix .o,$(basename $(XWOS_ASRCS))))
XWOS_OBJS := $(XWOS_COBJS) $(XWOS_AOBJS)
XWOS_LIB := $(OBJ_DIR)$(XWOS_KN_DIR)/xwos.a

LIBS :=
LIBS += $(XWOS_LIB)
LIBS += $(BDL_LIB)
LIBS += $(SOC_LIB)
LIBS += $(CPU_LIB)
LIBS += $(ARCH_LIB)

LIB_OBJS :=
LIB_OBJS += $(XWOS_OBJS)
LIB_OBJS += $(BDL_OBJS)
LIB_OBJS += $(SOC_OBJS)
LIB_OBJS += $(CPU_OBJS)
LIB_OBJS += $(ARCH_OBJS)

INCDIRS += $(if $(strip $(EINCDIRS)),$(addprefix -I,$(strip $(EINCDIRS))))

DSMS := $(addsuffix .dsm,$(basename $(OBJS)))
DSMS += $(addsuffix .dsm,$(basename $(LIB_OBJS)))

AS_ARGS = $(strip $(INCDIRS) $(AFLAGS) $(ARCH_AFLAGS) $(CPU_AFLAGS) $(BDL_AFLAGS))

CC_ARGS = $(strip $(INCDIRS) $(CFLAGS) $(ARCH_CFLAGS) $(CPU_CFLAGS) $(BDL_CFLAGS))

LD_ARGS = $(strip -Wl,--start-group \
                  $(XWEM) $(XWBM) $(XWPP) $(XWMD) \
                  $(XWOS_LIB) $(XWOS_EOBJS) \
                  $(BDL_LIB) $(BDL_EOBJS) \
                  $(SOC_LIB) $(SOC_EOBJS) \
                  $(CPU_LIB) $(CPU_EOBJS) \
                  $(ARCH_LIB) $(ARCH_EOBJS) \
                  $(ELIBS) \
                  -Wl,--end-group \
                  $(LDFLAGS) $(ARCH_LDFLAGS) $(CPU_LDFLAGS) $(BDL_LDFLAGS))

all: $(LIB_OBJS) $(LIBS) $(XWMD) $(XWPP) $(XWBM) $(XWEM) $(XWOS_WKSPC_DIR)/$(TARGET).elf \
     $(XWOS_WKSPC_DIR)/$(TARGET).hex $(XWOS_WKSPC_DIR)/$(TARGET).bin

ifeq ($(XuanWuOS_CFG_XWMD),y)
    include xwbs/xwmd.mk
endif

include xwbs/xwpp.mk

include xwbs/xwbm.mk

ifeq ($(XuanWuOS_CFG_XWEM),y)
    include xwbs/xwem.mk
endif

ifneq ($(NODEP),y)
    -include $(LIB_OBJS:.o=.o.d)
endif

$(ARCH_LIB): $(ARCH_OBJS)
	$(SHOW_AR) $(AR) rcs $@ $(ARCH_OBJS)

$(CPU_LIB): $(CPU_OBJS)
	$(SHOW_AR) $(AR) rcs $@ $(CPU_OBJS)

$(SOC_LIB): $(SOC_OBJS)
	$(SHOW_AR) $(AR) rcs $@ $(SOC_OBJS)

$(BDL_LIB): $(BDL_OBJS)
	$(SHOW_AR) $(AR) rcs $@ $(BDL_OBJS)

$(XWOS_LIB): $(XWOS_OBJS)
	$(SHOW_AR) $(AR) rcs $@ $(XWOS_OBJS)

dsm: $(DSMS) $(XWMD_DSM) $(XWPP_DSM) $(XWBM_DSM) $(XWEM_DSM) $(XWOS_BRD_DIR)/$(TARGET).dsm

$(OBJ_DIR)%.s: %.S
	@[ ! -d $(@D) ] && mkdir -p $(@D) || true
	$(SHOW_AS) $(AS) -E $(AS_ARGS) $< -o $@

$(OBJ_DIR)%.o.d: %.S
	@[ ! -d $(@D) ] && mkdir -p $(@D) || true
	$(SHOW_MM) $(MM) $(AS_ARGS) $< > $@;
	@sed -i 's,\(^.*\)\.o[ :]*,$*.o $@ : ,g' $@

$(OBJ_DIR)%.o: %.S
	@[ ! -d $(@D) ] && mkdir -p $(@D) || true
	$(SHOW_AS) $(AS) -c $(AS_ARGS) $< -o $@

$(OBJ_DIR)%.i: %.c
	@[ ! -d $(@D) ] && mkdir -p $(@D) || true
	$(SHOW_CC) $(CC) -E $(CC_ARGS) $< -o $@

$(OBJ_DIR)%.o.d: %.c
	@[ ! -d $(@D) ] && mkdir -p $(@D) || true
	$(SHOW_MM) $(MM) $(CC_ARGS) $< > $@;
	@sed -i 's,\(^.*\)\.o[ :]*,$*.o $@ : ,g' $@

$(OBJ_DIR)%.o: %.c
	@[ ! -d $(@D) ] && mkdir -p $(@D) || true
	$(SHOW_CC) $(CC) -c $(CC_ARGS) $< -o $@

$(XWOS_WKSPC_DIR)/$(TARGET).elf: $(LIB_OBJS) $(LIBS) $(XWMD) $(XWPP) $(XWBM) $(XWEM)
	$(SHOW_LD) $(LD) $(LD_ARGS) -o $@
	$(SHOW_SIZE) $(SIZE) $@

%.hex: %.elf
	$(SHOW_CP) $(CP) -O ihex $< $@

%.bin: %.elf
	$(SHOW_CP) $(CP) -O binary -S $< $@

$(XWOS_BRD_DIR)/$(TARGET).dsm: $(XWOS_BRD_DIR)/$(TARGET).elf
	$(SHOW_OD) $(OD) -D $< > $@

%.dsm: %.o
	$(SHOW_OD) $(OD) -D $< > $@

clean: $(XWMD_CLEAN) $(XWPP_CLEAN) $(XWBM_CLEAN) $(XWEM_CLEAN)
	@echo "clean ..."
	$(SHOW_RM)-rm -f $(LIB_OBJS)
	$(SHOW_RM)-rm -f $(LIB_OBJS:.o=.o.lst)
	$(SHOW_RM)-rm -f $(LIB_OBJS:.o=.dsm)
	$(SHOW_RM)-rm -f $(LIB_OBJS:.o=.i)
	$(SHOW_RM)-rm -f $(LIBS)
	$(SHOW_RM)-rm -f $(XWOS_WKSPC_DIR)/XuanWuOS.cfg
	$(SHOW_RM)-rm -f $(XWOS_WKSPC_DIR)/$(TARGET).elf
	$(SHOW_RM)-rm -f $(XWOS_WKSPC_DIR)/$(TARGET).dsm
	$(SHOW_RM)-rm -f $(XWOS_WKSPC_DIR)/$(TARGET).map
	$(SHOW_RM)-rm -f $(XWOS_WKSPC_DIR)/$(TARGET).hex
	$(SHOW_RM)-rm -f $(XWOS_WKSPC_DIR)/$(TARGET).bin

distclean: $(XWMD_DISTCLEAN) $(XWPP_DISTCLEAN) $(XWBM_DISTCLEAN) $(XWEM_DISTCLEAN)
	@echo "distclean ..."
	$(SHOW_RM)-rm -f $(LIB_OBJS)
	$(SHOW_RM)-rm -f $(LIB_OBJS:.o=.o.lst)
	$(SHOW_RM)-rm -f $(LIB_OBJS:.o=.o.d)
	$(SHOW_RM)-rm -f $(LIB_OBJS:.o=.dsm)
	$(SHOW_RM)-rm -f $(LIB_OBJS:.o=.i)
	$(SHOW_RM)-rm -f $(LIBS)
	$(SHOW_RM)-rm -f $(XWOS_WKSPC_DIR)/XuanWuOS.cfg
	$(SHOW_RM)-rm -f $(XWOS_WKSPC_DIR)/$(TARGET).elf
	$(SHOW_RM)-rm -f $(XWOS_WKSPC_DIR)/$(TARGET).dsm
	$(SHOW_RM)-rm -f $(XWOS_WKSPC_DIR)/$(TARGET).map
	$(SHOW_RM)-rm -f $(XWOS_WKSPC_DIR)/$(TARGET).hex
	$(SHOW_RM)-rm -f $(XWOS_WKSPC_DIR)/$(TARGET).bin
	$(SHOW_RM)-rm -rf doc/doxygen

.PHONY: clean distclean all
.PHONY: $(XWMD) $(XWMD_DSM) $(XWMD_CLEAN) $(XWMD_DISTCLEAN)
.PHONY: $(XWPP) $(XWPP_DSM) $(XWPP_CLEAN) $(XWPP_DISTCLEAN)
.PHONY: $(XWBM) $(XWBM_DSM) $(XWBM_CLEAN) $(XWBM_DISTCLEAN)
.PHONY: $(XWEM) $(XWEM_DSM) $(XWEM_CLEAN) $(XWEM_DISTCLEAN)
