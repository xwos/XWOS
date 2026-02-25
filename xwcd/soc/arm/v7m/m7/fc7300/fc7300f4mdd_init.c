/**
 * @file
 * @brief SOC描述层：FC7300F4MDD初始化
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

#include <xwcd/soc/arm/v7m/m7/fc7300/soc_init.h>
#include <xwos/ospl/skd.h>
#include <xwcd/soc/arm/v7m/armv7m_isa.h>

/******** SOC Reg ********/
/* DWT addresses and offsets */
#define DWT_CYCCNT_ADDR                         0xE0001004U

/* DEMCR addresses and offsets */
#define DEMCR_ADDR                              0xE000EDFCU
#define DEMCR_TRCENA                            (1U << 24U)  /* Bit 24: Global enable for all DWT and ITM features */

/* SCM addresses and offsets */
#define SCM_BASE_ADDR                           0x40072000U
#define SCM_MAMECCEN0_OFFSET                    0x0020U
#define SCM_MAMECCEN1_OFFSET                    0x0024U
#define SCM_CPU0ECCEN_OFFSET                    0x0028U
#define SCM_CPU1ECCEN_OFFSET                    0x0030U

/* RGM addresses and offsets */
#define RGM_BASE_ADDR                           0x40046000U
#define RGM_SRS_OFFSET                          0x0008U /* RGM System Reset Status Register */
#define RGM_SSRS_OFFSET                         0x0018U /* RGM Sticky Reset Status Register */

#define RGM_SRS_SYSRST_TOUT                     (1U << 31U) /* Bit 31:  System Reset Request Timeout Reset */
#define RGM_SRS_PINRST_TOUT                     (1U << 30U) /* Bit 30:  Pin Reset Request Timeout Reset */
#define RGM_SRS_FSM_ERR                         (1U << 29U) /* Bit 29:  FSM Error Reset */
#define RGM_SRS_LBIST                           (1U << 15U) /* Bit 15:  Logic BIST Reset */
#define RGM_SRS_CMU                             (1U << 14U) /* Bit 14:  CMU Reset */
#define RGM_SRS_SACKERR                         (1U << 13U) /* Bit 13:  Stop Acknowledge Error */
#define RGM_SRS_SYSAP                           (1U << 11U) /* Bit 11:  SYSAP System Reset */
#define RGM_SRS_JTAG                            (1U << 8U)  /* Bit 8 :  JTAG generated reset */
#define RGM_SRS_POR                             (1U << 7U)  /* Bit 7 :  Power-On Reset */
#define RGM_SRS_PIN                             (1U << 6U)  /* Bit 6 :  Pin Reset */
#define RGM_SRS_HSM_WDG                         (1U << 5U)  /* Bit 5 :  HSM Watchdog Reset */
#define RGM_SRS_FCSMU                           (1U << 4U)  /* Bit 4 :  FCSMU Reset */
#define RGM_SRS_CLKERR0                         (1U << 3U)  /* Bit 3 :  Clock Error0 Reset */
#define RGM_SRS_CLKERR1                         (1U << 2U)  /* Bit 2 :  Clock Error1 Reset */
#define RGM_SRS_LVR                             (1U << 1U)  /* Bit 1 :  Low Voltage Detect Reset */
#define RGM_SRS_WAKEUP                          (1U << 0U)  /* Bit 0 :  WAKEUP Reset */

/* SMC addresses and offsets */
#define SMC_BASE_ADDR                           0x40045000U

#define SMC_PMCTRL_OFFSET                       0x000CU
#define SMC_STANDBY_CFG_OFFSET                  0x0010U

#define SMC_STANDBY_CFG_OPTION_MASK             0x3U

/* WDOG */
#define WDOG0_BASE_ADDR                         0x40022000U
#define WDOG1_BASE_ADDR                         0x40433000U
#define WDOG2_BASE_ADDR                         0x40434000U

#define WDOG_CS_OFFSET                          0x0000U /* Watchdog Control and Status Register */
#define WDOG_COUNTER_OFFSET                     0x0004U /* Watchdog Counter Register */
#define WDOG_TIMEOUT_OFFSET                     0x0008U /* Watchdog Timeout Value Register */
#define WDOG_WINDOW_OFFSET                      0x000CU /* Watchdog Window Register */

#define WDOG_CS_UPDATE                          (1U << 5U) /* Bit 5:  Allow updates */
#define WDOG_CS_CLK_SEL_SHIFT                   (8U) /* Bits 8-9: Watchdog Clock */
#define WDOG_CS_CLK_SEL_MASK                    (3U << WDOG_CS_CLK_SEL_SHIFT)
#define WDOG_CS_CLK_SEL_BUS_CLK                 (0U << WDOG_CS_CLK_SEL_SHIFT) /* Bus clock */
#define WDOG_CS_CLK_SEL_AON_CLK                 (1U << WDOG_CS_CLK_SEL_SHIFT) /* AON clock */
#define WDOG_CS_CLK_SEL_SOSC_CLK                (2U << WDOG_CS_CLK_SEL_SHIFT) /* SOSC clock */
#define WDOG_CS_CLK_SEL_SIRC_CLK                (3U << WDOG_CS_CLK_SEL_SHIFT) /* SIRC clock */
#define WDOG_CS_RECFG_STAT                      (1U << 10U) /* Bit 10: Reconfiguration Success */
#define WDOG_CS_ULK_STAT                        (1U << 11U) /* Bit 11: Unlock status */
#define WDOG_CS_PRESCALER_SHIFT                 (12U) /* Bit 12: Watchdog 256 prescale enable/disable */
#define WDOG_CS_PRESCALER_MASK                  (1U << WDOG_CS_PRESCALER_SHIFT)
#define WDOG_CS_PRESCALER_ENABLE                (1U << WDOG_CS_PRESCALER_SHIFT) /* Bit 12: Watchdog prescalr */
#define WDOG_COUNTER_UNLOCK                     (0x08181982U) /* Value to unlock the watchdog registers */
#define WDOG_CS_DISABLE_WDOG                    (WDOG_CS_UPDATE | WDOG_CS_CLK_SEL_AON_CLK | \
                                                 WDOG_CS_PRESCALER_ENABLE | WDOG_CS_ULK_STAT)

/* STCU */
#define PCC_STCU_ADDR                           0x400241FCU
#define STCU_BASE_ADDR                          0x4007F000U

#define STCU_SRAM_INI_CTRL_OFFSET               0x0048U /* SRAM Initialization Control Register */
#define STCU_SRAM_INI_STATUS_OFFSET             0x004CU /* SRAM Initialization Status Register */
#define STCU_SRAM_INI_SEL_OFFSET                0x0050U /* SRAM Initialization Select Register */
#define STCU_SRAM_INI_DONE_STATUS_OFFSET        0x0054U /* SRAM Initialization Done Status Register */

#define STCU_SRAM_INI_CTRL_EN                   (1U << 0U)
#define STCU_SRAM_INI_CTRL_LOCK                 (1U << 1U)
#define STCU_SRAM_INI_CTRL_MODE_MASK            (0x30000U)
#define STCU_SRAM_INI_CTRL_MODE_SHIFT           (16U)
#define STCU_SRAM_INI_CTRL_MODE_WIDTH           (2U)
#define STCU_SRAM_INI_CTRL_MODE(x)              (((x) << STCU_SRAM_INI_CTRL_MODE_SHIFT) & \
                                                 STCU_SRAM_INI_CTRL_MODE_MASK)
#define STCU_SRAM_INI_CTRL_MODE_POR_STANDBY0    (0x0U)
#define STCU_SRAM_INI_CTRL_MODE_STANDBY1        (0x10000U)
#define STCU_SRAM_INI_CTRL_MODE_STANDBY2        (0x20000U)
#define STCU_SRAM_INI_CTRL_MODE_STANDBY3        (0x30000U)

#define STCU_SRAM_INI_STATUS_DONE               (1U << 0U)
#define STCU_SRAM_INI_STATUS_BUSY               (1U << 1U)
#define STCU_SRAM_INI_STATUS_ABORT              (1U << 2U)

#define STCU_SRAM_INI_SEL_SRAM0                 (1U << 0U)
#define STCU_SRAM_INI_SEL_SRAM1                 (1U << 1U)
#define STCU_SRAM_INI_SEL_SRAM2_1               (1U << 2U)
#define STCU_SRAM_INI_SEL_SRAM2_2               (1U << 3U)
#define STCU_SRAM_INI_SEL_ITCM_CPU0             (1U << 4U)
#define STCU_SRAM_INI_SEL_DTCM0_CPU0            (1U << 5U)
#define STCU_SRAM_INI_SEL_DTCM1_CPU0            (1U << 6U)
#define STCU_SRAM_INI_SEL_ITCM_CPU1             (1U << 7U)
#define STCU_SRAM_INI_SEL_DTCM0_CPU1            (1U << 8U)
#define STCU_SRAM_INI_SEL_DTCM1_CPU1            (1U << 9U)

#define STCU_SRAM_INI_SEL_ALL                                          \
        (STCU_SRAM_INI_SEL_SRAM0      | STCU_SRAM_INI_SEL_SRAM1      | \
         STCU_SRAM_INI_SEL_SRAM2_1    | STCU_SRAM_INI_SEL_SRAM2_2    | \
         STCU_SRAM_INI_SEL_ITCM_CPU0  | STCU_SRAM_INI_SEL_DTCM0_CPU0 | \
         STCU_SRAM_INI_SEL_DTCM1_CPU0 | STCU_SRAM_INI_SEL_ITCM_CPU1  | \
         STCU_SRAM_INI_SEL_DTCM0_CPU1 | STCU_SRAM_INI_SEL_DTCM1_CPU1)

#define STCU_SRAM_INI_DONE_STATUS_SRAM0         (1U << 0U)
#define STCU_SRAM_INI_DONE_STATUS_SRAM1         (1U << 1U)
#define STCU_SRAM_INI_DONE_STATUS_SRAM2_1       (1U << 2U)
#define STCU_SRAM_INI_DONE_STATUS_SRAM2_2       (1U << 3U)
#define STCU_SRAM_INI_DONE_STATUS_ITCM_CPU0     (1U << 4U)
#define STCU_SRAM_INI_DONE_STATUS_DTCM0_CPU0    (1U << 5U)
#define STCU_SRAM_INI_DONE_STATUS_DTCM1_CPU0    (1U << 6U)
#define STCU_SRAM_INI_DONE_STATUS_ITCM_CPU1     (1U << 7U)
#define STCU_SRAM_INI_DONE_STATUS_DTCM0_CPU1    (1U << 8U)
#define STCU_SRAM_INI_DONE_STATUS_DTCM1_CPU1    (1U << 9U)

void soc_enable_ecc(void)
{
        /* enable AXBS/CPU0/1 ECC */
        __asm__ volatile("dsb");
        __asm__ volatile("isb");
        xwmb_access(xwu32_t, SCM_BASE_ADDR + SCM_MAMECCEN0_OFFSET) = 0x0FFFFFCFU;
        xwmb_access(xwu32_t, SCM_BASE_ADDR + SCM_MAMECCEN1_OFFSET) = 0x0000FD3FU;
        xwmb_access(xwu32_t, SCM_BASE_ADDR + SCM_CPU0ECCEN_OFFSET) = 0x00000FCFU;
        xwmb_access(xwu32_t, SCM_BASE_ADDR + SCM_CPU1ECCEN_OFFSET) = 0x00000FCFU;
        __asm__ volatile("dsb");
        __asm__ volatile("isb");
}

void soc_disable_ecc(void)
{
        /* disable AXBS/CPU0/1 ECC */
        __asm__ volatile("dsb");
        __asm__ volatile("isb");
        xwmb_access(xwu32_t, SCM_BASE_ADDR + SCM_MAMECCEN0_OFFSET) = 0x0AAAAA8AU;
        xwmb_access(xwu32_t, SCM_BASE_ADDR + SCM_MAMECCEN1_OFFSET) = 0x0000A92AU;
        xwmb_access(xwu32_t, SCM_BASE_ADDR + SCM_CPU0ECCEN_OFFSET) = 0x00000A8AU;
        xwmb_access(xwu32_t, SCM_BASE_ADDR + SCM_CPU1ECCEN_OFFSET) = 0x00000A8AU;
        __asm__ volatile("dsb");
        __asm__ volatile("isb");
}

void soc_disable_wdog0(void)
{
        xwu32_t val;

        /* Unlock the wdog.
           Note: The unlock status only persist for 128 bus clocks.
           SHALL NOT use single-step or break points in the following lines. */
        xwmb_access(xwu32_t, WDOG0_BASE_ADDR + WDOG_COUNTER_OFFSET) =
                WDOG_COUNTER_UNLOCK;
        do {
                /* Wait until the unlock take effect. */
                val = xwmb_access(xwu32_t, WDOG0_BASE_ADDR + WDOG_CS_OFFSET);
        } while ((val & WDOG_CS_ULK_STAT) == 0U);
        /* Disable Watchdog */
        xwmb_access(xwu32_t, WDOG0_BASE_ADDR + WDOG_CS_OFFSET) = WDOG_CS_DISABLE_WDOG;
        xwmb_access(xwu32_t, WDOG0_BASE_ADDR + WDOG_TIMEOUT_OFFSET) = 0xF000U;
        do {
                /* Wait for done */
                val = xwmb_access(xwu32_t, WDOG0_BASE_ADDR + WDOG_CS_OFFSET);
        } while ((val & WDOG_CS_RECFG_STAT) == 0U);
}

void soc_disable_wdog1(void)
{
        xwu32_t val;

        /* Unlock the wdog.
           Note: The unlock status only persist for 128 bus clocks.
           SHALL NOT use single-step or break points in the following lines. */
        xwmb_access(xwu32_t, WDOG1_BASE_ADDR + WDOG_COUNTER_OFFSET) =
                WDOG_COUNTER_UNLOCK;
        do {
                /* Wait until the unlock take effect. */
                val = xwmb_access(xwu32_t, WDOG1_BASE_ADDR + WDOG_CS_OFFSET);
        } while ((val & WDOG_CS_ULK_STAT) == 0U);
        /* Disable Watchdog */
        xwmb_access(xwu32_t, WDOG1_BASE_ADDR + WDOG_CS_OFFSET) = WDOG_CS_DISABLE_WDOG;
        xwmb_access(xwu32_t, WDOG1_BASE_ADDR + WDOG_TIMEOUT_OFFSET) = 0xF000U;
        do {
                /* Wait for done */
                val = xwmb_access(xwu32_t, WDOG1_BASE_ADDR + WDOG_CS_OFFSET);
        } while ((val & WDOG_CS_RECFG_STAT) == 0U);
}

void soc_disable_wdog2(void)
{
}

void soc_disable_wdog3(void)
{
}

/**
 * @brief 通过STCU初始化内存
 * @note
 * 此函数不能访问内存，只能访问寄存器和Rom地址，
 * 需要反汇编检查编译器是否使用了栈创建变量。
 */
void soc_init_ram(void)
{
        register xwu32_t val;
        register xwu32_t stbmode;

        /* STCU PCC Enable */
        xwmb_access(xwu32_t, PCC_STCU_ADDR) = 0x00800000U;

        /* Get the reset reason */
        val = xwmb_access(xwu32_t, RGM_BASE_ADDR + RGM_SRS_OFFSET);
        if (RGM_SRS_WAKEUP == (val & RGM_SRS_WAKEUP)) {
                /* Clear the RAM according to standby mode when wakeup from standby */
                /* Select all memory regions ro clear */
                xwmb_access(xwu32_t, STCU_BASE_ADDR + STCU_SRAM_INI_SEL_OFFSET) =
                        STCU_SRAM_INI_SEL_ALL;
                val = xwmb_access(xwu32_t, SMC_BASE_ADDR + SMC_STANDBY_CFG_OFFSET);
                stbmode = val & SMC_STANDBY_CFG_OPTION_MASK;
                /* Keep the standby retention RAM */
                xwmb_access(xwu32_t, STCU_BASE_ADDR + STCU_SRAM_INI_CTRL_OFFSET) =
                        (STCU_SRAM_INI_CTRL_EN | STCU_SRAM_INI_CTRL_MODE(stbmode));
                do {
                        /* wait busy */
                        val = xwmb_access(xwu32_t,
                                          STCU_BASE_ADDR + STCU_SRAM_INI_STATUS_OFFSET);
                } while (0 == (val & STCU_SRAM_INI_STATUS_BUSY));
                do {
                        /* wait done */
                        val = xwmb_access(xwu32_t,
                                          STCU_BASE_ADDR + STCU_SRAM_INI_STATUS_OFFSET);
                } while (0 == (val & STCU_SRAM_INI_STATUS_DONE));
        } else {
                /* Clear all RAM when RAM is invalid */
                /* Select all memory regions ro clear */
                xwmb_access(xwu32_t, STCU_BASE_ADDR + STCU_SRAM_INI_SEL_OFFSET) =
                        STCU_SRAM_INI_SEL_ALL;
                xwmb_access(xwu32_t, STCU_BASE_ADDR + STCU_SRAM_INI_CTRL_OFFSET) =
                        STCU_SRAM_INI_CTRL_EN;
                do {
                        /* wait busy */
                        val = xwmb_access(xwu32_t,
                                          STCU_BASE_ADDR + STCU_SRAM_INI_STATUS_OFFSET);
                } while (0U == (val & STCU_SRAM_INI_STATUS_BUSY));
                do {
                        /* wait done */
                        val = xwmb_access(xwu32_t,
                                          STCU_BASE_ADDR + STCU_SRAM_INI_STATUS_OFFSET);
                } while (0 == (val & STCU_SRAM_INI_STATUS_DONE));
        }
}

void soc_lowlevel_init(void)
{
        /* Workaround for erratum ERR_Debug_001 */
        /* clear dwt counter to handle cpu0 lockstep error under debug */
        xwmb_access(xwu32_t, DEMCR_ADDR) = DEMCR_TRCENA;
        xwmb_access(xwu32_t, DWT_CYCCNT_ADDR) = 0U;
}
