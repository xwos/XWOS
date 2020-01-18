/**
 * @file
 * @brief xwmd设备栈：对象
 * @author
 * + 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <xwos/object.h>
#include <xwos/lib/xwlog.h>
#include <xwos/lib/bclst.h>
#include <xwos/osal/lock/seqlock.h>
#include <xwmd/ds/object.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         function prototypes         ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief 初始化设备栈
 * @param ds: (I) 设备栈指针
 */
__xwds_code
void xwds_init(struct xwds * ds)
{
        xwlib_bclst_init_node(&ds->devhead);
        xwosal_sqlk_init(&ds->devlistlock);
}

/**
 * @brief 设备栈对象的构造函数
 * @param obj: (I) 对象的指针
 */
__xwds_code
void xwds_obj_construct(struct xwds_object * obj)
{
        xwos_object_construct(&obj->xwobj);
        xwlib_bclst_init_node(&obj->node);
        obj->ds = NULL;
}

/**
 * @brief 设备栈对象的析构函数
 * @param obj: (I) 对象的指针
 */
__xwds_code
void xwds_obj_destruct(struct xwds_object * obj)
{
        obj->ds = NULL;
        xwos_object_destruct(&obj->xwobj);
}

/**
 * @brief 激活设备栈对象
 * @param obj: (I) 对象的指针
 * @return 错误码
 */
__xwds_code
xwer_t xwds_obj_activate(struct xwds_object * obj, xwobj_gc_f gcfunc)
{
        return xwos_object_activate(&obj->xwobj, gcfunc);
}

/**
 * @brief 将设备栈对象加入到设备栈
 * @param obj: (I) 对象的指针
 * @return 错误码
 */
__xwds_code
xwer_t xwds_obj_add(struct xwds * ds, struct xwds_object * obj)
{
        xwreg_t cpuirq;
        xwer_t rc;

        xwosal_sqlk_wr_lock_cpuirqsv(&ds->devlistlock, &cpuirq);
        if (obj->ds) {
                rc = -EPERM;
        } else {
                xwlib_bclst_add_tail(&ds->devhead, &obj->node); /* LIFO */
                obj->ds = ds;
                rc = OK;
        }
        xwosal_sqlk_wr_unlock_cpuirqrs(&ds->devlistlock, cpuirq);
        return rc;
}

/**
 * @brief 将设备栈对象从设备栈中删除
 * @param obj: (I) 对象的指针
 * @return 错误码
 */
__xwds_code
xwer_t xwds_obj_remove(struct xwds * ds, struct xwds_object * obj)
{
        xwreg_t cpuirq;
        xwer_t rc;

        xwosal_sqlk_wr_lock_cpuirqsv(&ds->devlistlock, &cpuirq);
        if (obj->ds != ds) {
                rc = -EOWNER;
        } else {
                xwlib_bclst_del_init(&obj->node); /* LIFO */
                obj->ds = NULL;
        }
        xwosal_sqlk_wr_unlock_cpuirqrs(&ds->devlistlock, cpuirq);
        return rc;
}
