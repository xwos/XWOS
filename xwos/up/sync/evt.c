/**
 * @file
 * @brief 玄武OS UP内核同步机制：事件
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <xwos/standard.h>
#include <string.h>
#include <xwos/lib/xwbop.h>
#include <xwos/mm/common.h>
#include <xwos/mm/kma.h>
#if defined(XWUPCFG_SYNC_EVT_STDC_MM) && (1 == XWUPCFG_SYNC_EVT_STDC_MM)
#  include <stdlib.h>
#endif
#include <xwos/ospl/irq.h>
#include <xwos/up/thd.h>
#include <xwos/up/lock/fakespinlock.h>
#include <xwos/up/sync/cond.h>
#include <xwos/up/sync/obj.h>
#include <xwos/up/sync/evt.h>

static __xwup_code
struct xwup_evt * xwup_evt_alloc(xwsz_t num);

static __xwup_code
void xwup_evt_free(struct xwup_evt * evt);

static __xwup_code
void xwup_evt_setup(struct xwup_evt * evt, xwsz_t num, xwbmp_t * bmp, xwbmp_t * msk);

static __xwup_code
void xwup_evt_activate(struct xwup_evt * evt, xwsq_t type);

static __xwup_code
xwer_t xwup_flg_trywait_level(struct xwup_evt * evt,
                              xwsq_t trigger, xwsq_t action,
                              xwbmp_t origin[], xwbmp_t msk[]);

static __xwup_code
xwer_t xwup_flg_trywait_edge(struct xwup_evt * evt, xwsq_t trigger,
                             xwbmp_t origin[], xwbmp_t msk[]);

static __xwup_code
xwer_t xwup_flg_timedwait_level(struct xwup_evt * evt,
                                xwsq_t trigger, xwsq_t action,
                                xwbmp_t origin[], xwbmp_t msk[],
                                xwtm_t * xwtm);

static __xwup_code
xwer_t xwup_flg_timedwait_edge(struct xwup_evt * evt, xwsq_t trigger,
                               xwbmp_t origin[], xwbmp_t msk[],
                               xwtm_t * xwtm);

static __xwup_code
void xwup_evt_setup(struct xwup_evt * evt, xwsz_t num,
                    xwbmp_t * bmp, xwbmp_t * msk)
{
        evt->num = num;
        evt->bmp = bmp;
        evt->msk = msk;
}

/**
 * @brief 从事件对象缓存中申请对象
 * @return 事件对象的指针
 */
static __xwup_code
struct xwup_evt * xwup_evt_alloc(xwsz_t num)
{
#if defined(XWUPCFG_SYNC_EVT_STDC_MM) && (1 == XWUPCFG_SYNC_EVT_STDC_MM)
        struct xwup_evt * evt;
        xwbmp_t * bmp, * msk;
        xwsz_t bmpnum, bmpsize;

        bmpnum = BITS_TO_XWBMP_T(num);
        bmpsize = bmpnum * sizeof(xwbmp_t);
        evt = malloc(sizeof(struct xwup_evt) + bmpsize + bmpsize);
        if (NULL == evt) {
                evt = err_ptr(-ENOMEM);
        } else {
                bmp = (void *)&evt[1];
                msk = &bmp[bmpnum];
                xwup_evt_setup(evt, num, bmp, msk);
        }
        return evt;
#else
        union {
                struct xwup_evt * evt;
                void * anon;
        } mem;
        xwbmp_t * bmp, * msk;
        xwsz_t bmpnum, bmpsize;
        xwer_t rc;

        bmpnum = BITS_TO_XWBMP_T(num);
        bmpsize = bmpnum * sizeof(xwbmp_t);
        rc = xwmm_kma_alloc(sizeof(struct xwup_evt) + bmpsize + bmpsize,
                            XWMM_ALIGNMENT, &mem.anon);
        if (XWOK == rc) {
                bmp = (void *)&mem.evt[1];
                msk = &bmp[bmpnum];
                xwup_evt_setup(mem.evt, num, bmp, msk);
        } else {
                mem.evt = err_ptr(-ENOMEM);
        }
        return mem.evt;
#endif
}

/**
 * @brief 释放事件对象
 * @param[in] evt: 事件对象的指针
 */
static __xwup_code
void xwup_evt_free(struct xwup_evt * evt)
{
#if defined(XWUPCFG_SYNC_EVT_STDC_MM) && (1 == XWUPCFG_SYNC_EVT_STDC_MM)
        free(evt);
#else
        xwmm_kma_free(evt);
#endif
}

/**
 * @brief 激活事件对象
 * @param[in] evt: 事件对象的指针
 * @param[in] type: 事件的类型，取值：
 *   @arg XWUP_EVT_TYPE_FLG: 事件标志
 *   @arg XWUP_EVT_TYPE_SEL: 信号选择器
 *   @arg XWUP_EVT_TYPE_BR: 线程栅栏
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
static __xwup_code
void xwup_evt_activate(struct xwup_evt * evt, xwsq_t type)
{
        xwsz_t size;

        size = BITS_TO_XWBMP_T(evt->num);
        xwup_cond_activate(&evt->cond);
        evt->type = type;
        switch (type & XWUP_EVT_TYPE_MASK) {
        case XWUP_EVT_TYPE_FLG:
                memset(evt->msk, 0xFF, size);
                break;
        case XWUP_EVT_TYPE_BR:
                memset(evt->msk, 0, size);
                break;
        case XWUP_EVT_TYPE_SEL:
                memset(evt->msk, 0, size);
                break;
        default:
                memset(evt->msk, 0xFF, size);
                break;
        }
        memset(evt->bmp, 0, size);
        xwup_splk_init(&evt->lock);
}

/**
 * @brief 使得事件对象无效
 * @param[in] evt: 事件对象的指针
 */
static __xwup_code
void xwup_evt_deactivate(struct xwup_evt * evt)
{
        XWOS_UNUSED(evt);
}

__xwup_api
xwer_t xwup_evt_create(struct xwup_evt ** ptrbuf, xwsq_t type, xwsz_t num)
{
        struct xwup_evt * evt;
        xwer_t rc;

        XWOS_VALIDATE((ptrbuf), "nullptr", -EFAULT);
        XWOS_VALIDATE(((type & XWUP_EVT_TYPE_MASK) < XWUP_EVT_TYPE_MAX),
                      "type-error", -EINVAL);

        *ptrbuf = NULL;
        evt = xwup_evt_alloc(num);
        if (__xwcc_unlikely(is_err(evt))) {
                rc = ptr_err(evt);
        } else {
                xwup_evt_activate(evt, type);
                *ptrbuf = evt;
                rc = XWOK;
        }
        return rc;
}

__xwup_api
xwer_t xwup_evt_delete(struct xwup_evt * evt)
{
        XWOS_VALIDATE((evt), "nullptr", -EFAULT);

        xwup_evt_deactivate(evt);
        xwup_evt_free(evt);
        return XWOK;
}

__xwup_api
xwer_t xwup_evt_init(struct xwup_evt * evt, xwsq_t type, xwsz_t num,
                     xwbmp_t * bmp, xwbmp_t * msk)
{
        XWOS_VALIDATE((evt), "nullptr", -EFAULT);
        XWOS_VALIDATE(((type & XWUP_EVT_TYPE_MASK) < XWUP_EVT_TYPE_MAX),
                      "type-error", -EINVAL);

        xwup_evt_setup(evt, num, bmp, msk);
        xwup_evt_activate(evt, type);
        return XWOK;
}

__xwup_api
xwer_t xwup_evt_fini(struct xwup_evt * evt)
{
        XWOS_VALIDATE((evt), "nullptr", -EFAULT);

        xwup_evt_deactivate(evt);
        return XWOK;
}

__xwup_api
xwer_t xwup_evt_bind(struct xwup_evt * evt, struct xwup_evt * sel, xwsq_t pos)
{
        return xwup_cond_bind(&evt->cond, sel, pos);
}

__xwup_api
xwer_t xwup_evt_unbind(struct xwup_evt * evt, struct xwup_evt * sel)
{
        return xwup_cond_unbind(&evt->cond, sel);
}

__xwup_api
xwer_t xwup_evt_intr_all(struct xwup_evt * evt)
{
        XWOS_VALIDATE((evt), "nullptr", -EFAULT);

        return xwup_cond_intr_all(&evt->cond);
}

__xwup_api
xwer_t xwup_evt_get_num(struct xwup_evt * evt, xwsz_t * numbuf)
{
        XWOS_VALIDATE((evt), "nullptr", -EFAULT);

        *numbuf = evt->num;
        return XWOK;
}

/******** type:XWUP_EVT_TYPE_FLG ********/
__xwup_api
xwer_t xwup_flg_s1m(struct xwup_evt * evt, xwbmp_t msk[])
{
        xwreg_t cpuirq;

        XWOS_VALIDATE((evt), "nullptr", -EFAULT);
        XWOS_VALIDATE((msk), "nullptr", -EFAULT);
        XWOS_VALIDATE(((evt->type & XWUP_EVT_TYPE_MASK) == XWUP_EVT_TYPE_FLG),
                      "type-error", -ETYPE);

        xwup_splk_lock_cpuirqsv(&evt->lock, &cpuirq);
        xwbmpop_s1m(evt->bmp, msk, evt->num);
        xwup_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
        xwup_cond_broadcast(&evt->cond);
        return XWOK;
}

__xwup_api
xwer_t xwup_flg_s1i(struct xwup_evt * evt, xwsq_t pos)
{
        xwreg_t cpuirq;
        xwer_t rc;

        XWOS_VALIDATE((evt), "nullptr", -EFAULT);
        XWOS_VALIDATE(((evt->type & XWUP_EVT_TYPE_MASK) == XWUP_EVT_TYPE_FLG),
                      "type-error", -ETYPE);

        if (pos >= evt->num) {
                rc = -ECHRNG;
                goto err_pos_range;
        }
        xwup_splk_lock_cpuirqsv(&evt->lock, &cpuirq);
        xwbmpop_s1i(evt->bmp, pos);
        xwup_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
        xwup_cond_broadcast(&evt->cond);
        return XWOK;

err_pos_range:
        return rc;
}

__xwup_api
xwer_t xwup_flg_c0m(struct xwup_evt * evt, xwbmp_t msk[])
{
        xwreg_t cpuirq;

        XWOS_VALIDATE((evt), "nullptr", -EFAULT);
        XWOS_VALIDATE((msk), "nullptr", -EFAULT);
        XWOS_VALIDATE(((evt->type & XWUP_EVT_TYPE_MASK) == XWUP_EVT_TYPE_FLG),
                      "type-error", -ETYPE);

        xwup_splk_lock_cpuirqsv(&evt->lock, &cpuirq);
        xwbmpop_c0m(evt->bmp, msk, evt->num);
        xwup_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
        xwup_cond_broadcast(&evt->cond);
        return XWOK;
}

__xwup_api
xwer_t xwup_flg_c0i(struct xwup_evt * evt, xwsq_t pos)
{
        xwreg_t cpuirq;
        xwer_t rc;

        XWOS_VALIDATE((evt), "nullptr", -EFAULT);
        XWOS_VALIDATE(((evt->type & XWUP_EVT_TYPE_MASK) == XWUP_EVT_TYPE_FLG),
                      "type-error", -ETYPE);

        if (pos >= evt->num) {
                rc = -ECHRNG;
                goto err_pos_range;
        }
        xwup_splk_lock_cpuirqsv(&evt->lock, &cpuirq);
        xwbmpop_c0i(evt->bmp, pos);
        xwup_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
        xwup_cond_broadcast(&evt->cond);
        return XWOK;

err_pos_range:
        return rc;
}

__xwup_api
xwer_t xwup_flg_x1m(struct xwup_evt * evt, xwbmp_t msk[])
{
        xwreg_t cpuirq;

        XWOS_VALIDATE((evt), "nullptr", -EFAULT);
        XWOS_VALIDATE((msk), "nullptr", -EFAULT);
        XWOS_VALIDATE(((evt->type & XWUP_EVT_TYPE_MASK) == XWUP_EVT_TYPE_FLG),
                      "type-error", -ETYPE);

        xwup_splk_lock_cpuirqsv(&evt->lock, &cpuirq);
        xwbmpop_x1m(evt->bmp, msk, evt->num);
        xwup_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
        xwup_cond_broadcast(&evt->cond);
        return XWOK;
}

__xwup_api
xwer_t xwup_flg_x1i(struct xwup_evt * evt, xwsq_t pos)
{
        xwreg_t cpuirq;
        xwer_t rc;

        XWOS_VALIDATE((evt), "nullptr", -EFAULT);
        XWOS_VALIDATE(((evt->type & XWUP_EVT_TYPE_MASK) == XWUP_EVT_TYPE_FLG),
                      "type-error", -ETYPE);

        if (pos >= evt->num) {
                rc = -ECHRNG;
                goto err_pos_range;
        }
        xwup_splk_lock_cpuirqsv(&evt->lock, &cpuirq);
        xwbmpop_x1i(evt->bmp, pos);
        xwup_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
        xwup_cond_broadcast(&evt->cond);
        return XWOK;

err_pos_range:
        return rc;
}

__xwup_api
xwer_t xwup_flg_read(struct xwup_evt * evt, xwbmp_t out[])
{
        xwreg_t cpuirq;

        XWOS_VALIDATE((evt), "nullptr", -EFAULT);
        XWOS_VALIDATE((out), "nullptr", -EFAULT);

        xwup_splk_lock_cpuirqsv(&evt->lock, &cpuirq);
        xwbmpop_assign(out, evt->bmp, evt->num);
        xwup_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
        return XWOK;
}

__xwup_api
xwer_t xwup_flg_wait(struct xwup_evt * evt,
                     xwsq_t trigger, xwsq_t action,
                     xwbmp_t origin[], xwbmp_t msk[])
{
        xwtm_t expected = XWTM_MAX;
        return xwup_flg_timedwait(evt, trigger, action, origin, msk, &expected);
}

static __xwup_code
xwer_t xwup_flg_trywait_level(struct xwup_evt * evt,
                              xwsq_t trigger, xwsq_t action,
                              xwbmp_t origin[], xwbmp_t msk[])
{
        xwreg_t cpuirq;
        bool triggered;
        xwer_t rc;

        XWOS_VALIDATE((trigger <= XWUP_FLG_TRIGGER_CLR_ANY),
                      "illegal-trigger", -EINVAL);
        XWOS_VALIDATE((action < XWUP_FLG_ACTION_NUM),
                      "illegal-action", -EINVAL);

        rc = XWOK;
        xwup_splk_lock_cpuirqsv(&evt->lock, &cpuirq);
        if (origin) {
                xwbmpop_assign(origin, evt->bmp, evt->num);
        }
        if (XWUP_FLG_ACTION_CONSUMPTION == action) {
                switch (trigger) {
                case XWUP_FLG_TRIGGER_SET_ALL:
                        triggered = xwbmpop_t1ma_then_c0m(evt->bmp, msk, evt->num);
                        break;
                case XWUP_FLG_TRIGGER_SET_ANY:
                        triggered = xwbmpop_t1mo_then_c0m(evt->bmp, msk, evt->num);
                        break;
                case XWUP_FLG_TRIGGER_CLR_ALL:
                        triggered = xwbmpop_t0ma_then_s1m(evt->bmp, msk, evt->num);
                        break;
                case XWUP_FLG_TRIGGER_CLR_ANY:
                        triggered = xwbmpop_t0mo_then_s1m(evt->bmp, msk, evt->num);
                        break;
                default:
                        triggered = true;
                        rc = -EINVAL;
                        break;
                }
        } else {
                switch (trigger) {
                case XWUP_FLG_TRIGGER_SET_ALL:
                        triggered = xwbmpop_t1ma(evt->bmp, msk, evt->num);
                        break;
                case XWUP_FLG_TRIGGER_SET_ANY:
                        triggered = xwbmpop_t1mo(evt->bmp, msk, evt->num);
                        break;
                case XWUP_FLG_TRIGGER_CLR_ALL:
                        triggered = xwbmpop_t0ma(evt->bmp, msk, evt->num);
                        break;
                case XWUP_FLG_TRIGGER_CLR_ANY:
                        triggered = xwbmpop_t0mo(evt->bmp, msk, evt->num);
                        break;
                default:
                        triggered = true;
                        rc = -EINVAL;
                        break;
                }
        }
        xwup_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
        if (!triggered) {
                rc = -ENODATA;
        }
        return rc;
}

static __xwup_code
xwer_t xwup_flg_trywait_edge(struct xwup_evt * evt, xwsq_t trigger,
                             xwbmp_t origin[], xwbmp_t msk[])
{
        xwreg_t cpuirq;
        xwssq_t cmprc;
        bool triggered;
        xwer_t rc;
        xwbmpop_declare(cur, evt->num);
        xwbmpop_declare(tmp, evt->num);

        XWOS_VALIDATE((origin), "nullptr", -EFAULT);

        xwbmpop_and(origin, msk, evt->num);
        xwup_splk_lock_cpuirqsv(&evt->lock, &cpuirq);
        xwbmpop_assign(cur, evt->bmp, evt->num);
        xwbmpop_and(cur, msk, evt->num);
        if (XWUP_FLG_TRIGGER_TGL_ALL == trigger) {
                xwbmpop_assign(tmp, cur, evt->num);
                xwbmpop_xor(tmp, origin, evt->num);
                cmprc = xwbmpop_cmp(tmp, msk, evt->num);
                if (0 == cmprc) {
                        triggered = true;
                        rc = XWOK;
                } else {
                        triggered = false;
                        rc = -ENODATA;
                }
        } else if (XWUP_FLG_TRIGGER_TGL_ANY == trigger) {
                cmprc = xwbmpop_cmp(origin, cur, evt->num);
                if (0 == cmprc) {
                        triggered = false;
                        rc = -ENODATA;
                } else {
                        triggered = true;
                        rc = XWOK;
                }
        } else {
                triggered = true;
                rc = -EINVAL;
        }
        if (triggered) {
                xwbmpop_assign(origin, cur, evt->num);
                xwup_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
        } else {
                xwup_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
        }
        return rc;
}

__xwup_api
xwer_t xwup_flg_trywait(struct xwup_evt * evt,
                        xwsq_t trigger, xwsq_t action,
                        xwbmp_t origin[], xwbmp_t msk[])
{
        xwer_t rc;

        XWOS_VALIDATE((evt), "nullptr", -EFAULT);
        XWOS_VALIDATE((msk), "nullptr", -EFAULT);
        XWOS_VALIDATE(((evt->type & XWUP_EVT_TYPE_MASK) == XWUP_EVT_TYPE_FLG),
                      "type-error", -ETYPE);

        if (trigger <= XWUP_FLG_TRIGGER_CLR_ANY) {
                rc = xwup_flg_trywait_level(evt, trigger, action, origin, msk);
        } else {
                rc = xwup_flg_trywait_edge(evt, trigger, origin, msk);
        }
        return rc;
}

static __xwup_code
xwer_t xwup_flg_timedwait_level(struct xwup_evt * evt,
                                xwsq_t trigger, xwsq_t action,
                                xwbmp_t origin[], xwbmp_t msk[],
                                xwtm_t * xwtm)
{
        xwreg_t cpuirq;
        bool triggered;
        xwsq_t lkst;
        xwer_t rc;

        XWOS_VALIDATE((trigger <= XWUP_FLG_TRIGGER_CLR_ANY),
                      "illegal-trigger", -EINVAL);
        XWOS_VALIDATE((action < XWUP_FLG_ACTION_NUM),
                      "illegal-action", -EINVAL);

        rc = XWOK;
        xwup_splk_lock_cpuirqsv(&evt->lock, &cpuirq);
        while (true) {
                if (origin) {
                        xwbmpop_assign(origin, evt->bmp, evt->num);
                }
                if (XWUP_FLG_ACTION_CONSUMPTION == action) {
                        switch (trigger) {
                        case XWUP_FLG_TRIGGER_SET_ALL:
                                triggered = xwbmpop_t1ma_then_c0m(evt->bmp, msk,
                                                                  evt->num);
                                break;
                        case XWUP_FLG_TRIGGER_SET_ANY:
                                triggered = xwbmpop_t1mo_then_c0m(evt->bmp, msk,
                                                                  evt->num);
                                break;
                        case XWUP_FLG_TRIGGER_CLR_ALL:
                                triggered = xwbmpop_t0ma_then_s1m(evt->bmp, msk,
                                                                  evt->num);
                                break;
                        case XWUP_FLG_TRIGGER_CLR_ANY:
                                triggered = xwbmpop_t0mo_then_s1m(evt->bmp, msk,
                                                                  evt->num);
                                break;
                        default:
                                triggered = true;
                                rc = -EINVAL;
                                break;
                        }
                } else {
                        switch (trigger) {
                        case XWUP_FLG_TRIGGER_SET_ALL:
                                triggered = xwbmpop_t1ma(evt->bmp, msk, evt->num);
                                break;
                        case XWUP_FLG_TRIGGER_SET_ANY:
                                triggered = xwbmpop_t1mo(evt->bmp, msk, evt->num);
                                break;
                        case XWUP_FLG_TRIGGER_CLR_ALL:
                                triggered = xwbmpop_t0ma(evt->bmp, msk, evt->num);
                                break;
                        case XWUP_FLG_TRIGGER_CLR_ANY:
                                triggered = xwbmpop_t0mo(evt->bmp, msk, evt->num);
                                break;
                        default:
                                triggered = true;
                                rc = -EINVAL;
                                break;
                        }
                }
                if (triggered) {
                        xwup_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
                        break;
                } else {
                        rc = xwup_cond_timedwait(&evt->cond,
                                                 &evt->lock, XWOS_LK_SPLK, NULL,
                                                 xwtm, &lkst);
                        if (XWOK == rc) {
                                if (XWOS_LKST_UNLOCKED == lkst) {
                                        xwup_splk_lock(&evt->lock);
                                }
                        } else {
                                if (XWOS_LKST_LOCKED == lkst) {
                                        xwup_splk_unlock(&evt->lock);
                                }
                                xwospl_cpuirq_restore_lc(cpuirq);
                                break;
                        }
                }
        }
        return rc;
}

static __xwup_code
xwer_t xwup_flg_timedwait_edge(struct xwup_evt * evt, xwsq_t trigger,
                               xwbmp_t origin[], xwbmp_t msk[],
                               xwtm_t * xwtm)
{
        xwreg_t cpuirq;
        xwsq_t lkst;
        xwssq_t cmprc;
        bool triggered;
        xwer_t rc;
        xwbmpop_declare(cur, evt->num);
        xwbmpop_declare(tmp, evt->num);

        XWOS_VALIDATE((origin), "nullptr", -EFAULT);

        xwbmpop_and(origin, msk, evt->num);
        xwup_splk_lock_cpuirqsv(&evt->lock, &cpuirq);
        while (true) {
                xwbmpop_assign(cur, evt->bmp, evt->num);
                xwbmpop_and(cur, msk, evt->num);
                if (XWUP_FLG_TRIGGER_TGL_ALL == trigger) {
                        xwbmpop_assign(tmp, cur, evt->num);
                        xwbmpop_xor(tmp, origin, evt->num);
                        cmprc = xwbmpop_cmp(tmp, msk, evt->num);
                        if (0 == cmprc) {
                                triggered = true;
                                rc = XWOK;
                        } else {
                                triggered = false;
                        }
                } else if (XWUP_FLG_TRIGGER_TGL_ANY == trigger) {
                        cmprc = xwbmpop_cmp(origin, cur, evt->num);
                        if (0 == cmprc) {
                                triggered = false;
                        } else {
                                triggered = true;
                                rc = XWOK;
                        }
                } else {
                        triggered = true;
                        rc = -EINVAL;
                }
                if (triggered) {
                        xwbmpop_assign(origin, cur, evt->num);
                        xwup_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
                        break;
                } else {
                        rc = xwup_cond_timedwait(&evt->cond,
                                                 &evt->lock, XWOS_LK_SPLK, NULL,
                                                 xwtm, &lkst);
                        if (XWOK == rc) {
                                if (XWOS_LKST_UNLOCKED == lkst) {
                                        xwup_splk_lock(&evt->lock);
                                }
                        } else {
                                if (XWOS_LKST_LOCKED == lkst) {
                                        xwup_splk_unlock(&evt->lock);
                                }
                                xwospl_cpuirq_restore_lc(cpuirq);
                                break;
                        }
                }
        }
        return rc;
}

__xwup_api
xwer_t xwup_flg_timedwait(struct xwup_evt * evt,
                          xwsq_t trigger, xwsq_t action,
                          xwbmp_t origin[], xwbmp_t msk[],
                          xwtm_t * xwtm)
{
        xwer_t rc;

        XWOS_VALIDATE((evt), "nullptr", -EFAULT);
        XWOS_VALIDATE((msk), "nullptr", -EFAULT);
        XWOS_VALIDATE(((evt->type & XWUP_EVT_TYPE_MASK) == XWUP_EVT_TYPE_FLG),
                      "type-error", -ETYPE);
        XWOS_VALIDATE((xwtm), "nullptr", -EFAULT);
        XWOS_VALIDATE((-EINTHD == xwup_irq_get_id(NULL)),
                      "not-in-thd", -ENOTINTHD);

        if (trigger <= XWUP_FLG_TRIGGER_CLR_ANY) {
                rc = xwup_flg_timedwait_level(evt, trigger, action, origin, msk, xwtm);
        } else {
                rc = xwup_flg_timedwait_edge(evt, trigger, origin, msk, xwtm);
        }
        return rc;
}

/******** type:XWUP_EVT_TYPE_SEL ********/
/**
 * @brief 绑定同步对象到事件对象，事件对象类型为XWUP_EVT_TYPE_SEL
 * @param[in] evt: 事件对象的指针
 * @param[in] obj: 同步对象的指针
 * @param[in] pos: 同步对象映射到位图中的位置
 * @param[in] exclusive: 是否为独占绑定
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -ETYPE: 事件对象类型错误
 * @retval -ECHRNG: 位置超出范围
 * @retval -EALREADY: 同步对象已经绑定到事件对象
 * @retval -EBUSY: 通道已经被其他同步对象独占
 */
__xwup_code
xwer_t xwup_sel_obj_bind(struct xwup_evt * evt,
                         struct xwup_synobj * synobj,
                         xwsq_t pos,
                         bool exclusive)
{
        struct xwup_evt * owner;
        xwreg_t cpuirq;
        bool existed;
        xwer_t rc;

        XWOS_VALIDATE(((evt->type & XWUP_EVT_TYPE_MASK) == XWUP_EVT_TYPE_SEL),
                      "type-error", -ETYPE);

        if (pos >= evt->num) {
                rc = -ECHRNG;
                goto err_pos_range;
        }
        xwup_splk_lock_cpuirqsv(&evt->lock, &cpuirq);
        owner = synobj->sel.evt;
        if (NULL != owner) {
                rc = -EALREADY;
                goto err_already;
        }
        existed = xwbmpop_t1i(evt->msk, pos);
        if (existed) {
                rc = -EBUSY;
                goto err_busy;
        }
        if (exclusive) {
                xwbmpop_s1i(evt->msk, pos);
        }
        synobj->sel.evt = evt;
        synobj->sel.pos = pos;
        xwup_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
        return XWOK;

err_busy:
err_already:
        xwup_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
err_pos_range:
        return rc;
}

/**
 * @brief 从事件对象上解绑同步对象，事件对象类型为XWUP_EVT_TYPE_SEL
 * @param[in] evt: 事件对象的指针
 * @param[in] obj: 同步对象的指针
 * @param[in] exclusive: 是否为独占绑定
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -ETYPE: 事件对象类型错误
 * @retval -ENOTCONN: 同步对象没有绑定到事件对象上
 */
__xwup_code
xwer_t xwup_sel_obj_unbind(struct xwup_evt * evt,
                           struct xwup_synobj * synobj,
                           bool exclusive)
{
        struct xwup_evt * owner;
        xwreg_t cpuirq;
        xwer_t rc;

        XWOS_VALIDATE(((evt->type & XWUP_EVT_TYPE_MASK) == XWUP_EVT_TYPE_SEL),
                      "type-error", -ETYPE);

        xwup_splk_lock_cpuirqsv(&evt->lock, &cpuirq);
        owner = synobj->sel.evt;
        if (evt != owner) {
                rc = -ENOTCONN;
                goto err_notconn;
        }
        if (exclusive) {
                xwbmpop_c0i(evt->msk, synobj->sel.pos);
        }
        synobj->sel.evt = NULL;
        synobj->sel.pos = 0;
        xwup_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
        return XWOK;

err_notconn:
        xwup_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
        return rc;
}

/**
 * @brief 在事件对象上设置同步对象的标志位，事件对象类型为XWUP_EVT_TYPE_SEL
 * @param[in] evt: 事件对象的指针
 * @param[in] obj: 同步对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -ENOTCONN: 同步对象没有绑定到事件对象上
 */
__xwup_code
xwer_t xwup_sel_obj_s1i(struct xwup_evt * evt, struct xwup_synobj * synobj)
{
        struct xwup_evt * owner;
        xwreg_t cpuirq;
        xwer_t rc;

        xwup_splk_lock_cpuirqsv(&evt->lock, &cpuirq);
        owner = synobj->sel.evt;
        if (evt != owner) {
                rc = -ENOTCONN;
                goto err_notconn;
        }
        xwbmpop_s1i(evt->bmp, synobj->sel.pos);
        xwup_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
        xwup_cond_broadcast(&evt->cond);
        return XWOK;

err_notconn:
        xwup_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
        return rc;
}

/**
 * @brief 在事件对象上清除同步对象的标志位，事件对象类型为XWUP_EVT_TYPE_SEL
 * @param[in] evt: 事件对象的指针
 * @param[in] obj: 同步对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -ENOTCONN: 同步对象没有绑定到事件对象上
 */
__xwup_code
xwer_t xwup_sel_obj_c0i(struct xwup_evt * evt, struct xwup_synobj * synobj)
{
        struct xwup_evt * owner;
        xwreg_t cpuirq;
        xwer_t rc;

        xwup_splk_lock_cpuirqsv(&evt->lock, &cpuirq);
        owner = synobj->sel.evt;
        if (evt != owner) {
                rc = -ENOTCONN;
                goto err_notconn;
        }
        xwbmpop_c0i(evt->bmp, synobj->sel.pos);
        xwup_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
        return XWOK;

err_notconn:
        xwup_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
        return rc;
}

__xwup_api
xwer_t xwup_sel_select(struct xwup_evt * evt, xwbmp_t msk[], xwbmp_t trg[])
{
        xwtm_t expected = XWTM_MAX;
        return xwup_sel_timedselect(evt, msk, trg, &expected);
}

__xwup_api
xwer_t xwup_sel_tryselect(struct xwup_evt * evt, xwbmp_t msk[], xwbmp_t trg[])
{
        xwer_t rc;
        xwreg_t cpuirq;
        bool triggered;

        XWOS_VALIDATE((evt), "nullptr", -EFAULT);
        XWOS_VALIDATE((msk), "nullptr", -EFAULT);
        XWOS_VALIDATE(((evt->type & XWUP_EVT_TYPE_MASK) == XWUP_EVT_TYPE_SEL),
                      "type-error", -ETYPE);

        xwup_splk_lock_cpuirqsv(&evt->lock, &cpuirq);
        triggered = xwbmpop_t1mo(evt->bmp, msk, evt->num);
        if (triggered) {
                if (NULL != trg) {
                        xwbmpop_assign(trg, evt->bmp, evt->num);
                        /* Clear non-exclusive bits */
                        xwbmpop_and(evt->bmp, evt->msk, evt->num);
                        xwup_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
                        xwbmpop_and(trg, msk, evt->num);
                } else {
                        /* Clear non-exclusive bits */
                        xwbmpop_and(evt->bmp, evt->msk, evt->num);
                        xwup_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
                }
                rc = XWOK;
        } else {
                rc = -ENODATA;
                xwup_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
        }
        return rc;
}

__xwup_api
xwer_t xwup_sel_timedselect(struct xwup_evt * evt, xwbmp_t msk[], xwbmp_t trg[],
                            xwtm_t * xwtm)
{
        xwer_t rc;
        xwreg_t cpuirq;
        bool triggered;
        xwsq_t lkst;

        XWOS_VALIDATE((evt), "nullptr", -EFAULT);
        XWOS_VALIDATE((msk), "nullptr", -EFAULT);
        XWOS_VALIDATE(((evt->type & XWUP_EVT_TYPE_MASK) == XWUP_EVT_TYPE_SEL),
                      "type-error", -ETYPE);
        XWOS_VALIDATE((xwtm), "nullptr", -EFAULT);
        XWOS_VALIDATE((-EINTHD == xwup_irq_get_id(NULL)),
                      "not-in-thd", -ENOTINTHD);

        xwup_splk_lock_cpuirqsv(&evt->lock, &cpuirq);
        while (true) {
                triggered = xwbmpop_t1mo(evt->bmp, msk, evt->num);
                if (triggered) {
                        if (NULL != trg) {
                                xwbmpop_assign(trg, evt->bmp, evt->num);
                                /* Clear non-exclusive bits */
                                xwbmpop_and(evt->bmp, evt->msk, evt->num);
                                xwup_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
                                xwbmpop_and(trg, msk, evt->num);
                        } else {
                                /* Clear non-exclusive bits */
                                xwbmpop_and(evt->bmp, evt->msk, evt->num);
                                xwup_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
                        }
                        rc = XWOK;
                        break;
                } else {
                        /* Clear non-exclusive bits */
                        xwbmpop_and(evt->bmp, evt->msk, evt->num);
                        rc = xwup_cond_timedwait(&evt->cond,
                                                 &evt->lock, XWOS_LK_SPLK, NULL,
                                                 xwtm, &lkst);
                        if (XWOK == rc) {
                                if (XWOS_LKST_UNLOCKED == lkst) {
                                        xwup_splk_lock(&evt->lock);
                                }
                        } else {
                                if (XWOS_LKST_LOCKED == lkst) {
                                        xwup_splk_unlock(&evt->lock);
                                }
                                xwospl_cpuirq_restore_lc(cpuirq);
                                if (NULL != trg) {
                                        xwbmpop_c0all(trg, evt->num);
                                }
                                break;
                        }
                }
        }
        return rc;
}

/******** type:XWUP_EVT_TYPE_BR ********/
__xwup_api
xwer_t xwup_br_sync(struct xwup_evt * evt, xwsq_t pos, xwbmp_t msk[])
{
        xwtm_t expected = XWTM_MAX;
        return xwup_br_timedsync(evt, pos, msk, &expected);
}

__xwup_api
xwer_t xwup_br_timedsync(struct xwup_evt * evt, xwsq_t pos, xwbmp_t msk[],
                         xwtm_t * xwtm)
{
        xwreg_t cpuirq;
        bool triggered;
        xwsq_t lkst;
        xwer_t rc;

        XWOS_VALIDATE((evt), "nullptr", -EFAULT);
        XWOS_VALIDATE((msk), "nullptr", -EFAULT);
        XWOS_VALIDATE(((evt->type & XWUP_EVT_TYPE_MASK) == XWUP_EVT_TYPE_BR),
                      "type-error", -ETYPE);
        XWOS_VALIDATE((xwtm), "nullptr", -EFAULT);
        XWOS_VALIDATE((-EINTHD == xwup_irq_get_id(NULL)),
                      "not-in-thd", -ENOTINTHD);

        if (pos >= evt->num) {
                rc = -ECHRNG;
                goto err_pos_range;
        }
        xwup_splk_lock_cpuirqsv(&evt->lock, &cpuirq);
        xwbmpop_s1i(evt->bmp, pos);
        triggered = xwbmpop_t1ma(evt->bmp, msk, evt->num);
        if (triggered) {
                xwbmpop_c0i(evt->bmp, pos);
                xwup_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
                xwup_cond_broadcast(&evt->cond);
                xwup_cthd_yield();
                rc = XWOK;
        } else {
                rc = xwup_cond_timedwait(&evt->cond,
                                         &evt->lock, XWOS_LK_SPLK, NULL,
                                         xwtm, &lkst);
                if (XWOS_LKST_UNLOCKED == lkst) {
                        xwup_splk_lock(&evt->lock);
                }
                xwbmpop_c0i(evt->bmp, pos);
                xwup_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
        }
        return XWOK;

err_pos_range:
        return rc;
}
