/**
 * @file
 * @brief 架构描述层：image标记
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This file is a free software and is licensed under
 * > the Apache License, Version 2.0 (the "License").
 * > You may not use this file except in compliance with the License.
 * > You may obtain a copy of the License at
 * >         <http://www.apache.org/licenses/LICENSE-2.0/>.
 */

#ifndef __arch_image_flag_h__
#define __arch_image_flag_h__

#include <xwos/standard.h>

struct arch_image_header {
        void * tailflag_addr;
        void * end_addr;
        void (*prolog)(void);
        void (*entry)(void);
        void (*epilog)(void);
};

struct arch_image_tail {
        xwu64_t flag1;
        xwu64_t flag2;
        __xwcc_aligned(16) xwu8_t aligned_bytes[16];
};

extern const struct arch_image_header arch_image_header;
extern const struct arch_image_tail arch_image_tail;

#endif /* arch_image_flag.h */
