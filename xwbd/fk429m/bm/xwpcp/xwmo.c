/**
 * @file
 * @brief Board Module: xwpcp
 * @author
 * + 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <www.starsoul.tech>
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

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <xwos/osal/scheduler.h>
#include <xwos/osal/thread.h>
#include <xwmd/isc/xwpcp/protocol.h>
#include <xwmd/isc/xwpcp/api.h>
#include <xwmd/ds/uart/dma.h>
#include <bm/stm32cube/xwac/xwds/stm32cube.h>
#include <bm/xwpcp/xwmo.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define BM_XWPCP_TXTHRD_PRIORITY XWOSAL_SD_PRIORITY_DROP(XWOSAL_SD_PRIORITY_RT_MAX, 1)
#define BM_XWPCP_RXTHRD_PRIORITY XWOSAL_SD_PRIORITY_DROP(XWOSAL_SD_PRIORITY_RT_MAX, 1)

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         function prototypes         ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
static
xwer_t bm_xwpcpif_open(struct xwpcp * xwpcp);

static
xwer_t bm_xwpcpif_close(struct xwpcp * xwpcp);

static
xwer_t bm_xwpcpif_tx(struct xwpcp * xwpcp, const xwu8_t * data, xwsz_t size);

static
xwer_t bm_xwpcpif_rx(struct xwpcp * xwpcp, xwu8_t * buf, xwsz_t * size);

static
void bm_xwpcpif_notify(struct xwpcp * xwpcp, xwsq_t ntf);

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
struct xwpcp bm_xwpcp;

const struct xwosal_thrd_desc bm_xwpcp_rxthrd_td = {
        .name = "xwmd.isc.xwpcp.rxthread",
        .prio = BM_XWPCP_RXTHRD_PRIORITY,
        .stack = XWOSAL_THRD_STACK_DYNAMIC,
        .stack_size = 2048,
        .func = (xwosal_thrd_f)xwpcp_rxthrd,
        .arg = (void *)&bm_xwpcp,
        .attr = XWSDOBJ_ATTR_PRIVILEGED,
};
xwid_t bm_xwpcp_rxthrd;

const struct xwosal_thrd_desc bm_xwpcp_txthrd_td = {
        .name = "xwmd.isc.xwpcp.txthread",
        .prio = BM_XWPCP_TXTHRD_PRIORITY,
        .stack = XWOSAL_THRD_STACK_DYNAMIC,
        .stack_size = 2048,
        .func = (xwosal_thrd_f)xwpcp_txthrd,
        .arg = (void *)&bm_xwpcp,
        .attr = XWSDOBJ_ATTR_PRIVILEGED,
};
xwid_t bm_xwpcp_txthrd;

const struct xwpcp_hwifal_operations bm_xwpcpif_ops = {
        .open = bm_xwpcpif_open,
        .close = bm_xwpcpif_close,
        .tx = bm_xwpcpif_tx,
        .rx = bm_xwpcpif_rx,
        .notify = bm_xwpcpif_notify,
};

xwu8_t __aligned(XWMMCFG_ALIGNMENT) bm_xwpcp_mempool[XWPCP_MEMPOOL_SIZE];

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
xwer_t bm_xwpcp_start(void)
{
        xwer_t rc;
        xwer_t trc;

        xwpcp_init(&bm_xwpcp);
        rc = xwpcp_start(&bm_xwpcp, "bm.xwpcp", &bm_xwpcpif_ops,
                         (xwptr_t)bm_xwpcp_mempool, XWPCP_MEMPOOL_SIZE);
        if (rc < 0) {
                goto err_xwpcp_start;
        }

        rc = xwosal_thrd_create(&bm_xwpcp_rxthrd,
                                bm_xwpcp_rxthrd_td.name,
                                bm_xwpcp_rxthrd_td.func,
                                bm_xwpcp_rxthrd_td.arg,
                                bm_xwpcp_rxthrd_td.stack_size,
                                bm_xwpcp_rxthrd_td.prio,
                                bm_xwpcp_rxthrd_td.attr);
        if (rc < 0) {
                goto err_rxthrd_create;
        }

        rc = xwosal_thrd_create(&bm_xwpcp_txthrd,
                                bm_xwpcp_txthrd_td.name,
                                bm_xwpcp_txthrd_td.func,
                                bm_xwpcp_txthrd_td.arg,
                                bm_xwpcp_txthrd_td.stack_size,
                                bm_xwpcp_txthrd_td.prio,
                                bm_xwpcp_txthrd_td.attr);
        if (rc < 0) {
                goto err_txthrd_create;
        }

        return OK;

err_txthrd_create:
        xwosal_thrd_terminate(bm_xwpcp_rxthrd, &trc);
        xwosal_thrd_delete(bm_xwpcp_rxthrd);
err_rxthrd_create:
        xwpcp_stop(&bm_xwpcp);
err_xwpcp_start:
        return rc;
}

xwer_t bm_xwpcp_stop(void)
{
        xwer_t rc, trc;

        rc = xwosal_thrd_terminate(bm_xwpcp_txthrd, &trc);
        if (OK == rc) {
                rc = xwosal_thrd_delete(bm_xwpcp_txthrd);
                if (OK == rc) {
                        bm_xwpcp_txthrd = 0;
                        xwpcplogf(INFO, "Terminate XWPCP TX thread... [OK]\n");
                }
        }

        rc = xwosal_thrd_terminate(bm_xwpcp_rxthrd, &trc);
        if (OK == rc) {
                rc = xwosal_thrd_delete(bm_xwpcp_rxthrd);
                if (OK == rc) {
                        bm_xwpcp_rxthrd = 0;
                        xwpcplogf(INFO, "Terminate XWPCP RX thread... [OK]\n");
                }
        }

        rc = xwpcp_stop(&bm_xwpcp);
        if (OK == rc) {
                xwpcplogf(INFO, "Stop XWPCP ... [OK]\n");
        } else {
                xwpcplogf(INFO, "Failed to stop XWPCP ... [rc:%d]\n", rc);
        }

        return rc;
}

static
xwer_t bm_xwpcpif_open(struct xwpcp * xwpcp)
{
        xwpcp->hwifcb = &stm32cube_dmauartc1_cb;
        return OK;
}

static
xwer_t bm_xwpcpif_close(struct xwpcp * xwpcp)
{
        xwpcp->hwifcb = NULL;
        return OK;
}

static
xwer_t bm_xwpcpif_tx(struct xwpcp * xwpcp, const xwu8_t * data, xwsz_t size)
{
        xwtm_t desired;

        desired = XWTM_MAX;
        return xwds_dmauartc_tx(xwpcp->hwifcb, data, size, &desired);
}

static
xwer_t bm_xwpcpif_rx(struct xwpcp * xwpcp, xwu8_t * buf, xwsz_t * size)
{
        xwtm_t desired;

        desired = XWTM_MAX;
	return xwds_dmauartc_rx(xwpcp->hwifcb, buf, size, &desired);
}

static
void bm_xwpcpif_notify(struct xwpcp * xwpcp, xwsq_t ntf)
{
        XWOS_UNUSED(xwpcp);
        if (XWPCP_HWIFNTF_NETUNREACH == ntf) {
        }
}
