#! /bin/make -f
# @file
# @brief SOC描述层的编译规则
# @author
# + 隐星魂 (Roy.Sun) <https://xwos.tech>
# @copyright
# + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
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

SOC_EOBJS :=

SOC_CSRCS :=
SOC_ASRCS :=

SOC_ASRCS += soc_eii_isr.S asm_sched.S

SOC_CSRCS += soc_init.c
SOC_CSRCS += soc_me.c
ifeq ($(SOCCFG_FLASH_OPCODE),y)
    SOC_CSRCS += soc_flash.c
endif
SOC_CSRCS += soc_wdg.c
SOC_CSRCS += soc_reset.c
SOC_CSRCS += soc_irq.c
SOC_CSRCS += soc_syshwt.c
SOC_CSRCS += soc_sched.c


ifeq ($(XuanWuOS_CFG_CORE),smp)
    SOC_CSRCS += soc_xwpmdm.c
    SOC_CSRCS += soc_smp_irqc.c
    SOC_CSRCS += soc_smp_irqc_drv.c
else
    SOC_CSRCS += soc_up_irqc_drv.c
endif

ifeq ($(XuanWuOS_CFG_XWMD),y)
    ifeq ($(XWMDCFG_ds),y)
        ifeq ($(XWMDCFG_ds_SOC),y)
            SOC_CSRCS += driver/ds/soc.c
        endif
        ifeq ($(XWMDCFG_ds_CLK),y)
            SOC_CSRCS += driver/ds/clk.c
        endif
        ifeq ($(XWMDCFG_ds_GPIO),y)
            SOC_CSRCS += driver/ds/gpio.c
        endif
        ifeq ($(XWMDCFG_ds_UART),y)
            ifeq ($(XWMDCFG_ds_UART_GNR),y)
                SOC_CSRCS += driver/ds/uartc.c
            endif
            ifeq ($(XWMDCFG_ds_UART_DMA),y)
                SOC_CSRCS += driver/ds/dmauart0.c
            endif
        endif
        ifeq ($(XWMDCFG_ds_MISC),y)
            SOC_CSRCS += driver/ds/rtc.c
        endif
        ifeq ($(XWMDCFG_ds_I2C_MASTER),y)
            SOC_CSRCS += driver/ds/i2cm.c
        endif
    endif
endif
