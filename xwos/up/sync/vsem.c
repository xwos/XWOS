/**
 * @file
 * @brief XWOS UP内核同步机制：信号量的虚基类
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#include <xwos/standard.h>
#include <xwos/ospl/irq.h>
#include <xwos/up/sync/obj.h>
#if defined(XWOSCFG_SYNC_EVT) && (1 == XWOSCFG_SYNC_EVT)
#  include <xwos/up/sync/evt.h>
#endif
#include <xwos/up/sync/vsem.h>


__xwup_code
void xwup_vsem_construct(struct xwup_vsem * vsem)
{
        xwup_synobj_construct(&vsem->synobj);
}

__xwup_code
void xwup_vsem_destruct(struct xwup_vsem * vsem)
{
        xwup_synobj_destruct(&vsem->synobj);
}

__xwup_code
xwer_t xwup_vsem_activate(struct xwup_vsem * vsem, xwobj_gc_f gcfunc)
{
        return xwup_synobj_activate(&vsem->synobj, gcfunc);
}

__xwup_code
xwer_t xwup_vsem_acquire(struct xwup_vsem * sem, xwsq_t tik)
{
        return xwup_synobj_acquire(&sem->synobj, tik);
}

__xwup_code
xwer_t xwup_vsem_release(struct xwup_vsem * sem, xwsq_t tik)
{
        return xwup_synobj_release(&sem->synobj, tik);
}

__xwup_code
xwer_t xwup_vsem_grab(struct xwup_vsem * sem)
{
        return xwup_synobj_grab(&sem->synobj);
}

__xwup_code
xwer_t xwup_vsem_put(struct xwup_vsem * sem)
{
        return xwup_synobj_put(&sem->synobj);
}

#if defined(XWOSCFG_SYNC_EVT) && (1 == XWOSCFG_SYNC_EVT)
__xwup_code
xwer_t xwup_vsem_bind(struct xwup_vsem * vsem, struct xwup_evt * evt, xwsq_t pos)
{
        xwreg_t cpuirq;
        xwer_t rc;

        xwospl_cpuirq_save_lc(&cpuirq);
        rc = xwup_sel_obj_bind(evt, &vsem->synobj, pos, true);
        if ((XWOK == rc) && (vsem->count > 0)) {
                rc = xwup_sel_obj_s1i(evt, &vsem->synobj);
        }
        xwospl_cpuirq_restore_lc(cpuirq);

        return rc;
}

__xwup_code
xwer_t xwup_vsem_unbind(struct xwup_vsem * vsem, struct xwup_evt * evt)
{
        xwreg_t cpuirq;
        xwer_t rc;

        xwospl_cpuirq_save_lc(&cpuirq);
        rc = xwup_sel_obj_unbind(evt, &vsem->synobj, true);
        if (XWOK == rc) {
                rc = xwup_sel_obj_c0i(evt, &vsem->synobj);
        }
        xwospl_cpuirq_restore_lc(cpuirq);

        return rc;
}
#endif

__xwup_code
xwer_t xwup_vsem_freeze(struct xwup_vsem * vsem)
{
        xwer_t rc;
        xwreg_t cpuirq;

        rc = XWOK;
        xwospl_cpuirq_save_lc(&cpuirq);
        if (__xwcc_unlikely(vsem->count < 0)) {
                rc = -EALREADY;
        } else {
                vsem->count = XWUP_VSEM_NEGTIVE;
#if defined(XWOSCFG_SYNC_EVT) && (1 == XWOSCFG_SYNC_EVT)
                struct xwup_evt * evt;
                struct xwup_synobj * synobj;

                synobj = &vsem->synobj;
                evt = synobj->sel.evt;
                if (NULL != evt) {
                        xwup_sel_obj_c0i(evt, synobj);
                }
#endif
        }
        xwospl_cpuirq_restore_lc(cpuirq);

        return rc;
}

__xwup_code
xwer_t xwup_vsem_thaw(struct xwup_vsem * vsem)
{
        xwer_t rc;
        xwreg_t cpuirq;

        rc = XWOK;
        xwospl_cpuirq_save_lc(&cpuirq);
        if (__xwcc_unlikely(vsem->count >= 0)) {
                rc = -EALREADY;
        } else {
                vsem->count = 0;
        }
        xwospl_cpuirq_restore_lc(cpuirq);
        return rc;
}

__xwup_code
xwer_t xwup_vsem_getvalue(struct xwup_vsem * vsem, xwssq_t * sval)
{
        *sval = vsem->count;
        return XWOK;
}
