/**
 * @file
 * @brief xwmd设备栈：I2C协议通用的定义
 * @author
 * + 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __xwmd_ds_i2c_common_h__
#define __xwmd_ds_i2c_common_h__

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <xwos/lib/xwbop.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define SODS_I2C_10BITADDR_LEADING_BYTE_MASK    ((xwu8_t)0xF0)

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief I2C消息标志枚举
 */
enum xwds_i2c_msgflag_em {
        SODS_I2C_F_NONE = 0,
        SODS_I2C_F_10BITADDR = BIT(0), /**< 10位外设地址 */
        SODS_I2C_F_RD = BIT(1), /**< 方向：读（若此位为0，方向为写）*/
        SODS_I2C_F_STOP = BIT(2), /**< 产生结束条件 */
};

/**
 * @brief I2C消息
 */
struct xwds_i2c_msg {
        xwu16_t addr; /**< 外设地址 */
        xwu16_t flags; /**< 消息标志 */
        xwu8_t * buf; /**< 消息数据缓存的指针 */
        xwsz_t size; /** 消息数据大小 */
};

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         function prototypes         ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********  inline functions implementations   ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

#endif /* xwmd/ds/i2c/common.h */
