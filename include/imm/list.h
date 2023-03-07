#ifndef IMM_LIST_H
#define IMM_LIST_H

#include "imm/cof.h"
#include <stddef.h>

struct imm_list_head
{
    struct imm_list_head *next, *prev;
};

#define IMM_LIST_HEAD_INIT(name)                                               \
    {                                                                          \
        &(name), &(name)                                                       \
    }

#define IMM_LIST_HEAD(name) struct imm_list_head name = IMM_LIST_HEAD_INIT(name)

static inline void IMM_INIT_LIST_HEAD(struct imm_list_head *list)
{
    list->next = list;
    list->prev = list;
}

static inline void __imm_list_add(struct imm_list_head *neu,
                                  struct imm_list_head *prev,
                                  struct imm_list_head *next)
{
    next->prev = neu;
    neu->next = next;
    neu->prev = prev;
    prev->next = neu;
}

static inline void imm_list_add(struct imm_list_head *neu,
                                struct imm_list_head *head)
{
    __imm_list_add(neu, head, head->next);
}

static inline void imm_list_add_tail(struct imm_list_head *neu,
                                     struct imm_list_head *head)
{
    __imm_list_add(neu, head->prev, head);
}

static inline void __imm_list_del(struct imm_list_head *prev,
                                  struct imm_list_head *next)
{
    next->prev = prev;
    prev->next = next;
}

static inline void __imm_list_del_entry(struct imm_list_head *entry)
{
    __imm_list_del(entry->prev, entry->next);
}

static inline void imm_list_del(struct imm_list_head *entry)
{
    __imm_list_del_entry(entry);
    IMM_INIT_LIST_HEAD(entry);
}

static inline void imm_list_replace(struct imm_list_head *old,
                                    struct imm_list_head *neu)
{
    neu->next = old->next;
    neu->next->prev = neu;
    neu->prev = old->prev;
    neu->prev->next = neu;
}

static inline int imm_list_is_first(const struct imm_list_head *list,
                                    const struct imm_list_head *head)
{
    return list->prev == head;
}

static inline int imm_list_is_last(const struct imm_list_head *list,
                                   const struct imm_list_head *head)
{
    return list->next == head;
}

static inline int imm_list_is_head(const struct imm_list_head *list,
                                   const struct imm_list_head *head)
{
    return list == head;
}

static inline int imm_list_empty(const struct imm_list_head *head)
{
    return head->next == head;
}

#define imm_list_for_each(pos, head)                                           \
    for (pos = (head)->next; !imm_list_is_head(pos, (head)); pos = pos->next)

#define imm_list_for_each_safe(pos, n, head)                                   \
    for (pos = (head)->next, n = pos->next; !imm_list_is_head(pos, (head));    \
         pos = n, n = pos->next)

#define imm_list_entry(ptr, type, member) imm_cof(ptr, type, member)

#define imm_list_first_entry(ptr, type, member)                                \
    imm_list_entry((ptr)->next, type, member)

#define imm_list_last_entry(ptr, type, member)                                 \
    imm_list_entry((ptr)->prev, type, member)

#define imm_list_entry_is_head(pos, head, member) (&pos->member == (head))

#define imm_list_next_entry(pos, member)                                       \
    imm_list_entry((pos)->member.next, __typeof__(*(pos)), member)

#define imm_list_for_each_entry(pos, head, member)                             \
    for (pos = imm_list_first_entry(head, __typeof__(*pos), member);           \
         !imm_list_entry_is_head(pos, head, member);                           \
         pos = imm_list_next_entry(pos, member))

#endif
