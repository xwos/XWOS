/**
 * @file
 * @brief 精简的点对点通讯协议：硬件接口抽象层
 * @author
 * + 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <xwos/lib/xwaop.h>
#include <xwos/lib/string.h>
#include <xwmd/isc/xwscp/protocol.h>
#include <xwmd/isc/xwscp/hwifal.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief 空白帧
 */
__xwmd_rodata const xwu8_t xwscp_frm_blank[] = {
        (xwu8_t)XWSCP_HWIFAL_EOF,
        (xwu8_t)XWSCP_HWIFAL_EOF,
        (xwu8_t)XWSCP_HWIFAL_EOF,
        (xwu8_t)XWSCP_HWIFAL_EOF,
        (xwu8_t)XWSCP_HWIFAL_EOF,
        (xwu8_t)XWSCP_HWIFAL_EOF,
        (xwu8_t)XWSCP_HWIFAL_EOF,
        (xwu8_t)XWSCP_HWIFAL_EOF,
};

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      static function prototypes     ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief 打开硬件接口
 * @param xwscp: (I) XWSCP对象的指针
 * @return 错误码
 */
__xwmd_code
xwer_t xwscp_hwifal_open(struct xwscp * xwscp)
{
        xwer_t rc;

        if ((xwscp->hwifops) && (xwscp->hwifops->open)) {
                rc = xwscp->hwifops->open(xwscp);
        } else {
                rc = OK;
        }
        xwscp->hwifst = XWSCP_HWIFST_OPENED;
        return rc;
}

/**
 * @brief 关闭硬件接口
 * @param xwscp: (I) XWSCP对象的指针
 * @return 错误码
 */
__xwmd_code
xwer_t xwscp_hwifal_close(struct xwscp * xwscp)
{
        xwer_t rc;

        if ((xwscp->hwifops) && (xwscp->hwifops->close)) {
                rc = xwscp->hwifops->close(xwscp);
        } else {
                rc = OK;
        }
        if (OK == rc) {
                xwscp->hwifst = XWSCP_HWIFST_CLOSED;
        }
        return rc;
}

/**
 * @brief 通过硬件接口发送一帧
 * @param xwscp: (I) XWSCP对象的指针
 * @param frm: (I) 指向发送帧缓冲区的指针
 * @param xwtm: 指向缓冲区的指针，此缓冲区：
 *              (I) 作为输入时，表示期望的阻塞等待时间
 *              (O) 作为输出时，返回剩余的期望时间
 * @return 错误码
 */
__xwmd_code
xwer_t xwscp_hwifal_tx(struct xwscp * xwscp, struct xwscp_frame * frm, xwtm_t * xwtm)
{
        xwu8_t * stream;
        xwsz_t txsize;

        stream = (xwu8_t *)frm;
        txsize = frm->head.frmlen + 2U;
        return xwscp->hwifops->tx(xwscp, stream, txsize, xwtm);
}

/**
 * @brief 通过硬件接口接收一帧
 * @param xwscp: (I) XWSCP对象的指针
 * @param frmslotbuf: (O) 指向缓冲区的指针，通过此缓冲区返回struct xwscp_frmslot *
 * @return 错误码
 */
__xwmd_code
xwer_t xwscp_hwifal_rx(struct xwscp * xwscp, struct xwscp_frmslot ** frmslotbuf)
{
        union {
                xwu8_t data[sizeof(struct xwscp_frmhead)];
                struct xwscp_frmhead head;
        } stream;
        xwu8_t delimiter;
        xwsz_t rxsize;
        xwsz_t total;
        xwssz_t rest;
        struct xwscp_frmslot * frmslot;
        xwer_t rc;

        /* 接收帧首定界符 */
        do {
                rxsize = 1;
                rc = xwscp->hwifops->rx(xwscp, &delimiter, &rxsize);
                if (__unlikely(rc < 0)) {
                        goto err_sof_ifrx;
                }
        } while (XWSCP_HWIFAL_SOF != delimiter);

        /* 接收head */
        do {
                total = 2;
                rest = (xwssz_t)total;
                do {
                        rxsize = (xwsz_t)rest;
                        rc = xwscp->hwifops->rx(xwscp,
                                                &stream.data[total - rxsize],
                                                &rxsize);
                        if (__unlikely(rc < 0)) {
                                goto err_head_ifrx;
                        }
                        rest -= (xwssz_t)rxsize;
                } while (rest > 0);
                if (XWSCP_HWIFAL_SOF == stream.data[0]) {
                        if (XWSCP_HWIFAL_EOF == stream.data[1]) {
                                total = sizeof(struct xwscp_frmhead);
                                rest = (xwssz_t)total - 2;
                                break;
                        } else if (XWSCP_HWIFAL_SOF == stream.data[1]) {
                                /* continue; */
                        } else {
                                total = sizeof(struct xwscp_frmhead);
                                rest = (xwssz_t)total - 1;
                                stream.data[0] = stream.data[1];
                                break;
                        }
                } else {
                        total = sizeof(struct xwscp_frmhead);
                        rest = (xwssz_t)total - 2;
                        break;
                }
        } while (true);
        do {
                rxsize = (xwsz_t)rest;
                rc = xwscp->hwifops->rx(xwscp,
                                        &stream.data[total - rxsize],
                                        &rxsize);
                if (__unlikely(rc < 0)) {
                        goto err_head_ifrx;
                }
                rest -= (xwssz_t)rxsize;
        } while (rest > 0);
        if (stream.head.frmlen != xwbop_rbit(xwu8_t, stream.head.mirror)) {
                rc = -EAGAIN;
                goto err_head_ifrx;
        }
        if (stream.head.frmlen < (sizeof(struct xwscp_frmhead) + XWSCP_CHKSUM_SIZE)) {
                rc = -EAGAIN;
                goto err_head_ifrx;
        }

        /* 申请接收帧槽 */
        rc = xwscp_alloc_frmslot(xwscp, &frmslot);
        if (__unlikely(rc < 0)) {
                goto err_alloc_frmslot;
        }
        xwlib_bclst_init_node(&frmslot->node);
        memcpy(&frmslot->frm.head, &stream.head, sizeof(stream.head));

        /* 接收body */
        total = frmslot->frm.head.frmlen - sizeof(struct xwscp_frmhead);
        rest = (xwssz_t)total;
        do {
                rxsize = (xwsz_t)rest;
                rc = xwscp->hwifops->rx(xwscp,
                                        &frmslot->frm.sdu[total - rxsize],
                                        &rxsize);
                if (__unlikely(rc < 0)) {
                        goto err_body_ifrx;
                }
                rest -= (xwssz_t)rxsize;
        } while (rest > 0);

        /* 接收帧尾定界符 */
        rxsize = 1;
        xwscp->hwifops->rx(xwscp, &delimiter, &rxsize);
        if (__unlikely(rc < 0)) {
                goto err_eof_ifrx;
        }
        if (XWSCP_HWIFAL_EOF != delimiter) {
                rc = -EAGAIN;
                goto err_eof_ifrx;
        }
        *frmslotbuf = frmslot;
        return OK;

err_eof_ifrx:
err_body_ifrx:
        xwscp_free_frmslot(xwscp, frmslot);
err_alloc_frmslot:
err_head_ifrx:
err_sof_ifrx:
        return rc;
}

/**
 * @brief 通知协议层的事件到硬件接口层
 * @param xwscp: (I) XWSCP对象的指针
 * @param evt: (I) 事件
 */
__xwmd_code
void xwscp_hwifal_notify(struct xwscp * xwscp, xwsq_t evt, xwtm_t * xwtm)
{
        if (XWSCP_HWIFNTF_NETUNREACH == evt) {
                xwscp->hwifops->tx(xwscp, xwscp_frm_blank,
                                   sizeof(xwscp_frm_blank),
                                   xwtm);
        }
        if (xwscp->hwifops->notify) {
                xwscp->hwifops->notify(xwscp, evt);
        }
}
