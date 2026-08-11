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

#ifndef __xwam_sample_cxx_basic_Thd_SThd_hxx__
#define __xwam_sample_cxx_basic_Thd_SThd_hxx__

#include <xwos/standard.hxx>
#include <xwos/cxx/SThd.hxx>

namespace sample {
namespace cxx {
namespace basic {
namespace Thd {

#define CPU0THD_THD_STACK_SIZE (2048U)
#define CPU0THD_THD_PRIORITY XWOS_SKD_PRIORITY_DROP(XWOS_SKD_PRIORITY_RT_MAX, 0)

/**
 * @brief CPU0上的静态线程模板
 */
class Cpu0Thd
    : public xwos::SThd<0> /* `0` 表示CPU0 */
{
  public:
    void init(); /**< 线程初始化，必须在其所在的CPU上调用，线程才会启动 */
  private:
    Cpu0Thd(xwstk_t stack[], xwsz_t stack_size);
    ~Cpu0Thd();
    virtual xwer_t thdMainFunction() override; /**< 线程函数 */

  public:
    static Cpu0Thd sInstance; /**< 单例模式 */
    static xwstk_t sThdStack[CPU0THD_THD_STACK_SIZE / sizeof(xwstk_t)]; /**< 线程栈 */
    static const xwtm_t skCfgLoopPeriod = XWTM_MS(1000); /**< 轮询周期 */
};


#define CPU1THD_THD_STACK_SIZE (2048U)
#define CPU1THD_THD_PRIORITY XWOS_SKD_PRIORITY_DROP(XWOS_SKD_PRIORITY_RT_MAX, 0)

/**
 * @brief CPU1上的静态线程模板
 */
class Cpu1Thd
    : public xwos::SThd<1> /* `1` 表示CPU1 */
{
  public:
    void init(); /**< 线程初始化，必须在其所在的CPU上调用，线程才会启动 */
  private:
    Cpu1Thd(xwstk_t stack[], xwsz_t stack_size);
    ~Cpu1Thd();
    virtual xwer_t thdMainFunction() override; /**< 线程函数 */

  public:
    static Cpu1Thd sInstance; /**< 单例模式 */
    static xwstk_t sThdStack[CPU1THD_THD_STACK_SIZE / sizeof(xwstk_t)]; /**< 线程栈 */
    static const xwtm_t skCfgLoopPeriod = XWTM_MS(1000); /**< 轮询周期 */
};

} // namespace Thd
} // namespace basic
} // namespace cxx
} // namespace sample

#endif /* xwam/sample/cxx/basic/Thd/SThd.hxx */
