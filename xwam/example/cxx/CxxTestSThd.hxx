/**
 * @file
 * @brief 示例：C++测试线程
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

#ifndef __xwam_example_cxx_CxxTestSThd_hxx__
#define __xwam_example_cxx_CxxTestSThd_hxx__

#include <xwos/standard.hxx>
#include <xwos/cxx/SThd.hxx>

class CxxTestSThd : public xwos::SThd
{
  public:
    CxxTestSThd();
    ~CxxTestSThd();

  private:
    virtual xwer_t thdMainFunction() override;

  private:
    static xwstk_t sStack[1024];
};

#endif /* xwam/example/cxx/CxxTestSThd.hxx */
