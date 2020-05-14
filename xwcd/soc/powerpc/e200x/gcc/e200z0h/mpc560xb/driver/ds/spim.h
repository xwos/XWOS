/**
 * @file
 * @brief 设备栈驱动：MPC560xB SPI master mode controller
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

#ifndef __driver_ds_spim_h__
#define __driver_ds_spim_h__

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <xwos/osal/lock/spinlock.h>
#include <xwos/osal/sync/condition.h>
#include <xwmd/ds/device.h>
#include <xwmd/ds/spi/master.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
struct mpc560xb_spim_drvdata {
        struct xwosal_cdt cdt; /**< condition */
        struct xwds_spim_msg * msg; /**< current message */
        xwsq_t txpos; /**< position of TX queue */
        xwsq_t rxpos; /**< position of RX queue */
        struct xwosal_splk lock; /**< lock to protect msttid */
};

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
extern __xwbsp_rodata const struct xwds_spim_driver mpc560xb_spim_drv;

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         function prototypes         ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
__xwbsp_isr
void mpc560xb_spim_isr_tfuf_rfof(void);

__xwbsp_isr
void mpc560xb_spim_isr_tfff(void);

__xwbsp_isr
void mpc560xb_spim_isr_rfdf(void);

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ******** inline  functions ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

#endif /* spim.h */
