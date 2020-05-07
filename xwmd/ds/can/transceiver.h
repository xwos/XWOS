/**
 * @file
 * @brief xwmd设备栈：CAN总线接收器
 * @author
 * + 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __xwmd_ds_can_transceiver_h__
#define __xwmd_ds_can_transceiver_h__

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwmd/ds/standard.h>
#include <xwos/lib/xwbop.h>
#include <xwmd/ds/device.h>
#include <xwmd/ds/can/controller.h>
#include <xwmd/ds/i2c/perpheral.h>
#include <xwmd/ds/spi/perpheral.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief CAN接收器模式枚举
 */
enum xwds_cantrcv_opmode_em {
        SODS_CANTRCV_OPMODE_NORMAL = 0, /**< Transceiver mode NORMAL */
        SODS_CANTRCV_OPMODE_SLEEP, /**< Transceiver mode SLEEP */
        SODS_CANTRCV_OPMODE_STANDBY, /**< Transceiver mode STANDBY */
        SODS_CANTRCV_OPMODE_NUM, /**< number of operation mode */
};

/**
 * @brief CAN接收器唤醒原因枚举
 */
enum xwds_cantrcv_wkuprs_em {
        SODS_CANTRCV_WKUPRS_NONE = 0, /**< No reason */
        SODS_CANTRCV_WKUPRS_BY_BUS, /** The transceiver has detected, that
                                        the network has caused the wake up of
                                        the ECU. */
        SODS_CANTRCV_WKUPRS_BY_PIN, /** The transceiver has detected a wake-up
                                        event at one of the transceiver's
                                        pins (not at the CAN bus). */
        SODS_CANTRCV_WKUPRS_INTERNALLY, /** The transceiver has detected, that
                                            the network has woken up by the ECU
                                            via a request to NORMAL mode. */
        SODS_CANTRCV_WKUPRS_NOT_SUPPORTED, /** The transceiver does not support
                                               any information for the wake up
                                               reason. */
        SODS_CANTRCV_WKUPRS_POWER_ON, /** The transceiver has detected, that the
                                          "wake up" is due to an ECU reset
                                          after power on. */
        SODS_CANTRCV_WKUPRS_RESET, /** The transceiver has detected, that the
                                       "wake up" is due to an ECU reset. */
        SODS_CANTRCV_WKUPRS_BY_SYSERR, /** The transceiver has detected, that
                                           the "wake up" of the ECU was caused by
                                           a HW related device failure. */
        SODS_CANTRCV_WKUPRS_NUM, /**< number of wakeup reason */
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
#if (defined(MDCFG_ds_I2C_PERPHERAL) && (1 == MDCFG_ds_I2C_PERPHERAL) && \
     defined(MDCFG_ds_CAN_TRANSCEIVER_I2CP) && (1 == MDCFG_ds_CAN_TRANSCEIVER_I2CP))
                struct xwds_i2cp i2cp; /**< 继承struct xwds_i2cp */
#endif
#if (defined(MDCFG_ds_SPI_PERPHERAL) && (1 == MDCFG_ds_SPI_PERPHERAL) && \
     defined(MDCFG_ds_CAN_TRANSCEIVER_SPIP) && (1 == MDCFG_ds_CAN_TRANSCEIVER_SPIP))
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

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         function prototypes         ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/******** ******** ******** constructor & destructor ******** ******** ********/
__xwds_api
void xwds_cantrcv_construct(struct xwds_cantrcv * cantrcv);

__xwds_api
void xwds_cantrcv_destruct(struct xwds_cantrcv * cantrcv);

/******** ******** ******** APIs ******** ******** ********/
__xwds_api
xwer_t xwds_cantrcv_set_opmode(struct xwds_cantrcv * cantrcv, xwsq_t opmode);

__xwds_api
void xwds_cantrcv_get_opmode(struct xwds_cantrcv * cantrcv, xwsq_t * opmode);

__xwds_api
xwer_t xwds_cantrcv_enable_wkup(struct xwds_cantrcv * cantrcv);

__xwds_api
xwer_t xwds_cantrcv_disable_wkup(struct xwds_cantrcv * cantrcv);

__xwds_api
void xwds_cantrcv_set_wkup_isr(struct xwds_cantrcv * cantrcv,
                               xwds_cantrcv_wkup_isr_f isr);

__xwds_api
void xwds_cantrcv_get_wkuprs(struct xwds_cantrcv * cantrcv, xwsq_t * wkuprs);

__xwds_api
void xwds_cantrcv_clear_wkuprs(struct xwds_cantrcv * cantrcv);

/******** ******** CAN controller Libraries for BSP driver ******** ********/
__xwds_code
void xwds_cantrcv_lib_wakeup_notification(struct xwds_cantrcv * cantrcv);

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********   inline function implementations   ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief 增加对象的引用计数
 * @param cantrcv: (I) CAN接收器对象指针
 * @return 错误码
 */
static __xwds_inline
xwer_t xwds_cantrcv_grab(struct xwds_cantrcv * cantrcv)
{
        return xwds_device_grab(&cantrcv->bc.dev);
}

/**
 * @brief 减少对象的引用计数
 * @param cantrcv: (I) CAN接收器对象指针
 * @return 错误码
 */
static __xwds_inline
xwer_t xwds_cantrcv_put(struct xwds_cantrcv * cantrcv)
{
        return xwds_device_put(&cantrcv->bc.dev);
}

/**
 * @brief 增加设备运行状态计数器
 * @param cantrcv: (I) CAN接收器对象指针
 * @return 错误码
 */
static __xwds_inline
xwer_t xwds_cantrcv_request(struct xwds_cantrcv * cantrcv)
{
        return xwds_device_request(&cantrcv->bc.dev);
}

/**
 * @brief 减少设备运行状态计数器
 * @param cantrcv: (I) CAN接收器对象指针
 * @return 错误码
 */
static __xwds_inline
xwer_t xwds_cantrcv_release(struct xwds_cantrcv * cantrcv)
{
        return xwds_device_release(&cantrcv->bc.dev);
}

#endif /* xwmd/ds/can/transceiver.h */
