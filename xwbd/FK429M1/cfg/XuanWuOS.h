/**
 * @file
 * @brief XuanWuOS基本配置
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
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
#define XuanWuOS_CFG_ARCH                               arm
#define XuanWuOS_CFG_SUBARCH                            v7m
#define XuanWuOS_CFG_COMPILER                           gcc
#define XuanWuOS_CFG_LDSCRIPT                           cfg/XuanWuOS.lds
#define XuanWuOS_CFG_ELF_MK                             elf.mk
#define XuanWuOS_CFG_XWMO_MK                            xwmo.mk
#define XuanWuOS_CFG_XWMO_PREBUILT_MK                   xwmo.prebuilt.mk
#include <cfg/arch.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********        CPU        ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define XuanWuOS_CFG_CPU                                m4
#include <cfg/cpu.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********        SOC        ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define XuanWuOS_CFG_SOC                                stm32
#include <cfg/soc.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       board       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define XuanWuOS_CFG_BOARD                              FK429M1
#include <cfg/board.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********    XWOS kernel    ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define XuanWuOS_CFG_CORE                               mp
#include <cfg/xwos.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********   chip & device   ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define XuanWuOS_CFG_XWCD                               1
#include <cfg/xwcd.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********  XWOS middleware  ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define XuanWuOS_CFG_XWMD                               1
#include <cfg/xwmd.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********  external module  ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define XuanWuOS_CFG_XWEM                               1
#include <cfg/xwem.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********     APP module    ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define XuanWuOS_CFG_XWAM                               1
#include <cfg/xwam.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********     OEM module    ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define XuanWuOS_CFG_OEMPATH                            ../oem
#include <cfg/oem.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********        auto-generated header        ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <autogen.h>

#endif /* cfg/XuanWuOS.h */
