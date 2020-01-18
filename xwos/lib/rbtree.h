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
 * @note
 * > 红黑树性质:
 * > + 节点不是红色就是黑色；
 * > + 根节点是黑色；
 * > + 所有的叶子都是黑色；
 * > + 每个红色的节点，如果有子节点，必须是黑色；
 * > + 从任意节点到其任意子孙叶子的路径上，所包含的黑色节点数量相同。
 */

#ifndef __xwos_lib_rbtree_h__
#define __xwos_lib_rbtree_h__

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
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
 * @note
 * > - 不使用位定义，因为在大端机器（PowerPC）和小端机器(ARM/X86)中，位的编号方式不同。
 * > - 此结构体的内存地址必须按4字节对齐，其地址信息的低两位必须为0。
 * >   这份代码实现的红黑树算法使用这两位分别存放位置(pos)和颜色(color)信息。
 * - 当位置为XWLIB_RBTREE_POS_RIGHT，当前节点为右孩子，其link指针指向父节点的right。
 * - 当位置为XWLIB_RBTREE_POS_LEFT，当前节点为左孩子，其link指针指向父节点的left。
 */
struct __alignptr xwlib_rbtree_node {
        struct xwlib_rbtree_node * left; /**< 指向左孩子 */
        struct xwlib_rbtree_node * right; /**< 指向右孩子 */
        union {
                struct xwlib_rbtree_node ** link; /**< 链指针，指向父节点中连接的位置 */
                xwptr_t pos; /**< 只使用bit0，位置信息 */
                xwptr_t color; /**< 只使用bit1，颜色信息 */
                xwptr_t v; /**< 所有信息 */
        } lpc; /**< 链指针，位置，颜色的合并信息 */
};

/**
 * @brief 红黑树
 */
struct __alignptr xwlib_rbtree {
        struct xwlib_rbtree_node * root; /**< 根节点 */
};

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** inline functions & macro functions  ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief 初始化红黑树
 * @param rbt: (I) 红黑树指针
 */
static __xwlib_inline
void xwlib_rbtree_init(struct xwlib_rbtree * rbt)
{
        rbt->root = NULL;
}

/**
 * @brief 初始化红黑树节点
 * @param rbn: (I) 红黑树节点指针
 */
static __xwlib_inline
void xwlib_rbtree_init_node(struct xwlib_rbtree_node * rbn)
{
        rbn->left = NULL;
        rbn->right = NULL;
        rbn->lpc.v = 0;
}

/**
 * @brief 获得lpc信息中的link指针
 * @param lpc: (I) 链指针，位置，颜色的合并值
 * @return 链指针
 */
#define xwlib_rbtree_get_link(lpc) \
        ((struct xwlib_rbtree_node **)(((xwptr_t)(lpc)) & (~((xwptr_t)3UL))))

/**
 * @brief 获得lpc信息中的link指针和位置信息（屏蔽颜色信息）
 * @param lpc: (I) 链指针，位置，颜色的合并值
 * @return 链指针和位置信息
 */
#define xwlib_rbtree_get_lnpos(lpc)     ((xwptr_t)lpc & (~((xwptr_t)2UL)))

/**
 * @brief 获得lpc信息中的位置信息
 * @param lpc: (I) 链指针，位置，颜色的合并值
 * @return 位置信息
 */
#define xwlib_rbtree_get_pos(lpc)       ((xwptr_t)(((xwptr_t)(lpc)) & (xwptr_t)1UL))

/**
 * @brief 测试当前lpc中的位置信息是否为右侧
 * @param lpc: (I) 链指针，位置，颜色的合并值
 * @retval true: 是
 * @retval false: 否
 */
#define xwlib_rbtree_tst_right(lpc)     (!!xwlib_rbtree_get_pos(lpc))

/**
 * @brief 测试当前lpc中的位置信息是否为左侧
 * @param lpc: (I) 链指针，位置，颜色的合并值
 * @retval true: 是
 * @retval false: 否
 */
#define xwlib_rbtree_tst_left(lpc)      (!xwlib_rbtree_tst_right(lpc))

/**
 * @brief 生成父节点左边的链指针、位置和颜色信息
 * @param parent: (I) 父节点指针
 * @param color: (I) 颜色
 * @return 指向父节点左边的链指针、位置和颜色信息
 */
#define xwlib_rbtree_get_lc(parent, color)      \
        ((xwptr_t)(&((parent)->left)) | (xwptr_t)(color))

/**
 * @brief 生成父节点右边的链指针、位置和颜色信息
 * @param parent: (I) 父节点指针
 * @param color: (I) 颜色
 * @return 指向父节点右边的链指针、位置和颜色信息
 */
#define xwlib_rbtree_get_rc(parent, color)      \
        ((xwptr_t)(&((parent)->right)) | (xwptr_t)(color) | XWLIB_RBTREE_POS_RIGHT)

/**
 * @brief 生成父节点左边的链指针、位置和红色信息
 * @param parent: (I) 父节点指针
 * @return 指向父节点左边的链指针、位置和红色信息
 */
#define xwlib_rbtree_get_lr(parent) xwlib_rbtree_get_lc(parent, XWLIB_RBTREE_COLOR_RED)

/**
 * @brief 生成父节点左边的链指针、位置和红色信息
 * @param parent: (I) 父节点指针
 * @return 指向父节点左边的链指针、位置和黑色信息
 */
#define xwlib_rbtree_get_lb(node) xwlib_rbtree_get_lc(node, XWLIB_RBTREE_COLOR_BLACK)

/**
 * @brief 生成父节点右边的链指针、位置和红色信息
 * @param parent: (I) 父节点指针
 * @return 指向父节点右边的链指针、位置和红色信息
 */
#define xwlib_rbtree_get_rr(node) xwlib_rbtree_get_rc(node, XWLIB_RBTREE_COLOR_RED)

/**
 * @brief 生成父节点右边的链指针、位置和红色信息
 * @param parent: (I) 父节点指针
 * @return 指向父节点右边的链指针、位置和黑色信息
 */
#define xwlib_rbtree_get_rb(node) xwlib_rbtree_get_rc(node, XWLIB_RBTREE_COLOR_BLACK)

/**
 * @brief 获得lpc信息中的颜色信息
 * @param lpc: (I) 链指针，位置，颜色的合并值
 * @return 颜色信息
 */
#define xwlib_rbtree_get_color(lpc)             (((xwptr_t)(lpc)) & (xwptr_t)2UL)

/**
 * @brief 测试当前lpc中的颜色信息是否为黑色
 * @param lpc: (I) 链指针，位置，颜色的合并值
 * @retval true: 是
 * @retval false: 否
 */
#define xwlib_rbtree_color_tst_black(lpc)       (!!xwlib_rbtree_get_color(lpc))

/**
 * @brief 测试当前lpc中的颜色信息是否为红色
 * @param lpc: (I) 链指针，位置，颜色的合并值
 * @retval true: 是
 * @retval false: 否
 */
#define xwlib_rbtree_color_tst_red(lpc)         (!xwlib_rbtree_get_color(lpc))

/**
 * @brief 从红黑树节点指针值计算出包含该节点成员的外层结构体的指针值
 * @param ptr: (I) 红黑树节点指针
 * @param type: (I) 外层结构体类型
 * @member: (I) 红黑树节点在外层结构体中的成员符号名(symbol)
 */
#define xwlib_rbtree_entry(ptr, type, member)   container_of(ptr, type, member)

/**
 * @brief 返回节点的父节点指针
 * @param node: (I) 子节点指针
 * @return 父节点指针
 * @note
 * - left是struct @ref xwlib_rbtree_node的第一个成员。
 * - &parent->left与&parent在数值上是相等的（指针的实质就是一个整数）。因此
 *   当link指向parent->left时，其数值也等于parent的地址；
 *   当link指向parent->right时，其数值减去sizeof(void *)后可获得parent地址。
 * - 当父节点不存在（红黑树第一个节点），其link指针指向root，此函数返回root的指针。
 */
static __xwlib_inline
struct xwlib_rbtree_node * xwlib_rbtree_get_parent(struct xwlib_rbtree_node * node)
{
        xwptr_t pos;
        struct xwlib_rbtree_node ** link;

        pos = xwlib_rbtree_get_pos(node->lpc.pos);
        link = xwlib_rbtree_get_link(node->lpc.link) - pos;
        return (struct xwlib_rbtree_node *)link;
}

/**
 * @brief 从lpc信息返回父节点指针
 * @param lpc: (I) 链指针，位置，颜色的合并值
 * @return 父节点指针
 */
static __xwlib_inline
struct xwlib_rbtree_node * xwlib_rbtree_lpc_to_parent(xwptr_t lpc)
{
        xwptr_t pos = xwlib_rbtree_get_pos(lpc);
        struct xwlib_rbtree_node ** link = xwlib_rbtree_get_link(lpc) - pos;
        return (struct xwlib_rbtree_node *)link;
}

/**
 * @brief 反转节点的颜色
 * @param node: (I) 红黑树节点指针
 */
static __xwlib_inline
void xwlib_rbtree_flip_color(struct xwlib_rbtree_node * node)
{
        node->lpc.color ^= (xwptr_t)2UL;
}

/**
 * @brief 将节点设为黑色
 * @param node: (I) 红黑树节点指针
 */
static __xwlib_inline
void xwlib_rbtree_set_black(struct xwlib_rbtree_node * node)
{
        node->lpc.color |= (xwptr_t)2UL;
}

/**
 * @brief 将节点设为红色
 * @param node: (I) 红黑树节点指针
 */
static __xwlib_inline
void xwlib_rbtree_set_red(struct xwlib_rbtree_node * node)
{
        node->lpc.color &= ~((xwptr_t)(2UL));
}

/**
 * @brief 链接节点，并设置位置及颜色信息
 * @param node: (I) 被链接的节点指针
 * @param lpc: (I) 链指针，位置，颜色的合并值
 */
static __xwlib_inline
void xwlib_rbtree_link(struct xwlib_rbtree_node * node, xwptr_t lpc)
{
        node->lpc.v = lpc;
        *xwlib_rbtree_get_link(lpc) = node;
}

/**
 * @brief 链接空（叶子）节点
 * @param link: (I) 链接的位置
 */
static __xwlib_inline
void xwlib_rbtree_link_nil(struct xwlib_rbtree_node ** link)
{
        *link = NULL;
}

/**
 * @brief 返回节点的前趋节点
 * @param node: (I) 节点指针
 * @return 前趋节点指针
 */
static __xwlib_inline
struct xwlib_rbtree_node * xwlib_rbtree_get_predecessor(struct xwlib_rbtree_node * node)
{
        struct xwlib_rbtree_node * c = node->left;
        if (!c) {
                c = node;
        } else {
                while (c->right) {
                        c = c->right;
                }
        }
        return c;
}

/**
 * @brief 返回节点的后继节点
 * @param node: (I) 节点指针
 * @return 后继节点指针
 */
static __xwlib_inline
struct xwlib_rbtree_node * xwlib_rbtree_get_successor(struct xwlib_rbtree_node * node)
{
        struct xwlib_rbtree_node * c = node->right;
        if (!c) {
                c = node;
        } else {
                while (c->left) {
                        c = c->left;
                }
        }
        return c;
}

/**
 * @brief 用新节点代替旧节点并继承它的链指针、颜色和位置信息，
 *        但新节点并不接管旧节点的孩子节点
 * @param newd: (I) 新节点指针
 * @param oldn: (I) 旧节点指针
 */
static __xwlib_inline
void xwlib_rbtree_transplant(struct xwlib_rbtree_node * newn,
                             struct xwlib_rbtree_node * oldn)
{
        newn->lpc.v = oldn->lpc.v;
        *xwlib_rbtree_get_link(oldn->lpc.v) = newn;
}

/**
 * @brief 用叶子(NIL)替换一个旧节点。
 * @param oldn: (I) 旧节点指针
 */
static __xwlib_inline
void xwlib_rbtree_transplant_nil(struct xwlib_rbtree_node * oldn)
{
        *xwlib_rbtree_get_link(oldn->lpc.v) = NULL;
}

/**
 * @brief 测试一棵红黑树是否为空树。
 * @param t: (I) 红黑树指针
 * @retval false: 非空
 * @retval true: 空
 */
static __xwlib_inline
bool xwlib_rbtree_tst_empty(struct xwlib_rbtree * t)
{
        return (NULL == t->root);
}

/**
 * @brief 测试红黑树节点是否已经链接到红黑树中。
 * @param n: (I) 节点指针
 * @retval false: 已链接
 * @retval true: 未链接
 */
static __xwlib_inline
bool xwlib_rbtree_tst_node_unlinked(struct xwlib_rbtree_node * n)
{
        return (0 == n->lpc.v);
}

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         function prototypes         ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
__xwlib_code
void xwlib_rbtree_insert_color(struct xwlib_rbtree * tree,
                               struct xwlib_rbtree_node * node);

__xwlib_code
void xwlib_rbtree_remove(struct xwlib_rbtree * tree,
                         struct xwlib_rbtree_node * node);

__xwlib_code
void xwlib_rbtree_replace(struct xwlib_rbtree_node * newn,
                          struct xwlib_rbtree_node * oldn);

#endif /* xwos/lib/rbtree.h */
