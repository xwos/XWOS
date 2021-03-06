/**
 * @file
 * @brief 玄武设备栈：UART common definitions
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
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

#ifndef __xwcd_ds_uart_common_h__
#define __xwcd_ds_uart_common_h__

#include <xwcd/ds/standard.h>
#include <xwos/lib/xwbop.h>
#include <xwcd/ds/device.h>

/**
 * @brief UART波特率枚举
 */
enum xwds_uart_baudrate_em {
        B50 = 50,
        B75 = 75,
        B110 = 110,
        B134 = 134,
        B150 = 150,
        B200 = 200,
        B300 = 300,
        B600 = 600,
        B1200 = 1200,
        B1800 = 1800,
        B2400 = 2400,
        B4800 = 4800,
        B9600 = 9600,
        B19200 = 19200,
        B38400 = 38400,
        B57600 = 57600,
        B115200 = 115200,
        B230400 = 230400,
        B460800 = 460800,
        B576000 = 576000,
        B921600 = 921600,
        B1000000 = 1000000,
        B1152000 = 1152000,
        B1500000 = 1500000,
        B2000000 = 2000000,
        B2500000 = 2500000,
        B3000000 = 3000000,
        B3500000 = 3500000,
        B4000000 = 4000000,
};

/**
 * @brief UART字长枚举
 */
enum xwds_uart_bits_em {
        XWDS_UART_BITS_8 = 0,
        XWDS_UART_BITS_9,
        XWDS_UART_BITS_7,
};

/**
 * @brief UART停止位枚举
 */
enum xwds_uart_stopbits_em {
        XWDS_UART_STOPBITS_1_0 = 0,
        XWDS_UART_STOPBITS_0_5,
        XWDS_UART_STOPBITS_2_0,
        XWDS_UART_STOPBITS_1_5,
};

/**
 * @brief UART奇偶校验枚举
 */
enum xwds_uart_parity_em {
        XWDS_UART_PARITY_NONE = 0,
        XWDS_UART_PARITY_EVEN,
        XWDS_UART_PARITY_ODD,
};

/**
 * @brief UART模式枚举
 */
enum xwds_uart_mode_em {
        XWDS_UART_MODE_CLOSE = 0,
        XWDS_UART_MODE_TX = XWBOP_BIT(0),
        XWDS_UART_MODE_RX = XWBOP_BIT(1),
};

/**
 * @brief UART硬件流控枚举
 */
enum xwds_uart_hardwareflowcontrol_em {
        XWDS_UART_HFC_NONE = 0,
        XWDS_UART_HFC_RTS = XWBOP_BIT(0),
        XWDS_UART_HFC_CTS = XWBOP_BIT(1),
};

/**
 * @brief UART配置
 */
struct xwds_uart_cfg {
        xwu32_t baudrate; /**< 波特率 */
        union {
                struct {
                        xwu32_t bits:1; /**< 字长 */
                        xwu32_t stopbits:2; /**< 停止位长度 */
                        xwu32_t parity:2; /**< 奇偶校验 */
                        xwu32_t hfc:2; /**< 硬件流控 */
                        xwu32_t mode:8; /**< 模式 */
                        xwu32_t reserved:17;
                } bit; /**< bit access */
                xwu32_t u32; /**< 4-byte access */
        } bus;
        void * xwccfg; /**< SOC私有配置 */
};

/**
 * @brief 测试UART波特率是否合法
 * @param[in] baudrate: 波特率
 */
static __xwcc_inline
bool xwds_uart_tst_baudrate(xwu32_t baudrate)
{
        return ((B50 == baudrate) || (B75 == baudrate) || (B110 == baudrate) ||
                (B134 == baudrate) || (B150 == baudrate) || (B200 == baudrate) ||
                (B300 == baudrate) || (B600 == baudrate) || (B1200 == baudrate) ||
                (B1800 == baudrate) || (B2400 == baudrate) || (B4800 == baudrate) ||
                (B9600 == baudrate) || (B19200 == baudrate) || (B38400 == baudrate) ||
                (B57600 == baudrate) || (B115200 == baudrate) ||
                (B230400 == baudrate) || (B460800 == baudrate) ||
                (B576000 == baudrate) || (B921600 == baudrate) ||
                (B1000000 == baudrate) || (B1152000 == baudrate) ||
                (B1500000 == baudrate) || (B2000000 == baudrate) ||
                (B2500000 == baudrate) || (B3000000 == baudrate) ||
                (B3500000 == baudrate) || (B4000000 == baudrate));
}

#endif /* xwcd/ds/uart/common.h */
