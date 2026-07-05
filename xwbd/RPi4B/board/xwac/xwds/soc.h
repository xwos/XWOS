/**
 * @file
 * @brief RPi4B::XWDS::SOC
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

#ifndef __board_xwac_xwds_soc_h__
#define __board_xwac_xwds_soc_h__

#include <board/std.h>
#include <xwcd/soc/arm64/v8a/a72/bcm2711/soc_dma.h>

/**
 * @brief BCM2711 GPIO引脚配置
 */
struct rpi4bxwds_gpio_cfg {
        xwu32_t function; /**< 功能选择: SOC_GPIO_ALT_GPI / GPO / ALT0~5 */
        xwu32_t pud; /**< 上下拉: SOC_GPIO_PUPDC_NO / PU / PD */
};

/**
 * @brief BCM2711 DMA通道配置
 */
struct __xwcc_aligned(32) rpi4bxwds_dma_cfg {
        union {
                struct soc_dma_cb dma; /**< 普通DMA (Ch 0~6) */
                struct soc_dma_lite_cb lite; /**< DMA Lite (Ch 7~10) */
                struct soc_dma4_cb dma4; /**< DMA4 (Ch 11~14) */
        } cb; /**< 控制块 */
        xwu32_t cs_priority; /**< AXI优先级 (0~15) */
        xwu32_t cs_panic_priority; /**< AXI panic优先级 (0~15) */
        xwu32_t cs_disdebug; /**< 禁用debug暂停: 0=正常, 1=禁用 */
        xwu32_t cs_wait_for_outstanding_writes; /**< 等待写响应: 0=不等, 1=等 */
};

#endif /* board/xwac/xwds/soc.h */
