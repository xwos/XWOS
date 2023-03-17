/**
 * @file
 * @brief SOC间点对点通讯协议：模块接口
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef __xwmd_isc_xwssc_mif_h__
#define __xwmd_isc_xwssc_mif_h__

#include <xwos/standard.h>

/**
 * @defgroup xwmd_isc_xwssc 点对点通讯协议
 * @ingroup xwmd_isc
 * @{
 */

#if (XWMDCFG_isc_xwssc_MEMBLK_SIZE & (XWMDCFG_isc_xwssc_MEMBLK_SIZE - 1))
#  error "XWMDCFG_isc_xwssc_MEMBLK_SIZE must be the power of 2!"
#endif
#if ((1 << XWMDCFG_isc_xwssc_MEMBLK_ODR) > 32768U)
#  error "power(2, XWMDCFG_isc_xwssc_MEMBLK_ODR) must be <= 32768!"
#endif
#if (XWMDCFG_isc_xwssc_PORT_NUM > 256)
#  error "XWSSC Only supports 256 ports (0 ~ 255)!"
#endif
#if (XWMDCFG_isc_xwssc_PRI_NUM > 64)
#  error "XWSSC Only supports 64 priorities!"
#endif

#define XWSSC_VERSION           ("1.0.0") /**< XWSSC的版本号字符串 */
#define XWSSC_VERSION_MAJOR     1U /**< XWSSC的大版本号 */
#define XWSSC_VERSION_MINOR     0U /**< XWSSC的小版本号 */
#define XWSSC_VERSION_REVISION  0U /**< XWSSC的修订版本号 */

#define XWSSC_PRI_NUM           (XWMDCFG_isc_xwssc_PRI_NUM) /**< 优先级数量 */
#define XWSSC_INVALID_PRI       (0xFFU) /**< 无效优先级 */
#define XWSSC_MAX_PRI           (XWSSC_PRI_NUM - 1) /**< XWSSC的大版本号 */
#define XWSSC_PORT_NUM          (XWMDCFG_isc_xwssc_PORT_NUM) /**< 端口号 */
#define XWSSC_RETRY_PERIOD      (XWMDCFG_isc_xwssc_RETRY_PERIOD) /**< 重试发送的周期 */
#define XWSSC_RETRY_NUM         (XWMDCFG_isc_xwssc_RETRY_NUM) /**< 重试发送的次数 */
#define XWSSC_MEMBLK_SIZE       (XWMDCFG_isc_xwssc_MEMBLK_SIZE) /**< 单位内存块的大小 */
#define XWSSC_MEMBLK_ODR        (XWMDCFG_isc_xwssc_MEMBLK_ODR) /**< 内存块数量的阶，内存块数量等于1左移“阶”次 */
#define XWSSC_MEMBLK_NUM        (1 << XWSSC_MEMBLK_ODR) /**< 内存块的数量 */
#define XWSSC_MEMPOOL_SIZE      (XWSSC_MEMBLK_SIZE * XWSSC_MEMBLK_NUM) /**< 内存池的总大小 */

/**
 * @brief 定义内存池
 * @param[in] name: 数值名
 */
#define XWSSC_DEF_MEMPOOL(name) __xwcc_alignl1cache xwu8_t name[XWSSC_MEMPOOL_SIZE]

struct xwssc;
struct xwssc_hwifal_operation;
struct xwssc_carrier;

/**
 * @brief 发送句柄
 */
typedef struct xwssc_carrier * xwssc_txh_t;

/**
 * @brief 发送状态枚举
 */
enum xwssc_carrier_state_em {
        XWSSC_CRS_IDLE = 0,
        XWSSC_CRS_READY,
        XWSSC_CRS_INPROGRESS,
        XWSSC_CRS_ABORT,
        XWSSC_CRS_FINISH,
};

/**
 * @brief 服务质量枚举
 */
enum xwssc_msg_qos_em {
        XWSSC_MSG_QOS_0 = 0, /**< 不可靠消息，无CRC校验码 */
        XWSSC_MSG_QOS_1, /**< 不可靠消息，有CRC校验码 */
        XWSSC_MSG_QOS_2, /**< 可靠消息，无CRC校验码 */
        XWSSC_MSG_QOS_3, /**< 可靠消息，有CRC校验码 */
        XWSSC_MSG_QOS_NUM,

        XWSSC_MSG_QOS_CHKSUM_MSK = (1 << 0U),
        XWSSC_MSG_QOS_ACK_MSK = (1 << 1U),
};

/**
 * @brief 通知发送结果的回调函数
 */
typedef void (* xwssc_ntf_f)(struct xwssc * /* xwssc */,
                             xwssc_txh_t /* txh */,
                             xwer_t /* rc */,
                             void * /* arg */);

xwer_t xwssc_start(struct xwssc * xwssc, const char * name,
                   const struct xwssc_hwifal_operation * hwifops, void * hwifcb,
                   xwu8_t * mem, xwsz_t memsize);
xwer_t xwssc_stop(struct xwssc * xwssc);
xwer_t xwssc_tx(struct xwssc * xwssc,
                const xwu8_t data[], xwsz_t * size,
                xwu8_t pri, xwu8_t port, xwu8_t qos,
                xwtm_t to);
xwer_t xwssc_eq(struct xwssc * xwssc,
                const xwu8_t data[], xwsz_t * size,
                xwu8_t pri, xwu8_t port, xwu8_t qos,
                xwssc_ntf_f ntfcb, void * cbarg,
                xwssc_txh_t * txhbuf);
xwer_t xwssc_abort(struct xwssc * xwssc, xwssc_txh_t txh);
xwsq_t xwssc_get_txstate(xwssc_txh_t txh);
xwer_t xwssc_rx(struct xwssc * xwssc, xwu8_t port,
                xwu8_t rxbuf[], xwsz_t * size, xwu8_t * qos,
                xwtm_t to);
xwer_t xwssc_try_rx(struct xwssc * xwssc, xwu8_t port,
                    xwu8_t rxbuf[], xwsz_t * size, xwu8_t * qos);

/**
 * @} xwmd_isc_xwssc
 */

#endif /* xwmd/isc/xwssc/mif.h */
