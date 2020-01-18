/**
 * @file
 * @brief 板级描述层：初始化
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

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <xwos/smp/irq.h>
#include <smp_nvic.h>
#include <bdl/standard.h>
#include <bdl/ds/description/s32k14xdkp.h>
#include <bdl/ds/xwds.h>
#include <bdl/board_init.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      macros       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         function prototypes         ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
static __xwos_init_code
xwer_t sys_mm_init(void);

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
__xwos_init_code
void board_lowlevel_init(void)
{
}

__xwos_init_code
void board_init(void)
{
        xwer_t rc;

        /* nvic data */
        xwos_irqc_set_data(&cortexm_nvic[0], (void *)&soc_irq_data_table);

        /* memory management */
        rc = sys_mm_init();
        BDL_BUG_ON(rc < 0);

        /* device stack */
        rc = bdl_xwds_init(&s32k14x_ds);
        BDL_BUG_ON(rc < 0);
}

static __xwos_init_code
xwer_t sys_mm_init(void)
{
        return OK;
}
