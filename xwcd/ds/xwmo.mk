#! /bin/make -f
# @file
# @brief XWOS模块的编译规则
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

include $(XuanWuOS_WKSPC_DIR)/XuanWuOS.cfg
include $(XWBS_UTIL_MK_XWMO)

XWMO_CSRCS :=
XWMO_CSRCS += xwds.c object.c device.c

ifeq ($(XWCDCFG_ds_SOC),y)
    XWMO_CSRCS += soc/chip.c
    ifeq ($(XWCDCFG_ds_SOC_CLK),y)
        XWMO_CSRCS += soc/clock.c
    endif
    ifeq ($(XWCDCFG_ds_SOC_PWR),y)
        XWMO_CSRCS += soc/power.c
    endif
    ifeq ($(XWCDCFG_ds_SOC_GPIO),y)
        XWMO_CSRCS += soc/gpio.c
    endif
    ifeq ($(XWCDCFG_ds_SOC_DMA),y)
        XWMO_CSRCS += soc/dma.c
    endif
    ifeq ($(XWCDCFG_ds_SOC_EIRQ),y)
        XWMO_CSRCS += soc/eirq.c
    endif
    ifeq ($(XWCDCFG_ds_SOC_ERAM),y)
        XWMO_CSRCS += soc/eram.c
    endif
endif
ifeq ($(XWCDCFG_ds_UART),y)
    ifeq ($(XWCDCFG_ds_UART_GNR),y)
        XWMO_CSRCS += uart/general.c
    endif
    ifeq ($(XWCDCFG_ds_UART_DMA),y)
        XWMO_CSRCS += uart/dma.c
    endif
endif
ifeq ($(XWCDCFG_ds_SPI_MASTER),y)
    XWMO_CSRCS += spi/master.c
endif
ifeq ($(XWCDCFG_ds_SPI_PERPHERAL),y)
    XWMO_CSRCS += spi/perpheral.c
endif
ifeq ($(XWCDCFG_ds_I2C_MASTER),y)
    XWMO_CSRCS += i2c/master.c
endif
ifeq ($(XWCDCFG_ds_I2C_SLAVE),y)
    XWMO_CSRCS += i2c/slave.c
endif
ifeq ($(XWCDCFG_ds_I2C_PERPHERAL),y)
    XWMO_CSRCS += i2c/perpheral.c
endif
ifeq ($(XWCDCFG_ds_LIN),y)
    XWMO_CSRCS += lin/controller.c
endif
ifeq ($(XWCDCFG_ds_LINID),y)
    XWOS_CSRCS += lin/linid.c
endif
ifeq ($(XWCDCFG_ds_CAN),y)
    ifeq ($(XWCDCFG_ds_CAN_CONTROLLER),y)
        XWMO_CSRCS += can/controller.c
    endif
    ifeq ($(XWCDCFG_ds_CAN_TRANSCEIVER),y)
        XWMO_CSRCS += can/transceiver.c
    endif
endif
ifeq ($(XWCDCFG_ds_MISC),y)
    XWMO_CSRCS += misc/chip.c
endif

XWMO_CFLAGS =
XWMO_INCDIRS =
include xwbs/$(XuanWuOS_CFG_XWMO_MK)
