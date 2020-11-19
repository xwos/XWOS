/**
 * @file
 * @brief 点对点通讯协议：硬件接口抽象层
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <xwos/standard.h>
#include <string.h>
#include <xwos/lib/xwaop.h>
#include <xwos/lib/xwlog.h>
#include <xwmd/isc/xwpcp/hwifal.h>
#include <xwmd/isc/xwpcp/protocol.h>

/**
 * @brief 空白帧
 */
__xwmd_rodata const xwu8_t xwpcp_frm_blank[] = {
        (xwu8_t)XWPCP_HWIFAL_EOF,
        (xwu8_t)XWPCP_HWIFAL_EOF,
        (xwu8_t)XWPCP_HWIFAL_EOF,
        (xwu8_t)XWPCP_HWIFAL_EOF,
        (xwu8_t)XWPCP_HWIFAL_EOF,
        (xwu8_t)XWPCP_HWIFAL_EOF,
        (xwu8_t)XWPCP_HWIFAL_EOF,
        (xwu8_t)XWPCP_HWIFAL_EOF,
};

/**
 * @brief 打开硬件接口
 * @param xwpcp: (I) XWPCP对象的指针
 * @param hwifcb: (I) 硬件接口控制块指针
 * @return 错误码
 */
__xwmd_code
xwer_t xwpcp_hwifal_open(struct xwpcp * xwpcp, void * hwifcb)
{
        xwer_t rc;

        rc = xwaop_teq_then_add(xwsq, &xwpcp->refcnt,
                                XWPCP_REFCNT_STARTED, 1,
                                NULL, NULL);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_grab_xwpcp;
        }

        xwpcp->hwifcb = hwifcb;
        if (__xwcc_likely((xwpcp->hwifops) && (xwpcp->hwifops->open))) {
                rc = xwpcp->hwifops->open(xwpcp);
        } else {
                rc = XWOK;
        }
        if (__xwcc_unlikely(rc < 0)) {
                goto err_hwifops_open;
        }

        xwaop_write(xwsq, &xwpcp->hwifst, XWPCP_HWIFST_RX, NULL);
        xwpcplogf(INFO, "Open HWIF ... [OK]\n");
        return rc;

err_hwifops_open:
        xwpcp->hwifcb = NULL;
        xwaop_sub(xwsq, &xwpcp->refcnt, 1, NULL, NULL);
err_grab_xwpcp:
        xwpcplogf(INFO, "Failed to open HWIF ... [rc:%d]\n", rc);
        return rc;
}

/**
 * @brief 关闭硬件接口
 * @param xwpcp: (I) XWPCP对象的指针
 * @return 错误码
 */
__xwmd_code
xwer_t xwpcp_hwifal_close(struct xwpcp * xwpcp)
{
        xwer_t rc;

        rc = xwaop_t1ma_then_c0m(xwsq, &xwpcp->hwifst, XWPCP_HWIFST_RX,
                                 NULL, NULL);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_xwpcp_clear_hwifst;
        }

        if ((xwpcp->hwifops) && (xwpcp->hwifops->close)) {
                rc = xwpcp->hwifops->close(xwpcp);
        } else {
                rc = XWOK;
        }
        if (__xwcc_unlikely(rc < 0)) {
                goto err_hwifops_close;
        }
        xwpcp->hwifcb = NULL;
        xwpcplogf(INFO, "close HWIF ... [OK]\n");
        return XWOK;

err_hwifops_close:
        xwaop_s1m(xwsq, &xwpcp->hwifst, XWPCP_HWIFST_RX, NULL, NULL);
err_xwpcp_clear_hwifst:
        xwpcplogf(INFO, "Open HWIF ... [rc:%d]\n", rc);
        return rc;
}

/**
 * @brief 通过硬件接口发送一帧
 * @param xwpcp: (I) XWPCP对象的指针
 * @param frm: (I) 指向发送帧缓冲区的指针
 * @return 错误码
 */
__xwmd_code
xwer_t xwpcp_hwifal_tx(struct xwpcp * xwpcp, struct xwpcp_frame * frm)
{
        xwu8_t * stream;
        xwsz_t txsize;

        stream = (xwu8_t *)frm;
        txsize = frm->head.frmlen + 2U;
        return xwpcp->hwifops->tx(xwpcp, stream, txsize);
}

/**
 * @brief 通过硬件接口接收一帧
 * @param xwpcp: (I) XWPCP对象的指针
 * @param frmslotbuf: (O) 指向缓冲区的指针，通过此缓冲区返回struct xwpcp_frmslot *
 * @return 错误码
 */
__xwmd_code
xwer_t xwpcp_hwifal_rx(struct xwpcp * xwpcp, struct xwpcp_frmslot ** frmslotbuf)
{
        union {
                xwu8_t data[sizeof(struct xwpcp_frmhead)];
                struct xwpcp_frmhead head;
        } stream;
        xwu8_t delimiter;
        xwsz_t rxsize;
        xwsz_t total;
        xwssz_t rest;
        xwsz_t need, neednum;
        xwssq_t odr;
        void * mem;
        struct xwpcp_frmslot * frmslot;
        xwer_t rc;

        /* 接收帧首定界符 */
        do {
                rxsize = 1;
                rc = xwpcp->hwifops->rx(xwpcp, &delimiter, &rxsize);
                if (__xwcc_unlikely(rc < 0)) {
                        goto err_sof_ifrx;
                }
        } while (XWPCP_HWIFAL_SOF != delimiter);

        /* 接收head */
        do {
                total = 2;
                rest = (xwssz_t)total;
                do {
                        rxsize = (xwsz_t)rest;
                        rc = xwpcp->hwifops->rx(xwpcp,
                                                &stream.data[total - rxsize],
                                                &rxsize);
                        if (__xwcc_unlikely(rc < 0)) {
                                goto err_head_ifrx;
                        }
                        rest -= (xwssz_t)rxsize;
                } while (rest > 0);
                if (XWPCP_HWIFAL_SOF == stream.data[0]) {
                        if (XWPCP_HWIFAL_EOF == stream.data[1]) {
                                total = sizeof(struct xwpcp_frmhead);
                                rest = (xwssz_t)total - 2;
                                break;
                        } else if (XWPCP_HWIFAL_SOF == stream.data[1]) {
                                /* continue; */
                        } else {
                                total = sizeof(struct xwpcp_frmhead);
                                rest = (xwssz_t)total - 1;
                                stream.data[0] = stream.data[1];
                                break;
                        }
                } else {
                        total = sizeof(struct xwpcp_frmhead);
                        rest = (xwssz_t)total - 2;
                        break;
                }
        } while (true);
        do {
                rxsize = (xwsz_t)rest;
                rc = xwpcp->hwifops->rx(xwpcp,
                                        &stream.data[total - rxsize],
                                        &rxsize);
                if (__xwcc_unlikely(rc < 0)) {
                        goto err_head_ifrx;
                }
                rest -= (xwssz_t)rxsize;
        } while (rest > 0);
        if (stream.head.frmlen != xwbop_rbit(xwu8_t, stream.head.mirror)) {
                rc = -EAGAIN;
                goto err_head_ifrx;
        }
        if (stream.head.frmlen < (sizeof(struct xwpcp_frmhead) + XWPCP_CHKSUM_SIZE)) {
                rc = -EAGAIN;
                goto err_head_ifrx;
        }

        /* 申请接收帧槽 */
        need = stream.head.frmlen + sizeof(struct xwpcp_frmslot) -
               sizeof(struct xwpcp_frame);
        neednum = DIV_ROUND_UP(need, XWPCP_MEMBLK_SIZE);
        odr = xwbop_fls(xwsz_t, neednum);
        if ((odr < 0) || ((XWPCP_MEMBLK_SIZE << odr) < need)) {
                odr++;
        }
        rc = xwmm_bma_alloc(xwpcp->slot.pool, (xwsq_t)odr, &mem);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_bma_alloc;
        }
        frmslot = mem;
        frmslot->cbfunc = NULL;
        frmslot->cbarg = NULL;
        xwlib_bclst_init_node(&frmslot->node);
        frmslot->priority = XWPCP_INVALID_PRIORITY;
        memcpy(&frmslot->frm.head, &stream.head, sizeof(stream.head));

        /* 接收body */
        total = frmslot->frm.head.frmlen - sizeof(struct xwpcp_frmhead);
        rest = (xwssz_t)total;
        do {
                rxsize = (xwsz_t)rest;
                rc = xwpcp->hwifops->rx(xwpcp,
                                        &frmslot->frm.sdu[total - rxsize],
                                        &rxsize);
                if (__xwcc_unlikely(rc < 0)) {
                        goto err_body_ifrx;
                }
                rest -= (xwssz_t)rxsize;
        } while (rest > 0);

        /* 接收帧尾定界符 */
        rxsize = 1;
        xwpcp->hwifops->rx(xwpcp, &delimiter, &rxsize);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_eof_ifrx;
        }
        if (XWPCP_HWIFAL_EOF != delimiter) {
                rc = -EAGAIN;
                goto err_eof_ifrx;
        }

        *frmslotbuf = frmslot;
        return XWOK;

err_eof_ifrx:
err_body_ifrx:
        xwmm_bma_free(xwpcp->slot.pool, mem);
err_bma_alloc:
err_head_ifrx:
err_sof_ifrx:
        return rc;
}

/**
 * @brief 通知来自协议层的事件到硬件接口层
 * @param xwpcp: (I) XWPCP对象的指针
 * @param evt: (I) 事件
 */
__xwmd_code
void xwpcp_hwifal_notify(struct xwpcp * xwpcp, xwsq_t evt)
{
        if (XWPCP_HWIFNTF_NETUNREACH == evt) {
                xwpcp->hwifops->tx(xwpcp, xwpcp_frm_blank, sizeof(xwpcp_frm_blank));
        }
        if (xwpcp->hwifops->notify) {
                xwpcp->hwifops->notify(xwpcp, evt);
        }
}
