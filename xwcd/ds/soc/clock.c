/**
 * @file
 * @brief 玄武设备栈：SOC时钟
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

#include <xwcd/ds/standard.h>
#include <string.h>
#include <xwcd/ds/soc/clock.h>

/**
 * @brief XWDS API：申请时钟
 * @param[in] soc: SOC对象指针
 * @param[in] id: 时钟ID
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -ERANGE: 时钟ID错误
 * @retval -ENOSYS: 不支持的API
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
__xwds_api
xwer_t xwds_clk_req(struct xwds_soc * soc, xwid_t id)
{
        const struct xwds_soc_driver * drv;
        xwer_t rc;

        XWDS_VALIDATE(soc, "nullptr", -EFAULT);
        XWDS_VALIDATE((id < soc->clk.num), "out-of-range", -ERANGE);

        rc = xwds_soc_grab(soc);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_soc_grab;
        }

        drv = xwds_cast(const struct xwds_soc_driver *, soc->dev.drv);
        if ((drv) && (drv->clk_req)) {
                rc = drv->clk_req(soc, id);
        } else {
                rc = -ENOSYS;
        }
        if (__xwcc_unlikely(rc < 0)) {
                goto err_drv_clk_req;
        }
        return XWOK;

err_drv_clk_req:
        xwds_soc_put(soc);
err_soc_grab:
        return rc;
}

/**
 * @brief XWDS API：释放时钟
 * @param[in] soc: SOC对象指针
 * @param[in] id: 时钟ID
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -ERANGE: 时钟ID错误
 * @retval -ENOSYS: 不支持的API
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
__xwds_api
xwer_t xwds_clk_rls(struct xwds_soc * soc, xwid_t id)
{
        const struct xwds_soc_driver * drv;
        xwer_t rc;

        XWDS_VALIDATE(soc, "nullptr", -EFAULT);
        XWDS_VALIDATE((id < soc->clk.num), "out-of-range", -ERANGE);

        drv = xwds_cast(struct xwds_soc_driver *, soc->dev.drv);
        if ((drv) && (drv->clk_rls)) {
                rc = drv->clk_rls(soc, id);
        } else {
                rc = -ENOSYS;
        }
        if (__xwcc_unlikely(rc < 0)) {
                goto err_drv_clk_rls;
        }

        xwds_soc_put(soc);
        return XWOK;

err_drv_clk_rls:
        return rc;
}

/**
 * @brief XWDS API：获取时钟频率
 * @param[in] soc: SOC对象指针
 * @param[in] id: 时钟ID
 * @param[out] buf: 返回时钟频率的缓冲区的指针
 * @param[in,out] num: 指向缓冲区的指针，此缓冲区：
 * + (I) 作为输入时，表示缓冲区数组的数量
 * + (O) 作为输出时，返回实际的频率数据的数量
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效指针
 * @retval -ERANGE: 时钟ID错误
 * @retval -ENOSYS: 不支持的API
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
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
        if (__xwcc_unlikely(rc < 0)) {
                goto err_soc_grab;
        }

        drv = xwds_cast(struct xwds_soc_driver *, soc->dev.drv);
        if ((drv) && (drv->clk_getfqcy)) {
                rc = drv->clk_getfqcy(soc, id, buf, num);
        } else {
                rc = -ENOSYS;
        }
        if (__xwcc_unlikely(rc < 0)) {
                goto err_drv_getfqcy;
        }

        xwds_soc_put(soc);
        return XWOK;

err_drv_getfqcy:
        xwds_soc_put(soc);
err_soc_grab:
        return rc;
}
