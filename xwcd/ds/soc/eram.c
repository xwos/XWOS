/**
 * @file
 * @brief 玄武设备栈：SOC外部RAM
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

#include <xwcd/ds/standard.h>
#include <xwcd/ds/soc/eram.h>

/**
 * @brief XWDS API：测试RAM
 * @param[in] soc: SOC对象指针
 * @param[in] erraddr: 返回错误地址的缓冲区的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 测试错误
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
__xwds_api
xwer_t xwds_eram_test(struct xwds_soc * soc, xwptr_t * erraddr)
{
        const struct xwds_soc_driver * drv;
        xwer_t rc;

        XWDS_VALIDATE(soc, "nullptr", -EFAULT);
        XWDS_VALIDATE(erraddr, "nullptr", -EFAULT);

        rc = xwds_soc_grab(soc);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_soc_grab;
        }
        drv = xwds_cast(struct xwds_soc_driver *, soc->dev.drv);
        if ((drv) && (drv->eram_tst)) {
                rc = drv->eram_tst(soc, erraddr);
                if (__xwcc_unlikely(rc < 0)) {
                        rc = EFAULT;
                }
        } else {
                rc = -ENOSYS;
        }
        if (__xwcc_unlikely(rc < 0)) {
                goto err_drv_tst;
        }
        xwds_soc_put(soc);
        return XWOK;

err_drv_tst:
        xwds_soc_put(soc);
err_soc_grab:
        return rc;
}
