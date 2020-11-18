/**
 * @file
 * @brief W25Qxx编程器：UART硬件接口
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

#ifndef __xwam_application_w25qrpt_hwif_uart_h__
#define __xwam_application_w25qrpt_hwif_uart_h__

#include <xwam/application/w25qrpt/hwifal.h>

extern
const struct w25qrpt_hwifal_operations w25qrptif_uart_ops;

#endif /* xwam/application/w25qrpt/hwif/uart.h */
