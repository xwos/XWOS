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
        B50 = 50U,
        B75 = 75U,
        B110 = 110U,
        B134 = 134U,
        B150 = 150U,
        B200 = 200U,
        B300 = 300U,
        B600 = 600U,
        B1200 = 1200U,
        B1800 = 1800U,
        B2400 = 2400U,
        B4800 = 4800U,
        B9600 = 9600U,
        B19200 = 19200U,
        B38400 = 38400U,
        B57600 = 57600U,
        B115200 = 115200U,
        B230400 = 230400U,
        B460800 = 460800U,
        B576000 = 576000U,
        B921600 = 921600U,
        B1000000 = 1000000U,
        B1152000 = 1152000U,
        B1500000 = 1500000U,
        B2000000 = 2000000U,
        B2500000 = 2500000U,
        B3000000 = 3000000U,
        B3500000 = 3500000U,
        B4000000 = 4000000U,
};

/**
 * @brief UART字长枚举
 */
enum xwds_uart_bits_em {
        XWDS_UART_BITS_8 = 0U,
        XWDS_UART_BITS_9,
        XWDS_UART_BITS_7,
};

/**
 * @brief UART停止位枚举
 */
enum xwds_uart_stopbits_em {
        XWDS_UART_STOPBITS_1_0 = 0U,
        XWDS_UART_STOPBITS_0_5,
        XWDS_UART_STOPBITS_2_0,
        XWDS_UART_STOPBITS_1_5,
};

/**
 * @brief UART奇偶校验枚举
 */
enum xwds_uart_parity_em {
        XWDS_UART_PARITY_NONE = 0U,
        XWDS_UART_PARITY_EVEN,
        XWDS_UART_PARITY_ODD,
};

/**
 * @brief UART模式枚举
 */
enum xwds_uart_mode_em {
        XWDS_UART_MODE_CLOSE = 0U,
        XWDS_UART_MODE_TX = XWBOP_BIT(0),
        XWDS_UART_MODE_RX = XWBOP_BIT(1),
};

/**
 * @brief UART硬件流控枚举
 */
enum xwds_uart_hardwareflowcontrol_em {
        XWDS_UART_HFC_NONE = 0U,
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
        enum xwds_uart_baudrate_em br = baudrate;
        return ((B50 == br) || (B75 == br) || (B110 == br) ||
                (B134 == br) || (B150 == br) || (B200 == br) ||
                (B300 == br) || (B600 == br) || (B1200 == br) ||
                (B1800 == br) || (B2400 == br) || (B4800 == br) ||
                (B9600 == br) || (B19200 == br) || (B38400 == br) ||
                (B57600 == br) || (B115200 == br) ||
                (B230400 == br) || (B460800 == br) ||
                (B576000 == br) || (B921600 == br) ||
                (B1000000 == br) || (B1152000 == br) ||
                (B1500000 == br) || (B2000000 == br) ||
                (B2500000 == br) || (B3000000 == br) ||
                (B3500000 == br) || (B4000000 == br));
}

#endif /* xwcd/ds/uart/common.h */
