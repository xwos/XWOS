/**
 * @file
 * @brief 板级描述层：设备栈
 * @author
 * + 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <www.starsoul.tech>
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

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <xwos/lib/xwlog.h>
#include <xwmd/ds/device.h>
#include <xwmd/ds/soc/chip.h>
#include <xwmd/ds/uart/general.h>
#include <xwmd/ds/i2c/master.h>
#include <xwmd/ds/i2c/perpheral.h>
#include <xwmd/ds/misc/chip.h>
#include <xwcd/perpheral/ds/i2c/eeprom/common/driver.h>
#include <bdl/standard.h>
#include <bdl/ds/description/mpc560xbdkp.h>
#include <bdl/ds/xwds.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********     static function prototypes      ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
static
xwer_t bdl_xwds_soc_start(void);

static
xwer_t bdl_xwds_soc_stop(void);

static
xwer_t bdl_xwds_uart_start(void);

static
xwer_t bdl_xwds_uart_stop(void);

static
xwer_t bdl_xwds_i2cm_start(void);

static
xwer_t bdl_xwds_i2cm_stop(void);

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief 安装设备栈
 */
xwer_t bdl_xwds_start(void)
{
        xwer_t rc;

        rc = bdl_xwds_soc_start();
        if (__unlikely(rc < 0)) {
                goto err_soc_start;
        }

        rc = bdl_xwds_uart_start();
        if (__unlikely(rc < 0)) {
                goto err_uart_start;
        }

        rc = bdl_xwds_i2cm_start();
        if (__unlikely(rc < 0)) {
                goto err_i2cm_start;
        }
        return OK;

err_i2cm_start:
        BDL_BUG();
err_uart_start:
        BDL_BUG();
err_soc_start:
        BDL_BUG();
        return rc;
}

/**
 * @brief 清理设备栈
 */
xwer_t bdl_xwds_stop(void)
{
        xwer_t rc;

        rc = bdl_xwds_i2cm_stop();
        if (__unlikely(rc < 0)) {
                goto err_i2cm_stop;
        }
        rc = bdl_xwds_uart_stop();
        if (__unlikely(rc < 0)) {
                goto err_uart_stop;
        }
        rc = bdl_xwds_soc_stop();
        if (__unlikely(rc < 0)) {
                goto err_soc_stop;
        }
        return OK;

err_soc_stop:
err_uart_stop:
err_i2cm_stop:
        return rc;
}

/**
 * @brief 启动设备：SOC
 */
static
xwer_t bdl_xwds_soc_start(void)
{
        xwer_t rc;

        rc = xwds_soc_probe(&mpc560xb_soc_cb);
        if (__unlikely(rc < 0)) {
                goto err_soc_probe;
        }
        rc = xwds_soc_start(&mpc560xb_soc_cb);
        if (__unlikely(rc < 0)) {
                goto err_soc_start;
        }

        return OK;

err_soc_start:
        BDL_BUG();
        /* xwds_soc_remove(&mpc560xb_soc_cb); */
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
static
xwer_t bdl_xwds_uart_start(void)
{
        xwer_t rc;

        rc = xwds_dmauartc_probe(&mpc560xb_uart0_cb);
        if (__unlikely(rc < 0)) {
                goto err_uart0_probe;
        }
        rc = xwds_dmauartc_start(&mpc560xb_uart0_cb);
        if (__unlikely(rc < 0)) {
                goto err_uart0_start;
        }
        return OK;

err_uart0_start:
        BDL_BUG();
        /* xwds_dmauartc_remove(&mpc560xb_uart0_cb); */
err_uart0_probe:
        BDL_BUG();
        return rc;
}

/**
 * @brief 关闭设备：UART
 */
static
xwer_t bdl_xwds_uart_stop(void)
{
        return -ENOSYS;
}

/**
 * @brief 启动设备：I2C主机
 */
static
xwer_t bdl_xwds_i2cm_start(void)
{
        xwer_t rc;

        rc = xwds_i2cm_probe(&mpc560xb_i2cm_cb);
        if (__unlikely(rc < 0)) {
                goto err_i2cm_probe;
        }
        rc = xwds_i2cm_start(&mpc560xb_i2cm_cb);
        if (__unlikely(rc < 0)) {
                goto err_i2cm_start;
        }

        return OK;

err_i2cm_start:
        BDL_BUG();
        /* xwds_i2cm_remove(&mpc560xb_i2cm_cb); */
err_i2cm_probe:
        BDL_BUG();
        return rc;
}

/**
 * @brief 关闭设备：I2C主机
 */
static
xwer_t bdl_xwds_i2cm_stop(void)
{
        return -ENOSYS;
}

/**
 * @brief 启动设备：EEPROM
 */
xwer_t bdl_xwds_eeprom_start(void)
{
        xwer_t rc;

        rc = xwds_i2cp_probe(&at24cxx_cb.i2cp);
        if (__unlikely(rc < 0)) {
                goto err_eeprom_probe;
        }
        rc = xwds_i2cp_start(&at24cxx_cb.i2cp);
        if (__unlikely(rc < 0)) {
                goto err_eeprom_start;
        }

err_eeprom_start:
        BDL_BUG();
        /* xwds_i2cp_remove(&at24cxx_cb.i2cp); */
err_eeprom_probe:
        BDL_BUG();
        return rc;
}

/**
 * @brief 关闭设备：EEPROM
 */
xwer_t bdl_xwds_eeprom_stop(void)
{
        BDL_BUG();
        return -ENOSYS;
}

/**
 * @brief 启动设备：MISC
 */
xwer_t bdl_xwds_misc_start(void)
{
        xwer_t rc;

        rc = xwds_misc_probe(&mpc560xb_misc_cb);
        if (__unlikely(rc < 0)) {
                goto err_misc_probe;
        }
        rc = xwds_misc_start(&mpc560xb_misc_cb);
        if (__unlikely(rc < 0)) {
                goto err_misc_start;
        }

        return OK;

err_misc_start:
        BDL_BUG();
        /* xwds_misc_remove(&mpc560xb_misc_cb); */
err_misc_probe:
        BDL_BUG();
        return rc;
}

/**
 * @brief 关闭设备：MISC
 */
xwer_t bdl_xwds_misc_stop(void)
{
        BDL_BUG();
        return -ENOSYS;
}

/**
 * @brief 启动设备：RTC
 */
xwer_t bdl_xwds_rtc_start(void)
{
        xwer_t rc;

        rc = xwds_misc_probe(&mpc560xb_rtc_cb);
        if (__unlikely(rc < 0)) {
                goto err_rtc_probe;
        }
        rc = xwds_misc_start(&mpc560xb_rtc_cb);
        if (__unlikely(rc < 0)) {
                goto err_rtc_start;
        }

        return OK;

err_rtc_start:
        BDL_BUG();
        /* xwds_rtc_remove(&mpc560xb_rtc_cb); */
err_rtc_probe:
        BDL_BUG();
        return rc;
}

/**
 * @brief 关闭设备：RTC
 */
xwer_t bdl_xwds_rtc_stop(void)
{
        BDL_BUG();
        return -ENOSYS;
}
