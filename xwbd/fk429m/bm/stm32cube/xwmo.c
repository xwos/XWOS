/**
 * @file
 * @brief Board Module: STM32CUBE
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

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <stdio.h>
#include <stdlib.h>
#include <xwos/lib/xwlog.h>
#include <bm/stm32cube/standard.h>
#include <xwos/mm/mempool/allocator.h>
#include <xwos/osal/scheduler.h>
#include <xwos/osal/thread.h>
#include <xwmd/ds/soc/gpio.h>
#include <xwem/fs/fatfs/ff.h>
#include "cubemx/Core/Inc/sdio.h"
#include "cubemx/override.h"
#include "xwac/xwds/stm32cube.h"
#include "xwac/xwds/init.h"
#include "xwmo.h"

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define LED_TASK_PRIORITY \
        (XWOSAL_SD_PRIORITY_RAISE(XWOSAL_SD_PRIORITY_RT_MIN, 1))

#define MEMTST_TASK_PRIORITY \
        (XWOSAL_SD_PRIORITY_RAISE(XWOSAL_SD_PRIORITY_RT_MIN, 3))

#define BLUE_LED_GPIO_PORT XWDS_GPIO_PORT_D
#define BLUE_LED_GPIO_PIN XWDS_GPIO_PIN_12

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         function prototypes         ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
xwer_t led_task(void * arg);

xwer_t memtst_task(void * arg);

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief 连接占位符
 * @note
 * + 连接时，若符号存在多重定义，优先选择包含占位符的文件里面的符号。
 */
void * const stm32cube_linkage_placeholder[] = {
        stm32cube_override_linkage_placeholder,
};

const struct xwosal_thrd_desc stm32cube_tbd[] = {
        [0] = {
                .name = "task.led",
                .prio = LED_TASK_PRIORITY,
                .stack = NULL,
                .stack_size = 4096,
                .func = led_task,
                .arg = NULL,
                .attr = XWSDOBJ_ATTR_PRIVILEGED,
        },
        [1] = {
                .name = "task.memtst",
                .prio = MEMTST_TASK_PRIORITY,
                .stack = NULL,
                .stack_size = 2048,
                .func = memtst_task,
                .arg = NULL,
                .attr = XWSDOBJ_ATTR_PRIVILEGED,
        },
};

xwid_t stm32cube_tid[xw_array_size(stm32cube_tbd)];

extern struct xwmm_mempool * eram_mempool;

FATFS fatfs_sdcard;

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
xwer_t bm_stm32cube_start(void)
{
        FRESULT frc;
        xwer_t rc;
        xwsq_t i;

        MX_SDIO_SD_Construct();
        MX_SDIO_SD_Init();
        rc = MX_SDIO_SD_TrimClk(100);
        if (rc < 0) {
                goto err_sd;
        }
        frc = f_mount(&fatfs_sdcard, "sd:", 1);
        if (FR_OK != frc) {
                goto err_sd;
        }

        /* start devices */
        rc = stm32cube_xwds_uart_start();
        if (rc < 0) {
                goto err_xwds_uart_start;
        }

        /* start thread */
        for (i = 0; i < xw_array_size(stm32cube_tbd); i++) {
                rc = xwosal_thrd_create(&stm32cube_tid[i], stm32cube_tbd[i].name,
                                        stm32cube_tbd[i].func,
                                        stm32cube_tbd[i].arg,
                                        stm32cube_tbd[i].stack_size,
                                        stm32cube_tbd[i].prio,
                                        stm32cube_tbd[i].attr);
                if (rc < 0) {
                        goto err_thrd_create;
                }
        }

        return OK;

err_thrd_create:
        stm32cube_xwds_uart_stop();
        BDL_BUG();
err_xwds_uart_start:
        BDL_BUG();
err_sd:
        BDL_BUG();
        return rc;
}

xwer_t led_task(void * arg)
{
        xwtm_t xwtm;
        xwer_t rc;

        XWOS_UNUSED(arg);
        rc = xwds_gpio_req(&stm32cube_soc_cb,
                           BLUE_LED_GPIO_PORT, BLUE_LED_GPIO_PIN);
        if (rc < 0) {
                goto err_gpio_req;
        }

        while (!xwosal_cthrd_shld_stop()) {
                if (xwosal_cthrd_shld_frz()) {
                        rc = xwds_gpio_rls(&stm32cube_soc_cb,
                                           BLUE_LED_GPIO_PORT, BLUE_LED_GPIO_PIN);
                        rc = xwosal_cthrd_freeze();
                        rc = xwds_gpio_req(&stm32cube_soc_cb,
                                           BLUE_LED_GPIO_PORT, BLUE_LED_GPIO_PIN);
                }
                xwds_gpio_set(&stm32cube_soc_cb,
                              BLUE_LED_GPIO_PORT, BLUE_LED_GPIO_PIN);
                xwtm = 1 * XWTM_S;
                xwosal_cthrd_sleep(&xwtm);
                xwds_gpio_reset(&stm32cube_soc_cb,
                                BLUE_LED_GPIO_PORT, BLUE_LED_GPIO_PIN);
                xwtm = 1 * XWTM_S;
                xwosal_cthrd_sleep(&xwtm);
        }
        return OK;

err_gpio_req:
        return rc;
}

xwer_t memtst_task(void * arg)
{
        xwer_t rc;
        void * mem;
        xwtm_t xwtm;
        __xw_io bool running = true;

        XWOS_UNUSED(arg);
        rc = OK;
        while ((!xwosal_cthrd_frz_shld_stop(NULL)) && (running)) {
                xwtm = 2 * XWTM_MS;
                xwosal_cthrd_sleep(&xwtm);
                rc = xwmm_mempool_malloc(eram_mempool, 8, &mem);
                xwtm = 10 * XWTM_MS;
                xwosal_cthrd_sleep(&xwtm);
                rc = xwmm_mempool_free(eram_mempool, mem);

                xwtm = 3 * XWTM_MS;
                xwosal_cthrd_sleep(&xwtm);
                rc = xwmm_mempool_malloc(eram_mempool, 16, &mem);
                xwtm = 6 * XWTM_MS;
                xwosal_cthrd_sleep(&xwtm);
                rc = xwmm_mempool_free(eram_mempool, mem);

                xwtm = 8 * XWTM_MS;
                xwosal_cthrd_sleep(&xwtm);
                rc = xwmm_mempool_malloc(eram_mempool, 32, &mem);
                xwtm = 12 * XWTM_MS;
                xwosal_cthrd_sleep(&xwtm);
                rc = xwmm_mempool_free(eram_mempool, mem);

                xwtm = 4 * XWTM_MS;
                xwosal_cthrd_sleep(&xwtm);
                rc = xwmm_mempool_malloc(eram_mempool, 64, &mem);
                xwtm = 9 * XWTM_MS;
                xwosal_cthrd_sleep(&xwtm);
                rc = xwmm_mempool_free(eram_mempool, mem);

                xwtm = 15 * XWTM_MS;
                xwosal_cthrd_sleep(&xwtm);
                rc = xwmm_mempool_malloc(eram_mempool, 128, &mem);
                xwtm = 8 * XWTM_MS;
                xwosal_cthrd_sleep(&xwtm);
                rc = xwmm_mempool_free(eram_mempool, mem);

                xwtm = 15 * XWTM_MS;
                xwosal_cthrd_sleep(&xwtm);
                rc = xwmm_mempool_malloc(eram_mempool, 192, &mem);
                xwtm = 8 * XWTM_MS;
                xwosal_cthrd_sleep(&xwtm);
                rc = xwmm_mempool_free(eram_mempool, mem);

                xwtm = 18 * XWTM_MS;
                xwosal_cthrd_sleep(&xwtm);
                rc = xwmm_mempool_malloc(eram_mempool, 256, &mem);
                xwtm = 6 * XWTM_MS;
                xwosal_cthrd_sleep(&xwtm);
                rc = xwmm_mempool_free(eram_mempool, mem);

                xwtm = 15 * XWTM_MS;
                xwosal_cthrd_sleep(&xwtm);
                rc = xwmm_mempool_malloc(eram_mempool, 384, &mem);
                xwtm = 8 * XWTM_MS;
                xwosal_cthrd_sleep(&xwtm);
                rc = xwmm_mempool_free(eram_mempool, mem);

                xwtm = 1 * XWTM_MS;
                xwosal_cthrd_sleep(&xwtm);
                rc = xwmm_mempool_malloc(eram_mempool, 512, &mem);
                xwtm = 1 * XWTM_MS;
                xwosal_cthrd_sleep(&xwtm);
                rc = xwmm_mempool_free(eram_mempool, mem);

                xwtm = 15 * XWTM_MS;
                xwosal_cthrd_sleep(&xwtm);
                rc = xwmm_mempool_malloc(eram_mempool, 768, &mem);
                xwtm = 8 * XWTM_MS;
                xwosal_cthrd_sleep(&xwtm);
                rc = xwmm_mempool_free(eram_mempool, mem);

                xwtm = 3 * XWTM_MS;
                xwosal_cthrd_sleep(&xwtm);
                rc = xwmm_mempool_malloc(eram_mempool, 1024, &mem);
                xwtm = 11 * XWTM_MS;
                xwosal_cthrd_sleep(&xwtm);
                rc = xwmm_mempool_free(eram_mempool, mem);

                xwtm = 8 * XWTM_MS;
                xwosal_cthrd_sleep(&xwtm);
                rc = xwmm_mempool_malloc(eram_mempool, 2048, &mem);
                xwtm = 3 * XWTM_MS;
                xwosal_cthrd_sleep(&xwtm);
                rc = xwmm_mempool_free(eram_mempool, mem);

                xwtm = 7 * XWTM_MS;
                xwosal_cthrd_sleep(&xwtm);
                rc = xwmm_mempool_malloc(eram_mempool, 4096, &mem);
                xwtm = 5 * XWTM_MS;
                xwosal_cthrd_sleep(&xwtm);
                rc = xwmm_mempool_free(eram_mempool, mem);

                xwtm = 3 * XWTM_MS;
                xwosal_cthrd_sleep(&xwtm);
                rc = xwmm_mempool_malloc(eram_mempool, 8192, &mem);
                xwtm = 2 * XWTM_MS;
                xwosal_cthrd_sleep(&xwtm);
                rc = xwmm_mempool_free(eram_mempool, mem);
        }

        return rc;
}
