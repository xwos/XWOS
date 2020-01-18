/**
 * @file
 * @brief XWOS通用库：红黑树
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
#include <xwos/lib/rbtree.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief 插入一个红色节点后修正红黑树的颜色
 * @param tree: (I) 红黑树的指针
 * @param node: (I) 待修正颜色的节点的指针
 * @note
 * - 节点在插入时的初始颜色为红色。
 */
__xwlib_code __hot
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
                 * case 1: rbtree is empty.
                 *
                 *               n(R)
                 *               /  \
                 *        NULL(B)    NULL(B)
                 *
                 *  flip_color (n)
                 * --------------->
                 *
                 *               N(B)
                 *               /  \
                 *        NULL(B)    NULL(B)
                 *
                 */
                xwlib_rbtree_flip_color(node);
                return;
        }

        parent = xwlib_rbtree_get_parent(node);
        if (xwlib_rbtree_color_tst_black(parent->lpc.v)) {
                /*
                 * case 2: Parent is black, and do nothing.
                 */
                return;
        }

        /* case 3: Parent is red. */
        gparent = xwlib_rbtree_get_parent(parent);
        if (xwlib_rbtree_tst_right(parent->lpc.v)) {
                tmp.uncle = gparent->left;
                /* `rotation' is used in case 3.2.2 to right-rotate parent */
                rotation = xwlib_rbtree_get_rr(node);
        } else {
                tmp.uncle = gparent->right;
                /* `rotation' is used in case 3.2.1 to left-rotate parent */
                rotation = xwlib_rbtree_get_lr(node);
        }

        if ((tmp.uncle) && xwlib_rbtree_color_tst_red(tmp.uncle->lpc.v)) {
                /*
                 * case 3.1: Uncle is red.
                 *                   |
                 *                  G(B)
                 *                __/  \__
                 *               /        \
                 *           p(R)          u(R)
                 *           /  \          /  \
                 *       n(R)
                 *       /  \
                 *
                 *  flip_color(p); flip_color(u); flip_color(G);
                 * ---------------------------------------------->
                 *
                 *                   |
                 *                  g(R)
                 *                __/  \__
                 *               /        \
                 *           P(B)          U(B)
                 *           /  \          /  \
                 *       n(R)
                 *       /  \
                 *
                 *  recursively fix_color(G);
                 * --------------------------->
                 *
                 * ******** ******** ******** ******** ******** ********
                 *  OR
                 * ******** ******** ******** ******** ******** ********
                 *
                 *                   |
                 *                  G(B)
                 *                __/  \__
                 *               /        \
                 *           u(R)          p(R)
                 *           /  \          /  \
                 *                             n(R)
                 *                             /  \
                 *
                 *  flip_color(p); flip_color(u); flip_color(G);
                 * ---------------------------------------------->
                 *
                 *                   |
                 *                  g(R)
                 *                __/  \__
                 *               /        \
                 *           U(B)          P(B)
                 *           /  \          /  \
                 *                             n(R)
                 *                             /  \
                 *
                 *  recursively fix_color(G);
                 * --------------------------->
                 *
                 */
                xwlib_rbtree_flip_color(parent);
                xwlib_rbtree_flip_color(tmp.uncle);
                xwlib_rbtree_flip_color(gparent);
                node = gparent;
                goto recursively_fix;
        }

        /* Case 3.2: Uncle is leaf or black-child */
        if (xwlib_rbtree_get_pos(parent->lpc.v) != xwlib_rbtree_get_pos(node->lpc.v)) {
                /*
                 * Case 3.2.1: node->lpc.pos != parent->lpc.pos.
                 *
                 *                      |
                 *                     G(B)
                 *                   __/  \__
                 *                  /        \
                 *               p(R)         U(B)
                 *             __/  \__       /  \
                 *            /        \
                 *        S(B)          n(R)
                 *        /  \          /  \
                 *                  L(B)    R(B)
                 *                  /  \    /  \
                 *
                 *  left_rotate(p);
                 * ----------------->
                 *
                 *                           |
                 *                          G(B)
                 *                        __/  \__
                 *                       /        \
                 *                   n(R)          U(B)
                 *                 __/  \__        /  \
                 *                /        \
                 *            p(R)          R(B)
                 *            /  \          /  \
                 *        S(B)    L(B)
                 *        /  \    /  \
                 *
                 * ******** ******** ******** ******** ******** ********
                 *  OR
                 * ******** ******** ******** ******** ******** ********
                 *
                 *               |
                 *              G(B)
                 *            __/  \__
                 *           /        \
                 *        U(B)         p(R)
                 *        /  \       __/  \__
                 *                  /        \
                 *              n(R)          S(B)
                 *              /  \          /  \
                 *          L(B)    R(B)
                 *          /  \    /  \
                 *
                 *  right_rotate(p);
                 * ----------------->
                 *
                 *               |
                 *              G(B)
                 *            __/  \__
                 *           /        \
                 *       U(B)          n(R)
                 *       /  \        __/  \__
                 *                  /        \
                 *              L(B)          p(R)
                 *                            /  \
                 *                        R(B)    S(B)
                 *                        /  \    /  \
                 *
                 * Transform into case 3.3: parent is new node.
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

                /* Transform into case 3.2.2 */
                tmp.child = parent;
                parent = node;
                node = tmp.child;
                /* lpc = parent->lpc.v; */ /* omitted */
        }

        /*
         * case 3.2.2: node->lpc.pos == parent->lpc.pos.
         *
         *                           |
         *                          G(B)
         *                        __/  \__
         *                       /        \
         *                   p(R)          U(B)
         *                 __/  \__        /  \
         *                /        \
         *            n(R)          S(B)
         *            /  \          /  \
         *        L(B)    R(B)
         *        /  \    /  \
         *
         *  right_rotate(G);
         * ------------------>
         *
         *                     |
         *                    p(R)
         *                 ___/  \___
         *                /          \
         *            n(R)            G(B)
         *            /  \            /  \
         *        L(B)    R(B)    S(B)    U(B)
         *        /  \    /  \    /  \    /  \
         *
         *  flip_color(p); flip_color(G);
         * ------------------------------->
         *
         *                     |
         *                    P(B)
         *                 ___/  \___
         *                /          \
         *            n(R)            g(R)
         *            /  \            /  \
         *        L(B)    R(B)    S(B)    U(B)
         *        /  \    /  \    /  \    /  \
         *
         * ******** ******** ******** ******** ******** ********
         *  OR
         * ******** ******** ******** ******** ******** ********
         *
         *               |
         *              G(B)
         *            __/  \__
         *           /        \
         *       U(B)          p(R)
         *       /  \        __/  \__
         *                  /        \
         *              S(B)          n(R)
         *                            /  \
         *                        L(B)    R(B)
         *                        /  \    /  \
         *
         *  left_rotate(G);
         * ------------------>
         *
         *                     |
         *                    p(R)
         *                 ___/  \___
         *                /          \
         *            G(B)            n(R)
         *            /  \            /  \
         *        U(B)    S(B)    L(B)    R(B)
         *        /  \    /  \    /  \    /  \
         *
         *  flip_color(p); flip_color(G);
         * ------------------------------->
         *
         *                     |
         *                    P(B)
         *                 ___/  \___
         *                /          \
         *            G(R)            n(R)
         *            /  \            /  \
         *        U(B)    S(B)    L(B)    R(B)
         *        /  \    /  \    /  \    /  \
         *
         */
        if (xwlib_rbtree_tst_right(node->lpc.v)) {
                tmp.sibling = parent->left;
                rotation = xwlib_rbtree_get_lr(parent); /* left_rotate gparent */
        } else {
                tmp.sibling = parent->right;
                rotation = xwlib_rbtree_get_rr(parent); /* right_rotate gparent */
        }
        lpc = parent->lpc.v;

        *xwlib_rbtree_get_link(gparent->lpc.v) = parent;
        parent->lpc.v = gparent->lpc.v; /* flip_color(p): black */

        *xwlib_rbtree_get_link(lpc) = tmp.sibling;
        if (tmp.sibling) {
                tmp.sibling->lpc.v = ((xwptr_t)lpc) | XWLIB_RBTREE_COLOR_BLACK;
        }/* else {} */

        *xwlib_rbtree_get_link(rotation) = gparent;
        gparent->lpc.v = rotation; /* flip_color(G): red */
}

/**
 * @brief 删除一个节点，并修正红黑树的颜色
 * @param tree: (I) 红黑树的指针
 * @param node: (I) 待删除的节点的指针
 * @note
 * - 这个函数假设不存在节点没有链接到红黑树的情形。
 */
__xwlib_code __hot
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

        /* Stage 1: remove node */
        rr.right =  node->right;
        sl.left =  node->left;
        if (!sl.left) {
                parent = xwlib_rbtree_get_parent(node);
                *xwlib_rbtree_get_link(node->lpc.v) = rr.right;
                if (!rr.right) {
                        /*
                         * Case 1: Node has no child.
                         *
                         *        |
                         *      p(Cp)
                         *      /   \
                         * s(Cs)     n(Cn)
                         *
                         *  transplant_nil(n)
                         * -------------------->
                         *
                         *       |                            |
                         *     p(Cp)                        p(Cp)
                         *     /   \         or             /   \          or
                         * s(R)     NULL(B)             S(B)     NULL(BB)
                         *                              /  \
                         *                       NULL(B)    NULL(B)
                         *
                         *           |
                         *         p(Cp)
                         *         /   \
                         *      s(R)    NULL(BB)
                         *      /  \
                         *  L(B)    R(B)
                         *
                         * ******** ******** ******** ******** ******** ********
                         *  OR
                         * ******** ******** ******** ******** ******** ********
                         *
                         *        |
                         *      p(Cp)
                         *      /   \
                         * n(Cn)     s(Cn)
                         *
                         *  transplant_nil(n)
                         * -------------------->
                         *
                         *          |                      |
                         *        p(Cp)                  p(Cp)
                         *        /   \      or          /   \              or
                         * NULL(B)     s(R)      NULL(BB)     S(B)
                         *                                    /  \
                         *                             NULL(B)    NULL(B)
                         *
                         *           |
                         *         p(Cp)
                         *         /   \
                         * NULL(BB)     s(R)
                         *              /  \
                         *          L(B)    R(B)
                         *
                         * * If node is red, the sibling is NULL or red due to 5).
                         * * If node is black, the sibling is not NULL
                         *   due to 5). And if sibling is red, it has two
                         *   black children. If sibling is black, it has
                         *   no child.
                         */
                        if (NULL == tree->root) {
                                return;
                        }

                        lpc = node->lpc.v;
                        if (xwlib_rbtree_color_tst_black(lpc)) {
                                /*
                                 * `rotation' is Used to right-rotate parent
                                 * in stage 2.
                                 */
                                if (xwlib_rbtree_tst_right(lpc)) {
                                        sibling = parent->left;
                                        rotation = xwlib_rbtree_get_rr(sibling);
                                } else {
                                        sibling = parent->right;
                                        rotation = xwlib_rbtree_get_lr(sibling);
                                }
                        } else {
                                sibling = NULL;
                        }
                } else {
                        /*
                         * Case 2.1: Node has no left child.
                         *
                         *    |
                         *   n(B)
                         *      \
                         *       r(R)
                         *
                         *  transplant(n->right, n)
                         * ------------------------->
                         *
                         *    |
                         *  r(RB)
                         *
                         *  set_black(r)
                         * ---------------->
                         *
                         *    |
                         *   r(B)
                         *
                         * If there is one child, it must be red due to 5),
                         * and the node is black due to 4).
                         */
                        rr.right->lpc.v = node->lpc.v; /* Inherit color */
                        sibling = NULL;
                }
        } else if (!rr.right) {
                /*
                 * Case 2.2: Node has no right child.
                 *
                 *       |
                 *      n(B)
                 *      /
                 *  r(R)
                 *
                 *  transplant(n->left, n)
                 * ------------------------->
                 *
                 *    |
                 *  r(RB)
                 *
                 *  set_black(r)
                 * ---------------->
                 *
                 *    |
                 *   r(B)
                 *
                 * If there is one child it must be red due to 5),
                 * and the node is black due to 4).
                 */
                xwlib_rbtree_link(sl.left, node->lpc.v); /* Inherit color */
                sibling = NULL;
        } else {
                /*
                 * Case 3: the left child & right child are existing.
                 */
                struct xwlib_rbtree_node * successor;

                successor = xwlib_rbtree_get_successor(node);
                if (successor == rr.right) {
                        /*
                         * Case 3.1: Node(n)'s successor is its right child(r).
                         *
                         *        |
                         *       n(Cn)
                         *      /   \
                         * l(Cl)     s(Cs)
                         *               \
                         *                c(R)_or_NULL(B)
                         *
                         *   transplant(s, n)
                         *  -------------------------------------------->
                         *   (s inherits color of n and leaves Cr to c)
                         *
                         *      |
                         *     s(Cn)                         n()
                         *         \                         /
                         *          c(RB)_or_NULL(BB)   l(Cl)
                         *
                         *   link(l, ((&s->left) | (RBTREE_LEFT) | color(l)))
                         *  -------------------------------------------------->
                         *
                         *         |
                         *       s(Cn)
                         *       /   \
                         *  l(Cl)     c(B)_or_NULL(BB)
                         *
                         * - Right child of successor must be red if existing
                         *   due to 5), and the successor is black due to 4).
                         * - If successor is red, it has no child due to 5).
                         * - If successor has no right child and successor is
                         *   black, left must be not NIL due to 5).
                         *
                         */

                        if (successor->right) {
                                xwlib_rbtree_set_black(successor->right);
                                sibling = NULL;
                        } else {
                                /*
                                 * `rotation' is Used to right-rotate parent
                                 * in stage 2.
                                 */
                                if (xwlib_rbtree_color_tst_black(successor->lpc.v)) {
                                        sibling = sl.left;
                                        parent = successor;
                                        rotation = xwlib_rbtree_get_rr(sibling);
                                } else {
                                        sibling = NULL;
                                }
                        }
                        xwlib_rbtree_transplant(successor, node);
                        cc.color = xwlib_rbtree_get_color(sl.left->lpc.v);
                        xwlib_rbtree_link(sl.left,
                                          xwlib_rbtree_get_lc(successor, cc.color));
                } else {
                        /*
                         * Case 3.2: Node(n)'s successor(s) is leftmost under
                         *           node(n)'s right child subtree.
                         *
                         *        |
                         *      n(Cn)
                         *      /   \
                         *     l     r
                         *          / \
                         *         p
                         *        / \
                         *   s(Cs)
                         *       \
                         *        c(R)_or_NIL(B)
                         *
                         *   transplant(s->right, s)
                         *  ------------------------->
                         *
                         *                  |
                         *                n(Cn)            s()
                         *                /   \
                         *               l     r
                         *                    / \
                         *                   p
                         *                  / \
                         *  c(RB)_or_NIL(BB)
                         *
                         *   link(r, ((&s->right) | (RBTREE_RIGHT) | color(r)))
                         *  ---------------------------------------------------->
                         *
                         *      |
                         *    n(Cn)              s()
                         *    /                    \
                         *   l                      r
                         *                         / \
                         *                        p
                         *                       / \
                         *        C(B)_or_NIL(BB)
                         *
                         *   link(l, ((&s->left) | (RBTREE_LEFT) | color(l)))
                         *  -------------------------------------------------->
                         *
                         *       |
                         *     n(Cn)            s()
                         *                      / \
                         *                     l   r
                         *                        / \
                         *                       p
                         *                      / \
                         *       C(B)_or_NIL(BB)
                         *
                         *   transplant(s, n)
                         *  ------------------>
                         *
                         *                    |
                         *                  s(Cn)
                         *                  /   \
                         *                 l     r
                         *                      / \
                         *                     p
                         *                    / \
                         *     C(B)_or_NIL(BB)
                         *
                         * - Right child of successor must be red if existing due to 5),
                         *   and the successor is black due to 4).
                         * - If successor has no right child and successor is black,
                         *   left must be not NULL due to 5).
                         */
                        parent = xwlib_rbtree_get_parent(successor);
                        lpc = successor->lpc.v; /* cache */
                        cc.child = successor->right;

                        *xwlib_rbtree_get_link(lpc) = cc.child;
                        if (cc.child) {
                                cc.child->lpc.v = lpc; /* Inherit color */
                                sibling = NULL;
                        } else {
                                /*
                                 * `rotation' is Used to left-rotate parent in stage 2.
                                 */
                                if (xwlib_rbtree_color_tst_black(successor->lpc.v)) {
                                        sibling = parent->right;
                                        rotation = xwlib_rbtree_get_lr(sibling);
                                } else {
                                        sibling = NULL;
                                }
                        }

                        /* link(r, ((&s->right) | (RBTREE_RIGHT) | color(r))) */
                        cc.color = xwlib_rbtree_get_color(rr.right->lpc.v);
                        xwlib_rbtree_link(rr.right,
                                          xwlib_rbtree_get_rc(successor, cc.color));

                        /* link(r, ((&s->right) | (RBTREE_RIGHT) | color(r))) */
                        cc.color = xwlib_rbtree_get_color(sl.left->lpc.v);
                        xwlib_rbtree_link(sl.left,
                                          xwlib_rbtree_get_lc(successor, cc.color));

                        /* transplant(s, n) */
                        xwlib_rbtree_transplant(successor, node);
                }
        }

        /* Stage 2: fix color */
        if (!sibling) {
                return;
        }

recursively_fix:
        if (xwlib_rbtree_color_tst_red(sibling->lpc.v)) {
                /*
                 * Case 1: Sibling is red.
                 *                          *
                 *         |                *           |
                 *        P(B)              *          P(B)
                 *        /  \              *          /  \
                 *   X(BB)    s(R)          *      s(R)    X(BB)
                 *            /  \          *      /  \
                 *        L(B)    R(B)      *  L(B)    R(B)
                 *                          *
                 *  left_rotate(P)          *  right_rotate(P)
                 * ---------------->        * ----------------->
                 *                          *
                 *            |             *           |
                 *           s(R)           *          s(R)
                 *           /  \           *          /  \
                 *       P(B)    R(B)       *      L(B)    P(B)
                 *       /  \               *              /  \
                 *  X(BB)    L(B)           *          R(B)    X(BB)
                 *                          *
                 *  flip_color(s);          *  flip_color(s);
                 * ---------------->        * ---------------->
                 *  flip_color(P);          *  flip_color(P);
                 *                          *
                 *            |             *           |
                 *           S(B)           *          S(B)
                 *           /  \           *          /  \
                 *       p(R)    R(B)       *      L(B)    p(R)
                 *       /  \               *              /  \
                 *  X(BB)    L(B)           *          R(B)    X(BB)
                 *                          *
                 * L is new sibling.        * R is new sibling.
                 *                          *
                 * - Parent must be black due to 4).
                 * - Sibling has two black children due to 5).
                 */
                lpc = sibling->lpc.v;
                cc.child = *xwlib_rbtree_get_link(rotation);

                *xwlib_rbtree_get_link(parent->lpc.v) = sibling;
                sibling->lpc.v = parent->lpc.v; /* flip_color: black */

                *xwlib_rbtree_get_link(lpc) = cc.child;
                cc.child->lpc.v = lpc | XWLIB_RBTREE_COLOR_BLACK;

                /* xwlib_rbtree_link(parent, rotation); */
                *xwlib_rbtree_get_link(rotation) = parent;
                parent->lpc.v = rotation; /* flip_color: red */

                /* parent = parent; */ /* omitted */
                sibling = cc.child;
        }

black_sibling:
        /*
         * Case 2: Sibling is black.
         */
        if (xwlib_rbtree_tst_right(sibling->lpc.v)) {
                /* `rotation' is used to left rotate parent in case 2.3. */
                rotation = xwlib_rbtree_get_lr(sibling);
                rr.reverse = sibling->left;
                sl.same = sibling->right;
                /* `lpc' is used to right rotate reverse in case 2.2. */
                if (rr.reverse) {
                        lpc = xwlib_rbtree_get_rr(rr.reverse);
                }/* else {} */
        } else {
                /* `rotation' is used to right rotate parent in case 2.3. */
                rotation = xwlib_rbtree_get_rr(sibling);
                rr.reverse = sibling->right;
                sl.same = sibling->left;
                /* `lpc' is used to left rotate reverse in case 2.2. */
                if (rr.reverse) {
                        lpc = xwlib_rbtree_get_lr(rr.reverse);
                }/* else {} */
        }

        if (!sl.same || xwlib_rbtree_color_tst_black(sl.same->lpc.v)) {
                if (!rr.reverse || xwlib_rbtree_color_tst_black(rr.reverse->lpc.v)) {
                        /*
                         * Case 2.1: Sibling has no red child.
                         *
                         *             |
                         *           p(Cp)
                         *           /   \
                         *       S(B)     X(BB)
                         *       /  \
                         *  Cs(B)    Cr(B)
                         *
                         *  flip_color(p); flip_color(S); flip_color(X);
                         * ---------------------------------------------->
                         *
                         *             |
                         *           p(RB_or_BB)
                         *           /    \
                         *       s(R)      X(B)
                         *       /  \
                         *  Cs(B)    Cr(B)
                         *
                         *  If p is red, set_black(p)
                         * --------------------------->
                         *  If p is black, p is new X.
                         *
                         * ******** ******** ******** ******** ******** ********
                         *  OR
                         * ******** ******** ******** ******** ******** ********
                         *
                         *            |
                         *          p(Cp)
                         *          /   \
                         *     X(BB)     S(B)
                         *               /  \
                         *          Cr(B)    Cs(B)
                         *
                         *  flip_color(p); flip_color(S); flip_color(X);
                         * ---------------------------------------------->
                         *
                         *            |
                         *          p(RB_or_BB)
                         *          /    \
                         *      X(B)      s(R)
                         *                /  \
                         *           Cr(B)    Cs(B)
                         *
                         *  If p is red, set_black(p)
                         * --------------------------->
                         *  If p is black, p is new X.
                         *
                         */
                        xwlib_rbtree_flip_color(sibling);
                        if (xwlib_rbtree_color_tst_black(parent->lpc.v)) {
                                if (parent != tree->root) {
                                        cc.child = parent;
                                        parent = xwlib_rbtree_get_parent(cc.child);
                                        if (xwlib_rbtree_tst_right(cc.child->lpc.v)) {
                                                sibling = parent->left;
                                                rotation = xwlib_rbtree_get_rr(sibling);
                                        } else {
                                                sibling = parent->right;
                                                rotation = xwlib_rbtree_get_lr(sibling);
                                        }
                                        goto recursively_fix;
                                }
                        } else {
                                xwlib_rbtree_flip_color(parent);
                        }
                        return;
                }

                /*
                 * Case 2.2: Child in reverse side is red and child in same side
                 *           is black.
                 *
                 *             |
                 *           p(Cp)
                 *           /   \
                 *       S(B)     X(BB)
                 *       /  \
                 *  Cs(B)    cr(R)
                 *           /
                 *      Gc(B)
                 *
                 *  left_rotate(S);
                 * ----------------->
                 *
                 *                  |
                 *                p(Cp)
                 *                /   \
                 *           cr(R)     X(BB)
                 *           /
                 *       S(B)
                 *       /  \
                 *  Cs(B)    Gc(B)
                 *
                 *  flip_color(cr); flip_color(S)
                 * ------------------------------->
                 *
                 *                  |
                 *                p(Cp)
                 *                /   \
                 *           Cr(B)     X(BB)
                 *           /
                 *       s(R)
                 *       /  \
                 *  Cs(B)    Gc(B)
                 *
                 * ******** ******** ******** ******** ******** ********
                 *  OR
                 * ******** ******** ******** ******** ******** ********
                 *
                 *            |
                 *          p(Cp)
                 *          /   \
                 *     X(BB)     S(B)
                 *               /  \
                 *          cr(R)    Cs(B)
                 *              \
                 *               Gs(B)
                 *
                 *  right_rotate(S);
                 * ------------------>
                 *
                 *            |
                 *          p(Cp)
                 *          /  \
                 *     X(BB)    cr(R)
                 *                  \
                 *                   S(B)
                 *                   /  \
                 *              Gc(B)    Cs(B)
                 *
                 *  flip_color(cr); flip_color(S)
                 * ------------------------------->
                 *
                 *            |
                 *          p(Cp)
                 *          /   \
                 *     X(BB)     Cr(B)
                 *                   \
                 *                    s(R)
                 *                    /  \
                 *               Gc(B)    Cs(B)
                 *
                 * Transform into case 2.3.
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

                /* Transform into case 2.3. */
                sibling = rr.reverse;
                goto black_sibling;
        }

        /*
         * Case 2.3: Child in same side is red.
         *
         *             |
         *           p(Cp)
         *           /   \
         *       S(B)     X(BB)
         *       /  \
         *  cs(R)    cr(Ccr)
         *
         *  right_rotate(p);
         * ------------------>
         *  leave color
         *
         *             |
         *           S(Cp)
         *           /   \
         *     Cs(RB)     P(B)
         *                /  \
         *         cr(Ccr)    X(B)
         *
         * ******** ******** ******** ******** ******** ********
         *  OR
         * ******** ******** ******** ******** ******** ********
         *
         *             |
         *           p(Cp)
         *           /   \
         *      X(BB)     S(B)
         *                /  \
         *         cr(Ccr)    cs(R)
         *
         *  left_rotate(p);
         * ------------------>
         *  leave color
         *
         *               |
         *             S(Cp)
         *             /   \
         *         P(B)     cs(RB)
         *         /  \
         *     X(B)    cr(Ccr)
         *
         *  S leaves color 'B' to cs and inherits color 'Cp' from p.
         *  p leaves color 'Cp' and inherits one color 'B' from X.
         *  X leaves one color 'B'.
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
__xwlib_code __hot
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
