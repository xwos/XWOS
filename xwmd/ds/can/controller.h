/**
 * @file
 * @brief xwmd设备栈：CAN总线控制器
 * @author
 * + 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __xwmd_ds_can_controller_h__
#define __xwmd_ds_can_controller_h__

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <xwos/lib/xwbop.h>
#include <xwmd/ds/device.h>
#include <xwos/osal/lock/spinlock.h>
#include <xwos/osal/sync/semaphore.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#if defined(XWMDCFG_ds_CAN_CONTROLLER_ROCBT) && (1 == XWMDCFG_ds_CAN_CONTROLLER_ROCBT)
  #define __xwds_canc_cbtbl_qualifier const
#else
  #define __xwds_canc_cbtbl_qualifier
#endif

#define SODS_CANC_RXQNUM        (XWMDCFG_ds_CAN_CONTROLLER_RXQNUM)
#define SODS_CANC_SDU_MAXSIZE   (XWMDCFG_ds_CAN_CONTROLLER_SDU_MAXSIZE)

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief CAN消息标志枚举
 */
enum xwds_can_msgflag_em {
        SODS_CAN_MSG_F_EXID = BIT(0), /**< 扩展ID */
        SODS_CAN_MSG_F_REMOTE = BIT(1), /**< 远程帧 */
};

/**
 * @brief CAN消息
 */
struct xwds_can_msg {
        xwu32_t id; /**< ID */
        xwsq_t flag; /**< 标志 */
        xwsz_t dlc; /** 数据长度 */
        xwu8_t sdu[SODS_CANC_SDU_MAXSIZE]; /**< 数据缓存 */
};

/**
 * @brief CAN总线控制器波特率配置
 */
struct xwds_canc_bd_cfg {
        xwid_t id; /**< 配置ID */
        xwu32_t bdkb; /**< 波特率（单位：Kb） */
        struct {
                xwu8_t ss; /**< 同步段(sync seg)：根据CAN网络协议始终设置为1 tq */
                xwu8_t pts; /**< 传播段(propagation time segment)：1 ~ 8 tq */
                xwu8_t ps1; /**< 相位缓冲段1(phase buffer segment 1)：1 ~ 8 tq */
                xwu8_t ps2; /**< 相位缓冲段2(phase buffer segment 2)：1 ~ 8 tq */
                xwu8_t sjw; /**< 重新同步跳转宽度(resynchronization jump width)：1 ~
                                 min(4, ps1) tq */
        } time_seg; /** 时间段 */
};

/**
 * @brief CAN对象（消息）类型
 */
enum xwds_canc_hwobj_type_em {
        SODS_CANC_HWOBJ_T_ID_STD = BIT(0), /**<  标准帧 */
        SODS_CANC_HWOBJ_T_ID_EXT = BIT(1), /**<  扩展帧 */
        SODS_CANC_HWOBJ_T_ID_MIXED = BIT(0) | BIT(1), /**<  混合帧 */
};

/**
 * @brief CAN发送对象（消息）配置
 */
struct xwds_canc_txobj_cfg {
        xwid_t objid; /**< ID */
        xwsq_t type; /**< 类型 */
};

/**
 * @brief CAN接收对象（消息）配置
 */
struct xwds_canc_rxobj_cfg {
        xwid_t objid; /**< ID */
        xwsq_t type; /**< 类型 */
        void * soc_hwfilter_cfgs; /**< 过滤器数组 */
        xwsz_t soc_hwfilter_cfgs_num; /**< 过滤器的数量 */
};

/**
 * @brief CAN控制器配置
 */
struct xwds_canc_cfg {
        const struct xwds_canc_bd_cfg * bdcfgs; /**< 波特率配置数组 */
        xwsz_t bdcfgs_num; /**< 波特率配置的数量 */
        xwid_t default_bdid; /**< 默认波特率配置的ID */
        const struct xwds_canc_txobj_cfg * txobjs; /**< 发送邮箱的数组 */
        xwsz_t txobjs_num; /**< 发送邮箱的数量 */
        const struct xwds_canc_rxobj_cfg * rxobjs; /**< 接收邮箱的数组 */
        xwsz_t rxobjs_num; /**< 接收邮箱的数量 */
        void * xwccfg; /**< SOC私有配置 */
};

/**
 * @brief CAN控制器的模式
 */
enum xwds_canc_mode_em {
        SODS_CANC_MODE_MIN = 0,
        SODS_CANC_MODE_UNINIT = SODS_CANC_MODE_MIN, /**< UNINIT mode. Default mode
                                                         of the CAN driver and all
                                                         CAN controllers connected
                                                         to one CAN network after
                                                         power on. */
        SODS_CANC_MODE_STOPPED, /**< STOPPED mode. At least one of all CAN controllers
                                     connected to one CAN network are halted and
                                     does not operate on the bus. */
        SODS_CANC_MODE_STARTED, /**< STARTED mode. All CAN controllers connected to
                                     one CAN network are started by the CAN driver
                                     and in full-operational mode. */
        SODS_CANC_MODE_SLEEP, /**< SLEEP mode. At least one of all CAN controllers
                                   connected to one CAN network are set into the
                                   SLEEP mode and can be woken up by request of the
                                   CAN driver or by a network event (must be
                                   supported by CAN hardware) */
        SODS_CANC_MODE_NUM, /**< number of CAN controller modes */
};

/**
 * @brief CAN控制器的错误码
 */
enum xwds_canc_err_em {
        CANC_ERR_NOERR = 0,
        CANC_ERR_BUS, /**< bus error */
        CANC_ERR_STUFF, /**< stuff error */
        CANC_ERR_FORM, /**< form error */
        CANC_ERR_ACK, /**< ACK error */
        CANC_ERR_CRC, /*  CRC Error Flag */
        CANC_ERR_B1, /**< Recessive Bit Error Flag */
        CANC_ERR_B0, /**< Dominant Bit Error Flag */
        CANC_ERR_ACKDELIMITER, /**< ACK Delimiter Error Flag */
        CANC_ERR_PASSIVE, /**< TX/RX Error counter > 127 */
        CANC_ERR_WARNING, /**< TX/RX Error counter > WARNING limit */
};

struct xwds_canc;

/**
 * @brief BSP中需要提供的CAN控制器驱动函数表
 */
struct xwds_canc_driver {
        struct xwds_driver base; /**< C语言面向对象：继承struct xwds_driver */
        xwer_t (* write)(struct xwds_canc *, const struct xwds_canc_txobj_cfg *,
                         struct xwds_can_msg *); /**< 写TX邮箱 */
        xwer_t (* set_mode)(struct xwds_canc *, xwsq_t); /**< 设置CAN控制的模式 */
        xwer_t (* set_bd)(struct xwds_canc *,
                          const struct xwds_canc_bd_cfg *); /**< 设置波特率 */
        xwer_t (* enable_irqs)(struct xwds_canc *); /**< 开启CAN控制器的所有中断 */
        xwer_t (* disable_irqs)(struct xwds_canc *); /**< 关闭CAN控制器的所有中断 */
};

/**
 * @brief CAN控制器应用层回调函数表
 */
struct xwds_canc_cbtbl {
        void (*tx_indication)(struct xwds_canc *, xwid_t,
                              xwer_t); /**< 指示消息发送结果 */
        void (*rx_indication)(struct xwds_canc *, xwid_t,
                              struct xwds_can_msg *); /**< 指示消息接收结果 */
        void (*wakeup_notification)(struct xwds_canc *); /**< 指示唤醒 */
        void (*mode_indication)(struct xwds_canc *, xwsq_t); /**< 指示模式切换 */
        void (*err_indication)(struct xwds_canc *, xwsq_t,
                               xwsq_t, xwsq_t); /**< 指示错误发生 */
        void (*busoff_indication)(struct xwds_canc *); /**< 指示busoff */
};

/**
 * @brief CAN控制器接收队列
 */
struct xwds_canc_rxqueue {
        struct xwds_can_msg q[SODS_CANC_RXQNUM]; /**< 接收队列（循环数组）*/
        struct xwosal_smr smr; /**< 接收队列的数据信号量 */
        struct xwosal_splk lock; /**< 保护接收队列的锁 */
        xwssq_t pos; /**< 当前可以接收数据的位置 */
        xwssz_t num; /**< 缓冲区中有效数据的数量 */
};

/**
 * @brief CAN控制器
 */
struct xwds_canc {
        union { /* union中继承多个基类相当于C++中的virtual继承，
                   struct xwds_device只继承一次。 */
                struct xwds_device dev; /**< C语言面向对象：继承struct xwds_device */
#if (defined(XWMDCFG_ds_I2C_PERPHERAL) && (1 == XWMDCFG_ds_I2C_PERPHERAL) && \
     defined(XWMDCFG_ds_CAN_CONTROLLER_I2CP) && (1 == XWMDCFG_ds_CAN_CONTROLLER_I2CP))
                struct xwds_i2cp i2cp; /**< 继承struct xwds_i2cp */
#endif
#if (defined(XWMDCFG_ds_SPI_PERPHERAL) && (1 == XWMDCFG_ds_SPI_PERPHERAL) && \
     defined(XWMDCFG_ds_CAN_CONTROLLER_SPIP) && (1 == XWMDCFG_ds_CAN_CONTROLLER_SPIP))
                struct xwds_spip spip; /**< 继承struct xwds_spip */
#endif
        } bc; /**< 基类(base class) */

        /* attributes */
        const struct xwds_canc_cfg * cfg; /**< CAN控制器的配置 */
        __xwds_canc_cbtbl_qualifier struct xwds_canc_cbtbl * cbtbl; /**< 应用层回调
                                                                         函数表 */

        /* private */
        xwsq_t mode; /**< 控制器模式 */
};

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         function prototypes         ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/******** ******** ******** constructor & destructor ******** ******** ********/
#if !defined(XWMDCFG_ds_NANO) || (1 != XWMDCFG_ds_NANO)
__xwds_code
void xwds_canc_construct(struct xwds_canc * canc);

__xwds_code
void xwds_canc_destruct(struct xwds_canc * canc);
#endif /* !XWMDCFG_ds_NANO */

/******** ******** ******** APIs ******** ******** ********/
#if defined(XWMDCFG_ds_NANO) && (1 == XWMDCFG_ds_NANO)
__xwds_api
xwer_t xwds_canc_probe(struct xwds_canc * canc);

__xwds_api
xwer_t xwds_canc_remove(struct xwds_canc * canc);

__xwds_api
xwer_t xwds_canc_start(struct xwds_canc * canc);

__xwds_api
xwer_t xwds_canc_stop(struct xwds_canc * canc);

#if defined(XWMDCFG_ds_LPM) && (1 == XWMDCFG_ds_LPM)
__xwds_api
xwer_t xwds_canc_suspend(struct xwds_canc * canc);

__xwds_api
xwer_t xwds_canc_resume(struct xwds_canc * canc);
#endif /* XWMDCFG_ds_LPM */
#endif /* XWMDCFG_ds_NANO */

__xwds_api
xwer_t xwds_canc_write(struct xwds_canc * canc, xwid_t txobjid,
                       struct xwds_can_msg * msg);

__xwds_api
xwer_t xwds_canc_enable_irqs(struct xwds_canc * canc);

__xwds_api
xwer_t xwds_canc_disable_irqs(struct xwds_canc * canc);

__xwds_api
xwer_t xwds_canc_set_mode(struct xwds_canc * canc, xwsq_t mode);

__xwds_api
xwer_t xwds_canc_set_bd(struct xwds_canc * canc, xwid_t bdcfgid);

/******** ******** CAN controller Libraries for BSP driver ******** ********/
__xwds_lib_code
void xwds_canc_lib_setcb_tx_indication(struct xwds_canc * canc,
                                       void (*cb)(struct xwds_canc *, xwid_t, xwer_t));

__xwds_lib_code
void xwds_canc_lib_setcb_rx_indication(struct xwds_canc * canc,
                                       void (*cb)(struct xwds_canc *,
                                                  xwid_t,
                                                  struct xwds_can_msg *));

__xwds_lib_code
void xwds_canc_lib_setcb_wakeup_notification(struct xwds_canc * canc,
                                             void (*cb)(struct xwds_canc *));

__xwds_lib_code
void xwds_canc_lib_setcb_mode_indication(struct xwds_canc * canc,
                                         void (*cb)(struct xwds_canc *, xwsq_t));

__xwds_lib_code
void xwds_canc_lib_setcb_err_indication(struct xwds_canc * canc,
                                        void (*cb)(struct xwds_canc *,
                                                   xwsq_t, xwsq_t, xwsq_t));

__xwds_lib_code
void xwds_canc_lib_setcb_busoff_indication(struct xwds_canc * canc,
                                           void (*cb)(struct xwds_canc *));

__xwds_lib_code
void xwds_canc_lib_init_msg(struct xwds_can_msg * msg, xwu32_t canid,
                            xwsq_t flag, xwsz_t dlc, xwu8_t sdu[]);

__xwds_lib_code
void xwds_canc_lib_tx_indication(struct xwds_canc * canc,
                                 xwid_t txobjid,
                                 xwer_t rc);

__xwds_lib_code
void xwds_canc_lib_rx_indication(struct xwds_canc * canc,
                                 xwid_t rxobjid,
                                 struct xwds_can_msg * rxmsg);

__xwds_lib_code
void xwds_canc_lib_wakeup_notification(struct xwds_canc * canc);

__xwds_lib_code
void xwds_canc_lib_mode_indication(struct xwds_canc * canc,
                                   xwsq_t mode);

__xwds_lib_code
void xwds_canc_lib_err_indication(struct xwds_canc * canc,
                                  xwsq_t errcode,
                                  xwsq_t tec,
                                  xwsq_t rec);

__xwds_lib_code
void xwds_canc_lib_busoff_indication(struct xwds_canc * canc);

__xwds_lib_code
void xwds_canc_lib_rxq_init(struct xwds_canc_rxqueue * rxq);

__xwds_lib_code
void xwds_canc_lib_rxq_publish(struct xwds_canc_rxqueue * rxq,
                               struct xwds_can_msg * msg);

__xwds_lib_code
xwer_t xwds_canc_lib_rxq_acquire(struct xwds_canc_rxqueue * rxq,
                                 struct xwds_can_msg * buf,
                                 xwtm_t * xwtm);

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********   inline function implementations   ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#if !defined(XWMDCFG_ds_NANO) || (1 != XWMDCFG_ds_NANO)
/**
 * @brief 增加对象的引用计数
 * @param canc: (I) CAN控制器对象指针
 * @return 错误码
 */
static __xwds_inline
xwer_t xwds_canc_grab(struct xwds_canc * canc)
{
        return xwds_device_grab(&canc->bc.dev);
}

/**
 * @brief 减少对象的引用计数
 * @param canc: (I) CAN控制器对象指针
 * @return 错误码
 */
static __xwds_inline
xwer_t xwds_canc_put(struct xwds_canc * canc)
{
        return xwds_device_put(&canc->bc.dev);
}

/**
 * @brief 增加设备运行状态计数器
 * @param canc: (I) CAN控制器对象指针
 * @return 错误码
 */
static __xwds_inline
xwer_t xwds_canc_request(struct xwds_canc * canc)
{
        return xwds_device_request(&canc->bc.dev);
}

/**
 * @brief 减少设备运行状态计数器
 * @param canc: (I) CAN控制器对象指针
 * @return 错误码
 */
static __xwds_inline
xwer_t xwds_canc_release(struct xwds_canc * canc)
{
        return xwds_device_release(&canc->bc.dev);
}
#endif /* !XWMDCFG_ds_NANO */

#endif /* xwmd/ds/can/controller.h */
