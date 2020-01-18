/**
 * @file
 * @brief C++ Vector Operator Test
 * @author
 * + 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <www.starsoul.tech>
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

#include <vector>
#include "test/vector.hxx"

class Data {
 public:
  Data(int v);
  ~Data();
 public:
  int mVersion;
};

Data::Data(int v) :
    mVersion(v)
{
}

Data::~Data()
{
}

/* Test static construct in .init_array */
Data gData(1);

void testStdVector(void)
{
  ::std::vector<Data> array;

  Data * d = new Data(2);
  array.push_back(gData);
  array.push_back(*d);

  auto sz = array.size();
  (void)sz;

  auto it = array.begin();
  for (; it != array.end(); ++it) {
    if (2 == it->mVersion) {
      break;
    }
  }
  array.erase(it);
  sz = array.size();
  delete d;
}
