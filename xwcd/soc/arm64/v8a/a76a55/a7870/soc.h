/**
 * @file
 * @brief SOC描述层：A7870寄存器
 * @author
 * + 隐星曜 (Roy Sun) <xwos@xwos.tech>
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

#ifndef __xwcd_soc_arm_v8a_a76a55_a7870_soc_h__
#define __xwcd_soc_arm_v8a_a76a55_a7870_soc_h__

#include <xwos/standard.h>

#define SOC_AON_SML_POWER_MANAGER_PHYS           0x00000000UL
#define SOC_APCPU_GIC600_PHYS                    0x12000000UL
#define SOC_AP_AHBREG_PHYS                       0x20000000UL
#define SOC_AP_CKG_PHYS                          0x20010000UL
#define SOC_AP_DMA_PHYS                          0x20020000UL
#define SOC_AP_APBREG_PHYS                       0x20100000UL
#define SOC_AP_NIC400_PHYS                       0x20150000UL
#define SOC_AP_UART0_PHYS                        0x20200000UL
#define SOC_AP_UART1_PHYS                        0x20210000UL
#define SOC_AP_UART2_PHYS                        0x20220000UL
#define SOC_AP_UART3_PHYS                        0x20230000UL
#define SOC_AP_SPI0_PHYS                         0x20260000UL
#define SOC_AP_SPI1_PHYS                         0x20270000UL
#define SOC_AP_SPI2_PHYS                         0x20280000UL
#define SOC_AP_IIS0_PHYS                         0x202b0000UL
#define SOC_AP_IIS1_PHYS                         0x202c0000UL
#define SOC_AP_IIS2_PHYS                         0x202d0000UL
#define SOC_AP_CE_PUB_PHYS                       0x202e0000UL
#define SOC_AP_CE_SEC_PHYS                       0x202f0000UL
#define SOC_AP_ROM_IP_PHYS                       0x21000000UL
#define SOC_AP_UFS_HCI_PHYS                      0x22000000UL
#define SOC_AP_EMMC_PHYS                         0x22200000UL
#define SOC_AP_SDIO0_PHYS                        0x22210000UL
#define SOC_AP_SDIO1_PHYS                        0x22220000UL
#define SOC_AP_SDIO2_PHYS                        0x22230000UL
#define SOC_AP_I2C0_PHYS                         0x22240000UL
#define SOC_AP_I2C1_PHYS                         0x22250000UL
#define SOC_AP_I2C2_PHYS                         0x22260000UL
#define SOC_AP_I2C3_PHYS                         0x22270000UL
#define SOC_AP_I2C4_PHYS                         0x22280000UL
#define SOC_AP_I2C5_PHYS                         0x22290000UL
#define SOC_AP_I2C6_PHYS                         0x222a0000UL
#define SOC_AP_I2C7_PHYS                         0x222b0000UL
#define SOC_AP_I2C8_PHYS                         0x222c0000UL
#define SOC_AP_I2C9_PHYS                         0x222d0000UL
#define SOC_AP_GPU_GLB_REG_PHYS                  0x23000000UL
#define SOC_AP_GPU_CLK_REG_PHYS                  0x23010000UL
#define SOC_AP_GPU_DVFS_REG_PHYS                 0x23014000UL
#define SOC_AP_GPU_MTX_REG_PHYS                  0x23018000UL
#define SOC_AP_GPU_GPU_IP_CFG_PHYS               0x23100000UL
#define SOC_IPA_APB_RF_WITHIN_FW_PHYS            0x25000000UL
#define SOC_IPA_CLK_PHYS                         0x25010000UL
#define SOC_IPA_USB31_DP_DWC_USB31_DP_DWC_USB31_PHYS 0x25100000UL
#define SOC_IPA_PAM_WIFI_PHYS                    0x25200000UL
#define SOC_IPA_PAM_U3_PHYS                      0x25210000UL
#define SOC_IPA_PHYS                             0x25220000UL
#define SOC_IPA_NR_TFT_PHYS                      0x25230000UL
#define SOC_IPA_GLB_APB_RF_WITHIN_FW_PHYS        0x25240000UL
#define SOC_IPA_AXI_BUSMON_PERF_APB_PAMU3_PHYS   0x25300000UL
#define SOC_IPA_AXI_BUSMON_PERF_APB_PAM_WIFI_PHYS 0x25300100UL
#define SOC_IPA_AXI_BUSMON_PERF_APB_IPA_M0_PHYS  0x25300200UL
#define SOC_IPA_NIC400_IPA_MAIN_MTX_PHYS         0x25303000UL
#define SOC_IPA_USB31_PLL_PHYS                   0x25304000UL
#define SOC_IPA_USB31_DP_DWC_USB31_DP_DWC_USBC31DPTXPHY_AUXPHY_PHYS 0x25310000UL
#define SOC_IPA_IRAM_PHYS                        0x25400000UL
#define SOC_PCIE_PWR_PCIE_GLB_PHYS               0x26000000UL
#define SOC_PCIE_PWR_PCIE_CLK_PHYS               0x26004000UL
#define SOC_PCIE_PWR_NIC400_CFG_PHYS             0x26005000UL
#define SOC_PCIE_PWR_PCIEPLL_V_PHYS              0x26008000UL
#define SOC_PCIE_PWR_PCIE3_PHY_GLB_PHYS          0x2600c000UL
#define SOC_PCIE_PWR_PCIE3_PHY_CR_PHYS           0x26010000UL
#define SOC_PCIE_PWR_PCIE_APB_PHYS               0x26100000UL
#define SOC_AI_APB_REG_PHYS                      0x27000000UL
#define SOC_AI_CLOCK_REG_PHYS                    0x27004000UL
#define SOC_AI_DVFS_REG_PHYS                     0x27008000UL
#define SOC_AI_MTX_REG_PHYS                      0x2700c000UL
#define SOC_AI_AXI_BUSMON_PHYS                   0x27010000UL
#define SOC_AI_POWERVR_PHYS                      0x27100000UL
#define SOC_AI_OCM_PHYS                          0x27200000UL
#define SOC_MM_GLB_PHYS                          0x30000000UL
#define SOC_MM_CLK_PHYS                          0x30010000UL
#define SOC_MM_DVFS_PHYS                         0x30014000UL
#define SOC_MM_MAILBOX_PHYS                      0x30020000UL
#define SOC_MM_UART_PHYS                         0x30030000UL
#define SOC_MM_VDSP_IDLE_PHYS                    0x30038000UL
#define SOC_MM_ISP_BLK_MTX_PHYS                  0x30040000UL
#define SOC_MM_VDSP_BLK_MTX_PHYS                 0x30050000UL
#define SOC_MM_DCAM_BLK_MTX_PHYS                 0x30060000UL
#define SOC_MM_MTX_PHYS                          0x30070000UL
#define SOC_DPU_VSP_APB_REG_PHYS                 0x30100000UL
#define SOC_DPU_VSP_CLK_DPU_VSP_CLK_PHYS         0x30110000UL
#define SOC_DPU_VSP_CLK_DVFS_PHYS                0x30114000UL
#define SOC_DPU_VSP_NIC400_DPU_MTX_PHYS          0x30120000UL
#define SOC_DPU_VSP_NIC400_VPU_GSP_MTX_PHYS      0x30130000UL
#define SOC_DPU_VSP_DISP_PHYS                    0x31000000UL
#define SOC_DPU_VSP_GSP_PHYS                     0x31100000UL
#define SOC_DPU_VSP_GSP_LITE_PHYS                0x31200000UL
#define SOC_DPU_VSP_DISP_DSI0_PHYS               0x31300000UL
#define SOC_DPU_VSP_DISP_DSI1_PHYS               0x31400000UL
#define SOC_IPA_DISPC1_GLB_APB_REG_PHYS          0x31800000UL
#define SOC_IPA_USB31_DP_PHYS                    0x31810000UL
#define SOC_IPA_DPU_LITE_PHYS                    0x31880000UL
#define SOC_IPA_USB31_DP_DWC_USB31_DP_DWC_DPTX_HDCP22_TRNG_DWC_DPTX_PHYS 0x31890000UL
#define SOC_IPA_USB31_DP_DWC_USB31_DP_DWC_DPTX_HDCP22_TRNG_DWC_HDCP_HDMI_DP_PHYS 0x31900000UL
#define SOC_IPA_USB31_DP_DWC_USB31_DP_DWC_DPTX_HDCP22_TRNG_DWC_TRNG_PHYS 0x31940000UL
#define SOC_IPA_NIC400_DPU_LITE_MTX_PHYS         0x31980000UL
#define SOC_DPU_VSP_VPU_ENC_DUAL_CORE_VENC_CORE0_PHYS 0x32000000UL
#define SOC_DPU_VSP_VPU_ENC_DUAL_CORE_VENC_CORE1_PHYS 0x32100000UL
#define SOC_DPU_VSP_VPU_DEC_PHYS                 0x32200000UL
#define SOC_MM_VDSP_S_PHYS                       0x34000000UL
#define SOC_MM_VDMA_PHYS                         0x34100000UL
#define SOC_MM_DVAU_PHYS                         0x34300000UL
#define SOC_MM_IDVAU_PHYS                        0x34400000UL
#define SOC_MM_JPG_PHYS                          0x36000000UL
#define SOC_MM_CPP_PHYS                          0x38000000UL
#define SOC_MM_ISP_PHYS                          0x3a000000UL
#define SOC_MM_CSI_SWITCH_PHYS                   0x3b600000UL
#define SOC_MM_FD_PHYS                           0x3c000000UL
#define SOC_MM_DEPTH_PHYS                        0x3d000000UL
#define SOC_MM_DCAM_IF_PHYS                      0x3e000000UL
#define SOC_MM_DCAM_IF_LITE_PHYS                 0x3e100000UL
#define SOC_MM_CSI0_PHYS                         0x3e200000UL
#define SOC_MM_CSI1_PHYS                         0x3e300000UL
#define SOC_MM_CSI2_PHYS                         0x3e400000UL
#define SOC_MM_CSI3_PHYS                         0x3e500000UL
#define SOC_PHY_CP_LLRAM_PHYS                    0x41000000UL
#define SOC_PUB_TO_DDRC_AXI6_PHYS                0x41200000UL
#define SOC_PS_CP_CR8_ACP_PHYS                   0x54100000UL
#define SOC_CP_AUD_DMEM_PHYS                     0x56000000UL
#define SOC_CP_AUD_PMEM_PHYS                     0x56060000UL
#define SOC_CP_AUD_AUD_CP_GLB_REG_PHYS           0x56200000UL
#define SOC_CP_AUD_AUD_CP_CLK_PHYS               0x56240000UL
#define SOC_CP_AUD_AUD_CP_DVFS_APB_PHYS          0x56250000UL
#define SOC_CP_AUD_NIC400_AUD_CP_MTX_PHYS        0x56254000UL
#define SOC_CP_AUD_UART_PHYS                     0x56270000UL
#define SOC_CP_AUD_IIS0_PHYS                     0x56270800UL
#define SOC_CP_AUD_IIS1_PHYS                     0x56270900UL
#define SOC_CP_AUD_IIS2_PHYS                     0x56270a00UL
#define SOC_CP_AUD_CEVAX_TIMER0_PHYS             0x56270c00UL
#define SOC_CP_AUD_CEVAX_TIMER1_PHYS             0x56270c80
#define SOC_CP_AUD_WDG16_PHYS                    0x56271000UL
#define SOC_CP_AUD_CEVAX_ICU_PHYS                0x56272800UL
#define SOC_CP_AUD_VBC_V4_LITE_VBC_PHYS          0x56300000UL
#define SOC_CP_AUD_MCDT_AUD_PHYS                 0x56340000UL
#define SOC_CP_AUD_DSP_SRC48K_PHYS               0x56350000UL
#define SOC_CP_AUD_4AD_PHYS                      0x56360000UL
#define SOC_CP_AUD_PDM_PHYS                      0x56370000UL
#define SOC_CP_AUD_VAD_2STG_PHYS                 0x56380000UL
#define SOC_CP_AUD_AUD_CP_AON_APB_PHYS           0x56390000UL
#define SOC_CP_AUD_AON_CLK_PHYS                  0x56394000UL
#define SOC_CP_AUD_TDM_PHYS                      0x563a0000UL
#define SOC_CP_AUD_MCDT_AP_PHYS                  0x56500000UL
#define SOC_CP_AUD_VBC_V4_LITE_OFLD_PHYS         0x56510000UL
#define SOC_CP_AUD_DMA_64B_AP_PHYS               0x56650000UL
#define SOC_CP_AUD_DMA_64B_CP_PHYS               0x56660000UL
#define SOC_CP_AUD_SPINLOCK_PHYS                 0x56680000UL
#define SOC_CP_AUD_AUDCP_IRAM_PHYS               0x56800000UL
#define SOC_CP_AUD_AUD_CP_IRAM_RET_PHYS          0x56820000UL
#define SOC_PUB_DMC_PHYS                         0x60000000UL
#define SOC_PUB_DMC_PHY_AHB_PHYS                 0x60001000UL
#define SOC_PUB_DMC_PHY1_AHB_PHYS                0x60002000UL
#define SOC_PUB_DFI_BUSMON_REGCTRL_PHYS          0x60010000UL
#define SOC_PUB_AHB_REG_PHYS                     0x60030000UL
#define SOC_PUB_APB_REG_PHYS                     0x60050000UL
#define SOC_PUB_PERF_TRACE_MONITOR_PHYS          0x60060000UL
#define SOC_PUB_DMC_BIST_2P_PHYS                 0x60070000UL
#define SOC_PUB_NIC400_PUB_DCAM_DPU_MERGE_MTX_PHYS 0x60080000UL
#define SOC_PUB_NIC400_PUB_MM_MERGE_MTX_PHYS     0x60081000UL
#define SOC_PUB_NIC400_PUB_AP_AON_ISE_IPA_MERGE_MTX_PHYS 0x60082000UL
#define SOC_PUB_NIC400_PUB_CP_ACC_MERGE_MTX_PHYS 0x60083000UL
#define SOC_PUB_NIC400_PUB_CPCPU_MERGE_MTX_PHYS  0x60084000UL
#define SOC_PUB_NIC400_PUB_CROSS_MTX_PHYS        0x60085000UL
#define SOC_PUB_DVFS_APB_PHYS                    0x60090000UL
#define SOC_PUB_DVFS_BWMON_APB_PHYS              0x60094000UL
#define SOC_PUB_DVFS_LATMON_APB_PHYS             0x60098000UL
#define SOC_CH_DMA_REG_PHYS                      0x62100000UL
#define SOC_CH_AHB_REG_PHYS                      0x62110000UL
#define SOC_CH_TMR_PHYS                          0x62200000UL
#define SOC_CH_SYST_PHYS                         0x62210000UL
#define SOC_CH_WDG_PHYS                          0x62220000UL
#define SOC_CH_UART0_PHYS                        0x62230000UL
#define SOC_CH_UART1_PHYS                        0x62240000UL
#define SOC_CH_UART2_PHYS                        0x62250000UL
#define SOC_CH_GPIO_PHYS                         0x62270000UL
#define SOC_CH_I3C0_PHYS                         0x62280000UL
#define SOC_CH_I3C1_PHYS                         0x62290000UL
#define SOC_CH_SPI_PHYS                          0x622a0000UL
#define SOC_CH_EIC_GPIO_PHYS                     0x622b0000UL
#define SOC_CH_SENSOR_TS_PHYS                    0x622c0000UL
#define SOC_SP_RAM_PHYS                          0x62800000UL
#define SOC_SP_AHB_REG_PHYS                      0x62910000UL
#define SOC_SP_TMR_PHYS                          0x62a00000UL
#define SOC_SP_SYST_PHYS                         0x62a10000UL
#define SOC_SP_WDG_PHYS                          0x62a20000UL
#define SOC_SP_EIC_DEEP_PHYS                     0x62ab0000UL
#define SOC_SP_EIC_LIGHT_PHYS                    0x62ac0000UL
#define SOC_AON_PSCP_WDG_PHYS                    0x64000000UL
#define SOC_AON_PSCP_SYST_PHYS                   0x64010000UL
#define SOC_AON_PSCP_TMR_PHYS                    0x64020000UL
#define SOC_AON_PWM0_PHYS                        0x64030000UL
#define SOC_AON_PWM1_PHYS                        0x64034000UL
#define SOC_AON_PWM2_PHYS                        0x64038000UL
#define SOC_AON_PWM3_PHYS                        0x6403c000UL
#define SOC_AON_TMR_PHYS                         0x64040000UL
#define SOC_AON_FRT_PHYS                         0x64050000UL
#define SOC_AON_SCC_PHYS                         0x640b0000UL
#define SOC_AON_LP_AP_INTC0_PHYS                 0x640c0000UL
#define SOC_AON_LP_AP_INTC1_PHYS                 0x640d0000UL
#define SOC_AON_LP_AP_INTC2_PHYS                 0x640e0000UL
#define SOC_AON_LP_AP_INTC3_PHYS                 0x640f0000UL
#define SOC_AON_LP_AP_INTC4_PHYS                 0x64100000UL
#define SOC_AON_LP_AP_INTC5_PHYS                 0x64110000UL
#define SOC_AON_LP_PSCP_INTC_PHYS                0x64120000UL
#define SOC_AON_LP_PHYCP_INTC_PHYS               0x64130000UL
#define SOC_AON_LP_AUDCP_INTC_PHYS               0x64140000UL
#define SOC_AON_EFUSE_PHYS                       0x64160000UL
#define SOC_AON_GPIO_PHYS                        0x64170000UL
#define SOC_AON_AP_TMR0_PHYS                     0x64180000UL
#define SOC_AON_AP_TMR1_PHYS                     0x64190000UL
#define SOC_AON_AP_TMR2_PHYS                     0x641a0000UL
#define SOC_AON_KPD_PHYS                         0x641b0000UL
#define SOC_AON_AP_SYST_PHYS                     0x641c0000UL
#define SOC_AON_APCPU_WDG_PHYS                   0x641e0000UL
#define SOC_AON_IIS_MST_PHYS                     0x641f0000UL
#define SOC_AON_EIC0_PHYS                        0x64200000UL
#define SOC_AON_EIC1_PHYS                        0x64210000UL
#define SOC_AON_EIC2_PHYS                        0x64220000UL
#define SOC_AON_EIC3_PHYS                        0x64230000UL
#define SOC_AON_EIC4_PHYS                        0x64240000UL
#define SOC_AON_EIC5_PHYS                        0x64250000UL
#define SOC_AON_CPALL_EIC_PHYS                   0x64260000UL
#define SOC_AON_THM0_PHYS                        0x64270000UL
#define SOC_AON_THM1_PHYS                        0x64280000UL
#define SOC_AON_THM2_PHYS                        0x64290000UL
#define SOC_AON_THM3_PHYS                        0x642a0000UL
#define SOC_AON_BUSMON_CSTMR_PUB0_PHYS           0x642b0000UL
#define SOC_AON_BUSMON_CSTMR_PUB1_PHYS           0x642c0000UL
#define SOC_AON_BUSMON_CSTMR_IPA_PHYS            0x642d0000UL
#define SOC_AON_PIN_REG_PHYS                     0x642e0000UL
#define SOC_AON_ANLG_PHY_G0L_PHYS                0x64300000UL
#define SOC_AON_ANLG_PHY_G1_PHYS                 0x64304000UL
#define SOC_AON_ANLG_PHY_G1L_PHYS                0x64308000UL
#define SOC_AON_ANLG_PHY_G2_PHYS                 0x6430c000UL
#define SOC_AON_ANLG_PHY_G2M_PHYS                0x64310000UL
#define SOC_AON_ANLG_PHY_G3_PHYS                 0x64314000UL
#define SOC_AON_ANLG_PHY_G4_PHYS                 0x64318000UL
#define SOC_AON_ANLG_PHY_G5_PHYS                 0x6431c000UL
#define SOC_AON_ANLG_PHY_G5R_PHYS                0x64320000UL
#define SOC_AON_ANLG_PHY_G5L_PHYS                0x64324000UL
#define SOC_AON_ANLG_PHY_G8_PHYS                 0x6432c000UL
#define SOC_AON_ANLG_PHY_G9_PHYS                 0x64330000UL
#define SOC_AON_ANLG_PHY_G10_PHYS                0x64334000UL
#define SOC_AON_ANLG_PHY_G11_PHYS                0x64338000UL
#define SOC_AON_ANLG_PHY_G13_PHYS                0x64340000UL
#define SOC_AON_ANLG_PHY_G1M_PHYS                0x64344000UL
#define SOC_AON_ANLG_PHY_G2L_PHYS                0x64348000UL
#define SOC_AON_ANLG_PHY_G4L_PHYS                0x6434c000UL
#define SOC_AON_DDC_CTRL_PHYS                    0x64350000UL
#define SOC_AON_DDC_ANALOG_PHYS                  0x64358000UL
#define SOC_AON_ANLG_PHY_G12_PHYS                0x64380000UL
#define SOC_AON_ANLG_PHY_G12_CR_PHYS             0x64390000UL
#define SOC_AON_ADI_PHYS                         0x64400000UL
#define SOC_AON_ISE_PHYS                         0x64500000UL
#define SOC_AON_RFTI_PHYS                        0x64510000UL
#define SOC_AON_SERDES_CTRL0_PHYS                0x64520000UL
#define SOC_AON_SERDES_CTRL1_PHYS                0x64530000UL
#define SOC_AON_CHIP_RESET_CTRL_PHYS             0x64550000UL
#define SOC_AON_APCPU_TS_PHYS                    0x64560000UL
#define SOC_AON_DJTAG_PHYS                       0x64570000UL
#define SOC_AON_APB_BUSMON_PHYS                  0x64580000UL
#define SOC_AON_AP_WDG_PHYS                      0x64590000UL
#define SOC_AON_MATRIX_AXI_REGU_PHYS             0x645a0000UL
#define SOC_AON_LP_AP_INTC6_PHYS                 0x645b0000UL
#define SOC_AON_LP_AP_INTC7_PHYS                 0x645c0000UL
#define SOC_AON_BB_SW_RFSPI_MST_PHYS             0x645d0000UL
#define SOC_AON_RTM_PHYS                         0x645e0000UL
#define SOC_AON_SW_RFFE_PHYS                     0x645f0000UL
#define SOC_AON_MBOX_PHYS                        0x64600000UL
#define SOC_AON_BUSMON_CSTMR_PUB2_PHYS           0x64640000UL
#define SOC_AON_SEC_TZPC_PHYS                    0x64800000UL
#define SOC_AON_SEC_REG_PHYS                     0x64820000UL
#define SOC_AON_SEC_TMR_PHYS                     0x64830000UL
#define SOC_AON_SEC_RTC_PHYS                     0x64840000UL
#define SOC_AON_SEC_WDG_PHYS                     0x64850000UL
#define SOC_AON_SEC_GPIO_PHYS                    0x64860000UL
#define SOC_AON_SEC_EIC_PHYS                     0x64870000UL
#define SOC_AON_SEC_CLK32K_DET_PHYS              0x64880000UL
#define SOC_AON_SEC_DBG_REG_PHYS                 0x64890000UL
#define SOC_AON_APB_REG_PHYS                     0x64900000UL
#define SOC_AON_PMU_PHYS                         0x64910000UL
#define SOC_AON_CLK_PHYS                         0x64920000UL
#define SOC_AON_PRE_DIV_PHYS                     0x64930000UL
#define SOC_AON_DVFS_PHYS                        0x64940000UL
#define SOC_AON_APCPU_DVFS_PHYS                  0x64950000UL
#define SOC_AON_USB2_PHYS                        0x64a00000UL
#define SOC_AON_SPINLOCK_PHYS                    0x64a10000UL
#define SOC_AON_RAM_RESERVED0_PHYS               0x65000800UL
#define SOC_AON_DDR_RETENTION_PHYS               0x65002000UL
#define SOC_AON_PSCP_CR8_BOOT_PHYS               0x65003400UL
#define SOC_AON_PHYCP_CR8_BOOT_PHYS              0x65004400UL
#define SOC_AON_AUDIO_DSP_BOOT_PHYS              0x65005400UL
#define SOC_AON_AUDIO_DSP_SHARE_PHYS             0x65006400UL
#define SOC_AON_SMSG_PHYS                        0x65007400UL
#define SOC_AON_PUB_DEBUGING_PHYS                0x65009400UL
#define SOC_AON_PUB_EFUSE_REPAIRE_PHYS           0x6500a400UL
#define SOC_AON_RAM_RESERVED1_PHYS               0x6500a800UL
#define SOC_AON_SECURITY_FW_BACK_PHYS            0x65015000UL
#define SOC_CH_RAM_PHYS                          0x65016000UL
#define SOC_DBG_STM_PHYS                         0x70000000UL
#define SOC_DBG_CXSTM_DD16_CD8_ID16_HWE_SPRD_0_PHYS 0x78001000UL
#define SOC_DBG_CXATBFUNNEL_X8_64B_SPRD_0_PHYS   0x78002000UL
#define SOC_DBG_CXTMC_ETF_32K_64B_SPRD_0_PHYS    0x78003000UL
#define SOC_DBG_CXATBREPLICATOR_64B_SPRD_0_PHYS  0x78004000UL
#define SOC_DBG_CXTMC_ETR_64B_D8_SPRD_0_PHYS     0x78005000UL
#define SOC_DBG_CXTPIU_0_PHYS                    0x78006000UL
#define SOC_DBG_CXCTI_0_PHYS                     0x78007000UL
#define SOC_DBG_CXCTI_1_PHYS                     0x78008000UL
#define SOC_DBG_CXAPBASYNCBRIDGE_SPRD_0_PHYS     0x78009000UL
#define SOC_DBG_SLV0_4K_DBG_PHYS                 0x7800a000UL
#define SOC_DBG_CXCTI2_PHYS                      0x7800b000UL
#define SOC_DBG_CXCTI3_PHYS                      0x7800c000UL
#define SOC_DBG_PSCP_DBG_APB_PHYS                0x78100000UL
#define SOC_DBG_PHYCP_DBG_APB_PHYS               0x78300000UL
#define SOC_DBG_APCPU_DBG_APB_PHYS               0x7a000000UL
#define SOC_PUB_TO_DDRC_AXI0_PHYS                0x80000000UL
#define SOC_DDR_ORIGIN                           SOC_PUB_TO_DDRC_AXI0_PHYS
#define SOC_PCIE_PCIE_AXIS_PHYS                  0x880000000UL

#define SOC_GIC3_REGBASE SOC_APCPU_GIC600_PHYS
#define SOC_GIC3_D_OFFSET 0x00000UL
#define SOC_GIC3_D_SIZE 0x10000UL
#define SOC_GIC3_A_OFFSET 0x10000UL
#define SOC_GIC3_A_SIZE 0x10000UL
#define SOC_GIC3_T_OFFSET 0x20000UL
#define SOC_GIC3_T_SIZE 0x10000UL
#define SOC_GIC3_P_OFFSET 0x30000UL
#define SOC_GIC3_P_SIZE 0x10000UL
#define SOC_GIC3_ITS_OFFSET 0x40000UL
#define SOC_GIC3_ITS_SIZE ((ARCHCFG_CICV3_ITSNUM) * 0x20000UL)
#define SOC_GIC3_ITS(n) (SOC_GIC3_ITS_OFFSET + ((n) * 0x20000UL))
#define SOC_GIC3_R_OFFSET (0x40000UL + SOC_GIC3_ITS_SIZE)
#define SOC_GIC3_R_SIZE ((ARCHCFG_CICV3_RDNUM) * 0x20000UL)
#define SOC_GIC3_R_LPI_OFFSET(n) (SOC_GIC3_R_OFFSET + ((n) * 0x20000UL))
#define SOC_GIC3_R_SGI_OFFSET(n) (SOC_GIC3_R_OFFSET + 0x10000 + ((n) * 0x20000UL))
#define SOC_GIC3_DA_OFFSET (SOC_GIC3_R_OFFSET + SOC_GIC3_R_SIZE)
#define SOC_GIC3_DA_SIZE 0x10000UL
#define SOC_GIC3_ITST_OFFSET 0x50000UL
#define SOC_GIC3_ITST_SIZE (0x10000UL * (ARCHCFG_CICV3_ITSNUM) * 2UL)
#define SOC_GIC3_ITST(n) (SOC_GIC3_ITST_OFFSET + (0x10000UL * (n) * 2UL))

#define SOC_MPIDR(cpuid) (0x81000000UL | (((cpuid) << 8UL) && 0xFF00UL))

#endif /* xwcd/soc/arm64/v8a/a76a55/a7870/soc.h */
