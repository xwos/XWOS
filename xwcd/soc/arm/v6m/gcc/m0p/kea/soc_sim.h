/**
 * @file
 * @brief SOC System Integration Module
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

#ifndef __soc_sim_h__
#define __soc_sim_h__

#include <xwos/standard.h>
#include <soc.h>

struct soc_sim_reg {
        union {
                struct {
                        volatile const xwu32_t reserved0:1;
                        volatile const xwu32_t lvd:1;
                        volatile const xwu32_t loc:1;
                        volatile const xwu32_t reserved1:2;
                        volatile const xwu32_t wdog:1;
                        volatile const xwu32_t pin:1;
                        volatile const xwu32_t por:1;
                        volatile const xwu32_t reserved2:1;
                        volatile const xwu32_t lockup:1;
                        volatile const xwu32_t sw:1;
                        volatile const xwu32_t mdmap:1;
                        volatile const xwu32_t reserved3:1;
                        volatile const xwu32_t sackerr:1;
                        volatile const xwu32_t reserved4:2;
                        volatile const xwu32_t pinid:4;
                        volatile const xwu32_t revid:4;
                        volatile const xwu32_t subfamid:4;
                        volatile const xwu32_t famid:4;
                } bit;
                volatile const xwu32_t word;
        } srsid; /**< System Reset Status and ID Register, offset: 0x0 */

        union {
                struct {
                        volatile xwu32_t reserved0:1;
                        volatile xwu32_t nmie:1;
                        volatile xwu32_t rstpe:1;
                        volatile xwu32_t swde:1;
                        volatile xwu32_t reserved1:1;
                        volatile xwu32_t actrg:1;
                        volatile xwu32_t reserved2:2;
                        volatile xwu32_t rxdfe:2;
                        volatile xwu32_t rtcc:1;
                        volatile xwu32_t acic:1;
                        volatile xwu32_t rxdce:1;
                        volatile xwu32_t reserved3:1;
                        volatile xwu32_t ftmsync:1;
                        volatile xwu32_t txdme:1;
                        volatile xwu32_t busref:3;
                        volatile xwu32_t clkoe:1;
                        volatile xwu32_t adhwt:3;
                        volatile xwu32_t dlyact:1;
                        volatile xwu32_t delay:8;
                } bit;
                volatile xwu32_t word;
        } sopt0; /**< System Options Register 0, offset: 0x4 */

        union {
                struct {
                        volatile xwu32_t i2c04wen:1;
                        volatile xwu32_t i2c0oinv:1;
                        volatile xwu32_t reserved0:1;
                        volatile xwu32_t acpwts:1;
                        volatile xwu32_t uartpwts:2;
                        volatile xwu32_t reserved1:26;
                } bit;
                volatile xwu32_t word;
        } sopt1; /**< System Options Register, offset: 0x8 */

        union {
                struct {
                        volatile xwu32_t irqps:3;
                        volatile xwu32_t reserved0:1;
                        volatile xwu32_t rtcps:1;
                        volatile xwu32_t i2c0ps:1;
                        volatile xwu32_t spi0ps:1;
                        volatile xwu32_t uart0os:1;
                        volatile xwu32_t ftm0ps0:1;
                        volatile xwu32_t ftm0ps1:1;
                        volatile xwu32_t ftm1ps0:1;
                        volatile xwu32_t ftm1ps1:1;
                        volatile xwu32_t reserved1:12;
                        volatile xwu32_t ftm0clkps:2;
                        volatile xwu32_t ftm1clkps:2;
                        volatile xwu32_t ftm2clkps:2;
                        volatile xwu32_t pwtclkps:2;
                } bit;
                volatile xwu32_t word;
        } pinsel0; /**< Pin Selection Register 0, offset: 0xC */

        union {
                struct {
                        volatile xwu32_t ftm2ps0:2;
                        volatile xwu32_t ftm2ps1:2;
                        volatile xwu32_t ftm2ps2:2;
                        volatile xwu32_t ftm2ps3:2;
                        volatile xwu32_t ftm2ps4:1;
                        volatile xwu32_t ftm2ps5:1;
                        volatile xwu32_t i2c1ps:1;
                        volatile xwu32_t spi1ps:1;
                        volatile xwu32_t uart1ps:1;
                        volatile xwu32_t uart2ps:1;
                        volatile xwu32_t pwtin0ps:1;
                        volatile xwu32_t pwtin1ps:1;
                        volatile xwu32_t mscanps:1;
                        volatile xwu32_t reserved0:15;
                } bit;
                volatile xwu32_t word;
        } pinsel1; /**< Pin Selection Register 1, offset: 0x10 */

        union {
                struct {
                        volatile xwu32_t rtc:1;
                        volatile xwu32_t pit:1;
                        volatile xwu32_t reserved0:2;
                        volatile xwu32_t pwt:1;
                        volatile xwu32_t ftm0:1;
                        volatile xwu32_t ftm1:1;
                        volatile xwu32_t ftm2:1;
                        volatile xwu32_t reserved1:2;
                        volatile xwu32_t crc:1;
                        volatile xwu32_t reserved2:1;
                        volatile xwu32_t flash:1;
                        volatile xwu32_t swd:1;
                        volatile xwu32_t reserved3:1;
                        volatile xwu32_t mscan:1;
                        volatile xwu32_t i2c0:1;
                        volatile xwu32_t i2c1:1;
                        volatile xwu32_t spi0:1;
                        volatile xwu32_t spi1:1;
                        volatile xwu32_t uart0:1;
                        volatile xwu32_t uart1:1;
                        volatile xwu32_t uart2:1;
                        volatile xwu32_t reserved4:1;
                        volatile xwu32_t kbi0:1;
                        volatile xwu32_t kbi1:1;
                        volatile xwu32_t reserved5:1;
                        volatile xwu32_t irq:1;
                        volatile xwu32_t reserved6:1;
                        volatile xwu32_t adc:1;
                        volatile xwu32_t acmp0:1;
                        volatile xwu32_t acmp1:1;
                } bit;
                volatile xwu32_t word;
        } scgc; /**< System Clock Gating Control Register, offset: 0x14 */

        union {
                struct {
                        volatile const xwu32_t id:32;
                } bit;
                volatile const xwu32_t word;
        } uuidl; /**< Universally Unique Identifier Low Register, offset: 0x18 */

        union {
                struct {
                        volatile const xwu32_t id:32;
                } bit;
                volatile const xwu32_t word;
        } uuidml; /**< Universally Unique Identifier Middle Low Register, offset: 0x1C */

        union {
                struct {
                        volatile const xwu32_t id:32;
                } bit;
                volatile const xwu32_t word;
        } uuidmh; /**< Universally Unique Identifier Middle High Register, offset: 0x20 */

        union {
                struct {
                        volatile xwu32_t reserved0:20;
                        volatile xwu32_t outdiv3:1;
                        volatile xwu32_t reserved1:3;
                        volatile xwu32_t outdiv2:1;
                        volatile xwu32_t reserved2:3;
                        volatile xwu32_t outdiv1:2;
                        volatile xwu32_t reserved3:2;
                } bit;
                volatile xwu32_t word;
        } clkdiv; /**< Clock Divider Register, offset: 0x24 */
};

#define SOC_SIM_BASE    (0x40048000U)
#define soc_sim         (*((volatile struct soc_sim_reg *)SOC_SIM_BASE))

#endif /* soc_sim.h */
