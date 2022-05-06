/**
 * @file
 * @brief 主模块：子线程
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

#include <xwos/mm/mempool/allocator.h>
#include <xwos/osal/thd.h>
#include <xwcd/ds/soc/gpio.h>
#include <bdl/standard.h>
#include <bm/stm32cube/mif.h>
#include <bm/main/thd.h>

#define LED_TASK_PRIORITY (XWOS_SKD_PRIORITY_RAISE(XWOS_SKD_PRIORITY_RT_MIN, 1))

#define MEMTST_TASK_PRIORITY (XWOS_SKD_PRIORITY_RAISE(XWOS_SKD_PRIORITY_RT_MIN, 3))

#define BLUE_LED_GPIO_PORT XWDS_GPIO_PORT_D
#define BLUE_LED_GPIO_PIN XWDS_GPIO_PIN_12

xwer_t led_task(void * arg);

xwer_t memtst_task(void * arg);

const struct xwos_thd_desc child_thd_desc[] = {
        [0] = {
                .attr = {
                        .name = "task.led",
                        .stack = NULL,
                        .stack_size = 4096,
                        .stack_guard_size = XWOS_STACK_GUARD_SIZE_DEFAULT,
                        .priority = LED_TASK_PRIORITY,
                        .detached = true,
                        .privileged = true,
                },
                .func = led_task,
                .arg = NULL,
        },
        [1] = {
                .attr = {
                        .name = "task.memtst",
                        .stack = NULL,
                        .stack_size = 2048,
                        .stack_guard_size = XWOS_STACK_GUARD_SIZE_DEFAULT,
                        .priority = MEMTST_TASK_PRIORITY,
                        .detached = true,
                        .privileged = true,
                },
                .func = memtst_task,
                .arg = NULL,
        },
};

xwos_thd_d child_thd[xw_array_size(child_thd_desc)];

xwer_t child_thd_start(void)
{
        xwer_t rc;
        xwsq_t i;

        for (i = 0; i < xw_array_size(child_thd_desc); i++) {
                rc = xwos_thd_create(&child_thd[i],
                                     &child_thd_desc[i].attr,
                                     child_thd_desc[i].func,
                                     child_thd_desc[i].arg);
                if (rc < 0) {
                        goto err_thd_create;
                }
        }
        return XWOK;

err_thd_create:
        BDL_BUG();
        return rc;
}

xwer_t led_task(void * arg)
{
        xwer_t rc;

        XWOS_UNUSED(arg);
        rc = xwds_gpio_req(&stm32cube_soc_cb,
                           BLUE_LED_GPIO_PORT, BLUE_LED_GPIO_PIN);
        if (rc < 0) {
                goto err_gpio_req;
        }

        while (!xwos_cthd_shld_stop()) {
                if (xwos_cthd_shld_frz()) {
                        rc = xwds_gpio_rls(&stm32cube_soc_cb,
                                           BLUE_LED_GPIO_PORT, BLUE_LED_GPIO_PIN);
                        rc = xwos_cthd_freeze();
                        rc = xwds_gpio_req(&stm32cube_soc_cb,
                                           BLUE_LED_GPIO_PORT, BLUE_LED_GPIO_PIN);
                }
                xwds_gpio_set(&stm32cube_soc_cb,
                              BLUE_LED_GPIO_PORT, BLUE_LED_GPIO_PIN);
                xwos_cthd_sleep(XWTM_S(1));
                xwds_gpio_reset(&stm32cube_soc_cb,
                                BLUE_LED_GPIO_PORT, BLUE_LED_GPIO_PIN);
                xwos_cthd_sleep(XWTM_S(1));
        }
        return XWOK;

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
        rc = XWOK;
        while ((!xwos_cthd_frz_shld_stop(NULL)) && (running)) {
                xwtm = XWTM_MS(2);
                xwos_cthd_sleep(xwtm);
                rc = xwmm_mempool_malloc(sdram_mempool, 8, &mem);
                xwtm = XWTM_MS(10);
                xwos_cthd_sleep(xwtm);
                rc = xwmm_mempool_free(sdram_mempool, mem);

                xwtm = XWTM_MS(3);
                xwos_cthd_sleep(xwtm);
                rc = xwmm_mempool_malloc(sdram_mempool, 16, &mem);
                xwtm = XWTM_MS(6);
                xwos_cthd_sleep(xwtm);
                rc = xwmm_mempool_free(sdram_mempool, mem);

                xwtm = XWTM_MS(8);
                xwos_cthd_sleep(xwtm);
                rc = xwmm_mempool_malloc(sdram_mempool, 32, &mem);
                xwtm = XWTM_MS(12);
                xwos_cthd_sleep(xwtm);
                rc = xwmm_mempool_free(sdram_mempool, mem);

                xwtm = XWTM_MS(4);
                xwos_cthd_sleep(xwtm);
                rc = xwmm_mempool_malloc(sdram_mempool, 64, &mem);
                xwtm = XWTM_MS(9);
                xwos_cthd_sleep(xwtm);
                rc = xwmm_mempool_free(sdram_mempool, mem);

                xwtm = XWTM_MS(15);
                xwos_cthd_sleep(xwtm);
                rc = xwmm_mempool_malloc(sdram_mempool, 128, &mem);
                xwtm = XWTM_MS(8);
                xwos_cthd_sleep(xwtm);
                rc = xwmm_mempool_free(sdram_mempool, mem);

                xwtm = XWTM_MS(15);
                xwos_cthd_sleep(xwtm);
                rc = xwmm_mempool_malloc(sdram_mempool, 192, &mem);
                xwtm = XWTM_MS(8);
                xwos_cthd_sleep(xwtm);
                rc = xwmm_mempool_free(sdram_mempool, mem);

                xwtm = XWTM_MS(18);
                xwos_cthd_sleep(xwtm);
                rc = xwmm_mempool_malloc(sdram_mempool, 256, &mem);
                xwtm = XWTM_MS(6);
                xwos_cthd_sleep(xwtm);
                rc = xwmm_mempool_free(sdram_mempool, mem);

                xwtm = XWTM_MS(15);
                xwos_cthd_sleep(xwtm);
                rc = xwmm_mempool_malloc(sdram_mempool, 384, &mem);
                xwtm = XWTM_MS(8);
                xwos_cthd_sleep(xwtm);
                rc = xwmm_mempool_free(sdram_mempool, mem);

                xwtm = XWTM_MS(1);
                xwos_cthd_sleep(xwtm);
                rc = xwmm_mempool_malloc(sdram_mempool, 512, &mem);
                xwtm = XWTM_MS(1);
                xwos_cthd_sleep(xwtm);
                rc = xwmm_mempool_free(sdram_mempool, mem);

                xwtm = XWTM_MS(15);
                xwos_cthd_sleep(xwtm);
                rc = xwmm_mempool_malloc(sdram_mempool, 768, &mem);
                xwtm = XWTM_MS(8);
                xwos_cthd_sleep(xwtm);
                rc = xwmm_mempool_free(sdram_mempool, mem);

                xwtm = XWTM_MS(3);
                xwos_cthd_sleep(xwtm);
                rc = xwmm_mempool_malloc(sdram_mempool, 1024, &mem);
                xwtm = XWTM_MS(11);
                xwos_cthd_sleep(xwtm);
                rc = xwmm_mempool_free(sdram_mempool, mem);

                xwtm = XWTM_MS(8);
                xwos_cthd_sleep(xwtm);
                rc = xwmm_mempool_malloc(sdram_mempool, 2048, &mem);
                xwtm = XWTM_MS(3);
                xwos_cthd_sleep(xwtm);
                rc = xwmm_mempool_free(sdram_mempool, mem);

                xwtm = XWTM_MS(7);
                xwos_cthd_sleep(xwtm);
                rc = xwmm_mempool_malloc(sdram_mempool, 4096, &mem);
                xwtm = XWTM_MS(5);
                xwos_cthd_sleep(xwtm);
                rc = xwmm_mempool_free(sdram_mempool, mem);

                xwtm = XWTM_MS(3);
                xwos_cthd_sleep(xwtm);
                rc = xwmm_mempool_malloc(sdram_mempool, 8192, &mem);
                xwtm = XWTM_MS(2);
                xwos_cthd_sleep(xwtm);
                rc = xwmm_mempool_free(sdram_mempool, mem);
        }
        return rc;
}
