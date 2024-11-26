/**
 * @file
 * @brief 玄武设备栈：CAN：总线控制器
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

#ifndef __xwcd_ds_can_controller_h__
#define __xwcd_ds_can_controller_h__

#include <xwcd/ds/standard.h>
#include <xwos/lib/xwbop.h>
#include <xwos/osal/lock/spinlock.h>
#include <xwos/osal/sync/sem.h>
#include <xwcd/ds/device.h>
#include <xwcd/ds/i2c/peripheral.h>
#include <xwcd/ds/spi/peripheral.h>

/**
 * @defgroup xwcd_ds_can CAN
 * @ingroup xwcd_ds
 */

/**
 * @defgroup xwcd_ds_can_controller CAN总线控制器
 * @ingroup xwcd_ds_can
 * @{
 */

#if defined(XWCDCFG_ds_CAN_CONTROLLER_ROCBT) && (1 == XWCDCFG_ds_CAN_CONTROLLER_ROCBT)
#  define __xwds_canc_cbtbl_qualifier const
#else
#  define __xwds_canc_cbtbl_qualifier
#endif

#define XWDS_CANC_RXQNUM        (XWCDCFG_ds_CAN_CONTROLLER_RXQNUM)
#define XWDS_CANC_SDU_MAXSIZE   (XWCDCFG_ds_CAN_CONTROLLER_SDU_MAXSIZE)

/**
 * @brief CAN消息标志枚举
 */
enum xwds_can_msgflag_em {
        XWDS_CAN_MSG_F_EXID = (1U), /**< 扩展ID，BIT(0) */
        XWDS_CAN_MSG_F_REMOTE = (2U), /**< 远程帧，BIT(1) */
};

/**
 * @brief CAN消息
 */
struct xwds_can_msg {
        xwu32_t id; /**< CAN ID */
        xwsq_t flag; /**< 标志 */
        xwsz_t dlc; /** 数据长度 */
        xwu8_t sdu[XWDS_CANC_SDU_MAXSIZE]; /**< 数据缓存 */
};

/**
 * @brief CAN总线控制器波特率配置
 */
struct xwds_canc_bd_cfg {
        xwid_t id; /**< 配置ID */
        xwu32_t bdkb; /**< 波特率（单位：Kb） */
        struct {
                xwu8_t ss; /**< 同步段(sync seg)：始终设置为1 tq */
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
        XWDS_CANC_HWOBJ_T_ID_STD = XWBOP_BIT(0), /**<  标准帧 */
        XWDS_CANC_HWOBJ_T_ID_EXT = XWBOP_BIT(1), /**<  扩展帧 */
        XWDS_CANC_HWOBJ_T_ID_MIXED = XWBOP_BIT(0) | XWBOP_BIT(1), /**<  混合帧 */
};

/**
 * @brief CAN发送对象（消息）配置
 */
struct xwds_canc_txobj_cfg {
        xwid_t objid; /**< 发送邮箱的ID */
        xwsq_t type; /**< 类型 */
};

/**
 * @brief CAN接收对象（消息）配置
 */
struct xwds_canc_rxobj_cfg {
        xwid_t objid; /**< 接收缓冲区的ID */
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
        XWDS_CANC_MODE_MIN = 0,
        XWDS_CANC_MODE_UNINIT = XWDS_CANC_MODE_MIN, /**< UNINIT mode. Default mode
                                                         of the CAN driver and all
                                                         CAN controllers connected
                                                         to one CAN network after
                                                         power on. */
        XWDS_CANC_MODE_STOPPED, /**< STOPPED mode. At least one of all CAN controllers
                                     connected to one CAN network are halted and
                                     does not operate on the bus. */
        XWDS_CANC_MODE_STARTED, /**< STARTED mode. All CAN controllers connected to
                                     one CAN network are started by the CAN driver
                                     and in full-operational mode. */
        XWDS_CANC_MODE_SLEEP, /**< SLEEP mode. At least one of all CAN controllers
                                   connected to one CAN network are set into the
                                   SLEEP mode and can be woken up by request of the
                                   CAN driver or by a network event (must be
                                   supported by CAN hardware) */
        XWDS_CANC_MODE_NUM, /**< number of CAN controller modes */
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
        CANC_ERR_CRC, /**< CRC Error Flag */
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
        void (* tx_indication)(struct xwds_canc *, xwid_t,
                               xwer_t); /**< 指示消息发送结果 */
        void (* rx_indication)(struct xwds_canc *, xwid_t,
                               struct xwds_can_msg *); /**< 指示消息接收结果 */
        void (* wakeup_notification)(struct xwds_canc *); /**< 指示唤醒 */
        void (* mode_indication)(struct xwds_canc *, xwsq_t); /**< 指示模式切换 */
        void (* err_indication)(struct xwds_canc *, xwsq_t,
                                xwsq_t, xwsq_t); /**< 指示错误发生 */
        void (* busoff_indication)(struct xwds_canc *); /**< 指示busoff */
};

/**
 * @brief CAN控制器接收队列
 */
struct xwds_canc_rxqueue {
        struct xwds_can_msg q[XWDS_CANC_RXQNUM]; /**< 接收队列（循环数组）*/
        struct xwos_sem sem; /**< 接收队列的数据信号量 */
        struct xwos_splk lock; /**< 保护接收队列的锁 */
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
#if (defined(XWCDCFG_ds_I2C_PERIPHERAL) && (1 == XWCDCFG_ds_I2C_PERIPHERAL))
                struct xwds_i2cp i2cp; /**< 继承struct xwds_i2cp */
#endif
#if (defined(XWCDCFG_ds_SPI_PERIPHERAL) && (1 == XWCDCFG_ds_SPI_PERIPHERAL))
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

/**
 * @brief XWDS API：CAN控制器的构造函数
 * @param[in] canc: CAN控制器对象指针
 */
void xwds_canc_construct(struct xwds_canc * canc);

/**
 * @brief XWDS API：CAN控制器对象的析构函数
 * @param[in] canc: CAN控制器对象指针
 */
void xwds_canc_destruct(struct xwds_canc * canc);

/**
 * @brief XWDS API：增加对象的引用计数
 * @param[in] canc: CAN控制器对象指针
 */
xwer_t xwds_canc_grab(struct xwds_canc * canc);

/**
 * @brief XWDS API：减少对象的引用计数
 * @param[in] canc: CAN控制器对象指针
 */
xwer_t xwds_canc_put(struct xwds_canc * canc);

/**
 * @brief XWDS API：将一条CAN消息写入发送邮箱
 * @param[in] canc: CAN控制器对象指针
 * @param[in] txobjid: 发送邮箱的ID
 * @param[in] msg: CAN消息结构体的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效指针
 * @retval -ERANGE: 找不到邮箱
 * @retval -EBADSLT: 邮箱配置与消息类型不匹配
 * @retval -ETIMEDOUT: 超时
 * @note
 * + 上下文：线程
 */
xwer_t xwds_canc_write(struct xwds_canc * canc, xwid_t txobjid,
                       struct xwds_can_msg * msg);

/**
 * @brief XWDS API：开启CAN控制器的中断
 * @param[in] canc: CAN控制器对象指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效指针
 * @note
 * + 上下文：中断、中断底半部、线程
 */
xwer_t xwds_canc_enable_irqs(struct xwds_canc * canc);

/**
 * @brief XWDS API：关闭CAN控制器的中断
 * @param[in] canc: CAN控制器对象指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效指针
 * @note
 * + 上下文：中断、中断底半部、线程
 */
xwer_t xwds_canc_disable_irqs(struct xwds_canc * canc);

/**
 * @brief XWDS API：设置CAN控制器的模式
 * @param[in] canc: CAN控制器对象指针
 * @param[in] mode: 模式
 * @return 错误码
 * @retval OK: 模式切换完成
 * @retval -EFAULT: 无效指针
 * @retval -EALREADY: 控制器已经处于此模式
 * @retval -ERANGE: 无效的模式
 * @note
 * + 上下文：中断、中断底半部、线程
 */
xwer_t xwds_canc_set_mode(struct xwds_canc * canc, xwsq_t mode);

/**
 * @brief XWDS API：设置CAN控制器的波特率
 * @param[in] canc: CAN控制器对象指针
 * @param[in] id: 波特率的ID
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效指针
 * @retval -ERANGE: 错误的波特率ID
 * @note
 * + 上下文：中断、中断底半部、线程
 */
xwer_t xwds_canc_set_bd(struct xwds_canc * canc, xwid_t bdcfgid);

/**
 * @brief XWDS API：设置 **指示发送结果** 的回调函数
 * @param[in] canc: CAN总线消息结构体的指针
 * @param[in] cb: 回调函数
 * @note
 * + 上下文：中断、中断底半部、线程
 */
void xwds_canc_setcb_tx_indication(struct xwds_canc * canc,
                                   void (*cb)(struct xwds_canc *,
                                              xwid_t, xwer_t));

/**
 * @brief XWDS API：设置 **指示接收结果** 的回调函数
 * @param[in] canc: CAN总线消息结构体的指针
 * @param[in] cb: 回调函数
 * @note
 * + 上下文：中断、中断底半部、线程
 */
void xwds_canc_setcb_rx_indication(struct xwds_canc * canc,
                                   void (*cb)(struct xwds_canc *,
                                              xwid_t,
                                              struct xwds_can_msg *));

/**
 * @brief XWDS API：设置 **唤醒通知** 回调函数
 * @param[in] canc: CAN总线消息结构体的指针
 * @param[in] cb: 回调函数
 * @note
 * + 上下文：中断、中断底半部、线程
 */
void xwds_canc_setcb_wakeup_notification(struct xwds_canc * canc,
                                         void (*cb)(struct xwds_canc *));

/**
 * @brief XWDS API：设置 **模式切换通知** 回调函数
 * @param[in] canc: CAN总线消息结构体的指针
 * @param[in] cb: 回调函数
 * @note
 * + 上下文：中断、中断底半部、线程
 */
void xwds_canc_setcb_mode_indication(struct xwds_canc * canc,
                                     void (*cb)(struct xwds_canc *, xwsq_t));

/**
 * @brief XWDS API：设置 **错误通知** 回调函数
 * @param[in] canc: CAN总线消息结构体的指针
 * @param[in] cb: 回调函数
 * @note
 * + 上下文：中断、中断底半部、线程
 */
void xwds_canc_setcb_err_indication(struct xwds_canc * canc,
                                    void (*cb)(struct xwds_canc *,
                                               xwsq_t, xwsq_t, xwsq_t));

/**
 * @brief XWDS API：设置 **BUSOFF通知** 回调函数
 * @param[in] canc: CAN总线消息结构体的指针
 * @param[in] cb: 回调函数
 * @note
 * + 上下文：中断、中断底半部、线程
 */
void xwds_canc_setcb_busoff_indication(struct xwds_canc * canc,
                                       void (*cb)(struct xwds_canc *));

/******** ******** RX Queue ******** ********/
/**
 * @brief XWDS API：初始化接收缓冲队列
 * @param[in] rxq: 接收缓冲队列对象指针
 * @note
 * + 上下文：中断、中断底半部、线程
 */
void xwds_canc_rxq_init(struct xwds_canc_rxqueue * rxq);

/**
 * @brief XWDS API：发布一条消息到接收缓冲队列中
 * @param[in] rxq: 接收缓冲队列对象指针
 * @param[in] msg: 待发布的CAN消息结构体指针
 * @note
 * + 上下文：中断、中断底半部、线程
 */
void xwds_canc_rxq_publish(struct xwds_canc_rxqueue * rxq,
                           struct xwds_can_msg * msg);

/**
 * @brief XWDS API：从接收缓冲队列中获取一条消息
 * @param[in] rxq: 接收缓冲队列对象指针
 * @param[in] buf: 获取CAN消息结构体的缓存指针
 * @param[in] to: 期望唤醒的时间点
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效指针
 * @note
 * + 上下文：线程
 * @details
 * 如果 `to` 是过去的时间点，将直接返回 `-ETIMEDOUT` 。
 */
xwer_t xwds_canc_rxq_acquire(struct xwds_canc_rxqueue * rxq,
                             struct xwds_can_msg * buf,
                             xwtm_t to);

/******** ******** Callbacks for driver ******** ********/
/**
 * @brief XWDS Driver Callback：初始化CAN总线消息结构体
 * @param[in] msg: CAN总线消息结构体的指针
 * @param[in] sdu: 数据缓冲区的指针
 */
void xwds_canc_drvcb_init_msg(struct xwds_can_msg * msg, xwu32_t canid,
                              xwsq_t flag, xwsz_t dlc, xwu8_t sdu[]);

/**
 * @brief XWDS Driver Callback：指示发送结果
 * @param[in] canc: CAN控制器对象指针
 * @param[in] txobjid: 发送邮箱的ID
 * @param[in] rc: 发送结果
 * @details
 * 此回调函数在中断上下文中被调用，用于通知接收结果。
 */
void xwds_canc_drvcb_tx_indication(struct xwds_canc * canc,
                                   xwid_t txobjid,
                                   xwer_t rc);

/**
 * @brief XWDS Driver Callback：接收到CAN消息后的回调函数
 * @param[in] canc: CAN控制器对象指针
 * @param[in] rxobjid: 接收邮箱的ID
 * @param[out] rxmsg: 指向缓冲区的指针，通过此缓冲区返回接收到的CAN消息
 * @details
 * 此回调函数在中断上下文中调用，用于通知接收结果。
 *
 * `rxmsg` 所指向的缓冲区的生命周期只在这个函数内有效。
 */
void xwds_canc_drvcb_rx_indication(struct xwds_canc * canc,
                                   xwid_t rxobjid,
                                   struct xwds_can_msg * rxmsg);

/**
 * @brief XWDS Driver Callback：CAN控制器的唤醒通知
 * @param[in] canc: CAN控制器对象指针
 * @details
 * 此回调函数在中断上下文中被调用，用于通知唤醒。
 */
void xwds_canc_drvcb_wakeup_notification(struct xwds_canc * canc);

/**
 * @brief XWDS Driver Callback：指示CAN控制器的模式已经切换
 * @param[in] canc: CAN控制器对象指针
 * @param[in] mode: 当前模式
 * @details
 * 此回调函数在中断上下文中被调用，用于通知模式切换。
 */
void xwds_canc_drvcb_mode_indication(struct xwds_canc * canc,
                                     xwsq_t mode);

/**
 * @brief XWDS Driver Callback：指示CAN控制器发生错误
 * @param[in] canc: CAN控制器对象指针
 * @param[in] errcode: 错误码
 * @param[in] tec: 发送错误计数
 * @param[in] rec: 接收错误计数
 * @details
 * 此回调函数在中断上下文中被调用，用于通知错误。
 */
void xwds_canc_drvcb_err_indication(struct xwds_canc * canc,
                                    xwsq_t errcode,
                                    xwsq_t tec,
                                    xwsq_t rec);

/**
 * @brief XWDS Driver Callback：指示CAN控制器发生busoff
 * @param[in] canc: CAN控制器对象指针
 * @details
 * 此回调函数在中断上下文中被调用，用于通知BUSOFF。
 */
void xwds_canc_drvcb_busoff_indication(struct xwds_canc * canc);

/**
 * @} xwcd_ds_can_controller
 */

#endif /* xwcd/ds/can/controller.h */
