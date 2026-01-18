/**
 * @file
 * @brief 架构描述层：陷阱
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
#include <xwcd/soc/arm64/v8a/arch_exception.h>
#include <xwcd/soc/arm64/v8a/arch_skd.h>

/* #define SOC_DBGF */
#include <soc_debug.h>

__xwcc_section(".armv8a.exception.text.el1")
void armv8a_trap_sync_el1(struct armv8a_reg_frame * regs, xwu64_t routine)
{
        xwid_t cpuid;
        xwu64_t sp_el0;
        xwu64_t far;
        xwu64_t esr;
        xwu64_t ec;
        xwu64_t il;
        xwu64_t iss;

        XWOS_UNUSED(regs);
        cpuid = xwospl_skd_get_cpuid_lc();
        armv8a_sysreg_read(&far, far_el1);
        armv8a_sysreg_read(&esr, esr_el1);
        ec = esr >> (xwu64_t)26;
        il = (esr & ((xwu64_t)1 << (xwu64_t)25)) >> (xwu64_t)25;
        iss = esr & 0x1FFFFFF;

        switch (ec) {
        case ARMV8A_ESR_ELx_EC_SVC32:
        case ARMV8A_ESR_ELx_EC_SVC64:
                /* SVC Call from EL0 & EL1 */
                soc_dbgf("SVC",
                         "SVC detected on CPU%d@EL1, ESR:0x%X\r\n",
                         cpuid, esr);
                break;
        default:
                armv8a_backtrace_frame(regs, "fault");
                armv8a_dump(regs);
                armv8a_sysreg_read(&sp_el0, sp_el0);
                soc_logf("sp_el0: 0x%016lX\r\n", sp_el0);

                soc_panic("Sync exception 0x%X detected on CPU%d@EL2, "
                          "ESR:0x%X(EC:0x%X,IL:0x%X,ISS:0x%X), FAR:0x%lX\r\n",
                          routine, cpuid, esr, ec, il, iss, far);
                break;
        }
}

__xwcc_section(".armv8a.exception.text.el1")
void armv8a_trap_serror_el1(struct armv8a_reg_frame * regs, xwu64_t routine)
{
        xwid_t cpuid;
        xwu64_t sp_el0;
        xwu64_t far;
        xwu64_t esr;
        xwu64_t ec;
        xwu64_t il;
        xwu64_t iss;

        XWOS_UNUSED(regs);
        cpuid = xwospl_skd_get_cpuid_lc();
        armv8a_sysreg_read(&far, far_el1);
        armv8a_sysreg_read(&esr, esr_el1);
        ec = esr >> (xwu64_t)26;
        il = (esr & ((xwu64_t)1 << (xwu64_t)25)) >> (xwu64_t)25;
        iss = esr & 0x1FFFFFF;

        armv8a_backtrace_frame(regs, "fault");
        armv8a_dump(regs);
        armv8a_sysreg_read(&sp_el0, sp_el0);
        soc_logf("sp_el0: 0x%016lX\r\n", sp_el0);

        soc_panic("SError exception 0x%x detected on CPU%d@EL1, "
                  "ESR:0x%X(EC:0x%X,IL:0x%X,ISS:0x%X), FAR:0x%lX\r\n",
                  routine, cpuid, esr, ec, il, iss, far);
}

__xwcc_section(".armv8a.exception.text.el2")
void armv8a_trap_sync_el2(struct armv8a_reg_frame * regs, xwu64_t routine)
{
        xwid_t cpuid;
        xwu64_t sp_el0;
        xwu64_t hcr;
        xwu64_t far;
        xwu64_t esr;
        xwu64_t ec;
        xwu64_t il;
        xwu64_t iss;

        XWOS_UNUSED(regs);
        cpuid = xwospl_skd_get_cpuid_lc();
        armv8a_sysreg_read(&far, far_el2);
        armv8a_sysreg_read(&esr, esr_el2);
        ec = esr >> (xwu64_t)26;
        il = (esr & ((xwu64_t)1 << (xwu64_t)25)) >> (xwu64_t)25;
        iss = esr & 0x1FFFFFF;

        switch (ec) {
        case ARMV8A_ESR_ELx_EC_SVC32:
        case ARMV8A_ESR_ELx_EC_SVC64:
                /* SVC Call from EL0 & EL1, when HCR_EL2.TGE == 1 */
                armv8a_sysreg_read(&hcr, hcr_el2);
                soc_dbgf("SVC",
                         "SVC detected on CPU%d@EL2, "
                         "ESR:0x%X, HCR_EL2:0x%lX\r\n",
                         cpuid, esr, hcr);
                break;
        case ARMV8A_ESR_ELx_EC_HVC32:
        case ARMV8A_ESR_ELx_EC_HVC64:
                /* HVC Call from EL1 & EL2 */
                armv8a_sysreg_read(&hcr, hcr_el2);
                if ((xwu64_t)8 == iss) {
                        armv8a_skd_swcx();
                } else {
                        soc_infof("HVC",
                                  "Sync exception 0x%X HVC detected on CPU%d@EL2, "
                                  "ESR:0x%X(ISS:0x%X), HCR_EL2:0x%lX\r\n",
                                  routine, cpuid, esr, iss, hcr);
                }
                break;
        default:
                armv8a_backtrace_frame(regs, "fault");
                armv8a_dump(regs);
                armv8a_sysreg_read(&sp_el0, sp_el0);
                soc_logf("sp_el0: 0x%016lX\r\n", sp_el0);

                soc_panic("Sync exception 0x%X detected on CPU%d@EL2, "
                          "ESR:0x%X(EC:0x%X,IL:0x%X,ISS:0x%X), FAR:0x%lX\r\n",
                          routine, cpuid, esr, ec, il, iss, far);
                break;
        }
}

__xwcc_section(".armv8a.exception.text.el2")
void armv8a_trap_serror_el2(struct armv8a_reg_frame * regs, xwu64_t routine)
{
        xwid_t cpuid;
        xwu64_t sp_el0;
        xwu64_t far;
        xwu64_t esr;
        xwu64_t ec;
        xwu64_t il;
        xwu64_t iss;

        XWOS_UNUSED(regs);
        cpuid = xwospl_skd_get_cpuid_lc();
        armv8a_sysreg_read(&far, far_el2);
        armv8a_sysreg_read(&esr, esr_el2);
        ec = esr >> (xwu64_t)26;
        il = (esr & ((xwu64_t)1 << (xwu64_t)25)) >> (xwu64_t)25;
        iss = esr & 0x1FFFFFF;

        armv8a_backtrace_frame(regs, "fault");
        armv8a_dump(regs);
        armv8a_sysreg_read(&sp_el0, sp_el0);
        soc_logf("sp_el0: 0x%016lX\r\n", sp_el0);

        soc_panic("SError exception 0x%x detected on CPU%d@EL1, "
                  "ESR:0x%X(EC:0x%X,IL:0x%X,ISS:0x%X), FAR:0x%lX\r\n",
                  routine, cpuid, esr, ec, il, iss, far);
}

__xwcc_section(".armv8a.exception.text.el3")
void armv8a_trap_sync_el3(struct armv8a_reg_frame * regs, xwu64_t routine)
{
        xwid_t cpuid;
        xwu64_t sp_el0;
        xwu64_t far;
        xwu64_t scr;
        xwu64_t esr;
        xwu64_t ec;
        xwu64_t il;
        xwu64_t iss;

        XWOS_UNUSED(regs);
        cpuid = xwospl_skd_get_cpuid_lc();
        armv8a_sysreg_read(&far, far_el3);
        armv8a_sysreg_read(&esr, esr_el3);
        ec = esr >> (xwu64_t)26;
        il = (esr & ((xwu64_t)1 << (xwu64_t)25)) >> (xwu64_t)25;
        iss = esr & 0x1FFFFFF;

        switch (ec) {
        case ARMV8A_ESR_ELx_EC_SMC32:
        case ARMV8A_ESR_ELx_EC_SMC64:
                /* HVC Call from EL1 & EL3 */
                armv8a_sysreg_read(&scr, scr_el3);
                soc_dbgf("SMC",
                         "SMC detected on CPU%d@EL3, "
                         "ESR:0x%X, SCR_EL3:0x%lX\r\n",
                         cpuid, esr, scr);
                break;
        default:
                armv8a_backtrace_frame(regs, "fault");
                armv8a_dump(regs);
                armv8a_sysreg_read(&sp_el0, sp_el0);
                soc_logf("sp_el0: 0x%016lX\r\n", sp_el0);

                soc_panic("Sync exception 0x%X detected on CPU%d@EL2, "
                          "ESR:0x%X(EC:0x%X,IL:0x%X,ISS:0x%X), FAR:0x%lX\r\n",
                          routine, cpuid, esr, ec, il, iss, far);
                break;
        }
}

__xwcc_section(".armv8a.exception.text.el3")
void armv8a_trap_serror_el3(struct armv8a_reg_frame * regs, xwu64_t routine)
{
        xwid_t cpuid;
        xwu64_t sp_el0;
        xwu64_t far;
        xwu64_t esr;
        xwu64_t ec;
        xwu64_t il;
        xwu64_t iss;

        XWOS_UNUSED(regs);
        cpuid = xwospl_skd_get_cpuid_lc();
        armv8a_sysreg_read(&far, far_el3);
        armv8a_sysreg_read(&esr, esr_el3);
        ec = esr >> (xwu64_t)26;
        il = (esr & ((xwu64_t)1 << (xwu64_t)25)) >> (xwu64_t)25;
        iss = esr & 0x1FFFFFF;

        armv8a_backtrace_frame(regs, "fault");
        armv8a_dump(regs);
        armv8a_sysreg_read(&sp_el0, sp_el0);
        soc_logf("sp_el0: 0x%016lX\r\n", sp_el0);

        soc_panic("SError exception 0x%x detected on CPU%d@EL1, "
                  "ESR:0x%X(EC:0x%X,IL:0x%X,ISS:0x%X), FAR:0x%lX\r\n",
                  routine, cpuid, esr, ec, il, iss, far);
}

__xwbsp_code
void armv8a_dump(struct armv8a_reg_frame  * frame)
{
        soc_logf("[Dump Registers 0x%016lX]\r\n", (xwu64_t)frame);
        soc_logf("        X0:  0x%016lX, X1:  0x%016lX, X2:  0x%016lX, X3:  0x%016lX,\r\n",
                 frame->r[0], frame->r[1], frame->r[2], frame->r[3]);
        soc_logf("        X4:  0x%016lX, X5:  0x%016lX, X6:  0x%016lX, X7:  0x%016lX,\r\n",
                 frame->r[4], frame->r[5], frame->r[6], frame->r[7]);
        soc_logf("        X8:  0x%016lX, X9:  0x%016lX, X10: 0x%016lX, X11: 0x%016lX,\r\n",
                 frame->r[8], frame->r[9], frame->r[10], frame->r[11]);
        soc_logf("        X12: 0x%016lX, X13: 0x%016lX, X14: 0x%016lX, X15: 0x%016lX,\r\n",
                 frame->r[12], frame->r[13], frame->r[14], frame->r[15]);
        soc_logf("        X16: 0x%016lX, X17: 0x%016lX, X18: 0x%016lX, X19: 0x%016lX,\r\n",
                 frame->r[16], frame->r[17], frame->r[18], frame->r[19]);
        soc_logf("        X20: 0x%016lX, X21: 0x%016lX, X22: 0x%016lX, X23: 0x%016lX,\r\n",
                 frame->r[20], frame->r[21], frame->r[22], frame->r[23]);
        soc_logf("        X24: 0x%016lX, X25: 0x%016lX, X26: 0x%016lX, X27: 0x%016lX,\r\n",
                 frame->r[24], frame->r[25], frame->r[26], frame->r[27]);
        soc_logf("        X28: 0x%016lX, X29: 0x%016lX, LR:  0x%016lX, SP:  0x%016lX,\r\n"
                 "        PC:  0x%016lX, PS:  0x%016lX\r\n",
                 frame->r[28], frame->r[29], frame->lr, frame->sp,
                 frame->elr, frame->spsr);
}

union armv8a_backtrace_chain {
        union armv8a_backtrace_chain * prev;
        xwu64_t * p64;
        xwu64_t u64;
};

__xwbsp_code
void armv8a_backtrace_frame(struct armv8a_reg_frame  * frame, const char * name)
{
        union armv8a_backtrace_chain fp;
        xwu64_t lr;

        fp.u64 = frame->r[29];
        if (NULL == name) {
                soc_logf("[Backtrace 0x%016lX]\r\n"
                         "        [0x%016lX]: 0x%016lX\r\n",
                         (xwu64_t)frame, (xwu64_t)frame, frame->elr);
        } else {
                soc_logf("[Backtrace 0x%016lX stack:%s]\r\n"
                         "        [0x%016lX]: 0x%016lX\r\n",
                         (xwu64_t)frame, name, (xwu64_t)frame, frame->elr);
        }
        while (NULL != fp.prev) {
                lr = *(fp.p64 + 1);
                soc_logf("        [0x%016lX]: 0x%016lX\r\n", (fp.u64 + 16), lr);
                fp.u64 = fp.prev->u64;
        }
}

__xwbsp_code
void armv8a_backtrace(xwu64_t fp)
{
        union armv8a_backtrace_chain fpchain;
        xwu64_t lr;

        fpchain.u64 = fp;
        soc_logf("[Backtrace fp:0x%016lX]\r\n", fp);
        while (NULL != fpchain.prev) {
                lr = *(fpchain.p64 + 1);
                soc_logf("        [0x%016lX]: 0x%016lX\r\n", (fpchain.u64 + 16), lr);
                fpchain.u64 = fpchain.prev->u64;
        }
}
