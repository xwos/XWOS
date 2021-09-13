/**
 * @file
 * @brief 玄武设备栈：对象
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

#include <xwcd/ds/standard.h>
#include <xwos/lib/object.h>
#include <xwos/lib/xwlog.h>
#include <xwos/lib/bclst.h>
#include <xwos/osal/lock/seqlock.h>
#include <xwcd/ds/xwds.h>
#include <xwcd/ds/object.h>

__xwds_code
void xwds_obj_construct(struct xwds_object * obj)
{
        xwos_object_construct(&obj->xwobj);
        xwlib_bclst_init_node(&obj->node);
        obj->ds = NULL;
}

__xwds_code
void xwds_obj_destruct(struct xwds_object * obj)
{
        obj->ds = NULL;
        xwos_object_destruct(&obj->xwobj);
}

__xwds_code
xwsq_t xwds_obj_gettik(struct xwds_object * obj)
{
        xwsq_t tik;

        if (obj) {
                tik = obj->xwobj.tik;
        } else {
                tik = 0;
        }
        return tik;
}

__xwds_code
xwer_t xwds_obj_probe(struct xwds_object * obj, xwobj_gc_f gcfunc)
{
        return xwos_object_activate(&obj->xwobj, gcfunc);
}

__xwds_code
xwer_t xwds_obj_remove(struct xwds_object * obj)
{
        xwer_t rc;

        rc = xwaop_teq_then_sub(xwsq, &obj->xwobj.refcnt,
                                XWDS_OBJ_REF_SHUTDOWN, 1,
                                NULL, NULL);
        if (XWOK == rc) {
                if (obj->xwobj.gcfunc) {
                        rc = obj->xwobj.gcfunc(obj);
                }
        } else {
                rc = -EPERM;
        }
        return rc;
}

__xwds_code
xwer_t xwds_obj_start(struct xwds_object * obj)
{
        xwer_t rc;

        rc = xwaop_teq_then_add(xwsq, &obj->xwobj.refcnt,
                                XWDS_OBJ_REF_SHUTDOWN, 2,
                                NULL, NULL);
        if (rc < 0) {
                rc = -EPERM;
        }/* else {} */
        return rc;
}

__xwds_code
xwer_t xwds_obj_stop(struct xwds_object * obj)
{
        xwer_t rc;

        rc = xwaop_teq_then_sub(xwsq, &obj->xwobj.refcnt,
                                XWDS_OBJ_REF_RUNNING, 2,
                                NULL, NULL);
        if (rc < 0) {
                rc = -EPERM;
        }/* else {} */
        return rc;
}

__xwds_code
xwer_t xwds_obj_suspend(struct xwds_object * obj)
{
        xwer_t rc;

        rc = xwaop_teq_then_sub(xwsq, &obj->xwobj.refcnt,
                                XWDS_OBJ_REF_RUNNING, 1,
                                NULL, NULL);
        if (rc < 0) {
                rc = -EPERM;
        }/* else {} */
        return rc;
}

__xwds_code
xwer_t xwds_obj_resume(struct xwds_object * obj)
{
        xwer_t rc;

        rc = xwaop_teq_then_add(xwsq, &obj->xwobj.refcnt,
                                XWDS_OBJ_REF_SUSPEND, 1,
                                NULL, NULL);
        if (rc < 0) {
                rc = -EPERM;
        }/* else {} */
        return rc;
}

__xwds_code
xwer_t xwds_obj_grab(struct xwds_object * obj)
{
        xwer_t rc;

        rc = xwaop_tge_then_add(xwsq, &obj->xwobj.refcnt,
                                XWDS_OBJ_REF_RUNNING, 1,
                                NULL, NULL);
        if (__xwcc_unlikely(rc < 0)) {
                rc = -EPERM;
        }/* else {} */
        return rc;
}

__xwds_code
xwer_t xwds_obj_put(struct xwds_object * obj)
{
        xwer_t rc;

        rc = xwaop_tgt_then_sub(xwsq, &obj->xwobj.refcnt,
                                XWDS_OBJ_REF_RUNNING, 1,
                                NULL, NULL);
        if (rc < 0) {
                rc = -EPERM;
        }/* else {} */
        return rc;
}

__xwds_code
xwsq_t xwds_obj_get_refcnt(struct xwds_object * obj)
{
        return xwos_object_get_refcnt(&obj->xwobj);
}

__xwds_code
xwer_t xwds_obj_add(struct xwds * ds, struct xwds_object * obj)
{
        xwreg_t cpuirq;
        xwer_t rc;

        xwos_sqlk_wr_lock_cpuirqsv(&ds->devlistlock, &cpuirq);
        if (obj->ds) {
                rc = -EPERM;
        } else {
                xwlib_bclst_add_tail(&ds->devhead, &obj->node); /* LIFO */
                obj->ds = ds;
                rc = XWOK;
        }
        xwos_sqlk_wr_unlock_cpuirqrs(&ds->devlistlock, cpuirq);
        return rc;
}

__xwds_code
xwer_t xwds_obj_del(struct xwds * ds, struct xwds_object * obj)
{
        xwreg_t cpuirq;
        xwer_t rc;

        xwos_sqlk_wr_lock_cpuirqsv(&ds->devlistlock, &cpuirq);
        if (obj->ds != ds) {
                rc = -EOWNER;
        } else {
                xwlib_bclst_del_init(&obj->node); /* LIFO */
                obj->ds = NULL;
                rc = XWOK;
        }
        xwos_sqlk_wr_unlock_cpuirqrs(&ds->devlistlock, cpuirq);
        return rc;
}
