/**
 * @file
 * @brief 玄武OS通用库：位图原子操作
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef __xwos_lib_xwbmpaop_h__
#define __xwos_lib_xwbmpaop_h__

#include <xwos/standard.h>
#include <xwos/lib/xwbop.h>
#include <xwos/lib/xwaop.h>

/**
 * @defgroup xwos_lib_xwbmpaop 位图原子操作
 * @ingroup xwos_lib
 * @{
 */

#include <xwos/ospl/soc/xwbmpaop.h>

/**
 * @brief XWOS BOPLIB：声明原子位图
 * @param[in] name: 符号名
 * @param[in] bits: 位图中的位数
 */
#define xwbmpaop_define(name, bits) atomic_xwbmp_t name[BITS_TO_XWBMP_T(bits)]

/**
 * @brief XWOS AOPLIB：测试位图中的某位是否被置1
 * @param[in] bmp: 位图的起始地址指针
 * @param[in] idx: 被测试的位的序号
 * @return 布尔值
 * @retval true: 置位
 * @retval false: 复位
 * @note
 * - 内存序：consume
 */
bool xwbmpaop_t1i(atomic_xwbmp_t * bmp, xwsq_t idx);

/**
 * @brief XWOS AOPLIB：将位图中某位置1
 * @param[in] bmp: 位图的起始地址指针
 * @param[in] idx: 被置1的位的序号
 * @note
 * - 内存序：acq_rel
 */
void xwbmpaop_s1i(atomic_xwbmp_t * bmp, xwsq_t idx);

/**
 * @brief XWOS AOPLIB：将位图中某位清0
 * @param[in] bmp: 位图的起始地址指针
 * @param[in] idx: 被清0的位的序号
 * @note
 * - 内存序：acq_rel
 */
void xwbmpaop_c0i(atomic_xwbmp_t * bmp, xwsq_t idx);

/**
 * @brief XWOS AOPLIB：将位图中某位翻转
 * @param[in] bmp: 位图的起始地址指针
 * @param[in] idx: 被翻转的位的序号
 * @note
 * - 内存序：acq_rel
 */
void xwbmpaop_x1i(atomic_xwbmp_t * bmp, xwsq_t idx);

/**
 * @brief XWOS AOPLIB：测试位图中某位是否为0，如果是，就将它置1
 * @param[in] bmp: 位图的起始地址指针
 * @param[in] idx: 被测试位的序号
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EACCES: 测试失败
 * @note
 * - 内存序：测试成功，acq_rel；测试失败，consume；
 */
xwer_t xwbmpaop_t0i_then_s1i(atomic_xwbmp_t * bmp, xwsq_t idx);

/**
 * @brief XWOS AOPLIB：测试位图中某位是否为1，如果是，就将它清0
 * @param[in] bmp: 位图的起始地址指针
 * @param[in] idx: 被测试位的序号
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EACCES: 测试失败
 * @note
 * - 内存序：测试成功，acq_rel；测试失败，consume；
 */
xwer_t xwbmpaop_t1i_then_c0i(atomic_xwbmp_t * bmp, xwsq_t idx);

/**
 * @brief XWOS AOPLIB：从最高位起查找位图中第一个为1的位并将它清0
 * @param[in] bmp: 位图的起始地址指针
 * @param[in] num: 位图中总的位数
 * @return 错误码
 * @retval >=0: 位的序号
 * @retval -ENODATA: 没有任何一个位为1
 * @note
 * - 内存序：测试成功，acq_rel；测试失败，consume；
 */
xwssq_t xwbmpaop_fls_then_c0i(atomic_xwbmp_t * bmp, xwsz_t num);

/**
 * @brief XWOS AOPLIB：从最高位起查找位图中第一个为0的位并将它置1
 * @param[in] bmp: 位图的起始地址指针
 * @param[in] num: 位图中总的位数
 * @return 错误码
 * @retval >=0: 位的序号
 * @retval -ENODATA: 没有任何一个位为0
 * @note
 * - 内存序：测试成功，acq_rel；测试失败，consume；
 */
xwssq_t xwbmpaop_flz_then_s1i(atomic_xwbmp_t * bmp, xwsz_t num);

/**
 * @brief XWOS AOPLIB：从最低位起查找位图中第一个为1的位并将它清0
 * @param[in] bmp: 位图的起始地址指针
 * @param[in] num: 位图中总的位数
 * @return 错误码
 * @retval >=0: 位的序号
 * @retval -ENODATA: 没有任何一个位为1
 * @note
 * - 内存序：测试成功，acq_rel；测试失败，consume；
 */
xwssq_t xwbmpaop_ffs_then_c0i(atomic_xwbmp_t * bmp, xwsz_t num);

/**
 * @brief XWOS AOPLIB：从最低位起查找位图中第一个为0的位并将它置1
 * @param[in] bmp: 位图的起始地址指针
 * @param[in] num: 位图中总的位数
 * @return 错误码
 * @retval >=0: 位的序号
 * @retval -ENODATA: 没有任何一个位为0
 * @note
 * - 内存序：测试成功，acq_rel；测试失败，consume；
 */
xwssq_t xwbmpaop_ffz_then_s1i(atomic_xwbmp_t * bmp, xwsz_t num);

/**
 * @brief XWOS AOPLIB：将位图所有位置1
 * @param[in] bmp: 位图的起始地址指针
 * @param[in] num: 位图中总的位数
 * @note
 * - 内存序：acq_rel
 */
static __xwlib_inline
void xwbmpaop_s1all(atomic_xwbmp_t * bmp, xwsq_t num)
{
        xwsz_t n = BITS_TO_XWBMP_T(num);
        xwsz_t i;

        for (i = 0; i < n; i++) {
                xwaop_s1m(xwbmp_t, &bmp[i], (xwbmp_t)(~0), NULL, NULL);
        }
}

/**
 * @brief XWOS AOPLIB：将位图所有位清0
 * @param[in] bmp: 位图的起始地址指针
 * @param[in] num: 位图中总的位数
 * @note
 * - 内存序：acq_rel
 */
static __xwlib_inline
void xwbmpaop_c0all(atomic_xwbmp_t * bmp, xwsq_t num)
{
        xwsz_t n = BITS_TO_XWBMP_T(num);
        xwsz_t i;

        for (i = 0; i < n; i++) {
                xwaop_c0m(xwbmp_t, &bmp[i], (xwbmp_t)(~0), NULL, NULL);
        }
}

/**
 * @} xwos_lib_xwbmpaop
 */

#endif /* xwos/lib/xwbmpaop.h */
