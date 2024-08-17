/**
 * @file
 * @brief SOC间点对点通讯协议：硬件接口抽象层
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#include <xwos/standard.h>
#include <string.h>
#include <xwos/lib/xwaop.h>
#include <xwos/lib/xwlog.h>
#include <xwmd/isc/xwssc/hwifal.h>
#include <xwmd/isc/xwssc/protocol.h>

/**
 * @ingroup xwmd_isc_xwssc_hwifal
 * @{
 */

/**
 * @brief 空白帧
 */
static __xwmd_rodata
const xwu8_t xwssc_frm_blank[8] = {
        (xwu8_t)XWSSC_EOF,
        (xwu8_t)XWSSC_EOF,
        (xwu8_t)XWSSC_EOF,
        (xwu8_t)XWSSC_EOF,
        (xwu8_t)XWSSC_EOF,
        (xwu8_t)XWSSC_EOF,
        (xwu8_t)XWSSC_EOF,
        (xwu8_t)XWSSC_EOF,
};

/**
 * @brief 打开硬件接口
 * @param[in] xwssc: XWSSC对象的指针
 * @param[in] hwifcb: 硬件接口控制块指针
 * @return 错误码
 */
__xwmd_code
xwer_t xwssc_hwifal_open(struct xwssc * xwssc, void * hwifcb)
{
        xwer_t rc;

        xwssc->hwifcb = hwifcb;
        if ((NULL != xwssc->hwifops) && (NULL != xwssc->hwifops->open)) {
                rc = xwssc->hwifops->open(xwssc);
        } else {
                rc = XWOK;
        }
        if (rc < 0) {
                goto err_hwifops_open;
        }

        xwaop_write(xwsq_t, &xwssc->hwifst, XWSSC_HWIFST_RX, NULL);
        xwssclogf(xwssc, I, "[HWIFAL] Open HWIF ... [OK]\r\n");
        return rc;

err_hwifops_open:
        xwssc->hwifcb = NULL;
        xwssclogf(xwssc, I, "[HWIFAL] Failed to open HWIF ... <rc:%d>\r\n", rc);
        return rc;
}

/**
 * @brief 关闭硬件接口
 * @param[in] xwssc: XWSSC对象的指针
 * @return 错误码
 */
__xwmd_code
xwer_t xwssc_hwifal_close(struct xwssc * xwssc)
{
        xwer_t rc;

        rc = xwaop_t1ma_then_c0m(xwsq_t, &xwssc->hwifst, XWSSC_HWIFST_RX,
                                 NULL, NULL);
        if (rc < 0) {
                goto err_xwssc_clear_hwifst;
        }

        if ((NULL != xwssc->hwifops) && (NULL != xwssc->hwifops->close)) {
                rc = xwssc->hwifops->close(xwssc);
        } else {
                rc = XWOK;
        }
        if (rc < 0) {
                goto err_hwifops_close;
        }
        xwssc->hwifcb = NULL;
        xwssclogf(xwssc, I, "[HWIFAL] close HWIF ... [OK]\r\n");
        return XWOK;

err_hwifops_close:
        xwaop_s1m(xwsq_t, &xwssc->hwifst, XWSSC_HWIFST_RX, NULL, NULL);
err_xwssc_clear_hwifst:
        xwssclogf(xwssc, I, "[HWIFAL] Open HWIF ... <rc:%d>\r\n", rc);
        return rc;
}

/**
 * @brief 通过硬件接口发送数据
 * @param[in] xwssc: XWSSC对象的指针
 * @param[in] stream: 数据
 * @param[in] size: 数据长度
 * @return 错误码
 */
__xwmd_code
xwer_t xwssc_hwifal_tx(struct xwssc * xwssc, const xwu8_t * stream, xwsz_t size)
{
        xwer_t rc;

        rc = xwssc->hwifops->tx(xwssc, stream, size);
        return rc;
}

/**
 * @brief 通过硬件接口接收消息帧头
 * @param[in] xwssc: XWSSC对象的指针
 * @param[out] frm: 指向缓冲区的指针，此缓冲区用于接收消息帧
 * @return 错误码
 * @retval XWOK: 接收成功
 * @retval -EAGAIN: 接收错误
 */
__xwmd_code
xwer_t xwssc_hwifal_rx_head(struct xwssc * xwssc, struct xwssc_frm * frm)
{
        xwer_t rc;
        xwu8_t * stream;
        xwu8_t delim;
        xwsz_t delimcnt;
        xwsz_t rxsize;
        xwsz_t total;
        xwssz_t rest;
        xwu8_t headsize;
        xwu8_t headsize_mirror;
        bool headchk;

        /* 接收帧首定界符 */
        delimcnt = 0;
        do { // cppcheck-suppress [misra-c2012-15.4]
                rxsize = 1;
                rc = xwssc->hwifops->rx(xwssc, &delim, &rxsize);
                if (rc < 0) {
                        goto err_sof_ifrx;
                }
                if (XWSSC_SOF == delim) {
                        delimcnt++;
                } else {
                        break;
                }
        } while (true);
        if (delimcnt < XWSSC_SOF_SIZE) {
                rc = -EAGAIN;
                goto err_sof_ifrx;
        }
        // cppcheck-suppress [misra-c2012-17.7]
        /* memset(frm->sof, XWSSC_SOF, XWSSC_SOF_SIZE); */

        /* 接收Head */
        stream = (xwu8_t *)&frm->head;
        frm->head.headsize = delim;
        headsize = XWSSC_FRMHEAD_SIZE(frm->head.headsize);
        headsize_mirror = XWSSC_FRMHEAD_SIZE_MIRROR(frm->head.headsize);
        if (headsize_mirror != xwbop_rbit(xwu8_t, headsize)) {
                rc = -EAGAIN;
                goto err_head_ifrx;
        }
        total = headsize;
        rest = (xwssz_t)total - 1;
        do {
                rxsize = (xwsz_t)rest;
                rc = xwssc->hwifops->rx(xwssc,
                                        &stream[total - rxsize],
                                        &rxsize);
                if (rc < 0) {
                        goto err_head_ifrx;
                }
                rest -= (xwssz_t)rxsize;
        } while (rest > 0);
        headchk = xwssc_chk_head((xwu8_t *)&frm->head, headsize);
        if (!headchk) {
                rc = -EAGAIN;
                goto err_head_ifrx;
        }
        return XWOK;

err_head_ifrx:
err_sof_ifrx:
        return rc;
}

/**
 * @brief 通过硬件接口接收消息体
 * @param[in] xwssc: XWSSC对象的指针
 * @param[out] frm: 指向缓冲区的指针，此缓冲区用于接收消息帧
 * @param[in] sdusize: 消息体的大小
 * @return 错误码
 * @retval XWOK: 接收成功
 * @retval -EAGAIN: 接收错误
 */
__xwmd_code
xwer_t xwssc_hwifal_rx_body(struct xwssc * xwssc,
                            struct xwssc_frm * frm, xwsz_t sdusize)
{
        xwer_t rc;
        xwsz_t rxsize;
        xwsz_t total;
        xwssz_t rest;
        xwu8_t * sdupos;
        xwu8_t delim;
        xwsz_t delimcnt;

        sdupos = &frm->head.ecsdusz[XWSSC_ECSIZE(&frm->head)];
        total = sdusize + XWSSC_CRC32_SIZE;
        rest = (xwssz_t)total;
        do {
                rxsize = (xwsz_t)rest;
                rc = xwssc->hwifops->rx(xwssc,
                                        &sdupos[total - rxsize],
                                        &rxsize);
                if (rc < 0) {
                        goto err_body_ifrx;
                }
                rest -= (xwssz_t)rxsize;
        } while (rest > 0);

        /* 接收帧尾定界符 */
        delimcnt = 0;
        do { // cppcheck-suppress [misra-c2012-15.4]
                rxsize = 1;
                rc = xwssc->hwifops->rx(xwssc, &delim, &rxsize);
                if (rc < 0) {
                        goto err_eof_ifrx;
                }
                if (XWSSC_EOF == delim) {
                        delimcnt++;
                        if (delimcnt >= XWSSC_EOF_SIZE) {
                                break;
                        }
                } else {
                        break;
                }
        } while (true);
        if (delimcnt < XWSSC_EOF_SIZE) {
                rc = -EAGAIN;
                goto err_eof_ifrx;
        }
        // cppcheck-suppress [misra-c2012-17.7]
        /* memset(&sdupos[sdusize + XWSSC_CRC32_SIZE], XWSSC_EOF, XWSSC_EOF_SIZE); */
        return XWOK;

err_eof_ifrx:
err_body_ifrx:
        return rc;
}

/**
 * @brief 通知来自协议层的事件到硬件接口层
 * @param[in] xwssc: XWSSC对象的指针
 * @param[in] evt: 事件
 */
__xwmd_code
void xwssc_hwifal_notify(struct xwssc * xwssc, xwsq_t evt)
{
        if (XWSSC_HWIFNTF_NETUNREACH == (enum xwssc_hwifal_notification_em)evt) {
                xwssc->hwifops->tx(xwssc, xwssc_frm_blank, sizeof(xwssc_frm_blank));
        }
        if (NULL != xwssc->hwifops->notify) {
                xwssc->hwifops->notify(xwssc, evt);
        }
}

/**
 * @} xwmd_isc_xwssc_hwifal
 */
