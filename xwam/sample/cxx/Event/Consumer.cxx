/**
 * @file
 * @brief sample::Event::Consumer
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

#include "xwam/sample/cxx/Event/Consumer.hxx"

#define SAMPLE_DEBUG
#define LOGTAG "sample::Event::Consumer"
#include "xwam/sample/cxx/Event/Log.hxx"

namespace sample {
namespace Event {
/* Consumer Non-static Member */
Consumer::Consumer(xwstk_t stack[], xwsz_t stack_size)
    : SThd("Event::Consumer", stack, stack_size,
           XWOS_STACK_GUARD_SIZE_DEFAULT, EVENT_CONSUMER_THD_PRIORITY)
    , mFlg()
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
    }
}

xwer_t Consumer::thdMainFunction()
{
    xwer_t rc;
    xwtm_t from;
    xwtm_t to;
    xwos::Bmp<EVENT_NUM> bmp;
    xwos::Bmp<EVENT_NUM> msk;
    msk.set(0UL);
    msk.set(1UL);
    msk.set(2UL);
    msk.set(3UL);
    msk.set(4UL);
    msk.set(10UL);
    msk.set(31UL);
    from = xwtm_now();
    to = from + skCfgLoopPeriod;
    while (!shouldStop()) {
        if (shouldFreeze()) {
            freeze();
        }
        rc = mFlg.wait(xwos::sync::Flg<EVENT_NUM>::Trigger::FlgSetAny,
                       true, &bmp, &msk,
                       xwos::sync::Flg<EVENT_NUM>::WaitMode::FlgWaitTimed,
                       to);
        if (XWOK == rc) {
            auto bit0 = bmp[0U];
            if (bit0) {
                sampleLogI("Event ... 0\r\n");
            }
            auto bit1 = bmp[1U];
            if (bit1) {
                sampleLogI("Event ... 1\r\n");
            }
            auto bit2 = bmp[2U];
            if (bit2) {
                sampleLogI("Event ... 2\r\n");
            }
            auto bit3 = bmp[3U];
            if (bit3) {
                sampleLogI("Event ... 3\r\n");
            }
            auto bit4 = bmp[4U];
            if (bit4) {
                sampleLogI("Event ... 4\r\n");
            }
            auto bit10 = bmp[10U];
            if (bit10) {
                sampleLogI("Event ... 10\r\n");
            }
            auto bit31 = bmp[31U];
            if (bit31) {
                sampleLogI("Event ... 31\r\n");
            }
        } else if (-ETIMEDOUT == rc) {
            from = xwtm_now();
            if (from >= to) {
                to += skCfgLoopPeriod;
            }
        } else {
            sampleLogE("mFlg.wait() ... rc:%d.\r\n", rc);
        }
    }
    return XWOK;
}

/* Consumer Static Member */
Consumer Consumer::sInstance(sThdStack, sizeof(sThdStack));
xwstk_t Consumer::sThdStack[EVENT_CONSUMER_THD_STACK_SIZE / sizeof(xwstk_t)];

} // namespace Event
} // namespace sample
