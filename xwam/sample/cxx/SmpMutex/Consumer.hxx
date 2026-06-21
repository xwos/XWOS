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

#ifndef __xwam_sample_cxx_SmpMutex_Consumer_hxx__
#define __xwam_sample_cxx_SmpMutex_Consumer_hxx__

#include <xwos/standard.hxx>
#include <xwos/cxx/SThd.hxx>
#include <xwos/cxx/lock/SMtx.hxx>

namespace sample {
namespace SmpMutex {

#define CONSUMER_THD_STACK_SIZE (2048U)
#define CONSUMER_THD_PRIORITY XWOS_SKD_PRIORITY_DROP(XWOS_SKD_PRIORITY_RT_MAX, 0)

class Consumer
    : public xwos::SThd<1>
{
  public:
    void init();
  private:
    Consumer(xwstk_t stack[], xwsz_t stack_size);
    ~Consumer();
    virtual xwer_t thdMainFunction() override;

  public:
    static Consumer sInstance; /**< 单例模式 */
    static xwstk_t sThdStack[CONSUMER_THD_STACK_SIZE / sizeof(xwstk_t)]; /**< Thread Stack */
    static const xwtm_t skCfgLoopPeriod = XWTM_MS(1000); /**< 轮询周期 */
};

} // namespace SmpMutex
} // namespace sample

#endif /* xwam/sample/cxx/Mutex/Consumer.hxx */
