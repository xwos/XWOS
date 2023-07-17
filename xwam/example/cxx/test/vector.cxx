/**
 * @file
 * @brief C++ Vector Test
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

#include <cstdio>
#include <vector>
#include "test/vector.hxx"

class Data {
 public:
  Data(int v);
  ~Data();
 public:
  int mVal;
};

Data::Data(int v) :
    mVal(v)
{
}

Data::~Data()
{
}

/* Test static construct in .init_array */
Data gData(1);

void testStdVector(void)
{
  std::vector<Data> array;

  printf("++++++++ C++ std::vector test ++++++++\n");
  Data * d = new Data(2);
  array.push_back(gData);
  array.push_back(*d);

  auto sz = array.size();
  printf("After inserting, vector size: %d\n", sz);

  printf("vector: ");
  auto it = array.begin();
  for (; it != array.end(); ++it) {
    printf("%d ", it->mVal);
  }
  printf("\n");
  array.erase(it);
  sz = array.size();
  printf("After erasing, vector size: %d\n", sz);
  printf("-------- C++ std::vector test --------\n");
  delete d;
}
