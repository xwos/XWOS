/**
 * @file
 * @brief C++ Literal Operator Test
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

#ifndef __xwam_example_cxx_test_literal_hxx__
#define __xwam_example_cxx_test_literal_hxx__

#include <string>

inline void LiteralOperator_s(::std::string & s, char c)
{
  s.push_back(c);
}

template<typename...types>
void LiteralOperator_s(::std::string & s, char c, types... args)
{
  s.push_back(c);
	LiteralOperator_s(s, args...);
}

template <char... chlst>
::std::string operator ""_s()
{
  ::std::string s;
  s.erase();
  LiteralOperator_s(s, chlst...);
  return s;
}

template <typename T, T... chlst>
::std::string operator ""_s()
{
  ::std::string s;
  s.erase();
  LiteralOperator_s(s, chlst...);
  return s;
}

void testLiteralOperator();

#endif /* xwam/example/cxx/test/literal.hxx */
