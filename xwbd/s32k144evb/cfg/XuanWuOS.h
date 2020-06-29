/**
 * @file
 * @brief XuanWuOS基本配置
 * @author
 * + 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * > Licensed under the Apache License, Version 2.0 (the "License");
 * > you may not use this file except in compliance with the License.
 * > You may obtain a copy of the License at
 * >
 * >         http://www.apache.org/licenses/LICENSE-2.0
 * >
 * > Unless required by applicable law or agreed to in writing, software
 * > distributed under the License is distributed on an "AS IS" BASIS,
 * > WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * > See the License for the specific language governing permissions and
 * > limitations under the License.
 */

#ifndef __cfg_XuanWuOS_h__
#define __cfg_XuanWuOS_h__

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********  ARCH & compiler  ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define XuanWuOS_CFG_ARCH                             arm
#define XuanWuOS_CFG_SUBARCH                          v7m
#define XuanWuOS_CFG_COMPILER                         gcc
#define XuanWuOS_CFG_LDSCRIPT                         cfg/XuanWuOS.lds
#define XuanWuOS_CFG_MK_RULE                          arm-cortex-m.gcc.rule
#define XuanWuOS_CFG_ELF_MK                           arm-cortex-m.gcc.mk
#define XuanWuOS_CFG_XWMO_MK                          arm-cortex-m.gcc.xwmo.mk
#include <cfg/arch.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********        CPU        ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define XuanWuOS_CFG_CPU                              m4
#include <cfg/cpu.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********        SOC        ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define XuanWuOS_CFG_SOC                              s32k14x
#include <cfg/soc.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********     perpheral     ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define XuanWuOS_CFG_PERPHERAL                        1
#include <cfg/perpheral.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       board       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define XuanWuOS_CFG_BOARD                            s32k144evb
#include <cfg/board.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********    XWOS kernel    ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define XuanWuOS_CFG_CORE                             smp
#include <cfg/xwos.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********  XWOS middleware  ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define XuanWuOS_CFG_XWMD                             1
#include <cfg/xwmd.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********  external module  ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define XuanWuOS_CFG_XWEM                             1
#include <cfg/xwem.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********     OEM module    ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define XuanWuOS_CFG_OEMPATH                          xwbd/s32k144evb/oem
#include <cfg/oem.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********        auto-generated header        ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <autogen.h>

#endif /* cfg/XuanWuOS.h */