/**
 * @file
 * @brief W25Qxx编程器
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

#ifndef __xwam_application_w25qpt_w25qpt_h__
#define __xwam_application_w25qpt_w25qpt_h__

#include <xwos/standard.h>
#include <xwos/lib/xwlog.h>
#include <xwos/osal/thd.h>
#include <xwcd/peripheral/spi/flash/w25qxx/device.h>
#include <xwam/application/w25qpt/hwifal.h>

#if defined(XWLIBCFG_LOG) && (1 == XWLIBCFG_LOG)
#  define W25QPT_LOG_TAG "w25qpt"
#  define w25qptlogf(lv, fmt, ...) xwlogf(lv, W25QPT_LOG_TAG, fmt, ##__VA_ARGS__)
#else
#  define w25qptlogf(lv, fmt, ...)
#endif

#define W25QPT_THD_STACK       (4096U)

/**
 * @brief W25Qxx编程器
 */
struct w25qpt {
        const char * name; /**< 名字 */
        struct xwds_w25qxx * flash; /**< Flash */
        xwsq_t hwifst; /**< 硬件接口抽象层状态 */
        const struct w25qpt_hwifal_operations * hwifops; /**< 硬件接口抽象层操作函数 */
        void * hwifcb; /**< 硬件接口 */
        struct xwos_thd thdobj; /**< 线程结构体 */
        xwos_thd_d thd; /**< 线程描述符 */
        __xwcc_alignl1cache xwu8_t thd_stack[W25QPT_THD_STACK]; /**< 线程的栈 */
};

#endif /* xwam/application/w25qpt/w25qpt.h */
