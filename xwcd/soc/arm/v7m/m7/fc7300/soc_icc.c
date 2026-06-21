/**
 * @file
 * @brief SOC描述层：Inter-Cpu Communication
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

#include <xwos/standard.h>
#include <xwos/ospl/skd.h>
#include "xwcd/soc/arm/v7m/m7/fc7300/soc_mb.h"
#include "xwcd/soc/arm/v7m/m7/fc7300/soc_icc.h"

__xwbsp_code
xwu32_t soc_icc_lock(xwu32_t ch)
{
        return soc_mb.channel[ch].sema & SOC_MB_SEMA_LOCK_MASK;
}

__xwbsp_code
void soc_icc_config_done_master_id(xwu32_t ch, xwu32_t mid)
{
        soc_mb.channel[ch].done_mask &= (xwu32_t)(~SOC_MB_DONE_MASK_DONE_MASTER_ID_MASK);
        soc_mb.channel[ch].done_mask |= SOC_MB_DONE_MASK_DONE_MASTER_ID(mid);
}

__xwbsp_code
void soc_icc_config_done_mask(xwu32_t ch, xwu32_t mask)
{
        soc_mb.channel[ch].done_mask &= (xwu32_t)(~SOC_MB_DONE_MASK_DONE_MASK_MASK);
        soc_mb.channel[ch].done_mask |= mask;
}

__xwbsp_code
void soc_icc_config_auto_clear(xwu32_t ch, xwu32_t mask)
{
        soc_mb.channel[ch].sema_unlk = mask & SOC_MB_SEMA_UNLK_AUTO_CLEAR_EN_MASK;
}

__xwbsp_code
void soc_icc_write_data(xwu32_t ch, xwu32_t data0, xwu32_t data1)
{
        soc_mb.channel[ch].data0 = data0;
        soc_mb.channel[ch].data1 = data1;
}

__xwbsp_code
void soc_icc_config_request(xwu32_t ch, xwu32_t mask)
{
        soc_mb.channel[ch].request = mask & SOC_MB_REQUEST_REQ_MASK;
}

__xwbsp_code
xwer_t soc_icc_send_data(xwu32_t ch, xwu32_t dstcpu, xwu32_t data0, xwu32_t data1)
{
        xwer_t rc;
        xwu32_t midx;
        xwu32_t lockcode;

        rc = -EBUSY;
        midx = SOC_MB_CPU2MB_IDX(xwospl_skd_get_cpuid_lc());
        lockcode = soc_icc_lock(ch);
        if (0U != lockcode) {
                soc_icc_config_done_master_id(ch, soc_mb_master_id_tab[dstcpu]);
                soc_icc_config_done_mask(ch, (xwu32_t)1 << midx);
                soc_icc_config_auto_clear(ch, (xwu32_t)1 << midx);
                soc_icc_write_data(ch, data0, data1);
                soc_icc_config_request(ch, (xwu32_t)1 << dstcpu);
                rc = XWOK;
        }
        return rc;
}

__xwbsp_code
xwu32_t soc_icc_get_done_master_id(xwu32_t ch)
{
        return ((soc_mb.channel[ch].done_mask & SOC_MB_DONE_MASK_DONE_MASTER_ID_MASK) >>
                SOC_MB_DONE_MASK_DONE_MASTER_ID_SHIFT);
}

__xwbsp_code
xwu32_t soc_icc_get_channel_locked(xwu32_t ch)
{
        return soc_mb.channel[ch].stat & SOC_MB_STAT_CURRENT_LOCK_STATUS_MASK;
}

__xwbsp_code
xwu32_t soc_icc_get_current_lock_master_id(xwu32_t ch)
{
        return (soc_mb.channel[ch].stat & SOC_MB_STAT_CURRENT_LOCK_MASTER_ID_MASK) >>
                SOC_MB_STAT_CURRENT_LOCK_MASTER_ID_SHIFT;
}

__xwbsp_code
xwu32_t soc_icc_get_core_index(xwu32_t mid)
{
        xwu32_t idx = 0U;
        xwu32_t i;

        for (i = 0U; i < SOC_MB_CORE_NUM; i++) {
                if (mid == soc_mb_master_id_tab[i]) {
                        idx = i;
                        break;
                }
        }
        return idx;
}

__xwbsp_code
xwu32_t soc_icc_get_done_mask(xwu32_t ch)
{
        return ((soc_mb.channel[ch].done_mask & SOC_MB_DONE_MASK_DONE_MASK_MASK) >>
                SOC_MB_DONE_MASK_DONE_MASK_SHIFT);
}

__xwbsp_code
void soc_icc_set_done(xwu32_t ch, xwu32_t mask)
{
        soc_mb.channel[ch].done = mask;
}

__xwbsp_code
void soc_icc_done(xwu32_t ch)
{
        xwu32_t done_mid;
        xwid_t local;
        xwu32_t local_mid;
        xwu32_t locked_mid;
        xwu32_t flag;

        done_mid = soc_icc_get_done_master_id(ch);
        local = xwospl_skd_get_cpuid_lc();
        local_mid = soc_mb_master_id_tab[SOC_MB_CPU2MB_IDX(local)];
        if ((0U != soc_icc_get_channel_locked(ch)) && (local_mid == done_mid)) {
                locked_mid = soc_icc_get_current_lock_master_id(ch);
                flag = (xwu32_t)1 << soc_icc_get_core_index(locked_mid);
                if (0U != (flag & soc_icc_get_done_mask(ch))) {
                        soc_icc_set_done(ch, flag);
                }
        }
}

__xwbsp_code
xwer_t soc_icc_call(xwid_t dstcpu, soc_icc_func_f func, void * arg)
{
        xwid_t local;
        xwu32_t data1;
        xwu32_t data2;
        xwer_t rc;

        local = xwospl_skd_get_cpuid_lc();
        if ((local != dstcpu) && (dstcpu < SOC_MB_CORE_NUM)) {
                data1 = (xwu32_t)func;
                data2 = (xwu32_t)arg;
                rc = soc_icc_send_data(SOC_MB_CH_ICC, dstcpu, data1, data2);
        } else {
                rc = -EPERM;
        }
        return rc;
}

__xwbsp_code
void soc_icc_finish(void)
{
        soc_icc_done(SOC_MB_CH_ICC);
}
