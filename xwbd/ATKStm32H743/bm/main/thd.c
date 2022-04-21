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

#include <xwos/standard.h>
#include <xwos/mm/mempool/allocator.h>
#include <xwos/osal/thd.h>
#include <xwcd/ds/soc/gpio.h>
#include <xwcd/perpheral/i2c/eeprom/driver.h>
#include <bdl/standard.h>
#include <bm/stm32cube/mif.h>
#include <bm/main/thd.h>

#define LED_TASK_PRIORITY (XWOS_SKD_PRIORITY_RAISE(XWOS_SKD_PRIORITY_RT_MIN, 1))

#define MEMTST_TASK_PRIORITY (XWOS_SKD_PRIORITY_RAISE(XWOS_SKD_PRIORITY_RT_MIN, 1))

xwer_t led_task(void * arg);

xwer_t memtst_task(void * arg);

const struct xwos_thd_desc child_thd_desc[] = {
        [0] = {
                .attr = {
                        .name = "led.thd",
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
                        .name = "memtst.thd",
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

xwer_t eeprom_task(void)
{
        xwer_t rc;
        xwtm_t to;
        xwsz_t size;
        xwu8_t eedata[8] = {0};

        to = xwtm_ft(1 * XWTM_S);
        rc = xwds_eeprom_reset(&stm32cube_at24c02_cb, to);
        if (rc < 0) {
                goto err_eeprom_reset;
        }

        size = 8;
        to = xwtm_ft(1 * XWTM_S);
        rc = xwds_eeprom_pgread(&stm32cube_at24c02_cb, eedata, &size, 0, to);
        if (rc < 0) {
                goto err_eeprom_read;
        }

        size = 8;
        to = xwtm_ft(1 * XWTM_S);
        rc = xwds_eeprom_pgread(&stm32cube_at24c02_cb, eedata, &size, 1, to);
        if (rc < 0) {
                goto err_eeprom_read;
        }
        return XWOK;

err_eeprom_read:
err_eeprom_reset:
        return rc;
}

xwer_t led_task(void * arg)
{
        xwer_t rc;

        XWOS_UNUSED(arg);

        rc = eeprom_task();
        if (rc < 0) {
                BDL_BUG();
        }

        xwds_gpio_req(&stm32cube_soc_cb,
                      XWDS_GPIO_PORT_B, XWDS_GPIO_PIN_0);
        while (!xwos_cthd_shld_stop()) {
                if (xwos_cthd_shld_frz()) {
                        xwos_cthd_freeze();
                }
                xwos_cthd_sleep(1 * XWTM_S);
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
        while ((!xwos_cthd_frz_shld_stop(NULL)) && (running)) {
                xwtm = 2 * XWTM_MS;
                xwos_cthd_sleep(xwtm);
                rc = xwmm_mempool_malloc(sdram_mempool, 8, &mem);
                xwtm = 10 * XWTM_MS;
                xwos_cthd_sleep(xwtm);
                rc = xwmm_mempool_free(sdram_mempool, mem);

                xwtm = 3 * XWTM_MS;
                xwos_cthd_sleep(xwtm);
                rc = xwmm_mempool_malloc(sdram_mempool, 16, &mem);
                xwtm = 6 * XWTM_MS;
                xwos_cthd_sleep(xwtm);
                rc = xwmm_mempool_free(sdram_mempool, mem);

                xwtm = 8 * XWTM_MS;
                xwos_cthd_sleep(xwtm);
                rc = xwmm_mempool_malloc(sdram_mempool, 32, &mem);
                xwtm = 12 * XWTM_MS;
                xwos_cthd_sleep(xwtm);
                rc = xwmm_mempool_free(sdram_mempool, mem);

                xwtm = 4 * XWTM_MS;
                xwos_cthd_sleep(xwtm);
                rc = xwmm_mempool_malloc(sdram_mempool, 64, &mem);
                xwtm = 9 * XWTM_MS;
                xwos_cthd_sleep(xwtm);
                rc = xwmm_mempool_free(sdram_mempool, mem);

                xwtm = 15 * XWTM_MS;
                xwos_cthd_sleep(xwtm);
                rc = xwmm_mempool_malloc(sdram_mempool, 128, &mem);
                xwtm = 8 * XWTM_MS;
                xwos_cthd_sleep(xwtm);
                rc = xwmm_mempool_free(sdram_mempool, mem);

                xwtm = 15 * XWTM_MS;
                xwos_cthd_sleep(xwtm);
                rc = xwmm_mempool_malloc(sdram_mempool, 192, &mem);
                xwtm = 8 * XWTM_MS;
                xwos_cthd_sleep(xwtm);
                rc = xwmm_mempool_free(sdram_mempool, mem);

                xwtm = 18 * XWTM_MS;
                xwos_cthd_sleep(xwtm);
                rc = xwmm_mempool_malloc(sdram_mempool, 256, &mem);
                xwtm = 6 * XWTM_MS;
                xwos_cthd_sleep(xwtm);
                rc = xwmm_mempool_free(sdram_mempool, mem);

                xwtm = 15 * XWTM_MS;
                xwos_cthd_sleep(xwtm);
                rc = xwmm_mempool_malloc(sdram_mempool, 384, &mem);
                xwtm = 8 * XWTM_MS;
                xwos_cthd_sleep(xwtm);
                rc = xwmm_mempool_free(sdram_mempool, mem);

                xwtm = 1 * XWTM_MS;
                xwos_cthd_sleep(xwtm);
                rc = xwmm_mempool_malloc(sdram_mempool, 512, &mem);
                xwtm = 1 * XWTM_MS;
                xwos_cthd_sleep(xwtm);
                rc = xwmm_mempool_free(sdram_mempool, mem);

                xwtm = 15 * XWTM_MS;
                xwos_cthd_sleep(xwtm);
                rc = xwmm_mempool_malloc(sdram_mempool, 768, &mem);
                xwtm = 8 * XWTM_MS;
                xwos_cthd_sleep(xwtm);
                rc = xwmm_mempool_free(sdram_mempool, mem);

                xwtm = 3 * XWTM_MS;
                xwos_cthd_sleep(xwtm);
                rc = xwmm_mempool_malloc(sdram_mempool, 1024, &mem);
                xwtm = 11 * XWTM_MS;
                xwos_cthd_sleep(xwtm);
                rc = xwmm_mempool_free(sdram_mempool, mem);

                xwtm = 8 * XWTM_MS;
                xwos_cthd_sleep(xwtm);
                rc = xwmm_mempool_malloc(sdram_mempool, 2048, &mem);
                xwtm = 3 * XWTM_MS;
                xwos_cthd_sleep(xwtm);
                rc = xwmm_mempool_free(sdram_mempool, mem);

                xwtm = 7 * XWTM_MS;
                xwos_cthd_sleep(xwtm);
                rc = xwmm_mempool_malloc(sdram_mempool, 4096, &mem);
                xwtm = 5 * XWTM_MS;
                xwos_cthd_sleep(xwtm);
                rc = xwmm_mempool_free(sdram_mempool, mem);

                xwtm = 3 * XWTM_MS;
                xwos_cthd_sleep(xwtm);
                rc = xwmm_mempool_malloc(sdram_mempool, 8192, &mem);
                xwtm = 2 * XWTM_MS;
                xwos_cthd_sleep(xwtm);
                rc = xwmm_mempool_free(sdram_mempool, mem);
        }
        return rc;
}
