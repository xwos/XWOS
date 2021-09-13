/**
 * @file
 * @brief 示例：C++
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

#include <xwos/standard.h>
#include <xwos/osal/skd.h>
#include <xwam/example/cxx/mif.h>

#define CXX_TASK_PRIORITY (XWOS_SKD_PRIORITY_RAISE(XWOS_SKD_PRIORITY_RT_MIN, 1))

extern xwer_t cxx_thd_main(void * arg);

const struct xwos_thd_desc cxx_thd_desc[] = {
        [0] = {
                .name = "task.cxx",
                .prio = CXX_TASK_PRIORITY,
                .stack = NULL,
                .stack_size = 4096,
                .func = cxx_thd_main,
                .arg = NULL,
                .attr = XWOS_SKDATTR_PRIVILEGED,
        },
};

struct xwos_thd * cxx_thd[xw_array_size(cxx_thd_desc)];

xwer_t example_cxx_start(void)
{
        xwer_t rc;
        xwsq_t i;

        for (i = 0; i < xw_array_size(cxx_thd_desc); i++) {
                rc = xwos_thd_create(&cxx_thd[i],
                                     cxx_thd_desc[i].name,
                                     cxx_thd_desc[i].func,
                                     cxx_thd_desc[i].arg,
                                     cxx_thd_desc[i].stack_size,
                                     cxx_thd_desc[i].prio,
                                     cxx_thd_desc[i].attr);
                if (rc < 0) {
                        break;
                }
        }
        return rc;
}
