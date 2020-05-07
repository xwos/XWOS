/**
 * @file
 * @brief xwmd设备栈：SOC电源
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
#include <xwmd/ds/standard.h>
#include <xwos/lib/string.h>
#include <xwmd/ds/soc/power.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********     static function prototypes      ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief SODS API：申请电源
 * @param soc: (I) SOC对象指针
 * @param id: (I) 电源ID
 * @return 错误码
 * @retval OK: OK
 * @retval -ERANGE: 电源ID错误
 * @retval -ENOSYS: 不支持的API
 * @note
 * - 同步/异步：同步
 * - 中断上下文：可以使用
 * - 中断底半部：可以使用
 * - 线程上下文：可以使用
 * - 重入性：对于同一个电源不可重入；对于不同电源可重入
 */
__xwds_api
xwer_t xwds_pwr_req(struct xwds_soc * soc, xwid_t id)
{
        const struct xwds_soc_driver * drv;
        xwer_t rc;

        SODS_VALIDATE(soc, "nullptr", -EFAULT);
        SODS_VALIDATE((id < soc->pwr.num), "out-of-range", -ERANGE);

        rc = xwds_soc_grab(soc);
        if (__unlikely(rc < 0)) {
                goto err_soc_grab;
        }

        drv = xwds_static_cast(const struct xwds_soc_driver *, soc->dev.drv);
        if ((drv) && (drv->pwr_req)) {
                rc = drv->pwr_req(soc, id);
        } else {
                rc = -ENOSYS;
        }
        if (__unlikely(rc < 0)) {
                goto err_drv_pwr_req;
        }
        return OK;

err_drv_pwr_req:
        xwds_soc_put(soc);
err_soc_grab:
        return rc;
}

/**
 * @brief SODS API：释放电源
 * @param soc: (I) SOC对象指针
 * @param id: (I) 电源ID
 * @return 错误码
 * @retval OK: OK
 * @retval -ERANGE: 电源ID错误
 * @retval -ENOSYS: 不支持的API
 * @note
 * - 同步/异步：同步
 * - 中断上下文：可以使用
 * - 中断底半部：可以使用
 * - 线程上下文：可以使用
 * - 重入性：对于同一个电源不可重入；对于不同电源可重入
 */
__xwds_api
xwer_t xwds_pwr_rls(struct xwds_soc * soc, xwid_t id)
{
        const struct xwds_soc_driver * drv;
        xwer_t rc;

        SODS_VALIDATE(soc, "nullptr", -EFAULT);
        SODS_VALIDATE((id < soc->pwr.num), "out-of-range", -ERANGE);

        drv = xwds_static_cast(const struct xwds_soc_driver *, soc->dev.drv);
        if ((drv) && (drv->pwr_rls)) {
                rc = drv->pwr_rls(soc, id);
        } else {
                rc = -ENOSYS;
        }
        if (__unlikely(rc < 0)) {
                goto err_drv_pwr_rls;
        }

        xwds_soc_put(soc);
        return OK;

err_drv_pwr_rls:
        return rc;
}

/**
 * @brief SODS API：得到电源电压
 * @param soc: (I) SOC对象指针
 * @param id: (I) 电源ID
 * @param buf: (O) 返回电源电压的缓冲区的指针
 * @param num: (I) 缓冲区数组的数量
 *             (O) 返回的数组的数量
 * @return 错误码
 * @retval OK: OK
 * @retval -EFAULT: 无效指针
 * @retval -ERANGE: 电源ID错误
 * @retval -ENOSYS: 不支持的API
 * @note
 * - 同步/异步：同步
 * - 中断上下文：可以使用
 * - 中断底半部：可以使用
 * - 线程上下文：可以使用
 * - 重入性：可重入
 */
__xwds_api
xwer_t xwds_pwr_getvltg(struct xwds_soc * soc, xwid_t id,
                        xwu32_t * buf, xwsz_t * num)
{
        const struct xwds_soc_driver * drv;
        xwer_t rc;

        SODS_VALIDATE(soc, "nullptr", -EFAULT);
        SODS_VALIDATE((id < soc->pwr.num), "out-of-range", -ERANGE);
        SODS_VALIDATE(buf, "nullptr", -EFAULT);
        SODS_VALIDATE(num, "nullptr", -EFAULT);

        rc = xwds_soc_grab(soc);
        if (__unlikely(rc < 0)) {
                goto err_soc_grab;
        }

        drv = xwds_static_cast(const struct xwds_soc_driver *, soc->dev.drv);
        if ((drv) && (drv->pwr_getvltg)) {
                rc = drv->pwr_getvltg(soc, id, buf, num);
        } else {
                rc = -ENOSYS;
        }
        if (__unlikely(rc < 0)) {
                goto err_drv_getvltg;
        }

        xwds_soc_put(soc);
        return OK;

err_drv_getvltg:
        xwds_soc_put(soc);
err_soc_grab:
        return rc;
}

/**
 * @brief SODS LIB：通过描述得到电源资源
 * @param base: (I) 电源资源数组的基地址
 * @param num: (I) 电源资源数量
 * @param descay: (I) 寄存器描述数组
 * @param descnum: (I) 寄存器描述数组数量
 * @param ret: (O) 返回电源资源地址的缓存
 * @return 错误码
 * @retval OK: OK
 * @retval -EFAULT: 无效指针
 * @retval -ENOKEY: 找不到描述的资源
 */
__xwds_lib_code
xwer_t xwds_get_pwrrsc(const struct xwds_resource_pwr base[], xwsz_t num,
                       const char * descay[], xwsz_t descnum,
                       const struct xwds_resource_pwr ** ret)
{
        xwer_t rc;
        xwsz_t i, m;

        for (i = 0; i < num; i++) {
                rc = OK;
                for (m = 0; m < descnum; m++) {
                        if (!strstr(base[i].description, descay[m])) {
                                rc = -ENOKEY;
                                break;
                        }
                }
                if (OK == rc) {
                        break;
                }
        }
        if (OK == rc) {
                *ret = &base[i];
        } else {
                *ret = NULL;
        }
        return rc;
}
