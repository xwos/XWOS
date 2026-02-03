/**
 * @file
 * @brief 玄武设备栈：I2C：协议通用的定义
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

#ifndef __xwcd_ds_i2c_common_h__
#define __xwcd_ds_i2c_common_h__

#include <xwcd/ds/standard.h>

/**
 * @defgroup xwcd_ds_i2c I2C
 * @ingroup xwcd_ds
 * @{
 */

/**
 * @brief I2C消息标志枚举
 */
enum xwds_i2c_msgflag_em {
        XWDS_I2C_F_10BITADDR = (1U), /**< 10位外设地址，BIT(0) */
        XWDS_I2C_F_ADDRMSK = (1U), /**< 地址掩码，BIT(0) */

        XWDS_I2C_F_RD = (2U), /**< 方向：读，BIT(1) */
        XWDS_I2C_F_DIRMSK = (2U), /**< 方向掩码，BIT(1) */

        XWDS_I2C_F_START = (4U), /**< 产生起始条件，BIT(2) */
        XWDS_I2C_F_STOP = (8U), /**< 产生结束条件，BIT(3) */
};

/**
 * @brief I2C消息
 */
struct xwds_i2c_msg {
        xwu16_t addr; /**< 外设地址，
                           + 7位地址模式，使用bit[0:6]
                           + 10位地址模式，使用bit[0:9] */
        xwu16_t flag; /**< 传输标志 */
        xwu8_t * data; /**< 数据缓存区的指针 */
        xwsz_t size; /** 数据缓存区的大小 */
};

/**
 * @} xwcd_ds_i2c
 */

#endif /* xwcd/ds/i2c/common.h */
