/**
 * @file
 * @brief 板级描述层：MISC设备驱动
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

#ifndef __bdl_ds_misc_driver_h__
#define __bdl_ds_misc_driver_h__

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <soc_adc.h>
#include <xwos/osal/lock/spinlock.h>
#include <xwos/osal/lock/mutex.h>
#include <xwmd/ds/misc/chip.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
struct mpc560xb_misc_drvdata {
        struct mpc560xb_adc_drvdata {
                xwid_t tid; /**< 线程ID */
                struct xwosal_splk lock; /**< 自旋锁 */
                struct xwosal_mtx mtx; /**< 互斥锁 */
        } adc;
};

struct mpc560xb_misc_private_cfg {
        const struct soc_adc_private_cfg * adc; /**< ADC配置 */
};

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
extern __xwmd_rodata const
struct xwds_misc_driver mpc560xb_misc_drv;

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         function prototypes         ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
extern __xwmd_isr
void mpc560xb_adc_eoc_isr(void);

extern __xwmd_isr
void mpc560xb_adc_er_isr(void);

extern __xwmd_isr
void mpc560xb_adc_wd_isr(void);

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ******** inline  functions ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

#endif /* bdl/ds/driver/adc.h */
