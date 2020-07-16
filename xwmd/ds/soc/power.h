/**
 * @file
 * @brief xwmd设备栈：SOC电源
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __xwmd_ds_soc_power_h__
#define __xwmd_ds_soc_power_h__

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwmd/ds/standard.h>
#include <xwmd/ds/soc/chip.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      macors       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         function prototypes         ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief XWDS API：申请电源
 * @param soc: (I) SOC对象指针
 * @param id: (I) 电源ID
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -ERANGE: 电源ID错误
 * @retval -ENOSYS: 不支持的API
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
__xwds_api
xwer_t xwds_pwr_req(struct xwds_soc * soc, xwid_t id);

/**
 * @brief XWDS API：释放电源
 * @param soc: (I) SOC对象指针
 * @param id: (I) 电源ID
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -ERANGE: 电源ID错误
 * @retval -ENOSYS: 不支持的API
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
__xwds_api
xwer_t xwds_pwr_rls(struct xwds_soc * soc, xwid_t id);

/**
 * @brief XWDS API：获取电源电压
 * @param soc: (I) SOC对象指针
 * @param id: (I) 电源ID
 * @param buf: (O) 返回电源电压的缓冲区的指针
 * @param num: (I) 缓冲区数组的数量
 *             (O) 返回的数组的数量
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效指针
 * @retval -ERANGE: 电源ID错误
 * @retval -ENOSYS: 不支持的API
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
__xwds_api
xwer_t xwds_pwr_getvltg(struct xwds_soc * soc, xwid_t id,
                        xwu32_t * buf, xwsz_t * num);

/**
 * @brief XWDS API：通过描述得到电源资源
 * @param base: (I) 电源资源数组的基地址
 * @param num: (I) 电源资源数量
 * @param descay: (I) 寄存器描述数组
 * @param descnum: (I) 寄存器描述数组数量
 * @param ret: (O) 返回电源资源地址的缓存
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效指针
 * @retval -ENOSR: 找不到描述的资源
 */
__xwds_api
xwer_t xwds_get_pwrrsc(const struct xwds_resource_pwr base[], xwsz_t num,
                       const char * descay[], xwsz_t descnum,
                       const struct xwds_resource_pwr ** ret);

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********  inline functions implementations   ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

#endif /* xwmd/ds/soc/power.h */
