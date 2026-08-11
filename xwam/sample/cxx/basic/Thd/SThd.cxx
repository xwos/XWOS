/**
 * @file
 * @brief sample::cxx::basic::Thd::SThd
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

#include "xwam/sample/cxx/basic/Thd/SThd.hxx"
#include <xwos/osal/time.hxx>
#include <xwos/lib/xwlog.hxx>

namespace sample {
namespace cxx {
namespace basic {
namespace Thd {

/* Cpu0Thd Non-static Member */
Cpu0Thd::Cpu0Thd(xwstk_t stack[], xwsz_t stack_size)
    : SThd("sample::cxx::basic::Cpu0Thd", stack, stack_size,
           XWOS_STACK_GUARD_SIZE_DEFAULT, CPU0THD_THD_PRIORITY)
{
}

Cpu0Thd::~Cpu0Thd()
{
}

void Cpu0Thd::init()
{
    xwer_t rc = launch();
    if (rc < 0) {
        xwlogf(E, "Cpu0Thd", "launch ... %d\r\n", rc);
    }
}

xwer_t Cpu0Thd::thdMainFunction()
{
    xwtm_t from = xwtm_now();
    while (!shouldStop()) {
        if (shouldFreeze()) {
            freeze();
        }
        xwlogf(I, "Cpu0Thd", "Sleep 1s ...\r\n");
        sleepFrom(&from, skCfgLoopPeriod);
    }
    return XWOK;
}

/* Cpu1Thd Static Member */
Cpu0Thd Cpu0Thd::sInstance(sThdStack, sizeof(sThdStack));
xwstk_t Cpu0Thd::sThdStack[CPU1THD_THD_STACK_SIZE / sizeof(xwstk_t)];

/* Cpu1Thd Non-static Member */
Cpu1Thd::Cpu1Thd(xwstk_t stack[], xwsz_t stack_size)
    : SThd("sample::cxx::basic::Cpu1Thd", stack, stack_size,
           XWOS_STACK_GUARD_SIZE_DEFAULT, CPU1THD_THD_PRIORITY)
{
}

Cpu1Thd::~Cpu1Thd()
{
}

void Cpu1Thd::init()
{
    xwer_t rc = launch();
    if (rc < 0) {
        xwlogf(E, "Cpu1Thd", "launch ... %d\r\n", rc);
    }
}

xwer_t Cpu1Thd::thdMainFunction()
{
    xwtm_t from = xwtm_now();
    while (!shouldStop()) {
        if (shouldFreeze()) {
            freeze();
        }
        xwlogf(I, "Cpu1Thd", "Sleep 1s ...\r\n");
        sleepFrom(&from, skCfgLoopPeriod);
    }
    return XWOK;
}

/* Cpu1Thd Static Member */
Cpu1Thd Cpu1Thd::sInstance(sThdStack, sizeof(sThdStack));
xwstk_t Cpu1Thd::sThdStack[CPU1THD_THD_STACK_SIZE / sizeof(xwstk_t)];

} // namespace Thd
} // namespace basic
} // namespace cxx
} // namespace sample

/* C Interface */
extern "C" {
void CxxbasicSample_Cpu0Thd_init(void)
{
    sample::cxx::basic::Thd::Cpu0Thd::sInstance.init();
}

void CxxbasicSample_Cpu1Thd_init(void)
{
    sample::cxx::basic::Thd::Cpu1Thd::sInstance.init();
}
}
