#ifndef IMM_STACK_H
#define IMM_STACK_H

#include "imm/list.h"

static inline void imm_stack_put(struct imm_list_head *neu,
                                 struct imm_list_head *stack)
{
    imm_list_add(neu, stack);
}

static inline void imm_stack_pop(struct imm_list_head *stack)
{
    imm_list_del(stack->next);
}

#endif
