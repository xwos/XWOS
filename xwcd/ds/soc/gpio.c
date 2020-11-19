/**
 * @file
 * @brief 玄武设备栈：GPIO
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
#include <string.h>
#include <xwos/lib/xwaop.h>
#include <xwcd/ds/soc/gpio.h>

/**
 * @brief XWDS API：申请GPIO
 * @param soc: (I) SOC对象指针
 * @param port: (I) GPIO端口，取值：@ref xwds_gpio_port_em中的一项
 * @param pinmask: (I) GPIO引脚，取值：@ref xwds_gpio_pin_em中的任意项的组合（或运算）
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效指针
 * @retval -ERANGE: GPIO PORT错误
 * @retval -EBUSY: GPIO PIN已被使用
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
__xwds_api
xwer_t xwds_gpio_req(struct xwds_soc * soc, xwid_t port, xwsq_t pinmask)
{
        const struct xwds_soc_driver * drv;
        xwer_t rc;

        XWDS_VALIDATE(soc, "nullptr", -EFAULT);
        XWDS_VALIDATE((port < soc->gpio.port_num), "out-of-range", -ERANGE);

        pinmask &= XWDS_GPIO_PIN_MASK(soc->gpio.pin_num);

        rc = xwds_soc_grab(soc);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_soc_grab;
        }

        rc = xwaop_t0ma_then_s1m(xwsq, &soc->gpio.pins[port], pinmask, NULL, NULL);
        if (__xwcc_unlikely(rc < 0)) {
                rc = -EBUSY;
                goto err_set_pin;
        }
        drv = xwds_cast(const struct xwds_soc_driver *, soc->dev.drv);
        if ((drv) && (drv->gpio_req)) {
                rc = drv->gpio_req(soc, port, pinmask);
                if (__xwcc_unlikely(rc < 0)) {
                        goto err_drv_gpio_req;
                }
        }
        return XWOK;

err_drv_gpio_req:
        xwaop_c0m(xwsq, &soc->gpio.pins[port], pinmask, NULL, NULL);
err_set_pin:
        xwds_soc_put(soc);
err_soc_grab:
        return rc;
}

/**
 * @brief XWDS API：释放GPIO
 * @param soc: (I) SOC对象指针
 * @param port: (I) GPIO端口，取值：@ref xwds_gpio_port_em中的一项
 * @param pinmask: (I) GPIO引脚，取值：@ref xwds_gpio_pin_em中的任意项的组合（或运算）
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效指针
 * @retval -ERANGE: GPIO PORT错误
 * @retval -EPERM: GPIO PIN未被使用
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
__xwds_api
xwer_t xwds_gpio_rls(struct xwds_soc * soc, xwid_t port, xwsq_t pinmask)
{
        const struct xwds_soc_driver * drv;
        xwsq_t pinsts;
        xwer_t rc;

        XWDS_VALIDATE(soc, "nullptr", -EFAULT);
        XWDS_VALIDATE((port < soc->gpio.port_num), "out-of-range", -ERANGE);

        pinmask &= XWDS_GPIO_PIN_MASK(soc->gpio.pin_num);
        pinsts = xwaop_load(xwsq, &soc->gpio.pins[port], xwmb_modr_relaxed);
        if (__xwcc_unlikely(pinmask & (~pinsts))) {
                rc = -EPERM;
                goto err_pinsts;
        }
        drv = xwds_cast(const struct xwds_soc_driver *, soc->dev.drv);
        if ((drv) && (drv->gpio_rls)) {
                rc = drv->gpio_rls(soc, port, pinmask);
                if (__xwcc_unlikely(rc < 0)) {
                        goto err_drv_gpio_rls;
                }
        }
        xwaop_c0m(xwsq, &soc->gpio.pins[port], pinmask, NULL, NULL);

        xwds_soc_put(soc);
        return XWOK;

err_drv_gpio_rls:
err_pinsts:
        return rc;
}

/**
 * @brief XWDS API：配置GPIO
 * @param soc: (I) SOC对象指针
 * @param port: (I) GPIO端口，取值：@ref xwds_gpio_port_em中的一项
 * @param pinmask: (I) GPIO引脚，取值：@ref xwds_gpio_pin_em中的任意项的组合（或运算）
 * @param cfg: (I) GPIO配置，取值：依据不同SOC
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效指针
 * @retval -ERANGE: GPIO PORT错误
 * @retval -ENOSYS: 不支持的API
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
__xwds_api
xwer_t xwds_gpio_cfg(struct xwds_soc * soc,
                     xwid_t port, xwsq_t pinmask,
                     void * cfg)
{
        const struct xwds_soc_driver * drv;
        xwer_t rc;

        XWDS_VALIDATE(soc, "nullptr", -EFAULT);
        XWDS_VALIDATE((port < soc->gpio.port_num), "out-of-range", -ERANGE);
        XWDS_VALIDATE(cfg, "nullptr", -EFAULT);

        pinmask &= XWDS_GPIO_PIN_MASK(soc->gpio.pin_num);

        rc = xwds_soc_grab(soc);
        if (__xwcc_unlikely(rc < 0)) {
               goto err_soc_grab;
        }

        drv = xwds_cast(const struct xwds_soc_driver *, soc->dev.drv);
        if ((drv) && (drv->gpio_cfg)) {
                rc = drv->gpio_cfg(soc, port, pinmask, cfg);
        } else {
                rc = -ENOSYS;
        }
        if (__xwcc_unlikely(rc < 0)) {
                goto err_drv_gpio_cfg;
        }

        xwds_soc_put(soc);
        return XWOK;

err_drv_gpio_cfg:
        xwds_soc_put(soc);
err_soc_grab:
        return rc;
}

/**
 * @brief XWDS API：将GPIO的PIN置为高电平
 * @param soc: (I) SOC对象指针
 * @param port: (I) GPIO端口，取值：@ref xwds_gpio_port_em中的一项
 * @param pinmask: (I) GPIO引脚，取值：@ref xwds_gpio_pin_em中的任意项的组合（或运算）
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效指针
 * @retval -ERANGE: GPIO PORT错误
 * @retval -EPERM: GPIO PIN未被使用
 * @retval -ENOSYS: 不支持的API
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
__xwds_api
xwer_t xwds_gpio_set(struct xwds_soc * soc, xwid_t port, xwsq_t pinmask)
{
        const struct xwds_soc_driver * drv;
        xwsq_t pinsts;
        xwer_t rc;

        XWDS_VALIDATE(soc, "nullptr", -EFAULT);
        XWDS_VALIDATE((port < soc->gpio.port_num), "out-of-range", -ERANGE);

        pinmask &= XWDS_GPIO_PIN_MASK(soc->gpio.pin_num);

        rc = xwds_soc_grab(soc);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_soc_grab;
        }

        pinsts = xwaop_load(xwsq, &soc->gpio.pins[port], xwmb_modr_relaxed);
        if (__xwcc_unlikely(pinmask & (~pinsts))) {
                rc = -EPERM;
                goto err_pinsts;
        }
        drv = xwds_cast(const struct xwds_soc_driver *, soc->dev.drv);
        if ((drv) && (drv->gpio_set)) {
                rc = drv->gpio_set(soc, port, pinmask);
        } else {
                rc = -ENOSYS;
        }
        if (__xwcc_unlikely(rc < 0)) {
                goto err_drv_set;
        }

        xwds_soc_put(soc);
        return XWOK;

err_drv_set:
err_pinsts:
        xwds_soc_put(soc);
err_soc_grab:
        return rc;
}

/**
 * @brief XWDS API：将GPIO的PIN置为低电平
 * @param soc: (I) SOC对象指针
 * @param port: (I) GPIO端口，取值：@ref xwds_gpio_port_em中的一项
 * @param pinmask: (I) GPIO引脚，取值：@ref xwds_gpio_pin_em中的任意项的组合（或运算）
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效指针
 * @retval -ERANGE: GPIO PORT错误
 * @retval -EPERM: GPIO PIN未被使用
 * @retval -ENOSYS: 不支持的API
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
__xwds_api
xwer_t xwds_gpio_reset(struct xwds_soc * soc, xwid_t port, xwsq_t pinmask)
{
        const struct xwds_soc_driver * drv;
        xwsq_t pinsts;
        xwer_t rc;

        XWDS_VALIDATE(soc, "nullptr", -EFAULT);
        XWDS_VALIDATE((port < soc->gpio.port_num), "out-of-range", -ERANGE);

        pinmask &= XWDS_GPIO_PIN_MASK(soc->gpio.pin_num);

        rc = xwds_soc_grab(soc);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_soc_grab;
        }

        pinsts = xwaop_load(xwsq, &soc->gpio.pins[port], xwmb_modr_relaxed);
        if (__xwcc_unlikely(pinmask & (~pinsts))) {
                rc = -EPERM;
                goto err_pinsts;
        }
        drv = xwds_cast(const struct xwds_soc_driver *, soc->dev.drv);
        if ((drv) && (drv->gpio_reset)) {
                rc = drv->gpio_reset(soc, port, pinmask);
        } else {
                rc = -ENOSYS;
        }
        if (__xwcc_unlikely(rc < 0)) {
                goto err_drv_reset;
        }

        xwds_soc_put(soc);
        return XWOK;

err_drv_reset:
err_pinsts:
        xwds_soc_put(soc);
err_soc_grab:
        return rc;
}

/**
 * @brief XWDS API：反转GPIO电平
 * @param soc: (I) SOC对象指针
 * @param port: (I) GPIO端口，取值：@ref xwds_gpio_port_em中的一项
 * @param pinmask: (I) GPIO引脚，取值：@ref xwds_gpio_pin_em中的任意项的组合（或运算）
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效指针
 * @retval -ERANGE: GPIO PORT错误
 * @retval -EPERM: GPIO PIN未被使用
 * @retval -ENOSYS: 不支持的API
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
__xwds_api
xwer_t xwds_gpio_toggle(struct xwds_soc * soc, xwid_t port, xwsq_t pinmask)
{
        const struct xwds_soc_driver * drv;
        xwsq_t pinsts;
        xwer_t rc;

        XWDS_VALIDATE(soc, "nullptr", -EFAULT);
        XWDS_VALIDATE((port < soc->gpio.port_num), "out-of-range", -ERANGE);

        pinmask &= XWDS_GPIO_PIN_MASK(soc->gpio.pin_num);

        rc = xwds_soc_grab(soc);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_soc_grab;
        }

        pinsts = xwaop_load(xwsq, &soc->gpio.pins[port], xwmb_modr_relaxed);
        if (__xwcc_unlikely(pinmask & (~pinsts))) {
                rc = -EPERM;
                goto err_pinsts;
        }
        drv = xwds_cast(const struct xwds_soc_driver *, soc->dev.drv);
        if ((drv) && (drv->gpio_toggle)) {
                rc = drv->gpio_toggle(soc, port, pinmask);
        } else {
                rc = -ENOSYS;
        }
        if (__xwcc_unlikely(rc < 0)) {
                goto err_drv_toggle;
        }

        xwds_soc_put(soc);
        return XWOK;

err_drv_toggle:
err_pinsts:
        xwds_soc_put(soc);
err_soc_grab:
        return rc;
}

/**
 * @brief XWDS API：输出GPIO
 * @param soc: (I) SOC对象指针
 * @param port: (I) GPIO端口，取值：@ref xwds_gpio_port_em中的一项
 * @param pinmask: (I) GPIO引脚，取值：@ref xwds_gpio_pin_em中的任意项的组合（或运算）
 * @param out: (I) 输出值，取值：@ref xwds_gpio_pin_em中的任意项的组合（或运算），
 *                 只有被pinmask掩码覆盖的部分有效，未覆盖的pin输出不会发生改变。
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效指针
 * @retval -ERANGE: GPIO PORT错误
 * @retval -EPERM: GPIO PIN未被使用
 * @retval -ENOSYS: 不支持的API
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
__xwds_api
xwer_t xwds_gpio_output(struct xwds_soc * soc,
                        xwid_t port, xwsq_t pinmask,
                        xwsq_t out)
{
        const struct xwds_soc_driver * drv;
        xwsq_t pinsts;
        xwer_t rc;

        XWDS_VALIDATE(soc, "nullptr", -EFAULT);
        XWDS_VALIDATE((port < soc->gpio.port_num), "out-of-range", -ERANGE);

        pinmask &= XWDS_GPIO_PIN_MASK(soc->gpio.pin_num);

        rc = xwds_soc_grab(soc);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_soc_grab;
        }

        pinsts = xwaop_load(xwsq, &soc->gpio.pins[port], xwmb_modr_relaxed);
        if (__xwcc_unlikely(pinmask & (~pinsts))) {
                rc = -EPERM;
                goto err_pinsts;
        }
        drv = xwds_cast(const struct xwds_soc_driver *, soc->dev.drv);
        if ((drv) && (drv->gpio_output)) {
                rc = drv->gpio_output(soc, port, pinmask, out);
        } else {
                rc = -ENOSYS;
        }
        if (__xwcc_unlikely(rc < 0)) {
                goto err_drv_out;
        }

        xwds_soc_put(soc);
        return XWOK;

err_drv_out:
err_pinsts:
        xwds_soc_put(soc);
err_soc_grab:
        return rc;
}

/**
 * @brief XWDS API：从GPIO端口读取输入值
 * @param soc: (I) SOC对象指针
 * @param port: (I) GPIO端口，取值：@ref xwds_gpio_port_em中的一项
 * @param pinmask: (I) GPIO引脚，取值：@ref xwds_gpio_pin_em中的任意项的组合（或运算）
 * @param inbuf: (O) 输入缓冲区
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效指针
 * @retval -ERANGE: GPIO PORT错误
 * @retval -EPERM: GPIO PIN未被使用
 * @retval -ENOSYS: 不支持的API
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
__xwds_api
xwer_t xwds_gpio_input(struct xwds_soc * soc,
                       xwid_t port, xwsq_t pinmask,
                       xwsq_t * inbuf)
{
        const struct xwds_soc_driver * drv;
        xwsq_t pinsts;
        xwer_t rc;

        XWDS_VALIDATE(soc, "nullptr", -EFAULT);
        XWDS_VALIDATE((port < soc->gpio.port_num), "out-of-range", -ERANGE);

        pinmask &= XWDS_GPIO_PIN_MASK(soc->gpio.pin_num);

        rc = xwds_soc_grab(soc);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_soc_grab;
        }

        pinsts = xwaop_load(xwsq, &soc->gpio.pins[port], xwmb_modr_relaxed);
        if (__xwcc_unlikely(pinmask & (~pinsts))) {
                rc = -EPERM;
                goto err_pinsts;
        }
        drv = xwds_cast(const struct xwds_soc_driver *, soc->dev.drv);
        if ((drv) && (drv->gpio_input)) {
                rc = drv->gpio_input(soc, port, pinmask, inbuf);
        } else {
                rc = -ENOSYS;
        }
        if (__xwcc_unlikely(rc < 0)) {
                goto err_drv_in;
        }

        xwds_soc_put(soc);
        return XWOK;

err_drv_in:
err_pinsts:
        xwds_soc_put(soc);
err_soc_grab:
        return rc;
}
