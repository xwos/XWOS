/**
 * @file
 * @brief W25Qxx编程器：协议
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
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

#ifndef __xwam_application_w25qrpt_protocol_h__
#define __xwam_application_w25qrpt_protocol_h__

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      macros       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define W25QRPT_PROTOCOL_VERSION        ("1.0.0")
#define W25QRPT_ADDRESS_SIZE            4U
#define W25QRPT_SDU_MAXSIZE             256U
#define W25QRPT_HEAD_SIZE               4U
#define W25QRPT_CHKSUM_SIZE             4U
#define W25QRPT_SOF_SIZE                4U
#define W25QRPT_EOF_SIZE                2U
#define W25QRPT_HWIFAL_SOF              0xAAU
#define W25QRPT_HWIFAL_EOF              0x55U

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief 远程端命令枚举
 */
enum w25qrpt_rpc_em {
        W25QRPT_RPC_READ_UID = 0,
        W25QRPT_RPC_READ_MID,
        W25QRPT_RPC_READ_JID,

        W25QRPT_RPC_ERASE_CHIP,
        W25QRPT_RPC_ERASE_SECTOR,
        W25QRPT_RPC_ERASE_32KBLOCK,
        W25QRPT_RPC_ERASE_64KBLOCK,

        W25QRPT_RPC_WRITE,
        W25QRPT_RPC_READ,
        W25QRPT_RPC_NUM,
        W25QRPT_RPC_ACK = 0x80,
        W25QRPT_RPC_MSK = W25QRPT_RPC_ACK - 1,
};

/**
 * @brief 错误枚举
 */
enum w25qrpt_error_em {
        W25QRPT_ERR_NONE = 0,
        W25QRPT_ERR_INTR,
        W25QRPT_ERR_TIMEOUT,
        W25QRPT_ERR_IO,
};

/**
 * @brief 通讯消息
 * @note 所有数据使用大端格式（MSB <---> LSB）传输
 */
struct __xwcc_packed w25qrpt_msg {
        xwu8_t sof[W25QRPT_SOF_SIZE]; /**< 帧首定界符 (SOF:Start-of-Frame Delimiter) */
        xwu8_t rpc; /**< 命令或命令的回应 */
        xwu8_t oprc; /**< 操作的结果 */
        xwu8_t sdusize[2]; /**< 数据sdu大小，最高两位为偶校验位 */
        xwu8_t address[W25QRPT_ADDRESS_SIZE]; /**< Flash地址 */
        xwu8_t sdu[W25QRPT_SDU_MAXSIZE]; /**< Flash数据，若整个结构体对齐到4的边界上
                                              sdu也刚好对齐到4的边界上 */
        xwu8_t crc32[W25QRPT_CHKSUM_SIZE]; /**< CRC32校验和
                                                (rpc oprc size mirror address sdu) */
        xwu8_t eof[W25QRPT_EOF_SIZE]; /**< 帧尾定界符 (EOF:End-of-Frame Delimiter) */
};

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         function prototypes         ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

#endif /* xwam/application/w25qrpt/protocol.h */
