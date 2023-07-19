/**
 * @file
 * @brief XWOS通用库：红黑树
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef __xwos_lib_rbtree_h__
#define __xwos_lib_rbtree_h__

#include <xwos/standard.h>

/**
 * @defgroup xwos_lib_rbtree 红黑树
 * @ingroup xwos_lib
 * ## 红黑树性质
 *
 * + 1. 节点不是红色就是黑色；
 * + 2. 根节点是黑色；
 * + 3. 所有的叶子都是黑色；
 * + 4. 每个红色的节点，如果有子节点，必须是黑色；
 * + 5. 从任意节点到其任意子树的叶子的路径上，所包含的黑色节点数量相同。
 *
 *
 * ## 本算法的特点
 *
 * 和传统的红黑树算法不同，本算法实现时比较有技巧性。
 *
 * + 高效地查找父节点
 *   + 在每个节点中引入一个 **链指针** ，目的是能快速找到父节点的地址。
 *     + **链指针** 是一个双重指针，指向本节点在父节点上连接的位置，通过 **链指针**
 *       可快速计算出父节点的地址。而传统二叉树的实现查找父节点时需要进行遍历。
 *   + 实现
 *     + `xwlib_rbtree_get_parent()`
 *     + `xwlib_rbtree_get_parent_from_lpc()`
 * + 节省内存
 *   + 由于强制约定节点的内存地址必须按 `sizeof(void *)` 对齐，
 *     节点结构体内部三个指针成员肯定也是按照 `sizeof(void *)` 对齐的，
 *     也就是说它们的地址的低两位必定为 `0b00` 。
 *   + 使用 **链指针** 指向 `left` 或 `right` 时，
 *     **链指针** 在数值上的低两位必定为 `0b00` 。
 *   + **链指针** 这两个空闲的位可用来存放 **位置(pos)** 和 **颜色(color)** 信息。
 *   + **链指针** 、 **位置(pos)** 和 **颜色(color)** 统一被称为 **lpc** 。
 * + 高通用性
 *   + 用户可将红黑树的节点结构体 `xwlib_rbtree_node` 嵌入到自己的结构体中来使用本算法。
 *     已知红黑树节点的指针，可通过 @ref xwlib_rbtree_entry() 来计算用户结构体的指针。
 *   + 用户需要自己实现二叉树的查找、插入和删除方法：
 *     + 查找：可以参考 @ref xwlib_map_find() 。用户需要自己实现遍历的循环。
 *     + 插入：可以参考 @ref xwlib_map_insert() 。用户需要自己实现遍历的循环，
 *       插入后，可通过 `xwlib_rbtree_insert_color()` 调整红黑树颜色。
 *     + 删除：可以参考 @ref xwlib_map_erase() 。
 *       使用 `xwlib_rbtree_remove()` 来删除节点。
 *
 * @{
 */

/**
 * @brief 红黑树节点位置枚举
 */
enum xwlib_rbtree_pos_em {
        XWLIB_RBTREE_POS_LEFT = 0, /**< 左 */
        XWLIB_RBTREE_POS_RIGHT = 1, /**< 右 */
};

/**
 * @brief 红黑树节点颜色枚举
 */
enum xwlib_rbtree_color_em {
        XWLIB_RBTREE_COLOR_RED = 0, /**< 红色 */
        XWLIB_RBTREE_COLOR_BLACK = 2, /**< 黑色 */
};

/**
 * @brief 红黑树节点
 */
struct __xwcc_alignptr xwlib_rbtree_node {
        struct xwlib_rbtree_node * left; /**< 指向左孩子 */
        struct xwlib_rbtree_node * right; /**< 指向右孩子 */
        union {
                struct xwlib_rbtree_node ** link; /**< 链指针，指向父节点中连接的位置 */
                xwptr_t pos; /**< 只使用 `bit0` ，位置信息 */
                xwptr_t color; /**< 只使用 `bit1` ，颜色信息 */
                xwptr_t v; /**< 所有信息的整数表达 */
        } lpc; /**< 链指针，位置，颜色的合并信息 */
};

/**
 * @brief 红黑树
 */
struct __xwcc_alignptr xwlib_rbtree {
        struct xwlib_rbtree_node * root; /**< 根节点 */
};

/**
 * @brief 初始化红黑树
 * @param[in] rbt: 红黑树指针
 */
static __xwlib_inline
void xwlib_rbtree_init(struct xwlib_rbtree * rbt)
{
        rbt->root = NULL;
}

/**
 * @brief 初始化红黑树节点
 * @param[in] rbn: 红黑树节点指针
 */
static __xwlib_inline
void xwlib_rbtree_init_node(struct xwlib_rbtree_node * rbn)
{
        rbn->left = NULL;
        rbn->right = NULL;
        rbn->lpc.v = 0;
}

/**
 * @brief 获取lpc信息中的link指针
 * @param[in] lpc: 链指针，位置，颜色的合并值
 * @return 链指针
 */
#define xwlib_rbtree_get_link(lpc) \
        ((struct xwlib_rbtree_node **)(((xwptr_t)(lpc)) & (~((xwptr_t)3UL))))

/**
 * @brief 获取lpc信息中的link指针和位置信息（屏蔽颜色信息）
 * @param[in] lpc: 链指针，位置，颜色的合并值
 * @return 链指针和位置信息
 */
#define xwlib_rbtree_get_lnpos(lpc)     ((xwptr_t)(lpc) & (~((xwptr_t)2UL)))

/**
 * @brief 获取lpc信息中的位置信息
 * @param[in] lpc: 链指针，位置，颜色的合并值
 * @return 位置信息
 */
#define xwlib_rbtree_get_pos(lpc)       ((xwptr_t)(((xwptr_t)(lpc)) & (xwptr_t)1UL))

/**
 * @brief 测试当前lpc中的位置信息是否为右侧
 * @param[in] lpc: 链指针，位置，颜色的合并值
 * @return 布尔值
 * @retval true: 是
 * @retval false: 否
 */
#define xwlib_rbtree_tst_right(lpc)     (!!xwlib_rbtree_get_pos(lpc))

/**
 * @brief 测试当前lpc中的位置信息是否为左侧
 * @param[in] lpc: 链指针，位置，颜色的合并值
 * @return 布尔值
 * @retval true: 是
 * @retval false: 否
 */
#define xwlib_rbtree_tst_left(lpc)      (!xwlib_rbtree_tst_right(lpc))

/**
 * @brief 生成节点左边的链指针、位置和颜色信息
 * @param[in] n: 父节点指针
 * @param[in] color: 颜色
 * @return 指向节点左边的链指针、位置和颜色信息
 */
#define xwlib_rbtree_gen_lc(n, color) \
        ((xwptr_t)(&((n)->left)) | (xwptr_t)(color))

/**
 * @brief 生成节点右边的链指针、位置和颜色信息
 * @param[in] n: 父节点指针
 * @param[in] color: 颜色
 * @return 指向节点右边的链指针、位置和颜色信息
 */
#define xwlib_rbtree_gen_rc(n, color) \
        ((xwptr_t)(&((n)->right)) | (xwptr_t)(color) | (xwptr_t)XWLIB_RBTREE_POS_RIGHT)

/**
 * @brief 生成节点左边的链指针、位置和红色信息
 * @param[in] n: 节点指针
 * @return 指向节点左边的链指针、位置和红色信息
 */
#define xwlib_rbtree_gen_lr(n) xwlib_rbtree_gen_lc((n), XWLIB_RBTREE_COLOR_RED)

/**
 * @brief 生成节点左边的链指针、位置和红色信息
 * @param[in] n: 节点指针
 * @return 指向节点左边的链指针、位置和黑色信息
 */
#define xwlib_rbtree_gen_lb(n) xwlib_rbtree_gen_lc((n), XWLIB_RBTREE_COLOR_BLACK)

/**
 * @brief 生成节点右边的链指针、位置和红色信息
 * @param[in] n: 节点指针
 * @return 指向节点右边的链指针、位置和红色信息
 */
#define xwlib_rbtree_gen_rr(n) xwlib_rbtree_gen_rc((n), XWLIB_RBTREE_COLOR_RED)

/**
 * @brief 生成节点右边的链指针、位置和红色信息
 * @param[in] n: 节点指针
 * @return 指向节点右边的链指针、位置和黑色信息
 */
#define xwlib_rbtree_gen_rb(n) xwlib_rbtree_gen_rc((n), XWLIB_RBTREE_COLOR_BLACK)

/**
 * @brief 获取lpc信息中的颜色信息
 * @param[in] lpc: 链指针，位置，颜色的合并值
 * @return 颜色信息
 */
#define xwlib_rbtree_get_color(lpc)             (((xwptr_t)(lpc)) & (xwptr_t)2UL)

/**
 * @brief 测试当前lpc中的颜色信息是否为黑色
 * @param[in] lpc: 链指针，位置，颜色的合并值
 * @retval true: 是
 * @retval false: 否
 */
#define xwlib_rbtree_tst_black(lpc)             (!!xwlib_rbtree_get_color(lpc))

/**
 * @brief 测试当前lpc中的颜色信息是否为红色
 * @param[in] lpc: 链指针，位置，颜色的合并值
 * @retval true: 是
 * @retval false: 否
 */
#define xwlib_rbtree_tst_red(lpc)               (!xwlib_rbtree_get_color(lpc))

/**
 * @brief 从红黑树节点指针值计算出包含该节点成员的外层结构体的指针值
 * @param[in] ptr: 红黑树节点指针
 * @param[in] type: 外层结构体类型
 * @param[in] member: 红黑树节点在外层结构体中的成员符号名(symbol)
 */
#define xwlib_rbtree_entry(ptr, type, member)   xwcc_derof((ptr), type, member)

/**
 * @brief 返回节点的父节点指针
 * @param[in] node: 子节点指针
 * @return 父节点指针
 * @note
 * + `left` 是 @ref xwlib_rbtree_node 的第一个成员。
 * + `&parent->left` 与 `&parent` 在数值上是相等的（指针
 *   就是位宽等于CPU位宽的无符号整数）。
 *   + 当 `link` 指向 `parent->left` 时，其数值就等于 `parent` 的地址；
 *   + 当 `link` 指向 `parent->right` 时，其数值减去 `sizeof(void *)` 后，
 *     可获取 `parent` 地址。
 * + 当父节点不存在，表示此节点是红黑树的第一个节点，其 `link` 指针指向 `root` ，
 *   此函数返回 `root` 的地址。
 */
static __xwlib_inline
struct xwlib_rbtree_node * xwlib_rbtree_get_parent(struct xwlib_rbtree_node * node)
{
        xwptr_t pos;
        struct xwlib_rbtree_node ** link;

        pos = xwlib_rbtree_get_pos(node->lpc.pos);
        link = &(xwlib_rbtree_get_link(node->lpc.link)[-pos]);
        return (struct xwlib_rbtree_node *)link;
}

/**
 * @brief 从lpc信息返回父节点指针
 * @param[in] lpc: 链指针，位置，颜色的合并值
 * @return 父节点指针
 * @note
 * `lpc` 是从节点内取出的值，说明同 @ref xwlib_rbtree_get_parent() 。
 */
static __xwlib_inline
struct xwlib_rbtree_node * xwlib_rbtree_get_parent_from_lpc(xwptr_t lpc)
{
        xwptr_t pos;
        struct xwlib_rbtree_node ** link;

        pos = xwlib_rbtree_get_pos(lpc);
        link = &(xwlib_rbtree_get_link(lpc)[-pos]);
        return (struct xwlib_rbtree_node *)link;
}

/**
 * @brief 测试当前节点是否为根节点
 * @param[in] rbt: 红黑树指针
 * @param[in] node: 节点指针
 * @return 布尔值
 * @retval true: 是
 * @retval false: 否
 */
static __xwlib_inline
bool xwlib_rbtree_tst_root(struct xwlib_rbtree * rbt,
                           struct xwlib_rbtree_node * node)
{
        struct xwlib_rbtree_node * p;

        p = xwlib_rbtree_get_parent(node);
        return !!(p == (struct xwlib_rbtree_node *)&rbt->root);
}

/**
 * @brief 测试是否连接到根节点（是否为根节点的子节点）
 * @param[in] rbt: 红黑树指针
 * @param[in] link: 链指针
 * @return 布尔值
 * @retval true: 是
 * @retval false: 否
 */
static __xwlib_inline
bool xwlib_rbtree_tst_link_root(struct xwlib_rbtree * rbt,
                                struct xwlib_rbtree_node ** link)
{
        return !!(link == &rbt->root);
}

/**
 * @brief 反转节点的颜色
 * @param[in] node: 红黑树节点指针
 */
static __xwlib_inline
void xwlib_rbtree_flip_color(struct xwlib_rbtree_node * node)
{
        node->lpc.color ^= (xwptr_t)2UL;
}

/**
 * @brief 将节点设置为黑色
 * @param[in] node: 红黑树节点指针
 */
static __xwlib_inline
void xwlib_rbtree_set_black(struct xwlib_rbtree_node * node)
{
        node->lpc.color |= (xwptr_t)2UL;
}

/**
 * @brief 将节点设置为红色
 * @param[in] node: 红黑树节点指针
 */
static __xwlib_inline
void xwlib_rbtree_set_red(struct xwlib_rbtree_node * node)
{
        node->lpc.color &= ~((xwptr_t)(2UL));
}

/**
 * @brief 链接节点，并设置位置及颜色信息
 * @param[in] node: 被链接的节点指针
 * @param[in] lpc: 链指针、位置和颜色的合并值
 */
static __xwlib_inline
void xwlib_rbtree_link(struct xwlib_rbtree_node * node, xwptr_t lpc)
{
        node->lpc.v = lpc;
        *xwlib_rbtree_get_link(lpc) = node;
}

/**
 * @brief 链接空（叶子）节点
 * @param[in] link: 链接的位置
 */
static __xwlib_inline
void xwlib_rbtree_link_nil(struct xwlib_rbtree_node ** link)
{
        *link = NULL;
}

/**
 * @brief 返回节点的前趋节点
 * @param[in] node: 节点指针
 * @return 前趋节点指针
 */
static __xwlib_inline
struct xwlib_rbtree_node *
xwlib_rbtree_get_predecessor(struct xwlib_rbtree_node * node)
{
        struct xwlib_rbtree_node * c = node->left;
        if (!c) {
                c = node;
        } else {
                while (NULL != c->right) {
                        c = c->right;
                }
        }
        return c;
}

/**
 * @brief 返回节点的后继节点
 * @param[in] node: 节点指针
 * @return 后继节点指针
 */
static __xwlib_inline
struct xwlib_rbtree_node *
xwlib_rbtree_get_successor(struct xwlib_rbtree_node * node)
{
        struct xwlib_rbtree_node * c = node->right;
        if (!c) {
                c = node;
        } else {
                while (NULL != c->left) {
                        c = c->left;
                }
        }
        return c;
}

/**
 * @brief 用新节点代替旧节点并继承它的链指针、颜色和位置信息，
 *        但新节点并不接管旧节点的子节点
 * @param[in] newn: 新节点指针
 * @param[in] oldn: 旧节点指针
 */
static __xwlib_inline
void xwlib_rbtree_transplant(struct xwlib_rbtree_node * newn,
                             struct xwlib_rbtree_node * oldn)
{
        newn->lpc.v = oldn->lpc.v;
        *xwlib_rbtree_get_link(oldn->lpc.v) = newn;
}

/**
 * @brief 用叶子(NIL)替换一个旧节点
 * @param[in] oldn: 旧节点指针
 */
static __xwlib_inline
void xwlib_rbtree_transplant_nil(struct xwlib_rbtree_node * oldn)
{
        *xwlib_rbtree_get_link(oldn->lpc.v) = NULL;
}

/**
 * @brief 测试一棵红黑树是否为空树
 * @param[in] t: 红黑树指针
 * @retval false: 非空
 * @retval true: 空
 */
static __xwlib_inline
bool xwlib_rbtree_tst_empty(struct xwlib_rbtree * t)
{
        return !!(NULL == t->root);
}

/**
 * @brief 测试红黑树节点是否已经链接到红黑树中
 * @param[in] n: 节点指针
 * @retval false: 已链接
 * @retval true: 未链接
 */
static __xwlib_inline
bool xwlib_rbtree_tst_node_unlinked(struct xwlib_rbtree_node * n)
{
        return !!((xwptr_t)0 == n->lpc.v);
}

/**
 * @brief 插入一个红色节点后修正红黑树的颜色
 * @param[in] tree: 红黑树的指针
 * @param[in] node: 待修正颜色的节点的指针
 */
void xwlib_rbtree_insert_color(struct xwlib_rbtree * tree,
                               struct xwlib_rbtree_node * node);

/**
 * @brief 删除一个节点，并修正红黑树的颜色
 * @param[in] tree: 红黑树的指针
 * @param[in] node: 待删除的节点的指针
 * @note
 * + 此函数假设不存在节点没有链接到红黑树的情形。
 */
void xwlib_rbtree_remove(struct xwlib_rbtree * tree,
                         struct xwlib_rbtree_node * node);

/**
 * @brief 用一个新节点代替旧节点，继承它的颜色、位置信息并接管它的子树
 * @param[in] newn: 新节点的指针
 * @param[in] oldn: 旧节点的指针
 */
void xwlib_rbtree_replace(struct xwlib_rbtree_node * newn,
                          struct xwlib_rbtree_node * oldn);

/**
 * @} xwos_lib_rbtree
 */

#endif /* xwos/lib/rbtree.h */
