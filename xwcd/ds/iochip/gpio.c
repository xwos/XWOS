/**
 * @file
 * @brief 玄武设备栈：IO扩展芯片：GPIO
 * @author
 * + 隐星曜 (Roy Sun) <xwos@xwos.tech>
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
#include <xwos/lib/xwaop.h>
#include <xwcd/ds/iochip/gpio.h>

__xwds_api
xwer_t xwds_iochip_gpio_req(struct xwds_iochip * iochip,
                            xwid_t port, xwsq_t pinmask,
                            xwtm_t to)
{
        const struct xwds_iochip_driver * drv;
        xwer_t rc;

        XWDS_VALIDATE(iochip, "nullptr", -EFAULT);
        XWDS_VALIDATE((port < iochip->gpio.port_num), "out-of-range", -ERANGE);

        pinmask &= XWDS_IOCHIP_GPIO_PIN_MASK(iochip->gpio.pin_num);
        rc = xwds_iochip_grab(iochip);
        if (rc < 0) {
                goto err_iochip_grab;
        }
        rc = xwaop_t0ma_then_s1m(xwsq_t, &iochip->gpio.pins[port], pinmask, NULL, NULL);
        if (rc < 0) {
                rc = -EBUSY;
                goto err_set_pin;
        }
        drv = xwds_cast(const struct xwds_iochip_driver *, iochip->bc.dev.drv);
        if ((drv) && (drv->gpio_req)) {
                rc = drv->gpio_req(iochip, port, pinmask, to);
                if (rc < 0) {
                        goto err_drv_gpio_req;
                }
        }
        return XWOK;

err_drv_gpio_req:
        xwaop_c0m(xwsq_t, &iochip->gpio.pins[port], pinmask, NULL, NULL);
err_set_pin:
        xwds_iochip_put(iochip);
err_iochip_grab:
        return rc;
}

__xwds_api
xwer_t xwds_iochip_gpio_rls(struct xwds_iochip * iochip,
                            xwid_t port, xwsq_t pinmask,
                            xwtm_t to)
{
        const struct xwds_iochip_driver * drv;
        xwsq_t pinsts;
        xwer_t rc;

        XWDS_VALIDATE(iochip, "nullptr", -EFAULT);
        XWDS_VALIDATE((port < iochip->gpio.port_num), "out-of-range", -ERANGE);

        pinmask &= XWDS_IOCHIP_GPIO_PIN_MASK(iochip->gpio.pin_num);
        pinsts = xwaop_load(xwsq_t, &iochip->gpio.pins[port], xwaop_mo_relaxed);
        if (pinmask & (~pinsts)) {
                rc = -EPERM;
                goto err_pinsts;
        }
        drv = xwds_cast(const struct xwds_iochip_driver *, iochip->bc.dev.drv);
        if ((drv) && (drv->gpio_rls)) {
                rc = drv->gpio_rls(iochip, port, pinmask, to);
                if (rc < 0) {
                        goto err_drv_gpio_rls;
                }
        }
        xwaop_c0m(xwsq_t, &iochip->gpio.pins[port], pinmask, NULL, NULL);

        xwds_iochip_put(iochip);
        return XWOK;

err_drv_gpio_rls:
err_pinsts:
        return rc;
}

__xwds_api
xwer_t xwds_iochip_gpio_cfg(struct xwds_iochip * iochip,
                            xwid_t port, xwsq_t pinmask, void * cfg,
                            xwtm_t to)
{
        const struct xwds_iochip_driver * drv;
        xwer_t rc;

        XWDS_VALIDATE(iochip, "nullptr", -EFAULT);
        XWDS_VALIDATE((port < iochip->gpio.port_num), "out-of-range", -ERANGE);
        XWDS_VALIDATE(cfg, "nullptr", -EFAULT);

        pinmask &= XWDS_IOCHIP_GPIO_PIN_MASK(iochip->gpio.pin_num);
        rc = xwds_iochip_grab(iochip);
        if (rc < 0) {
               goto err_iochip_grab;
        }
        drv = xwds_cast(const struct xwds_iochip_driver *, iochip->bc.dev.drv);
        if ((drv) && (drv->gpio_cfg)) {
                rc = drv->gpio_cfg(iochip, port, pinmask, cfg, to);
        } else {
                rc = -ENOSYS;
        }
        if (rc < 0) {
                goto err_drv_gpio_cfg;
        }
        xwds_iochip_put(iochip);
        return XWOK;

err_drv_gpio_cfg:
        xwds_iochip_put(iochip);
err_iochip_grab:
        return rc;
}

__xwds_api
xwer_t xwds_iochip_gpio_set(struct xwds_iochip * iochip,
                            xwid_t port, xwsq_t pinmask,
                            xwtm_t to)
{
        const struct xwds_iochip_driver * drv;
        xwsq_t pinsts;
        xwer_t rc;

        XWDS_VALIDATE(iochip, "nullptr", -EFAULT);
        XWDS_VALIDATE((port < iochip->gpio.port_num), "out-of-range", -ERANGE);

        pinmask &= XWDS_IOCHIP_GPIO_PIN_MASK(iochip->gpio.pin_num);
        rc = xwds_iochip_grab(iochip);
        if (rc < 0) {
                goto err_iochip_grab;
        }
        pinsts = xwaop_load(xwsq_t, &iochip->gpio.pins[port], xwaop_mo_relaxed);
        if (pinmask & (~pinsts)) {
                rc = -EPERM;
                goto err_pinsts;
        }
        drv = xwds_cast(const struct xwds_iochip_driver *, iochip->bc.dev.drv);
        if ((drv) && (drv->gpio_set)) {
                rc = drv->gpio_set(iochip, port, pinmask, to);
        } else {
                rc = -ENOSYS;
        }
        if (rc < 0) {
                goto err_drv_set;
        }
        xwds_iochip_put(iochip);
        return XWOK;

err_drv_set:
err_pinsts:
        xwds_iochip_put(iochip);
err_iochip_grab:
        return rc;
}

__xwds_api
xwer_t xwds_iochip_gpio_reset(struct xwds_iochip * iochip,
                              xwid_t port, xwsq_t pinmask,
                              xwtm_t to)
{
        const struct xwds_iochip_driver * drv;
        xwsq_t pinsts;
        xwer_t rc;

        XWDS_VALIDATE(iochip, "nullptr", -EFAULT);
        XWDS_VALIDATE((port < iochip->gpio.port_num), "out-of-range", -ERANGE);

        pinmask &= XWDS_IOCHIP_GPIO_PIN_MASK(iochip->gpio.pin_num);
        rc = xwds_iochip_grab(iochip);
        if (rc < 0) {
                goto err_iochip_grab;
        }
        pinsts = xwaop_load(xwsq_t, &iochip->gpio.pins[port], xwaop_mo_relaxed);
        if (pinmask & (~pinsts)) {
                rc = -EPERM;
                goto err_pinsts;
        }
        drv = xwds_cast(const struct xwds_iochip_driver *, iochip->bc.dev.drv);
        if ((drv) && (drv->gpio_reset)) {
                rc = drv->gpio_reset(iochip, port, pinmask, to);
        } else {
                rc = -ENOSYS;
        }
        if (rc < 0) {
                goto err_drv_reset;
        }
        xwds_iochip_put(iochip);
        return XWOK;

err_drv_reset:
err_pinsts:
        xwds_iochip_put(iochip);
err_iochip_grab:
        return rc;
}

__xwds_api
xwer_t xwds_iochip_gpio_toggle(struct xwds_iochip * iochip,
                               xwid_t port, xwsq_t pinmask,
                               xwtm_t to)
{
        const struct xwds_iochip_driver * drv;
        xwsq_t pinsts;
        xwer_t rc;

        XWDS_VALIDATE(iochip, "nullptr", -EFAULT);
        XWDS_VALIDATE((port < iochip->gpio.port_num), "out-of-range", -ERANGE);

        pinmask &= XWDS_IOCHIP_GPIO_PIN_MASK(iochip->gpio.pin_num);
        rc = xwds_iochip_grab(iochip);
        if (rc < 0) {
                goto err_iochip_grab;
        }
        pinsts = xwaop_load(xwsq_t, &iochip->gpio.pins[port], xwaop_mo_relaxed);
        if (pinmask & (~pinsts)) {
                rc = -EPERM;
                goto err_pinsts;
        }
        drv = xwds_cast(const struct xwds_iochip_driver *, iochip->bc.dev.drv);
        if ((drv) && (drv->gpio_toggle)) {
                rc = drv->gpio_toggle(iochip, port, pinmask, to);
        } else {
                rc = -ENOSYS;
        }
        if (rc < 0) {
                goto err_drv_toggle;
        }
        xwds_iochip_put(iochip);
        return XWOK;

err_drv_toggle:
err_pinsts:
        xwds_iochip_put(iochip);
err_iochip_grab:
        return rc;
}

__xwds_api
xwer_t xwds_iochip_gpio_output(struct xwds_iochip * iochip,
                               xwid_t port, xwsq_t pinmask, xwsq_t out,
                               xwtm_t to)
{
        const struct xwds_iochip_driver * drv;
        xwsq_t pinsts;
        xwer_t rc;

        XWDS_VALIDATE(iochip, "nullptr", -EFAULT);
        XWDS_VALIDATE((port < iochip->gpio.port_num), "out-of-range", -ERANGE);

        pinmask &= XWDS_IOCHIP_GPIO_PIN_MASK(iochip->gpio.pin_num);
        rc = xwds_iochip_grab(iochip);
        if (rc < 0) {
                goto err_iochip_grab;
        }
        pinsts = xwaop_load(xwsq_t, &iochip->gpio.pins[port], xwaop_mo_relaxed);
        if (pinmask & (~pinsts)) {
                rc = -EPERM;
                goto err_pinsts;
        }
        drv = xwds_cast(const struct xwds_iochip_driver *, iochip->bc.dev.drv);
        if ((drv) && (drv->gpio_output)) {
                rc = drv->gpio_output(iochip, port, pinmask, out, to);
        } else {
                rc = -ENOSYS;
        }
        if (rc < 0) {
                goto err_drv_out;
        }
        xwds_iochip_put(iochip);
        return XWOK;

err_drv_out:
err_pinsts:
        xwds_iochip_put(iochip);
err_iochip_grab:
        return rc;
}

__xwds_api
xwer_t xwds_iochip_gpio_input(struct xwds_iochip * iochip,
                              xwid_t port, xwsq_t pinmask, xwsq_t * inbuf,
                              xwtm_t to)
{
        const struct xwds_iochip_driver * drv;
        xwsq_t pinsts;
        xwer_t rc;

        XWDS_VALIDATE(iochip, "nullptr", -EFAULT);
        XWDS_VALIDATE((port < iochip->gpio.port_num), "out-of-range", -ERANGE);

        pinmask &= XWDS_IOCHIP_GPIO_PIN_MASK(iochip->gpio.pin_num);
        rc = xwds_iochip_grab(iochip);
        if (rc < 0) {
                goto err_iochip_grab;
        }
        pinsts = xwaop_load(xwsq_t, &iochip->gpio.pins[port], xwaop_mo_relaxed);
        if (pinmask & (~pinsts)) {
                rc = -EPERM;
                goto err_pinsts;
        }
        drv = xwds_cast(const struct xwds_iochip_driver *, iochip->bc.dev.drv);
        if ((drv) && (drv->gpio_input)) {
                rc = drv->gpio_input(iochip, port, pinmask, inbuf, to);
        } else {
                rc = -ENOSYS;
        }
        if (rc < 0) {
                goto err_drv_in;
        }
        xwds_iochip_put(iochip);
        return XWOK;

err_drv_in:
err_pinsts:
        xwds_iochip_put(iochip);
err_iochip_grab:
        return rc;
}
