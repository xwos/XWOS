/**
 * @file
 * @brief xwos::autosar::cp::os::counter
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#include <xwos/arcos/counter.h>
#include <xwos/osal/skd.h>

xwer_t xwarcos_counter_node_validate(struct xwarcos_counter_node * node)
{
        xwer_t rc;

        if (NULL == node) {
                rc = -E_XWARCOS_ILLEGAL_ADDRESS;
                goto err_nullptr;
        }
        if (NULL == node->operation) {
                rc = -E_XWARCOS_NOFUNC;
                goto err_nullptr;
        }
        if (xwos_skd_get_cpuid_lc() != node->core_id) {
                rc = -E_XWARCOS_CORE;
                goto err_core;
        }
        if (NULL != node->counter) {
                rc = -E_XWARCOS_VALUE;
                goto err_value;
        }
        rc = E_XWARCOS_OK;

err_value:
err_core:
err_nullptr:
        return rc;
}

xwer_t xwarcos_counter_add(struct xwarcos_counter * counter,
                           struct xwarcos_counter_node * node,
                           void (* post_add)(struct xwarcos_counter_node *))
{
        xwer_t rc;
        xwreg_t cpuirq;
        const struct xwarcos_counter_config * cfg;

        if (NULL == counter) {
                rc = -E_XWARCOS_ILLEGAL_ADDRESS;
                goto err_nullptr;
        }
        if (NULL == node) {
                rc = -E_XWARCOS_ILLEGAL_ADDRESS;
                goto err_nullptr;
        }
        cfg = counter->cfg;
        if (cfg->core_id != node->core_id) {
                /* [SWS_Os_00630], [SWS_Os_00631] */
                rc = -E_XWARCOS_CORE;
                goto err_core;
        }
        rc = E_XWARCOS_OK;
        xwlib_bclst_init_node(&node->bcln);
        xwos_sqlk_wr_lock_cpuirqsv(&counter->lock, &cpuirq);
        node->counter = counter;
        xwlib_bclst_add_head(&counter->head, &node->bcln);
        if (NULL != post_add) {
                post_add(node);
        }
        xwos_sqlk_wr_unlock_cpuirqrs(&counter->lock, cpuirq);

err_core:
err_nullptr:
        return rc;
}

xwer_t xwarcos_counter_del(struct xwarcos_counter * counter,
                           struct xwarcos_counter_node * node,
                           void (* pre_del)(struct xwarcos_counter_node *))
{
        xwer_t rc;
        xwreg_t cpuirq;

        if (NULL == counter) {
                rc = -E_XWARCOS_ILLEGAL_ADDRESS;
                goto err_nullptr;
        }
        if (NULL == node) {
                rc = -E_XWARCOS_ILLEGAL_ADDRESS;
                goto err_nullptr;
        }
        xwos_sqlk_wr_lock_cpuirqsv(&counter->lock, &cpuirq);
        if (node->counter == counter) {
                if (NULL != pre_del) {
                        pre_del(node);
                }
                xwlib_bclst_del_init(&node->bcln);
                node->counter = NULL;
                rc = E_XWARCOS_OK;
        } else {
                rc = -E_XWARCOS_CORE;
        }
        xwos_sqlk_wr_unlock_cpuirqrs(&counter->lock, cpuirq);

err_nullptr:
        return rc;
}

xwer_t xwarcos_counter_validate_and_init(struct xwarcos_counter * counter)
{
        XWOS_UNUSED(counter);
        /* TODO */
        return E_XWARCOS_OK;
}

/* [SWS_Os_00301] */
xwer_t xwarcos_counter_increment(struct xwarcos_counter * counter)
{
        xwer_t rc;
        xwreg_t cpuirq;
        bool proceed;
        const struct xwarcos_counter_config * cfg;
        struct xwarcos_counter_node * node;
        struct xwarcos_counter_node * next;

        if (NULL == counter) {
                /* [SWS_Os_00285] */
                rc = -E_XWARCOS_ID;
                goto err_id;
        }
        cfg = counter->cfg;
        if (cfg->core_id != xwos_skd_get_cpuid_lc()) {
                /* [SWS_Os_00629] */
                rc = -E_XWARCOS_CORE;
                goto err_core;
        }
        /* [SWS_Os_00286], [SWS_Os_00321], [SWS_Os_00529] */
        rc = E_XWARCOS_OK;
        xwos_sqlk_wr_lock_cpuirqsv(&counter->lock, &cpuirq);
        counter->value += cfg->ticks_per_base;
        if (counter->value > cfg->max_allowed_value) {
                counter->value = 0;
        }
        xwlib_bclst_itr_next_entry_safe(node, next, &counter->head,
                                        struct xwarcos_counter_node, bcln) {
                if (NULL != node->operation) {
                        proceed = node->operation(counter, node);
                        if (!proceed) {
                                xwlib_bclst_del_init(&node->bcln);
                                node->counter = NULL;
                        }
                }
        }
        xwos_sqlk_wr_unlock_cpuirqrs(&counter->lock, cpuirq);

err_core:
err_id:
        return rc;
}

xwer_t xwarcos_counter_get_value(struct xwarcos_counter * counter,
                                 xwtk_t * value)
{
        xwer_t rc;
        xwsq_t seq;
        xwtk_t tick;

        if (NULL == counter) {
                /* [SWS_Os_00376] */
                rc = -E_XWARCOS_ID;
                goto err_id;
        }
        if (NULL == value) {
                rc = -E_XWARCOS_ILLEGAL_ADDRESS;
                goto err_nullptr;
        }
        /* [SWS_Os_00377] */
        rc = E_XWARCOS_OK;
        do {
                /* [SWS_Os_00531] */
                seq = xwos_sqlk_rd_begin(&counter->lock);
                tick = counter->value;
        } while (xwos_sqlk_rd_retry(&counter->lock, seq));
        *value = tick;

err_nullptr:
err_id:
        return rc;
}

xwer_t xwarcos_counter_get_elapsed_value(struct xwarcos_counter * counter,
                                         xwtk_t * value, xwtk_t * elapsed)
{
        xwer_t rc;
        xwsq_t seq;
        xwtk_t tick;
        const struct xwarcos_counter_config * cfg;

        if (NULL == counter) {
                /* [SWS_Os_00381] */
                rc = -E_XWARCOS_ID;
                goto err_id;
        }
        if (NULL == value) {
                rc = -E_XWARCOS_ILLEGAL_ADDRESS;
                goto err_nullptr;
        }
        if (NULL == elapsed) {
                rc = -E_XWARCOS_ILLEGAL_ADDRESS;
                goto err_nullptr;
        }
        cfg = counter->cfg;
        if (*value > cfg->max_allowed_value) {
                /* [SWS_Os_00391] */
                rc = -E_XWARCOS_VALUE;
                goto err_value;
        }
        rc = E_XWARCOS_OK;
        do {
                seq = xwos_sqlk_rd_begin(&counter->lock);
                tick = counter->value;
        } while (xwos_sqlk_rd_retry(&counter->lock, seq));
        /* [SWS_Os_00382] */
        *elapsed = ((tick >= *value) ?
                    (tick - *value) :
                    (tick + (cfg->max_allowed_value - *value + 1)));
        /* [SWS_Os_00460] */
        *value = tick;

err_value:
err_nullptr:
err_id:
        return rc;
}
