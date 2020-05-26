/**
 * @file
 * @brief S32K14x Log
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
#include <core_cm.h>
#include <soc_dbg.h>
#include <soc_log.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      macros       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         function prototypes         ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
static __xwbsp_code
void cortexm_itm_init(void);

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief Init cortexm ITM
 */
static __xwbsp_code
void cortexm_itm_init(void)
{
        cm_itm.lar.w = 0xC5ACCE55; /* key to access the ITM registers */
        cm_itm.ter[0].w = 0x1; /* enable ITM port 0 */
        cm_itm.tpr.w = 0x1; /* enable unprivileged code */
}

/**
 * @brief Init hardware log system
 */
__xwbsp_code
void soc_log_init(void)
{
        cortexm_itm_init();
}

/**
 * @brief Send a string to debug port
 * @param s: string
 * @param n: max length of string
 */
__xwbsp_code
xwssz_t soc_log_write(const char * s, xwsz_t n)
{
        return cm_itm_write(0, s, n);
}
