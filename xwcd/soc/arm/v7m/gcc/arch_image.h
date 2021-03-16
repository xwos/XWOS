/**
 * @file
 * @brief 架构描述层：image标记
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

#ifndef __arch_image_h__
#define __arch_image_h__

#include <xwos/standard.h>

struct arch_image_description {
        void * head;
        void * tail_flag_addr;
        void * end_addr;
        void (* entry)(void);
};

struct __xwcc_aligned(32) arch_image_tail {
        char flag[32];
};

extern const struct arch_image_description arch_image_description;

extern const struct arch_image_tail arch_image_tail;

xwer_t arch_boot_image(void * addr);

#endif /* arch_image.h */
