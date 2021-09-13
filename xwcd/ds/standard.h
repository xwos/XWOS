/**
 * @file
 * @brief 玄武设备栈：顶级头文件
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

#ifndef __xwcd_ds_standard_h__
#define __xwcd_ds_standard_h__

#include <xwos/standard.h>

#define __xwds_code             __xwcd_code
#define __xwds_isr              __xwcd_isr
#define __xwds_api              __xwcd_api
#define __xwds_inline           __xwcd_inline
#define __xwds_inline_api       __xwcd_inline_api
#define __xwds_vop              __xwcd_code
#define __xwds_data             __xwcd_data
#define __xwds_rodata           __xwcd_rodata

#define xwds_cast(type, dev)    ((type)(dev))

#define XWDS_BUG()              XWOS_BUG()
#define XWDS_BUG_ON(x)          XWOS_BUG_ON(x)

#if (defined(XWCDCFG_CHECK_PARAMETERS) && (1 == XWCDCFG_CHECK_PARAMETERS))
#  define XWDS_VALIDATE(exp, errstr, ...)       \
        if (__xwcc_unlikely(!(exp))) {          \
            return __VA_ARGS__;                 \
        }
#else
#  define XWDS_VALIDATE(exp, errstr, ...)
#endif

#define XWDS_LOG_TAG "xwds"

/**
 * @brief 设备栈寄存器资源
 */
struct xwds_resource_reg {
        void * base; /**< 基地址 */
        xwsz_t size; /**< 大小 */
        const char * description; /**< 资源描述 */
};

/**
 * @brief 设备栈时钟资源
 */
struct xwds_resource_clk {
        void * soc; /**< SOC芯片 */
        xwid_t clkid; /**< ID */
        const char * description; /**< 资源描述 */
};

/**
 * @brief 设备栈电源资源
 */
struct xwds_resource_pwr {
        void * soc; /**< SOC芯片 */
        xwid_t pwrid; /**< ID */
        const char * description; /**< 资源描述 */
};

/**
 * @brief 设备栈GPIO资源
 */
struct xwds_resource_gpio {
        void * soc; /**< SOC芯片 */
        xwid_t port; /**< port ID */
        xwsq_t pinmask; /**< pin 掩码 */
        const char * description; /**< 资源描述 */
};

/**
 * @brief 设备栈DMA资源
 */
struct xwds_resource_dma {
        void * soc; /**< SOC芯片 */
        xwid_t ch; /**< DMA通道 */
        void * xwccfg; /**< SOC DMA 通道私有配置 */
        const char * description; /**< 资源描述 */
};

/**
 * @brief 设备栈资源
 */
struct xwds_resources {
        const struct xwos_irq_resource * irqrsc_array; /**< IRQ资源数组 */
        xwsz_t irqrsc_num; /**< IRQ资源数量 */
        const struct xwds_resource_reg * regrsc_array; /**< 寄存器资源数组 */
        xwsz_t regrsc_num; /**< 寄存器资源数量 */
        const struct xwds_resource_pwr * pwrrsc_array; /**< 电源资源数组 */
        xwsz_t pwrrsc_num; /**< 电源资源数量 */
        const struct xwds_resource_clk * clkrsc_array; /**< 时钟资源数组 */
        xwsz_t clkrsc_num; /**< 时钟资源数量 */
        const struct xwds_resource_gpio * gpiorsc_array; /**< GPIO资源数组 */
        xwsz_t gpiorsc_num; /**< GPIO资源数量 */
        const struct xwds_resource_dma * dmarsc_array; /**< DMA资源数组 */
        xwsz_t dmarsc_num; /**< DMA资源数量 */
};

#endif /* xwcd/ds/standard.h */
