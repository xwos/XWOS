/**
 * @file
 * @brief 设备栈驱动：MPC560xb DMA UART0
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

#ifndef __driver_ds_dmauart0_h__
#define __driver_ds_dmauart0_h__

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <xwos/osal/lock/spinlock.h>
#include <xwos/osal/sync/condition.h>
#include <xwmd/ds/uart/dma.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
struct mpc560xb_dmauart0_drvdata {
        struct {
                struct xwosal_cdt cdt;
                struct xwosal_splk splk;
                xwer_t rc;
        } tx;
};

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
extern __xwbsp_rodata const struct xwds_dmauartc_driver mpc560xb_dmauart0_drv;

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         function prototypes         ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
extern __xwbsp_isr
void mpc560xb_dmauart0_err_isr(void);

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ******** inline  functions ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

#endif /* driver/ds/dmauart0.h */
