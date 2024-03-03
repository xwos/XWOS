/**
 * @file
 * @brief W25Qxx编程器：硬件接口抽象层
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
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

#include <xwos/standard.h>
#include <xwos/lib/xwbop.h>
#include <xwam/application/w25qpt/w25qpt.h>
#include <xwam/application/w25qpt/protocol.h>
#include <xwam/application/w25qpt/hwifal.h>

/**
 * @brief 打开硬件接口
 * @param[in] w25qpt: W25QPT对象的指针
 * @return 错误码
 */
xwer_t w25qpt_hwifal_open(struct w25qpt * w25qpt, void * hwifcb)
{
        xwer_t rc;

        w25qpt->hwifcb = hwifcb;
        if ((w25qpt->hwifops) && (w25qpt->hwifops->open)) {
                rc = w25qpt->hwifops->open(w25qpt);
        } else {
                rc = XWOK;
        }
        if (XWOK == rc) {
                w25qpt->hwifst = W25QPT_HWIFST_OPENED;
        }
        return rc;
}

/**
 * @brief 关闭硬件接口
 * @param[in] w25qpt: W25QPT对象的指针
 * @return 错误码
 */
xwer_t w25qpt_hwifal_close(struct w25qpt * w25qpt)
{
        xwer_t rc;

        if ((w25qpt->hwifops) && (w25qpt->hwifops->close)) {
                rc = w25qpt->hwifops->close(w25qpt);
        } else {
                rc = XWOK;
        }
        if (XWOK == rc) {
                w25qpt->hwifst = W25QPT_HWIFST_CLOSED;
        }
        w25qpt->hwifcb = NULL;
        return rc;
}

/**
 * @brief 通过硬件接口发送消息
 * @param[in] w25qpt: W25QPT对象的指针
 * @param[in] msg: 指向发送帧缓冲区的指针
 * @return 错误码
 */
xwer_t w25qpt_hwifal_tx(struct w25qpt * w25qpt, struct w25qpt_msg * msg)
{
        xwsz_t txsize;

        txsize = W25QPT_SOF_SIZE + W25QPT_HEAD_SIZE + W25QPT_ADDRESS_SIZE +
                        w25qpt_get_sdu_size(msg) + W25QPT_CHKSUM_SIZE +
                        W25QPT_EOF_SIZE;
        return w25qpt->hwifops->tx(w25qpt, (const xwu8_t *)msg, &txsize, XWTM_MAX);
}

void w25qpt_fill_msg_parity(struct w25qpt_msg * msg)
{
        xwu8_t parity;

        parity = (((xwu8_t)(msg->sdusize[1] >> 0U) & 3U) ^
                  ((xwu8_t)(msg->sdusize[1] >> 2U) & 3U) ^
                  ((xwu8_t)(msg->sdusize[1] >> 4U) & 3U) ^
                  ((xwu8_t)(msg->sdusize[1] >> 6U) & 3U) ^
                  ((xwu8_t)(msg->sdusize[0] >> 0U) & 3U) ^
                  ((xwu8_t)(msg->sdusize[0] >> 2U) & 3U) ^
                  ((xwu8_t)(msg->sdusize[0] >> 4U) & 3U));
        parity <<= 6U;
        msg->sdusize[0] &= W25QPT_SDU_SIZE_HBYTE_MSK;
        msg->sdusize[0] |= parity;
}

xwer_t w25qpt_chk_size_parity(xwu8_t * size)
{
        xwer_t rc;
        xwu8_t parity;

        parity = (((xwu8_t)(size[1] >> 0U) & 3U) ^
                  ((xwu8_t)(size[1] >> 2U) & 3U) ^
                  ((xwu8_t)(size[1] >> 4U) & 3U) ^
                  ((xwu8_t)(size[1] >> 6U) & 3U) ^
                  ((xwu8_t)(size[0] >> 0U) & 3U) ^
                  ((xwu8_t)(size[0] >> 2U) & 3U) ^
                  ((xwu8_t)(size[0] >> 4U) & 3U) ^
                  ((xwu8_t)(size[0] >> 6U) & 3U));
        if (parity) {
                rc = -EBADMSG;
        } else {
                rc = XWOK;
        }
        return rc;
}

xwsz_t w25qpt_get_sdu_size(struct w25qpt_msg * msg)
{
        xwsz_t size;

        size = ((xwsz_t)(msg->sdusize[0] & W25QPT_SDU_SIZE_HBYTE_MSK)) << 8U;
        size |= (xwsz_t)msg->sdusize[1];
        return size;
}

/**
 * @brief 通过硬件接口接收消息
 * @param[in] w25qpt: W25QPT对象的指针
 * @param[out] msgbuf: 指向缓冲区的指针，通过此缓冲区返回struct w25qpt_msg
 * @return 错误码
 */
xwer_t w25qpt_hwifal_rx(struct w25qpt * w25qpt, struct w25qpt_msg * msgbuf)
{
        union {
                xwu8_t sof[W25QPT_SOF_SIZE];
                xwu8_t eof[W25QPT_EOF_SIZE];
        } delimiter;
        union {
                xwu8_t data[W25QPT_HEAD_SIZE];
                struct __xwcc_packed {
                        xwu8_t rpc;
                        xwu8_t oprc;
                        xwu8_t sdusize[2];
                } head;
        } stream;
        xwsz_t sofcnt;
        xwu8_t * eofpos;
        xwsz_t rxsize;
        xwsz_t total;
        xwssz_t rest;
        xwsz_t sdusize;
        xwsq_t i;
        xwer_t rc;

        /* 接收帧首定界符 */
        sofcnt = 0;
        do {
                rxsize = 1;
                rc = w25qpt->hwifops->rx(w25qpt, delimiter.sof, &rxsize, XWTM_MAX);
                if (rc < 0) {
                        goto err_sof_ifrx;
                }
                if (W25QPT_HWIFAL_SOF == delimiter.sof[0]) {
                        sofcnt++;
                } else {
                        break;
                }
        } while (true);
        if (sofcnt < W25QPT_SOF_SIZE) {
                rc = -EAGAIN;
                goto err_sof_ifrx;
        }
        for (i = 0; i < W25QPT_SOF_SIZE; i++) {
                msgbuf->sof[i] = W25QPT_HWIFAL_SOF;
        }

        /* 接收Head */
        total = W25QPT_HEAD_SIZE;
        rest = (xwssz_t)total - 1;
        stream.data[0] = delimiter.sof[0];
        do {
                rxsize = (xwsz_t)rest;
                rc = w25qpt->hwifops->rx(w25qpt,
                                         &stream.data[total - rxsize],
                                         &rxsize, XWTM_MAX);
                if (rc < 0) {
                        goto err_head_ifrx;
                }
                rest -= (xwssz_t)rxsize;
        } while (rest > 0);
        rc = w25qpt_chk_size_parity(stream.head.sdusize);
        if (rc < 0) {
                rc = -EAGAIN;
                goto err_head_ifrx;
        }
        msgbuf->rpc = stream.head.rpc;
        msgbuf->oprc = stream.head.oprc;
        msgbuf->sdusize[0] = stream.head.sdusize[0];
        msgbuf->sdusize[1] = stream.head.sdusize[1];
        sdusize = w25qpt_get_sdu_size(msgbuf);
        if (sdusize > W25QPT_SDU_MAXSIZE) {
                rc = -EAGAIN;
                goto err_head_ifrx;
        }

        /* 接收地址、数据、校验和 */
        total = sizeof(msgbuf->address) + sdusize + sizeof(msgbuf->crc32);
        rest = (xwssz_t)total;
        do {
                rxsize = (xwsz_t)rest;
                rc = w25qpt->hwifops->rx(w25qpt,
                                         &msgbuf->address[total - rxsize],
                                         &rxsize, XWTM_MAX);
                if (rc < 0) {
                        goto err_body_ifrx;
                }
                rest -= (xwssz_t)rxsize;
        } while (rest > 0);

        /* 接收帧尾定界符 */
        total = W25QPT_EOF_SIZE;
        rest = (xwssz_t)total;
        do {
                rxsize = (xwsz_t)rest;
                rc = w25qpt->hwifops->rx(w25qpt, delimiter.eof, &rxsize, XWTM_MAX);
                if (rc < 0) {
                        goto err_eof_ifrx;
                }
                rest -= (xwssz_t)rxsize;
        } while (rest > 0);
        eofpos = &msgbuf->sdu[sdusize + W25QPT_CHKSUM_SIZE];
        for (i = 0; i < W25QPT_EOF_SIZE; i++) {
                if (W25QPT_HWIFAL_EOF != delimiter.eof[i]) {
                        rc = -EAGAIN;
                        goto err_eof_ifrx;
                }
                eofpos[i] = W25QPT_HWIFAL_EOF;
        }
        return XWOK;

err_eof_ifrx:
err_body_ifrx:
err_head_ifrx:
err_sof_ifrx:
        return rc;
}
