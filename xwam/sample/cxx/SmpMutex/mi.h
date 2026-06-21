/**
 * @file
 * @brief Sample::SmpMutex::mi
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

#ifndef __xwam_sample_cxx_SmpMutex_mi_h__
#define __xwam_sample_cxx_SmpMutex_mi_h__

#include <xwos/standard.h>

/**
 * @brief 生产者线程初始化
 * @note
 * 在CPU0上调用
 */
void SampleSmpMutex_Producer_init(void);

/**
 * @brief 消费者线程初始化
 * @note
 * 在CPU1上调用
 */
void SampleSmpMutex_Consumer_init(void);

#endif /* xwam/sample/cxx/SmpMutex/mi.h */
