/**
 * @file
 * @brief W25Qxx编程器：编程器
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

#ifndef __xwam_application_w25qrpt_w25qrpt_h__
#define __xwam_application_w25qrpt_w25qrpt_h__

#include <xwos/standard.h>
#include <xwos/lib/xwlog.h>
#include <xwos/osal/skd.h>
#include <xwcd/perpheral/spi/flash/w25qxx/device.h>
#include <xwam/application/w25qrpt/hwifal.h>

#if defined(XWLIBCFG_LOG) && (1 == XWLIBCFG_LOG)
#  define W25QRPT_LOG_TAG "w25qrpt"
#  define w25qrptlogf(lv, fmt, ...) xwlogf(lv, W25QRPT_LOG_TAG, fmt, ##__VA_ARGS__)
#else
#  define w25qrptlogf(lv, fmt, ...)
#endif

/**
 * @brief W25Qxx编程器
 */
struct w25qrpt {
        const char * name; /**< 名字 */
        struct xwos_thd * thd; /**< 线程描述符 */
        struct xwds_w25qxx * flash; /**< Flash */
        xwsq_t hwifst; /**< 硬件接口抽象层状态 */
        const struct w25qrpt_hwifal_operations * hwifops; /**< 硬件接口抽象层操作函数 */
        void * hwifcb; /**< 硬件接口 */
};

#endif /* xwam/application/w25qrpt/w25qrpt.h */
