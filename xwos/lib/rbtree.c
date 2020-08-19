/**
 * @file
 * @brief XWOS通用库：红黑树
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <xwos/lib/rbtree.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief 插入一个红色节点后修正红黑树的颜色
 * @param tree: (I) 红黑树的指针
 * @param node: (I) 待修正颜色的节点的指针
 */
__xwlib_code __xwcc_hot
void xwlib_rbtree_insert_color(struct xwlib_rbtree * tree,
                               struct xwlib_rbtree_node * node)
{
        struct xwlib_rbtree_node * parent, * gparent;
        union {
                struct xwlib_rbtree_node * child;
                struct xwlib_rbtree_node * sibling;
                struct xwlib_rbtree_node * uncle;
        } tmp;
        xwptr_t rotation, lpc;

recursively_fix:
        if (node == tree->root) {
                /*
                 * 情况 1：红黑树为空
                 * ==================
                 *               n(r)
                 *               /  \
                 *        NULL(B)    NULL(B)
                 *
                 *  flip_color(n)
                 * --------------->
                 *
                 *               n(B)
                 *               /  \
                 *        NULL(B)    NULL(B)
                 *
                 */
                xwlib_rbtree_flip_color(node);
                return;
        }

        parent = xwlib_rbtree_get_parent(node);
        if (xwlib_rbtree_tst_black(parent->lpc.v)) {
                /*
                 * 情况 2：父节点为黑色
                 * ====================
                 */
                return;
        }

        /* 情况 3： 父节点为红色 */
        gparent = xwlib_rbtree_get_parent(parent);
        if (xwlib_rbtree_tst_right(parent->lpc.v)) {
                tmp.uncle = gparent->left;
                rotation = xwlib_rbtree_gen_rr(node); /* `rotation' 用于右旋 */
        } else {
                tmp.uncle = gparent->right;
                rotation = xwlib_rbtree_gen_lr(node); /* `rotation' 用于左旋 */
        }

        if ((tmp.uncle) && xwlib_rbtree_tst_red(tmp.uncle->lpc.v)) {
                /*
                 * 情况 3.1：叔节点(u)是红色
                 * =========================
                 *
                 *                   |
                 *                  g(B)
                 *                __/  \__
                 *               /        \
                 *           p(r)          u(r)
                 *           /  \          /  \
                 *       n(r)
                 *       /  \
                 *
                 *  flip_color(p); flip_color(u); flip_color(g);
                 * ---------------------------------------------->
                 *
                 *                   |
                 *                  g(r)
                 *                __/  \__
                 *               /        \
                 *           p(B)          u(B)
                 *           /  \          /  \
                 *       n(r)
                 *       /  \
                 *
                 *  recursively fix_color(g);
                 * --------------------------->
                 *
                 * ******** ******** ******** ******** ******** ********
                 *  OR
                 * ******** ******** ******** ******** ******** ********
                 *
                 *                   |
                 *                  g(B)
                 *                __/  \__
                 *               /        \
                 *           u(r)          p(r)
                 *           /  \          /  \
                 *                             n(r)
                 *                             /  \
                 *
                 *  flip_color(p); flip_color(u); flip_color(g);
                 * ---------------------------------------------->
                 *
                 *                   |
                 *                  g(r)
                 *                __/  \__
                 *               /        \
                 *           u(B)          p(B)
                 *           /  \          /  \
                 *                             n(r)
                 *                             /  \
                 *
                 *  递归地修正颜色，节点g为待修正颜色的节点
                 * ----------------------------------------->
                 *
                 */
                xwlib_rbtree_flip_color(parent);
                xwlib_rbtree_flip_color(tmp.uncle);
                xwlib_rbtree_flip_color(gparent);
                node = gparent;
                goto recursively_fix;
        }

        /* 情况 3.2: 叔节点(U)是叶子或黑色 */
        if (xwlib_rbtree_get_pos(parent->lpc.v) != xwlib_rbtree_get_pos(node->lpc.v)) {
                /*
                 * Case 3.2.1: node->lpc.pos != parent->lpc.pos
                 * ============================================
                 *
                 *                      |
                 *                     g(B)
                 *                   __/  \__
                 *                  /        \
                 *               p(r)         u(B)
                 *             __/  \__       /  \
                 *            /        \
                 *        s(B)          n(r)
                 *        /  \          /  \
                 *                  l(B)    r(B)
                 *                  /  \    /  \
                 *
                 *  left_rotate(p);
                 * ----------------->
                 *
                 *                           |
                 *                          g(B)
                 *                        __/  \__
                 *                       /        \
                 *                   n(r)          u(B)
                 *                 __/  \__        /  \
                 *                /        \
                 *            p(r)          r(B)
                 *            /  \          /  \
                 *        s(B)    l(B)
                 *        /  \    /  \
                 *
                 * ******** ******** ******** ******** ******** ********
                 *  OR
                 * ******** ******** ******** ******** ******** ********
                 *
                 *               |
                 *              g(B)
                 *            __/  \__
                 *           /        \
                 *        u(B)         p(r)
                 *        /  \       __/  \__
                 *                  /        \
                 *              n(r)          s(B)
                 *              /  \          /  \
                 *          l(B)    r(B)
                 *          /  \    /  \
                 *
                 *  right_rotate(p);
                 * ----------------->
                 *
                 *               |
                 *              g(B)
                 *            __/  \__
                 *           /        \
                 *       u(B)          n(r)
                 *       /  \        __/  \__
                 *                  /        \
                 *              l(B)          p(r)
                 *                            /  \
                 *                        r(B)    s(B)
                 *                        /  \    /  \
                 *
                 * 转换为情况 3.2.2，原来的父节点变为待修正颜色的节点(n)。
                 */
                tmp.child = *xwlib_rbtree_get_link(rotation);
                lpc = parent->lpc.v;

                *xwlib_rbtree_get_link(node->lpc.v) = tmp.child;
                if (tmp.child) {
                        tmp.child->lpc.v = node->lpc.v | XWLIB_RBTREE_COLOR_BLACK;
                }/* else {} */

                *xwlib_rbtree_get_link(rotation) = parent;
                parent->lpc.v = rotation; /* color: red */

                /* *xwlib_rbtree_get_link(lpc) = node; */ /* omitted */
                node->lpc.v = lpc; /* color: red */

                /* 转换为情况 3.2.2 */
                tmp.child = parent;
                parent = node;
                node = tmp.child;
                /* lpc = parent->lpc.v; */ /* omitted */
        }

        /*
         * 情况 3.2.2: node->lpc.pos == parent->lpc.pos
         * ============================================
         *
         *                           |
         *                          g(B)
         *                        __/  \__
         *                       /        \
         *                   p(r)          u(B)
         *                 __/  \__        /  \
         *                /        \
         *            n(r)          s(B)
         *            /  \          /  \
         *        l(B)    r(B)
         *        /  \    /  \
         *
         *  right_rotate(g);
         * ------------------>
         *
         *                     |
         *                    p(r)
         *                 ___/  \___
         *                /          \
         *            n(r)            g(B)
         *            /  \            /  \
         *        l(B)    r(B)    s(B)    u(B)
         *        /  \    /  \    /  \    /  \
         *
         *  flip_color(p); flip_color(g);
         * ------------------------------->
         *
         *                     |
         *                    P(B)
         *                 ___/  \___
         *                /          \
         *            n(r)            g(r)
         *            /  \            /  \
         *        l(B)    r(B)    s(B)    u(B)
         *        /  \    /  \    /  \    /  \
         *
         * ******** ******** ******** ******** ******** ********
         *  OR
         * ******** ******** ******** ******** ******** ********
         *
         *               |
         *              g(B)
         *            __/  \__
         *           /        \
         *       u(B)          p(r)
         *       /  \        __/  \__
         *                  /        \
         *              s(B)          n(r)
         *                            /  \
         *                        l(B)    r(B)
         *                        /  \    /  \
         *
         *  left_rotate(g);
         * ------------------>
         *
         *                     |
         *                    p(r)
         *                 ___/  \___
         *                /          \
         *            g(B)            n(r)
         *            /  \            /  \
         *        u(B)    s(B)    l(B)    r(B)
         *        /  \    /  \    /  \    /  \
         *
         *  flip_color(p); flip_color(g);
         * ------------------------------->
         *
         *                     |
         *                    p(B)
         *                 ___/  \___
         *                /          \
         *            g(r)            n(r)
         *            /  \            /  \
         *        u(B)    s(B)    l(B)    r(B)
         *        /  \    /  \    /  \    /  \
         *
         */
        if (xwlib_rbtree_tst_right(node->lpc.v)) {
                tmp.sibling = parent->left;
                rotation = xwlib_rbtree_gen_lr(parent); /* 左旋g */
        } else {
                tmp.sibling = parent->right;
                rotation = xwlib_rbtree_gen_rr(parent); /* 右旋g */
        }
        lpc = parent->lpc.v;

        *xwlib_rbtree_get_link(gparent->lpc.v) = parent;
        parent->lpc.v = gparent->lpc.v; /* flip_color(p): 黑 */

        *xwlib_rbtree_get_link(lpc) = tmp.sibling;
        if (tmp.sibling) {
                tmp.sibling->lpc.v = ((xwptr_t)lpc) | XWLIB_RBTREE_COLOR_BLACK;
        }/* else {} */

        *xwlib_rbtree_get_link(rotation) = gparent;
        gparent->lpc.v = rotation; /* flip_color(g): 红 */
}

/**
 * @brief 删除一个节点，并修正红黑树的颜色
 * @param tree: (I) 红黑树的指针
 * @param node: (I) 待删除的节点的指针
 * @note
 * - 此函数假设不存在节点没有链接到红黑树的情形。
 */
__xwlib_code __xwcc_hot
void xwlib_rbtree_remove(struct xwlib_rbtree * tree,
                         struct xwlib_rbtree_node * node)
{
        struct xwlib_rbtree_node * parent, * sibling;
        union {
                struct xwlib_rbtree_node * same;
                struct xwlib_rbtree_node * left;
        } sl;
        union {
                struct xwlib_rbtree_node * reverse;
                struct xwlib_rbtree_node * right;
        } rr;
        union {
                struct xwlib_rbtree_node * child;
                xwptr_t color;
        } cc;
        xwptr_t lpc;
        xwptr_t rotation;

        /* 步骤 1：删除节点 */
        rr.right =  node->right;
        sl.left =  node->left;
        if (!sl.left) {
                parent = xwlib_rbtree_get_parent(node);
                *xwlib_rbtree_get_link(node->lpc.v) = rr.right;
                if (!rr.right) {
                        /*
                         * 情况 1：待删除的节点(n)没有子节点
                         * =================================
                         *
                         *                  |
                         *                p(*)
                         *                /   \
                         * s(r) or NULL(B)     n(r)
                         *
                         *  transplant_nil(n)
                         * -------------------->
                         *
                         *                  |
                         *                p(*)
                         *                /   \
                         * s(r) or NULL(B)     NULL(B)
                         *
                         * =====================================================
                         *
                         *               |
                         *             p(*)
                         *             /   \
                         * s(B) or s(r)     n(B)
                         *
                         *  transplant_nil(n)
                         * -------------------->
                         *
                         *               |                              |
                         *             p(*)                           p(*)
                         *             /   \            or            /   \
                         *         s(B)     NULL(BB)              s(r)     NULL(BB)
                         *         /  \                           /  \
                         *  NULL(B)    NULL(B)                l(B)    r(B)
                         *
                         * ******** ******** ******** ******** ******** ********
                         *  OR
                         * ******** ******** ******** ******** ******** ********
                         *        |
                         *      p(*)
                         *      /   \
                         *  n(r)     NULL(B) or s(r)
                         *
                         *  transplant_nil(n)
                         * -------------------->
                         *
                         *          |
                         *        p(*)
                         *        /   \
                         * NULL(B)     NULL(B) or s(r)
                         *
                         * =====================================================
                         *
                         *        |
                         *      p(*)
                         *      /   \
                         *  n(B)     s(B) or s(r)
                         *
                         *  transplant_nil(n)
                         * -------------------->
                         *
                         *            |                                 |
                         *          p(*)                              p(*)
                         *          /   \                             /   \
                         *  NULL(BB)     s(B)           or    NULL(BB)     s(r)
                         *               /  \                              /  \
                         *        NULL(B)    NULL(B)                   l(B)    r(B)
                         *
                         * + 如果待删除的节点(n)是红色，根据性质5，
                         *   它的兄弟节点(s)不是叶子，就是红色节点，且没有子节点；
                         * + 如果待删除的节点(n)是黑色，根据性质5，
                         *   它的兄弟节点(s)一定不是叶子：
                         *   - 如果兄弟节点(s)是红色，那么兄弟节点(s)有两个黑色子节点；
                         *   - 如果兄弟节点(s)是黑色，那么兄弟节点(s)没子节点。
                         */
                        if (NULL == tree->root) {
                                return;
                        }

                        lpc = node->lpc.v;
                        if (xwlib_rbtree_tst_black(lpc)) {
                                if (xwlib_rbtree_tst_right(lpc)) {
                                        sibling = parent->left;
                                        rotation = xwlib_rbtree_gen_rr(sibling);
                                } else {
                                        sibling = parent->right;
                                        rotation = xwlib_rbtree_gen_lr(sibling);
                                }
                        } else {
                                sibling = NULL;
                        }
                } else {
                        /*
                         * 情况 2：待删除的节点(n)只有一个子节点
                         * =====================================
                         */
                        /*
                         * 情况 2.1：待删除的节点(n)没有左子节点
                         * =====================================
                         *    |
                         *   n(B)
                         *      \
                         *       r(r)
                         *
                         *  transplant(n->right, n)
                         * ------------------------->
                         *
                         *    |
                         *  r(rB)
                         *
                         *  set_black(r)
                         * ---------------->
                         *
                         *    |
                         *   r(B)
                         *
                         * + 根据性质5，右子节点(r)一定是红色；
                         * + 根据性质4，待删除的节点(n)一定是黑色；
                         */
                        rr.right->lpc.v = node->lpc.v; /* Inherit color */
                        sibling = NULL;
                }
        } else if (!rr.right) {
                /*
                 * 情况 2.2：待删除的节点(n)没有右子节点
                 * =====================================
                 *
                 *       |
                 *      n(B)
                 *      /
                 *  r(r)
                 *
                 *  transplant(n->left, n)
                 * ------------------------->
                 *
                 *    |
                 *  r(rB)
                 *
                 *  set_black(r)
                 * ---------------->
                 *
                 *    |
                 *   r(B)
                 *
                 * + 根据性质5，左子节点(r)一定是红色；
                 * + 根据性质4，待删除的节点(n)一定是黑色；
                 */
                xwlib_rbtree_link(sl.left, node->lpc.v); /* Inherit color */
                sibling = NULL;
        } else {
                /*
                 * 情况 3：待删除的节点(n)同时存在左右子节点
                 */
                struct xwlib_rbtree_node * successor;

                successor = xwlib_rbtree_get_successor(node);
                if (successor == rr.right) {
                        /*
                         * 情况 3.1：待删除的节点(n)的后继节点(s)是它的右节点
                         * ==================================================
                         *
                         *        |
                         *       n(*)
                         *      /   \
                         *  l(*)     s(*)
                         *               \
                         *                c(r)_or_NULL(B)
                         *
                         *   transplant(s, n)
                         *  -------------------------------------------->
                         *  s把自己的颜色留给了c，n把自己的颜色留给了s
                         *
                         *      |
                         *    s(*)                          n()
                         *        \                         /
                         *         c(rB)_or_NULL(BB)    l(*)
                         *
                         *   link(l, ((&s->left) | (RBTREE_LEFT) | color(l)))
                         *  -------------------------------------------------->
                         *
                         *         |
                         *       s(*)                       n()
                         *       /   \
                         *   l(*)     c(B)_or_NULL(BB)
                         *
                         * + 如果后继节点(s)有右子节点，根据性质5，(s)的右子节点
                         *   必然为红色，再根据性质4，(s)必然不可能为红色（即为黑色）；
                         * + 如果后继节点(s)为红色，其不可能存在子节点；
                         * + 如果后继节点(s)没有右子节点且(s)为黑色，根据性质5，
                         *   (s)的兄弟节点也即是(n)的左子节点(l)必然存在。
                         */

                        if (successor->right) {
                                xwlib_rbtree_set_black(successor->right);
                                sibling = NULL;
                        } else {
                                if (xwlib_rbtree_tst_black(successor->lpc.v)) {
                                        sibling = sl.left;
                                        parent = successor;
                                        rotation = xwlib_rbtree_gen_rr(sibling);
                                } else {
                                        sibling = NULL;
                                }
                        }
                        xwlib_rbtree_transplant(successor, node);
                        cc.color = xwlib_rbtree_get_color(sl.left->lpc.v);
                        xwlib_rbtree_link(sl.left,
                                          xwlib_rbtree_gen_lc(successor, cc.color));
                } else {
                        /*
                         * 情况 3.2：(n)的后继节点(s)是(n)的右子树上最左端的节点
                         * =====================================================
                         *
                         *        |
                         *      n(*)
                         *      /   \
                         *     l     r
                         *          / \
                         *         p
                         *        / \
                         *    s(*)
                         *       \
                         *        c(r)_or_NULL(B)
                         *
                         *   transplant(s->right, s)
                         *  ------------------------->
                         *   s把自己的颜色留给了c
                         *
                         *                  |
                         *                n(*)             s()
                         *                /   \
                         *               l     r
                         *                    / \
                         *                   p
                         *                  / \
                         * c(rB)_or_NULL(BB)
                         *
                         *   link(r, ((&s->right) | (RBTREE_RIGHT) | color(r)))
                         *  ---------------------------------------------------->
                         *
                         *      |
                         *    n(*)               s()
                         *    /                    \
                         *   l                      r
                         *                         / \
                         *                        p
                         *                       / \
                         *       c(B)_or_NULL(BB)
                         *
                         *   link(l, ((&s->left) | (RBTREE_LEFT) | color(l)))
                         *  -------------------------------------------------->
                         *
                         *       |
                         *     n(*)             s()
                         *                      / \
                         *                     l   r
                         *                        / \
                         *                       p
                         *                      / \
                         *      c(B)_or_NULL(BB)
                         *
                         *   transplant(s, n)
                         *  --------------------->
                         *   n把自己的颜色留给了s
                         *
                         *                    |
                         *                  s(*)
                         *                  /   \
                         *                 l     r
                         *                      / \
                         *                     p
                         *                    / \
                         *    C(B)_or_NULL(BB)
                         *
                         * + 如果后继节点(s)的右子节点(r)存在，根据性质5，
                         *   (r)一定是红色，再根据性质4，(s)一定为黑色；
                         * + 根据性质5，如果后继节点(s)没有右子节点且(s)为黑色，
                         *   (n)的左子节点(l)必然存在。
                         */
                        parent = xwlib_rbtree_get_parent(successor);
                        lpc = successor->lpc.v; /* cache */
                        cc.child = successor->right;

                        *xwlib_rbtree_get_link(lpc) = cc.child;
                        if (cc.child) {
                                cc.child->lpc.v = lpc; /* Inherit color */
                                sibling = NULL;
                        } else {
                                if (xwlib_rbtree_tst_black(successor->lpc.v)) {
                                        sibling = parent->right;
                                        rotation = xwlib_rbtree_gen_lr(sibling);
                                } else {
                                        sibling = NULL;
                                }
                        }

                        /* link(r, ((&s->right) | (RBTREE_RIGHT) | color(r))) */
                        cc.color = xwlib_rbtree_get_color(rr.right->lpc.v);
                        xwlib_rbtree_link(rr.right,
                                          xwlib_rbtree_gen_rc(successor, cc.color));

                        /* link(r, ((&s->right) | (RBTREE_RIGHT) | color(r))) */
                        cc.color = xwlib_rbtree_get_color(sl.left->lpc.v);
                        xwlib_rbtree_link(sl.left,
                                          xwlib_rbtree_gen_lc(successor, cc.color));

                        /* transplant(s, n) */
                        xwlib_rbtree_transplant(successor, node);
                }
        }

        /* 步骤 2：修复颜色 */
        /* X有可能是NULL或节点(n) */
        if (!sibling) {
                return;
        }

recursively_fix:
        if (xwlib_rbtree_tst_red(sibling->lpc.v)) {
                /*
                 * Case 1: 兄弟节点(s)是红色
                 * ===================================================
                 *                          *
                 *         |                *           |
                 *        p(B)              *          p(B)
                 *        /  \              *          /  \
                 *   X(BB)    s(r)          *      s(r)    X(BB)
                 *            /  \          *      /  \
                 *        l(B)    r(B)      *  l(B)    r(B)
                 *                          *
                 *  left_rotate(p)          *  right_rotate(p)
                 * ---------------->        * ----------------->
                 *                          *
                 *            |             *           |
                 *           s(r)           *          s(r)
                 *           /  \           *          /  \
                 *       P(B)    r(B)       *      l(B)    P(B)
                 *       /  \               *              /  \
                 *  X(BB)    l(B)           *          r(B)    X(BB)
                 *                          *
                 *  flip_color(s);          *  flip_color(s);
                 * ---------------->        * ---------------->
                 *  flip_color(P);          *  flip_color(P);
                 *                          *
                 *            |             *           |
                 *           s(B)           *          s(B)
                 *           /  \           *          /  \
                 *       p(r)    r(B)       *      l(B)    p(r)
                 *       /  \               *              /  \
                 *  X(BB)    l(B)           *          r(B)    X(BB)
                 *                          *
                 * l是新的兄弟节点          * r是新的兄弟节点
                 *                          *
                 * + 根据性质4，父节点(p)一定为黑色；
                 * + 根据性质5，兄弟节点(s)有两个黑色的子节点。
                 */
                lpc = sibling->lpc.v;
                cc.child = *xwlib_rbtree_get_link(rotation);

                *xwlib_rbtree_get_link(parent->lpc.v) = sibling;
                sibling->lpc.v = parent->lpc.v; /* flip_color: 黑色 */

                *xwlib_rbtree_get_link(lpc) = cc.child;
                cc.child->lpc.v = lpc | XWLIB_RBTREE_COLOR_BLACK;

                /* xwlib_rbtree_link(parent, rotation); */
                *xwlib_rbtree_get_link(rotation) = parent;
                parent->lpc.v = rotation; /* flip_color: 红色 */

                /* parent = parent; */ /* omitted */
                sibling = cc.child;
        }

black_sibling:
        /*
         * 情况 2：兄弟节点(s)是黑色
         */
        if (xwlib_rbtree_tst_right(sibling->lpc.v)) {
                rotation = xwlib_rbtree_gen_lr(sibling); /* 左旋 */
                rr.reverse = sibling->left;
                sl.same = sibling->right;
                if (rr.reverse) {
                        lpc = xwlib_rbtree_gen_rr(rr.reverse); /* 右旋 */
                }/* else {} */
        } else {
                rotation = xwlib_rbtree_gen_rr(sibling); /* 右旋 */
                rr.reverse = sibling->right;
                sl.same = sibling->left;
                if (rr.reverse) {
                        lpc = xwlib_rbtree_gen_lr(rr.reverse); /* 左旋 */
                }/* else {} */
        }

        if (!sl.same || xwlib_rbtree_tst_black(sl.same->lpc.v)) {
                if (!rr.reverse || xwlib_rbtree_tst_black(rr.reverse->lpc.v)) {
                        /*
                         * 情况 2.1：兄弟节点(s)没有红色的子节点
                         * =====================================
                         *
                         *               |
                         *             p(*)
                         *             /   \
                         *         s(B)     x(BB)
                         *         /  \
                         *  same(B)    reverse(B)
                         *
                         *  flip_color(p); flip_color(s); flip_color(x);
                         * ---------------------------------------------->
                         *
                         *                |
                         *             p(rB_or_BB)
                         *             /    \
                         *         s(r)      X(B)
                         *         /  \
                         *  same(B)    reverse(B)
                         *
                         *  如果p是红色，就把p设置为黑色
                         * --------------------------------------->
                         *  如果p是黑色，p就作为新的X递归调整颜色
                         *
                         * ******** ******** ******** ******** ******** ********
                         *  OR
                         * ******** ******** ******** ******** ******** ********
                         *
                         *            |
                         *          p(*)
                         *          /   \
                         *     X(BB)     s(B)
                         *               /  \
                         *     reverse(B)    same(B)
                         *
                         *  flip_color(p); flip_color(S); flip_color(X);
                         * ---------------------------------------------->
                         *
                         *            |
                         *          p(rB_or_BB)
                         *          /    \
                         *      X(B)      s(r)
                         *                /  \
                         *      reverse(B)    same(B)
                         *
                         *  如果p是红色，就把p设置为黑色
                         * --------------------------------------->
                         *  如果p是黑色，p就作为新的X递归调整颜色
                         */
                        xwlib_rbtree_flip_color(sibling);
                        if (xwlib_rbtree_tst_black(parent->lpc.v)) {
                                if (parent != tree->root) {
                                        cc.child = parent;
                                        parent = xwlib_rbtree_get_parent(cc.child);
                                        if (xwlib_rbtree_tst_right(cc.child->lpc.v)) {
                                                sibling = parent->left;
                                                rotation = xwlib_rbtree_gen_rr(sibling);
                                        } else {
                                                sibling = parent->right;
                                                rotation = xwlib_rbtree_gen_lr(sibling);
                                        }
                                        goto recursively_fix;
                                }
                        } else {
                                xwlib_rbtree_flip_color(parent);
                        }
                        return;
                }

                /*
                 * 情况 2.2：与兄弟节点(s)位置相同的子节点为黑色，
                 *           位置相反的子节点为红色
                 *
                 *               |
                 *             p(*)
                 *             /   \
                 *         s(B)     X(BB)
                 *         /  \
                 *  same(B)    reverse(r)
                 *            /
                 *        z(B)
                 *
                 *  left_rotate(s);
                 * ----------------->
                 *
                 *                  |
                 *                p(*)
                 *                /   \
                 *      reverse(r)     X(BB)
                 *             /
                 *         s(B)
                 *         /  \
                 *  same(B)    z(B)
                 *
                 *  flip_color(reverse); flip_color(s)
                 * ----------------------------------->
                 *
                 *                  |
                 *                p(*)
                 *                /   \
                 *      reverse(B)     X(BB)
                 *             /
                 *         s(r)
                 *         /  \
                 *  same(B)    z(B)
                 *
                 * ******** ******** ******** ******** ******** ********
                 *  OR
                 * ******** ******** ******** ******** ******** ********
                 *
                 *            |
                 *          p(*)
                 *          /   \
                 *     X(BB)     s(B)
                 *               /  \
                 *     reverse(r)    same(B)
                 *              \
                 *               z(B)
                 *
                 *  right_rotate(s);
                 * ------------------>
                 *
                 *            |
                 *          p(*)
                 *          /  \
                 *     X(BB)    reverse(r)
                 *                  \
                 *                   s(B)
                 *                   /  \
                 *               z(B)    same(B)
                 *
                 *  flip_color(reverse); flip_color(s)
                 * ----------------------------------->
                 *
                 *            |
                 *          p(*)
                 *          /   \
                 *     X(BB)     reverse(B)
                 *                   \
                 *                    s(r)
                 *                    /  \
                 *                z(B)    same(B)
                 *
                 * 转换为情况 2.3
                 */
                rotation = sibling->lpc.v;
                cc.child = *xwlib_rbtree_get_link(lpc);

                /* xwlib_rbtree_link(sibling, lpc); */
                *xwlib_rbtree_get_link(lpc) = sibling;
                sibling->lpc.v = lpc; /* flip_color(S): red */

                /* omitted */
                /* if (cc.child) {                                                 */
                /*      xwlib_rbtree_link(cc.child,                                */
                /*                        rr.reverse->lpc.v |                      */
                /*                        XWLIB_RBTREE_COLOR_BLACK);               */
                /* } else {                                                        */
                /*      xwlib_rbtree_link_nil(rr.reverse->lpc.v);                  */
                /* }                                                               */
                *xwlib_rbtree_get_link(rr.reverse->lpc.v) = cc.child;
                if (cc.child) {
                        cc.child->lpc.v = rr.reverse->lpc.v | XWLIB_RBTREE_COLOR_BLACK;
                }/* else {} */

                /* xwlib_rbtree_link(rr.reverse,
                                     (rotation | XWLIB_RBTREE_COLOR_BLACK));       */
                *xwlib_rbtree_get_link(rotation) = rr.reverse;
                /* flip_color(cr) */
                rr.reverse->lpc.v = rotation | XWLIB_RBTREE_COLOR_BLACK;

                /* 转换为情况 2.3 */
                sibling = rr.reverse;
                goto black_sibling;
        }

        /*
         * 情况 2.3：与兄弟节点(s)位置相同的子节点为红色
         *
         *               |
         *             p(*)
         *             /   \
         *         s(B)     X(BB)
         *         /  \
         *  same(r)    reverse(*)
         *
         *  right_rotate(p);
         * ------------------>
         *  s, p, X留下颜色
         *
         *               |
         *             s(*)
         *             /   \
         *     same(rB)     p(B)
         *                  /  \
         *        reverse(*)    X(B)
         *
         * ******** ******** ******** ******** ******** ********
         *  OR
         * ******** ******** ******** ******** ******** ********
         *
         *             |
         *           p(*)
         *           /   \
         *      X(BB)     s(B)
         *                /  \
         *      reverse(*)    same(r)
         *
         *  left_rotate(p);
         * ------------------>
         *  s, p, X留下颜色
         *
         *               |
         *             s(*)
         *             /   \
         *         p(B)     same(rB)
         *         /  \
         *     X(B)    reverse(*)
         *
         * + s把黑色留给same，然后继承p的颜色；
         * + p留下颜色然后继承X的一份黑色，X带走另一份；
         *
         */
        lpc = parent->lpc.v;

        /* xwlib_rbtree_link(parent, rotation | XWLIB_RBTREE_COLOR_BLACK); */
        *xwlib_rbtree_get_link(rotation) = parent;
        parent->lpc.v = rotation | XWLIB_RBTREE_COLOR_BLACK;

        /* cc.color = xwlib_rbtree_get_color(rr.reverse->lpc.v); */
        /* xwlib_rbtree_link(rr.reverse,
                             xwlib_rbtree_get_lnpos(sibling->lpc.v) | cc.color); */
        *xwlib_rbtree_get_link(sibling->lpc.v) = rr.reverse;
        if (rr.reverse) {
                cc.color = xwlib_rbtree_get_color(rr.reverse->lpc.v);
                rr.reverse->lpc.v = xwlib_rbtree_get_lnpos(sibling->lpc.v) | cc.color;
        }/* else {} */

        /* xwlib_rbtree_link(sibling, lpc); */
        sibling->lpc.v = lpc; /* Inherit color. */
        *xwlib_rbtree_get_link(lpc) = sibling;
        xwlib_rbtree_set_black(sl.same); /* Inherit color of sibling */
}

/**
 * @brief 用一个新节点代替旧节点，继承它的颜色、位置信息并接管它的子孙。
 * @param newd: (I) 新节点的指针
 * @param oldn: (I) 旧节点的指针
 */
__xwlib_code __xwcc_hot
void xwlib_rbtree_replace(struct xwlib_rbtree_node * newn,
                          struct xwlib_rbtree_node * oldn)
{
        struct xwlib_rbtree_node * left = oldn->left;
        struct xwlib_rbtree_node * right = oldn->right;

        xwlib_rbtree_transplant(newn, oldn);
        if (left) {
                xwptr_t lpc = xwlib_rbtree_get_color(left->lpc.v);
                lpc |= (xwptr_t)(&newn->left);
                xwlib_rbtree_link(left, lpc);
        }/* else {} */

        if (right) {
                xwptr_t lpc = xwlib_rbtree_get_color(right->lpc.v);
                lpc |= ((xwptr_t)(&newn->right) | XWLIB_RBTREE_POS_RIGHT);
                xwlib_rbtree_link(right, lpc);
        }/* else {} */
}
