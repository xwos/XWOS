/**
 * @file
 * @brief 玄武设备栈：I2C协议通用的定义
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

#ifndef __xwcd_ds_i2c_common_h__
#define __xwcd_ds_i2c_common_h__

#include <xwcd/ds/standard.h>

#define XWDS_I2C_10BITADDR_LEADING_BYTE_MASK    ((xwu8_t)0xF0)

/**
 * @brief I2C消息标志枚举
 */
enum xwds_i2c_msgflag_em {
        XWDS_I2C_F_7BITADDR = (0U), /**< 7位外设地址 */
        XWDS_I2C_F_10BITADDR = (1U << 0U), /**< 10位外设地址 */
        XWDS_I2C_F_ADDRMSK = (1U << 0U), /**< 地址掩码 */

        XWDS_I2C_F_RD = (1U << 1U), /**< 方向：读 */
        XWDS_I2C_F_WR = (0U << 1U), /**< 方向：写 */
        XWDS_I2C_F_DIRMSK = (1U << 1U), /**< 方向掩码 */

        XWDS_I2C_F_START = (1U << 2U), /**< 产生起始条件：
                                            + 如果传输方向发生改变，需要增加START标志，
                                              传输方向发生改变是指：
                                              - 两次传输地址发生改变；
                                              - 传输方向由无变为读；
                                              - 传输方向由无变为写；
                                              - 传输方向由写变为读；
                                              - 传输方向由读变为写。
                                            + I2C外设驱动需要自己管理START标志，
                                              xwds/i2cm框架不保证能纠正标志错误。 */
        XWDS_I2C_F_STOP = (1U << 3U), /**< 产生结束条件 */
};

/**
 * @brief I2C消息
 */
struct xwds_i2c_msg {
        xwu16_t addr; /**< 外设地址，
                           + 7位地址模式，使用bit[1:7]，bit[0]默认为0；
                           + 10位地址模式，使用bit[0:9] */
        xwu16_t flag; /**< 传输标志 */
        xwu8_t * data; /**< 数据缓存区的指针 */
        xwsz_t size; /** 数据缓存区的大小 */
};

#endif /* xwcd/ds/i2c/common.h */
