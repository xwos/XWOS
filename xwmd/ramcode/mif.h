/**
 * @file
 * @brief XWOS Ramcode：模块接口
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

#ifndef __xwmd_ramcode_mif_h__
#define __xwmd_ramcode_mif_h__

#include <xwos/standard.h>

struct ramcode;

struct ramcode_info {
        char head[8];
        xwptr_t origin;
        xwsz_t size;
        xwu8_t crc32[4];
};

struct ramcode_operation {
        xwer_t (* load)(struct ramcode *,
                        xwu8_t *, xwsz_t *,
                        xwtm_t *);
};

struct ramcode {
        struct ramcode_info info;
        const struct ramcode_operation * op;
        void * opcb;
};

xwer_t ramcode_init(struct ramcode * ramcode,
                    const struct ramcode_operation * op,
                    void * opcb);
xwer_t ramcode_load(struct ramcode * ramcode);
xwer_t ramcode_boot(struct ramcode * ramcode);

#endif /* xwmd/ramcode/mif.h */
