/**
 * @file
 * @brief XWOS通用库：CRC32
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __xwos_lib_crc32_h__
#define __xwos_lib_crc32_h__

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>

#if ((!defined(XWLIBCFG_CRC32_0X04C11DB7) || (1 != XWLIBCFG_CRC32_0X04C11DB7)) \
      && \
     (!defined(XWLIBCFG_CRC32_0XEDB88320) || (1 != XWLIBCFG_CRC32_0XEDB88320)))
  #error "Can't find the configuration of crc32 polynomial"
#endif

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief 计算CRC32的移位方向枚举
 */
enum xwlib_crc32_shift_direction_em {
        XWLIB_CRC32_LEFT_SHIFT, /**< 左移 */
        XWLIB_CRC32_RIGHT_SHIFT, /**< 右移 */
};

/**
 * @brief CRC32多项式枚举
 */
enum xwlib_crc32_plynml_em {
#if (defined(XWLIBCFG_CRC32_0X04C11DB7) && (1 == XWLIBCFG_CRC32_0X04C11DB7))
        XWLIB_CRC32_PLYNML_0X04C11DB7, /**< 多项式:0x04C11DB7 */
#endif

#if (defined(XWLIBCFG_CRC32_0XEDB88320) && (1 == XWLIBCFG_CRC32_0XEDB88320))
        XWLIB_CRC32_PLYNML_0XEDB88320, /**< 多项式:0XEDB88320 */
#endif
        XWLIB_CRC32_PLYNML_NUM, /**< 多项式的数量 */
};

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         function prototypes         ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief 计算一段数据的CRC32校验值
 * @param crc32: (I) 初始值
 *               (O) 计算结果
 * @param xorout: (I) 与结果异或的值
 * @param refin: (I) 是否按位镜像翻转输入的每个字节(xwbop_rbit8)
 * @param refout: (I) 是否按位镜像翻转输出(xwbop_rbit32)
 * @param plynml: (I) 多项式
 * @param direction: (I) 数据移位的方向
 * @param stream: (I) 指向数据的指针
 * @param len: (I) 数据长度
 */
__xwlib_code
void xwlib_crc32_cal(xwu32_t * crc32, xwu32_t xorout,
                     bool refin, bool refout,
                     int plynml, int direction,
                     const xwu8_t stream[], xwsz_t len);

/**
 * @brief 用主流CRC32参数模型计算一段数据的CRC32校验值
 * @param stream: (I) 指向数据的指针
 * @param len: (I) 数据长度
 * @return CRC32
 * @note
 * > 主流CRC32参数模型：
 * > - 多项式：0xEDB88320
 * > - 初始值：0xFFFFFFFF
 * > - 数据移位方向：右移
 * > - 是否按位镜像翻转输入：否
 * > - 是否按位镜像翻转输出：否
 * > - 与结果进行异或计算：0xFFFFFFFF
 */
__xwlib_code
xwu32_t xwlib_crc32_calms(const xwu8_t stream[], xwsz_t len);

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********  inline functions implementations   ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

#endif /* xwos/lib/crc32.h */
