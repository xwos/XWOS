/**
 * @file
 * @brief XWOS通用库：玄武OS的对象
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <xwos/standard.h>
#include <xwos/lib/object.h>
#include <xwos/osal/irq.h>
#include <xwos/osal/skd.h>

#define XWOS_OBJTIX_CHUNK        ((xwsq_t)(1 << 20)) /* 1M */
#define XWOS_OBJTIX_CHUNK_MSK    (XWOS_OBJTIX_CHUNK - 1)

/**
 * @brief 对象标签分配器，每次分配XWOS_OBJTIX_CHUNK个ID给CPU
 */
__xwlib_data xwsq_a xwos_objtix_dispatcher = (CPUCFG_CPU_NUM * XWOS_OBJTIX_CHUNK);

/**
 * @brief 每CPU的对象标签分配器
 */
__xwlib_data xwsq_a xwos_objtix[CPUCFG_CPU_NUM] __xwcc_alignl1cache = {0};

/**
 * @brief 初始化对象标签分配器
 */
__xwlib_code
void xwos_objtix_init(void)
{
        xwsq_t i;

        xwos_objtix_dispatcher = (CPUCFG_CPU_NUM * XWOS_OBJTIX_CHUNK);
        for (i = 0; i < CPUCFG_CPU_NUM; i++) {
                xwos_objtix[i] = (i * XWOS_OBJTIX_CHUNK);
        }
}

/**
 * @brief 从对象标签分配器获取一个标签
 * @return 标签
 * @note
 * - 为了避免频繁地访问多CPU共享原子变量xwos_objtix_dispatcher，每次获取标签
 *   都是从当前CPU的每CPU变量xwos_objtix中分配的，只有当xwos_objtix的分配额
 *   达到最大额度XWOS_OBJTIX_CHUNK时，才会访问xwos_objtix_dispatcher再次预先分配
 *   XWOS_OBJTIX_CHUNK个标签。
 */
__xwlib_code
xwsq_t xwos_objtix_get(void)
{
        xwid_t cpu;
        xwsq_a * objtix;
        xwsq_t curr, max, ov;
        xwer_t rc;

        cpu = xwos_skd_id_lc();
        objtix = &xwos_objtix[cpu];
        while (true) {
                curr = xwaop_load(xwsq, objtix, xwmb_modr_relaxed);
                max = (curr & (~XWOS_OBJTIX_CHUNK_MSK)) + XWOS_OBJTIX_CHUNK_MSK;
                rc = xwaop_tlt_then_add(xwsq, objtix, max, 1, NULL, &ov);
                if (rc < 0) {
                        xwreg_t cpuirq;
                        xwsq_t chunk;

                        xwos_cpuirq_save_lc(&cpuirq);
                        curr = xwaop_load(xwsq, objtix, xwmb_modr_relaxed);
                        if (curr == max) {
                                ov = curr;
                                xwaop_add(xwsq, &xwos_objtix_dispatcher,
                                          XWOS_OBJTIX_CHUNK, NULL, &chunk);
                                xwaop_store(xwsq, objtix, xwmb_modr_relaxed, chunk);
                                xwos_cpuirq_restore_lc(cpuirq);
                                break;
                        } else {
                                xwos_cpuirq_restore_lc(cpuirq);
                        }
                } else {
                        break;
                }
        }
        return ov;
}

/**
 * @brief XWOS对象的构造函数
 * @param obj: (I) 对象指针
 */
__xwlib_code
void xwos_object_construct(struct xwos_object * obj)
{
        obj->refcnt = 0;
        obj->gcfunc = NULL;
        obj->ticket = 0;
}

/**
 * @brief XWOS对象的析构函数
 * @param obj: (I) 对象指针
 */
__xwlib_code
void xwos_object_destruct(struct xwos_object * obj)
{
        obj->gcfunc = NULL;
        obj->ticket = 0;
}

/**
 * @brief 激活一个XWOS对象（将其引用计数初始化位1）
 * @param obj: (I) 对象指针
 * @param gcfunc: (I) 垃圾回收函数：当对象应用计数为0，调用此函数回收资源。
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EOBJACTIVE: 对象已激活
 */
__xwlib_code
xwer_t xwos_object_activate(struct xwos_object * obj, xwobj_gc_f gcfunc)
{
        xwer_t rc;

        rc = xwaop_teq_then_add(xwsq, &obj->refcnt, 0, 1, NULL, NULL);
        if (__xwcc_likely(XWOK == rc)) {
                obj->ticket = xwos_objtix_get();
                obj->gcfunc = gcfunc;
        } else {
                rc = -EOBJACTIVE;
        }
        return rc;
}

/**
 * @brief 获取对象的描述符
 * @param obj: (I) 对象指针
 * @return 对象描述符
 */
__xwlib_code
xwobj_d xwos_object_get_d(struct xwos_object * obj)
{
        xwobj_d objd;

        if (obj) {
                objd.obj = obj;
                objd.ticket = obj->ticket;
        } else {
                objd.obj = NULL;
                objd.ticket = 0;
        }
        return objd;
}

/**
 * @brief 增加对象的引用计数
 * @param obj: (I) 对象指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EOBJDEAD: 对象已销毁
 */
__xwlib_code
xwer_t xwos_object_grab(struct xwos_object * obj)
{
        xwer_t rc;

        rc = xwaop_tge_then_add(xwsq, &obj->refcnt, 1, 1, NULL, NULL);
        if (__xwcc_unlikely(rc < 0)) {
                rc = -EOBJDEAD;
        }/* else {} */
        return rc;
}

/**
 * @brief 安全地增加对象的引用计数
 * @param objd: (I) 对象描述符
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EPERM: 对象检查失败
 * @note
 * - 此函数会检测对象的标签，防止出现相同地址不同对象（即ABA）的问题。
 */
__xwlib_code
xwer_t xwos_object_grab_safely(xwobj_d objd)
{
        xwer_t rc;

        if ((NULL == objd.obj) || (objd.ticket != objd.obj->ticket)) {
                rc = -EPERM;
        } else {
                rc = xwos_object_grab(objd.obj);
        }
        return rc;
}

/**
 * @brief 减少对象的引用计数
 * @param obj: (I) 对象指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EOBJDEAD: 对象已销毁
 */
__xwlib_code
xwer_t xwos_object_put(struct xwos_object * obj)
{
        xwer_t rc;
        xwsq_t nv;

        rc = xwaop_tgt_then_sub(xwsq, &obj->refcnt,
                                0, 1,
                                &nv, NULL);
        if (__xwcc_likely(XWOK == rc)) {
                if (__xwcc_unlikely(0 == nv)) {
                        if (obj->gcfunc) {
                                rc = obj->gcfunc(obj);
                        }/* else {} */
                        obj->ticket = 0;
                }/* else {} */
        } else {
                rc = -EOBJDEAD;
        }
        return rc;
}

/**
 * @brief 获取XWOS对象的引用计数
 * @param obj: (I) 对象指针
 * @return 引用计数
 */
__xwlib_code
xwsq_t xwos_object_get_refcnt(struct xwos_object * obj)
{
        return xwaop_load(xwsq, &obj->refcnt, xwmb_modr_relaxed);
}
