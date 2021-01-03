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
        (xwu8_t)XWPCP_EOF,
        (xwu8_t)XWPCP_EOF,
        (xwu8_t)XWPCP_EOF,
        (xwu8_t)XWPCP_EOF,
        (xwu8_t)XWPCP_EOF,
        (xwu8_t)XWPCP_EOF,
        (xwu8_t)XWPCP_EOF,
        (xwu8_t)XWPCP_EOF,
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
 * @brief 通过硬件接口发送数据
 * @param xwpcp: (I) XWPCP对象的指针
 * @param stream: (I) 数据
 * @param size: (I) 数据长度
 * @return 错误码
 */
__xwmd_code
xwer_t xwpcp_hwifal_tx(struct xwpcp * xwpcp, xwu8_t * stream, xwsz_t size)
{
        xwer_t rc;

        rc = xwpcp->hwifops->tx(xwpcp, stream, size);
        return rc;
}

/**
 * @brief 通过硬件接口接收一帧
 * @param xwpcp: (I) XWPCP对象的指针
 * @param slotbuf: (O) 指向缓冲区的指针，通过此缓冲区返回union xwpcp_slot *
 * @return 错误码
 */
__xwmd_code
xwer_t xwpcp_hwifal_rx(struct xwpcp * xwpcp, union xwpcp_slot ** slotbuf)
{
        xwu8_t delim;
        xwsz_t delimcnt;

        xwsz_t rxsize;
        xwsz_t total;
        xwssz_t rest;

        union {
                xwu8_t data[XWPCP_FRMHEAD_MAXSIZE];
                struct xwpcp_frmhead head;
        } stream;
        xwu8_t headsize;
        xwu8_t headsize_mirror;
        bool headchk;
        xwsz_t ecsize, sdusize;

        xwsz_t need, neednum;
        xwssq_t odr;
        void * mem;
        xwu8_t * sdupos;
        union xwpcp_slot * slot;
        xwer_t rc;

        /* 接收帧首定界符 */
        delimcnt = 0;
        do {
                rxsize = 1;
                rc = xwpcp->hwifops->rx(xwpcp, &delim, &rxsize);
                if (__xwcc_unlikely(rc < 0)) {
                        goto err_sof_ifrx;
                }
                if (XWPCP_SOF == delim) {
                        delimcnt++;
                } else {
                        break;
                }
        } while (true);
        if (delimcnt < XWPCP_SOF_SIZE) {
                rc = -EAGAIN;
                goto err_sof_ifrx;
        }

        /* 接收Head */
        stream.head.headsize = delim;
        headsize = XWPCP_FRMHEAD_SIZE(stream.head.headsize);
        headsize_mirror = XWPCP_FRMHEAD_SIZE_MIRROR(stream.head.headsize);
        if (headsize_mirror != xwbop_rbit(xwu8_t, headsize)) {
                rc = -EAGAIN;
                goto err_head_ifrx;
        }
        total = headsize;
        rest = (xwssz_t)total - 1;
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
        headchk = xwpcp_chk_head((xwu8_t *)&stream.head, headsize);
        if (!headchk) {
                rc = -EAGAIN;
                goto err_head_ifrx;
        }
        xwpcp_decode_sdusize(stream.head.ecsdusz, &sdusize);

        /* 申请接收帧槽 */
        ecsize = XWPCP_ECSIZE(&stream.head);
        need = sizeof(union xwpcp_slot) + ecsize + sdusize +
               XWPCP_CRC32_SIZE + XWPCP_EOF_SIZE;
        neednum = DIV_ROUND_UP(need, XWPCP_MEMBLK_SIZE);
        odr = xwbop_fls(xwsz_t, neednum);
        if ((odr < 0) || ((XWPCP_MEMBLK_SIZE << odr) < need)) {
                odr++;
        }
        rc = xwmm_bma_alloc(xwpcp->mempool, (xwsq_t)odr, &mem);
        if (__xwcc_unlikely(rc < 0)) {
                xwpcp_tx_sdu_ack(xwpcp,
                                 stream.head.port, stream.head.id,
                                 XWPCP_ACK_NOMEM);
                goto err_bma_alloc;
        }
        slot = mem;
        xwlib_bclst_init_node(&slot->rx.node);
        slot->rx.frmsize = sizeof(struct xwpcp_frm) + ecsize + sdusize +
                           XWPCP_CRC32_SIZE + XWPCP_EOF_SIZE;
        memset(slot->rx.frm.sof, XWPCP_SOF, XWPCP_SOF_SIZE);
        memcpy(&slot->rx.frm.head, &stream.data,
               sizeof(struct xwpcp_frmhead) + ecsize);
        sdupos = &slot->rx.frm.head.ecsdusz[ecsize];

        /* 接收body */
        total = sdusize + XWPCP_CRC32_SIZE;
        rest = (xwssz_t)total;
        do {
                rxsize = (xwsz_t)rest;
                rc = xwpcp->hwifops->rx(xwpcp,
                                        &sdupos[total - rxsize],
                                        &rxsize);
                if (__xwcc_unlikely(rc < 0)) {
                        goto err_body_ifrx;
                }
                rest -= (xwssz_t)rxsize;
        } while (rest > 0);

        /* 接收帧尾定界符 */
        delimcnt = 0;
        do {
                rxsize = 1;
                rc = xwpcp->hwifops->rx(xwpcp, &delim, &rxsize);
                if (__xwcc_unlikely(rc < 0)) {
                        goto err_eof_ifrx;
                }
                if (XWPCP_EOF == delim) {
                        delimcnt++;
                        if (delimcnt >= XWPCP_EOF_SIZE) {
                                break;
                        }
                } else {
                        break;
                }
        } while (true);
        if (delimcnt < XWPCP_EOF_SIZE) {
                rc = -EAGAIN;
                goto err_eof_ifrx;
        }
        memset(&sdupos[sdusize + XWPCP_CRC32_SIZE], XWPCP_EOF, XWPCP_EOF_SIZE);

        *slotbuf = slot;
        return XWOK;

err_eof_ifrx:
err_body_ifrx:
        xwmm_bma_free(xwpcp->mempool, mem);
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
