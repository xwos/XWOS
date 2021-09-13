/**
 * @file
 * @brief 精简的点对点通讯协议：模块接口
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __xwmd_isc_xwscp_mif_h__
#define __xwmd_isc_xwscp_mif_h__

#include <xwos/standard.h>

#if (XWMDCFG_isc_xwscp_MEMBLK_SIZE & (XWMDCFG_isc_xwscp_MEMBLK_SIZE - 1))
#  error "XWMDCFG_isc_xwscp_MEMBLK_SIZE must be the power of 2!"
#endif
#if ((1 << XWMDCFG_isc_xwscp_MEMBLK_ODR) > 32768U)
#  error "power(2, XWMDCFG_isc_xwscp_MEMBLK_ODR) must be <= 32768!"
#endif

#define XWSCP_RETRY_PERIOD      (XWMDCFG_isc_xwscp_PERIOD)
#define XWSCP_MEMBLK_SIZE       (XWMDCFG_isc_xwscp_MEMBLK_SIZE)
#define XWSCP_MEMBLK_ODR        (XWMDCFG_isc_xwscp_MEMBLK_ODR)
#define XWSCP_MEMBLK_NUM        (1 << XWSCP_MEMBLK_ODR)
#define XWSCP_MEMPOOL_SIZE      (XWSCP_MEMBLK_SIZE * XWSCP_MEMBLK_NUM)
#define XWSCP_DEF_MEMPOOL(name) __xwcc_alignl1cache xwu8_t name[XWSCP_MEMPOOL_SIZE]

#define XWSCP_VERSION           ("3.0.0")
#define XWSCP_VERSION_MAJOR     3U
#define XWSCP_VERSION_MINOR     0U
#define XWSCP_VERSION_REVISION  0U

/**
 * @brief 服务质量枚举
 */
enum xwscp_msg_qos_em {
        XWSCP_MSG_QOS_0 = 0, /**< 不可靠消息，无CRC校验码 */
        XWSCP_MSG_QOS_1, /**< 不可靠消息，有CRC校验码 */
        XWSCP_MSG_QOS_2, /**< 可靠消息，无CRC校验码 */
        XWSCP_MSG_QOS_3, /**< 可靠消息，有CRC校验码 */
        XWSCP_MSG_QOS_NUM,

        XWSCP_MSG_QOS_CHKSUM_MSK = (1 << 0U),
        XWSCP_MSG_QOS_ACK_MSK = (1 << 1U),
};

struct xwscp;
struct xwscp_hwifal_operations;

xwer_t xwscp_start(struct xwscp * xwscp, const char * name,
                   const struct xwscp_hwifal_operations * hwifops, void * hwifcb,
                   xwu8_t * mem, xwsz_t memsize);
xwer_t xwscp_stop(struct xwscp * xwscp);
xwer_t xwscp_connect(struct xwscp * xwscp, xwtm_t * xwtm);
xwer_t xwscp_tx(struct xwscp * xwscp,
                const xwu8_t msg[], xwsz_t * size, xwu8_t qos,
                xwtm_t * xwtm);
xwer_t xwscp_rx(struct xwscp * xwscp,
                xwu8_t buf[], xwsz_t * size,
                xwtm_t * xwtm);
xwer_t xwscp_try_rx(struct xwscp * xwscp,
                    xwu8_t buf[], xwsz_t * size);

#endif /* xwmd/isc/xwscp/mif.h */
