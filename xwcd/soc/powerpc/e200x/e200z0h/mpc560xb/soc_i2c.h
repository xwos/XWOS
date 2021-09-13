/**
 * @file
 * @brief SOC描述层：I2C
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

#ifndef __soc_i2c_h__
#define __soc_i2c_h__

#include <xwos/standard.h>
#include <soc.h>

/******** ********Inter-integrated Circuit Interface ******** ********/
/******** SOC_I2C_IBCR register ********/
#define SOC_I2C_IBCR_DMAED_MASK         ((xwu8_t)0x02) /**< DMA Enable */
#define SOC_I2C_IBCR_RSTA_MASK          ((xwu8_t)0x04) /**< Repeat Start Condition */
#define SOC_I2C_IBCR_NOACK_MASK         ((xwu8_t)0x08) /**< Data Acknowledge disable */
#define SOC_I2C_IBCR_TXRX_MASK          ((xwu8_t)0x10) /**< Transmit/#Receive mode select */
#define SOC_I2C_IBCR_MSSL_MASK          ((xwu8_t)0x20) /**< Master/#Slave mode select */
#define SOC_I2C_IBCR_IBIE_MASK          ((xwu8_t)0x40) /**< I-Bus Interrupt Enable */
#define SOC_I2C_IBCR_MDIS_MASK          ((xwu8_t)0x80) /**< module disable */

/**
 * @brief mpc560xb i2c private configurations
 */
struct soc_i2c_private_cfg {
        xwu8_t bfdr;
};

#endif /* soc_i2c.h */
