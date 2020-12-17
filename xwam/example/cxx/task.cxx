/**
 * @file
 * @brief 示例：C++测试线程
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
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
#include <xwos/osal/skd.hxx>
#include <vector>
#include "test/literal.hxx"
#include "test/vector.hxx"
#include "task.hxx"

xwer_t cxx_thd_main(void * arg)
{
  xwtm_t xwtm;
  xwer_t rc;

  XWOS_UNUSED(arg);

  testLiteralOperator();
  testStdVector();

  rc = XWOK;
  while (!xwos_cthd_frz_shld_stop(NULL)) {
    xwtm = 2000 * XWTM_MS;
    xwos_cthd_sleep(&xwtm);
  }
  xwos_thd_detach(xwos_cthd_self());
  return rc;
}
