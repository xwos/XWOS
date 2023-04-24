/**
 * @file
 * @brief 架构描述层：firmware标记
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This file is a free software and is licensed under
 * > the Apache License, Version 2.0 (the "License").
 * > You may not use this file except in compliance with the License.
 * > You may obtain a copy of the License at
 * >         <http://www.apache.org/licenses/LICENSE-2.0/>.
 */

#ifndef __arch_firmware_h__
#define __arch_firmware_h__

#include <xwos/standard.h>

struct firmware_info {
        void * head;
        void * tail_flag_addr;
        void * end_addr;
        void (* entry)(void);
        xwstk_t * sp;
};

struct __xwcc_aligned(32) firmware_tail {
        char flag[32];
};

xwer_t arch_boot_firmware(void * firmware, xwsz_t info_offset, const char * tailflag);

#endif /* arch_firmware.h */
