/**
 * @file
 * @brief 设备栈驱动：UART0
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

#ifndef __bdl_ds_driver_uart0_h__
#define __bdl_ds_driver_uart0_h__

#include <xwos/standard.h>
#include <xwos/osal/lock/spinlock.h>
#include <xwos/osal/sync/cond.h>
#include <xwcd/ds/uart/controller.h>

struct mpc560xb_uart0_drvdata {
        struct {
                struct xwos_cond cond;
                struct xwos_splk splk;
                xwer_t rc;
        } tx;
};

extern __xwbsp_rodata const struct xwds_uartc_driver mpc560xb_uart0_drv;

void mpc560xb_uart0_err_isr(void);

#endif /* bdl/ds/driver/uart0.h */
