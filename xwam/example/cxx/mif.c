/**
 * @file
 * @brief 示例：C++
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

#include <xwos/standard.h>
#include <xwos/osal/thd.h>
#include <xwam/example/cxx/mif.h>

#define CXX_TASK_PRIORITY (XWOS_SKD_PRIORITY_RAISE(XWOS_SKD_PRIORITY_RT_MIN, 1))

extern xwer_t cxx_thd_main(void * arg);

struct xwos_thd * cxx_thd;

xwer_t example_cxx_start(void)
{
        struct xwos_thd_attr attr;
        xwer_t rc;

        xwos_thd_attr_init(&attr);
        attr.name = "cxx.thd";
        attr.stack = NULL;
        attr.stack_size = 4096;
        attr.priority = CXX_TASK_PRIORITY;
        attr.detached = false;
        attr.privileged = true;
        rc = xwos_thd_create(&cxx_thd, &attr, cxx_thd_main, NULL);
        return rc;
}
