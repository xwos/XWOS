/**
 * @file
 * @brief xwmd设备栈：SOC DMA
 * @author
 * + 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __xwmd_ds_soc_dma_h__
#define __xwmd_ds_soc_dma_h__

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <xwos/lib/xwbop.h>
#include <xwmd/ds/soc/chip.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief SODS DMA 通道
 */
#define SODS_DMA_CH(x)          ((xwid_t)x)

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         function prototypes         ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
__xwds_api
xwer_t xwds_dma_req(struct xwds_soc * soc, xwid_t ch);

__xwds_api
xwer_t xwds_dma_rls(struct xwds_soc * soc, xwid_t ch);

__xwds_api
xwer_t xwds_dma_cfg(struct xwds_soc * soc, xwid_t ch, void * cfg,
                    xwds_dma_f cb, xwds_dma_cbarg_t arg);

__xwds_api
xwer_t xwds_dma_enable(struct xwds_soc * soc, xwid_t ch);

__xwds_api
xwer_t xwds_dma_disable(struct xwds_soc * soc, xwid_t ch);

__xwds_api
xwer_t xwds_dma_start(struct xwds_soc * soc, xwid_t ch);

__xwds_api
xwer_t xwds_dma_stop(struct xwds_soc * soc, xwid_t ch);

__xwds_lib_code
xwer_t xwds_get_dmarsc(const struct xwds_resource_dma base[], xwsz_t num,
                       const char * descay[], xwsz_t descnum,
                       const struct xwds_resource_dma ** ret);

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********  inline functions implementations   ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

#endif /* xwmd/ds/soc/dma.h */
