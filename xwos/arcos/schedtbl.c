/**
 * @file
 * @brief xwos::autosar::cp::os::schedtbl
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#include <xwos/arcos/schedtbl.h>
#include <xwos/arcos/hook.h>

static
void xwarcos_schedtbl_post_add_rel(struct xwarcos_counter_node * node)
{
        struct xwarcos_schedtbl * schedtbl = xwcc_derof(node,
                                                        struct xwarcos_schedtbl,
                                                        node);
        struct xwarcos_counter * counter = node->counter;

        schedtbl->start_point = counter->value + schedtbl->start_offset;
        if (schedtbl->start_point > counter->cfg->max_allowed_value) {
                schedtbl->start_point = 0;
        }
        schedtbl->tick = 0;
        schedtbl->epidx = 0;
        schedtbl->status = XWARCOS_SCHEDTBL_STATUS_RUNNING;
}

static
void xwarcos_schedtbl_post_add_abs(struct xwarcos_counter_node * node)
{
        struct xwarcos_schedtbl * schedtbl = xwcc_derof(node,
                                                        struct xwarcos_schedtbl,
                                                        node);
        struct xwarcos_counter * counter = node->counter;

        if (schedtbl->start_point >= counter->value) {
                schedtbl->start_offset = schedtbl->start_point - counter->value;
        } else {
                schedtbl->start_offset = schedtbl->start_point +
                        (counter->cfg->max_allowed_value - counter->value) + 1;
        }
        schedtbl->tick = 0;
        schedtbl->epidx = 0;
        schedtbl->status = XWARCOS_SCHEDTBL_STATUS_RUNNING;
}

static
void xwarcos_schedtbl_pre_del(struct xwarcos_counter_node * node)
{
        struct xwarcos_schedtbl * schedtbl = xwcc_derof(node,
                                                        struct xwarcos_schedtbl,
                                                        node);
        schedtbl->status = XWARCOS_SCHEDTBL_STATUS_STOPPED;
}

bool xwarcos_schedtbl_counter_operation(struct xwarcos_counter * counter,
                                        struct xwarcos_counter_node * node)
{
        struct xwarcos_schedtbl * schedtbl = xwcc_derof(node,
                                                        struct xwarcos_schedtbl,
                                                        node);
        const struct xwarcos_schedtbl_config * cfg = schedtbl->cfg;
        const struct xwarcos_schedtbl_ep * ep = cfg->ep;
        bool proceed;
        XWOS_UNUSED(counter);

        if (XWARCOS_SCHEDTBL_STATUS_RUNNING == schedtbl->status) {
                if (schedtbl->start_offset > 0) {
                        schedtbl->start_offset--;
                        proceed = true;
                } else {
                        if (schedtbl->tick < cfg->duration) {
                                schedtbl->tick++;
                        }
                        if (schedtbl->tick == cfg->duration) {
                                if (cfg->repeating) {
                                        schedtbl->tick = 0;
                                        proceed = true;
                                } else {
                                        xwarcos_schedtbl_pre_del(node);
                                        proceed = false;
                                }
                        } else {
                                proceed = true;
                        }
                }
                if (0 == schedtbl->start_offset) {
                        while (schedtbl->tick == ep[schedtbl->epidx].offset) {
                                for (xwsz_t i = 0;
                                     i < ep[schedtbl->epidx].action_number;
                                     i++) {
                                        if (NULL != ep[schedtbl->epidx].actions[i]) {
                                                ep[schedtbl->epidx].actions[i](
                                                        schedtbl, &ep[schedtbl->epidx]);
                                        }
                                }
                                schedtbl->epidx++;
                                if (schedtbl->epidx >= cfg->ep_number) {
                                        schedtbl->epidx = 0;
                                }
                        }
                }
        } else {
                proceed = false;
        }
        return proceed;
}

xwer_t xwarcos_schedtbl_validate_and_init(struct xwarcos_schedtbl * schedtbl)
{
        xwer_t rc;
        const struct xwarcos_schedtbl_config * cfg;

        if (NULL == schedtbl) {
                rc = -E_XWARCOS_ILLEGAL_ADDRESS;
                goto err_nullptr;
        }
        if (NULL == schedtbl->cfg) {
                rc = -E_XWARCOS_ILLEGAL_ADDRESS;
                goto err_nullptr;
        }
        cfg = schedtbl->cfg;
        if (NULL == cfg->counter) {
                rc = -E_XWARCOS_ILLEGAL_ADDRESS;
                goto err_nullptr;
        }
        if (NULL == cfg->ep) {
                rc = -E_XWARCOS_ILLEGAL_ADDRESS;
                goto err_nullptr;
        }
        if (0 == cfg->ep_number) {
                rc = -E_XWARCOS_VALUE;
                goto err_value;
        }
        schedtbl->node.operation = xwarcos_schedtbl_counter_operation;
        rc = xwarcos_counter_node_validate(&schedtbl->node);
        if (rc < 0) {
                goto err_counter_node_validate;
        }
        schedtbl->start_point = 0;
        schedtbl->start_offset = 0;
        schedtbl->tick = 0;
        schedtbl->epidx = 0;
        schedtbl->status = XWARCOS_SCHEDTBL_STATUS_STOPPED;

err_counter_node_validate:
err_value:
err_nullptr:
        return rc;
}

xwer_t xwarcos_schedtbl_start_rel(struct xwarcos_schedtbl * schedtbl, xwtk_t offset)
{
        xwer_t rc;
        const struct xwarcos_schedtbl_config * cfg;
        struct xwarcos_counter * counter;
        const struct xwarcos_schedtbl_ep * ep;

        if (NULL == schedtbl) { /* [SWS_Os_00275] */
                rc = -E_XWARCOS_ID;
                goto err_id;
        }
        cfg = schedtbl->cfg;
        counter = cfg->counter;
        ep = cfg->ep;
        if (0 == offset) {
                /* [SWS_Os_00332] */
                rc = -E_XWARCOS_VALUE;
                goto err_value;
        }
        if (offset > counter->cfg->max_allowed_value - ep[0].offset) {
                /* [SWS_Os_00276] */
                rc = -E_XWARCOS_VALUE;
                goto err_value;
        }
        if (XWARCOS_SCHEDTBL_STATUS_STOPPED != schedtbl->status) {
                /* [SWS_Os_00277] */
                rc = -E_XWARCOS_STATE;
                goto err_state;
        }

        /* [SWS_Os_00278] */
        schedtbl->start_offset = offset;
        rc = xwarcos_counter_add(counter, &schedtbl->node,
                                 xwarcos_schedtbl_post_add_rel);
        if (rc < 0) {
                goto err_counter_add;
        }

err_counter_add:
err_state:
err_value:
err_id:
        return rc;
}

xwer_t xwarcos_schedtbl_start_abs(struct xwarcos_schedtbl * schedtbl, xwtk_t start)
{
        xwer_t rc;
        const struct xwarcos_schedtbl_config * cfg;
        struct xwarcos_counter * counter;

        if (NULL == schedtbl) {
                /* [SWS_Os_00348] */
                rc = -E_XWARCOS_ID;
                goto err_id;
        }
        cfg = schedtbl->cfg;
        counter = cfg->counter;
        if (start > counter->cfg->max_allowed_value) {
                /* [SWS_Os_00349] */
                rc = -E_XWARCOS_VALUE;
                goto err_value;
        }
        if (XWARCOS_SCHEDTBL_STATUS_STOPPED != schedtbl->status) {
                /* [SWS_Os_00350] */
                rc = -E_XWARCOS_STATE;
                goto err_state;
        }
        /* [SWS_Os_00351] */
        schedtbl->start_point = start;
        rc = xwarcos_counter_add(counter, &schedtbl->node,
                                 xwarcos_schedtbl_post_add_abs);
        if (rc < 0) {
                goto err_counter_add;
        }

err_counter_add:
err_state:
err_value:
err_id:
        return rc;
}

xwer_t xwarcos_schedtbl_stop(struct xwarcos_schedtbl * schedtbl)
{
        xwer_t rc;
        const struct xwarcos_schedtbl_config * cfg;
        struct xwarcos_counter * counter;

        if (NULL == schedtbl) {
                /* [SWS_Os_00279] */
                rc = -E_XWARCOS_ILLEGAL_ADDRESS;
                goto err_nullptr;
        }
        cfg = schedtbl->cfg;
        if (XWARCOS_SCHEDTBL_STATUS_STOPPED == schedtbl->status) {
                /* [SWS_Os_00280] */
                rc = -E_XWARCOS_NOFUNC;
                goto err_nofunc;
        }
        counter = cfg->counter;
        /* [SWS_Os_00281] */
        rc = xwarcos_counter_del(counter, &schedtbl->node, xwarcos_schedtbl_pre_del);
        if (rc < 0) {
                goto err_counter_del;
        }

err_counter_del:
err_nofunc:
err_nullptr:
        return rc;
}

xwer_t xwarcos_schedtbl_get_status(struct xwarcos_schedtbl * schedtbl,
                                   enum xwarcos_schedtbl_status_em * status)
{
        xwer_t rc;

        if (NULL == schedtbl) {
                rc = -E_XWARCOS_ID;
                goto err_id;
        }
        if (NULL == status) {
                rc = -E_XWARCOS_ILLEGAL_ADDRESS;
                goto err_nullptr;
        }
        rc = E_XWARCOS_OK;
        *status = schedtbl->status;

err_nullptr:
err_id:
        return rc;
}
