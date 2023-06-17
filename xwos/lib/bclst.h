/**
 * @file
 * @brief XWOS通用库：双循环链表
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef __xwos_lib_bclst_h__
#define __xwos_lib_bclst_h__

#include <xwos/standard.h>

/**
 * @defgroup xwos_lib_bclst 双循环链表
 * @ingroup xwos_lib
 * @{
 */

/**
 * @brief 双循环链表的节点
 */
struct xwlib_bclst_node {
        struct xwlib_bclst_node * next; /**< 下一个节点 */
        struct xwlib_bclst_node * prev; /**< 上一个节点 */
};

/**
 * @brief 双循环链表头
 */
#define xwlib_bclst_head        xwlib_bclst_node

/**
 * @brief 双循环链表头的初始化结构体
 */
#define XWLIB_BCLST_HEAD_INIT(n) (struct xwlib_bclst_head){&(n), &(n)}

/**
 * @brief 双循环链表节点的初始化结构体
 */
#define XWLIB_BCLST_NODE_INIT(n) (struct xwlib_bclst_node){&(n), &(n)}

/**
 * @brief 从一个链表节点的指针值计算出包含此节点成员的外层结构体的指针值。
 * @param[in] ptr: 节点指针
 * @param[in] type: 外层结构体类型
 * @param[in] member: 节点在外层结构体中的成员符号名(symbol)
 * @return 外层结构体的指针
 */
#define xwlib_bclst_entry(ptr, type, member) xwcc_baseof((ptr), type, member)

/**
 * @brief 获得包含链表第一个节点的外层结构体的指针。
 * @param[in] head: 链表头指针
 * @param[in] type: 外层结构体类型
 * @param[in] member: 节点在外层结构体中的成员符号名(symbol)
 * @return 外层结构体的指针
 */
#define xwlib_bclst_first_entry(head, type, member) \
        xwlib_bclst_entry((head)->next, type, member)

/**
 * @brief 获得包含链表最后一个节点的外层结构体的指针。
 * @param[in] head: 链表头指针
 * @param[in] type: 外层结构体类型
 * @param[in] member: 节点在外层结构体中的成员符号名(symbol)
 * @return 外层结构体的指针
 */
#define xwlib_bclst_last_entry(head, type, member) \
        xwlib_bclst_entry((head)->prev, type, member)

/**
 * @brief 从一个包含链表节点的外层结构体指针得到下一个外层结构体的指针
 * @param[in] p: 包含链表节点的外层结构体指针
 * @param[in] type: 外层结构体类型
 * @param[in] member: 节点在外层结构体中的成员符号名(symbol)
 * @return 下一个外层结构体的指针
 */
#define xwlib_bclst_next_entry(p, type, member) \
        xwlib_bclst_entry((p)->member.next, type, member)

/**
 * @brief 从一个包含链表节点的外层结构体指针得到上一个外层结构体的指针
 * @param[in] p: 包含链表节点的外层结构体指针
 * @param[in] type: 外层结构体类型
 * @param[in] member: 节点在外层结构体中的成员符号名(symbol)
 * @return 上一个外层结构体的指针
 */
#define xwlib_bclst_prev_entry(p, type, member) \
        xwlib_bclst_entry((p)->member.prev, type, member)

/**
 * @brief 向前遍历(iterate over)整个链表
 * @param[in] p: 光标指针，指向当前遍历到的节点
 * @param[in] head: 链表头指针
 */
#define xwlib_bclst_itr_next(p, head) \
        for ((p) = (head)->next; (p) != (head); (p) = (p)->next)

/**
 * @brief 向后遍历(iterate over)整个链表
 * @param[in] p: 光标指针，指向当前遍历到的节点
 * @param[in] head: 链表头指针
 */
#define xwlib_bclst_itr_prev(p, head) \
        for ((p) = (head)->prev; (p) != (head); (p) = (p)->prev)

/**
 * @brief 向前遍历(iterate over)整个链表，并防止因遍历到的节点被删除而造成的错误。
 * @param[in] p: 光标指针，指向当前遍历到的节点
 * @param[in] n: 暂存光标指针所指向的节点的next指针，
 *               防止此节点被删除后取到错误的next指针。
 * @param[in] head: 链表头指针
 */
#define xwlib_bclst_itr_next_safe(p, n, head) \
        for ((p) = (head)->next, (n) = (p)->next; \
             (p) != (head); \
             (p) = (n), (n) = (p)->next)

/**
 * @brief 向后遍历(iterate over)整个链表，并防止因遍历到的节点被删除而造成的错误。
 * @param[in] p: 光标指针，指向当前遍历到的节点
 * @param[in] n: 暂存光标指针所指向的节点的prev指针，
 *               防止此节点被删除后取到错误的prev指针。
 * @param[in] head: 链表头指针
 */
#define xwlib_bclst_itr_prev_safe(p, n, head) \
        for ((p) = (head)->prev, (n) = (p)->prev; \
             (p) != (head); \
             (p) = (n), (n) = (p)->prev)

/**
 * @brief 向前遍历(iterate over)整个链表，并将节点指针转化为包含它们的外层结构体指针。
 * @param[in] p: 光标指针，指向当前遍历到的节点所对应的外层结构体
 * @param[in] head: 链表头指针
 * @param[in] type: 外层结构体类型
 * @param[in] member: 节点在外层结构体中的成员符号名(symbol)
 */
#define xwlib_bclst_itr_next_entry(p, head, type, member) \
        for ((p) = xwlib_bclst_first_entry(head, type, member); \
             &(p)->member != (head); \
             (p) = xwlib_bclst_next_entry((p), type, member))

/**
 * @brief 向后遍历(iterate over)整个链表，并将节点指针转化为包含它们的外层结构体指针。
 * @param[in] p: 光标指针，指向当前遍历到的节点所对应的外层结构体
 * @param[in] head: 链表头指针
 * @param[in] type: 外层结构体类型
 * @param[in] member: 节点在外层结构体中的成员符号名(symbol)
 */
#define xwlib_bclst_itr_prev_entry(p, head, type, member) \
        for ((p) = xwlib_bclst_last_entry(head, type, member); \
             &(p)->member != (head); \
             (p) = xwlib_bclst_prev_entry((p), type, member))

/**
 * @brief 向前遍历(iterate over)整个链表，并防止因遍历到的节点被删除而造成的错误。
 *        同时将节点指针转化为包含它们的外层结构体指针。
 * @param[in] p: 光标指针，指向当前遍历到的节点所对应的外层结构体
 * @param[in] n: 暂存光标指针所指向的节点的next指针所对应的外层结构体指针，
 *               防止此节点被删除后取到错误的next指针
 * @param[in] head: 链表头指针
 * @param[in] type: 外层结构体类型
 * @param[in] member: 节点在外层结构体中的成员符号名(symbol)
 */
#define xwlib_bclst_itr_next_entry_safe(p, n, head, type, member) \
        for ((p) = xwlib_bclst_first_entry(head, type, member), \
             (n) = xwlib_bclst_next_entry((p), type, member); \
             &(p)->member != (head); \
             (p) = (n), (n) = xwlib_bclst_next_entry((n), type, member))

/**
 * @brief 以删除节点为目的，向前遍历(iterate over)整个链表，
 *        同时将节点指针转化为包含它们的外层结构体指针。
 * @param[in] p: 光标指针，指向当前遍历到的节点所对应的外层结构体
 * @param[in] head: 链表头指针
 * @param[in] type: 外层结构体类型
 * @param[in] member: 节点在外层结构体中的成员符号名(symbol)
 * @note
 * > 这个宏仅用于在退出操作中清理链表中剩余节点。在迭代操作的循环体中，
 * > 必须将节点所对应的结构体释放掉，并将节点从链表中删除。
 */
#define xwlib_bclst_itr_next_entry_del(p, head, type, member) \
        for ((p) = xwlib_bclst_first_entry(head, type, member); \
             &(p)->member != (head); \
             (p) = xwlib_bclst_first_entry(head, type, member))

/**
 * @brief 向后遍历(iterate over)整个链表，并防止因遍历到的节点被删除而造成的错误。
 *        同时将节点指针转化为包含它们的外层结构体指针。
 * @param[in] p: 光标指针，指向当前遍历到的节点所对应的外层结构体
 * @param[in] n: 暂存光标指针所指向的节点的prev指针所对应的外层结构体指针，
 *               防止此节点被删除后取到错误的prev指针
 * @param[in] head: 链表头指针
 * @param[in] type: 外层结构体类型
 * @param[in] member: 节点在外层结构体中的成员符号名(symbol)
 */
#define xwlib_bclst_itr_prev_entry_safe(p, n, head, type, member) \
        for ((p) = xwlib_bclst_last_entry(head, type, member), \
             (n) = xwlib_bclst_prev_entry((p), type, member); \
             &(p)->member != (head); \
             (p) = (n), (n) = xwlib_bclst_prev_entry((n), type, member))

/**
 * @brief 以删除节点为目的，向后遍历(iterate over)整个链表，
 *        同时将节点指针转化为包含它们的外层结构体指针。
 * @param[in] p: 光标指针，指向当前遍历到的节点所对应的外层结构体
 * @param[in] head: 链表头指针
 * @param[in] type: 外层结构体类型
 * @param[in] member: 节点在外层结构体中的成员符号名(symbol)
 * @note
 * > 这个宏通仅用于在退出操作中清理链表中剩余节点。在迭代操作的循环体中，
 * > 必须将节点所对应的结构体释放掉，并将节点从链表中删除。
 */
#define xwlib_bclst_itr_prev_entry_del(p, head, type, member) \
        for ((p) = xwlib_bclst_last_entry(head, type, member); \
             &(p)->member != (head); \
             (p) = xwlib_bclst_last_entry(head, type, member))

/**
 * @brief 初始化一个链表头。
 * @param[in] h: 链表头指针
 */
static __xwlib_inline
void xwlib_bclst_init_head(struct xwlib_bclst_head * h)
{
        h->next = h;
        h->prev = h;
}

/**
 * @brief 初始化一个链表节点。
 * @param[in] n: 链表节点指针
 */
static __xwlib_inline
void xwlib_bclst_init_node(struct xwlib_bclst_node * n)
{
        n->next = n;
        n->prev = n;
}

/**
 * @brief 测试链表是否为空。
 * @param[in] h: 链表头指针
 * @retval true: 空
 * @retval false: 非空
 */
static __xwlib_inline
bool xwlib_bclst_tst_empty(const struct xwlib_bclst_head * h)
{
        return (h->next == h);
}

/**
 * @brief 测试一个节点是否为指定链表的第一个节点。
 * @param[in] h: 链表头指针
 * @param[in] n: 被测试的节点
 * @retval true: 是该链表的第一个节点
 * @retval false: 不是该链表的第一个节点
 */
static __xwlib_inline
bool xwlib_bclst_tst_first(struct xwlib_bclst_node * h, struct xwlib_bclst_node * n)
{
        return (n == h->next);
}

/**
 * @brief 测试一个节点是否为指定链表的最后一个节点。
 * @param[in] h: 链表头指针
 * @param[in] n: 被测试的节点
 * @retval true: 是该链表的最后一个节点
 * @retval false: 不是该链表的最后一个节点
 */
static __xwlib_inline
bool xwlib_bclst_tst_last(struct xwlib_bclst_node * h, struct xwlib_bclst_node * n)
{
        return (n == h->prev);
}

/**
 * @brief 测试链表是否为空且是否没有正在被修改
 * @param[in] h: 链表头指针
 * @note
 * 测试链表是否为空且是否没有正在被另一些CPU修改它的任意成员符号(next or prev)。
 */
static __xwlib_inline
bool xwlib_bclst_tst_empty_carefully(const struct xwlib_bclst_head * h)
{
        struct xwlib_bclst_node * next = h->next;
        return ((next == (struct xwlib_bclst_node *)h) && (next == h->prev));
}

/**
 * @brief 将一个新节点加入到prev与next之间
 * @param[in] newn: 新节点指针
 * @param[in] prev: prev节点指针
 * @param[in] next: next节点指针
 */
static __xwlib_inline
void xwlib_bclst_add_between(struct xwlib_bclst_node * newn,
                             struct xwlib_bclst_node * prev,
                             struct xwlib_bclst_node * next)
{
        next->prev = newn;
        newn->next = next;
        newn->prev = prev;
        prev->next = newn;
}

/**
 * @brief 将一个节点加入到另一个节点的前面
 * @param[in] newn: 被加入的节点指针
 * @param[in] next: 另一个节点指针
 */
static __xwlib_inline
void xwlib_bclst_add_front(struct xwlib_bclst_node * newn,
                           struct xwlib_bclst_node * next)
{
        xwlib_bclst_add_between(newn, next->prev, next);

}

/**
 * @brief 将一个节点加入到另一个节点的后面
 * @param[in] newn: 被加入的节点指针
 * @param[in] prev: 另一个节点指针
 */
static __xwlib_inline
void xwlib_bclst_add_behind(struct xwlib_bclst_node * newn,
                            struct xwlib_bclst_node * prev)
{
        xwlib_bclst_add_between(newn, prev, prev->next);
}

/**
 * @brief 将一个节点加入链表头部（链表头的后面）
 * @param[in] head: 链表头指针
 * @param[in] newn: 被加入的节点指针
 */
static __xwlib_inline
void xwlib_bclst_add_head(struct xwlib_bclst_head * head,
                          struct xwlib_bclst_node * newn)
{
        xwlib_bclst_add_between(newn, head, head->next);
}

/**
 * @brief 将一个节点加入链表尾部（链表头的前面）
 * @param[in] head: 链表头指针
 * @param[in] newn: 被加入的节点指针
 */
static __xwlib_inline
void xwlib_bclst_add_tail(struct xwlib_bclst_head * head,
                          struct xwlib_bclst_node * newn)
{
        xwlib_bclst_add_between(newn, head->prev, head);
}

/**
 * @brief 删除prev节点与next节点之间的节点
 * @param[in] prev: prev节点指针
 * @param[in] next: next节点指针
 */
static __xwlib_inline
void xwlib_bclst_del_between(struct xwlib_bclst_node * prev,
                             struct xwlib_bclst_node * next)
{
        next->prev = prev;
        prev->next = next;
}

/**
 * @brief 删除一个节点
 * @param[in] node: 被删除的节点指针
 */
static __xwlib_inline
void xwlib_bclst_del(struct xwlib_bclst_node * node)
{
        xwlib_bclst_del_between(node->prev, node->next);
}

/**
 * @brief 删除一个节点，并重新初始化它
 * @param[in] node: 被删除的节点指针
 */
static __xwlib_inline
void xwlib_bclst_del_init(struct xwlib_bclst_node * node)
{
        xwlib_bclst_del_between(node->prev, node->next);
        xwlib_bclst_init_node(node);
}

/**
 * @brief 用一个新节点代替旧节点
 * @param[in] newn: 新节点指针
 * @param[in] oldn: 旧节点指针
 */
static __xwlib_inline
void xwlib_bclst_replace(struct xwlib_bclst_node * newn,
                         struct xwlib_bclst_node * oldn)
{
        newn->next = oldn->next;
        newn->next->prev = newn;
        newn->prev = oldn->prev;
        newn->prev->next = newn;
}

/**
 * @brief 用一个新节点代替旧节点，并重新初始化旧节点
 * @param[in] newn: 新节点指针
 * @param[in] oldn: 旧节点指针
 */
static __xwlib_inline
void xwlib_bclst_replace_init(struct xwlib_bclst_node * newn,
                              struct xwlib_bclst_node * oldn)
{
        xwlib_bclst_replace(newn, oldn);
        xwlib_bclst_init_node(oldn);
}

/**
 * @brief 将一条链表衔接到另一条链表的prev节点与next节点之间
 * @param[in] list: 待衔接的链表头指针
 * @param[in] prev: 另一条链表的prev节点指针
 * @param[in] next: 另一条链表的next节点指针
 */
static __xwlib_inline
void xwlib_bclst_splice_between(struct xwlib_bclst_head * list,
                                struct xwlib_bclst_node * prev,
                                struct xwlib_bclst_node * next)
{
        struct xwlib_bclst_node * first = list->next;
        struct xwlib_bclst_node * last = list->prev;

        first->prev = prev;
        prev->next = first;
        last->next = next;
        next->prev = last;
}

/**
 * @brief 将一条链表衔接到另一条链表头之后
 * @param[in] head: 另一条链表头指针
 * @param[in] list: 待衔接的链表头指针
 */
static __xwlib_inline
void xwlib_bclst_splice_head(struct xwlib_bclst_head * head,
                             struct xwlib_bclst_head * list)
{
        if (!xwlib_bclst_tst_empty(list)) {
                xwlib_bclst_splice_between(list, head, head->next);
        }/* else {} */
}

/**
 * @brief 将一条链表衔接到另一条链表头之前
 * @param[in] head: 另一条链表头指针
 * @param[in] list: 待衔接的链表头指针
 */
static __xwlib_inline
void xwlib_bclst_splice_tail(struct xwlib_bclst_head * head,
                             struct xwlib_bclst_head * list)
{
        if (!xwlib_bclst_tst_empty(list)) {
                xwlib_bclst_splice_between(list, head->prev, head);
        }/* else {} */
}

/**
 * @brief 将一条链表衔接到另一条链表头之后，并重新初始化剩下的空链表头
 * @param[in] head: 另一条链表头指针
 * @param[in] list: 待衔接的链表头指针
 */
static __xwlib_inline
void xwlib_bclst_splice_head_init(struct xwlib_bclst_head * head,
                                  struct xwlib_bclst_head * list)
{
        if (!xwlib_bclst_tst_empty(list)) {
                xwlib_bclst_splice_between(list, head, head->next);
                xwlib_bclst_init_head(list);
        }/* else {} */
}

/**
 * @brief 将一条链表衔接到另一条链表头之前，并重新初始化剩下的空链表头
 * @param[in] head: 另一条链表头指针
 * @param[in] list: 待衔接的链表头指针
 */
static __xwlib_inline
void xwlib_bclst_splice_tail_init(struct xwlib_bclst_head * head,
                                  struct xwlib_bclst_head * list)
{
        if (!xwlib_bclst_tst_empty(list)) {
                xwlib_bclst_splice_between(list, head->prev, head);
                xwlib_bclst_init_head(list);
        }
}

/**
 * @brief 将一段链表片段(无链表头)衔接到另一条链表的prev节点与next节点之间
 * @param[in] seg: 待衔接的链表碎片的第一个节点指针
 * @param[in] prev: 另一条链表的prev节点指针
 * @param[in] next: 另一条链表的next节点指针
 */
static __xwlib_inline
void xwlib_bclst_insseg_between(struct xwlib_bclst_node * seg,
                                struct xwlib_bclst_node * prev,
                                struct xwlib_bclst_node * next)
{
        struct xwlib_bclst_node * first = seg;
        struct xwlib_bclst_node * last = seg->prev;

        first->prev = prev;
        prev->next = first;
        last->next = next;
        next->prev = last;
}

/**
 * @brief 将一段链表片段(无链表头)衔接到另一条链表头之后
 * @param[in] head: 另一条链表头指针
 * @param[in] seg: 待衔接的链表碎片的第一个节点指针
 */
static __xwlib_inline
void xwlib_bclst_insseg_head(struct xwlib_bclst_head * head,
                             struct xwlib_bclst_node * seg)
{
        xwlib_bclst_insseg_between(seg, head, head->next);
}

/**
 * @brief 将一段链表片段(无链表头)衔接到另一条链表头之前
 * @param[in] head: 另一条链表头指针
 * @param[in] seg: 待衔接的链表碎片的第一个节点指针
 */
static __xwlib_inline
void xwlib_bclst_insseg_tail(struct xwlib_bclst_head * head,
                             struct xwlib_bclst_node * seg)
{
        xwlib_bclst_insseg_between(seg, head->prev, head);
}

/**
 * @} xwos_lib_bclst
 */

#endif /* xwos/lib/bclst.h */
