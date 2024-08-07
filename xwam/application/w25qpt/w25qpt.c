/**
 * @file
 * @brief W25Qxx编程器
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
#include <string.h>
#include <xwos/lib/xwbop.h>
#include <xwos/lib/crc32.h>
#include <xwos/lib/xwlog.h>
#include <xwos/mm/common.h>
#include <xwos/osal/thd.h>
#include <xwcd/peripheral/spi/flash/w25qxx/device.h>
#include <xwcd/peripheral/spi/flash/w25qxx/driver.h>
#include <xwam/application/w25qpt/w25qpt.h>
#include <xwam/application/w25qpt/hwifal.h>
#include <xwam/application/w25qpt/protocol.h>
#include <xwam/application/w25qpt/mif.h>

#define W25QPT_THD_PRIORITY XWOS_SKD_PRIORITY_DROP(XWOS_SKD_PRIORITY_RT_MAX, 1)

xwer_t w25qpt_thd_func(void * arg);

/**
 * @brief 启动W25Qxx编程器模块
 */
xwer_t w25qpt_start(struct w25qpt * w25qpt,
                     const char * name,
                     struct xwds_w25qxx * flash,
                     const struct w25qpt_hwifal_operations * hwifops,
                     void * hwifcb)
{
        xwer_t rc;
        struct xwos_thd_attr attr;

        XWOS_VALIDATE((w25qpt), "nullptr", -EFAULT);
        XWOS_VALIDATE((flash), "flash", -EFAULT);
        XWOS_VALIDATE((hwifops), "nullptr", -EFAULT);
        XWOS_VALIDATE((hwifops->tx), "nullptr", -EFAULT);
        XWOS_VALIDATE((hwifops->rx), "nullptr", -EFAULT);

        w25qpt->flash = flash;
        w25qpt->hwifst = W25QPT_HWIFST_CLOSED;
        w25qpt->name = name;
        w25qpt->hwifops = hwifops;

        rc = xwds_w25qxx_reset(flash, XWTM_MAX);
        if (rc < 0) {
                goto err_flash_reset;
        }

        rc = xwds_w25qxx_init_parameter(flash, XWTM_MAX);
        if (rc < 0) {
                goto err_flash_init_parameter;
        }

        /* 打开端口 */
        rc = w25qpt_hwifal_open(w25qpt, hwifcb);
        if (rc < 0) {
                goto err_hwifal_open;
        }

        xwos_thd_attr_init(&attr);
        attr.name = "w25qpt.thd";
        attr.stack = (xwstk_t *)w25qpt->thd_stack;
        attr.stack_size = sizeof(w25qpt->thd_stack);
        attr.priority = W25QPT_THD_PRIORITY;
        attr.detached = false;
        attr.privileged = true;
        rc = xwos_thd_init(&w25qpt->thdobj, &w25qpt->thd, &attr,
                           w25qpt_thd_func, w25qpt);
        if (rc < 0) {
                goto err_thd_init;
        }
        return XWOK;

err_thd_init:
        w25qpt_hwifal_close(w25qpt);
err_hwifal_open:
err_flash_init_parameter:
err_flash_reset:
        return rc;
}

/**
 * @brief 停止W25Qxx编程器模块
 */
xwer_t w25qpt_stop(struct w25qpt * w25qpt)
{
        xwer_t rc, childrc;

        XWOS_VALIDATE((w25qpt), "nullptr", -EFAULT);

        rc = xwos_thd_stop(w25qpt->thd, &childrc);
        w25qpt_hwifal_close(w25qpt);
        return rc;
}

/**
 * @brief 远程端读取Flash的UID
 * @param[in] w25qpt: W25QPT对象的指针
 * @param[in] msg: 消息
 * @return 错误码
 * @note
 * + 协议
 *   - 输入
 * ```
 * -----------------------------------------------------------------------------
 * AA AA AA AA | 00  | 00   | 00 00    | 00 00 00 00 |     | 65 22 DF 69 | 55 55
 * SOF         | rpc | oprc | sdusize  | address     | sdu | CRC32       | EOF
 * ```
 *
 *   - 应答（输出）
 * ```
 * --------------------------------------------------------------------------------
 * AA AA AA AA | 80      | 00   | 80 08   | 00 00 00 00 | UID[8] | CRC32[4] | 55 55
 * SOF         | rpc+ack | oprc | sdusize | address     | sdu    | CRC32    | EOF
 * ```
 */
static
xwer_t w25qpt_rpc_read_uid(struct w25qpt * w25qpt, struct w25qpt_msg * msg)
{
        struct xwds_w25qxx * w25qxx;
        xwu8_t * crc32pos;
        xwu32_t crc32;
        xwsz_t sdusize;
        xwsz_t infolen;
        xwu8_t * eofpos;
        xwsq_t i;
        xwer_t rc;

        w25qxx = w25qpt->flash;
        sdusize = sizeof(w25qxx->parameter.uid);
        for (i = 0; i < W25QPT_SOF_SIZE; i++) {
                msg->sof[i] = W25QPT_HWIFAL_SOF;
        }
        msg->rpc |= W25QPT_RPC_ACK;
        msg->oprc = W25QPT_ERR_NONE;
        msg->sdusize[0] = (xwu8_t)((sdusize >> 8U) & 0xFF);
        msg->sdusize[1] = (xwu8_t)((sdusize >> 0U) & 0xFF);
        w25qpt_fill_msg_parity(msg);
        msg->address[0] = 0;
        msg->address[1] = 0;
        msg->address[2] = 0;
        msg->address[3] = 0;
        msg->sdu[0] = (xwu8_t)((w25qxx->parameter.uid >> 56U) & 0xFF);
        msg->sdu[1] = (xwu8_t)((w25qxx->parameter.uid >> 48U) & 0xFF);
        msg->sdu[2] = (xwu8_t)((w25qxx->parameter.uid >> 40U) & 0xFF);
        msg->sdu[3] = (xwu8_t)((w25qxx->parameter.uid >> 32U) & 0xFF);
        msg->sdu[4] = (xwu8_t)((w25qxx->parameter.uid >> 24U) & 0xFF);
        msg->sdu[5] = (xwu8_t)((w25qxx->parameter.uid >> 16U) & 0xFF);
        msg->sdu[6] = (xwu8_t)((w25qxx->parameter.uid >> 8U) & 0xFF);
        msg->sdu[7] = (xwu8_t)((w25qxx->parameter.uid >> 0U) & 0xFF);
        infolen = sizeof(msg->rpc) + sizeof(msg->oprc) + sizeof(msg->sdusize) +
                  sizeof(msg->address) + sdusize;
        crc32 = xwlib_crc32_calms((xwu8_t *)&msg->rpc, &infolen);
        crc32pos = &msg->sdu[sdusize];
        crc32pos[0] = (xwu8_t)((crc32 >> 24U) & 0xFF);
        crc32pos[1] = (xwu8_t)((crc32 >> 16U) & 0xFF);
        crc32pos[2] = (xwu8_t)((crc32 >> 8U) & 0xFF);
        crc32pos[3] = (xwu8_t)((crc32 >> 0U) & 0xFF);
        eofpos = &msg->sdu[sdusize + W25QPT_CHKSUM_SIZE];
        for (i = 0; i < W25QPT_EOF_SIZE; i++) {
                eofpos[i] = W25QPT_HWIFAL_EOF;
        }

        rc = w25qpt_hwifal_tx(w25qpt, msg);
        return rc;
}

/**
 * @brief 远程端读取Flash的MID
 * @param[in] w25qpt: W25QPT对象的指针
 * @param[in] msg: 消息
 * @return 错误码
 * @note
 * + 协议
 *   - 输入
 * ```
 * ----------------------------------------------------------------------------
 * AA AA AA AA | 01  | 00   | 00 00   | 00 00 00 00 |     | A9 88 DF F7 | 55 55
 * SOF         | rpc | oprc | sdusize | address     | sdu | CRC32       | EOF
 * ```
 *
 *   - 应答（输出）
 * ```
 * --------------------------------------------------------------------------------
 * AA AA AA AA | 81      | 00   | 80 02   | 00 00 00 00 | MID[2] | CRC32[4] | 55 55
 * SOF         | rpc+ack | oprc | sdusize | address     | sdu    | CRC32    | EOF
 * ```
 */
static
xwer_t w25qpt_rpc_read_mid(struct w25qpt * w25qpt, struct w25qpt_msg * msg)
{
        struct xwds_w25qxx * w25qxx;
        xwu8_t * crc32pos;
        xwu32_t crc32;
        xwsz_t sdusize;
        xwsz_t infolen;
        xwu8_t * eofpos;
        xwsq_t i;
        xwer_t rc;

        w25qxx = w25qpt->flash;
        sdusize = sizeof(w25qxx->parameter.mid);
        for (i = 0; i < W25QPT_SOF_SIZE; i++) {
                msg->sof[i] = W25QPT_HWIFAL_SOF;
        }
        msg->rpc |= W25QPT_RPC_ACK;
        msg->oprc = W25QPT_ERR_NONE;
        msg->sdusize[0] = (xwu8_t)((sdusize >> 8U) & 0xFF);
        msg->sdusize[1] = (xwu8_t)((sdusize >> 0U) & 0xFF);
        w25qpt_fill_msg_parity(msg);
        msg->address[0] = 0;
        msg->address[1] = 0;
        msg->address[2] = 0;
        msg->address[3] = 0;
        msg->sdu[0] = (xwu8_t)((w25qxx->parameter.mid >> 8U) & 0xFF);
        msg->sdu[1] = (xwu8_t)((w25qxx->parameter.mid >> 0U) & 0xFF);
        infolen = sizeof(msg->rpc) + sizeof(msg->oprc) + sizeof(msg->sdusize) +
                  sizeof(msg->address) + sdusize;
        crc32 = xwlib_crc32_calms((xwu8_t *)&msg->rpc, &infolen);
        crc32pos = &msg->sdu[sdusize];
        crc32pos[0] = (xwu8_t)((crc32 >> 24U) & 0xFF);
        crc32pos[1] = (xwu8_t)((crc32 >> 16U) & 0xFF);
        crc32pos[2] = (xwu8_t)((crc32 >> 8U) & 0xFF);
        crc32pos[3] = (xwu8_t)((crc32 >> 0U) & 0xFF);
        eofpos = &msg->sdu[sdusize + W25QPT_CHKSUM_SIZE];
        for (i = 0; i < W25QPT_EOF_SIZE; i++) {
                eofpos[i] = W25QPT_HWIFAL_EOF;
        }

        rc = w25qpt_hwifal_tx(w25qpt, msg);
        return rc;
}

/**
 * @brief 远程端读取Flash的JID
 * @param[in] w25qpt: W25QPT对象的指针
 * @param[in] msg: 消息
 * @return 错误码
 * @note
 * + 协议
 *   - 输入
 * ```
 * ----------------------------------------------------------------------------
 * AA AA AA AA | 02  | 00   | 00 00   | 00 00 00 00 |     | 27 07 D8 14 | 55 55
 * SOF         | rpc | oprc | sdusize | address     | sdu | CRC32       | EOF
 * ```
 *
 *   - 应答（输出）
 * ```
 * --------------------------------------------------------------------------------
 * AA AA AA AA | 82      | 00   | 40 04   | 00 00 00 00 | JID[4] | CRC32[4] | 55 55
 * SOF         | rpc+ack | oprc | sdusize | address     | sdu    | CRC32    | EOF
 * ```
 */
static
xwer_t w25qpt_rpc_read_jid(struct w25qpt * w25qpt, struct w25qpt_msg * msg)
{
        struct xwds_w25qxx * w25qxx;
        xwu8_t * crc32pos;
        xwu32_t crc32;
        xwsz_t sdusize;
        xwsz_t infolen;
        xwu8_t * eofpos;
        xwsq_t i;
        xwer_t rc;

        w25qxx = w25qpt->flash;
        sdusize = sizeof(w25qxx->parameter.jid);
        for (i = 0; i < W25QPT_SOF_SIZE; i++) {
                msg->sof[i] = W25QPT_HWIFAL_SOF;
        }
        msg->rpc |= W25QPT_RPC_ACK;
        msg->oprc = W25QPT_ERR_NONE;
        msg->sdusize[0] = (xwu8_t)((sdusize >> 8U) & 0xFF);
        msg->sdusize[1] = (xwu8_t)((sdusize >> 0U) & 0xFF);
        w25qpt_fill_msg_parity(msg);
        msg->address[0] = 0;
        msg->address[1] = 0;
        msg->address[2] = 0;
        msg->address[3] = 0;
        msg->sdu[0] = (xwu8_t)((w25qxx->parameter.jid >> 24U) & 0xFF);
        msg->sdu[1] = (xwu8_t)((w25qxx->parameter.jid >> 16U) & 0xFF);
        msg->sdu[2] = (xwu8_t)((w25qxx->parameter.jid >> 8U) & 0xFF);
        msg->sdu[3] = (xwu8_t)((w25qxx->parameter.jid >> 0U) & 0xFF);
        infolen = sizeof(msg->rpc) + sizeof(msg->oprc) + sizeof(msg->sdusize) +
                  sizeof(msg->address) + sdusize;
        crc32 = xwlib_crc32_calms((xwu8_t *)&msg->rpc, &infolen);
        crc32pos = &msg->sdu[sdusize];
        crc32pos[0] = (xwu8_t)((crc32 >> 24U) & 0xFF);
        crc32pos[1] = (xwu8_t)((crc32 >> 16U) & 0xFF);
        crc32pos[2] = (xwu8_t)((crc32 >> 8U) & 0xFF);
        crc32pos[3] = (xwu8_t)((crc32 >> 0U) & 0xFF);
        eofpos = &msg->sdu[sdusize + W25QPT_CHKSUM_SIZE];
        for (i = 0; i < W25QPT_EOF_SIZE; i++) {
                eofpos[i] = W25QPT_HWIFAL_EOF;
        }

        rc = w25qpt_hwifal_tx(w25qpt, msg);
        return rc;
}

/**
 * @brief 远程端擦除Flash
 * @param[in] w25qpt: W25QPT对象的指针
 * @param[in] msg: 消息
 * @return 错误码
 * @note
 * + 协议
 *   - 输入
 * ```
 * ----------------------------------------------------------------------------
 * AA AA AA AA | 03  | 00   | 00 00   | 00 00 00 00 |     | EB AD D8 8A | 55 55
 * SOF         | rpc | oprc | sdusize | address     | sdu | CRC32       | EOF
 * ```
 *
 *   - 应答（输出）
 * ```
 * -----------------------------------------------------------------------------
 * AA AA AA AA | 83      | oprc | 00 00   | 00 00 00 00 |     | CRC32[4] | 55 55
 * SOF         | rpc+ack | oprc | sdusize | address     | sdu | CRC32    | EOF
 * ```
 */
static
xwer_t w25qpt_rpc_erase_chip(struct w25qpt * w25qpt, struct w25qpt_msg * msg)
{
        struct xwds_w25qxx * w25qxx;
        xwsq_t i;
        xwer_t rc;
        xwu8_t oprc;
        xwu8_t * crc32pos;
        xwu32_t crc32;
        xwsz_t sdusize;
        xwsz_t infolen;
        xwu8_t * eofpos;

        w25qxx = w25qpt->flash;
        sdusize = 0;
        rc = xwds_w25qxx_erase_chip(w25qxx, xwtm_ft(XWTM_S(300)));
        switch (rc) {
        case XWOK:
                oprc = W25QPT_ERR_NONE;
                break;
        case -EINTR:
                oprc = W25QPT_ERR_INTR;
                break;
        case -ETIMEDOUT:
                oprc = W25QPT_ERR_TIMEOUT;
                break;
        default:
                oprc = W25QPT_ERR_IO;
                break;
        }

        for (i = 0; i < W25QPT_SOF_SIZE; i++) {
                msg->sof[i] = W25QPT_HWIFAL_SOF;
        }
        msg->rpc |= W25QPT_RPC_ACK;
        msg->oprc = oprc;
        msg->sdusize[0] = (xwu8_t)((sdusize >> 8U) & 0xFF);
        msg->sdusize[1] = (xwu8_t)((sdusize >> 0U) & 0xFF);
        w25qpt_fill_msg_parity(msg);
        msg->address[0] = 0;
        msg->address[1] = 0;
        msg->address[2] = 0;
        msg->address[3] = 0;
        infolen = sizeof(msg->rpc) + sizeof(msg->oprc) + sizeof(msg->sdusize) +
                  sizeof(msg->address) + sdusize;
        crc32 = xwlib_crc32_calms((xwu8_t *)&msg->rpc, &infolen);
        crc32pos = &msg->sdu[sdusize];
        crc32pos[0] = (xwu8_t)((crc32 >> 24U) & 0xFF);
        crc32pos[1] = (xwu8_t)((crc32 >> 16U) & 0xFF);
        crc32pos[2] = (xwu8_t)((crc32 >> 8U) & 0xFF);
        crc32pos[3] = (xwu8_t)((crc32 >> 0U) & 0xFF);
        eofpos = &msg->sdu[sdusize + W25QPT_CHKSUM_SIZE];
        for (i = 0; i < W25QPT_EOF_SIZE; i++) {
                eofpos[i] = W25QPT_HWIFAL_EOF;
        }

        rc = w25qpt_hwifal_tx(w25qpt, msg);
        return rc;
}

/**
 * @brief 远程端擦除Flash 4K的扇区
 * @param[in] w25qpt: W25QPT对象的指针
 * @param[in] msg: 消息
 * @return 错误码
 * @note
 * + 协议
 *   - 输入
 * ```
 * ------------------------------------------------------------------------
 * AA AA AA AA | 04  | 00   | 00 00   | address[4] |     | CRC32[4] | 55 55
 * SOF         | rpc | oprc | sdusize | address    | sdu | CRC32    | EOF
 * ```
 *
 *   - 应答（输出）
 * ```
 * ----------------------------------------------------------------------------
 * AA AA AA AA | 84      | oprc | 00 00   | address[4] |     | CRC32[4] | 55 55
 * SOF         | rpc+ack | oprc | sdusize | address    | sdu | CRC32    | EOF
 * ```
 */
static
xwer_t w25qpt_rpc_erase_sector(struct w25qpt * w25qpt, struct w25qpt_msg * msg)
{
        struct xwds_w25qxx * w25qxx;
        xwsq_t i;
        xwer_t rc;
        xwu32_t address;
        xwu8_t oprc;
        xwu8_t * crc32pos;
        xwu32_t crc32;
        xwsz_t sdusize;
        xwsz_t infolen;
        xwu8_t * eofpos;

        w25qxx = w25qpt->flash;
        address = ((xwu32_t)msg->address[0] << 24U);
        address |= ((xwu32_t)msg->address[1] << 16U);
        address |= ((xwu32_t)msg->address[2] << 8U);
        address |= ((xwu32_t)msg->address[3] << 0U);
        rc = xwds_w25qxx_erase_sector(w25qxx, address, xwtm_ft(XWTM_S(2)));
        switch (rc) {
        case XWOK:
                oprc = W25QPT_ERR_NONE;
                break;
        case -EINTR:
                oprc = W25QPT_ERR_INTR;
                break;
        case -ETIMEDOUT:
                oprc = W25QPT_ERR_TIMEOUT;
                break;
        default:
                oprc = W25QPT_ERR_IO;
                break;
        }

        sdusize = 0;
        for (i = 0; i < W25QPT_SOF_SIZE; i++) {
                msg->sof[i] = W25QPT_HWIFAL_SOF;
        }
        msg->rpc |= W25QPT_RPC_ACK;
        msg->oprc = oprc;
        msg->sdusize[0] = (xwu8_t)((sdusize >> 8U) & 0xFF);
        msg->sdusize[1] = (xwu8_t)((sdusize >> 0U) & 0xFF);
        w25qpt_fill_msg_parity(msg);
        /* msg->address is unchanged. */
        infolen = sizeof(msg->rpc) + sizeof(msg->oprc) + sizeof(msg->sdusize) +
                  sizeof(msg->address) + sdusize;
        crc32 = xwlib_crc32_calms((xwu8_t *)&msg->rpc, &infolen);
        crc32pos = &msg->sdu[sdusize];
        crc32pos[0] = (xwu8_t)((crc32 >> 24U) & 0xFF);
        crc32pos[1] = (xwu8_t)((crc32 >> 16U) & 0xFF);
        crc32pos[2] = (xwu8_t)((crc32 >> 8U) & 0xFF);
        crc32pos[3] = (xwu8_t)((crc32 >> 0U) & 0xFF);
        eofpos = &msg->sdu[sdusize + W25QPT_CHKSUM_SIZE];
        for (i = 0; i < W25QPT_EOF_SIZE; i++) {
                eofpos[i] = W25QPT_HWIFAL_EOF;
        }

        rc = w25qpt_hwifal_tx(w25qpt, msg);
        return rc;
}

/**
 * @brief 远程端擦除Flash 32K的块
 * @param[in] w25qpt: W25QPT对象的指针
 * @param[in] msg: 消息
 * @return 错误码
 * @note
 * + 协议
 *   - 输入
 * ```
 * ------------------------------------------------------------------------
 * AA AA AA AA | 05  | 00   | 00 00   | address[4] |     | CRC32[4] | 55 55
 * SOF         | rpc | oprc | sdusize | address    | sdu | CRC32    | EOF
 * ```
 *
 *   - 应答（输出）
 * ```
 * ----------------------------------------------------------------------------
 * AA AA AA AA | 85      | oprc | 00 00   | address[4] |     | CRC32[4] | 55 55
 * SOF         | rpc+ack | oprc | sdusize | address    | sdu | CRC32    | EOF
 * ```
 */
static
xwer_t w25qpt_rpc_erase_32kblock(struct w25qpt * w25qpt, struct w25qpt_msg * msg)
{
        struct xwds_w25qxx * w25qxx;
        xwsq_t i;
        xwer_t rc;
        xwu32_t address;
        xwu8_t oprc;
        xwu8_t * crc32pos;
        xwu32_t crc32;
        xwsz_t sdusize;
        xwsz_t infolen;
        xwu8_t * eofpos;

        w25qxx = w25qpt->flash;
        address = ((xwu32_t)msg->address[0] << 24U);
        address |= ((xwu32_t)msg->address[1] << 16U);
        address |= ((xwu32_t)msg->address[2] << 8U);
        address |= ((xwu32_t)msg->address[3] << 0U);
        rc = xwds_w25qxx_erase_32kblk(w25qxx, address, xwtm_ft(XWTM_S(8)));
        switch (rc) {
        case XWOK:
                oprc = W25QPT_ERR_NONE;
                break;
        case -EINTR:
                oprc = W25QPT_ERR_INTR;
                break;
        case -ETIMEDOUT:
                oprc = W25QPT_ERR_TIMEOUT;
                break;
        default:
                oprc = W25QPT_ERR_IO;
                break;
        }

        sdusize = 0;
        for (i = 0; i < W25QPT_SOF_SIZE; i++) {
                msg->sof[i] = W25QPT_HWIFAL_SOF;
        }
        msg->rpc |= W25QPT_RPC_ACK;
        msg->oprc = oprc;
        msg->sdusize[0] = (xwu8_t)((sdusize >> 8U) & 0xFF);
        msg->sdusize[1] = (xwu8_t)((sdusize >> 0U) & 0xFF);
        w25qpt_fill_msg_parity(msg);
        /* msg->address is unchanged. */
        infolen = sizeof(msg->rpc) + sizeof(msg->oprc) + sizeof(msg->sdusize) +
                  sizeof(msg->address) + sdusize;
        crc32 = xwlib_crc32_calms((xwu8_t *)&msg->rpc, &infolen);
        crc32pos = &msg->sdu[sdusize];
        crc32pos[0] = (xwu8_t)((crc32 >> 24U) & 0xFF);
        crc32pos[1] = (xwu8_t)((crc32 >> 16U) & 0xFF);
        crc32pos[2] = (xwu8_t)((crc32 >> 8U) & 0xFF);
        crc32pos[3] = (xwu8_t)((crc32 >> 0U) & 0xFF);
        eofpos = &msg->sdu[sdusize + W25QPT_CHKSUM_SIZE];
        for (i = 0; i < W25QPT_EOF_SIZE; i++) {
                eofpos[i] = W25QPT_HWIFAL_EOF;
        }

        rc = w25qpt_hwifal_tx(w25qpt, msg);
        return rc;
}

/**
 * @brief 远程端擦除Flash 64K的块
 * @param[in] w25qpt: W25QPT对象的指针
 * @param[in] msg: 消息
 * @return 错误码
 * @note
 * + 协议
 *   - 输入
 * ```
 * ------------------------------------------------------------------------
 * AA AA AA AA | 06  | 00   | 00 00   | address[4] |     | CRC32[4] | 55 55
 * SOF         | rpc | oprc | sdusize | address    | sdu | CRC32    | EOF
 * ```
 *
 *   - 应答（输出）
 * ```
 * ----------------------------------------------------------------------------
 * AA AA AA AA | 86      | oprc | 00 00   | address[4] |     | CRC32[4] | 55 55
 * SOF         | rpc+ack | oprc | sdusize | address    | sdu | CRC32    | EOF
 * ```
 */
static
xwer_t w25qpt_rpc_erase_64kblock(struct w25qpt * w25qpt, struct w25qpt_msg * msg)
{
        struct xwds_w25qxx * w25qxx;
        xwsq_t i;
        xwer_t rc;
        xwu32_t address;
        xwu8_t oprc;
        xwu8_t * crc32pos;
        xwu32_t crc32;
        xwsz_t sdusize;
        xwsz_t infolen;
        xwu8_t * eofpos;

        w25qxx = w25qpt->flash;
        address = ((xwu32_t)msg->address[0] << 24U);
        address |= ((xwu32_t)msg->address[1] << 16U);
        address |= ((xwu32_t)msg->address[2] << 8U);
        address |= ((xwu32_t)msg->address[3] << 0U);
        rc = xwds_w25qxx_erase_64kblk(w25qxx, address, xwtm_ft(XWTM_S(16)));
        switch (rc) {
        case XWOK:
                oprc = W25QPT_ERR_NONE;
                break;
        case -EINTR:
                oprc = W25QPT_ERR_INTR;
                break;
        case -ETIMEDOUT:
                oprc = W25QPT_ERR_TIMEOUT;
                break;
        default:
                oprc = W25QPT_ERR_IO;
                break;
        }

        sdusize = 0;
        for (i = 0; i < W25QPT_SOF_SIZE; i++) {
                msg->sof[i] = W25QPT_HWIFAL_SOF;
        }
        msg->rpc |= W25QPT_RPC_ACK;
        msg->oprc = oprc;
        msg->sdusize[0] = (xwu8_t)((sdusize >> 8U) & 0xFF);
        msg->sdusize[1] = (xwu8_t)((sdusize >> 0U) & 0xFF);
        w25qpt_fill_msg_parity(msg);
        /* msg->address is unchanged. */
        infolen = sizeof(msg->rpc) + sizeof(msg->oprc) + sizeof(msg->sdusize) +
                  sizeof(msg->address) + sdusize;
        crc32 = xwlib_crc32_calms((xwu8_t *)&msg->rpc, &infolen);
        crc32pos = &msg->sdu[sdusize];
        crc32pos[0] = (xwu8_t)((crc32 >> 24U) & 0xFF);
        crc32pos[1] = (xwu8_t)((crc32 >> 16U) & 0xFF);
        crc32pos[2] = (xwu8_t)((crc32 >> 8U) & 0xFF);
        crc32pos[3] = (xwu8_t)((crc32 >> 0U) & 0xFF);
        eofpos = &msg->sdu[sdusize + W25QPT_CHKSUM_SIZE];
        for (i = 0; i < W25QPT_EOF_SIZE; i++) {
                eofpos[i] = W25QPT_HWIFAL_EOF;
        }

        rc = w25qpt_hwifal_tx(w25qpt, msg);
        return rc;
}

/**
 * @brief 远程端写Flash
 * @param[in] w25qpt: W25QPT对象的指针
 * @param[in] msg: 消息
 * @return 错误码
 * @note
 * + 协议
 *   - 输入
 * ```
 * -------------------------------------------------------------------------------------
 * AA AA AA AA | 07  | 00   | sdusize[2] | address[4] | data[sdusize] | CRC32[4] | 55 55
 * SOF         | rpc | oprc | sdusize    | address    | sdu           | CRC32    | EOF
 * ```
 *
 *   - 应答（输出）
 * ```
 * --------------------------------------------------------------------------------------
 * AA AA AA AA | 87      | oprc | 40 04   | address[4] | write_size[4] | CRC32[4] | 55 55
 * SOF         | rpc+ack | oprc | sdusize | address    | sdu           | CRC32    | EOF
 * ```
 */
static
xwer_t w25qpt_rpc_write(struct w25qpt * w25qpt, struct w25qpt_msg * msg)
{
        struct xwds_w25qxx * w25qxx;
        xwsq_t i;
        xwer_t rc;
        xwu32_t address;
        xwsz_t wrsz;
        xwu8_t oprc;
        xwu8_t * crc32pos;
        xwu32_t crc32;
        xwsz_t sdusize;
        xwsz_t infolen;
        xwu8_t * eofpos;

        w25qxx = w25qpt->flash;
        address = ((xwu32_t)msg->address[0] << 24U);
        address |= ((xwu32_t)msg->address[1] << 16U);
        address |= ((xwu32_t)msg->address[2] << 8U);
        address |= ((xwu32_t)msg->address[3] << 0U);
        wrsz = w25qpt_get_sdu_size(msg);
        rc = xwds_w25qxx_write(w25qxx, address, msg->sdu, &wrsz,
                               xwtm_ft(XWTM_MS(1000)));
        switch (rc) {
        case XWOK:
                oprc = W25QPT_ERR_NONE;
                break;
        case -EINTR:
                oprc = W25QPT_ERR_INTR;
                break;
        case -ETIMEDOUT:
                oprc = W25QPT_ERR_TIMEOUT;
                break;
        default:
                oprc = W25QPT_ERR_IO;
                break;
        }

        sdusize = 4;
        for (i = 0; i < W25QPT_SOF_SIZE; i++) {
                msg->sof[i] = W25QPT_HWIFAL_SOF;
        }
        msg->rpc |= W25QPT_RPC_ACK;
        msg->oprc = oprc;
        msg->sdusize[0] = (xwu8_t)((sdusize >> 8U) & 0xFF);
        msg->sdusize[1] = (xwu8_t)((sdusize >> 0U) & 0xFF);
        w25qpt_fill_msg_parity(msg);
        /* msg->address is unchanged. */
        msg->sdu[0] = (xwu8_t)((wrsz >> 24U) & 0xFF);
        msg->sdu[1] = (xwu8_t)((wrsz >> 16U) & 0xFF);
        msg->sdu[2] = (xwu8_t)((wrsz >> 8U) & 0xFF);
        msg->sdu[3] = (xwu8_t)((wrsz >> 0U) & 0xFF);
        infolen = sizeof(msg->rpc) + sizeof(msg->oprc) + sizeof(msg->sdusize) +
                  sizeof(msg->address) + sdusize;
        crc32 = xwlib_crc32_calms((xwu8_t *)&msg->rpc, &infolen);
        crc32pos = &msg->sdu[sdusize];
        crc32pos[0] = (xwu8_t)((crc32 >> 24U) & 0xFF);
        crc32pos[1] = (xwu8_t)((crc32 >> 16U) & 0xFF);
        crc32pos[2] = (xwu8_t)((crc32 >> 8U) & 0xFF);
        crc32pos[3] = (xwu8_t)((crc32 >> 0U) & 0xFF);
        eofpos = &msg->sdu[sdusize + W25QPT_CHKSUM_SIZE];
        for (i = 0; i < W25QPT_EOF_SIZE; i++) {
                eofpos[i] = W25QPT_HWIFAL_EOF;
        }

        rc = w25qpt_hwifal_tx(w25qpt, msg);
        return rc;
}

/**
 * @brief 远程端读Flash
 * @param[in] w25qpt: W25QPT对象的指针
 * @param[in] msg: 消息
 * @return 错误码
 * @note
 * + 协议
 *   - 输入
 * ```
 * ---------------------------------------------------------------------------------
 * AA AA AA AA | 08  | 00   | 40 04   | address[4] | read_size[4] | CRC32[4] | 55 55
 * SOF         | rpc | oprc | sdusize | address    | sdu          | CRC32    | EOF
 * ```
 *
 *   - 应答（输出）
 * ```
 * ----------------------------------------------------------------------------------------
 * AA AA AA AA | 88      | oprc | sdusize[2] | address[4] | sdu[sdusize] | CRC32[4] | 55 55
 * SOF         | rpc+ack | oprc | sdusize    | address    | sdu          | CRC32    | EOF
 * ```
 */
static
xwer_t w25qpt_rpc_read(struct w25qpt * w25qpt, struct w25qpt_msg * msg)
{
        struct xwds_w25qxx * w25qxx;
        xwsq_t i;
        xwer_t rc;
        xwu32_t address;
        xwsz_t rdsz;
        xwu8_t oprc;
        xwu8_t * crc32pos;
        xwu32_t crc32;
        xwsz_t sdusize;
        xwsz_t infolen;
        xwu8_t * eofpos;

        w25qxx = w25qpt->flash;
        address = ((xwu32_t)msg->address[0] << 24U);
        address |= ((xwu32_t)msg->address[1] << 16U);
        address |= ((xwu32_t)msg->address[2] << 8U);
        address |= ((xwu32_t)msg->address[3] << 0U);
        rdsz = ((xwu32_t)msg->sdu[0] << 24U);
        rdsz |= ((xwu32_t)msg->sdu[1] << 16U);
        rdsz |= ((xwu32_t)msg->sdu[2] << 8U);
        rdsz |= ((xwu32_t)msg->sdu[3] << 0U);
        rc = xwds_w25qxx_read(w25qxx, address, msg->sdu, &rdsz,
                              xwtm_ft(XWTM_MS(1000)));
        switch (rc) {
        case XWOK:
                oprc = W25QPT_ERR_NONE;
                break;
        case -EINTR:
                oprc = W25QPT_ERR_INTR;
                break;
        case -ETIMEDOUT:
                oprc = W25QPT_ERR_TIMEOUT;
                break;
        default:
                oprc = W25QPT_ERR_IO;
                break;
        }

        sdusize = rdsz;
        for (i = 0; i < W25QPT_SOF_SIZE; i++) {
                msg->sof[i] = W25QPT_HWIFAL_SOF;
        }
        msg->rpc |= W25QPT_RPC_ACK;
        msg->oprc = oprc;
        msg->sdusize[0] = (xwu8_t)((sdusize >> 8U) & 0xFF);
        msg->sdusize[1] = (xwu8_t)((sdusize >> 0U) & 0xFF);
        w25qpt_fill_msg_parity(msg);
        /* msg->address is unchanged. */
        infolen = sizeof(msg->rpc) + sizeof(msg->oprc) + sizeof(msg->sdusize) +
                  sizeof(msg->address) + sdusize;
        crc32 = xwlib_crc32_calms((xwu8_t *)&msg->rpc, &infolen);
        crc32pos = &msg->sdu[sdusize];
        crc32pos[0] = (xwu8_t)((crc32 >> 24U) & 0xFF);
        crc32pos[1] = (xwu8_t)((crc32 >> 16U) & 0xFF);
        crc32pos[2] = (xwu8_t)((crc32 >> 8U) & 0xFF);
        crc32pos[3] = (xwu8_t)((crc32 >> 0U) & 0xFF);
        eofpos = &msg->sdu[sdusize + W25QPT_CHKSUM_SIZE];
        for (i = 0; i < W25QPT_EOF_SIZE; i++) {
                eofpos[i] = W25QPT_HWIFAL_EOF;
        }

        rc = w25qpt_hwifal_tx(w25qpt, msg);
        return rc;
}

/**
 * @brief W25Qxx编程器接收一条消息
 * @param[in] w25qpt: W25QPT对象的指针
 * @param[in] msg: 消息
 * @return 错误码
 */
static
xwer_t w25qpt_rx_msg(struct w25qpt * w25qpt, struct w25qpt_msg * msg)
{
        xwer_t rc;
        xwu8_t rpc;

        rpc = msg->rpc;
        w25qptlogf(DEBUG, "rpc: 0x%X\n", rpc);
        switch (rpc) {
        case W25QPT_RPC_READ_UID:
                rc = w25qpt_rpc_read_uid(w25qpt, msg);
                break;
        case W25QPT_RPC_READ_MID:
                rc = w25qpt_rpc_read_mid(w25qpt, msg);
                break;
        case W25QPT_RPC_READ_JID:
                rc = w25qpt_rpc_read_jid(w25qpt, msg);
                break;
        case W25QPT_RPC_ERASE_CHIP:
                rc = w25qpt_rpc_erase_chip(w25qpt, msg);
                break;
        case W25QPT_RPC_ERASE_SECTOR:
                rc = w25qpt_rpc_erase_sector(w25qpt, msg);
                break;
        case W25QPT_RPC_ERASE_32KBLOCK:
                rc = w25qpt_rpc_erase_32kblock(w25qpt, msg);
                break;
        case W25QPT_RPC_ERASE_64KBLOCK:
                rc = w25qpt_rpc_erase_64kblock(w25qpt, msg);
                break;
        case W25QPT_RPC_WRITE:
                rc = w25qpt_rpc_write(w25qpt, msg);
                break;
        case W25QPT_RPC_READ:
                rc = w25qpt_rpc_read(w25qpt, msg);
                break;
        default:
                rc = -EBADMSG;
                break;
        }
        return rc;
}

xwer_t w25qpt_chk_msg(struct w25qpt_msg * msg)
{
        xwu8_t * crc32pos;
        xwu32_t crc32;
        xwsz_t sdusize;
        xwsz_t infolen;
        xwer_t rc;

        if ((msg->rpc & W25QPT_RPC_MSK) < W25QPT_RPC_NUM) {
                sdusize = w25qpt_get_sdu_size(msg);
                infolen = sizeof(msg->rpc) + sizeof(msg->oprc) + sizeof(msg->sdusize) +
                          sizeof(msg->address) + sdusize;
                crc32pos = &msg->sdu[sdusize];
                crc32 = xwlib_crc32_calms((xwu8_t *)&msg->rpc, &infolen);
                if ((((crc32 >> 24) & 0xFF) != crc32pos[0]) ||
                    (((crc32 >> 16) & 0xFF) != crc32pos[1]) ||
                    (((crc32 >> 8) & 0xFF) != crc32pos[2]) ||
                    (((crc32 >> 0) & 0xFF) != crc32pos[3])) {
                        rc = -EBADMSG;
                } else {
                        rc = XWOK;
                }
        } else {
                rc = -EBADMSG;
        }
        return rc;
}

/**
 * @brief W25Qxx编程器状态机
 * @param[in] w25qpt: W25QPT对象的指针
 */
xwer_t w25qpt_fsm(struct w25qpt * w25qpt)
{
        __xwcc_aligned(XWMM_ALIGNMENT) struct w25qpt_msg msg;
        xwer_t rc;

        do {
                rc = w25qpt_hwifal_rx(w25qpt, &msg);
                if (XWOK == rc) {
                        rc = w25qpt_chk_msg(&msg);
                }
        } while ((-EAGAIN == rc) || (-EBADMSG == rc));
        if (XWOK == rc) {
                rc = w25qpt_rx_msg(w25qpt, &msg);
        }
        return rc;
}

/**
 * @brief W25Qxx编程器线程的主函数
 */
xwer_t w25qpt_thd_func(void * arg)
{
        xwer_t rc = XWOK;
        struct w25qpt * w25qpt;

        rc = XWOK;
        w25qpt = arg;
        while (!xwos_cthd_shld_stop()) {
                if (xwos_cthd_shld_frz()) {
                        w25qptlogf(DEBUG, "开始冻结 ...\n");
                        rc = xwos_cthd_freeze();
                        if (rc < 0) {
                                w25qptlogf(ERR, "冻结失败 ... [%d]\n", rc);
                                xwos_cthd_yield();
                        }/* else {} */
                        w25qptlogf(DEBUG, "开始复苏 ...\n");
                } else if (W25QPT_HWIFST_OPENED == w25qpt->hwifst) {
                        rc = w25qpt_fsm(w25qpt);
                        if (XWOK == rc) {
                        } else if (-ETIMEDOUT == rc) {
                        } else if ((-EINTR == rc) || (-ERESTARTSYS == rc)) {
                                w25qptlogf(DEBUG, "被中断 ... [%d]\n", rc);
                        } else if (-EBADMSG == rc) {
                                w25qptlogf(WARNING, "错误的消息\n");
                        } else {
                                w25qptlogf(ERR, "其他错误 ... [%d]\n", rc);
                                break;
                        }
                } else {
                        break;
                }
        }
        return rc;
}
