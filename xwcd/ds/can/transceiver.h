/**
 * @file
 * @brief 玄武设备栈：CAN：总线接收器
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

#ifndef __xwcd_ds_can_transceiver_h__
#define __xwcd_ds_can_transceiver_h__

#include <xwcd/ds/standard.h>
#include <xwos/lib/xwbop.h>
#include <xwcd/ds/device.h>
#include <xwcd/ds/can/controller.h>
#include <xwcd/ds/i2c/peripheral.h>
#include <xwcd/ds/spi/peripheral.h>

/**
 * @defgroup xwcd_ds_can_transceiver CAN总线接收器
 * @ingroup xwcd_ds_can
 * @{
 */

/**
 * @brief CAN接收器模式枚举
 */
enum xwds_cantrcv_opmode_em {
        XWDS_CANTRCV_OPMODE_NORMAL = 0, /**< Transceiver mode NORMAL */
        XWDS_CANTRCV_OPMODE_SLEEP, /**< Transceiver mode SLEEP */
        XWDS_CANTRCV_OPMODE_STANDBY, /**< Transceiver mode STANDBY */
        XWDS_CANTRCV_OPMODE_NUM, /**< number of operation mode */
};

/**
 * @brief CAN接收器唤醒原因枚举
 */
enum xwds_cantrcv_wkuprs_em {
        XWDS_CANTRCV_WKUPRS_NONE = 0, /**< No reason */
        XWDS_CANTRCV_WKUPRS_BY_BUS, /** The transceiver has detected, that
                                        the network has caused the wake up of
                                        the ECU. */
        XWDS_CANTRCV_WKUPRS_BY_PIN, /** The transceiver has detected a wake-up
                                        event at one of the transceiver's
                                        pins (not at the CAN bus). */
        XWDS_CANTRCV_WKUPRS_INTERNALLY, /** The transceiver has detected, that
                                            the network has woken up by the ECU
                                            via a request to NORMAL mode. */
        XWDS_CANTRCV_WKUPRS_NOT_SUPPORTED, /** The transceiver does not support
                                               any information for the wake up
                                               reason. */
        XWDS_CANTRCV_WKUPRS_POWER_ON, /** The transceiver has detected, that the
                                          "wake up" is due to an ECU reset
                                          after power on. */
        XWDS_CANTRCV_WKUPRS_RESET, /** The transceiver has detected, that the
                                       "wake up" is due to an ECU reset. */
        XWDS_CANTRCV_WKUPRS_BY_SYSERR, /** The transceiver has detected, that
                                           the "wake up" of the ECU was caused by
                                           a HW related device failure. */
        XWDS_CANTRCV_WKUPRS_NUM, /**< number of wakeup reason */
};

struct xwds_cantrcv;

/**
 * @brief 唤醒中断回调函数
 */
typedef void (*xwds_cantrcv_wkup_isr_f)(struct xwds_cantrcv *, struct xwds_canc *);

/**
 * @brief CAN接收器配置
 */
struct xwds_cantrcv_cfg {
        xwsq_t init_opmode; /**< 初始状态 */
        bool wkup_supported; /**< 是否支持唤醒 */
        void * chipcfg; /**< 芯片其他配置 */
};

struct xwds_cantrcv;

/**
 * @brief BSP中需要提供的CAN接收器驱动函数表
 */
struct xwds_cantrcv_driver {
        struct xwds_driver base; /**< C语言面向对象：继承struct xwds_driver */
        xwer_t (* set_opmode)(struct xwds_cantrcv *, xwsq_t); /**< 设置运行模式 */
        xwer_t (* enable_wkup)(struct xwds_cantrcv *); /**< 打开唤醒 */
        xwer_t (* disable_wkup)(struct xwds_cantrcv *); /**< 关闭唤醒 */
};

/**
 * @brief CAN接收器
 */
struct xwds_cantrcv {
        union {
                struct xwds_device dev; /**< 继承struct xwds_device */
#if (defined(XWCDCFG_ds_I2C_PERIPHERAL) && (1 == XWCDCFG_ds_I2C_PERIPHERAL))
                struct xwds_i2cp i2cp; /**< 继承struct xwds_i2cp */
#endif
#if (defined(XWCDCFG_ds_SPI_PERIPHERAL) && (1 == XWCDCFG_ds_SPI_PERIPHERAL))
                struct xwds_spip spip; /**< 继承struct xwds_spip */
#endif
        } bc; /**< 基类(base class) */

        /* attributes */
        struct xwds_canc * canc; /**< 连接的CAN控制器 */
        const struct xwds_cantrcv_cfg * cfg; /**< CAN接收器的配置 */
        xwds_cantrcv_wkup_isr_f wkup_isr; /**< 唤醒中断回调函数 */

        /* private */
        xwsq_t opmode; /**< CAN接收器模式 */
        xwsq_t wkuprs; /**< CAN接收器唤醒原因 */
};

/**
 * @brief XWDS API：CAN接收器的构造函数
 * @param[in] cantrcv: CAN接收器对象指针
 */
void xwds_cantrcv_construct(struct xwds_cantrcv * cantrcv);

/**
 * @brief XWDS API：CAN接收器对象的析构函数
 * @param[in] cantrcv: CAN接收器对象指针
 */
void xwds_cantrcv_destruct(struct xwds_cantrcv * cantrcv);

/**
 * @brief XWDS API：增加对象的引用计数
 * @param[in] cantrcv: CAN接收器对象指针
 */
xwer_t xwds_cantrcv_grab(struct xwds_cantrcv * cantrcv);

/**
 * @brief XWDS API：减少对象的引用计数
 * @param[in] cantrcv: CAN接收器对象指针
 */
xwer_t xwds_cantrcv_put(struct xwds_cantrcv * cantrcv);

/**
 * @brief XWDS API：设置CAN接收器的运行模式
 * @param[in] cantrcv: CAN接收器对象指针
 * @param[in] opmode: 唤醒模式
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -ERANGE: 不支持的模式
 * @note
 * + 上下文：中断、中断底半部、线程
 */
xwer_t xwds_cantrcv_set_opmode(struct xwds_cantrcv * cantrcv, xwsq_t opmode);

/**
 * @brief XWDS API：设置CAN接收器的模式
 * @param[in] cantrcv: CAN接收器对象指针
 * @param[out] opmode: 指向缓冲区的指针，通过次缓冲区返回模式
 * @note
 * + 上下文：中断、中断底半部、线程
 */
void xwds_cantrcv_get_opmode(struct xwds_cantrcv * cantrcv, xwsq_t * opmode);

/**
 * @brief XWDS API：开启CAN接收器的唤醒
 * @param[in] cantrcv: CAN接收器对象指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -EOPNOTSUPP: 不支持此API
 * @note
 * + 上下文：中断、中断底半部、线程
 */
xwer_t xwds_cantrcv_enable_wkup(struct xwds_cantrcv * cantrcv);

/**
 * @brief XWDS API：关闭CAN接收器的唤醒
 * @param[in] cantrcv: CAN接收器对象指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -EOPNOTSUPP: 不支持此API
 * @note
 * + 上下文：中断、中断底半部、线程
 */
xwer_t xwds_cantrcv_disable_wkup(struct xwds_cantrcv * cantrcv);

/**
 * @brief XWDS API：设置唤醒中断的回调函数
 * @param[in] cantrcv: CAN接收器对象指针
 * @param[in] isr: 回调函数
 * @note
 * + 上下文：中断、中断底半部、线程
 * @details
 * 必须调用 `xwds_cantrcv_disable_wkup()` 后才可修改回调函数。
 */
void xwds_cantrcv_set_wkup_isr(struct xwds_cantrcv * cantrcv,
                               xwds_cantrcv_wkup_isr_f isr);

/**
 * @brief XWDS API：获取CAN接收器的唤醒原因
 * @param[in] cantrcv: CAN接收器对象指针
 * @param[out] wkuprs: 指向缓冲区的指针，通过次缓冲区返回唤醒原因
 * @note
 * + 上下文：中断、中断底半部、线程
 */
void xwds_cantrcv_get_wkuprs(struct xwds_cantrcv * cantrcv, xwsq_t * wkuprs);

/**
 * @brief XWDS API：清除CAN接收器的唤醒原因
 * @param[in] cantrcv: CAN接收器对象指针
 * @note
 * + 上下文：中断、中断底半部、线程
 */
void xwds_cantrcv_clear_wkuprs(struct xwds_cantrcv * cantrcv);

/******** ******** Callbacks for Driver ******** ********/
/**
 * @brief CAN接收器回调函数：CAN接收器的唤醒通知
 * @param[in] cantrcv: CAN接收器对象指针
 * @note
 * + 上下文：中断、中断底半部
 * @details
 * 此回调函数在中断上下文中被调用，用于通知唤醒。
 */
void xwds_cantrcv_drvcb_wakeup_notification(struct xwds_cantrcv * cantrcv);

/**
 * @} xwcd_ds_can_transceiver
 */

#endif /* xwcd/ds/can/transceiver.h */
