/**
 * @file
 * @brief 板级描述层：设备栈
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

#include <xwos/standard.h>
#include <xwos/lib/xwlog.h>
#include <xwcd/ds/xwds.h>
#include <xwcd/ds/device.h>
#include <xwcd/ds/soc/chip.h>
#include <xwcd/ds/uart/general.h>
#include <xwcd/ds/misc/chip.h>
#include <xwcd/perpheral/i2c/eeprom/device.h>
#include <bdl/standard.h>
#include <bdl/ds/description/mpc560xbdkp.h>
#include <bdl/ds/device.h>

static
xwer_t bdl_xwds_soc_start(void);

static
xwer_t bdl_xwds_soc_stop(void);

/*
static
xwer_t bdl_xwds_uart_start(void);

static
xwer_t bdl_xwds_uart_stop(void);
*/

/**
 * @brief 设备栈
 */
struct xwds bdl_xwds;

/**
 * @brief 启动设备栈
 */
xwer_t bdl_xwds_start(void)
{
        xwer_t rc;


        xwds_init(&bdl_xwds);

        rc = bdl_xwds_soc_start();
        if (__xwcc_unlikely(rc < 0)) {
                goto err_soc_start;
        }

/*
        rc = bdl_xwds_uart_start();
        if (__xwcc_unlikely(rc < 0)) {
                goto err_uart_start;
        }
*/
        return XWOK;
/*
err_uart_start:
        BDL_BUG();
*/
err_soc_start:
        BDL_BUG();
        return rc;
}

/**
 * @brief 停止设备栈
 */
xwer_t bdl_xwds_stop(void)
{
        xwer_t rc;

/*
        rc = bdl_xwds_uart_stop();
        if (__xwcc_unlikely(rc < 0)) {
                goto err_uart_stop;
        }
*/
        rc = bdl_xwds_soc_stop();
        if (__xwcc_unlikely(rc < 0)) {
                goto err_soc_stop;
        }
        return XWOK;

err_soc_stop:
/*
err_uart_stop:
*/
        return rc;
}

/**
 * @brief 启动设备：SOC
 */
static
xwer_t bdl_xwds_soc_start(void)
{
        xwer_t rc;

        xwds_soc_construct(&mpc560xb_soc_cb);
        rc = xwds_device_probe(&bdl_xwds,
                               xwds_cast(struct xwds_device *, &mpc560xb_soc_cb),
                               NULL);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_soc_probe;
        }
        rc = xwds_device_start(xwds_cast(struct xwds_device *, &mpc560xb_soc_cb));
        if (__xwcc_unlikely(rc < 0)) {
                goto err_soc_start;
        }

        return XWOK;

err_soc_start:
        BDL_BUG();
        /* xwds_device_remove(xwds_cast(struct xwds_device *, &mpc560xb_soc_cb)); */
err_soc_probe:
        BDL_BUG();
        return rc;
}

/**
 * @brief 关闭设备：SOC
 */
static
xwer_t bdl_xwds_soc_stop(void)
{
        return -ENOSYS;
}

/**
 * @brief 启动设备：UART
 */
/*
static
xwer_t bdl_xwds_uart_start(void)
{
        xwer_t rc;

        xwds_dmauartc_construct(&mpc560xb_uart0_cb);
        rc = xwds_device_probe(&bdl_xwds,
                               xwds_cast(struct xwds_device *, &mpc560xb_uart0_cb),
                               NULL);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_uart0_probe;
        }
        rc = xwds_device_start(xwds_cast(struct xwds_device *, &mpc560xb_uart0_cb));
        if (__xwcc_unlikely(rc < 0)) {
                goto err_uart0_start;
        }
        return XWOK;

err_uart0_start:
        BDL_BUG();
        xwds_device_remove(xwds_cast(struct xwds_device *, &mpc560xb_uart0_cb));
err_uart0_probe:
        BDL_BUG();
        return rc;
}
*/

/**
 * @brief 关闭设备：UART
 */
/*
static
xwer_t bdl_xwds_uart_stop(void)
{
        return -ENOSYS;
}
*/
