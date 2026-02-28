#! /bin/make -f
# @file
# @brief ARCH描述层的编译规则
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

ARCH_INCDIRS :=
ARCH_AFLAGS :=
ARCH_CFLAGS :=
ARCH_CXXFLAGS :=
ARCH_LDFLAGS := -fno-use-cxa-atexit

ARCH_ASRCS :=
ARCH_CSRCS :=
ARCH_CXXSRCS :=

ARCH_EOBJS :=

ifeq ($(ARCHCFG_LIB_XWAOP8),y)
  ARCH_CSRCS += xwosimpl_soc_xwaop/u8/add.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u8/and.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u8/load.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u8/or.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u8/read.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u8/rsb.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u8/store.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u8/sub.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u8/t1ma_then_c0m.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u8/t1mo_then_c0m.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u8/t0ma_then_s1m.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u8/t0mo_then_s1m.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u8/teq_then_add.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u8/teq_then_rsb.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u8/teq_then_sub.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u8/teq_then_write.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u8/tge_then_add.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u8/tge_then_rsb.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u8/tge_then_sub.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u8/tge_then_write.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u8/tgele_then_add.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u8/tgele_then_rsb.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u8/tgele_then_sub.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u8/tgele_then_write.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u8/tgelt_then_add.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u8/tgelt_then_rsb.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u8/tgelt_then_sub.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u8/tgelt_then_write.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u8/tgt_then_add.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u8/tgt_then_rsb.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u8/tgt_then_sub.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u8/tgt_then_write.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u8/tgtle_then_add.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u8/tgtle_then_rsb.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u8/tgtle_then_sub.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u8/tgtle_then_write.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u8/tgtlt_then_add.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u8/tgtlt_then_rsb.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u8/tgtlt_then_sub.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u8/tgtlt_then_write.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u8/tle_then_add.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u8/tle_then_rsb.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u8/tle_then_sub.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u8/tle_then_write.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u8/tlt_then_add.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u8/tlt_then_rsb.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u8/tlt_then_sub.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u8/tlt_then_write.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u8/tne_then_add.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u8/tne_then_rsb.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u8/tne_then_sub.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u8/tne_then_write.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u8/tst_then_op.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u8/write.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u8/xor.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u8/c0m.c

  ARCH_CSRCS += xwosimpl_soc_xwaop/s8/add.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s8/and.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s8/load.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s8/or.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s8/read.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s8/rsb.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s8/store.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s8/sub.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s8/teq_then_add.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s8/teq_then_rsb.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s8/teq_then_sub.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s8/teq_then_write.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s8/tge_then_add.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s8/tge_then_rsb.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s8/tge_then_sub.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s8/tge_then_write.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s8/tgele_then_add.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s8/tgele_then_rsb.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s8/tgele_then_sub.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s8/tgele_then_write.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s8/tgelt_then_add.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s8/tgelt_then_rsb.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s8/tgelt_then_sub.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s8/tgelt_then_write.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s8/tgt_then_add.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s8/tgt_then_rsb.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s8/tgt_then_sub.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s8/tgt_then_write.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s8/tgtle_then_add.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s8/tgtle_then_rsb.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s8/tgtle_then_sub.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s8/tgtle_then_write.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s8/tgtlt_then_add.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s8/tgtlt_then_rsb.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s8/tgtlt_then_sub.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s8/tgtlt_then_write.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s8/tle_then_add.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s8/tle_then_rsb.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s8/tle_then_sub.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s8/tle_then_write.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s8/tlt_then_add.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s8/tlt_then_rsb.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s8/tlt_then_sub.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s8/tlt_then_write.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s8/tne_then_add.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s8/tne_then_rsb.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s8/tne_then_sub.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s8/tne_then_write.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s8/tst_then_op.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s8/write.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s8/xor.c
endif

ifeq ($(ARCHCFG_LIB_XWAOP16),y)
  ARCH_CSRCS += xwosimpl_soc_xwaop/u16/add.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u16/and.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u16/load.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u16/or.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u16/read.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u16/rsb.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u16/store.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u16/sub.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u16/t1ma_then_c0m.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u16/t1mo_then_c0m.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u16/t0ma_then_s1m.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u16/t0mo_then_s1m.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u16/teq_then_add.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u16/teq_then_rsb.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u16/teq_then_sub.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u16/teq_then_write.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u16/tge_then_add.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u16/tge_then_rsb.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u16/tge_then_sub.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u16/tge_then_write.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u16/tgele_then_add.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u16/tgele_then_rsb.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u16/tgele_then_sub.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u16/tgele_then_write.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u16/tgelt_then_add.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u16/tgelt_then_rsb.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u16/tgelt_then_sub.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u16/tgelt_then_write.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u16/tgt_then_add.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u16/tgt_then_rsb.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u16/tgt_then_sub.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u16/tgt_then_write.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u16/tgtle_then_add.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u16/tgtle_then_rsb.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u16/tgtle_then_sub.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u16/tgtle_then_write.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u16/tgtlt_then_add.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u16/tgtlt_then_rsb.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u16/tgtlt_then_sub.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u16/tgtlt_then_write.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u16/tle_then_add.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u16/tle_then_rsb.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u16/tle_then_sub.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u16/tle_then_write.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u16/tlt_then_add.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u16/tlt_then_rsb.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u16/tlt_then_sub.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u16/tlt_then_write.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u16/tne_then_add.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u16/tne_then_rsb.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u16/tne_then_sub.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u16/tne_then_write.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u16/tst_then_op.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u16/write.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u16/xor.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u16/c0m.c

  ARCH_CSRCS += xwosimpl_soc_xwaop/s16/add.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s16/and.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s16/load.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s16/or.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s16/read.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s16/rsb.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s16/store.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s16/sub.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s16/teq_then_add.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s16/teq_then_rsb.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s16/teq_then_sub.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s16/teq_then_write.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s16/tge_then_add.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s16/tge_then_rsb.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s16/tge_then_sub.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s16/tge_then_write.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s16/tgele_then_add.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s16/tgele_then_rsb.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s16/tgele_then_sub.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s16/tgele_then_write.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s16/tgelt_then_add.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s16/tgelt_then_rsb.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s16/tgelt_then_sub.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s16/tgelt_then_write.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s16/tgt_then_add.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s16/tgt_then_rsb.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s16/tgt_then_sub.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s16/tgt_then_write.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s16/tgtle_then_add.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s16/tgtle_then_rsb.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s16/tgtle_then_sub.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s16/tgtle_then_write.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s16/tgtlt_then_add.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s16/tgtlt_then_rsb.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s16/tgtlt_then_sub.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s16/tgtlt_then_write.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s16/tle_then_add.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s16/tle_then_rsb.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s16/tle_then_sub.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s16/tle_then_write.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s16/tlt_then_add.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s16/tlt_then_rsb.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s16/tlt_then_sub.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s16/tlt_then_write.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s16/tne_then_add.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s16/tne_then_rsb.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s16/tne_then_sub.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s16/tne_then_write.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s16/tst_then_op.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s16/write.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s16/xor.c
endif

ifeq ($(ARCHCFG_LIB_XWAOP32),y)
  ARCH_CSRCS += xwosimpl_soc_xwaop/u32/add.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u32/and.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u32/load.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u32/or.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u32/read.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u32/rsb.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u32/store.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u32/sub.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u32/t1ma_then_c0m.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u32/t1mo_then_c0m.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u32/t0ma_then_s1m.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u32/t0mo_then_s1m.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u32/teq_then_add.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u32/teq_then_rsb.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u32/teq_then_sub.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u32/teq_then_write.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u32/tge_then_add.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u32/tge_then_rsb.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u32/tge_then_sub.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u32/tge_then_write.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u32/tgele_then_add.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u32/tgele_then_rsb.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u32/tgele_then_sub.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u32/tgele_then_write.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u32/tgelt_then_add.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u32/tgelt_then_rsb.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u32/tgelt_then_sub.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u32/tgelt_then_write.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u32/tgt_then_add.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u32/tgt_then_rsb.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u32/tgt_then_sub.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u32/tgt_then_write.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u32/tgtle_then_add.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u32/tgtle_then_rsb.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u32/tgtle_then_sub.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u32/tgtle_then_write.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u32/tgtlt_then_add.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u32/tgtlt_then_rsb.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u32/tgtlt_then_sub.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u32/tgtlt_then_write.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u32/tle_then_add.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u32/tle_then_rsb.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u32/tle_then_sub.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u32/tle_then_write.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u32/tlt_then_add.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u32/tlt_then_rsb.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u32/tlt_then_sub.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u32/tlt_then_write.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u32/tne_then_add.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u32/tne_then_rsb.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u32/tne_then_sub.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u32/tne_then_write.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u32/tst_then_op.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u32/write.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u32/xor.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u32/c0m.c

  ARCH_CSRCS += xwosimpl_soc_xwaop/s32/add.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s32/and.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s32/load.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s32/or.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s32/read.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s32/rsb.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s32/store.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s32/sub.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s32/teq_then_add.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s32/teq_then_rsb.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s32/teq_then_sub.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s32/teq_then_write.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s32/tge_then_add.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s32/tge_then_rsb.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s32/tge_then_sub.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s32/tge_then_write.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s32/tgele_then_add.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s32/tgele_then_rsb.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s32/tgele_then_sub.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s32/tgele_then_write.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s32/tgelt_then_add.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s32/tgelt_then_rsb.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s32/tgelt_then_sub.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s32/tgelt_then_write.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s32/tgt_then_add.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s32/tgt_then_rsb.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s32/tgt_then_sub.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s32/tgt_then_write.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s32/tgtle_then_add.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s32/tgtle_then_rsb.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s32/tgtle_then_sub.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s32/tgtle_then_write.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s32/tgtlt_then_add.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s32/tgtlt_then_rsb.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s32/tgtlt_then_sub.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s32/tgtlt_then_write.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s32/tle_then_add.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s32/tle_then_rsb.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s32/tle_then_sub.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s32/tle_then_write.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s32/tlt_then_add.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s32/tlt_then_rsb.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s32/tlt_then_sub.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s32/tlt_then_write.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s32/tne_then_add.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s32/tne_then_rsb.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s32/tne_then_sub.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s32/tne_then_write.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s32/tst_then_op.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s32/write.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s32/xor.c
endif

ifeq ($(ARCHCFG_LIB_XWAOP64),y)
  ARCH_CSRCS += xwosimpl_soc_xwaop/u64/add.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u64/and.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u64/load.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u64/or.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u64/read.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u64/rsb.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u64/store.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u64/sub.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u64/t1ma_then_c0m.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u64/t1mo_then_c0m.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u64/t0ma_then_s1m.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u64/t0mo_then_s1m.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u64/teq_then_add.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u64/teq_then_rsb.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u64/teq_then_sub.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u64/teq_then_write.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u64/tge_then_add.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u64/tge_then_rsb.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u64/tge_then_sub.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u64/tge_then_write.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u64/tgele_then_add.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u64/tgele_then_rsb.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u64/tgele_then_sub.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u64/tgele_then_write.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u64/tgelt_then_add.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u64/tgelt_then_rsb.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u64/tgelt_then_sub.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u64/tgelt_then_write.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u64/tgt_then_add.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u64/tgt_then_rsb.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u64/tgt_then_sub.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u64/tgt_then_write.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u64/tgtle_then_add.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u64/tgtle_then_rsb.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u64/tgtle_then_sub.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u64/tgtle_then_write.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u64/tgtlt_then_add.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u64/tgtlt_then_rsb.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u64/tgtlt_then_sub.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u64/tgtlt_then_write.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u64/tle_then_add.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u64/tle_then_rsb.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u64/tle_then_sub.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u64/tle_then_write.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u64/tlt_then_add.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u64/tlt_then_rsb.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u64/tlt_then_sub.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u64/tlt_then_write.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u64/tne_then_add.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u64/tne_then_rsb.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u64/tne_then_sub.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u64/tne_then_write.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u64/tst_then_op.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u64/write.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u64/xor.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/u64/c0m.c

  ARCH_CSRCS += xwosimpl_soc_xwaop/s64/add.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s64/and.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s64/load.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s64/or.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s64/read.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s64/rsb.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s64/store.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s64/sub.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s64/teq_then_add.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s64/teq_then_rsb.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s64/teq_then_sub.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s64/teq_then_write.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s64/tge_then_add.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s64/tge_then_rsb.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s64/tge_then_sub.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s64/tge_then_write.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s64/tgele_then_add.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s64/tgele_then_rsb.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s64/tgele_then_sub.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s64/tgele_then_write.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s64/tgelt_then_add.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s64/tgelt_then_rsb.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s64/tgelt_then_sub.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s64/tgelt_then_write.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s64/tgt_then_add.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s64/tgt_then_rsb.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s64/tgt_then_sub.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s64/tgt_then_write.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s64/tgtle_then_add.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s64/tgtle_then_rsb.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s64/tgtle_then_sub.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s64/tgtle_then_write.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s64/tgtlt_then_add.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s64/tgtlt_then_rsb.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s64/tgtlt_then_sub.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s64/tgtlt_then_write.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s64/tle_then_add.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s64/tle_then_rsb.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s64/tle_then_sub.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s64/tle_then_write.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s64/tlt_then_add.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s64/tlt_then_rsb.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s64/tlt_then_sub.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s64/tlt_then_write.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s64/tne_then_add.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s64/tne_then_rsb.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s64/tne_then_sub.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s64/tne_then_write.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s64/tst_then_op.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s64/write.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/s64/xor.c
endif

ifeq ($(ARCHCFG_LIB_XWBMPAOP),y)
  ARCH_CSRCS += xwosimpl_soc_xwaop/bmp/t1i.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/bmp/c0i.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/bmp/x1i.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/bmp/s1i.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/bmp/t0i_then_s1i.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/bmp/t1i_then_c0i.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/bmp/ffs_then_c0i.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/bmp/ffz_then_s1i.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/bmp/fls_then_c0i.c
  ARCH_CSRCS += xwosimpl_soc_xwaop/bmp/flz_then_s1i.c
endif

ifeq ($(ARCHCFG_LIB_XWBOP_FFS8),y)
  ARCH_CSRCS += xwosimpl_soc_xwbop/ffs8.c
endif
ifeq ($(ARCHCFG_LIB_XWBOP_FLS8),y)
  ARCH_CSRCS += xwosimpl_soc_xwbop/fls8.c
endif
ifeq ($(ARCHCFG_LIB_XWBOP_RBIT8),y)
  ARCH_CSRCS += xwosimpl_soc_xwbop/rbit8.c
endif

ifeq ($(ARCHCFG_LIB_XWBOP_FFS16),y)
  ARCH_CSRCS += xwosimpl_soc_xwbop/ffs16.c
endif
ifeq ($(ARCHCFG_LIB_XWBOP_FLS16),y)
  ARCH_CSRCS += xwosimpl_soc_xwbop/fls16.c
endif
ifeq ($(ARCHCFG_LIB_XWBOP_RBIT16),y)
  ARCH_CSRCS += xwosimpl_soc_xwbop/rbit16.c
endif
ifeq ($(ARCHCFG_LIB_XWBOP_RE16),y)
  ARCH_CSRCS += xwosimpl_soc_xwbop/re16.c
endif

ifeq ($(ARCHCFG_LIB_XWBOP_FFS32),y)
  ARCH_CSRCS += xwosimpl_soc_xwbop/ffs32.c
endif
ifeq ($(ARCHCFG_LIB_XWBOP_FLS32),y)
  ARCH_CSRCS += xwosimpl_soc_xwbop/fls32.c
endif
ifeq ($(ARCHCFG_LIB_XWBOP_RBIT32),y)
  ARCH_CSRCS += xwosimpl_soc_xwbop/rbit32.c
endif
ifeq ($(ARCHCFG_LIB_XWBOP_RE32),y)
  ARCH_CSRCS += xwosimpl_soc_xwbop/re32.c
endif

ifeq ($(ARCHCFG_LIB_XWBOP_FFS64),y)
  ARCH_CSRCS += xwosimpl_soc_xwbop/ffs64.c
endif
ifeq ($(ARCHCFG_LIB_XWBOP_FLS64),y)
  ARCH_CSRCS += xwosimpl_soc_xwbop/fls64.c
endif
ifeq ($(ARCHCFG_LIB_XWBOP_RBIT64),y)
  ARCH_CSRCS += xwosimpl_soc_xwbop/rbit64.c
endif
ifeq ($(ARCHCFG_LIB_XWBOP_RE64),y)
  ARCH_CSRCS += xwosimpl_soc_xwbop/re64.c
endif
ifeq ($(ARCHCFG_LIB_XWBMPOP_S1I),y)
  ARCH_CSRCS += xwosimpl_soc_xwbop/xwbmpop_s1i.c
endif
ifeq ($(ARCHCFG_LIB_XWBMPOP_C0I),y)
  ARCH_CSRCS += xwosimpl_soc_xwbop/xwbmpop_c0i.c
endif
ifeq ($(ARCHCFG_LIB_XWBMPOP_X1I),y)
  ARCH_CSRCS += xwosimpl_soc_xwbop/xwbmpop_x1i.c
endif
ifeq ($(ARCHCFG_LIB_XWBMPOP_FFS),y)
  ARCH_CSRCS += xwosimpl_soc_xwbop/xwbmpop_ffs.c
endif
ifeq ($(ARCHCFG_LIB_XWBMPOP_FFZ),y)
  ARCH_CSRCS += xwosimpl_soc_xwbop/xwbmpop_ffz.c
endif
ifeq ($(ARCHCFG_LIB_XWBMPOP_FLS),y)
  ARCH_CSRCS += xwosimpl_soc_xwbop/xwbmpop_fls.c
endif
ifeq ($(ARCHCFG_LIB_XWBMPOP_FLZ),y)
  ARCH_CSRCS += xwosimpl_soc_xwbop/xwbmpop_flz.c
endif

ifeq ($(XWLIBCFG_SETJMP),y)
  ARCH_CSRCS += xwosimpl_soc_setjmp.c
endif

ARCH_CSRCS += arch_init.c
ARCH_CSRCS += arch_firmware.c arch_irq.c xwosimpl_soc_lfq.c
ARCH_CSRCS += arch_skd.c arch_systick.c
ARCH_CSRCS += xwosimpl_tls.c

ARCH_AFLAGS_llvm := --target=armv6m-none-eabi
ARCH_CFLAGS_llvm := --target=armv6m-none-eabi
ARCH_CXXFLAGS_llvm := --target=armv6m-none-eabi
ARCH_LDFLAGS_llvm := --target=armv6m-none-eabi
