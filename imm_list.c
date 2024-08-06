#include "imm_list.h"

void imm_list_init(struct imm_list *list)
{
  list->next = list;
  list->prev = list;
}

static inline void list_add(struct imm_list *neu, struct imm_list *prev,
                            struct imm_list *next)
{
  next->prev = neu;
  neu->next = next;
  neu->prev = prev;
  prev->next = neu;
}

void imm_list_add(struct imm_list *neu, struct imm_list *head)
{
  list_add(neu, head, head->next);
}

static inline void list_del(struct imm_list *prev, struct imm_list *next)
{
  next->prev = prev;
  prev->next = next;
}

static inline void list_del_entry(struct imm_list *entry)
{
  list_del(entry->prev, entry->next);
}

void imm_list_del(struct imm_list *entry)
{
  list_del_entry(entry);
  imm_list_init(entry);
}

bool imm_list_is_head(const struct imm_list *list, const struct imm_list *head)
{
  return list == head;
}

bool imm_list_empty(const struct imm_list *head) { return head->next == head; }
