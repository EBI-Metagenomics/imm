#include "stack.h"

void imm_stack_put(struct imm_list *neu, struct imm_list *stack)
{
  imm_list_add(neu, stack);
}

void imm_stack_pop(struct imm_list *stack) { imm_list_del(stack->next); }
