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
ARCH_AFLAGS := -mcmodel=medany -mno-save-restore
ARCH_CFLAGS := -mcmodel=medany -mno-save-restore
ARCH_CXXFLAGS := -mcmodel=medany -mno-save-restore
ARCH_LDFLAGS := -mcmodel=medany -mno-save-restore

ARCH_ASRCS :=
ARCH_CSRCS :=
ARCH_CXXSRCS :=

ARCH_EOBJS :=

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

ifeq ($(XWLIBCFG_SETJMP),y)
  ARCH_CSRCS += xwosimpl_soc_setjmp.c
endif

ARCH_CSRCS += xwosimpl_soc_lfq.c xwosimpl_soc_xwsc.c
ARCH_CSRCS += xwosimpl_tls.c

ARCH_ASRCS += xwosimpl_soc_spinlock.S xwosasmimpl_soc_xwsc.S
