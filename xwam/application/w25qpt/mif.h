/**
 * @file
 * @brief W25Qxx编程器
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

#ifndef __xwam_application_w25qpt_mif_h__
#define __xwam_application_w25qpt_mif_h__

#include <xwos/standard.h>
#include <xwcd/peripheral/spi/flash/w25qxx/device.h>
#include <xwam/application/w25qpt/w25qpt.h>
#include <xwam/application/w25qpt/hwifal.h>
#include <xwam/application/w25qpt/hwif/uart.h>

xwer_t w25qpt_start(struct w25qpt * w25qpt,
                    const char * name,
                    struct xwds_w25qxx * flash,
                    const struct w25qpt_hwifal_operations * hwifops,
                    void * hwifcb);

xwer_t w25qpt_stop(struct w25qpt * w25qpt);

#endif /* xwam/application/w25qpt/mif.h */
