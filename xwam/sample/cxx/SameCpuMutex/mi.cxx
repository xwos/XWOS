/**
 * @file
 * @brief sample::SameCpuMutex::Consumer
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

#include "xwam/sample/cxx/SameCpuMutex/Consumer.hxx"
#include "xwam/sample/cxx/SameCpuMutex/Producer.hxx"

extern "C" {
void SampleSameCpuMutex_Consumer_init(void)
{
    sample::SameCpuMutex::Consumer::sInstance.init();
}

void SampleSameCpuMutex_Producer_init(void)
{
    sample::SameCpuMutex::Producer::sInstance.init();
}
}
