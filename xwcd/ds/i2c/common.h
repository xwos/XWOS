/**
 * @file
 * @brief xwcd设备栈：I2C协议通用的定义
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

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwcd/ds/standard.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define XWDS_I2C_10BITADDR_LEADING_BYTE_MASK    ((xwu8_t)0xF0)

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief I2C消息标志枚举
 */
enum xwds_i2c_msgflag_em {
        XWDS_I2C_F_7BITADDR = (0 << 0), /**< 7位外设地址 */
        XWDS_I2C_F_10BITADDR = (1 << 0), /**< 10位外设地址 */
        XWDS_I2C_F_ADDRMSK = (1 << 0), /**< 地址掩码 */

        XWDS_I2C_F_NONE = (0 << 1), /**< 方向：无 */
        XWDS_I2C_F_RD = (1 << 1), /**< 方向：读 */
        XWDS_I2C_F_WR = (2 << 1), /**< 方向：写 */
        XWDS_I2C_F_DIRMSK = (3 << 1), /**< 方向掩码 */

        XWDS_I2C_F_START = (1 << 2), /**< 产生起始条件：
                                        + 如果传输方向发生改变，需要增加START标志，
                                          传输方向发生改变是指：
                                          - 两次传输地址发生改变；
                                          - 传输方向由无变为读；
                                          - 传输方向由无变为写；
                                          - 传输方向由无写为读；
                                          - 传输方向由无读为写。
                                        + I2C外设驱动需要自己管理START标志，xwds/i2cm
                                          框架不保证能纠正标志错误。
                                     */
        XWDS_I2C_F_STOP = (1 << 3), /**< 产生结束条件 */
};

/**
 * @brief I2C消息
 */
struct xwds_i2c_msg {
        xwu16_t addr; /**< 外设地址，
                           + 7位地址使用bit[1:7]，bit[0]默认为0；
                           + 10位地址使用bit[0:9] */
        xwu16_t flag; /**< 消息标志 */
        xwu8_t * data; /**< 消息数据缓存的指针 */
        xwsz_t size; /** 消息数据大小 */
};

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         function prototypes         ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********  inline functions implementations   ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

#endif /* xwcd/ds/i2c/common.h */
