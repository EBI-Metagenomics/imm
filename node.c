#include "node.h"
#include "state_table.h"

void imm_node_dump(struct imm_node const *x,
                   struct imm_state_table const *state_table,
                   imm_state_name *callb, FILE *restrict fp)
{
  char name[IMM_STATE_NAME_SIZE] = {0};
  if (!callb) callb = &imm_state_default_name;
  unsigned id = imm_state_table_id(state_table, x->state_source);
  fprintf(fp, "(%s;%u;%.9g)", (*callb)(id, name), x->emission_size, x->score);
}
