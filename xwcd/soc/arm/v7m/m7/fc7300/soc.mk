#! /bin/make -f
# @file
# @brief SOC描述层的编译规则
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

SOC_INCDIRS :=
SOC_AFLAGS := -mfpu=fpv5-sp-d16
SOC_CFLAGS := -mfpu=fpv5-sp-d16
SOC_CXXFLAGS := -mfpu=fpv5-sp-d16
SOC_LDFLAGS := -mfpu=fpv5-sp-d16

SOC_ASRCS :=
SOC_CSRCS :=
SOC_CXXSRCS :=

SOC_EOBJS :=

SOC_CSRCS += soc_init.c
ifeq ($(SOCCFG_CHIP_FC7300F4MDS),y)
  SOC_CSRCS += fc7300f4mds_init.c
endif
ifeq ($(SOCCFG_CHIP_FC7300F4MDD),y)
  SOC_CSRCS += fc7300f4mdd_init.c
endif
ifeq ($(SOCCFG_CHIP_FC7300F8MDT),y)
  SOC_CSRCS += fc7300f8mdt_init.c
endif
ifeq ($(SOCCFG_CHIP_FC7300F8MDQ),y)
  SOC_CSRCS += fc7300f8mdq_init.c
endif
SOC_CSRCS += xwosimpl_irq.c
SOC_CSRCS += xwosimpl_skd.c
SOC_CSRCS += xwosimpl_syshwt.c

ifeq ($(SOCCFG_LIB_XWAOP8),y)
  SOC_CSRCS += xwosimpl_soc_xwaop/u8/add.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u8/and.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u8/load.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u8/or.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u8/read.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u8/rsb.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u8/store.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u8/sub.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u8/t1ma_then_c0m.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u8/t1mo_then_c0m.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u8/t0ma_then_s1m.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u8/t0mo_then_s1m.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u8/teq_then_add.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u8/teq_then_rsb.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u8/teq_then_sub.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u8/teq_then_write.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u8/tge_then_add.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u8/tge_then_rsb.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u8/tge_then_sub.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u8/tge_then_write.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u8/tgele_then_add.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u8/tgele_then_rsb.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u8/tgele_then_sub.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u8/tgele_then_write.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u8/tgelt_then_add.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u8/tgelt_then_rsb.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u8/tgelt_then_sub.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u8/tgelt_then_write.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u8/tgt_then_add.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u8/tgt_then_rsb.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u8/tgt_then_sub.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u8/tgt_then_write.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u8/tgtle_then_add.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u8/tgtle_then_rsb.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u8/tgtle_then_sub.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u8/tgtle_then_write.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u8/tgtlt_then_add.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u8/tgtlt_then_rsb.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u8/tgtlt_then_sub.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u8/tgtlt_then_write.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u8/tle_then_add.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u8/tle_then_rsb.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u8/tle_then_sub.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u8/tle_then_write.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u8/tlt_then_add.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u8/tlt_then_rsb.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u8/tlt_then_sub.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u8/tlt_then_write.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u8/tne_then_add.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u8/tne_then_rsb.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u8/tne_then_sub.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u8/tne_then_write.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u8/tst_then_op.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u8/write.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u8/xor.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u8/c0m.c

  SOC_CSRCS += xwosimpl_soc_xwaop/s8/add.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s8/and.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s8/load.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s8/or.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s8/read.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s8/rsb.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s8/store.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s8/sub.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s8/teq_then_add.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s8/teq_then_rsb.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s8/teq_then_sub.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s8/teq_then_write.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s8/tge_then_add.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s8/tge_then_rsb.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s8/tge_then_sub.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s8/tge_then_write.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s8/tgele_then_add.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s8/tgele_then_rsb.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s8/tgele_then_sub.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s8/tgele_then_write.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s8/tgelt_then_add.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s8/tgelt_then_rsb.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s8/tgelt_then_sub.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s8/tgelt_then_write.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s8/tgt_then_add.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s8/tgt_then_rsb.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s8/tgt_then_sub.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s8/tgt_then_write.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s8/tgtle_then_add.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s8/tgtle_then_rsb.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s8/tgtle_then_sub.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s8/tgtle_then_write.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s8/tgtlt_then_add.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s8/tgtlt_then_rsb.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s8/tgtlt_then_sub.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s8/tgtlt_then_write.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s8/tle_then_add.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s8/tle_then_rsb.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s8/tle_then_sub.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s8/tle_then_write.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s8/tlt_then_add.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s8/tlt_then_rsb.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s8/tlt_then_sub.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s8/tlt_then_write.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s8/tne_then_add.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s8/tne_then_rsb.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s8/tne_then_sub.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s8/tne_then_write.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s8/tst_then_op.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s8/write.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s8/xor.c
endif

ifeq ($(SOCCFG_LIB_XWAOP16),y)
  SOC_CSRCS += xwosimpl_soc_xwaop/u16/add.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u16/and.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u16/load.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u16/or.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u16/read.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u16/rsb.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u16/store.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u16/sub.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u16/t1ma_then_c0m.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u16/t1mo_then_c0m.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u16/t0ma_then_s1m.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u16/t0mo_then_s1m.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u16/teq_then_add.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u16/teq_then_rsb.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u16/teq_then_sub.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u16/teq_then_write.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u16/tge_then_add.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u16/tge_then_rsb.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u16/tge_then_sub.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u16/tge_then_write.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u16/tgele_then_add.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u16/tgele_then_rsb.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u16/tgele_then_sub.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u16/tgele_then_write.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u16/tgelt_then_add.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u16/tgelt_then_rsb.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u16/tgelt_then_sub.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u16/tgelt_then_write.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u16/tgt_then_add.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u16/tgt_then_rsb.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u16/tgt_then_sub.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u16/tgt_then_write.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u16/tgtle_then_add.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u16/tgtle_then_rsb.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u16/tgtle_then_sub.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u16/tgtle_then_write.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u16/tgtlt_then_add.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u16/tgtlt_then_rsb.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u16/tgtlt_then_sub.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u16/tgtlt_then_write.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u16/tle_then_add.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u16/tle_then_rsb.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u16/tle_then_sub.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u16/tle_then_write.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u16/tlt_then_add.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u16/tlt_then_rsb.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u16/tlt_then_sub.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u16/tlt_then_write.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u16/tne_then_add.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u16/tne_then_rsb.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u16/tne_then_sub.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u16/tne_then_write.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u16/tst_then_op.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u16/write.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u16/xor.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u16/c0m.c

  SOC_CSRCS += xwosimpl_soc_xwaop/s16/add.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s16/and.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s16/load.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s16/or.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s16/read.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s16/rsb.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s16/store.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s16/sub.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s16/teq_then_add.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s16/teq_then_rsb.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s16/teq_then_sub.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s16/teq_then_write.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s16/tge_then_add.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s16/tge_then_rsb.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s16/tge_then_sub.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s16/tge_then_write.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s16/tgele_then_add.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s16/tgele_then_rsb.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s16/tgele_then_sub.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s16/tgele_then_write.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s16/tgelt_then_add.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s16/tgelt_then_rsb.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s16/tgelt_then_sub.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s16/tgelt_then_write.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s16/tgt_then_add.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s16/tgt_then_rsb.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s16/tgt_then_sub.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s16/tgt_then_write.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s16/tgtle_then_add.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s16/tgtle_then_rsb.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s16/tgtle_then_sub.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s16/tgtle_then_write.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s16/tgtlt_then_add.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s16/tgtlt_then_rsb.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s16/tgtlt_then_sub.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s16/tgtlt_then_write.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s16/tle_then_add.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s16/tle_then_rsb.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s16/tle_then_sub.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s16/tle_then_write.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s16/tlt_then_add.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s16/tlt_then_rsb.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s16/tlt_then_sub.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s16/tlt_then_write.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s16/tne_then_add.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s16/tne_then_rsb.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s16/tne_then_sub.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s16/tne_then_write.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s16/tst_then_op.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s16/write.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s16/xor.c
endif

ifeq ($(SOCCFG_LIB_XWAOP32),y)
  SOC_CSRCS += xwosimpl_soc_xwaop/u32/add.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u32/and.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u32/load.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u32/or.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u32/read.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u32/rsb.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u32/store.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u32/sub.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u32/t1ma_then_c0m.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u32/t1mo_then_c0m.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u32/t0ma_then_s1m.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u32/t0mo_then_s1m.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u32/teq_then_add.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u32/teq_then_rsb.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u32/teq_then_sub.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u32/teq_then_write.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u32/tge_then_add.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u32/tge_then_rsb.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u32/tge_then_sub.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u32/tge_then_write.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u32/tgele_then_add.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u32/tgele_then_rsb.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u32/tgele_then_sub.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u32/tgele_then_write.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u32/tgelt_then_add.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u32/tgelt_then_rsb.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u32/tgelt_then_sub.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u32/tgelt_then_write.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u32/tgt_then_add.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u32/tgt_then_rsb.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u32/tgt_then_sub.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u32/tgt_then_write.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u32/tgtle_then_add.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u32/tgtle_then_rsb.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u32/tgtle_then_sub.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u32/tgtle_then_write.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u32/tgtlt_then_add.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u32/tgtlt_then_rsb.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u32/tgtlt_then_sub.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u32/tgtlt_then_write.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u32/tle_then_add.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u32/tle_then_rsb.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u32/tle_then_sub.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u32/tle_then_write.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u32/tlt_then_add.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u32/tlt_then_rsb.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u32/tlt_then_sub.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u32/tlt_then_write.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u32/tne_then_add.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u32/tne_then_rsb.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u32/tne_then_sub.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u32/tne_then_write.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u32/tst_then_op.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u32/write.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u32/xor.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u32/c0m.c

  SOC_CSRCS += xwosimpl_soc_xwaop/s32/add.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s32/and.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s32/load.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s32/or.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s32/read.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s32/rsb.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s32/store.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s32/sub.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s32/teq_then_add.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s32/teq_then_rsb.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s32/teq_then_sub.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s32/teq_then_write.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s32/tge_then_add.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s32/tge_then_rsb.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s32/tge_then_sub.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s32/tge_then_write.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s32/tgele_then_add.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s32/tgele_then_rsb.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s32/tgele_then_sub.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s32/tgele_then_write.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s32/tgelt_then_add.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s32/tgelt_then_rsb.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s32/tgelt_then_sub.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s32/tgelt_then_write.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s32/tgt_then_add.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s32/tgt_then_rsb.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s32/tgt_then_sub.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s32/tgt_then_write.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s32/tgtle_then_add.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s32/tgtle_then_rsb.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s32/tgtle_then_sub.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s32/tgtle_then_write.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s32/tgtlt_then_add.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s32/tgtlt_then_rsb.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s32/tgtlt_then_sub.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s32/tgtlt_then_write.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s32/tle_then_add.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s32/tle_then_rsb.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s32/tle_then_sub.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s32/tle_then_write.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s32/tlt_then_add.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s32/tlt_then_rsb.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s32/tlt_then_sub.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s32/tlt_then_write.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s32/tne_then_add.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s32/tne_then_rsb.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s32/tne_then_sub.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s32/tne_then_write.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s32/tst_then_op.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s32/write.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s32/xor.c
endif

ifeq ($(SOCCFG_LIB_XWAOP64),y)
  SOC_CSRCS += xwosimpl_soc_xwaop/u64/add.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u64/and.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u64/load.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u64/or.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u64/read.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u64/rsb.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u64/store.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u64/sub.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u64/t1ma_then_c0m.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u64/t1mo_then_c0m.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u64/t0ma_then_s1m.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u64/t0mo_then_s1m.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u64/teq_then_add.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u64/teq_then_rsb.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u64/teq_then_sub.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u64/teq_then_write.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u64/tge_then_add.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u64/tge_then_rsb.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u64/tge_then_sub.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u64/tge_then_write.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u64/tgele_then_add.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u64/tgele_then_rsb.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u64/tgele_then_sub.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u64/tgele_then_write.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u64/tgelt_then_add.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u64/tgelt_then_rsb.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u64/tgelt_then_sub.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u64/tgelt_then_write.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u64/tgt_then_add.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u64/tgt_then_rsb.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u64/tgt_then_sub.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u64/tgt_then_write.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u64/tgtle_then_add.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u64/tgtle_then_rsb.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u64/tgtle_then_sub.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u64/tgtle_then_write.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u64/tgtlt_then_add.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u64/tgtlt_then_rsb.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u64/tgtlt_then_sub.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u64/tgtlt_then_write.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u64/tle_then_add.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u64/tle_then_rsb.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u64/tle_then_sub.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u64/tle_then_write.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u64/tlt_then_add.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u64/tlt_then_rsb.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u64/tlt_then_sub.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u64/tlt_then_write.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u64/tne_then_add.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u64/tne_then_rsb.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u64/tne_then_sub.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u64/tne_then_write.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u64/tst_then_op.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u64/write.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u64/xor.c
  SOC_CSRCS += xwosimpl_soc_xwaop/u64/c0m.c

  SOC_CSRCS += xwosimpl_soc_xwaop/s64/add.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s64/and.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s64/load.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s64/or.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s64/read.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s64/rsb.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s64/store.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s64/sub.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s64/teq_then_add.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s64/teq_then_rsb.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s64/teq_then_sub.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s64/teq_then_write.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s64/tge_then_add.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s64/tge_then_rsb.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s64/tge_then_sub.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s64/tge_then_write.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s64/tgele_then_add.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s64/tgele_then_rsb.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s64/tgele_then_sub.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s64/tgele_then_write.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s64/tgelt_then_add.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s64/tgelt_then_rsb.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s64/tgelt_then_sub.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s64/tgelt_then_write.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s64/tgt_then_add.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s64/tgt_then_rsb.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s64/tgt_then_sub.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s64/tgt_then_write.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s64/tgtle_then_add.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s64/tgtle_then_rsb.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s64/tgtle_then_sub.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s64/tgtle_then_write.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s64/tgtlt_then_add.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s64/tgtlt_then_rsb.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s64/tgtlt_then_sub.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s64/tgtlt_then_write.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s64/tle_then_add.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s64/tle_then_rsb.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s64/tle_then_sub.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s64/tle_then_write.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s64/tlt_then_add.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s64/tlt_then_rsb.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s64/tlt_then_sub.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s64/tlt_then_write.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s64/tne_then_add.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s64/tne_then_rsb.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s64/tne_then_sub.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s64/tne_then_write.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s64/tst_then_op.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s64/write.c
  SOC_CSRCS += xwosimpl_soc_xwaop/s64/xor.c
endif

ifeq ($(SOCCFG_LIB_LFQ),y)
  SOC_CSRCS += xwosimpl_soc_lfq.c
endif
