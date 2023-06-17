/**
 * @file
 * @brief XWOS通用库：键值对容器
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#include <xwos/standard.h>
#include <xwos/lib/map.h>

/**
 * @brief 插入键值对容器
 * @param[in] map: map的指针
 * @param[in] newmc: 键值对容器的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EEXIST: 键值对已经存在
 */
__xwlib_code
xwer_t xwlib_map_insert(struct xwlib_map * map, struct xwlib_map_container * newmc)
{
        struct xwlib_rbtree_node ** pos;
        struct xwlib_rbtree_node * rbn;
        struct xwlib_map_container * mc;
        struct xwlib_map_container * parent;
        xwptr_t lpc;
        xwssq_t cmprc;
        xwer_t rc;

        pos = &map->rbtree.root;
        lpc = (xwptr_t)pos;

        rbn = *pos;
        while (rbn) {
                mc = xwlib_rbtree_entry(rbn, struct xwlib_map_container, rbn);
                cmprc = map->cmp(newmc->key, mc->key);
                if (cmprc < 0) {
                        pos = &rbn->left;
                        lpc = (xwptr_t)pos;
                        rbn = rbn->left;
                } else if (cmprc > 0) {
                        pos = &rbn->right;
                        lpc = (xwptr_t)pos | XWLIB_RBTREE_POS_RIGHT;
                        rbn = rbn->right;
                } else {
                        lpc = (xwptr_t)0;
                        break;
                }
        }
        if (lpc) {
                if (xwlib_rbtree_tst_link_root(&map->rbtree,
                                               xwlib_rbtree_get_link(lpc))) {
                        xwlib_bclst_add_head(&map->bclh, &newmc->bcln);
                } else {
                        rbn = xwlib_rbtree_get_parent_from_lpc(lpc);
                        parent = xwlib_rbtree_entry(rbn, struct xwlib_map_container,
                                                    rbn);
                        if (xwlib_rbtree_tst_left(lpc)) {
                                xwlib_bclst_add_front(&newmc->bcln, &parent->bcln);
                        } else {
                                xwlib_bclst_add_behind(&newmc->bcln, &parent->bcln);
                        }
                }
                xwlib_rbtree_link(&newmc->rbn, lpc);
                xwlib_rbtree_insert_color(&map->rbtree, &newmc->rbn);
                rc = XWOK;
        } else {
                rc = -EEXIST;
        }
        return rc;
}

/**
 * @brief 删除键值对容器
 * @param[in] map: map的指针
 * @param[in] mc: 键值对容器的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -ESRCH: 此键值对容器不在键值对集合中
 */
__xwlib_code
xwer_t xwlib_map_erase(struct xwlib_map * map, struct xwlib_map_container * mc)
{
        xwer_t rc;

        if (xwlib_rbtree_tst_node_unlinked(&mc->rbn)) {
                rc = -ESRCH;
        } else {
                xwlib_rbtree_remove(&map->rbtree, &mc->rbn);
                xwlib_rbtree_init_node(&mc->rbn);
                xwlib_bclst_del_init(&mc->bcln);
                rc = XWOK;
        }
        return rc;
}

/**
 * @brief 根据“键”查找键值对容器
 * @param[in] map: map的指针
 * @param[in] key: 键值对容器的指针
 * @param[out] mcbuf: 指向指针缓存的指针，此指针缓存用于返回查找到的键值对容器的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -ESRCH: 目标不存在
 */
__xwlib_code
xwer_t xwlib_map_find(struct xwlib_map * map, void * key,
                      struct xwlib_map_container ** mcbuf)
{
        struct xwlib_rbtree_node * rbn;
        struct xwlib_map_container * mc;
        xwssq_t cmprc;
        xwer_t rc;

        rbn = map->rbtree.root;
        while (rbn) {
                mc = xwlib_rbtree_entry(rbn, struct xwlib_map_container, rbn);
                cmprc = map->cmp(key, mc->key);
                if (cmprc < 0) {
                        rbn = rbn->left;
                } else if (cmprc > 0) {
                        rbn = rbn->right;
                } else {
                        break;
                }
        }
        if (rbn) {
                *mcbuf = xwlib_rbtree_entry(rbn, struct xwlib_map_container, rbn);
                rc = XWOK;
        } else {
                *mcbuf = NULL;
                rc = -ESRCH;
        }
        return rc;
}
