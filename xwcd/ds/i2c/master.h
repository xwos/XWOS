/**
 * @file
 * @brief 玄武设备栈：I2C主机
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

#ifndef __xwcd_ds_i2c_master_h__
#define __xwcd_ds_i2c_master_h__

#include <xwcd/ds/standard.h>
#include <xwos/osal/lock/mtx.h>
#include <xwcd/ds/device.h>
#include <xwcd/ds/i2c/common.h>

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
                struct xwos_mtx apimtx; /**< 传输API的互斥锁 */
        } xfer; /**< 传输API的状态 */
        struct {
                struct xwos_mtx apimtx; /**< 中止API的互斥锁 */
        } abort; /**< 中止API的状态 */
};

void xwds_i2cm_construct(struct xwds_i2cm * i2cm);
void xwds_i2cm_destruct(struct xwds_i2cm * i2cm);
xwer_t xwds_i2cm_xfer(struct xwds_i2cm * i2cm, struct xwds_i2c_msg * msg,
                      xwtm_t * xwtm);
xwer_t xwds_i2cm_abort(struct xwds_i2cm * i2cm,
                       xwu16_t address, xwu16_t addrmode,
                       xwtm_t * xwtm);

/**
 * @brief XWDS API：增加对象的引用计数
 * @param i2cm: (I) I2C主机控制器对象指针
 * @return 错误码
 * @retval @ref xwds_device_grab()
 */
static __xwds_inline_api
xwer_t xwds_i2cm_grab(struct xwds_i2cm * i2cm)
{
        return xwds_device_grab(&i2cm->dev);
}

/**
 * @brief XWDS API：减少对象的引用计数
 * @param i2cm: (I) I2C主机控制器对象指针
 * @return 错误码
 * @retval @ref xwds_device_put()
 */
static __xwds_inline_api
xwer_t xwds_i2cm_put(struct xwds_i2cm * i2cm)
{
        return xwds_device_put(&i2cm->dev);
}

/**
 * @brief XWDS API：增加设备运行状态计数器
 * @param i2cm: (I) I2C主机控制器对象指针
 * @return 错误码
 * @retval @ref xwds_device_request()
 */
static __xwds_inline_api
xwer_t xwds_i2cm_request(struct xwds_i2cm * i2cm)
{
        return xwds_device_request(&i2cm->dev);
}

/**
 * @brief XWDS API：减少设备运行状态计数器
 * @param i2cm: (I) I2C主机控制器对象指针
 * @return 错误码
 * @retval @ref xwds_device_release()
 */
static __xwds_inline_api
xwer_t xwds_i2cm_release(struct xwds_i2cm * i2cm)
{
        return xwds_device_release(&i2cm->dev);
}

#endif /* xwcd/ds/i2c/master.h */
