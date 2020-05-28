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
#include <xwmd/ds/standard.h>
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
__xwds_api
xwer_t xwds_gpio_req(struct xwds_soc * soc, xwid_t port, xwsq_t pinmask)
{
        const struct xwds_soc_driver * drv;
        xwer_t rc;

        XWDS_VALIDATE(soc, "nullptr", -EFAULT);
        XWDS_VALIDATE((port < soc->gpio.port_num), "out-of-range", -ERANGE);

        pinmask &= XWDS_GPIO_PIN_MASK(soc->gpio.pin_num);

        rc = xwds_soc_grab(soc);
        if (__unlikely(rc < 0)) {
                goto err_soc_grab;
        }

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
        xwds_soc_put(soc);
err_soc_grab:
        return rc;
}

__xwds_api
xwer_t xwds_gpio_rls(struct xwds_soc * soc, xwid_t port, xwsq_t pinmask)
{
        const struct xwds_soc_driver * drv;
        xwsq_t pinsts;
        xwer_t rc;

        XWDS_VALIDATE(soc, "nullptr", -EFAULT);
        XWDS_VALIDATE((port < soc->gpio.port_num), "out-of-range", -ERANGE);

        pinmask &= XWDS_GPIO_PIN_MASK(soc->gpio.pin_num);
        pinsts = xwaop_load(xwsq_t, &soc->gpio.pins[port], xwmb_modr_relaxed);
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

        xwds_soc_put(soc);
        return OK;

err_drv_gpio_rls:
err_pinsts:
        return rc;
}

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
        if (__unlikely(rc < 0)) {
               goto err_soc_grab;
        }

        drv = xwds_static_cast(const struct xwds_soc_driver *, soc->dev.drv);
        if ((drv) && (drv->gpio_cfg)) {
                rc = drv->gpio_cfg(soc, port, pinmask, cfg);
        } else {
                rc = -ENOSYS;
        }
        if (__unlikely(rc < 0)) {
                goto err_drv_gpio_cfg;
        }

        xwds_soc_put(soc);
        return OK;

err_drv_gpio_cfg:
        xwds_soc_put(soc);
err_soc_grab:
        return rc;
}

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
        if (__unlikely(rc < 0)) {
                goto err_soc_grab;
        }

        pinsts = xwaop_load(xwsq_t, &soc->gpio.pins[port], xwmb_modr_relaxed);
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

        xwds_soc_put(soc);
        return OK;

err_drv_set:
err_pinsts:
        xwds_soc_put(soc);
err_soc_grab:
        return rc;
}

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
        if (__unlikely(rc < 0)) {
                goto err_soc_grab;
        }

        pinsts = xwaop_load(xwsq_t, &soc->gpio.pins[port], xwmb_modr_relaxed);
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

        xwds_soc_put(soc);
        return OK;

err_drv_reset:
err_pinsts:
        xwds_soc_put(soc);
err_soc_grab:
        return rc;
}

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
        if (__unlikely(rc < 0)) {
                goto err_soc_grab;
        }

        pinsts = xwaop_load(xwsq_t, &soc->gpio.pins[port], xwmb_modr_relaxed);
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

        xwds_soc_put(soc);
        return OK;

err_drv_toggle:
err_pinsts:
        xwds_soc_put(soc);
err_soc_grab:
        return rc;
}

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
        if (__unlikely(rc < 0)) {
                goto err_soc_grab;
        }

        pinsts = xwaop_load(xwsq_t, &soc->gpio.pins[port], xwmb_modr_relaxed);
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

        xwds_soc_put(soc);
        return OK;

err_drv_out:
err_pinsts:
        xwds_soc_put(soc);
err_soc_grab:
        return rc;
}

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
        if (__unlikely(rc < 0)) {
                goto err_soc_grab;
        }

        pinsts = xwaop_load(xwsq_t, &soc->gpio.pins[port], xwmb_modr_relaxed);
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

        xwds_soc_put(soc);
        return OK;

err_drv_in:
err_pinsts:
        xwds_soc_put(soc);
err_soc_grab:
        return rc;
}

/**
 * @brief XWDS LIB：通过描述得到GPIO资源
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
