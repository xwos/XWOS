/**
 * @file
 * @brief 玄武设备栈：CAN收发器：通用型104x/105x系列收发器：驱动
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
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

#ifndef __xwcd_peripheral_can_transceiver_xxx10xx_driver_h__
#define __xwcd_peripheral_can_transceiver_xxx10xx_driver_h__

#include <xwos/standard.h>
#include <xwcd/ds/soc/eirq.h>
#include <xwcd/peripheral/can/transceiver/xxx10xx/device.h>

/**
 * @ingroup xwcd_peripheral_can_transceiver_xxx10xx
 * @{
 */

/**
 * @brief XXX10XX基本驱动：探测设备
 */
xwer_t xwds_cantrcv_xxx10xx_drv_probe(struct xwds_device * dev);

/**
 * @brief XXX10XX基本驱动：启动设备
 */
xwer_t xwds_cantrcv_xxx10xx_drv_start(struct xwds_device * dev);

/**
 * @brief XXX10XX基本驱动：停止设备
 */
xwer_t xwds_cantrcv_xxx10xx_drv_stop(struct xwds_device * dev);

#if defined(XWCDCFG_ds_PM) && (1 == XWCDCFG_ds_PM)
/**
 * @brief XXX10XX基本驱动：暂停设备
 */
xwer_t xwds_cantrcv_xxx10xx_drv_suspend(struct xwds_device * dev);

/**
 * @brief XXX10XX基本驱动：继续设备
 */
xwer_t xwds_cantrcv_xxx10xx_drv_resume(struct xwds_device * dev);
#endif

/**
 * @brief XXX10XX CAN接收器驱动：设置CAN接收器的运行模式
 * @param[in] cantrcv: CAN接收器对象指针
 * @param[in] opmode: 唤醒模式
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -ERANGE: 不支持的模式
 * @note
 * + 上下文：中断、中断底半部、线程
 */
xwer_t xwds_cantrcv_xxx10xx_drv_set_opmode(struct xwds_cantrcv * cantrcv,
                                           xwsq_t opmode);

/**
 * @brief XXX10XX CAN接收器驱动：开启CAN接收器的唤醒
 * @param[in] cantrcv: CAN接收器对象指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -EOPNOTSUPP: 不支持此API
 * @note
 * + 上下文：中断、中断底半部、线程
 */
xwer_t xwds_cantrcv_xxx10xx_drv_enable_wkup(struct xwds_cantrcv * cantrcv);

/**
 * @brief XXX10XX CAN接收器驱动：关闭CAN接收器的唤醒
 * @param[in] cantrcv: CAN接收器对象指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -EOPNOTSUPP: 不支持此API
 * @note
 * + 上下文：中断、中断底半部、线程
 */
xwer_t xwds_cantrcv_xxx10xx_drv_disable_wkup(struct xwds_cantrcv * cantrcv);

/**
 * @brief XXX10XX的驱动集合
 */
extern const struct xwds_cantrcv_driver xwds_cantrcv_xxx10xx_drv;

/**
 * @brief XXX10XX中断函数
 */
void xwds_cantrcv_xxx10xx_eirq_wkup(struct xwds_soc * soc, xwid_t eiid,
                                    xwds_eirq_arg_t arg);

/**
 * @} xwcd_peripheral_can_transceiver_xxx10xx
 */

#endif /* xwcd/peripheral/can/transceiver/xxx10xx/driver.h */
