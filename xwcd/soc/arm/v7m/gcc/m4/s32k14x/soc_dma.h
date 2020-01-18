/**
 * @file
 * @brief S32K14x EDMA
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

#ifndef __soc_dma_h__
#define __soc_dma_h__

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <soc.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      macros       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define SOC_DMAC_CHANNEL_NUM            (16U)

#define SOC_DMA_TCD_CFG_NULL            (0)

#define SOC_DMA_TCD_SIZE_8BIT           (0)
#define SOC_DMA_TCD_SIZE_16BIT          (1)
#define SOC_DMA_TCD_SIZE_32BIT          (2)
#define SOC_DMA_TCD_SIZE_16BYTE         (4)
#define SOC_DMA_TCD_SIZE_32BYTE         (5)

#define SOC_DMA_TCD_BWC_NONE            (0)
#define SOC_DMA_TCD_BWC_4               (2)
#define SOC_DMA_TCD_BWC_8               (3)

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief soc DMA sources enumerations
 */
enum soc_dma_source_em {
        SOC_DMAMUX_RESERVED0,
        SOC_DMAMUX_ENET,
        SOC_DMAMUX_LPUART0_RX,
        SOC_DMAMUX_LPUART0_TX,
        SOC_DMAMUX_LPUART1_RX,
        SOC_DMAMUX_LPUART1_TX,
        SOC_DMAMUX_LPUART2_RX,
        SOC_DMAMUX_LPUART2_TX,
        SOC_DMAMUX_LPI2C1_RX,
        SOC_DMAMUX_LPI2C1_TX,
        SOC_DMAMUX_FlexIO_Shifter0,
        SOC_DMAMUX_FlexIO_Shifter1,
        SOC_DMAMUX_FlexIO_Shifter2_SAI1_RX,
        SOC_DMAMUX_FlexIO_Shifter3_SAI1_TX,
        SOC_DMAMUX_LPSPI0_RX,
        SOC_DMAMUX_LPSPI0_TX,
        SOC_DMAMUX_LPSPI1_RX,
        SOC_DMAMUX_LPSPI1_TX,
        SOC_DMAMUX_LPSPI2_RX,
        SOC_DMAMUX_LPSPI2_TX,
        SOC_DMAMUX_FTM1_CH0,
        SOC_DMAMUX_FTM1_CH1,
        SOC_DMAMUX_FTM1_CH2,
        SOC_DMAMUX_FTM1_CH3,
        SOC_DMAMUX_FTM1_CH4,
        SOC_DMAMUX_FTM1_CH5,
        SOC_DMAMUX_FTM1_CH6,
        SOC_DMAMUX_FTM1_CH7,
        SOC_DMAMUX_FTM2_CH0,
        SOC_DMAMUX_FTM2_CH1,
        SOC_DMAMUX_FTM2_CH2,
        SOC_DMAMUX_FTM2_CH3,
        SOC_DMAMUX_FTM2_CH4,
        SOC_DMAMUX_FTM2_CH5,
        SOC_DMAMUX_FTM2_CH6,
        SOC_DMAMUX_FTM2_CH7,
        SOC_DMAMUX_FTM0,
        SOC_DMAMUX_FTM3,
        SOC_DMAMUX_FTM4,
        SOC_DMAMUX_FTM5,
        SOC_DMAMUX_FTM6,
        SOC_DMAMUX_FTM7,
        SOC_DMAMUX_ADC0,
        SOC_DMAMUX_ADC1,
        SOC_DMAMUX_LPI2C0_RX,
        SOC_DMAMUX_LPI2C0_TX,
        SOC_DMAMUX_PDB0,
        SOC_DMAMUX_PDB1,
        SOC_DMAMUX_CMP0,
        SOC_DMAMUX_PORTA,
        SOC_DMAMUX_PORTB,
        SOC_DMAMUX_PORTC,
        SOC_DMAMUX_PORTD,
        SOC_DMAMUX_PORTE,
        SOC_DMAMUX_FlexCAN0,
        SOC_DMAMUX_FlexCAN1,
        SOC_DMAMUX_FlexCAN2,
        SOC_DMAMUX_SAI0_RX,
        SOC_DMAMUX_SAI0_TX,
        SOC_DMAMUX_LPTMR0,
        SOC_DMAMUX_QuadSPI_RX,
        SOC_DMAMUX_QuadSPI_TX,
        SOC_DMAMUX_ALWAYSEN62,
        SOC_DMAMUX_ALWAYSEN63,
        SOC_DMAMUX_NUM,
};

/**
 * @brief soc DMA controller config
 */
struct soc_dmac_private_cfg {
        union {
                struct {
                        xwu32_t reserved0:1;
                        xwu32_t edbg:1;
                        xwu32_t erca:1;
                        xwu32_t reserved3:1;
                        xwu32_t hoe:1;
                        xwu32_t reserved5:1;
                        xwu32_t clm:1;
                        xwu32_t emlm:1;
                        xwu32_t reserved8_31:24;
                } bit;
                xwu32_t u32;
        } cr;
        union {
                struct {
                        xwu8_t chpri:4;
                        xwu8_t reserved4_5:2;
                        xwu8_t dpa:1;
                        xwu8_t ecp:1;
                } bit;
                xwu8_t u8;
        } cpr[SOC_DMAC_CHANNEL_NUM];
};

struct soc_dma_tcd {
        xwu32_t SADDR; /**< TCD Source Address, array offset: 0x00 */
        xwu16_t SOFF; /**< TCD Signed Source Address Offset, array offset: 0x04 */
        union {
                xwu16_t HW;
                struct {
                        xwu16_t DSIZE:3;
                        xwu16_t DMOD:5;
                        xwu16_t SSIZE:3;
                        xwu16_t SMOD:5;
                } ATTR;
        } W1_H; /**< TCD Transfer Attributes, array offset: 0x06 */
        union {
                struct {
                        xwu32_t NBYTES;
                } EMLM_0; /**< CR[EMLM] = 0; */
                union {
                        struct {
                                xwu32_t NBYTES:10;
                                xwu32_t MLOFF:20;
                                xwu32_t DMLOE:1; /* DMLOE = 1: Destination Minor Loop Offset enable */
                                xwu32_t SMLOE:1; /* SMLOE = 1: Source Minor Loop Offset enable. */
                        } S1;
                        struct {
                                xwu32_t NBYTES:30;
                                xwu32_t DMLOE:1; /* DMLOE = 0: Destination Minor Loop Offset disable */
                                xwu32_t SMLOE:1; /* SMLOE = 0: Source Minor Loop Offset disable. */
                        } S2;
                } EMLM_1; /**< CR[EMLM] = 1; */
        } W2; /* offset: 0x08 */
        xwu32_t SLAST; /**< TCD Last Source Address Adjustment, array offset: 0x0C */
        xwu32_t DADDR; /**< TCD Destination Address, array offset: 0x10*/
        xwu16_t DOFF; /**< TCD Signed Destination Address Offset, array offset: 0x14 */
        union {
                struct {
                        xwu16_t CITER:15;
                        xwu16_t ELINK:1; /**< ELINK = 0; */
                } ELINK_0;
                struct {
                        xwu16_t CITER:9;
                        xwu16_t LINKCH:4;
                        xwu16_t RESERVED0:2;
                        xwu16_t ELINK:1; /**< ELINK = 1; */
                } ELINK_1;
        } W5_H; /* offset: 0x16 */
        xwu32_t DLASTSGA; /**< TCD Last Destination Address Adjustment/Scatter Gather Address, array offset: 0x18 */
        union {
                xwu16_t HW;
                struct {
                        xwu16_t START:1;
                        xwu16_t INT_MAJ:1;
                        xwu16_t INT_HALF:1;
                        xwu16_t D_REQ:1;
                        xwu16_t E_SG:1;
                        xwu16_t MAJORE_LINK:1;
                        xwu16_t ACTIVE:1;
                        xwu16_t DONE:1;
                        xwu16_t MAJORLINKCH:4;
                        xwu16_t RESERVED0:2;
                        xwu16_t BWC:2;
                } CSR;
        } W7_L; /**< TCD Control and Status, array offset: 0x1C */
        union {
                struct {
                        xwu16_t BITER:15;
                        xwu16_t ELINK:1; /**< ELINK = 0; */
                } ELINK_0;
                struct {
                        xwu16_t BITER:9;
                        xwu16_t LINKCH:4;
                        xwu16_t RESERVED0:2;
                        xwu16_t ELINK:1; /**< ELINK = 1; */
                } ELINK_1;
        } W7_H; /* offset: 0x1E */
};

/**
 * @brief soc DMA channel config
 */
struct soc_dmach_private_cfg {
        union {
                struct {
                        xwu8_t src:6;
                        xwu8_t trig:1;
                        xwu8_t enbl:1;
                } bit;
                xwu8_t u8;
        } dmamux;
        struct soc_dma_tcd * tcd;
};
/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********     registers     ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

#endif /* soc_dma.h */
