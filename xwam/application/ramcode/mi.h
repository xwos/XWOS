/**
 * @file
 * @brief XWOS Ramcode：模块接口
 * @author
 * + 隐星曜 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef __xwam_application_ramcode_mi_h__
#define __xwam_application_ramcode_mi_h__

#include <xwos/standard.h>

struct ramcode;

struct ramcode_info {
        char tag[8]; /* "XWOS.RC" */
        xwptr_t origin;
        xwsz_t size;
        xwu32_t crc32;
};

struct ramcode_operation {
        xwer_t (* load)(struct ramcode * /*ramcode*/,
                        xwu8_t * /*loadbuf*/, xwsz_t * /*bufsize*/,
                        xwtm_t /*to*/);
};

struct ramcode {
        struct ramcode_info info;
        xwsz_t firmware_info_offset;
        const char * firmware_tailflag;
        const struct ramcode_operation * op;
        void * opcb;
};

xwer_t ramcode_init(struct ramcode * ramcode,
                    const struct ramcode_operation * op, void * opcb,
                    xwsz_t firmware_info_offset,
                    const char * firmware_tailflag);
xwer_t ramcode_load(struct ramcode * ramcode);
xwer_t ramcode_boot(struct ramcode * ramcode);

#endif /* xwam/application/ramcode/mi.h */
