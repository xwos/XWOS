/**
 * @file
 * @brief SOC描述层：Mailbox
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

#include <xwcd/soc/arm/v7m/m7/fc7300/soc_mb.h>
#include <xwos/ospl/skd.h>

__xwbsp_code
xwu32_t soc_mb_lock(xwu32_t ch)
{
        xwid_t cpuid;
        xwid_t masterid;
        xwu32_t lockcode;

        lockcode = (soc_mb.channel[ch].sema & SOC_MB_SEMA_LOCK_MASTER_ID_MASK);
        if (0U == (lockcode & SOC_MB_SEMA_LOCK_MASK)) {
                cpuid = xwospl_skd_get_cpuid_lc();
                masterid = ((lockcode & SOC_MB_SEMA_MASTER_ID_MASK) >>
                            (SOC_MB_SEMA_MASTER_ID_SHIFT + 1U));
                if (cpuid == masterid) {
                        lockcode |= SOC_MB_NESTED_LOCKCODE;
                }
        }
        return lockcode;
}

__xwbsp_code
void soc_mb_unlock(xwu32_t ch, xwu32_t lockcode)
{
        if (SOC_MB_SEMA_LOCK_MASK & lockcode) {
                soc_mb.channel[ch].done = SOC_MB_MASTER_DONE_CODE;
        }
}
