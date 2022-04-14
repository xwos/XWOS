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

#include <xwos/standard.hxx>
#include <xwos/osal/thd.hxx>
#include <vector>
#include "test/vector.hxx"
#if !defined(__clang__)
#  include "test/literal.hxx"
#  include "test/exception.hxx"
#endif
#include "task.hxx"

xwer_t cxx_thd_main(void * arg)
{
  xwer_t rc;

  XWOS_UNUSED(arg);

#if !defined(__clang__)
  testLiteralOperator();
#endif
  testStdVector();
#if !defined(__clang__)
  testStdExcept(1);
#endif

  rc = XWOK;
  while (!xwos_cthd_frz_shld_stop(NULL)) {
    xwos_cthd_sleep(2000 * XWTM_MS);
  }
  xwos_thd_detach(xwos_cthd_self());
  return rc;
}
