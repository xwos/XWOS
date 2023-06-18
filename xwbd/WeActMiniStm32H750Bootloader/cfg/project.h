/**
 * @file
 * @brief 工程基本配置
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
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

#ifndef __cfg_project_h__
#define __cfg_project_h__

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********  ARCH & compiler  ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define XWOS_CFG_ARCH                                   arm
#define XWOS_CFG_SUBARCH                                v7m
#define XWOS_CFG_COMPILER                               gcc
#define XWOS_CFG_LIBC                                   newlib
#define XWOS_CFG_LDSCRIPT                               brd.lds
#define XWOS_CFG_ELF_MK                                 elf.mk
#define XWOS_CFG_XWMO_MK                                xwmo.mk
#define XWOS_CFG_XWMO_PREBUILT_MK                       xwmo.prebuilt.mk
#define XWOS_CFG_XWMO_RUST_MK                           xwmo.rust.mk
#include <cfg/arch.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********        CPU        ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define XWOS_CFG_CPU                                    m7
#include <cfg/cpu.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********        SOC        ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define XWOS_CFG_SOC                                    stm32
#include <cfg/soc.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       board       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define XWOS_CFG_BOARD                                  WeActMiniStm32H750Bootloader
#include <cfg/board.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********    XWOS kernel    ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define XWOS_CFG_CORE                                   up
#include <cfg/xwos.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********   chip & device   ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define XWOS_CFG_XWCD                                   1
#include <cfg/xwcd.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********  XWOS middleware  ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define XWOS_CFG_XWMD                                   1
#include <cfg/xwmd.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********  external module  ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define XWOS_CFG_XWEM                                   1
#include <cfg/xwem.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********     APP module    ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define XWOS_CFG_XWAM                                   1
#include <cfg/xwam.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********     OEM module    ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define XWOS_CFG_OEMPATH                                oem
#include <cfg/oem.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********        auto-generated header        ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <cfg/autogen.h>

#endif /* cfg/project.h */
