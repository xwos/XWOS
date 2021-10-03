/**
 * @file
 * @brief C++ Exception Test
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

#include <xwos/lib/xwlog.hxx>
#include <stdexcept>
#include "test/exception.hxx"

#undef LOGTAG
#undef cxxlogf
#define LOGTAG "C++Except"
#define cxxlogf(lv, fmt, ...) xwlogf(lv, LOGTAG, fmt, ##__VA_ARGS__)

void throwFunc(std::out_of_range * e)
{
  throw e;
}

void testStdExcept(int a)
{
  try {
    if (a > 100) {
      throwFunc(new std::out_of_range("GT 100"));
    } else {
      throwFunc(new std::out_of_range("LE 100"));
    }
  } catch (std::out_of_range * e) {
    cxxlogf(INFO, "throw exception:%s\n", e->what());
    delete e;
  }
}
