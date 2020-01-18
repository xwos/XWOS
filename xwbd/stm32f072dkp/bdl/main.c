/**
 * @file
 * @brief 应用程序入口
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
#include <xwos/osal/scheduler.h>
#include <bdl/standard.h>
#include <bm/stm32cube/init.h>
#include <bm/xwtst/sync/barrier/demo.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         function prototypes         ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
xwer_t xwos_main(void)
{
        xwer_t rc;

        /* stm32cube init */
        rc = bm_stm32cube_start();
        if (rc < 0) {
                goto bm_stm32cube_start;
        }

        rc = bm_xwtst_sync_barrier_start();
        if (rc < 0) {
                goto err_bm_xwtst_sync_barrier_start;
        }

        rc = xwosal_scheduler_start_lc();
        if (rc < 0) {
                goto err_scheduler_start_lc;
        }
        return OK;

err_bm_xwtst_sync_barrier_start:
        BDL_BUG();
err_scheduler_start_lc:
        BDL_BUG();
bm_stm32cube_start:
        BDL_BUG();
        return rc;
}
