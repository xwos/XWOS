/**
 * @file
 * @brief 玄武设备栈：CAN收发器：通用型104x/105x系列收发器
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

#ifndef __xwcd_peripheral_can_transceiver_xxx10xx_device_h__
#define __xwcd_peripheral_can_transceiver_xxx10xx_device_h__

#include <xwos/standard.h>
#include <xwcd/ds/can/transceiver.h>

/**
 * @defgroup xwcd_peripheral_can_transceiver_xxx10xx 通用型104x/105x系列收发器
 * @ingroup xwcd_peripheral_can_transceiver
 * 通用型104x/105x系列收发器的驱动，例如：TJA1042、TJA1043、SIT1044等收发器。
 * @{
 */

/**
 * @brief XXX10XX对象
 */
struct xwds_cantrcv_xxx10xx {
        struct xwds_cantrcv cantrcv; /**< C语言面向对象：继承 `struct xwds_iochip` */
        struct {
                const struct xwds_resource_gpio * stb; /**< STANDBY */
                const struct xwds_resource_gpio * eirq; /**< 唤醒中断 */
        } gpiorsc; /**< GPIO资源 */
        xwid_t eirq; /**< 上游SOC的外部中断号 */
};

/**
 * @brief XXX10XX API：XXX10XX对象的构造函数
 * @param[in] xxx10xx: XXX10XX对象指针
 */
void xwds_cantrcv_xxx10xx_construct(struct xwds_cantrcv_xxx10xx * xxx10xx);

/**
 * @brief XXX10XX API：XXX10XX对象的析构函数
 * @param[in] xxx10xx: XXX10XX对象指针
 */
void xwds_cantrcv_xxx10xx_destruct(struct xwds_cantrcv_xxx10xx * xxx10xx);

/**
 * @brief XXX10XX API：增加对象的引用计数
 * @param[in] xxx10xx: I2C XXX10XX对象指针
 * @return 错误码
 * @retval @ref xwds_i2cp_grab()
 */
static __xwds_inline
xwer_t xwds_cantrcv_xxx10xx_grab(struct xwds_cantrcv_xxx10xx * xxx10xx)
{
        return xwds_cantrcv_grab(&xxx10xx->cantrcv);
}

/**
 * @brief XXX10XX API：减少对象的引用计数
 * @param[in] xxx10xx: I2C XXX10XX对象指针
 * @return 错误码
 * @retval @ref xwds_i2cp_put()
 */
static __xwds_inline
xwer_t xwds_cantrcv_xxx10xx_put(struct xwds_cantrcv_xxx10xx * xxx10xx)
{
        return xwds_cantrcv_put(&xxx10xx->cantrcv);
}

/**
 * @} xwcd_peripheral_can_transceiver_xxx10xx
 */

#endif /* xwcd/peripheral/can/transceiver/xxx10xx/device.h */
