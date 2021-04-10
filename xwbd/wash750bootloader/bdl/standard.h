/**
 * @file
 * @brief 板级描述层：标准头文件
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

#ifndef __bdl_standard_h__
#define __bdl_standard_h__

#include <xwos/standard.h>

#define BDL_DEBUG

extern void bm_reset(void);

#ifdef BDL_DEBUG
  #define BDL_BUG()             XWOS_BUG()
  #define BDL_BUG_ON(x)         XWOS_BUG_ON(x)

#else
  #define BDL_BUG()             bm_reset()
  #define BDL_BUG_ON(x)         if (__xwcc_unlikely(x)) BDL_BUG()
#endif

#endif /* bdl/standard.h */
