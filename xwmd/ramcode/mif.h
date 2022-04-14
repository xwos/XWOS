/**
 * @file
 * @brief XWOS Ramcode：模块接口
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
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
        xwer_t (* load)(struct ramcode * /*ramcode*/,
                        xwu8_t * /*loadbuf*/, xwsz_t * /*bufsize*/,
                        xwtm_t /*to*/);
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
