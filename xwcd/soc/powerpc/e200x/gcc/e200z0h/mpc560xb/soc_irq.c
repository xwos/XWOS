/**
 * @file
 * @brief SOC Adaptation Code：IRQ
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

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <soc_sched.h>
#include <soc_irq.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief SOC Adaptation Function：Get Current IRQ Number
 */
__xwbsp_code
xwer_t soc_irq_get_id(xwirq_t * irqnbuf)
{
        xwer_t rc;
        xwirq_t irq;

        if (SOC_IRQN_NIL == soc_context.irqn) {
                rc = -ENOTINISR;
                irq = 0;
        } else {
                rc = XWOK;
                irq = soc_context.irqn;
        }
        if (!is_err_or_null(irqnbuf)) {
                *irqnbuf = irq;
        }
        return rc;
}

/**
 * @brief SOC default ISR
 */
__xwos_isr
void soc_isr_nop(void)
{
}
