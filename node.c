#include "node.h"
#include "state_table.h"

static char *id_state_name(unsigned id, char *name)
{
  sprintf(name, "%u", id);
  return name;
}

void imm_node_dump(struct imm_node const *x,
                   struct imm_state_table const *state_table,
                   imm_state_name *callb, FILE *restrict fp)
{
  char name[IMM_STATE_NAME_SIZE] = {0};
  if (!callb) callb = &id_state_name;
  unsigned id = imm_state_table_id(state_table, x->state_source);
  fprintf(fp, "(%s;%u;%.9g)", (*callb)(id, name), x->emission_size, x->score);
}
