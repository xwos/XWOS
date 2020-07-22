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
#include <bm/stm32cube/standard.h>
#include <xwos/mm/mempool/allocator.h>
#include <xwos/osal/scheduler.h>
#include <xwos/osal/thread.h>
#include <xwmd/ds/soc/gpio.h>
#include <xwmd/ds/uart/dma.h>
#include <bm/stm32cube/cubemx/Core/Inc/main.h>
#include <bm/stm32cube/cubemx/Core/Inc/isr.h>
#include <bm/stm32cube/xwac/xwds/stm32cube.h>
#include <bm/stm32cube/xwmo.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define LED_TASK_PRIORITY \
        (XWOSAL_SD_PRIORITY_RAISE(XWOSAL_SD_PRIORITY_RT_MIN, 1))

#define MEMTST_TASK_PRIORITY \
        (XWOSAL_SD_PRIORITY_RAISE(XWOSAL_SD_PRIORITY_RT_MIN, 1))

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         function prototypes         ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
extern
xwer_t sdcard_fatfs_mount(void);

xwer_t led_task(void * arg);

xwer_t memtst_task(void * arg);

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief 连接占位符
 * @note
 * + 确保链接时使用此符号的文件。
 */
void * const stm32cube_linkage_placeholder[] = {
        stm32cube_override_linkage_msp,
        stm32cube_override_linkage_it,
};

const struct xwosal_thrd_desc stm32cube_tbd[] = {
        [0] = {
                .name = "task.led",
                .prio = LED_TASK_PRIORITY,
                .stack = NULL,
                .stack_size = 2048,
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

extern struct xwmm_mempool * sdram_mempool;

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
xwer_t bm_stm32cube_start(void)
{
        xwer_t rc;
        xwsq_t i;

        rc = sdcard_fatfs_mount();
        if (rc < 0) {
                goto err_fatfs_mount;
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
        return XWOK;

err_thrd_create:
        BDL_BUG();
err_fatfs_mount:
        BDL_BUG();
        return rc;
}

xwer_t led_task(void * arg)
{
        xwtm_t xwtm;

        XWOS_UNUSED(arg);

        xwds_gpio_req(&stm32cube_soc_cb,
                      XWDS_GPIO_PORT_B, XWDS_GPIO_PIN_0);
        while (!xwosal_cthrd_shld_stop()) {
                if (xwosal_cthrd_shld_frz()) {
                        xwosal_cthrd_freeze();
                }
                xwtm = 1 * XWTM_S;
                xwosal_cthrd_sleep(&xwtm);
                xwds_gpio_toggle(&stm32cube_soc_cb,
                                 XWDS_GPIO_PORT_B,
                                 XWDS_GPIO_PIN_0);
        }
        return XWOK;
}

xwer_t memtst_task(void * arg)
{
        xwer_t rc;
        void * mem;
        xwtm_t xwtm;
        __xw_io bool running = true;

        XWOS_UNUSED(arg);
        rc = XWOK;
        while ((!xwosal_cthrd_frz_shld_stop(NULL)) && (running)) {
                xwtm = 2 * XWTM_MS;
                xwosal_cthrd_sleep(&xwtm);
                rc = xwmm_mempool_malloc(sdram_mempool, 8, &mem);
                xwtm = 10 * XWTM_MS;
                xwosal_cthrd_sleep(&xwtm);
                rc = xwmm_mempool_free(sdram_mempool, mem);

                xwtm = 3 * XWTM_MS;
                xwosal_cthrd_sleep(&xwtm);
                rc = xwmm_mempool_malloc(sdram_mempool, 16, &mem);
                xwtm = 6 * XWTM_MS;
                xwosal_cthrd_sleep(&xwtm);
                rc = xwmm_mempool_free(sdram_mempool, mem);

                xwtm = 8 * XWTM_MS;
                xwosal_cthrd_sleep(&xwtm);
                rc = xwmm_mempool_malloc(sdram_mempool, 32, &mem);
                xwtm = 12 * XWTM_MS;
                xwosal_cthrd_sleep(&xwtm);
                rc = xwmm_mempool_free(sdram_mempool, mem);

                xwtm = 4 * XWTM_MS;
                xwosal_cthrd_sleep(&xwtm);
                rc = xwmm_mempool_malloc(sdram_mempool, 64, &mem);
                xwtm = 9 * XWTM_MS;
                xwosal_cthrd_sleep(&xwtm);
                rc = xwmm_mempool_free(sdram_mempool, mem);

                xwtm = 15 * XWTM_MS;
                xwosal_cthrd_sleep(&xwtm);
                rc = xwmm_mempool_malloc(sdram_mempool, 128, &mem);
                xwtm = 8 * XWTM_MS;
                xwosal_cthrd_sleep(&xwtm);
                rc = xwmm_mempool_free(sdram_mempool, mem);

                xwtm = 15 * XWTM_MS;
                xwosal_cthrd_sleep(&xwtm);
                rc = xwmm_mempool_malloc(sdram_mempool, 192, &mem);
                xwtm = 8 * XWTM_MS;
                xwosal_cthrd_sleep(&xwtm);
                rc = xwmm_mempool_free(sdram_mempool, mem);

                xwtm = 18 * XWTM_MS;
                xwosal_cthrd_sleep(&xwtm);
                rc = xwmm_mempool_malloc(sdram_mempool, 256, &mem);
                xwtm = 6 * XWTM_MS;
                xwosal_cthrd_sleep(&xwtm);
                rc = xwmm_mempool_free(sdram_mempool, mem);

                xwtm = 15 * XWTM_MS;
                xwosal_cthrd_sleep(&xwtm);
                rc = xwmm_mempool_malloc(sdram_mempool, 384, &mem);
                xwtm = 8 * XWTM_MS;
                xwosal_cthrd_sleep(&xwtm);
                rc = xwmm_mempool_free(sdram_mempool, mem);

                xwtm = 1 * XWTM_MS;
                xwosal_cthrd_sleep(&xwtm);
                rc = xwmm_mempool_malloc(sdram_mempool, 512, &mem);
                xwtm = 1 * XWTM_MS;
                xwosal_cthrd_sleep(&xwtm);
                rc = xwmm_mempool_free(sdram_mempool, mem);

                xwtm = 15 * XWTM_MS;
                xwosal_cthrd_sleep(&xwtm);
                rc = xwmm_mempool_malloc(sdram_mempool, 768, &mem);
                xwtm = 8 * XWTM_MS;
                xwosal_cthrd_sleep(&xwtm);
                rc = xwmm_mempool_free(sdram_mempool, mem);

                xwtm = 3 * XWTM_MS;
                xwosal_cthrd_sleep(&xwtm);
                rc = xwmm_mempool_malloc(sdram_mempool, 1024, &mem);
                xwtm = 11 * XWTM_MS;
                xwosal_cthrd_sleep(&xwtm);
                rc = xwmm_mempool_free(sdram_mempool, mem);

                xwtm = 8 * XWTM_MS;
                xwosal_cthrd_sleep(&xwtm);
                rc = xwmm_mempool_malloc(sdram_mempool, 2048, &mem);
                xwtm = 3 * XWTM_MS;
                xwosal_cthrd_sleep(&xwtm);
                rc = xwmm_mempool_free(sdram_mempool, mem);

                xwtm = 7 * XWTM_MS;
                xwosal_cthrd_sleep(&xwtm);
                rc = xwmm_mempool_malloc(sdram_mempool, 4096, &mem);
                xwtm = 5 * XWTM_MS;
                xwosal_cthrd_sleep(&xwtm);
                rc = xwmm_mempool_free(sdram_mempool, mem);

                xwtm = 3 * XWTM_MS;
                xwosal_cthrd_sleep(&xwtm);
                rc = xwmm_mempool_malloc(sdram_mempool, 8192, &mem);
                xwtm = 2 * XWTM_MS;
                xwosal_cthrd_sleep(&xwtm);
                rc = xwmm_mempool_free(sdram_mempool, mem);
        }

        return rc;
}
