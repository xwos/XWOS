/**
 * @file
 * @brief SOC描述层：EDMA
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

#ifndef __soc_dma_h__
#define __soc_dma_h__

#include <xwos/standard.h>
#include <soc.h>

#if ((defined(SOCCFG_CHIP_MPC5607B)) && (1 == SOCCFG_CHIP_MPC5607B))
  #define SOC_DMAC_CHANNEL_NUM          (16U)
#endif

#define SOC_DMA_TCD_CFG_NULL            (0)

#define SOC_DMA_TCD_SIZE_8BIT           (0)
#define SOC_DMA_TCD_SIZE_16BIT          (1)
#define SOC_DMA_TCD_SIZE_32BIT          (2)
#define SOC_DMA_TCD_SIZE_16BYTE         (4)
#define SOC_DMA_TCD_SIZE_32BYTE         (5)

#define SOC_DMA_TCD_BWC_NONE            (0)
#define SOC_DMA_TCD_BWC_4               (2)
#define SOC_DMA_TCD_BWC_8               (3)

/**
 * @brief soc DMA sources enumerations
 */
enum soc_dma_source_em {
        SOC_DMAMUX_RESERVED0,
        SOC_DMAMUX_DSPI0_TX,
        SOC_DMAMUX_DSPI0_RX,
        SOC_DMAMUX_DSPI1_TX,
        SOC_DMAMUX_DSPI1_RX,
        SOC_DMAMUX_DSPI2_TX,
        SOC_DMAMUX_DSPI2_RX,
        SOC_DMAMUX_DSPI3_TX,
        SOC_DMAMUX_DSPI3_RX,
        SOC_DMAMUX_DSPI4_TX,
        SOC_DMAMUX_DSPI4_RX,
        SOC_DMAMUX_DSPI5_TX,
        SOC_DMAMUX_DSPI5_RX,
        SOC_DMAMUX_RESERVED13,
        SOC_DMAMUX_RESERVED14,
        SOC_DMAMUX_RESERVED15,
        SOC_DMAMUX_RESERVED16,
        SOC_DMAMUX_EMIOS0_CH0,
        SOC_DMAMUX_EMIOS0_CH1,
        SOC_DMAMUX_EMIOS0_CH9,
        SOC_DMAMUX_EMIOS0_CH18,
        SOC_DMAMUX_EMIOS0_CH25,
        SOC_DMAMUX_EMIOS0_CH26,
        SOC_DMAMUX_EMIOS1_CH0,
        SOC_DMAMUX_EMIOS1_CH9,
        SOC_DMAMUX_EMIOS1_CH17,
        SOC_DMAMUX_EMIOS1_CH18,
        SOC_DMAMUX_EMIOS1_CH25,
        SOC_DMAMUX_EMIOS1_CH26,
        SOC_DMAMUX_ADC0_EOC,
        SOC_DMAMUX_ADC1_EOC,
        SOC_DMAMUX_I2C_RX,
        SOC_DMAMUX_I2C_TX,
        SOC_DMAMUX_LINFLEX0_RX,
        SOC_DMAMUX_LINFLEX0_TX,
        SOC_DMAMUX_LINFLEX1_RX,
        SOC_DMAMUX_LINFLEX1_TX,
        SOC_DMAMUX_RESERVED37,
        SOC_DMAMUX_RESERVED38,
        SOC_DMAMUX_RESERVED39,
        SOC_DMAMUX_RESERVED40,
        SOC_DMAMUX_RESERVED41,
        SOC_DMAMUX_RESERVED42,
        SOC_DMAMUX_RESERVED43,
        SOC_DMAMUX_RESERVED44,
        SOC_DMAMUX_RESERVED45,
        SOC_DMAMUX_RESERVED46,
        SOC_DMAMUX_RESERVED47,
        SOC_DMAMUX_RESERVED48,
        SOC_DMAMUX_RESERVED49,
        SOC_DMAMUX_RESERVED50,
        SOC_DMAMUX_RESERVED51,
        SOC_DMAMUX_RESERVED52,
        SOC_DMAMUX_RESERVED53,
        SOC_DMAMUX_RESERVED54,
        SOC_DMAMUX_RESERVED55,
        SOC_DMAMUX_RESERVED56,
        SOC_DMAMUX_RESERVED57,
        SOC_DMAMUX_RESERVED58,
        SOC_DMAMUX_RESERVED59,
        SOC_DMAMUX_ALWAYSEN60,
        SOC_DMAMUX_ALWAYSEN61,
        SOC_DMAMUX_ALWAYSEN62,
        SOC_DMAMUX_ALWAYSEN63,
};

/**
 * @brief soc DMA controller config
 */
struct soc_dmac_private_cfg {
        union {
                struct {
                        xwu32_t reserved0:14;
                        xwu32_t reserved1:1;
                        xwu32_t reserved2:1;
                        xwu32_t reserved3:6;
                        xwu32_t grp0pri:2;
                        xwu32_t emlm:1;
                        xwu32_t clm:1;
                        xwu32_t reserved4:1;
                        xwu32_t hoe:1;
                        xwu32_t erga:1;
                        xwu32_t erca:1;
                        xwu32_t edbg:1;
                        xwu32_t ebw:1;
                } bit;
                xwu32_t u32;
        } cr;
        union {
                struct {
                        xwu8_t ecp:1;
                        xwu8_t dpa:1;
                        xwu8_t reserved:2;
                        xwu8_t chpri:4;
                } bit;
                xwu8_t u8;
        } cpr[SOC_DMAC_CHANNEL_NUM];
};

union soc_dma_tcd {
        struct EDMA_TCD_COM_tag com;
        struct EDMA_TCD_STD_tag std;
        struct EDMA_TCD_MLMIRROR_tag ml;
        struct EDMA_TCD_CHLINK_tag cl;
        struct EDMA_TCD_MLMIRROR_CHLINK_tag mlcl;
};

/**
 * @brief soc DMA channel config
 */
struct soc_dmach_private_cfg {
        union {
                struct {
                        xwu8_t enbl:1;
                        xwu8_t trig:1;
                        xwu8_t src:6;
                } bit;
                xwu8_t u8;
        } dmamux;
        union soc_dma_tcd * tcds;
};

#endif /* soc_dma.h */
