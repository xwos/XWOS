/**
 * @file
 * @brief 架构描述层：ARMv6 Cortex-M
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

#ifndef __armv6m_isa_h__
#define __armv6m_isa_h__

#include <xwos/standard.h>

/******** SCB Application Interrupt and Reset Control Register ********/
#define SCB_AIRCR_VECTKEY_POS           (16UL)
#define SCB_AIRCR_VECTKEY_MSK           (0xFFFFUL << SCB_AIRCR_VECTKEY_POS)

#define SCB_AIRCR_VECTKEYSTAT_POS       (16UL)
#define SCB_AIRCR_VECTKEYSTAT_MSK       (0xFFFFUL << SCB_AIRCR_VECTKEYSTAT_POS)

#define SCB_AIRCR_ENDIANESS_POS         (15UL)
#define SCB_AIRCR_ENDIANESS_MSK         (1UL << SCB_AIRCR_ENDIANESS_POS)

#define SCB_AIRCR_PRIGROUP_POS          (8UL)
#define SCB_AIRCR_PRIGROUP_MSK          (7UL << SCB_AIRCR_PRIGROUP_POS)

#define SCB_AIRCR_SYSRESETREQ_POS       (2UL)
#define SCB_AIRCR_SYSRESETREQ_MSK       (1UL << SCB_AIRCR_SYSRESETREQ_POS)

#define SCB_AIRCR_VECTCLRACTIVE_POS     (1UL)
#define SCB_AIRCR_VECTCLRACTIVE_MSK     (1UL << SCB_AIRCR_VECTCLRACTIVE_POS)

#define SCB_AIRCR_VECTRESET_POS         (0UL)
#define SCB_AIRCR_VECTRESET_MSK         (1UL << SCB_AIRCR_VECTRESET_POS)

/******** TPI Asynchronous Clock Prescaler Register Definitions ********/
#define TPI_ACPR_PRESCALER_POS          (0UL)
#define TPI_ACPR_PRESCALER_MSK          (0x1FFFUL << TPI_ACPR_PRESCALER_POS)

/* TPI Selected Pin Protocol Register Definitions */
#define TPI_SPPR_TXMODE_POS             (0UL)
#define TPI_SPPR_TXMODE_MSK             (0x3UL << TPI_SPPR_TXMODE_POS)
#define TPI_SPPR_TXMODE_TRACEPORTMODE   (0UL)
#define TPI_SPPR_TXMODE_MANCHESTER      (1UL)
#define TPI_SPPR_TXMODE_NRZ             (2UL)

/* TPI Formatter and Flush Status Register Definitions */
#define TPI_FFSR_FTNONSTOP_POS          (3UL)
#define TPI_FFSR_FTNONSTOP_MSK          (0x1UL << TPI_FFSR_FTNONSTOP_POS)

#define TPI_FFSR_TCPRESENT_POS          (2UL)
#define TPI_FFSR_TCPRESENT_MSK          (0x1UL << TPI_FFSR_TCPRESENT_POS)

#define TPI_FFSR_FTSTOPPED_POS          (1UL)
#define TPI_FFSR_FTSTOPPED_MSK          (0x1UL << TPI_FFSR_FTSTOPPED_POS)

#define TPI_FFSR_FLINPROG_POS           (0UL)
#define TPI_FFSR_FLINPROG_MSK           (0x1UL << TPI_FFSR_FLINPROG_POS)

/* TPI Formatter and Flush Control Register Definitions */
#define TPI_FFCR_TRIGIN_POS             (8UL)
#define TPI_FFCR_TRIGIN_MSK             (0x1UL << TPI_FFCR_TRIGIN_POS)

#define TPI_FFCR_ENFCONT_POS            (1UL)
#define TPI_FFCR_ENFCONT_MSK            (0x1UL << TPI_FFCR_ENFCONT_POS)

/* TPI TRIGGER Register Definitions */
#define TPI_TRIGGER_TRIGGER_POS         (0UL)
#define TPI_TRIGGER_TRIGGER_MSK         (0x1UL << TPI_TRIGGER_TRIGGER_POS)

/* TPI Integration ETM Data Register Definitions (FIFO0) */
#define TPI_FIFO0_ITM_ATVALID_POS       29
#define TPI_FIFO0_ITM_ATVALID_MSK       (0x3UL << TPI_FIFO0_ITM_ATVALID_POS)

#define TPI_FIFO0_ITM_BYTECOUNT_POS     27
#define TPI_FIFO0_ITM_BYTECOUNT_MSK     (0x3UL << TPI_FIFO0_ITM_BYTECOUNT_POS)

#define TPI_FIFO0_ETM_ATVALID_POS       26
#define TPI_FIFO0_ETM_ATVALID_MSK       (0x3UL << TPI_FIFO0_ETM_ATVALID_POS)

#define TPI_FIFO0_ETM_BYTECOUNT_POS     24
#define TPI_FIFO0_ETM_BYTECOUNT_MSK     (0x3UL << TPI_FIFO0_ETM_BYTECOUNT_POS)

#define TPI_FIFO0_ETM2_POS              16
#define TPI_FIFO0_ETM2_MSK              (0xFFUL << TPI_FIFO0_ETM2_POS)

#define TPI_FIFO0_ETM1_POS              8
#define TPI_FIFO0_ETM1_MSK              (0xFFUL << TPI_FIFO0_ETM1_POS)

#define TPI_FIFO0_ETM0_POS              0
#define TPI_FIFO0_ETM0_MSK              (0xFFUL << TPI_FIFO0_ETM0_POS)

/* TPI ITATBCTR2 Register Definitions */
#define TPI_ITATBCTR2_ATREADY_POS       0
#define TPI_ITATBCTR2_ATREADY_MSK       (0x1UL << TPI_ITATBCTR2_ATREADY_POS)

/* TPI Integration ITM Data Register Definitions (FIFO1) */
#define TPI_FIFO1_ITM_ATVALID_POS       29
#define TPI_FIFO1_ITM_ATVALID_MSK       (0x3UL << TPI_FIFO1_ITM_ATVALID_POS)

#define TPI_FIFO1_ITM_BYTECOUNT_POS     27
#define TPI_FIFO1_ITM_BYTECOUNT_MSK     (0X3UL << TPI_FIFO1_ITM_BYTECOUNT_POS)

#define TPI_FIFO1_ETM_ATVALID_POS       26
#define TPI_FIFO1_ETM_ATVALID_MSK       (0X3UL << TPI_FIFO1_ETM_ATVALID_POS)

#define TPI_FIFO1_ETM_BYTECOUNT_POS     24
#define TPI_FIFO1_ETM_BYTECOUNT_MSK     (0X3UL << TPI_FIFO1_ETM_BYTECOUNT_POS)

#define TPI_FIFO1_ITM2_POS              16
#define TPI_FIFO1_ITM2_MSK              (0XFFUL << TPI_FIFO1_ITM2_POS)

#define TPI_FIFO1_ITM1_POS              8
#define TPI_FIFO1_ITM1_MSK              (0XFFUL << TPI_FIFO1_ITM1_POS)

#define TPI_FIFO1_ITM0_POS              0
#define TPI_FIFO1_ITM0_MSK              (0XFFUL << TPI_FIFO1_ITM0_POS)

/* TPI ITATBCTR0 Register Definitions */
#define TPI_ITATBCTR0_ATREADY_POS       0
#define TPI_ITATBCTR0_ATREADY_MSK       (0X1UL << TPI_ITATBCTR0_ATREADY_POS)

/* TPI Integration Mode Control Register Definitions */
#define TPI_ITCTRL_MODE_POS             0
#define TPI_ITCTRL_MODE_MSK             (0X1UL << TPI_ITCTRL_MODE_POS)

/* TPI DEVID Register Definitions */
#define TPI_DEVID_NRZVALID_POS          11
#define TPI_DEVID_NRZVALID_MSK          (0X1UL << TPI_DEVID_NRZVALID_POS)

#define TPI_DEVID_MANCVALID_POS         10
#define TPI_DEVID_MANCVALID_MSK         (0X1UL << TPI_DEVID_MANCVALID_POS)

#define TPI_DEVID_PTINVALID_POS         9
#define TPI_DEVID_PTINVALID_MSK         (0X1UL << TPI_DEVID_PTINVALID_POS)

#define TPI_DEVID_MINBUFSZ_POS          6
#define TPI_DEVID_MINBUFSZ_MSK          (0X7UL << TPI_DEVID_MINBUFSZ_POS)

#define TPI_DEVID_ASYNCLKIN_POS         5
#define TPI_DEVID_ASYNCLKIN_MSK         (0X1UL << TPI_DEVID_ASYNCLKIN_POS)

#define TPI_DEVID_NRTRACEINPUT_POS      0
#define TPI_DEVID_NRTRACEINPUT_MSK      (0X1FUL << TPI_DEVID_NRTRACEINPUT_POS)

/* TPI DEVTYPE Register Definitions */
#define TPI_DEVTYPE_SUBTYPE_POS         0
#define TPI_DEVTYPE_SUBTYPE_MSK         (0XFUL << TPI_DEVTYPE_SUBTYPE_POS)

#define TPI_DEVTYPE_MAJORTYPE_POS       4
#define TPI_DEVTYPE_MAJORTYPE_MSK       (0XFUL << TPI_DEVTYPE_MAJORTYPE_POS)

/******** ITM Trace Control Register Definitions ********/
/* ITM Trace Privilege Register Definitions */
#define ITM_TPR_PRIVMASK_POS            0 /**< ITM TPR: PRIVMASK Position */
#define ITM_TPR_PRIVMASK_MSK            (0xFUL << ITM_TPR_PRIVMASK_POS) /**< ITM TPR: PRIVMASK Mask */

/* ITM Trace Control Register Definitions */
#define ITM_TCR_BUSY_POS                23 /**< ITM TCR: BUSY Position */
#define ITM_TCR_BUSY_MSK                (1UL << ITM_TCR_BUSY_POS) /**< ITM TCR: BUSY Mask */

#define ITM_TCR_TRACEBUSID_POS          16 /**< ITM TCR: ATBID Position */
#define ITM_TCR_TRACEBUSID_MSK          (0x7FUL << ITM_TCR_TRACEBUSID_POS) /**< ITM TCR: ATBID Mask */

#define ITM_TCR_GTSFREQ_POS             10 /**< ITM TCR: Global timestamp frequency Position */
#define ITM_TCR_GTSFREQ_MSK             (3UL << ITM_TCR_GTSFREQ_Pos) /**< ITM TCR: Global timestamp frequency Mask */

#define ITM_TCR_TSPRESCALE_POS          8 /**< ITM TCR: TSPrescale Position */
#define ITM_TCR_TSPRESCALE_MSK          (3UL << ITM_TCR_TSPRESCALE_POS) /**< ITM TCR: TSPrescale Mask */

#define ITM_TCR_SWOENA_POS              4 /**< ITM TCR: SWOENA Position */
#define ITM_TCR_SWOENA_MSK              (1UL << ITM_TCR_SWOENA_POS) /**< ITM TCR: SWOENA Mask */

#define ITM_TCR_DWTENA_POS              3 /**< ITM TCR: DWTENA Position */
#define ITM_TCR_DWTENA_MSK              (1UL << ITM_TCR_DWTENA_POS) /**< ITM TCR: DWTENA Mask */

#define ITM_TCR_SYNCENA_POS             2 /**< ITM TCR: SYNCENA Position */
#define ITM_TCR_SYNCENA_MSK             (1UL << ITM_TCR_SYNCENA_POS) /**< ITM TCR: SYNCENA Mask */

#define ITM_TCR_TSENA_POS               1 /**< ITM TCR: TSENA Position */
#define ITM_TCR_TSENA_MSK               (1UL << ITM_TCR_TSENA_POS) /**< ITM TCR: TSENA Mask */

#define ITM_TCR_ITMENA_POS              0 /**< ITM TCR: ITM Enable bit Position */
#define ITM_TCR_ITMENA_MSK              (1UL << ITM_TCR_ITMENA_POS) /**< ITM TCR: ITM Enable bit Mask */

/* ITM Integration Write Register Definitions */
#define ITM_IWR_ATVALIDM_POS            0 /**< ITM IWR: ATVALIDM Position */
#define ITM_IWR_ATVALIDM_MSK            (1UL << ITM_IWR_ATVALIDM_POS) /**< ITM IWR: ATVALIDM Mask */

/* ITM Integration Read Register Definitions */
#define ITM_IRR_ATREADYM_POS            0 /**< ITM IRR: ATREADYM Position */
#define ITM_IRR_ATREADYM_MSK            (1UL << ITM_IRR_ATREADYM_POS) /**< ITM IRR: ATREADYM Mask */

/* ITM Integration Mode Control Register Definitions */
#define ITM_IMCR_INTEGRATION_POS        0 /**< ITM IMCR: INTEGRATION Position */
#define ITM_IMCR_INTEGRATION_MSK        (1UL << ITM_IMCR_INTEGRATION_POS) /**< ITM IMCR: INTEGRATION Mask */

/* ITM Lock Status Register Definitions */
#define ITM_LSR_BYTEACC_POS             2 /**< ITM LSR: ByteAcc Position */
#define ITM_LSR_BYTEACC_MSK             (1UL << ITM_LSR_BYTEACC_POS) /**< ITM LSR: ByteAcc Mask */

#define ITM_LSR_ACCESS_POS              1 /**< ITM LSR: Access Position */
#define ITM_LSR_ACCESS_MSK              (1UL << ITM_LSR_ACCESS_POS) /**< ITM LSR: Access Mask */

#define ITM_LSR_PRESENT_POS             0 /**< ITM LSR: Present Position */
#define ITM_LSR_PRESENT_MSK             (1UL << ITM_LSR_PRESENT_POS) /**< ITM LSR: Present Mask */

/**
 * @brief program status register
 * @note little-endian
 */
union cm_xpsr_reg {
        struct {
                xwu32_t reserved0:16; /**< bit0~15 Reserved */
                __xw_io xwu32_t ge:4; /**< bit16~19 Greater than or Equal flags */
                xwu32_t reserved1:7; /**< bit20~26 Reserved */
                __xw_io xwu32_t q:1; /**< bit27 Saturation condition flag */
                __xw_io xwu32_t v:1; /**< bit28 Overflow condition code flag */
                __xw_io xwu32_t c:1; /**< bit29 Carry condition code flag */
                __xw_io xwu32_t z:1; /**< bit30 Zero condition code flag */
                __xw_io xwu32_t n:1; /**< bit31 Negative condition code flag */
        } apsr; /**< Structure used for bit access */

        struct {
                __xw_io xwu32_t isr:9; /**< bit0~8 Exception number */
                xwu32_t reserved0:23; /**< bit9~31 Reserved */
        } ipsr; /**< Structure used for bit access */

        struct {
                xwu32_t reserved0:10;
                union {
                        __xw_io xwu32_t ici_b5_0:6;
                        __xw_io xwu32_t it_b7_2:6;
                } u1;
                xwu32_t reserved1:8;
                __xw_io xwu32_t t:1;
                union {
                        __xw_io xwu32_t ici_b7_6:2;
                        __xw_io xwu32_t it_b1_0:2;
                } u2;
                __xw_io xwu32_t reserved2:5;
        } epsr; /**< structure used for bit access */

        __xw_io xwu32_t u32; /**< Used for word access */
};

/**
 *@brief Control Registers (CONTROL).
 */
union cm_control_reg {
        struct {
                __xw_io xwu32_t npriv:1; /**< b0: Execution privilege in Thread mode */
                __xw_io xwu32_t spsel:1; /**< b1: Stack to be used */
                __xw_io xwu32_t fpca:1; /**< b2: FP extension active flag */
                __xw_io xwu32_t reserved0:29;/**< b3~31 Reserved */
        } bit; /**< Used for bit access */
        __xw_io xwu32_t u32; /**< Used for word access */
};

/**
 * @brief System Control Space (offset: 0xE000E000)
 */
struct cm_scs_reg {
        union {
                __xw_io xwu32_t u32;
        } mcr; /**< offset:0x000(r/w) master control register */

        union {
                __xw_i xwu32_t u32;
        } ictr; /**< offset:0x004(r/ ) interrupt controller type register */

        union {
                __xw_io xwu32_t u32;
                struct {
                        __xw_io xwu32_t dismcycint:1;
                        __xw_io xwu32_t disdefwbuf:1;
                        __xw_io xwu32_t disfold:1;
                        __xw_io xwu32_t reserved0:5;
                        __xw_io xwu32_t disfpca:1;
                        __xw_io xwu32_t disoofp:1;
                        __xw_io xwu32_t reserved1:22;
                } bit;
        } actlr; /**< offset:0x008(r/w) auxiliary control register */

        xwu32_t reserved0; /**< offset:0x00c */

        struct {
                union {
                        __xw_io xwu32_t u32;
                        struct {
                                __xw_io xwu32_t en:1;
                                __xw_io xwu32_t int_en:1;
                                __xw_io xwu32_t clksrc:1;
                                __xw_io xwu32_t reserved0:13;
                                __xw_i xwu32_t cntflag:1;
                                __xw_io xwu32_t reserved1:15;
                        } bit;
                } csr; /**< offset:0x010(r/w) systick control and status register */

                union {
                        __xw_io xwu32_t u32;
                        struct {
                                __xw_io xwu32_t reload:24;
                                __xw_io xwu32_t reserved0:8;
                        } bit;
                } rvr; /**< offset:0x014(r/w) systick reload value register */

                union {
                        __xw_io xwu32_t u32;
                } cvr; /**< offset:0x018(r/w) systick current value register */

                union {
                        __xw_i xwu32_t u32;
                        struct {
                                __xw_i xwu32_t tenms:24;
                                __xw_i xwu32_t reserved0:6;
                                __xw_i xwu32_t skew:1;
                                __xw_i xwu32_t noref:1;
                        } bit;
                } calib; /**< offset:0x01c(r/ ) systick calibration register */

                xwu32_t reserved0[56]; /**< offset:0x020 */
        } systick; /**< offset:0x010 (r/w) */

        struct {
                union {
                        __xw_io xwu32_t u32;
                } iser[16]; /**< offset:0x100(r/w) interrupt set enable register */

                xwu32_t reserved0[16]; /**< offset:0x140 */

                union {
                        __xw_io xwu32_t u32;
                } icer[16]; /**< offset:0x180(r/w) interrupt clear enable register */

                xwu32_t reserved1[16]; /**< offset:0x1c0 */

                union {
                        __xw_io xwu32_t u32;
                } ispr[16]; /**< offset:0x200(r/w) interrupt set pending register */

                xwu32_t reserved2[16]; /**< offset:0x240 */

                union {
                        __xw_io xwu32_t u32;
                } icpr[16]; /**< offset:0x280(r/w) interrupt clear pending register */

                xwu32_t reserved3[16]; /**< offset:0x2c0 */

                union {
                        __xw_io xwu32_t u32;
                } iabr[16]; /**< offset:0x300(r/w) interrupt active bit register */

                xwu32_t reserved4[16]; /**< offset:0x340 */

                xwu32_t reserved5[32]; /**< offset:0x380 */

                union {
                        __xw_io xwu8_t u8;
                } ipr[496]; /**< offset:0x400(r/w) interrupt priority register */

                xwu32_t reserved6[128]; /**< offset:0x5f0 */

                xwu32_t reserved7[324]; /**< offset:0x7f0 */
        } nvic; /**< offset:0x100 (r/w) */

        struct {
                union {
                        __xw_i xwu32_t u32;
                        struct {
                                __xw_i xwu32_t rev:4;
                                __xw_i xwu32_t partno:12;
                                __xw_i xwu32_t __1:4;
                                __xw_i xwu32_t variant:4;
                                __xw_i xwu32_t implementer:8;
                        } bit;
                } cpuid; /**< offset:0xd00(r/ ) cpuid base register */

                union {
                        __xw_io xwu32_t u32;
                        struct {
                                __xw_io xwu32_t vect_active:9;
                                __xw_io xwu32_t reserved0:2;
                                __xw_io xwu32_t rettobase:1;
                                __xw_io xwu32_t vect_pending:9;
                                __xw_io xwu32_t reserved1:1;
                                __xw_io xwu32_t isr_pending:1;
                                __xw_io xwu32_t isr_preempt:1;
                                __xw_io xwu32_t reserved2:1;
                                __xw_io xwu32_t pendst_clr:1;
                                __xw_io xwu32_t pendst_set:1;
                                __xw_io xwu32_t pendsv_clr:1;
                                __xw_io xwu32_t pendsv_set:1;
                                __xw_io xwu32_t reserved3:2;
                                __xw_io xwu32_t nmipend_set:1;
                        } bit;
                } icsr; /**< offset:0xd04(r/w) interrupt control and state register */

                union {
                        __xw_io xwu32_t u32;
                        struct {
                                __xw_io xwu32_t reserved0:7;
                                __xw_io xwu32_t tbloff:25;
                        } bit;
                } vtor; /**< offset:0xd08(r/w) vector table offset register */

                union {
                        __xw_io xwu32_t u32;
                        struct {
                                __xw_io xwu32_t vect_reset:1;
                                __xw_io xwu32_t vect_clractive:1;
                                __xw_io xwu32_t sysreset_req:1;
                                __xw_io xwu32_t reserved0:5;
                                __xw_io xwu32_t prigroup:3;
                                __xw_io xwu32_t reserved1:4;
                                __xw_io xwu32_t endianness:1;
                                __xw_io xwu32_t vectkey:16;
                        } bit;
                } aircr; /**< offset:0xd0c (r/w) application interrupt and reset control register */

                union {
                        __xw_io xwu32_t u32;
                        struct {
                                __xw_io xwu32_t reserved0:1;
                                __xw_io xwu32_t sleeponexit:1;
                                __xw_io xwu32_t sleepdeep:1;
                                __xw_io xwu32_t reserved1:1;
                                __xw_io xwu32_t sevonpend:1;
                                __xw_io xwu32_t reserved2:27;
                        } bit;
                } scr; /**< offset:0xd10 (r/w) system control register */

                union {
                        __xw_io xwu32_t u32;
                        struct {
                                __xw_io xwu32_t nonbasethrdena:1;
                                __xw_io xwu32_t usersetmpend:1;
                                __xw_io xwu32_t reserved0:1;
                                __xw_io xwu32_t unalign_trp:1;
                                __xw_io xwu32_t div_0_trp:1;
                                __xw_io xwu32_t reserved1:3;
                                __xw_io xwu32_t bfhfnmign:1;
                                __xw_io xwu32_t stkalign:1;
                                __xw_io xwu32_t reserved2:6;
                                __xw_io xwu32_t dc:1;
                                __xw_io xwu32_t ic:1;
                                __xw_io xwu32_t bp:1;
                                __xw_io xwu32_t reserved3:13;
                        } bit;
                } ccr; /**< offset:0xd14 (r/w) configuration control register */

                union {
                        __xw_io xwu8_t u8;
                } shpr[12]; /**< offset:0xd18 (r/w) system handlers priority registers (4-7, 8-11, 12-15) */

                union {
                        __xw_io xwu32_t u32;
                        struct {
                                __xw_io xwu32_t memfault_act:1;
                                __xw_io xwu32_t busfault_act:1;
                                __xw_io xwu32_t reserved0:1;
                                __xw_io xwu32_t usgfault_act:1;
                                __xw_io xwu32_t reserved1:3;
                                __xw_io xwu32_t svcall_act:1;
                                __xw_io xwu32_t monitor_act:1;
                                __xw_io xwu32_t reserved2:1;
                                __xw_io xwu32_t pendsv_act:1;
                                __xw_io xwu32_t systick_act:1;
                                __xw_io xwu32_t usgfault_pended:1;
                                __xw_io xwu32_t memfault_pended:1;
                                __xw_io xwu32_t busfault_pended:1;
                                __xw_io xwu32_t svcall_pended:1;
                                __xw_io xwu32_t memfault_en:1;
                                __xw_io xwu32_t busfault_en:1;
                                __xw_io xwu32_t usgfault_en:1;
                                __xw_io xwu32_t reserved3:13;
                        } bit;
                } shcsr; /**< offset:0xd24 (r/w) system handler control and state register */

                struct {
                        union {
                                __xw_io xwu8_t u8;
                                struct {
                                        __xw_io xwu8_t iaccviol:1;
                                        __xw_io xwu8_t daccviol:1;
                                        __xw_io xwu8_t reserved0:1;
                                        __xw_io xwu8_t munstkerr:1;
                                        __xw_io xwu8_t mstkerr:1;
                                        __xw_io xwu8_t mlsperr:1;
                                        __xw_io xwu8_t reserved1:1;
                                        __xw_io xwu8_t mmarvalid:1;
                                } bit;
                        } mmfsr;

                        union {
                                __xw_io xwu8_t u8;
                                struct {
                                        __xw_io xwu8_t ibuserr:1;
                                        __xw_io xwu8_t preciserr:1;
                                        __xw_io xwu8_t impreciserr:1;
                                        __xw_io xwu8_t unstkerr:1;
                                        __xw_io xwu8_t stkerr:1;
                                        __xw_io xwu8_t lsperr:1;
                                        __xw_io xwu8_t reserved0:1;
                                        __xw_io xwu8_t bfarvalid:1;
                                } bit;
                        } bfsr;

                        union {
                                __xw_io xwu16_t u16;
                                struct {
                                        __xw_io xwu8_t undefinstr:1;
                                        __xw_io xwu8_t invstate:1;
                                        __xw_io xwu8_t invpc:1;
                                        __xw_io xwu8_t nocp:1;
                                        __xw_io xwu8_t reserved0:4;
                                        __xw_io xwu8_t unaligned:1;
                                        __xw_io xwu8_t div0:1;
                                        __xw_io xwu8_t reserved1:6;
                                } bit;
                        } ufsr;
                } cfsr; /**< offset:0xd28 (r/w) configurable fault status register */

                union {
                        __xw_io xwu32_t u32;
                        struct {
                                __xw_io xwu32_t reserved0:1;
                                __xw_io xwu32_t vecttbl:1;
                                __xw_io xwu32_t reserved1:28;
                                __xw_io xwu32_t forced:1;
                                __xw_io xwu32_t debugevt:1;
                        } bit;
                } hfsr; /**< offset:0xd2c (r/w) hardfault status register */

                union {
                        __xw_io xwu32_t u32;
                        struct {
                                __xw_io xwu32_t halted:1;
                                __xw_io xwu32_t bkpt:1;
                                __xw_io xwu32_t dwttrap:1;
                                __xw_io xwu32_t vcatch:1;
                                __xw_io xwu32_t external:1;
                                __xw_io xwu32_t reserved0:27;
                        } bit;
                } dfsr; /**< offset:0xd30 (r/w) debug fault status register */

                union {
                        __xw_io xwu32_t u32;
                } mmfar; /**< offset:0xd34 (r/w) memmanage fault address register */

                union {
                        __xw_io xwu32_t u32;
                } bfar; /**< offset:0xd38 (r/w) busfault address register */

                union {
                        __xw_io xwu32_t u32;
                } afsr; /**< offset:0xd3c (r/w) auxiliary fault status register */

                union {
                        __xw_i xwu32_t u32;
                } pfr[2]; /**< offset:0xd40 (r/ ) processor feature register */

                union {
                        __xw_i xwu32_t u32;
                } dfr; /**< offset:0xd48 (r/ ) debug feature register */

                union {
                        __xw_i xwu32_t u32;
                } afr; /**< offset:0xd4c (r/ ) auxiliary feature register */

                union {
                        __xw_i xwu32_t u32;
                } mmfr[4]; /**< offset:0xd50 (r/ ) memory model feature register */

                union {
                        __xw_i xwu32_t u32;
                } isar[5]; /**< offset:0xd60 (r/ ) instruction set attributes register */

                xwu32_t reserved0; /**< offset:0xd74 */

                union {
                        __xw_i xwu32_t u32;
                } clidr; /* offset:0xd78 (r/ ) cache level ID register */

                union {
                        __xw_i xwu32_t u32;
                } ctr; /* offset:0xd7C (r/ ) cache type register */

                union {
                        __xw_i xwu32_t u32;
                } ccsidr; /* offset:0xd80 (r/ ) cache size ID register */

                union {
                        __xw_i xwu32_t u32;
                } csselr; /* offset:0xd84 (r/ ) cache size selection register */

                union {
                        __xw_io xwu32_t u32;
                        struct {
                                __xw_io xwu32_t cp0:2;
                                __xw_io xwu32_t cp1:2;
                                __xw_io xwu32_t cp2:2;
                                __xw_io xwu32_t cp3:2;
                                __xw_io xwu32_t cp4:2;
                                __xw_io xwu32_t cp5:2;
                                __xw_io xwu32_t cp6:2;
                                __xw_io xwu32_t cp7:2;
                                __xw_io xwu32_t reserved0:4;
                                __xw_io xwu32_t cp10:2;
                                __xw_io xwu32_t cp11:2;
                                __xw_io xwu32_t reserved1:8;
                        } bit;
                } cpacr; /**< offset:0xd88 (r/w) coprocessor access control register */

                xwu32_t reserved1; /**< offset:0xd8c */
        } scb; /**< offset:0xd00 (r/w) */

        struct {
                union {
                        __xw_i  xwu32_t u32;
                } type; /**< offset:0xd90 (r/ ) mpu type register */

                union {
                        __xw_io xwu32_t u32;
                } ctrl; /**< offset:0xd94 (r/w) mpu control register */

                union {
                        __xw_io xwu32_t u32;
                } rnr; /**< offset:0xd98 (r/w) mpu region rnrber register */

                union {
                        __xw_io xwu32_t u32;
                } rbar; /**< offset:0xd9c (r/w) mpu region base address register */

                union {
                        __xw_io xwu32_t u32;
                } rasr; /**< offset:0xda0 (r/w) mpu region attribute and size register */

                union {
                        __xw_io xwu32_t u32;
                } rbar_a1; /**< offset:0xda4 (r/w) mpu alias 1 region base address register */

                union {
                        __xw_io xwu32_t u32;
                } rasr_a1; /**< offset:0xda8 (r/w) mpu alias 1 region attribute and size register */

                union {
                        __xw_io xwu32_t u32;
                } rbar_a2; /**< offset:0xdac (r/w) mpu alias 2 region base address register */

                union {
                        __xw_io xwu32_t u32;
                } rasr_a2; /**< offset:0xdb0 (r/w) mpu alias 2 region attribute and size register */

                union {
                        __xw_io xwu32_t u32;
                } rbar_a3; /**< offset:0xdb4 (r/w) mpu alias 3 region base address register */

                union {
                        __xw_io xwu32_t u32;
                } rasr_a3; /**< offset:0xdb8 (r/w) mpu alias 3 region attribute and size register */

                xwu32_t reserved0[13]; /**< offset:0xdbc */
        } mpu; /**< offset:0xd90 */

        struct {
                union {
                        __xw_io xwu32_t u32;
                        struct {
                                __xw_io xwu32_t c_debugen:1;
                                __xw_io xwu32_t c_halt:1;
                                __xw_io xwu32_t c_step:1;
                                __xw_io xwu32_t c_maskints:1;
                                xwu32_t reserved0:1;
                                __xw_io xwu32_t c_snapstall:1;
                                xwu32_t reserved1:10;
                                __xw_i xwu32_t s_regrdy:1;
                                __xw_i xwu32_t s_halt:1;
                                __xw_i xwu32_t s_sleep:1;
                                __xw_i xwu32_t s_lockup:1;
                                xwu32_t reserved2:4;
                                __xw_i xwu32_t s_reset_st:1;
                                __xw_i xwu32_t s_retire_st:1;
                                xwu32_t reserved3:6;
                        } bit;
                } dhcsr; /**< offset:0xdf0 (r/w) debug halting control and status register */

                union {
                        __xw_io xwu32_t u32;
                        struct {
                                __xw_io xwu32_t regsel:7;
                                xwu32_t reserved0:9;
                                __xw_io xwu32_t regwnr:1;
                                xwu32_t reserved1:15;
                        } bit;
                } dcrsr; /**< offset:0xdf4 ( /w) debug core register selector register */

                union {
                        __xw_io xwu32_t u32;
                } dcrdr; /**< offset:0xdf8 (r/w) debug core register data register */

                union {
                        __xw_io xwu32_t u32;
                        struct {
                                __xw_io xwu32_t vc_corereset:1;
                                xwu32_t reserved0:3;
                                __xw_io xwu32_t vc_mmerr:1;
                                __xw_io xwu32_t vc_nocperr:1;
                                __xw_io xwu32_t vc_chkerr:1;
                                __xw_io xwu32_t vc_staterr:1;
                                __xw_io xwu32_t vc_buserr:1;
                                __xw_io xwu32_t vc_interr:1;
                                __xw_io xwu32_t vc_harderr:1;
                                xwu32_t reserved1:5;
                                __xw_io xwu32_t mon_en:1;
                                __xw_io xwu32_t mon_pend:1;
                                __xw_io xwu32_t mon_step:1;
                                __xw_io xwu32_t mon_req:1;
                                xwu32_t reserved2:4;
                                __xw_io xwu32_t trcena:1;
                                xwu32_t reserved3:7;
                        } bit;
                } demcr; /**< offset:0xdfc (r/w) debug exception and monitor control register */

                xwu32_t reserved0[64]; /**< offset:0xe00 */
        } dcb; /**< offset:0xdf0 (r/w) */

        union {
                __xw_io xwu32_t u32;
        } stir; /**< offset: 0xf00 ( /w) software trigger interrupt register */

        xwu32_t reserved1[12]; /**< offset:0xf04 */

        union {
                __xw_io xwu32_t u32;
                struct {
                        __xw_io xwu32_t lspact:1;
                        __xw_io xwu32_t user:1;
                        __xw_io xwu32_t reserved0:1;
                        __xw_io xwu32_t thread:1;
                        __xw_io xwu32_t hfrdy:1;
                        __xw_io xwu32_t mmrdy:1;
                        __xw_io xwu32_t bfrdy:1;
                        __xw_io xwu32_t reserved1:1;
                        __xw_io xwu32_t monrdy:1;
                        __xw_io xwu32_t reserved2:21;
                        __xw_io xwu32_t lspen:1;
                        __xw_io xwu32_t aspen:1;
                } bit;
        } fpccr; /**< offset:0xf34 (r/w) floating point context control register */

        union {
                __xw_io xwu32_t u32;
        } fpcar; /**< offset:0xf38 (r/w) floating point context address register */

        union {
                __xw_io xwu32_t u32;
                struct {
                        __xw_io xwu32_t reserved0:22;
                        __xw_io xwu32_t rmode:2;
                        __xw_io xwu32_t fz:1;
                        __xw_io xwu32_t dn:1;
                        __xw_io xwu32_t ahp:1;
                        __xw_io xwu32_t reserved1:5;
                } bit;
        } fpdscr; /**< offset:0xf3c (r/w) floating point default status control register */

        union {
                __xw_io xwu32_t u32;
        } mvfr0; /**< offset:0xf40 (r/w) media and fp feature register 0 */

        union {
                __xw_io xwu32_t u32;
        } mvfr1; /**< offset:0xf44 (r/w) media and fp feature register 1 */

        union {
                __xw_io xwu32_t u32;
        } mvfr2; /**< offset:0xf48 (r/w) media and fp feature register 2 */

        xwu32_t reserved2[33]; /**< offset:0xf4c */

        union {
                __xw_i xwu32_t u32;
        } pid4; /**< offset:0xfd0 (r/ ) */

        union {
                __xw_i xwu32_t u32;
        } pid5; /**< offset:0xfd4 (r/ ) */

        union {
                __xw_i xwu32_t u32;
        } pid6; /**< offset:0xfd8 (r/ ) */

        union {
                __xw_i xwu32_t u32;
        } pid7; /**< offset:0xfdc (r/ ) */

        union {
                __xw_i xwu32_t u32;
        } pid0; /**< offset:0xfe0 (r/ ) */

        union {
                __xw_i xwu32_t u32;
        } pid1; /**< offset:0xfe4 (r/ ) */

        union {
                __xw_i xwu32_t u32;
        } pid2; /**< offset:0xfe8 (r/ ) */

        union {
                __xw_i xwu32_t u32;
        } pid3; /**< offset:0xfec (r/ ) */

        union {
                __xw_i xwu32_t u32;
        } cid0; /**< offset:0xff0 (r/ ) */

        union {
                __xw_i xwu32_t u32;
        } cid1; /**< offset:0xff4 (r/ ) */

        union {
                __xw_i xwu32_t u32;
        } cid2; /**< offset:0xff8 (r/ ) */

        union {
                __xw_i xwu32_t u32;
        } cid3; /**< offset:0xffc (r/ ) */
};

/**
 * @brief Instrumentation Trace Macrocell Register (ITM).
 */
struct cm_itm_reg {
        union {
                __xw_io xwu8_t u8; /**< ( /W) ITM Stimulus Port 8-bit */
                __xw_io xwu16_t u16; /**< ( /W) ITM Stimulus Port 16-bit */
                __xw_io xwu32_t u32; /**< ( /W) ITM Stimulus Port 32-bit */
                struct {
                        __xw_i xwu32_t fifordy:1; /**< FIFO ready */
                } bit;
        } port[256]; /**< Offset:0x000 ( /W) ITM Stimulus Port Registers */

        xwu32_t reserved0[640];

        union {
                __xw_io xwu32_t u32;
        } ter[8]; /**< Offset:0xE00 (R/W) ITM Trace Enable Register */

        xwu32_t reserved1[8];

        union {
                __xw_io xwu32_t u32;
        } tpr; /**< Offset:0xE40 (R/W) ITM Trace Privilege Register */

        xwu32_t reserved2[15];

        union {
                __xw_io xwu32_t u32;
                struct {
                        __xw_io xwu32_t itmena:1;
                        __xw_io xwu32_t tsena:1;
                        __xw_io xwu32_t syncena:1;
                        __xw_io xwu32_t txena:1;
                        __xw_io xwu32_t swoena:1;
                        __xw_io xwu32_t reserved0:3;
                        __xw_io xwu32_t tsprescale:2;
                        __xw_io xwu32_t gtsfreq:2;
                        __xw_io xwu32_t reserved1:4;
                        __xw_io xwu32_t tracebusid:7;
                        __xw_io xwu32_t busy:1;
                        __xw_io xwu32_t reserved2:8;
                } bit;
        } tcr; /**< Offset:0xE80 (R/W) ITM Trace Control Register */

        xwu32_t reserved3[29];

        union {
                __xw_io xwu32_t u32;
        } iwr; /**< Offset:0xEF8 ( /W) ITM Integration Write Register */

        union {
                __xw_i xwu32_t u32;
        } irr; /**< Offset:0xEFC (R/ ) ITM Integration Read Register */

        union {
                __xw_io xwu32_t u32;
        } imcr; /**< Offset:0xF00 (R/W) ITM Integration Mode Control Register */

        xwu32_t reserved4[43];

        union {
                __xw_io xwu32_t u32;
        } lar; /**< Offset:0xFB0 ( /W) ITM Lock Access Register */

        union {
                __xw_i xwu32_t u32;
        } lsr; /**< Offset:0xFB4 (R/ ) ITM Lock Status Register */

        xwu32_t reserved5[6];

        union {
                __xw_i xwu32_t u32;
        } pid4; /**< Offset:0xFD0 (R/ ) ITM Peripheral Identification Register #4 */

        union {
                __xw_i xwu32_t u32;
        } pid5; /**< Offset:0xFD4 (R/ ) ITM Peripheral Identification Register #5 */

        union {
                __xw_i xwu32_t u32;
        } pid6; /**< Offset:0xFD8 (R/ ) ITM Peripheral Identification Register #6 */

        union {
                __xw_i xwu32_t u32;
        } pid7; /**< Offset:0xFDC (R/ ) ITM Peripheral Identification Register #7 */

        union {
                __xw_i xwu32_t u32;
        } pid0; /**< Offset:0xFE0 (R/ ) ITM Peripheral Identification Register #0 */

        union {
                __xw_i xwu32_t u32;
        } pid1; /**< Offset:0xFE4 (R/ ) ITM Peripheral Identification Register #1 */

        union {
                __xw_i xwu32_t u32;
        } pid2; /**< Offset:0xFE8 (R/ ) ITM Peripheral Identification Register #2 */

        union {
                __xw_i xwu32_t u32;
        } pid3; /**< Offset:0xFEC (R/ ) ITM Peripheral Identification Register #3 */

        union {
                __xw_i xwu32_t u32;
        } cid0; /**< Offset:0xFF0 (R/ ) ITM Component Identification Register #0 */

        union {
                __xw_i xwu32_t u32;
        } cid1; /**< Offset:0xFF4 (R/ ) ITM Component Identification Register #1 */

        union {
                __xw_i xwu32_t u32;
        } cid2; /**< Offset:0xFF8 (R/ ) ITM Component Identification Register #2 */

        union {
                __xw_i xwu32_t u32;
        } cid3; /**< Offset:0xFFC (R/ ) ITM Component Identification Register #3 */
};

/**
 * @brief Data Watchpoint and Trace Register (DWT)
 */
struct cm_dwt_reg {
        union {
                __xw_io xwu32_t u32;
        } ctrl; /**< Offset: 0x000 (R/W) Control Register */

        union {
                __xw_io xwu32_t u32;
        } cyccnt; /**< Offset: 0x004 (R/W) Cycle Count Register */

        union {
                __xw_io xwu32_t u32;
        } cpicnt; /**< Offset: 0x008 (R/W) CPI Count Register */

        union {
                __xw_io xwu32_t u32;
        } exccnt; /**< Offset: 0x00C (R/W) Exception Overhead Count Register */

        union {
                __xw_io xwu32_t u32;
        } sleepcnt; /**< Offset: 0x010 (R/W) Sleep Count Register */

        union {
                __xw_io xwu32_t u32;
        } lsucnt; /**< Offset: 0x014 (R/W) LSU Count Register */

        union {
                __xw_io xwu32_t u32;
        } foldcnt; /**< Offset: 0x018 (R/W) Folded-instruction Count Register */

        union {
                __xw_i xwu32_t u32;
        } pcsr; /**< Offset: 0x01C (R/ ) Program Counter Sample Register */

        union {
                __xw_io xwu32_t u32;
        } comp0; /**< Offset: 0x020 (R/W) Comparator Register 0 */

        union {
                __xw_io xwu32_t u32;
        } mask0; /**< Offset: 0x024 (R/W) Mask Register 0 */

        union {
                __xw_io xwu32_t u32;
        } function0; /**< Offset: 0x028 (R/W) Function Register 0 */

        xwu32_t reserved0[1];

        union {
                __xw_io xwu32_t u32;
        } comp1; /**< Offset: 0x030 (R/W) Comparator Register 1 */

        union {
                __xw_io xwu32_t u32;
        } mask1; /**< Offset: 0x034 (R/W) Mask Register 1 */

        union {
                __xw_io xwu32_t u32;
        } function1; /**< Offset: 0x038 (R/W) Function Register 1 */

        xwu32_t reserved1[1];

        union {
                __xw_io xwu32_t u32;
        } comp2; /**< Offset: 0x040 (R/W) Comparator Register 2 */

        union {
                __xw_io xwu32_t u32;
        } mask2; /**< Offset: 0x044 (R/W) Mask Register 2 */

        union {
                __xw_io xwu32_t u32;
        } function2; /**< Offset: 0x048 (R/W) Function Register 2 */

        xwu32_t reserved2[1];

        union {
                __xw_io xwu32_t u32;
        } comp3; /**< Offset: 0x050 (R/W) Comparator Register 3 */

        union {
                __xw_io xwu32_t u32;
        } mask3; /**< Offset: 0x054 (R/W) Mask Register 3 */

        union {
                __xw_io xwu32_t u32;
        } function3; /**< Offset: 0x058 (R/W) Function Register 3 */
};

/**
 * @brief Trace Port Interface Register (TPI).
 */
struct cm_tpiu_reg {
        union {
                __xw_i xwu32_t u32;
        } sspsr; /**< Offset: 0x000 (R/ ) Supported Parallel Port Size Register */

        union {
                __xw_io xwu32_t u32;
        } cspsr; /**< Offset: 0x004 (R/W) Current Parallel Port Size Register */

        xwu32_t reserved0[2];

        union {
                __xw_io xwu32_t u32;
                struct {
                        __xw_io xwu32_t swoscaler:16;
                        xwu32_t reserved0:16;
                } bit;
        } acpr; /**< Offset: 0x010 (R/W) Asynchronous Clock Prescaler Register */

        xwu32_t reserved1[55];

        union {
                __xw_io xwu32_t u32;
                struct {
                        __xw_io xwu32_t txmode:2;
                        xwu32_t reserved0:30;
                } bit;
        } sppr; /**< Offset: 0x0F0 (R/W) Selected Pin Protocol Register */

        xwu32_t reserved2[131];

        union {
                __xw_i xwu32_t u32;
                struct {
                        __xw_io xwu32_t flinprog:1;
                        __xw_io xwu32_t ftstopped:1;
                        __xw_io xwu32_t tcpresent:1;
                        __xw_io xwu32_t ftnonstop:1;
                } bit;
        } ffsr; /**< Offset: 0x300 (R/ ) Formatter and Flush Status Register */

        union {
                __xw_io xwu32_t u32;
        } ffcr; /**< Offset: 0x304 (R/W) Formatter and Flush Control Register */

        union {
                __xw_i xwu32_t u32;
        } fscr; /**< Offset: 0x308 (R/ ) Formatter Synchronization Counter Register */

        xwu32_t reserved3[759];

        union {
                __xw_i xwu32_t u32;
        } trigger; /**< Offset: 0xEE8 (R/ ) TRIGGER */

        union {
                __xw_i xwu32_t u32;
        } fifo0; /**< Offset: 0xEEC (R/ ) Integration ETM Data */

        union {
                __xw_i xwu32_t u32;
        } itatbctr2; /**< Offset: 0xEF0 (R/ ) ITATBCTR2 */

        xwu32_t reserved4[1];

        union {
                __xw_i xwu32_t u32;
        } itatbctr0; /**< Offset: 0xEF8 (R/ )  ITATBCTR0 */

        union {
                __xw_i xwu32_t u32;
        } fifo1; /**< Offset: 0xEFC (R/ )  Integration ITM Data */

        union {
                __xw_io xwu32_t u32;
        } itctrl; /**< Offset: 0xF00 (R/W)  Integration Mode Control */

        xwu32_t reserved5[39];

        union {
                __xw_io xwu32_t u32;
        } claimset; /**< Offset: 0xFA0 (R/W) Claim tag set */

        union {
                __xw_io xwu32_t u32;
        } claimclr; /**< Offset: 0xFA4 (R/W) Claim tag clear */

        xwu32_t reserved7[8];

        union {
                __xw_i xwu32_t u32;
                struct {
                        xwu32_t reserved0:6;
                        __xw_i xwu32_t fifosz:3;
                        __xw_i xwu32_t ptinvalid:1;
                        __xw_i xwu32_t mancvalid:1;
                        __xw_i xwu32_t nrzvalid:1;
                        xwu32_t reserved1:20;
                } bit;
        } devid; /**< Offset: 0xFC8 (R/ ) TPIU_DEVID */

        union {
                __xw_i xwu32_t u32;
        } devtype; /**< Offset: 0xFCC (R/ ) TPIU_DEVTYPE */
};

/* Memory mapping of Core Hardware */
#define ARMv6m_SCS_BASE         (0xE000E000UL) /**< System Control Space Base Address */
#define ARMv6m_ITM_BASE         (0xE0000000UL) /**< ITM Base Address */
#define ARMv6m_DWT_BASE         (0xE0001000UL) /**< DWT Base Address */
#define ARMv6m_TPI_BASE         (0xE0040000UL) /**< TPI Base Address */

#define cm_scs                  (*((__xw_io struct cm_scs_reg *)ARMv6m_SCS_BASE))
#define cm_itm                  (*((__xw_io struct cm_itm_reg *)ARMv6m_ITM_BASE))
#define cm_dwt                  (*((__xw_io struct cm_dwt_reg *)ARMv6m_DWT_BASE))
#define cm_tpiu                 (*((__xw_io struct cm_tpiu_reg *)ARMv6m_TPI_BASE))

/******** ******** special registers ******** ********/
/**
 * @brief Get xPSR.
 * @param[out] xpsr: buffer to return result
 */
static __xwbsp_inline
void cm_get_xpsr(xwu32_t * xpsr)
{
        __asm__ volatile(
        "mrs    %[__xpsr], xpsr"
        : [__xpsr] "=&r" (*xpsr)
        :
        : "memory", "cc"
        );
}

/**
 * @brief Set xPSR.
 * @param[in] xpsr: new value
 */
static __xwbsp_inline
void cm_set_xpsr(xwu32_t xpsr)
{
        __asm__ volatile(
        "msr    xpsr, %[__xpsr]"
        :
        : [__xpsr] "r" (xpsr)
        : "memory", "cc"
        );
}

/**
 * @brief Get APSR.
 * @param[out] apsr: buffer to return result
 */
static __xwbsp_inline
void cm_get_apsr(xwu32_t * apsr)
{
        __asm__ volatile(
        "mrs    %[__apsr], apsr"
        : [__apsr] "=&r" (*apsr)
        :
        : "memory", "cc"
        );
}

/**
 * @brief Set APSR.
 * @param[in] apsr: new value
 */
static __xwbsp_inline
void cm_set_apsr(xwu32_t apsr)
{
        __asm__ volatile(
        "msr    apsr, %[__apsr]"
        :
        : [__apsr] "r" (apsr)
        : "memory", "cc"
        );
}

/**
 * @brief Get IPSR.
 * @param[out] ipsr: buffer to return result
 */
static __xwbsp_inline
void cm_get_ipsr(xwu32_t * ipsr)
{
        __asm__ volatile(
        "mrs    %[__ipsr], ipsr"
        : [__ipsr] "=&r" (*ipsr)
        :
        : "memory", "cc"
        );
}

/**
 * @brief Set IPSR.
 * @param[in] ipsr: new value
 */
static __xwbsp_inline
void cm_set_ipsr(xwu32_t ipsr)
{
        __asm__ volatile(
        "msr    ipsr, %[__ipsr]"
        :
        : [__ipsr] "r" (ipsr)
        : "memory", "cc"
        );
}

/**
 * @brief Get EPSR
 * @param[out] epsr: buffer to return result
 */
static __xwbsp_inline
void cm_get_epsr(xwu32_t * epsr)
{
        __asm__ volatile(
        "mrs    %[__epsr], epsr"
        : [__epsr] "=&r" (*epsr)
        :
        : "memory", "cc"
        );
}

/**
 * @brief Set EPSR
 * @param[in] epsr: new value
 */
static __xwbsp_inline
void cm_set_epsr(xwu32_t epsr)
{
        __asm__ volatile(
        "msr    epsr, %[__epsr]"
        :
        : [__epsr] "r" (epsr)
        : "memory", "cc"
        );
}

/**
 * @brief Get MSP
 * @param[out] msp: buffer to return result
 */
static __xwbsp_inline
void cm_get_msp(xwptr_t * msp)
{
        __asm__ volatile(
        "mrs    %[__msp], msp"
        : [__msp] "=&r" (*msp)
        :
        : "memory", "cc"
        );
}

/**
 * @brief Set MSP
 * @param[in] msp: new value
 */
static __xwbsp_inline
void cm_set_msp(xwptr_t msp)
{
        __asm__ volatile(
        "msr    msp, %[__msp]"
        :
        : [__msp] "r" (msp)
        : "memory", "cc"
        );
}

/**
 * @brief Get PSP
 * @param[out] psp: buffer to return result
 */
static __xwbsp_inline
void cm_get_psp(xwptr_t * psp)
{
        __asm__ volatile(
        "mrs    %[__psp], psp"
        : [__psp] "=&r" (*psp)
        :
        : "memory", "cc"
        );
}

/**
 * @brief Set PSP
 * @param[in] psp: new value
 */
static __xwbsp_inline
void cm_set_psp(xwptr_t psp)
{
        __asm__ volatile(
        "msr    psp, %[__psp]"
        :
        : [__psp] "r" (psp)
        : "memory", "cc"
        );
}

/**
 * @brief Get PRIMASK
 * @param[out] primask: buffer to return result
 */
static __xwbsp_inline
void cm_get_primask(xwu32_t * primask)
{
        __asm__ volatile(
        "mrs    %[__primask], primask"
        : [__primask] "=&r" (*primask)
        :
        : "memory", "cc"
        );
}

/**
 * @brief Set PRIMASK
 * @param[in] primask: new value
 */
static __xwbsp_inline
void cm_set_primask(xwu32_t primask)
{
        __asm__ volatile(
        "msr    primask, %[__primask]"
        :
        : [__primask] "r" (primask)
        : "memory", "cc"
        );
}

/**
 * @brief Get FAULTMASK
 * @param[out] faultmask: buffer to return result
 */
static __xwbsp_inline
void cm_get_faultmask(xwu32_t * faultmask)
{
        __asm__ volatile(
        "mrs    %[__faultmask], faultmask"
        : [__faultmask] "=&r" (*faultmask)
        :
        : "memory", "cc"
        );
}

/**
 * @brief Set FAULTMASK
 * @param[in] faultmask: new value
 */
static __xwbsp_inline
void cm_set_faultmask(xwu32_t faultmask)
{
        __asm__ volatile(
        "msr    faultmask, %[__faultmask]"
        :
        : [__faultmask] "r" (faultmask)
        : "memory", "cc"
        );
}

/**
 * @brief Get CONTROL
 * @param[out] control: buffer to return result
 */
static __xwbsp_inline
void cm_get_control(xwu32_t * control)
{
        __asm__ volatile(
        "mrs    %[__control], control"
        : [__control] "=&r" (*control)
        :
        : "memory", "cc"
        );
}

/**
 * @brief Set CONTROL
 * @param[in] control: new value
 */
static __xwbsp_inline
void cm_set_control(xwu32_t control)
{
        __asm__ volatile(
        "msr    control, %[__control]"
        :
        : [__control] "r" (control)
        : "memory", "cc"
        );
}

/******** ******** ITM ******** ********/
/**
 * @brief Send a byte to ITM port
 * @param[in] c: byte
 */
static __xwbsp_inline
void cm_itm_putc(xwu32_t port, const char c)
{
        if (__xwcc_likely((cm_itm.tcr.bit.itmena) &&
                     (cm_itm.ter[port >> 5].u32 & (1U << (port & 0x1FU))))) {
                while (0 == cm_itm.port[port].u32) {
                }
                cm_itm.port[port].u8 = (xwu8_t)c;
        }
}

/**
 * @brief Send a string to ITM port
 * @param[in] s: string
 */
static __xwbsp_inline
xwssz_t cm_itm_puts(xwu32_t port, const char * s)
{
        xwssz_t i;

        i = 0;
        while (s[i]) {
                cm_itm_putc(port, s[i]);
                i++;
        }
        return i;
}

/**
 * @brief Send a string at most n bytes to ITM port
 * @param[in] s: string
 * @param[in] n: max length
 */
static __xwbsp_inline
xwssz_t cm_itm_putns(xwu32_t port, const char * s, xwsz_t n)
{
        xwssz_t i;

        i = 0;
        while ((s[i]) && ((xwsz_t)i < n)) {
                cm_itm_putc(port, s[i]);
                i++;
        }
        return i;
}

/**
 * @brief noop function
 */
#define noop()          __asm__ volatile("nop")

/**
 * @brief System Resets
 */
static __xwbsp_inline
void cm_reset_system(void)
{
        armv6m_dsb(); /* Ensure all outstanding memory accesses
                         included buffered write are completed before reset */
        cm_scs.scb.aircr.u32  = (xwu32_t)((0x5FA << SCB_AIRCR_VECTKEY_POS) |
                                          (cm_scs.scb.aircr.u32 &
                                           SCB_AIRCR_PRIGROUP_MSK) |
                                          SCB_AIRCR_SYSRESETREQ_MSK); /* Keep priority
                                                                         group */
        armv6m_dsb();
        while (true) {
        }
}

/**
 * @brief Clear exclusive
 */
static __xwbsp_inline
void cm_clrex(void)
{
        __asm__ volatile("clrex\n" : : : "memory");
}

/**
 * @brief Load word exclusively
 * @param[in] addr: address to be load
 * @return word in the address
 */
static __xwbsp_inline
xwu32_t cm_ldrex(volatile void * addr)
{
        xwu32_t tmp;

        __asm__ volatile(
        "ldrex  %[__tmp], [%[__addr]]\n"
        : [__tmp] "=&r" (tmp)
        : [__addr] "r" (addr)
        : "memory"
        );
        return tmp;
}

/**
 * @brief Store word exclusively
 * @param[in] addr: address
 * @param[in] value: value to be stored
 * @return error code
 * @retval 1: need to retry
 * @retval 0: OK
 */
static __xwbsp_inline
xwer_t cm_strex(volatile void * addr, xwu32_t value)
{
        xwer_t rc;

        __asm__ volatile(
        "strex  %[__rc], %[__value], [%[__addr]]\n"
        : [__rc] "=&r" (rc)
        : [__value] "r" (value),
          [__addr] "r" (addr)
        : "memory"
        );
        return rc;
}

/**
 * @brief Load byte exclusively
 * @param[in] addr: address to be load
 * @return byte in the address
 */
static __xwbsp_inline
xwu8_t cm_ldrexb(volatile void * addr)
{
        xwu8_t tmp;

        __asm__ volatile(
        "ldrexb %[__tmp], [%[__addr]]\n"
        : [__tmp] "=&r" (tmp)
        : [__addr] "r" (addr)
        : "memory"
        );
        return tmp;
}

/**
 * @brief Store byte exclusively
 * @param[in] addr: address
 * @param[in] value: value to be stored
 * @return error code
 * @retval 1: need to retry
 * @retval 0: OK
 */
static __xwbsp_inline
xwer_t cm_strexb(volatile void * addr, xwu8_t value)
{
        xwer_t rc;

        __asm__ volatile(
        "strexb %[__rc], %[__value], [%[__addr]]\n"
        : [__rc] "=&r" (rc)
        : [__value] "r" (value),
          [__addr] "r" (addr)
        : "memory"
        );
        return rc;
}

/**
 * @brief Load half-word exclusively
 * @param[in] addr: address to be load
 * @return half-word in the address
 */
static __xwbsp_inline
xwu16_t cm_ldrexh(volatile void * addr)
{
        xwu16_t tmp;

        __asm__ volatile(
        "ldrexh %[__tmp], [%[__addr]]\n"
        : [__tmp] "=&r" (tmp)
        : [__addr] "r" (addr)
        : "memory"
        );
        return tmp;
}

/**
 * @brief Store half-word exclusively
 * @param[in] addr: address
 * @param[in] value: value to be stored
 * @retval 1: need to retry
 * @retval 0: OK
 */
static __xwbsp_inline
xwer_t cm_strexh(volatile void * addr, xwu16_t value)
{
        xwer_t rc;

        __asm__ volatile(
        "strexh %[__rc], %[__value], [%[__addr]]\n"
        : [__rc] "=&r" (rc)
        : [__value] "r" (value),
          [__addr] "r" (addr)
        : "memory"
        );
        return rc;
}

/**
 * @brief Prefetch data
 * @param[in] addr: address
 */
static __xwbsp_inline
void cm_prefetch(const volatile void * addr)
{
        __asm__ volatile(
        "pld    [%[__addr], #0]\n"
        :
        : [__addr] "r" (addr)
        : "memory"
        );
}

/**
 * @brief Wait for event
 */
static __xwbsp_inline
void cm_wfe(void)
{
        __asm__ volatile("wfe");
}

/**
 * @brief Wait for interrupt
 */
static __xwbsp_inline
void cm_wfi(void)
{
        __asm__ volatile("wfi");
}

/**
 * @brief Send event
 */
static __xwbsp_inline
void cm_sev(void)
{
        __asm__ volatile("sev");
}

#endif /* armv6m_isa.h */
