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
#include <bdl/standard.h>
#include <bdl/ds/description/s32k14xdkp.h>
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
xwer_t bdl_xwds_soc_setup(struct xwds * ds);

static
xwer_t bdl_xwds_soc_clean(struct xwds * ds);

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
xwer_t bdl_xwds_init(struct xwds * ds)
{
        xwer_t rc;

        /* init device stack */
        xwds_init(ds);

        /* init xwmd root device: soc */
        rc = bdl_xwds_soc_setup(ds);
        if (__unlikely(rc < 0)) {
                goto err_soc_setup;
        }
        return OK;

err_soc_setup:
        return rc;
}

xwer_t bdl_xwds_exit(struct xwds * ds)
{
        xwer_t rc;

        rc = bdl_xwds_soc_clean(ds);
        if (__unlikely(rc < 0)) {
                goto err_soc_clean;
        }
        return OK;

err_soc_clean:
        return rc;
}

static
xwer_t bdl_xwds_soc_setup(struct xwds * ds)
{
        xwer_t rc;

        xwds_soc_construct(&s32k14x_soc_cb);
        rc = xwds_device_probe(ds,
                               xwds_static_cast(struct xwds_device *, &s32k14x_soc_cb),
                               NULL);
        if (__unlikely(rc < 0)) {
                goto err_dev_probe;
        }
        rc = xwds_device_start(xwds_static_cast(struct xwds_device *, &s32k14x_soc_cb));
        if (__unlikely(rc < 0)) {
                goto err_dev_start;
        }

        return OK;

err_dev_start:
        BDL_BUG();
err_dev_probe:
        BDL_BUG();
        return rc;
}

static
xwer_t bdl_xwds_soc_clean(struct xwds * ds)
{
        XWOS_UNUSED(ds);
        BDL_BUG();
        return -EBUG;
}
