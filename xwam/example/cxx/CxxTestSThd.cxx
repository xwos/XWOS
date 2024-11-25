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

#include "xwam/example/cxx/CxxTestSThd.hxx"
#include <vector>
#include "test/vector.hxx"
#if !defined(__clang__)
#  include "test/literal.hxx"
#  include "test/exception.hxx"
#endif

__xwcc_alignl1cache xwstk_t CxxTestSThd::sStack[1024];

CxxTestSThd::CxxTestSThd()
    : SThd("cxx.thd", sStack, sizeof(sStack))
{
}

CxxTestSThd::~CxxTestSThd()
{
}

xwer_t CxxTestSThd::thdMainFunction()
{
#if !defined(__clang__)
    testLiteralOperator();
#endif
    testStdVector();
#if !defined(__clang__)
    testStdExcept(1);
#endif

    while (!shouldStop()) {
        if (shouldFreeze()) {
            freeze();
        }
        sleep(xwtm_ms(2000));
    }
    detach();
    return XWOK;
}
