/**
 * @file
 * @brief 设备栈驱动：MPC560xB UART
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

#ifndef __driver_ds_uartc_h__
#define __driver_ds_uartc_h__

#include <xwos/standard.h>
#include <xwcd/ds/device.h>
#include <xwcd/ds/uart/general.h>

extern const struct xwds_uartc_driver mpc560xb_uartc_drv;

void mpc560xb_uartc_rx_isr(void);
void mpc560xb_uartc_tx_isr(void);
void mpc560xb_uartc_err_isr(void);

#endif /* uart.h */
