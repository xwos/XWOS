/**
 * @file
 * @brief xwmd设备栈：SOC GPIO
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
#include <xwos/standard.h>
#include <xwos/lib/string.h>
#include <xwos/lib/xwaop.h>
#include <xwmd/ds/soc/gpio.h>

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
 * @brief SODS API：申请GPIO
 * @param soc: (I) SOC对象指针
 * @param port: (I) GPIO端口，取值：@ref xwds_gpio_port_em中的一项
 * @param pinmask: (I) GPIO引脚，取值：@ref xwds_gpio_pin_em中的任意项的组合（或运算）
 * @return 错误码
 * @retval OK: OK
 * @retval -EFAULT: 无效指针
 * @retval -ERANGE: GPIO PORT错误
 * @retval -EBUSY: GPIO PIN已被使用
 * @note
 * - 同步/异步：同步
 * - 中断上下文：可以使用
 * - 中断底半部：可以使用
 * - 线程上下文：可以使用
 * - 重入性：对于同一个GPIO不可重入；对于不同GPIO可重入
 */
__xwds_api
xwer_t xwds_gpio_req(struct xwds_soc * soc, xwid_t port, xwsq_t pinmask)
{
        const struct xwds_soc_driver * drv;
        xwer_t rc;

        SODS_VALIDATE(soc, "nullptr", -EFAULT);
        SODS_VALIDATE((port < soc->gpio.port_num), "out-of-range", -ERANGE);

        pinmask &= SODS_GPIO_PIN_MASK(soc->gpio.pin_num);

#if !defined(XWMDCFG_ds_NANO) || (1 != XWMDCFG_ds_NANO)
        rc = xwds_soc_grab(soc);
        if (__unlikely(rc < 0)) {
                goto err_soc_grab;
        }
#endif /* !XWMDCFG_ds_NANO */

        rc = xwaop_t0ma_then_s1m(xwsq_t, &soc->gpio.pins[port], pinmask, NULL, NULL);
        if (__unlikely(rc < 0)) {
                rc = -EBUSY;
                goto err_set_pin;
        }
        drv = xwds_static_cast(const struct xwds_soc_driver *, soc->dev.drv);
        if ((drv) && (drv->gpio_req)) {
                rc = drv->gpio_req(soc, port, pinmask);
                if (__unlikely(rc < 0)) {
                        goto err_drv_gpio_req;
                }
        }
        return OK;

err_drv_gpio_req:
        xwaop_c0m(xwsq_t, &soc->gpio.pins[port], pinmask, NULL, NULL);
err_set_pin:
#if !defined(XWMDCFG_ds_NANO) || (1 != XWMDCFG_ds_NANO)
        xwds_soc_put(soc);
err_soc_grab:
#endif /* !XWMDCFG_ds_NANO */
        return rc;
}

/**
 * @brief SODS API：释放GPIO
 * @param soc: (I) SOC对象指针
 * @param port: (I) GPIO端口，取值：@ref xwds_gpio_port_em中的一项
 * @param pinmask: (I) GPIO引脚，取值：@ref xwds_gpio_pin_em中的任意项的组合（或运算）
 * @return 错误码
 * @retval OK: OK
 * @retval -EFAULT: 无效指针
 * @retval -ERANGE: GPIO PORT错误
 * @retval -EPERM: GPIO PIN未被使用
 * @note
 * - 同步/异步：同步
 * - 中断上下文：可以使用
 * - 中断底半部：可以使用
 * - 线程上下文：可以使用
 * - 重入性：对于同一个GPIO不可重入；对于不同GPIO可重入
 */
__xwds_api
xwer_t xwds_gpio_rls(struct xwds_soc * soc, xwid_t port, xwsq_t pinmask)
{
        const struct xwds_soc_driver * drv;
        xwsq_t pinsts;
        xwer_t rc;

        SODS_VALIDATE(soc, "nullptr", -EFAULT);
        SODS_VALIDATE((port < soc->gpio.port_num), "out-of-range", -ERANGE);

        pinmask &= SODS_GPIO_PIN_MASK(soc->gpio.pin_num);
        pinsts = xwaop_load(xwsq_t, &soc->gpio.pins[port], xwmb_modr_consume);
        if (__unlikely(pinmask & (~pinsts))) {
                rc = -EPERM;
                goto err_pinsts;
        }
        drv = xwds_static_cast(const struct xwds_soc_driver *, soc->dev.drv);
        if ((drv) && (drv->gpio_rls)) {
                rc = drv->gpio_rls(soc, port, pinmask);
                if (__unlikely(rc < 0)) {
                        goto err_drv_gpio_rls;
                }
        }
        xwaop_c0m(xwsq_t, &soc->gpio.pins[port], pinmask, NULL, NULL);

#if !defined(XWMDCFG_ds_NANO) || (1 != XWMDCFG_ds_NANO)
        xwds_soc_put(soc);
#endif /* !XWMDCFG_ds_NANO */

        return OK;

err_drv_gpio_rls:
err_pinsts:
        return rc;
}

/**
 * @brief SODS API：配置GPIO
 * @param soc: (I) SOC对象指针
 * @param port: (I) GPIO端口，取值：@ref xwds_gpio_port_em中的一项
 * @param pinmask: (I) GPIO引脚，取值：@ref xwds_gpio_pin_em中的任意项的组合（或运算）
 * @param cfg: (I) GPIO配置，取值：依据不同SOC
 * @return 错误码
 * @retval OK: OK
 * @retval -EFAULT: 无效指针
 * @retval -ERANGE: GPIO PORT错误
 * @retval -ENOSYS: 不支持的API
 * @note
 * - 同步/异步：同步
 * - 中断上下文：可以使用
 * - 中断底半部：可以使用
 * - 线程上下文：可以使用
 * - 重入性：对于同一个GPIO不可重入；对于不同GPIO可重入
 */
__xwds_api
xwer_t xwds_gpio_cfg(struct xwds_soc * soc,
                     xwid_t port, xwsq_t pinmask,
                     void * cfg)
{
        const struct xwds_soc_driver * drv;
        xwer_t rc;

        SODS_VALIDATE(soc, "nullptr", -EFAULT);
        SODS_VALIDATE((port < soc->gpio.port_num), "out-of-range", -ERANGE);
        SODS_VALIDATE(cfg, "nullptr", -EFAULT);

        pinmask &= SODS_GPIO_PIN_MASK(soc->gpio.pin_num);

#if !defined(XWMDCFG_ds_NANO) || (1 != XWMDCFG_ds_NANO)
        rc = xwds_soc_grab(soc);
        if (__unlikely(rc < 0)) {
               goto err_soc_grab;
        }
#endif /* !XWMDCFG_ds_NANO */

        drv = xwds_static_cast(const struct xwds_soc_driver *, soc->dev.drv);
        if ((drv) && (drv->gpio_cfg)) {
                rc = drv->gpio_cfg(soc, port, pinmask, cfg);
        } else {
                rc = -ENOSYS;
        }
        if (__unlikely(rc < 0)) {
                goto err_drv_gpio_cfg;
        }

#if !defined(XWMDCFG_ds_NANO) || (1 != XWMDCFG_ds_NANO)
        xwds_soc_put(soc);
#endif /* !XWMDCFG_ds_NANO */

        return OK;

err_drv_gpio_cfg:
#if !defined(XWMDCFG_ds_NANO) || (1 != XWMDCFG_ds_NANO)
        xwds_soc_put(soc);
err_soc_grab:
#endif /* !XWMDCFG_ds_NANO */
        return rc;
}

/**
 * @brief SODS API：将GPIO的PIN置为高电平
 * @param soc: (I) SOC对象指针
 * @param port: (I) GPIO端口，取值：@ref xwds_gpio_port_em中的一项
 * @param pinmask: (I) GPIO引脚，取值：@ref xwds_gpio_pin_em中的任意项的组合（或运算）
 * @return 错误码
 * @retval OK: OK
 * @retval -EFAULT: 无效指针
 * @retval -ERANGE: GPIO PORT错误
 * @retval -EPERM: GPIO PIN未被使用
 * @retval -ENOSYS: 不支持的API
 * @note
 * - 同步/异步：同步
 * - 中断上下文：可以使用
 * - 中断底半部：可以使用
 * - 线程上下文：可以使用
 * - 重入性：对于同一个GPIO不可重入；对于不同GPIO可重入
 */
__xwds_api
xwer_t xwds_gpio_set(struct xwds_soc * soc, xwid_t port, xwsq_t pinmask)
{
        const struct xwds_soc_driver * drv;
        xwsq_t pinsts;
        xwer_t rc;

        SODS_VALIDATE(soc, "nullptr", -EFAULT);
        SODS_VALIDATE((port < soc->gpio.port_num), "out-of-range", -ERANGE);

        pinmask &= SODS_GPIO_PIN_MASK(soc->gpio.pin_num);

#if !defined(XWMDCFG_ds_NANO) || (1 != XWMDCFG_ds_NANO)
        rc = xwds_soc_grab(soc);
        if (__unlikely(rc < 0)) {
                goto err_soc_grab;
        }
#endif /* !XWMDCFG_ds_NANO */

        pinsts = xwaop_load(xwsq_t, &soc->gpio.pins[port], xwmb_modr_consume);
        if (__unlikely(pinmask & (~pinsts))) {
                rc = -EPERM;
                goto err_pinsts;
        }
        drv = xwds_static_cast(const struct xwds_soc_driver *, soc->dev.drv);
        if ((drv) && (drv->gpio_set)) {
                rc = drv->gpio_set(soc, port, pinmask);
        } else {
                rc = -ENOSYS;
        }
        if (__unlikely(rc < 0)) {
                goto err_drv_set;
        }

#if !defined(XWMDCFG_ds_NANO) || (1 != XWMDCFG_ds_NANO)
        xwds_soc_put(soc);
#endif /* !XWMDCFG_ds_NANO */

        return OK;

err_drv_set:
err_pinsts:
#if !defined(XWMDCFG_ds_NANO) || (1 != XWMDCFG_ds_NANO)
        xwds_soc_put(soc);
err_soc_grab:
#endif /* !XWMDCFG_ds_NANO */
        return rc;
}

/**
 * @brief SODS API：将GPIO的PIN置为低电平
 * @param soc: (I) SOC对象指针
 * @param port: (I) GPIO端口，取值：@ref xwds_gpio_port_em中的一项
 * @param pinmask: (I) GPIO引脚，取值：@ref xwds_gpio_pin_em中的任意项的组合（或运算）
 * @return 错误码
 * @retval OK: OK
 * @retval -EFAULT: 无效指针
 * @retval -ERANGE: GPIO PORT错误
 * @retval -EPERM: GPIO PIN未被使用
 * @retval -ENOSYS: 不支持的API
 * @note
 * - 同步/异步：同步
 * - 中断上下文：可以使用
 * - 中断底半部：可以使用
 * - 线程上下文：可以使用
 * - 重入性：对于同一个GPIO不可重入；对于不同GPIO可重入
 */
__xwds_api
xwer_t xwds_gpio_reset(struct xwds_soc * soc, xwid_t port, xwsq_t pinmask)
{
        const struct xwds_soc_driver * drv;
        xwsq_t pinsts;
        xwer_t rc;

        SODS_VALIDATE(soc, "nullptr", -EFAULT);
        SODS_VALIDATE((port < soc->gpio.port_num), "out-of-range", -ERANGE);

        pinmask &= SODS_GPIO_PIN_MASK(soc->gpio.pin_num);

#if !defined(XWMDCFG_ds_NANO) || (1 != XWMDCFG_ds_NANO)
        rc = xwds_soc_grab(soc);
        if (__unlikely(rc < 0)) {
                goto err_soc_grab;
        }
#endif /* !XWMDCFG_ds_NANO */

        pinsts = xwaop_load(xwsq_t, &soc->gpio.pins[port], xwmb_modr_consume);
        if (__unlikely(pinmask & (~pinsts))) {
                rc = -EPERM;
                goto err_pinsts;
        }
        drv = xwds_static_cast(const struct xwds_soc_driver *, soc->dev.drv);
        if ((drv) && (drv->gpio_reset)) {
                rc = drv->gpio_reset(soc, port, pinmask);
        } else {
                rc = -ENOSYS;
        }
        if (__unlikely(rc < 0)) {
                goto err_drv_reset;
        }

#if !defined(XWMDCFG_ds_NANO) || (1 != XWMDCFG_ds_NANO)
        xwds_soc_put(soc);
#endif /* !XWMDCFG_ds_NANO */

        return OK;

err_drv_reset:
err_pinsts:
#if !defined(XWMDCFG_ds_NANO) || (1 != XWMDCFG_ds_NANO)
        xwds_soc_put(soc);
err_soc_grab:
#endif /* !XWMDCFG_ds_NANO */
        return rc;
}

/**
 * @brief SODS API：反转GPIO电平
 * @param soc: (I) SOC对象指针
 * @param port: (I) GPIO端口，取值：@ref xwds_gpio_port_em中的一项
 * @param pinmask: (I) GPIO引脚，取值：@ref xwds_gpio_pin_em中的任意项的组合（或运算）
 * @return 错误码
 * @retval OK: OK
 * @retval -EFAULT: 无效指针
 * @retval -ERANGE: GPIO PORT错误
 * @retval -EPERM: GPIO PIN未被使用
 * @retval -ENOSYS: 不支持的API
 * @note
 * - 同步/异步：同步
 * - 中断上下文：可以使用
 * - 中断底半部：可以使用
 * - 线程上下文：可以使用
 * - 重入性：对于同一个GPIO不可重入；对于不同GPIO可重入
 */
__xwds_api
xwer_t xwds_gpio_toggle(struct xwds_soc * soc, xwid_t port, xwsq_t pinmask)
{
        const struct xwds_soc_driver * drv;
        xwsq_t pinsts;
        xwer_t rc;

        SODS_VALIDATE(soc, "nullptr", -EFAULT);
        SODS_VALIDATE((port < soc->gpio.port_num), "out-of-range", -ERANGE);

        pinmask &= SODS_GPIO_PIN_MASK(soc->gpio.pin_num);

#if !defined(XWMDCFG_ds_NANO) || (1 != XWMDCFG_ds_NANO)
        rc = xwds_soc_grab(soc);
        if (__unlikely(rc < 0)) {
                goto err_soc_grab;
        }
#endif /* !XWMDCFG_ds_NANO */

        pinsts = xwaop_load(xwsq_t, &soc->gpio.pins[port], xwmb_modr_consume);
        if (__unlikely(pinmask & (~pinsts))) {
                rc = -EPERM;
                goto err_pinsts;
        }
        drv = xwds_static_cast(const struct xwds_soc_driver *, soc->dev.drv);
        if ((drv) && (drv->gpio_toggle)) {
                rc = drv->gpio_toggle(soc, port, pinmask);
        } else {
                rc = -ENOSYS;
        }
        if (__unlikely(rc < 0)) {
                goto err_drv_toggle;
        }

#if !defined(XWMDCFG_ds_NANO) || (1 != XWMDCFG_ds_NANO)
        xwds_soc_put(soc);
#endif /* !XWMDCFG_ds_NANO */

        return OK;

err_drv_toggle:
err_pinsts:
#if !defined(XWMDCFG_ds_NANO) || (1 != XWMDCFG_ds_NANO)
        xwds_soc_put(soc);
err_soc_grab:
#endif /* !XWMDCFG_ds_NANO */
        return rc;
}

/**
 * @brief SODS API：输出GPIO
 * @param soc: (I) SOC对象指针
 * @param port: (I) GPIO端口，取值：@ref xwds_gpio_port_em中的一项
 * @param pinmask: (I) GPIO引脚，取值：@ref xwds_gpio_pin_em中的任意项的组合（或运算）
 * @param out: (I) 输出值，取值：@ref xwds_gpio_pin_em中的任意项的组合（或运算），
 *                 只有被pinmask掩码覆盖的部分有效，未覆盖的pin输出不会发生改变。
 * @return 错误码
 * @retval OK: OK
 * @retval -EFAULT: 无效指针
 * @retval -ERANGE: GPIO PORT错误
 * @retval -EPERM: GPIO PIN未被使用
 * @retval -ENOSYS: 不支持的API
 * @note
 * - 同步/异步：同步
 * - 中断上下文：可以使用
 * - 中断底半部：可以使用
 * - 线程上下文：可以使用
 * - 重入性：对于同一个GPIO不可重入；对于不同GPIO可重入
 */
__xwds_api
xwer_t xwds_gpio_output(struct xwds_soc * soc,
                        xwid_t port, xwsq_t pinmask,
                        xwsq_t out)
{
        const struct xwds_soc_driver * drv;
        xwsq_t pinsts;
        xwer_t rc;

        SODS_VALIDATE(soc, "nullptr", -EFAULT);
        SODS_VALIDATE((port < soc->gpio.port_num), "out-of-range", -ERANGE);

        pinmask &= SODS_GPIO_PIN_MASK(soc->gpio.pin_num);

#if !defined(XWMDCFG_ds_NANO) || (1 != XWMDCFG_ds_NANO)
        rc = xwds_soc_grab(soc);
        if (__unlikely(rc < 0)) {
                goto err_soc_grab;
        }
#endif /* !XWMDCFG_ds_NANO */

        pinsts = xwaop_load(xwsq_t, &soc->gpio.pins[port], xwmb_modr_consume);
        if (__unlikely(pinmask & (~pinsts))) {
                rc = -EPERM;
                goto err_pinsts;
        }
        drv = xwds_static_cast(const struct xwds_soc_driver *, soc->dev.drv);
        if ((drv) && (drv->gpio_output)) {
                rc = drv->gpio_output(soc, port, pinmask, out);
        } else {
                rc = -ENOSYS;
        }
        if (__unlikely(rc < 0)) {
                goto err_drv_out;
        }

#if !defined(XWMDCFG_ds_NANO) || (1 != XWMDCFG_ds_NANO)
        xwds_soc_put(soc);
#endif /* !XWMDCFG_ds_NANO */

        return OK;

err_drv_out:
err_pinsts:
#if !defined(XWMDCFG_ds_NANO) || (1 != XWMDCFG_ds_NANO)
        xwds_soc_put(soc);
err_soc_grab:
#endif /* !XWMDCFG_ds_NANO */
        return rc;
}

/**
 * @brief SODS API：从GPIO端口读取输入值
 * @param soc: (I) SOC对象指针
 * @param port: (I) GPIO端口，取值：@ref xwds_gpio_port_em中的一项
 * @param pinmask: (I) GPIO引脚，取值：@ref xwds_gpio_pin_em中的任意项的组合（或运算）
 * @param inbuf: (O) 输入缓冲区
 * @return 错误码
 * @retval OK: OK
 * @retval -EFAULT: 无效指针
 * @retval -ERANGE: GPIO PORT错误
 * @retval -EPERM: GPIO PIN未被使用
 * @retval -ENOSYS: 不支持的API
 * @note
 * - 同步/异步：同步
 * - 中断上下文：可以使用
 * - 中断底半部：可以使用
 * - 线程上下文：可以使用
 * - 重入性：对于同一个GPIO不可重入；对于不同GPIO可重入
 */
__xwds_api
xwer_t xwds_gpio_input(struct xwds_soc * soc,
                       xwid_t port, xwsq_t pinmask,
                       xwsq_t * inbuf)
{
        const struct xwds_soc_driver * drv;
        xwsq_t pinsts;
        xwer_t rc;

        SODS_VALIDATE(soc, "nullptr", -EFAULT);
        SODS_VALIDATE((port < soc->gpio.port_num), "out-of-range", -ERANGE);

        pinmask &= SODS_GPIO_PIN_MASK(soc->gpio.pin_num);

#if !defined(XWMDCFG_ds_NANO) || (1 != XWMDCFG_ds_NANO)
        rc = xwds_soc_grab(soc);
        if (__unlikely(rc < 0)) {
                goto err_soc_grab;
        }
#endif /* !XWMDCFG_ds_NANO */

        pinsts = xwaop_load(xwsq_t, &soc->gpio.pins[port], xwmb_modr_consume);
        if (__unlikely(pinmask & (~pinsts))) {
                rc = -EPERM;
                goto err_pinsts;
        }
        drv = xwds_static_cast(const struct xwds_soc_driver *, soc->dev.drv);
        if ((drv) && (drv->gpio_input)) {
                rc = drv->gpio_input(soc, port, pinmask, inbuf);
        } else {
                rc = -ENOSYS;
        }
        if (__unlikely(rc < 0)) {
                goto err_drv_in;
        }

#if !defined(XWMDCFG_ds_NANO) || (1 != XWMDCFG_ds_NANO)
        xwds_soc_put(soc);
#endif /* !XWMDCFG_ds_NANO */

        return OK;

err_drv_in:
err_pinsts:
#if !defined(XWMDCFG_ds_NANO) || (1 != XWMDCFG_ds_NANO)
        xwds_soc_put(soc);
err_soc_grab:
#endif /* !XWMDCFG_ds_NANO */
        return rc;
}

/**
 * @brief SODS LIB：通过描述得到GPIO资源
 * @param base: (I) GPIO资源数组的基地址
 * @param num: (I) GPIO资源数量
 * @param descay: (I) 寄存器描述数组
 * @param descnum: (I) 寄存器描述数组数量
 * @param ret: (O) 返回GPIO资源地址的缓存
 * @return 错误码
 * @retval OK: OK
 * @retval -EFAULT: 无效指针
 * @retval -ENOKEY: 找不到描述的资源
 */
__xwds_lib_code
xwer_t xwds_get_gpiorsc(const struct xwds_resource_gpio base[], xwsz_t num,
                        const char * descay[], xwsz_t descnum,
                        const struct xwds_resource_gpio ** ret)
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
