/**
 * @file
 * @brief 精简的点对点通讯协议：应用接口层
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __xwmd_isc_xwscp_api_h__
#define __xwmd_isc_xwscp_api_h__

#include <xwos/standard.h>

struct xwscp;
struct xwscp_hwifal_operations;

xwer_t xwscp_start(struct xwscp * xwscp, const char * name,
                   const struct xwscp_hwifal_operations * hwifops,
                   void * hwifcb);
xwer_t xwscp_stop(struct xwscp * xwscp);
xwer_t xwscp_connect(struct xwscp * xwscp, xwtm_t * xwtm);
xwer_t xwscp_tx(struct xwscp * xwscp,
                const xwu8_t msg[], xwsz_t * size,
                xwtm_t * xwtm);
xwer_t xwscp_rx(struct xwscp * xwscp,
                xwu8_t buf[], xwsz_t * size,
                xwtm_t * xwtm);

#endif /* xwmd/isc/xwscp/api.h */
