/**
 * @file
 * @brief S32K14x Timer
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

#ifndef __soc_tmr_h__
#define __soc_tmr_h__

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <soc.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      macros       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
enum soc_tmr_ftm_clksrc_em {
        SOC_TMR_FTM_CLKSRC_NONE = 0,
        SOC_TMR_FTM_CLKSRC_SYSCLK,
        SOC_TMR_FTM_CLKSRC_FIXFQCY_CLK,
        SOC_TMR_FTM_CLKSRC_EXT_CLK,
};

enum soc_tmr_lptmr_psr_pcs_em {
        SOC_TMR_LPTMR_PSR_PCS_SIRCDIV2_CLK = 0,
        SOC_TMR_LPTMR_PSR_PCS_LPO1K_CLK,
        SOC_TMR_LPTMR_PSR_PCS_RTC_CLK,
        SOC_TMR_LPTMR_PSR_PCS_PCC,
};

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         function prototypes         ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

#endif /* soc_tmr.h */
