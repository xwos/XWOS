/**
 * @file
 * @brief sample::cxx::basic::Swt::SSwt
 * @author
 * + 隐星曜 (Roy Sun) <xwos@xwos.tech>
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

#include "xwam/sample/cxx/basic/Swt/SSwt.hxx"
#include <xwos/lib/xwlog.hxx>

namespace sample {
namespace cxx {
namespace basic {
namespace Swt {

/* Cpu0Swt Non-static Member */
Cpu0Swt::Cpu0Swt()
    : SSwt(XWOS_SWT_FLAG_RESTART) /* 自动重载 */
{
}

Cpu0Swt::~Cpu0Swt()
{
}

void Cpu0Swt::swtAlarmFunction()
{
    /* 当 XWOSCFG_SKD_BH == 1，此函数运行在中断底半部上下文 */
    /* 当 XWOSCFG_SKD_BH == 0，此函数运行在中断顶半部上下文 */
}

/* Cpu0Swt Static Member */
Cpu0Swt Cpu0Swt::sInstance;

} // namespace Swt
} // namespace basic
} // namespace cxx
} // namespace sample

/* C Interface */
extern "C" {
void CxxbasicSample_Cpu0Swt_start(void)
{
    xwer_t rc;
    xwtm_t now = xwtm_now();

    rc = sample::cxx::basic::Swt::Cpu0Swt::sInstance.start(now, sample::cxx::basic::Swt::Cpu0Swt::skCfgPeriod);
    if (rc < 0) {
        xwlogf(E, "Cpu0Swt", "start ... %d\r\n", rc);
    }
}

void CxxbasicSample_Cpu0Swt_stop(void)
{
    xwer_t rc;

    rc = sample::cxx::basic::Swt::Cpu0Swt::sInstance.stop();
    if (rc < 0) {
        xwlogf(E, "Cpu0Swt", "stop ... %d\r\n", rc);
    }
}
}
