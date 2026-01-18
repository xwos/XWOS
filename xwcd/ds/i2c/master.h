/**
 * @file
 * @brief 玄武设备栈：I2C：主机
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

#ifndef __xwcd_ds_i2c_master_h__
#define __xwcd_ds_i2c_master_h__

#include <xwcd/ds/standard.h>
#include <xwos/osal/lock/mtx.h>
#include <xwcd/ds/device.h>
#include <xwcd/ds/i2c/common.h>

/**
 * @defgroup xwcd_ds_i2c_master I2C主机
 * @ingroup xwcd_ds_i2c
 * @{
 */

struct xwds_i2cm;

/**
 * @brief BSP中需要提供的I2C主机驱动函数表
 */
struct xwds_i2cm_driver {
        struct xwds_driver base; /**< C语言面向对象：继承struct xwds_driver */
        xwer_t (* xfer)(struct xwds_i2cm * /*i2cm*/,
                        struct xwds_i2c_msg * /*msg*/,
                        xwtm_t /*to*/); /**< 传输 */
        xwer_t (* abort)(struct xwds_i2cm * /*i2cm*/,
                         xwu16_t /*address*/, xwu16_t /*addrmode*/,
                         xwtm_t /*to*/); /**< 中止传输 */
};

/**
 * @brief I2C主机
 */
struct xwds_i2cm {
        struct xwds_device dev; /**< C语言面向对象：继承struct xwds_device */

        /* attributes */
        void * xwccfg; /**< SOC芯片私有配置 */

        /* private */
        struct {
                struct xwos_mtx apimtx; /**< 传输API的互斥锁 */
        } xfer; /**< 传输API的状态 */
        struct {
                struct xwos_mtx apimtx; /**< 中止API的互斥锁 */
        } abort; /**< 中止API的状态 */
};

/**
 * @brief XWDS API：I2C主机控制器的构造函数
 * @param[in] i2cm: I2C主机控制器对象指针
 */
void xwds_i2cm_construct(struct xwds_i2cm * i2cm);

/**
 * @brief XWDS API：I2C主机控制器对象的析构函数
 * @param[in] i2cm: I2C主机控制器对象指针
 */
void xwds_i2cm_destruct(struct xwds_i2cm * i2cm);

/**
 * @brief XWDS API：增加对象的引用计数
 * @param[in] i2cm: I2C主机控制器对象指针
 */
xwer_t xwds_i2cm_grab(struct xwds_i2cm * i2cm);

/**
 * @brief XWDS API：减少对象的引用计数
 * @param[in] i2cm: I2C主机控制器对象指针
 */
xwer_t xwds_i2cm_put(struct xwds_i2cm * i2cm);

/**
 * @brief XWDS API：传输I2C消息
 * @param[in] i2cm: I2C主机控制器对象指针
 * @param[in,out] msg: I2C消息结构体的指针
 * + (I) addr: 地址，只作为输入
 * + (I) flag: 传输标志，只作为输入，取值：
 *   - 地址标志：
 *     + XWDS_I2C_F_7BITADDR: 7位地址
 *     + XWDS_I2C_F_10BITADDR: 10位地址
 *   - 方向标志：
 *     + XWDS_I2C_F_RD: 读
 *     + XWDS_I2C_F_WR: 写
 *   - 开始/停止条件标志：
 *     + XWDS_I2C_F_START: 产生开始条件
 *     + XWDS_I2C_F_STOP: 产生结束条件
 * + data: 数据缓冲区：
 *   - (I) 当方向标志为读时，只作为输入缓冲区
 *   - (O) 当方向标志为写时，只作为输出缓冲区
 * + size: 传输的字节数
 *   - (I) 当作为输入时，表示缓冲区的大小
 *   - (O) 当作为输出时，返回实际传输的数据大小
 * @param[in] to: 期望唤醒的时间点
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EINVAL: 设备对象不可引用
 * @retval -ESHUTDOWN: 设备没有运行
 * @retval -EADDRNOTAVAIL: 地址无响应
 * @retval -ETIMEDOUT: 超时
 * @note
 * + 上下文：线程
 * @details
 * `to` 表示等待超时的时间点：
 * + `to` 通常是未来的时间，即 **当前系统时间** + `delta` ，
 *   可以使用 `xwtm_ft(delta)` 表示；
 * + 如果 `to` 是过去的时间点，将直接返回 `-ETIMEDOUT` 。
 */
xwer_t xwds_i2cm_xfer(struct xwds_i2cm * i2cm,
                      struct xwds_i2c_msg * msg,
                      xwtm_t to);

/**
 * @brief XWDS API：中止I2C总线传输
 * @param[in] i2cm: I2C主机控制器对象指针
 * @param[in] address: I2C地址
 * @param[in] addrmode: I2C地址模式，取值：
 *   @arg XWDS_I2C_F_7BITADDR 7位地址模式
 *   @arg XWDS_I2C_F_10BITADDR 10位地址模式
 * @param[in] to: 期望唤醒的时间点
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EINVAL: 设备对象不可引用
 * @retval -ESHUTDOWN: 设备没有运行
 * @retval -EADDRNOTAVAIL: 地址无响应
 * @retval -ETIMEDOUT: 超时
 * @note
 * + 上下文：线程
 * + 异步函数
 * @details
 * `to` 表示等待超时的时间点：
 * + `to` 通常是未来的时间，即 **当前系统时间** + `delta` ，
 *   可以使用 `xwtm_ft(delta)` 表示；
 * + 如果 `to` 是过去的时间点，将直接返回 `-ETIMEDOUT` 。
 */
xwer_t xwds_i2cm_abort(struct xwds_i2cm * i2cm,
                       xwu16_t address, xwu16_t addrmode,
                       xwtm_t to);

/**
 * @} xwcd_ds_i2c_master
 */

#endif /* xwcd/ds/i2c/master.h */
