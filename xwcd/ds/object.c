/**
 * @file
 * @brief xwcd设备栈：对象
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

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwcd/ds/standard.h>
#include <xwos/lib/object.h>
#include <xwos/lib/xwlog.h>
#include <xwos/lib/bclst.h>
#include <xwos/osal/lock/seqlock.h>
#include <xwcd/ds/xwds.h>
#include <xwcd/ds/object.h>

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
 * @brief 设备栈对象的构造函数
 * @param obj: (I) 对象的指针
 */
__xwds_code
void xwds_obj_construct(struct xwds_object * obj)
{
        xwos_object_construct(&obj->xwobj, XWCD_OBJ_DEVICE);
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
 * @brief 增加对象的引用计数
 * @param obj: (I) 对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EINVAL: 对象未被激活（引用计数小于1）
 */
__xwds_code
xwer_t xwds_obj_grab(struct xwds_object * obj)
{
        return xwos_object_grab(&obj->xwobj);
}

/**
 * @brief 减少对象的引用计数
 * @param obj: (I) 对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EINVAL: 对象未被激活（引用计数小于1）
 */
__xwds_code
xwer_t xwds_obj_put(struct xwds_object * obj)
{
        return xwos_object_put(&obj->xwobj);
}

/**
 * @brief 得到对象的引用计数
 * @param obj: (I) 对象的指针
 * @return 引用计数
 */
__xwds_code
xwsq_t xwds_obj_get_refcnt(struct xwds_object * obj)
{
        return xwos_object_get_refcnt(&obj->xwobj);
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

        xwos_sqlk_wr_lock_cpuirqsv(&ds->devlistlock, &cpuirq);
        if (obj->ds != ds) {
                rc = -EOWNER;
        } else {
                xwlib_bclst_del_init(&obj->node); /* LIFO */
                obj->ds = NULL;
        }
        xwos_sqlk_wr_unlock_cpuirqrs(&ds->devlistlock, cpuirq);
        return rc;
}
