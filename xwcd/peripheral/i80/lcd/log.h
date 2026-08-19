/**
 * @file
 * @brief Intel8080 LCD Controller Log
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

#ifndef __xwcd_peripheral_i80_lcd_log_h__
#define __xwcd_peripheral_i80_lcd_log_h__

#include <xwos/standard.h>
#include <xwos/lib/xwlog.h>

#ifndef LOGTAG
#  define LOGTAG "I80LCD"
#endif

#if defined(I80LCD_DEBUG)
#  define i80lcdlogd(fmt, ...) xwlogf(D, LOGTAG, fmt, ##__VA_ARGS__)
#else
#  define i80lcdlogd(fmt, ...)
#endif
#define i80lcdlogi(fmt, ...) xwlogf(I, LOGTAG, fmt, ##__VA_ARGS__)
#define i80lcdloge(fmt, ...) xwlogf(E, LOGTAG, fmt, ##__VA_ARGS__)


#endif /* xwcd/peripheral/i80/lcd/log.h */
