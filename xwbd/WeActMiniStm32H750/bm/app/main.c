/**
 * @file
 * @brief 板级描述层：用户程序：应用程序入口
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

#include "board/std.h"
#include <xwos/lib/xwlog.h>
#include <xwos/osal/thd.h>
#include <xwcd/perpheral/spi/lcd/st7735/driver.h>
#ifdef XWCFG_LIBC__newlib
#  include <xwmd/libc/newlibac/mif.h>
#endif
#ifdef XWCFG_LIBC__picolibc
#  include <xwmd/libc/picolibcac/mif.h>
#endif
#include <xwem/vm/lua/mif.h>
#include <xwam/example/cxx/mif.h>
#include "bm/xwac/xwds/device.h"
#include "bm/stm32cube/mif.h"
#include "bm/app/xwssc.h"
#include "bm/app/thd.h"

#define MAIN_THD_PRIORITY XWOS_SKD_PRIORITY_DROP(XWOS_SKD_PRIORITY_RT_MAX, 0)

xwer_t main_task(void * arg);

xwos_thd_d main_thd;

xwer_t xwos_main(void)
{
        xwer_t rc;
        struct xwos_thd_attr attr;

        xwos_thd_attr_init(&attr);
        attr.name = "main.thd";
        attr.stack = NULL;
        attr.stack_size = 8192;
        attr.priority = MAIN_THD_PRIORITY;
        attr.detached = true;
        attr.privileged = true;
        rc = xwos_thd_create(&main_thd,
                             &attr,
                             main_task,
                             NULL);
        if (rc < 0) {
                goto err_thd_create;
        }

        rc = xwos_skd_start_lc();
        if (rc < 0) {
                goto err_skd_start_lc;
        }
        return XWOK;

err_skd_start_lc:
        BOARD_BUG();
err_thd_create:
        BOARD_BUG();
        return rc;
}

extern const xwu8_t bootlogo[25600];
extern void xwrust_main(void);

xwer_t main_task(void * arg)
{
        xwer_t rc;

        XWOS_UNUSED(arg);

        rc = stm32cube_start();
        if (rc < 0) {
                goto err_stm32cube_start;
        }

        xwds_st7735_draw(&st7735, 0, 0, 160, 80, bootlogo, XWTM_MAX);

#ifdef XWCFG_LIBC__newlib
        rc = newlibac_init();
        if (rc < 0) {
                xwlogf(ERR, "main", "Init newlib ... <rc:%d>", rc);
                goto err_newlibac_init;
        }
#endif

#ifdef XWCFG_LIBC__picolibc
        rc = picolibcac_init();
        if (rc < 0) {
                xwlogf(ERR, "main", "Init picolibc ... <rc:%d>", rc);
                goto err_picolibcac_init;
        }
#endif

        rc = xwos_example_cxx();
        if (rc < 0) {
                xwlogf(ERR, "main", "Start C++ example ... <rc:%d>", rc);
        }

        rc = child_thd_start();
        if (rc < 0) {
                xwlogf(ERR, "main", "Start child threads ... <rc:%d>", rc);
                goto err_child_thd_start;
        }

        rc = board_xwssc_start();
        if (rc < 0) {
                xwlogf(ERR, "main", "Start XWSSC ... <rc:%d>", rc);
                goto err_xwssc_start;
        }

#if defined(XWEMCFG_vm_lua) && (1 == XWEMCFG_vm_lua)
        rc = xwlua_start();
        if (rc < 0) {
                xwlogf(ERR, "main", "Start lua VM ... <rc:%d>", rc);
                goto err_xwlua_start;
        }
#endif

        xwrust_main();

        return XWOK;

#if defined(XWEMCFG_vm_lua) && (1 == XWEMCFG_vm_lua)
err_xwlua_start:
        BOARD_BUG();
#endif
err_xwssc_start:
        BOARD_BUG();
err_child_thd_start:

#ifdef XWCFG_LIBC__picolibc
        BOARD_BUG();
err_picolibcac_init:
#endif
#ifdef XWCFG_LIBC__newlib
        BOARD_BUG();
err_newlibac_init:
#endif
        BOARD_BUG();
err_stm32cube_start:
        BOARD_BUG();
        return rc;
}
