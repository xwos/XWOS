/**
 * @file
 * @brief 玄武设备栈：SOC外部中断
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
#include <xwcd/ds/soc/gpio.h>
#include <xwcd/ds/soc/eirq.h>

/**
 * @brief XWDS API：申请外部中断
 * @param[in] soc: SOC对象指针
 * @param[in] port: GPIO端口
 * @param[in] pinmask: GPIO PIN
 * @param[in] eiid: 外部中断ID
 * @param[in] eiflag: 触发标志
 * @param[in] isr: 中断响应函数
 * @param[in] arg: 中断响应函数参数
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效指针
 * @retval -ERANGE: 外部中断ID错误
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
__xwds_api
xwer_t xwds_eirq_req(struct xwds_soc * soc, xwid_t port, xwsq_t pinmask,
                     xwid_t eiid, xwsq_t eiflag,
                     xwds_eirq_f isr, xwds_eirq_arg_t arg)
{
        const struct xwds_soc_driver * drv;
        xwer_t rc;

        XWDS_VALIDATE(soc, "nullptr", -EFAULT);
        XWDS_VALIDATE((eiid < soc->eirq.num), "out-of-range", -ERANGE);

        rc = xwds_soc_grab(soc);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_soc_grab;
        }

#if defined(XWCDCFG_ds_SOC_EIRQ_ROISRT) && (1 == XWCDCFG_ds_SOC_EIRQ_ROISRT)
        XWOS_UNUSED(isr);
        XWOS_UNUSED(arg);
        if (__xwcc_unlikely(NULL == soc->eirq.isrs[eiid])) {
                rc = -EPERM;
                goto err_perm;
        }
#else
        if (__xwcc_unlikely(soc->eirq.isrs[eiid])) {
                rc = -EBUSY;
                goto err_perm;
        }
        soc->eirq.isrs[eiid] = isr ? isr : (xwds_eirq_f)err_ptr(-EBUSY);
        soc->eirq.isrargs[eiid] = arg;
#endif

        drv = xwds_cast(const struct xwds_soc_driver *, soc->dev.drv);
        if ((drv) && (drv->eirq_req)) {
                rc = drv->eirq_req(soc, port, pinmask, eiid, eiflag);
        } else {
                rc = -ENOSYS;
        }
        if (__xwcc_unlikely(rc < 0)) {
                goto err_drv_eirq_req;
        }
        return XWOK;

err_drv_eirq_req:
#if !defined(XWCDCFG_ds_SOC_EIRQ_ROISRT) || (1 != XWCDCFG_ds_SOC_EIRQ_ROISRT)
        soc->eirq.isrs[eiid] = NULL;
        soc->eirq.isrargs[eiid] = NULL;
#endif
err_perm:
        xwds_soc_put(soc);
err_soc_grab:
        return rc;
}

/**
 * @brief XWDS API：释放外部中断
 * @param[in] soc: SOC对象指针
 * @param[in] port: GPIO端口
 * @param[in] pinmask: GPIO PIN
 * @param[in] eiid: 外部中断ID
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效指针
 * @retval -ERANGE: 外部中断ID错误
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
__xwds_api
xwer_t xwds_eirq_rls(struct xwds_soc * soc, xwid_t port, xwsq_t pinmask, xwid_t eiid)
{
        const struct xwds_soc_driver * drv;
        xwer_t rc;

        XWDS_VALIDATE(soc, "nullptr", -EFAULT);
        XWDS_VALIDATE((eiid < soc->eirq.num), "out-of-range", -ERANGE);

        if (__xwcc_unlikely(NULL == soc->eirq.isrs[eiid])) {
                rc = -EPERM;
                goto err_notinused;
        }
        drv = xwds_cast(const struct xwds_soc_driver *, soc->dev.drv);
        if ((drv) && (drv->eirq_rls)) {
                rc = drv->eirq_rls(soc, port, pinmask, eiid);
        } else {
                rc = -ENOSYS;
        }
        if (__xwcc_unlikely(rc < 0)) {
                goto err_drv_rlsei;
        }
#if !defined(XWCDCFG_ds_SOC_EIRQ_ROISRT) || (1 != XWCDCFG_ds_SOC_EIRQ_ROISRT)
        soc->eirq.isrs[eiid] = NULL;
        soc->eirq.isrargs[eiid] = NULL;
#endif

        xwds_soc_put(soc);
        return XWOK;

err_drv_rlsei:
err_notinused:
        return rc;
}
