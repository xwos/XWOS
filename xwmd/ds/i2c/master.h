/**
 * @file
 * @brief xwmd设备栈：I2C主机
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __xwmd_ds_i2c_master_h__
#define __xwmd_ds_i2c_master_h__

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwmd/ds/standard.h>
#include <xwos/lib/xwbop.h>
#include <xwos/osal/lock/mutex.h>
#include <xwmd/ds/device.h>
#include <xwmd/ds/i2c/common.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
struct xwds_i2cm;

/**
 * @brief BSP中需要提供的I2C主机驱动函数表
 */
struct xwds_i2cm_driver {
        struct xwds_driver base; /**< C语言面向对象：继承struct xwds_driver */
        xwer_t (* xfer)(struct xwds_i2cm * /*i2cm*/,
                        struct xwds_i2c_msg * /*msg*/,
                        xwtm_t * /*xwtm*/); /**< 传输 */
        xwer_t (* abort)(struct xwds_i2cm * /*i2cm*/,
                         xwu16_t /*address*/, xwu16_t /*addrmode*/,
                         xwtm_t * /*xwtm*/); /**< 中止传输 */
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
                struct xwosal_mtx lock; /**< 传输API的互斥锁 */
        } xfer; /**< 传输API的状态 */
        struct {
                struct xwosal_mtx lock; /**< 中止API的互斥锁 */
        } abort; /**< 中止API的状态 */
};

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         function prototypes         ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/******** ******** ******** constructor & destructor ******** ******** ********/
__xwds_api
void xwds_i2cm_construct(struct xwds_i2cm * i2cm);

__xwds_api
void xwds_i2cm_destruct(struct xwds_i2cm * i2cm);

/******** ******** ******** APIs ******** ******** ********/
/**
 * @brief XWDS API：传输I2C消息
 * @param i2cm: (I) I2C主机控制器对象指针
 * @param msg: (I) I2C消息结构体的指针
 * @param xwtm: 指向缓冲区的指针，此缓冲区：
 *              (I) 作为输入时，表示期望的阻塞等待时间
 *              (O) 作为输出时，返回剩余的期望时间
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EINVAL: 设备对象不可引用
 * @retval -ESHUTDOWN: 设备没有运行
 * @retval -EADDRNOTAVAIL: 地址无响应
 * @retval -ETIMEDOUT: 超时
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：可重入
 */
__xwds_api
xwer_t xwds_i2cm_xfer(struct xwds_i2cm * i2cm, struct xwds_i2c_msg * msg,
                      xwtm_t * xwtm);

/**
 * @brief XWDS API：中止I2C总线传输
 * @param i2cm: (I) I2C主机控制器对象指针
 * @param address: (I) I2C地址
 * @param addrmode: (I) I2C地址模式，取值：
 *                      @ref XWDS_I2C_F_7BITADDR 7位地址模式
 *                      @ref XWDS_I2C_F_10BITADDR 10位地址模式
 * @param xwtm: 指向缓冲区的指针，此缓冲区：
 *              (I) 作为输入时，表示期望的阻塞等待时间
 *              (O) 作为输出时，返回剩余的期望时间
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EINVAL: 设备对象不可引用
 * @retval -ESHUTDOWN: 设备没有运行
 * @retval -EADDRNOTAVAIL: 地址无响应
 * @retval -ETIMEDOUT: 超时
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：可重入
 */
__xwds_api
xwer_t xwds_i2cm_abort(struct xwds_i2cm * i2cm,
                       xwu16_t address, xwu16_t addrmode,
                       xwtm_t * xwtm);

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********   inline function implementations   ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief 增加对象的引用计数
 * @param i2cm: (I) I2C主机控制器对象指针
 * @return 错误码
 * @retval @ref xwds_device_grab()
 */
static __xwds_inline
xwer_t xwds_i2cm_grab(struct xwds_i2cm * i2cm)
{
        return xwds_device_grab(&i2cm->dev);
}

/**
 * @brief 减少对象的引用计数
 * @param i2cm: (I) I2C主机控制器对象指针
 * @return 错误码
 * @retval @ref xwds_device_put()
 */
static __xwds_inline
xwer_t xwds_i2cm_put(struct xwds_i2cm * i2cm)
{
        return xwds_device_put(&i2cm->dev);
}

/**
 * @brief 增加设备运行状态计数器
 * @param i2cm: (I) I2C主机控制器对象指针
 * @return 错误码
 * @retval @ref xwds_device_request()
 */
static __xwds_inline
xwer_t xwds_i2cm_request(struct xwds_i2cm * i2cm)
{
        return xwds_device_request(&i2cm->dev);
}

/**
 * @brief 减少设备运行状态计数器
 * @param i2cm: (I) I2C主机控制器对象指针
 * @return 错误码
 * @retval @ref xwds_device_release()
 */
static __xwds_inline
xwer_t xwds_i2cm_release(struct xwds_i2cm * i2cm)
{
        return xwds_device_release(&i2cm->dev);
}

#endif /* xwmd/ds/i2c/master.h */
