/**
 * @file
 * @brief xwmd设备栈：SOC时钟
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
#include <xwmd/ds/soc/clock.h>

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
 * @brief XWDS API：申请时钟
 * @param soc: (I) SOC对象指针
 * @param id: (I) 时钟ID
 * @return 错误码
 * @retval OK: OK
 * @retval -ERANGE: 时钟ID错误
 * @retval -ENOSYS: 不支持的API
 * @note
 * - 同步/异步：同步
 * - 中断上下文：可以使用
 * - 中断底半部：可以使用
 * - 线程上下文：可以使用
 * - 重入性：对于同一个时钟不可重入；对于不同时钟可重入
 */
__xwds_api
xwer_t xwds_clk_req(struct xwds_soc * soc, xwid_t id)
{
        const struct xwds_soc_driver * drv;
        xwer_t rc;

        XWDS_VALIDATE(soc, "nullptr", -EFAULT);
        XWDS_VALIDATE((id < soc->clk.num), "out-of-range", -ERANGE);

        rc = xwds_soc_grab(soc);
        if (__unlikely(rc < 0)) {
                goto err_soc_grab;
        }

        drv = xwds_static_cast(const struct xwds_soc_driver *, soc->dev.drv);
        if ((drv) && (drv->clk_req)) {
                rc = drv->clk_req(soc, id);
        } else {
                rc = -ENOSYS;
        }
        if (__unlikely(rc < 0)) {
                goto err_drv_clk_req;
        }
        return OK;

err_drv_clk_req:
        xwds_soc_put(soc);
err_soc_grab:
        return rc;
}

/**
 * @brief XWDS API：释放时钟
 * @param soc: (I) SOC对象指针
 * @param id: (I) 时钟ID
 * @return 错误码
 * @retval OK: OK
 * @retval -ERANGE: 时钟ID错误
 * @retval -ENOSYS: 不支持的API
 * @note
 * - 同步/异步：同步
 * - 中断上下文：可以使用
 * - 中断底半部：可以使用
 * - 线程上下文：可以使用
 * - 重入性：对于同一个时钟不可重入；对于不同时钟可重入
 */
__xwds_api
xwer_t xwds_clk_rls(struct xwds_soc * soc, xwid_t id)
{
        const struct xwds_soc_driver * drv;
        xwer_t rc;

        XWDS_VALIDATE(soc, "nullptr", -EFAULT);
        XWDS_VALIDATE((id < soc->clk.num), "out-of-range", -ERANGE);

        drv = xwds_static_cast(struct xwds_soc_driver *, soc->dev.drv);
        if ((drv) && (drv->clk_rls)) {
                rc = drv->clk_rls(soc, id);
        } else {
                rc = -ENOSYS;
        }
        if (__unlikely(rc < 0)) {
                goto err_drv_clk_rls;
        }

        xwds_soc_put(soc);
        return OK;

err_drv_clk_rls:
        return rc;
}

/**
 * @brief XWDS API：得到时钟频率
 * @param soc: (I) SOC对象指针
 * @param id: (I) 时钟ID
 * @param buf: (O) 返回时钟频率的缓冲区的指针
 * @param num: (I) 缓冲区数组的数量
 *             (O) 返回的数组的数量
 * @return 错误码
 * @retval OK: OK
 * @retval -EFAULT: 无效指针
 * @retval -ERANGE: 时钟ID错误
 * @retval -ENOSYS: 不支持的API
 * @note
 * - 同步/异步：同步
 * - 中断上下文：可以使用
 * - 中断底半部：可以使用
 * - 线程上下文：可以使用
 * - 重入性：可重入
 */
__xwds_api
xwer_t xwds_clk_getfqcy(struct xwds_soc * soc, xwid_t id,
                        xwu32_t * buf, xwsz_t * num)
{
        const struct xwds_soc_driver * drv;
        xwer_t rc;

        XWDS_VALIDATE(soc, "nullptr", -EFAULT);
        XWDS_VALIDATE((id < soc->clk.num), "out-of-range", -ERANGE);
        XWDS_VALIDATE(buf, "nullptr", -EFAULT);
        XWDS_VALIDATE(num, "nullptr", -EFAULT);

        rc = xwds_soc_grab(soc);
        if (__unlikely(rc < 0)) {
                goto err_soc_grab;
        }

        drv = xwds_static_cast(struct xwds_soc_driver *, soc->dev.drv);
        if ((drv) && (drv->clk_getfqcy)) {
                rc = drv->clk_getfqcy(soc, id, buf, num);
        } else {
                rc = -ENOSYS;
        }
        if (__unlikely(rc < 0)) {
                goto err_drv_getfqcy;
        }

        xwds_soc_put(soc);
        return OK;

err_drv_getfqcy:
        xwds_soc_put(soc);
err_soc_grab:
        return rc;
}

/**
 * @brief 通过描述得到时钟资源
 * @param base: (I) 时钟资源数组的基地址
 * @param num: (I) 时钟资源数量
 * @param descay: (I) 寄存器描述数组
 * @param descnum: (I) 寄存器描述数组数量
 * @param ret: (O) 返回时钟资源地址的缓存
 * @return 错误码
 * @retval OK: OK
 * @retval -EFAULT: 无效指针
 * @retval -ENOKEY: 找不到描述的资源
 */
__xwds_lib_code
xwer_t xwds_get_clkrsc(const struct xwds_resource_clk base[], xwsz_t num,
                       const char * descay[], xwsz_t descnum,
                       const struct xwds_resource_clk ** ret)
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
