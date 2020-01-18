/**
 * @file
 * @brief xwmd设备栈：LIN控制器
 * @author
 * + 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __xwmd_ds_lin_controller_h__
#define __xwmd_ds_lin_controller_h__

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <xwos/lib/xwbop.h>
#include <xwos/osal/lock/mutex.h>
#include <xwmd/ds/device.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define SODS_LIN_DIAG_MSTREQ_MSG        0x3C
#define SODS_LIN_DIAG_SLCREP_MSG        0x3D
#define SODS_LIN_DEFAULT_DATA_SIZE      8

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief LIN接口模式枚举
 */
enum xwds_lin_mode_em {
        SODS_LIN_MODE_CLOSE = 0,
        SODS_LIN_MODE_TX = BIT(0),
        SODS_LIN_MODE_RX = BIT(1),
};

/**
 * @brief LIN协议类型
 */
enum xwds_lin_chksum_type_em {
        SODS_LIN_CHKSUM_CLASSIC = 0,
        SODS_LIN_CHKSUM_ENHANCED,
};

/**
 * @brief LIN消息结构体
 */
struct xwds_lin_msg {
        xwu8_t id;
        xwu8_t data[SODS_LIN_DEFAULT_DATA_SIZE];
        xwu8_t size;
};

/**
 * @brief LIN消息信息
 */
struct xwds_lin_msg_info {
        xwu8_t protected_id; /**< LIN保护ID */
        xwu8_t size; /**< 消息大小 */
};

/**
 * @brief LIN控制器配置
 */
struct xwds_lin_cfg {
        xwu32_t baudrate; /**< 波特率 */
        union {
                struct {
                        xwu32_t mode:8; /**< 模式 */
                        xwu32_t chksum:1; /**< 校验方式 */
                        xwu32_t reserved:23;
                } bit;
                xwu32_t u32;
        } attr;
        void * xwccfg; /**< SOC芯片私有配置 */
};

struct xwds_linc;

/**
 * @brief LIN控制器驱动函数表
 */
struct xwds_linc_driver {
        struct xwds_driver base; /**< C语言面向对象：继承struct xwds_driver */
        xwer_t (* msttx)(struct xwds_linc *, xwu8_t, struct xwds_lin_msg *,
                         xwtm_t *); /**< 主机节点发送 */
        xwer_t (* slvtx)(struct xwds_linc *, struct xwds_lin_msg *,
                         xwtm_t *); /**< 从机节点发送 */
        xwer_t (* rx)(struct xwds_linc *, struct xwds_lin_msg *,
                      xwtm_t *); /**< 接收 */
};

/**
 * @brief LIN控制器
 */
struct xwds_linc {
        struct xwds_device dev; /**< 继承struct xwds_device */

        /* attributes */
        const struct xwds_lin_cfg * cfg; /**< 配置s */
        const struct xwds_lin_msg_info * msgitable; /**< LIN消息信息表 */
        xwsz_t msgitable_itemnum; /**< LIN消息信息表条目数量 */
        xwer_t (*get_msg_size)(struct xwds_linc *,
                               xwu8_t ,
                               xwu8_t *); /**< 虚函数：从LIN消息表中查询消息的大小。
                                               默认 @ref xwds_linc_get_msg_size()
                                               用户可以重载这个函数以便实现
                                               高效率的查询算法。*/

        /* private */
        struct xwosal_mtx txlock; /**< 提供多线程访问安全的锁 */
};

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         function prototypes         ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/******** ******** ******** constructor & destructor ******** ******** ********/
#if !defined(XWMDCFG_ds_NANO) || (1 != XWMDCFG_ds_NANO)
__xwds_code
void xwds_linc_construct(struct xwds_linc * linc);

__xwds_code
void xwds_linc_destruct(struct xwds_linc * linc);
#endif /* !XWMDCFG_ds_NANO */

/******** ******** ******** APIs ******** ******** ********/
#if defined(XWMDCFG_ds_NANO) && (1 == XWMDCFG_ds_NANO)
__xwds_api
xwer_t xwds_linc_probe(struct xwds_linc * linc);

__xwds_api
xwer_t xwds_linc_remove(struct xwds_linc * linc);

__xwds_api
xwer_t xwds_linc_start(struct xwds_linc * linc);

__xwds_api
xwer_t xwds_linc_stop(struct xwds_linc * linc);

#if defined(XWMDCFG_ds_LPM) && (1 == XWMDCFG_ds_LPM)
__xwds_api
xwer_t xwds_linc_suspend(struct xwds_linc * linc);

__xwds_api
xwer_t xwds_linc_resume(struct xwds_linc * linc);
#endif /* XWMDCFG_ds_LPM */
#endif /* XWMDCFG_ds_NANO */

__xwds_api
xwer_t xwds_linc_msttx(struct xwds_linc * linc,
                       xwu8_t id, struct xwds_lin_msg * msg,
                       xwtm_t * xwtm);

__xwds_api
xwer_t xwds_linc_slvtx(struct xwds_linc * linc,
                       struct xwds_lin_msg * msg,
                       xwtm_t * xwtm);

__xwds_api
xwer_t xwds_linc_rx(struct xwds_linc * linc,
                    struct xwds_lin_msg * msgbuf,
                    xwtm_t * xwtm);

__xwds_api
xwer_t xwds_linc_get_msg_size(struct xwds_linc * linc,
                              xwu8_t protected_id,
                              xwu8_t * ret);

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********   inline function implementations   ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#if !defined(XWMDCFG_ds_NANO) || (1 != XWMDCFG_ds_NANO)
/**
 * @brief 增加对象的引用计数
 * @param linc: (I) LIN控制器对象指针
 * @return 错误码
 * @retval @ref xwds_device_grab()
 */
static __xw_inline
xwer_t xwds_linc_grab(struct xwds_linc * linc)
{
        return xwds_device_grab(&linc->dev);
}

/**
 * @brief 减少对象的引用计数
 * @param linc: (I) LIN控制器对象指针
 * @return 错误码
 * @retval @ref xwds_device_put()
 */
static __xw_inline
xwer_t xwds_linc_put(struct xwds_linc * linc)
{
        return xwds_device_put(&linc->dev);
}

/**
 * @brief 增加设备运行状态计数器
 * @param linc: (I) LIN控制器对象指针
 * @return 错误码
 * @retval @ref xwds_device_request()
 */
static __xw_inline
xwer_t xwds_linc_request(struct xwds_linc * linc)
{
        return xwds_device_request(&linc->dev);
}

/**
 * @brief 减少设备运行状态计数器
 * @param linc: (I) LIN控制器对象指针
 * @return 错误码
 * @retval @ref xwds_device_release()
 */
static __xw_inline
xwer_t xwds_linc_release(struct xwds_linc * linc)
{
        return xwds_device_release(&linc->dev);
}
#endif /* !XWMDCFG_ds_NANO */

#endif /* xwmd/ds/lin/controller.h */
