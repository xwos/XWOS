/**
 * @file
 * @brief 架构描述层：对称多核系统的中断控制器配驱动
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

#ifndef __mp_nvic_drv_h__
#define __mp_nvic_drv_h__

#include <xwos/standard.h>

struct xwmp_irqc_driver;

extern const struct xwmp_irqc_driver cortexm_nvic_drv;

#endif /* mp_nvic_drv.h */
