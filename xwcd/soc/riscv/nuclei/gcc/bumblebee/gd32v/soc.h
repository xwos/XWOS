/**
 * @file
 * @brief SOC描述层：SOC
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * Copyright (c) 2019 Nuclei Limited. All rights reserved.
 * @copyright
 * + (c) 2021 隐星魂 (Roy.Sun) <https://xwos.tech>
 * + (c) 2019 Nuclei Limited. All rights reserved.
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

#ifndef __soc_h__
#define __soc_h__

#include <xwos/standard.h>

/** @addtogroup Nuclei
 * @{
 */

/** @addtogroup GD32V
 * @{
 */

/** @addtogroup Configuration_of_NMSIS
 * @{
 */

/**
 * @brief IRQ Number Enumeration (For NMSIS)
 */
typedef enum IRQn {
        Reserved0_IRQn = 0, /**< Internal reserved */
        Reserved1_IRQn = 1, /**< Internal reserved */
        Reserved2_IRQn = 2, /**< Internal reserved */
        SysTimerSW_IRQn = 3, /**< System Timer SW interrupt */
        Reserved3_IRQn = 4, /**< Internal reserved */
        Reserved4_IRQn = 5, /**< Internal reserved */
        Reserved5_IRQn = 6, /**< Internal reserved */
        SysTimer_IRQn = 7, /**< System Timer Interrupt */
        Reserved6_IRQn = 8, /**< Internal reserved */
        Reserved7_IRQn = 9, /**< Internal reserved */
        Reserved8_IRQn = 10, /**< Internal reserved */
        Reserved9_IRQn = 11, /**< Internal reserved */
        Reserved10_IRQn = 12, /**< Internal reserved */
        Reserved11_IRQn = 13, /**< Internal reserved */
        Reserved12_IRQn = 14, /**< Internal reserved */
        Reserved13_IRQn = 15, /**< Internal reserved */
        Reserved14_IRQn = 16, /**< Internal reserved */
        BusError_IRQn = 17, /**< Bus Error interrupt */
        PerfMon_IRQn = 18, /**< Performance Monitor */
} IRQn_Type;

/* ======== Configuration of the Nuclei N/NX Processor and Core Peripherals ======== */
/* ========    __NUCLEI_N_REV if your device is a Nuclei-N Class device     ======== */
#define __NUCLEI_N_REV          0x0100 /**< Core Revision r1p0 */
#define __ECLIC_PRESENT         1 /**< Set to 1 if ECLIC is present */
#define __ECLIC_BASEADDR        0xD2000000UL /**< Set to ECLIC baseaddr of your device */
#define __ECLIC_INTCTLBITS      4 /**< Set to 1 - 8, the number of hardware bits
                                   *   are actually implemented in
                                   *   the clicintctl registers. */
#define __ECLIC_INTNUM          86 /**< Set to 1 - 1005, the external interrupt number
                                    *   of ECLIC Unit */
#define __SYSTIMER_PRESENT      1 /**< Set to 1 if System Timer is present */
#define __SYSTIMER_BASEADDR     0xD1000000UL /**< Set to SysTimer baseaddr of your device */
#define __FPU_PRESENT           0 /**< Set to 0, 1, or 2, 0 not present,
                                   *   + 1 single floating point unit present,
                                   *   + 2 double floating point unit present */
#define __DSP_PRESENT           0 /**< Set to 1 if DSP is present */
#define __PMP_PRESENT           1 /**< Set to 1 if PMP is present */
#define __PMP_ENTRY_NUM         8 /**< Set to 8 or 16, the number of PMP entries */
#define __ICACHE_PRESENT        0 /**< Set to 1 if I-Cache is present */
#define __DCACHE_PRESENT        0 /**< Set to 1 if D-Cache is present */
#define __Vendor_SysTickConfig  0 /**< Set to 1 if different SysTick Config is used */

/** @} */ /* End of group Configuration_of_NMSIS */

#include <NMSIS/Core/Include/nmsis_core.h>

/** @} */ /* End of group GD32V */

/** @} */ /* End of group Nuclei */

#endif /* soc.h */
