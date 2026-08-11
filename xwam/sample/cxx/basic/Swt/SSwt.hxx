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

#ifndef __xwam_sample_cxx_basic_Swt_SSwt_hxx__
#define __xwam_sample_cxx_basic_Swt_SSwt_hxx__

#include <xwos/standard.hxx>
#include <xwos/cxx/SSwt.hxx>

namespace sample {
namespace cxx {
namespace basic {
namespace Swt {

/**
 * @brief CPU0上的静态软件定时器
 */
class Cpu0Swt
    : public xwos::SSwt<0> /* 0表示CPU0 */
{
  private:
    Cpu0Swt();
    ~Cpu0Swt();
    virtual void swtAlarmFunction() override; /**< 线程函数 */

  public:
    static Cpu0Swt sInstance; /**< 单例模式 */
    static const xwtm_t skCfgPeriod = XWTM_MS(1000); /**< 周期 */
};

} // namespace Swt
} // namespace basic
} // namespace cxx
} // namespace sample

#endif /* xwam/sample/cxx/basic/SSwt.hxx */
