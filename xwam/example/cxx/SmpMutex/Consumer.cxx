/**
 * @file
 * @brief sample::SmpMutex::Consumer
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

#include "xwam/sample/cxx/SmpMutex/Consumer.hxx"
#include "xwam/sample/cxx/SmpMutex/Producer.hxx"

#define SAMPLE_DEBUG
#define LOGTAG "sample::SmpMutex::Consumer"
#include "xwam/sample/cxx/SmpMutex/Log.hxx"

namespace sample {
namespace SmpMutex {
/* Consumer Non-static Member */
Consumer::Consumer(xwstk_t stack[], xwsz_t stack_size)
    : SThd("SmpMutexSample::Consumer", stack, stack_size,
           XWOS_STACK_GUARD_SIZE_DEFAULT, CONSUMER_THD_PRIORITY)
{
}

Consumer::~Consumer()
{
}

void Consumer::init()
{
    xwer_t rc = launch();
    if (rc < 0) {
        sampleLogE("launch ... %d\r\n", rc);
    } else {
        sampleLogI("launch ... OK\r\n");
    }
}

xwer_t Consumer::thdMainFunction()
{
    xwu32_t copy;
    xwtm_t from = xwtm_now();
    while (!shouldStop()) {
        if (shouldFreeze()) {
            freeze();
        }
        {
            xwos::lock::Mtx::Grd grd(Producer::sInstance.mMutex);
            copy = Producer::sInstance.mData;
        }
        sampleLogI("Read: %d\r\n", copy);
        sleepFrom(&from, skCfgLoopPeriod);
    }
    return XWOK;
}

/* Consumer Static Member */
Consumer Consumer::sInstance(sThdStack, sizeof(sThdStack));
xwstk_t Consumer::sThdStack[CONSUMER_THD_STACK_SIZE / sizeof(xwstk_t)];

} // namespace SmpMutex
} // namespace sample
